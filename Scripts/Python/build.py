import sys
from pathlib import Path
from typing import Sequence

sys.path.insert(0, str(Path(__file__).resolve().parent))

from opentwin import build_project, cli


def main(argv: Sequence[str]) -> int:
    if not 1 <= len(argv) <= 3:
        raise SystemExit("usage: build.py <PROJECT> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]")

    env, target = cli.prepare(argv[0])
    configurations = cli.configurations(cli.argument(argv, 1))
    rebuild = cli.build_type(cli.argument(argv, 2))
    return build_project(env, target, configurations, rebuild)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
