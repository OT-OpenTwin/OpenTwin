import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from opentwin import launch_editor, cli


def main(argv: Sequence[str]) -> int:
    if not 1 <= len(argv) <= 2:
        raise SystemExit("usage: edit.py <PROJECT> [EDITOR]")

    env, target = cli.prepare(argv[0])
    return launch_editor(env, target, cli.argument(argv, 1))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
