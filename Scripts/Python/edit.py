import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ot_commands import build_env, resolve_root, launch_editor

def main(argv):
    if not 1 <= len(argv) <= 2:
        raise SystemExit("usage: edit.py <PROJECT> [EDITOR]")

    env = build_env()
    target = resolve_root(env, argv[0])
    editor = argv[1] if len(argv) > 1 else None
    return launch_editor(env, target, editor)

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
