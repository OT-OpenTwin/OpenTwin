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
import subprocess
import sys
import tempfile
import unittest
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from ot_dev.deployment import BUILD_INFO, create_build_information

DEV = Path(os.environ["OPENTWIN_DEV_ROOT"])
THIRD = Path(os.environ["OPENTWIN_THIRDPARTY_ROOT"])
BATCH = DEV / "Scripts" / "BuildAndTest" / "CreateBuildInformation.bat"
PREFIX = "Build date/time: "


def _revision(root: Path) -> str:
    result = subprocess.run(["git", "rev-parse", "--short", "HEAD"], cwd=root,
                            capture_output=True, text=True, errors="replace")
    return result.stdout.strip()


def _run_batch(target: Path) -> bytes:
    """Run the original batch with only its output path redirected."""
    lines = BATCH.read_text().splitlines(keepends=True)
    replaced = 0
    for index, line in enumerate(lines):
        if line.startswith("set buildInfoFile="):
            lines[index] = 'set buildInfoFile="' + str(target) + '"\n'
            replaced += 1
    assert replaced == 1, f"expected one output path in the batch, found {replaced}"
    probe = target.parent / "probe.bat"
    probe.write_text("".join(lines))
    subprocess.run(["cmd", "/c", str(probe)], capture_output=True)
    return target.read_bytes()


class BuildInformation(unittest.TestCase):
    def setUp(self) -> None:
        self._temp = tempfile.TemporaryDirectory()
        folder = Path(self._temp.name)
        self.batch = _run_batch(folder / "batch.txt")
        create_build_information(folder / "python.txt")
        self.python = (folder / "python.txt").read_bytes()

    def tearDown(self) -> None:
        self._temp.cleanup()

    def _lines(self, data: bytes) -> list[str]:
        text = data.decode()
        self.assertTrue(text.endswith("\r\n"))
        return text[:-2].split("\r\n")

    def test_byte_length_matches_batch(self) -> None:
        self.assertEqual(len(self.batch), len(self.python))

    def test_revision_lines_match_batch_exactly(self) -> None:
        batch, python = self._lines(self.batch), self._lines(self.python)
        self.assertEqual(len(batch), 3)
        self.assertEqual(batch[1:], python[1:])

    def test_revision_lines_match_git(self) -> None:
        lines = self._lines(self.python)
        self.assertEqual(lines[1], f"OpenTwin: {_revision(DEV)} ")
        self.assertEqual(lines[2], f"ThirdParty: {_revision(THIRD)} ")

    def test_timestamp_format_matches_batch(self) -> None:
        for data in (self.batch, self.python):
            line = self._lines(data)[0]
            self.assertTrue(line.startswith(PREFIX))
            self.assertTrue(line.endswith(" "))
            datetime.strptime(line[len(PREFIX):-1], "%Y-%m-%d %H:%M:%S")

    def test_timestamps_are_close(self) -> None:
        stamps = [datetime.strptime(self._lines(d)[0][len(PREFIX):-1], "%Y-%m-%d %H:%M:%S")
                  for d in (self.batch, self.python)]
        self.assertLess(abs((stamps[0] - stamps[1]).total_seconds()), 120)

    def test_every_line_ends_with_space_and_crlf(self) -> None:
        self.assertNotIn(b"\n", self.python.replace(b"\r\n", b""))
        for line in self._lines(self.python):
            self.assertTrue(line.endswith(" "), line)

    def test_default_target_is_the_deployment_file(self) -> None:
        self.assertEqual(BUILD_INFO, ("Deployment", "BuildInfo.txt"))


if __name__ == "__main__":
    unittest.main(verbosity=2)
