from typing import Mapping

import SetupEnvironment as definitions


def project_roots() -> dict[str, str]:
    return {name.removeprefix("OT_").removesuffix("_ROOT"): name
            for group, _ in definitions.GROUPS for name in group}


def resolve_root(env: Mapping[str, str], key: str) -> str:
    roots = project_roots()
    name = roots.get(key.upper())
    if name not in env:
        raise SystemExit(f"Unknown project '{key}'. Known: " + ", ".join(sorted(roots)))
    return env[name]
