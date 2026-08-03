import os
from pathlib import Path

import SetupEnvironment as definitions

from .expansion import expand, load_module


def check_required() -> None:
    missing = [name for name in definitions.REQUIRED if not os.environ.get(name)]
    if missing:
        raise SystemExit("Please specify the following environment variables: " + ", ".join(missing))


def _thirdparty(env: dict[str, str], third: Path) -> None:
    load_module("ot_thirdparty_env", third / definitions.THIRDPARTY_MODULE).apply(env)


def _projects(env: dict[str, str], dev: Path) -> None:
    env.update(definitions.RELATIVE)
    for group, folder in definitions.GROUPS:
        for name, project in group.items():
            env[name] = str(dev / folder / project)


def _dev_paths(env: dict[str, str], dev: Path) -> None:
    for name, relative in definitions.DEV_PATHS.items():
        env[name] = str(dev / relative)


def _certificates(env: dict[str, str], dev: Path) -> None:
    root = Path(env.get(definitions.CERTIFICATES_OVERRIDE) or dev / definitions.CERTIFICATES_ROOT)
    for name, filename in definitions.CERTIFICATES.items():
        env[name] = str(root / filename)


def _composites(env: dict[str, str]) -> None:
    for name, template in definitions.COMPOSITES.items():
        env[name] = expand(env, template)


def _service_args(env: dict[str, str], dev: Path) -> None:
    module = load_module("ot_service_args", dev.joinpath(*definitions.SERVICE_ARGS_MODULE))
    for name, template in module.DEFAULTS.items():
        if name not in env:
            env[name] = expand(env, template)


def _path(env: dict[str, str]) -> None:
    for entry in definitions.PATH_PREPEND:
        env["PATH"] = expand(env, f"{entry};%PATH%")


def build_env(base: str | Path | None = None) -> dict[str, str]:
    check_required()
    env = os.environ.copy()
    dev = Path(base or env["OPENTWIN_DEV_ROOT"])
    third = Path(env["OPENTWIN_THIRDPARTY_ROOT"])

    _thirdparty(env, third)
    _projects(env, dev)
    _dev_paths(env, dev)
    _certificates(env, dev)
    _composites(env)
    _service_args(env, dev)
    _path(env)

    env[definitions.READY_FLAG] = "1"
    print("OpenTwin Developer environment was set up successfully.")
    return env
