import importlib.util
import re
from pathlib import Path
from types import ModuleType
from typing import Iterable, Mapping, MutableMapping

_VAR = re.compile(r"%([^%]+)%")


def get(env: Mapping[str, str], name: str) -> str:
    if name in env:
        return env[name]
    low = name.lower()
    return next((value for key, value in env.items() if key.lower() == low), "")


def expand(env: Mapping[str, str], value: str) -> str:
    return _VAR.sub(lambda match: get(env, match.group(1)), value)


def merge(env: MutableMapping[str, str], values: Mapping[str, str]) -> MutableMapping[str, str]:
    names = {key.lower(): key for key in env}
    for name, value in values.items():
        env[names.get(name.lower(), name)] = value
    return env


def unique(values: Iterable[str]) -> list[str]:
    seen: set[str] = set()
    result: list[str] = []
    for value in values:
        key = value.lower()
        if value and key not in seen:
            seen.add(key)
            result.append(value)
    return result


def load_module(name: str, path: Path) -> ModuleType:
    spec = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise SystemExit(f"Cannot load module {name!r} from {path}")

    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module
