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

SEPARATOR = "=" * 90

# CheckForFailedBuilds and the build server look for this text in the summary.
FAILED_BUILD = "Build failed"


def build_result(target: str, code: int) -> str:
    return f"--- Build {'successful' if code == 0 else 'failed'}: {target} ---\n"


def finish(failed: bool) -> None:
    print("---", flush=True)
    print("FAILED" if failed else "SUCCESS", flush=True)
