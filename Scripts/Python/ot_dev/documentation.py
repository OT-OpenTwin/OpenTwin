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
import shutil
import subprocess
from datetime import datetime
from pathlib import Path
from typing import Mapping, TextIO

from .actions import SEPARATOR
from .environment import build_env
from .platform import USE_SHELL

DEVELOPER = ("Documentation", "Developer")
DOXYFILE = ("Documentation", "Doxygen", "Doxyfile")
CODEDOC = ("Documentation", "Developer", "_build", "html", "_static")
CODEDOC_FOLDER = "codedochtml"
CODEDOC_INDEX = ("index.xhtml", "code_doc_index.xhtml")

LOG_DIR = ("Scripts", "BuildAndTest")
SPHINX_LOG = "Documentation_buildLog.txt"
DOXYGEN_LOG = "DoxygenDocumentation_buildLog.txt"

SPHINX = "sphinx-build"
DOXYGEN = "doxygen"
BUILD_DIR = "_build"

# BuildDocumentation.bat builds the MathJax path six levels up from the code docs.
MATHJAX_DEPTH = 6

MODES = ("BOTH", "SPHINX", "DOXYGEN")


def _stamp(label: str) -> str:
    return f"{label}{datetime.now():%Y-%m-%d %H:%M:%S}"


def _run(command, cwd: Path, env: Mapping[str, str], out: TextIO) -> int:
    out.write(f"$ {' '.join(str(c) for c in command)}\n")
    out.flush()
    try:
        return subprocess.run(command, cwd=cwd, env=env, stdout=out,
                              stderr=subprocess.STDOUT, shell=USE_SHELL).returncode
    except FileNotFoundError:
        out.write(f"{command[0]} was not found\n")
        return 1


def mathjax_relative_path(env: Mapping[str, str]) -> str | None:
    """Relative path from the code docs to MathJax in the ThirdParty tree."""
    existing = env.get("MATHJAX_REL_PATH")
    if existing:
        return existing

    dev = Path(env["OPENTWIN_DEV_ROOT"]).resolve()
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"]).resolve()
    if dev.parent != third.parent:
        return None

    suffix = env.get("MATHJAX_REL_PATH_SUFFIX", "")
    return str(Path(*([".."] * MATHJAX_DEPTH)) / third.name / suffix)


def _codedoc_root(env: Mapping[str, str]) -> Path:
    configured = env.get("OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT")
    if configured:
        return Path(configured)
    return Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*CODEDOC)


def build_sphinx(env: Mapping[str, str], logs: Path) -> int:
    root = Path(env.get("OT_DOCUMENTATION_ROOT") or
                Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*DEVELOPER))
    sphinx = env.get("SPHINXBUILD") or SPHINX

    print("Build Sphinx Documentation", flush=True)
    with open(logs / SPHINX_LOG, "w", encoding="utf-8") as out:
        out.write(_stamp("Started at: ") + "\n")
        code = _run([sphinx, "-M", "html", ".", BUILD_DIR], root, env, out)
        out.write(_stamp("Finished at: ") + "\n")

    if code:
        print(f"  {sphinx} failed, see {logs / SPHINX_LOG}", flush=True)
    return code


def build_doxygen(env: Mapping[str, str], logs: Path) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    output = _codedoc_root(env)
    mathjax = mathjax_relative_path(env)
    if mathjax is None:
        print("  the dev root and third party root do not share a parent directory, "
              "set MATHJAX_REL_PATH to continue", flush=True)
        return 1

    step = dict(env)
    step["OPENTWIN_CODEDOC_BUILD_OUTPUT_ROOT"] = str(output)
    step["MATHJAX_REL_PATH"] = mathjax

    # sphinx regenerates _static, so the previous code docs are dropped first
    shutil.rmtree(output / CODEDOC_FOLDER, ignore_errors=True)

    print("Build Doxygen Documentation", flush=True)
    with open(logs / DOXYGEN_LOG, "w", encoding="utf-8") as out:
        out.write(f"Output Path      = {output}\n")
        out.write(f"MATHJAX_REL_PATH = {mathjax}\n")
        out.write(_stamp("Started at: ") + "\n")
        code = _run([DOXYGEN, str(dev.joinpath(*DOXYFILE))], dev, step, out)
        out.write(_stamp("Finished at: ") + "\n")

    source, target = CODEDOC_INDEX
    index = output / CODEDOC_FOLDER / source
    if index.is_file():
        os.replace(index, index.with_name(target))
        print(f"  renamed {source} to {target}", flush=True)
    elif code == 0:
        print(f"  {source} was not produced", flush=True)

    if code:
        print(f"  {DOXYGEN} failed, see {logs / DOXYGEN_LOG}", flush=True)
    return code


def build_documentation(env: Mapping[str, str] | None = None, mode: str = "BOTH") -> int:
    mode = mode.upper()
    if mode not in MODES:
        raise SystemExit(f"Unknown mode '{mode}'. Known: " + ", ".join(MODES))

    env = dict(env if env is not None else build_env())
    logs = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*LOG_DIR)
    logs.mkdir(parents=True, exist_ok=True)
    failed = 0

    if mode in ("BOTH", "SPHINX"):
        failed = build_sphinx(env, logs) or failed
    if mode in ("BOTH", "DOXYGEN"):
        failed = build_doxygen(env, logs) or failed

    print(SEPARATOR, flush=True)
    print("SUCCESS" if failed == 0 else "FAILED", flush=True)
    return failed
