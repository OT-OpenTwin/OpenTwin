import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ot_commands import build_env, resolve_root, clean_project


def main(argv):
    if len(argv) != 1:
        raise SystemExit("usage: clean.py <PROJECT>")

    env = build_env()
    target = resolve_root(env, argv[0])
    return clean_project(target)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
