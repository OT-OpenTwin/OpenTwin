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
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from ot_dev.documentation import (CODEDOC, DEVELOPER, DOXYFILE, MATHJAX_DEPTH, MODES,
                                  _codedoc_root, build_documentation,
                                  mathjax_relative_path)

DEV = Path(os.environ["OPENTWIN_DEV_ROOT"])
THIRD = Path(os.environ["OPENTWIN_THIRDPARTY_ROOT"])

BASE = {
    "OPENTWIN_DEV_ROOT": str(DEV),
    "OPENTWIN_THIRDPARTY_ROOT": str(THIRD),
    "MATHJAX_REL_PATH_SUFFIX": r"MathJax\MathJax-3.2.2\es5",
}


class MathJaxPath(unittest.TestCase):
    def test_matches_what_the_batch_builds(self):
        """BuildDocumentation.bat:68 - six levels up, then the ThirdParty folder name."""
        expected = "\\".join([".."] * MATHJAX_DEPTH + [THIRD.name, r"MathJax\MathJax-3.2.2\es5"])
        self.assertEqual(mathjax_relative_path(BASE), expected)

    def test_an_explicit_value_wins(self):
        env = dict(BASE, MATHJAX_REL_PATH="already/set")
        self.assertEqual(mathjax_relative_path(env), "already/set")

    def test_unshared_parent_is_refused(self):
        env = dict(BASE, OPENTWIN_THIRDPARTY_ROOT=r"D:\Elsewhere\ThirdParty")
        self.assertIsNone(mathjax_relative_path(env))

    def test_depth_is_six(self):
        self.assertEqual(mathjax_relative_path(BASE).count(".."), MATHJAX_DEPTH)


class CodeDocRoot(unittest.TestCase):
    def test_defaults_under_the_developer_docs(self):
        self.assertEqual(_codedoc_root(BASE), DEV.joinpath(*CODEDOC))

    def test_environment_override_wins(self):
        env = dict(BASE, OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT=r"C:\somewhere\else")
        self.assertEqual(_codedoc_root(env), Path(r"C:\somewhere\else"))


class Modes(unittest.TestCase):
    def test_unknown_mode_is_refused(self):
        with self.assertRaises(SystemExit):
            build_documentation(dict(BASE), mode="NONSENSE")

    def test_known_modes(self):
        self.assertEqual(MODES, ("BOTH", "SPHINX", "DOXYGEN"))


class Layout(unittest.TestCase):
    """The paths the batch scripts hard-code must still exist."""

    def test_developer_docs_exist(self):
        self.assertTrue(DEV.joinpath(*DEVELOPER).is_dir())

    def test_doxyfile_exists(self):
        self.assertTrue(DEV.joinpath(*DOXYFILE).is_file())


if __name__ == "__main__":
    unittest.main(verbosity=2)
