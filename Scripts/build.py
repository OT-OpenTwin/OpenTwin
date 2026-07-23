import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ot_dev import build_env, resolve_root, build_project


def main(argv):
    if not argv:
        raise SystemExit("usage: build.py <SERVICE_KEY> [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]")

    config = (argv[1] if len(argv) > 1 else "BOTH").upper()
    buildtype = (argv[2] if len(argv) > 2 else "REBUILD").upper()
    configs = {"DEBUG": ["debug"], "RELEASE": ["release"]}.get(config, ["debug", "release"])

    env = build_env()
    target = resolve_root(env, argv[0])
    return build_project(env, target, configs, buildtype != "BUILD")


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
