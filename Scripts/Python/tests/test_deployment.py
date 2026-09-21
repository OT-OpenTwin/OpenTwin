# License:
# Copyright 2026 by OpenTwin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import re
import stat
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from ot_dev.config import manifest
from ot_dev.deployment import (ACTIONS, Plan, Result, _deployment_env, _do_copy,
                               _do_remove, _do_tree, _execute)
from ot_dev.expansion import expand

BATCH = Path(os.environ["OPENTWIN_DEV_ROOT"]) / "Scripts" / "BuildAndTest"
TOKEN = re.compile(r'"([^"]*)"|(\S+)')
SKIP = ("REM", "@ECHO", ":", "IF", "GOTO", "SET", "PAUSE", ")")
VERBS = {"COPY": "copy", "XCOPY": "tree", "MKDIR": "mkdir",
         "RMDIR": "rmtree", "DEL": "remove", "FOR": "remove_glob",
         "REN": "rename", "MOVE": "move"}

# Deliberate deviations from the batch. OTRandom was deleted from the repository,
# so the batch deletes and copies a DLL that can never exist; the python rail drops it.
OMITTED = ("OTRandom.dll",)


def _args(rest):
    return [a or b for a, b in TOKEN.findall(rest)]


def _parse(name, env, splice=None):
    """Independently derive the operations the batch performs."""
    steps = []
    for raw in (BATCH / name).read_text().splitlines():
        line = raw.strip()
        if not line or any(line.upper().startswith(s) for s in SKIP):
            continue
        upper = line.upper()
        if upper.startswith("ECHO ") and ">" in line:
            continue
        if upper.startswith("CALL "):
            target = Path(_args(line[5:])[0]).name
            if splice and target.lower() == splice[0].lower():
                steps.extend(splice[1])
            continue
        verb = next((v for v in ("XCOPY", "COPY", "MKDIR", "RMDIR", "DEL", "FOR", "REN", "MOVE")
                     if upper.startswith(v + " ")), None)
        if verb is None:
            continue
        rest = line[len(verb):].strip()
        paths = [a for a in _args(rest) if not a.startswith("/") and not a.startswith("2>")]
        kind = VERBS[verb]
        if any(name in p for p in paths for name in OMITTED):
            continue
        if kind == "remove" and any("%%" in p for p in paths):
            continue
        if kind == "remove_glob":
            steps.append(("remove_glob", expand(env, "%OT_DEPLOYMENT_DIR%\\plugins")))
        elif kind == "rename":
            steps.append(("rename", expand(env, paths[0]), paths[1]))
        elif kind == "move":
            # the batch runs makensis from the script directory, so its relative
            # source names a file in BATCH; the plan states that path outright
            source = expand(env, paths[0])
            if not Path(source).is_absolute():
                source = str(BATCH / source)
            steps.append((kind, source, expand(env, paths[1])))
        elif kind in ("copy", "tree"):
            steps.append((kind, expand(env, paths[0]), expand(env, paths[1])))
        else:
            steps.append((kind, expand(env, paths[0])))
    return steps


def _plan_steps(builder, env):
    plan = Plan(dict(env))
    builder(plan)
    out = []
    for step in plan.steps:
        if step[0] in ("action", "run"):
            continue
        if step[0] == "tree":
            out.append(("tree", step[1], step[2]))
        elif step[0] in ("copy", "rename", "move"):
            out.append((step[0], step[1], step[2]))
        elif step[0] == "write":
            continue
        elif step[0] == "remove_glob":
            out.append(("remove_glob", step[1]))
        else:
            out.append((step[0], step[1]))
    return out


class PlanMatchesBatch(unittest.TestCase):
    """The batch scripts are the oracle: the plan must perform the same work."""

    @classmethod
    def setUpClass(cls):
        cls.env = _deployment_env(None)

    def test_update_libraries_matches_batch(self):
        expected = _parse("UpdateDeploymentLibrariesOnly.bat", self.env)
        self.assertEqual(_plan_steps(manifest.update_libraries, self.env), expected)

    def test_create_deployment_matches_batch(self):
        inner = _parse("UpdateDeploymentLibrariesOnly.bat", self.env)
        expected = _parse("CreateDeployment.bat", self.env,
                          splice=("UpdateDeploymentLibrariesOnly.bat", inner))
        self.assertEqual(_plan_steps(manifest.create_deployment, self.env), expected)

    def test_frontend_installer_matches_batch(self):
        expected = _parse("CreateFrontendInstaller.bat", self.env)
        self.assertEqual(_plan_steps(manifest.create_frontend_installer, self.env), expected)

    def test_frontend_installer_renames_the_executable(self):
        plan = Plan(dict(self.env))
        manifest.create_frontend_installer(plan)
        renames = [s for s in plan.steps if s[0] == "rename"]
        self.assertEqual(len(renames), 1)
        self.assertTrue(renames[0][1].endswith("uiFrontend.exe"))
        self.assertEqual(renames[0][2], "OpenTwin.exe")

    def test_frontend_installer_runs_makensis(self):
        """The step that actually builds the installer must be present."""
        plan = Plan(dict(self.env))
        manifest.create_frontend_installer(plan)
        runs = [s for s in plan.steps if s[0] == "run"]
        self.assertEqual(len(runs), 1)
        command = runs[0][2]
        self.assertIn("makensis", command[0].lower())
        self.assertTrue(command[1].endswith(".nsi"), command[1])
        self.assertTrue(Path(runs[0][3]).is_absolute(), runs[0][3])

    def test_frontend_installer_moves_an_absolute_path(self):
        """makensis writes into its working directory, so the move must not be relative."""
        plan = Plan(dict(self.env))
        manifest.create_frontend_installer(plan)
        moves = [s for s in plan.steps if s[0] == "move"]
        self.assertEqual(len(moves), 1)
        self.assertTrue(Path(moves[0][1]).is_absolute(), moves[0][1])

    def test_frontend_paths_are_expanded(self):
        for step in _plan_steps(manifest.create_frontend_installer, self.env):
            for value in step[1:]:
                self.assertNotIn("%", value, f"unresolved variable in {step}")

    def test_debug_files_matches_batch(self):
        expected = _parse("CreateDebugFiles.bat", self.env)
        self.assertEqual(_plan_steps(manifest.create_debug_files, self.env), expected)

    def test_debug_files_actions(self):
        plan = Plan(dict(self.env))
        manifest.create_debug_files(plan)
        names = [s[1] for s in plan.steps if s[0] == "action"]
        self.assertEqual(names, ["shutdown", "build all"])
        for name in names:
            self.assertIn(name, ACTIONS)

    def test_omitted_entries_are_absent_from_the_plan(self):
        """The deviations in OMITTED must genuinely not be in any plan."""
        for builder in (manifest.update_libraries, manifest.create_deployment,
                        manifest.create_frontend_installer, manifest.create_debug_files):
            for step in _plan_steps(builder, self.env):
                for value in step[1:]:
                    for name in OMITTED:
                        self.assertNotIn(name, value)

    def test_the_batch_still_contains_what_we_omit(self):
        """If the batch drops OTRandom too, this exception should be deleted."""
        text = (BATCH / "UpdateDeploymentLibrariesOnly.bat").read_text()
        for name in OMITTED:
            self.assertIn(name, text, f"{name} is gone from the batch; remove it from OMITTED")

    def test_qt_conf_content_is_exact(self):
        """qt.conf is read by Qt at runtime; the escaping must survive edits."""
        plan = Plan(dict(self.env))
        manifest.create_deployment(plan)
        writes = [s for s in plan.steps if s[0] == "write"]
        self.assertEqual(len(writes), 1)
        built = "".join(line + "\r\n" for line in writes[0][2]).encode()
        self.assertEqual(built, b"[Paths] \r\nPlugins = .\\\\plugins \r\n")

    def test_every_named_action_is_implemented(self):
        for builder in (manifest.update_libraries, manifest.create_deployment):
            plan = Plan(dict(self.env))
            builder(plan)
            for step in plan.steps:
                if step[0] == "action":
                    self.assertIn(step[1], ACTIONS)

    def test_no_path_is_left_unexpanded(self):
        for builder in (manifest.update_libraries, manifest.create_deployment):
            for step in _plan_steps(builder, self.env):
                for value in step[1:]:
                    self.assertNotIn("%", value, f"unresolved variable in {step}")
                    self.assertTrue(value.strip(), f"empty path in {step}")

    def _actions(self, builder):
        plan = Plan(dict(self.env))
        builder(plan)
        return plan.steps, [s[1] for s in plan.steps if s[0] == "action"]

    def test_update_libraries_actions(self):
        steps, names = self._actions(manifest.update_libraries)
        self.assertEqual(names, ["shutdown", "build information"])
        self.assertEqual(steps[0][0], "action")
        self.assertEqual(steps[-1][0], "action")

    def test_create_deployment_actions(self):
        steps, names = self._actions(manifest.create_deployment)
        self.assertEqual(names, ["shutdown", "shutdown", "build information"])

    def test_every_shutdown_precedes_the_deletes_it_guards(self):
        for builder in (manifest.update_libraries, manifest.create_deployment):
            steps, _ = self._actions(builder)
            kinds = [s[0] for s in steps]
            first_shutdown = kinds.index("action")
            first_delete = min((i for i, k in enumerate(kinds) if k in ("rmtree", "remove")),
                               default=len(kinds))
            self.assertLess(first_shutdown, first_delete)


class Executor(unittest.TestCase):
    """The copy semantics are new code, so they are exercised directly."""

    def setUp(self):
        self._temp = tempfile.TemporaryDirectory()
        self.root = Path(self._temp.name)
        self.src = self.root / "src"
        self.dst = self.root / "dst"
        (self.src / "nested" / "deep").mkdir(parents=True)
        (self.src / "empty").mkdir()
        (self.src / "a.dll").write_text("a")
        (self.src / "b.txt").write_text("b")
        (self.src / "noext").write_text("n")
        (self.src / "nested" / "c.dll").write_text("c")
        self.dst.mkdir()

    def tearDown(self):
        self._temp.cleanup()

    def test_copy_single_file_into_directory(self):
        failed = Result()
        _do_copy(str(self.src / "a.dll"), str(self.dst), failed)
        self.assertEqual(len(failed), 0)
        self.assertEqual((self.dst / "a.dll").read_text(), "a")

    def test_copy_wildcard_is_not_recursive(self):
        failed = Result()
        _do_copy(str(self.src / "*.dll"), str(self.dst), failed)
        self.assertTrue((self.dst / "a.dll").exists())
        self.assertFalse((self.dst / "c.dll").exists())

    def test_copy_star_dot_star_takes_extensionless_files(self):
        failed = Result()
        _do_copy(str(self.src / "*.*"), str(self.dst), failed)
        self.assertTrue((self.dst / "noext").exists())

    def test_missing_source_is_reported_not_raised(self):
        failed = Result()
        _do_copy(str(self.src / "absent.dll"), str(self.dst), failed)
        self.assertEqual(len(failed), 1)

    def test_tree_is_recursive_and_skips_empty_dirs(self):
        failed = Result()
        _do_tree(str(self.src), str(self.dst), False, failed)
        self.assertTrue((self.dst / "nested" / "c.dll").exists())
        self.assertFalse((self.dst / "empty").exists())

    def test_tree_with_empty_keeps_empty_dirs(self):
        failed = Result()
        _do_tree(str(self.src), str(self.dst), True, failed)
        self.assertTrue((self.dst / "empty").is_dir())

    def test_tree_of_a_single_file_copies_it(self):
        """XCOPY /S on one file copies that file (CreateDeployment.bat:251)."""
        failed = Result()
        _do_tree(str(self.src / "a.dll"), str(self.dst), False, failed)
        self.assertEqual(len(failed), 0)
        self.assertEqual((self.dst / "a.dll").read_text(), "a")

    def test_tree_pattern_filters_files(self):
        failed = Result()
        _do_tree(str(self.src / "*.dll"), str(self.dst), False, failed)
        self.assertTrue((self.dst / "nested" / "c.dll").exists())
        self.assertFalse((self.dst / "b.txt").exists())

    def test_remove_wildcard_deletes_every_match(self):
        (self.dst / "one.otcsf").write_text("1")
        (self.dst / "two.otcsf").write_text("2")
        (self.dst / "keep.txt").write_text("k")
        _do_remove(str(self.dst / "*.otcsf"))
        self.assertEqual(sorted(p.name for p in self.dst.iterdir()), ["keep.txt"])

    def test_remove_missing_file_is_silent(self):
        _do_remove(str(self.dst / "absent.dll"))

    def test_one_unwritable_file_does_not_abort_the_tree(self):
        """A single locked file must cost one file, not the rest of the copy."""
        for name in ("z1.dll", "z2.dll", "z3.dll"):
            (self.src / name).write_text(name)
        blocked = self.dst / "z2.dll"
        blocked.write_text("old")
        os.chmod(blocked, stat.S_IREAD)
        try:
            failed = Result()
            _do_tree(str(self.src), str(self.dst), False, failed)
            self.assertEqual(len(failed), 1, failed.blocked + failed.missing)
            self.assertTrue((self.dst / "z1.dll").exists())
            self.assertTrue((self.dst / "z3.dll").exists())
            self.assertTrue((self.dst / "nested" / "c.dll").exists())
        finally:
            os.chmod(blocked, stat.S_IWRITE)

    def test_one_unwritable_file_does_not_abort_a_glob_copy(self):
        blocked = self.dst / "a.dll"
        blocked.write_text("old")
        os.chmod(blocked, stat.S_IREAD)
        try:
            failed = Result()
            _do_copy(str(self.src / "*.*"), str(self.dst), failed)
            self.assertEqual(len(failed), 1, failed.blocked + failed.missing)
            self.assertTrue((self.dst / "b.txt").exists())
            self.assertTrue((self.dst / "noext").exists())
        finally:
            os.chmod(blocked, stat.S_IWRITE)

    def test_execute_runs_steps_in_order(self):
        plan = Plan({"D": str(self.dst)})
        plan.mkdir("%D%\\made")
        plan.write("%D%\\made\\out.txt", ("one ", "two "))
        _execute(plan)
        self.assertEqual((self.dst / "made" / "out.txt").read_bytes(), b"one \r\ntwo \r\n")


if __name__ == "__main__":
    unittest.main(verbosity=2)
