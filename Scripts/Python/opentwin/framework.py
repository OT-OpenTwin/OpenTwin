import shutil
import subprocess
from pathlib import Path
from typing import Any, Mapping, Sequence, TextIO

ROOT = ("Framework", "OpenTwin")
QT_CONF = ("Assets", "qt.conf")
LOG_NAME = "Framework_buildLog.txt"
FINGERPRINTS = ["output-lib-open_twin", "output-bin-open_twin"]

CARGO: dict[str, dict[str, Any]] = {
    "debug": {
        "clean": ["cargo", "clean", "-p", "open_twin"],
        "build": ["cargo", "build"],
        "target": "debug",
    },
    "release": {
        "clean": ["cargo", "clean", "--release", "-p", "open_twin"],
        "build": ["cargo", "build", "--release"],
        "target": "release",
    },
}


def _run(command: Sequence[str], cwd: Path, env: Mapping[str, str], out: TextIO) -> int:
    return subprocess.run(command, cwd=cwd, env=env, stdout=out,
                          stderr=subprocess.STDOUT, shell=True).returncode


def _write_fingerprints(root: Path, target: str, out: TextIO) -> None:
    directory = root / "target" / target / ".fingerprint"
    if not directory.is_dir():
        return
    for name in FINGERPRINTS:
        for path in sorted(directory.rglob(name)):
            out.write(path.read_text(encoding="utf-8", errors="replace") + "\n")


def _install_qt_conf(dev: Path, root: Path, target: str) -> None:
    source = dev.joinpath(*QT_CONF)
    destination = root / "target" / target
    if source.is_file() and destination.is_dir():
        shutil.copy(source, destination / "qt.conf")


def build_framework(env: Mapping[str, str], configs: Sequence[str],
                    rebuild: bool, logs: str | Path) -> int:
    dev = Path(env["OPENTWIN_DEV_ROOT"])
    root = dev.joinpath(*ROOT)
    failed = 0

    print(f"Building Project {root}", flush=True)
    with open(Path(logs) / LOG_NAME, "w", encoding="utf-8") as out:
        for config in configs:
            spec = CARGO[config]
            print(config.upper(), flush=True)
            out.write(f"Microservices OpenTwin Build {config.upper()}\n")
            out.flush()
            if rebuild:
                _run(spec["clean"], root, env, out)
            code = _run(spec["build"], root, env, out)
            _write_fingerprints(root, spec["target"], out)
            _install_qt_conf(dev, root, spec["target"])
            out.write(f"--- Build {'successful' if code == 0 else 'failed'}: {root} ---\n")
            failed = failed or code

    print("---", flush=True)
    print("SUCCESS" if failed == 0 else "FAILED", flush=True)
    return failed
