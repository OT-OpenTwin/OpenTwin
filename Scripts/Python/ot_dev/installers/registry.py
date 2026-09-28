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

"""Read-only registry lookups."""

from ..core.platform import WINDOWS

# TODO(linux): NSIS and 7-Zip are found through the Windows registry.
if WINDOWS:
    import winreg


def read_value(root: str, key: str, name: str) -> str:
    """The value as text, or "" when the key or value is missing. Opens the key with KEY_READ only."""
    try:
        with winreg.OpenKey(getattr(winreg, root), key, 0, winreg.KEY_READ) as handle:
            return str(winreg.QueryValueEx(handle, name)[0])
    except OSError:
        return ""
