import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from opentwin import clean_all, cli, project_roots


def main(argv: Sequence[str]) -> int:
    if argv:
        raise SystemExit("usage: clean_all.py")

    env = cli.environment()
    return clean_all(env, sorted(project_roots()))


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
