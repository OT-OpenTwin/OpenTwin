import subprocess
from pathlib import Path
from typing import Mapping, Sequence

ROOT = ("Tools", "AdminPanel")
LOG_NAME = "AdminPanel_buildLog.txt"
COMMANDS = [["yarn", "install"], ["yarn", "build"]]


def build_admin_panel(env: Mapping[str, str], configs: Sequence[str],
                      rebuild: bool, logs: str | Path) -> int:
    root = Path(env["OPENTWIN_DEV_ROOT"]).joinpath(*ROOT)
    failed = 0

    print(f"Building Project {root}", flush=True)
    with open(Path(logs) / LOG_NAME, "w", encoding="utf-8") as out:
        for command in COMMANDS:
            out.write(f"$ {' '.join(command)}\n")
            out.flush()
            code = subprocess.run(command, cwd=root, env=env, stdout=out,
                                  stderr=subprocess.STDOUT, shell=True).returncode
            failed = failed or code
        out.write(f"--- Build {'successful' if failed == 0 else 'failed'}: {root} ---\n")

    print("---", flush=True)
    print("SUCCESS" if failed == 0 else "FAILED", flush=True)
    return failed
