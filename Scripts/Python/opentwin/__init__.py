import sys
from pathlib import Path

_ROOT = str(Path(__file__).resolve().parent.parent)
if _ROOT not in sys.path:
    sys.path.insert(0, _ROOT)

from .actions import build_project, clean_project, launch_editor
from .admin_panel import build_admin_panel
from .batch import build_all, clean_all
from .environment import build_env, check_required
from .framework import build_framework
from .projects import project_roots, resolve_root
from .toolchain import apply_toolchain

__all__ = [
    "apply_toolchain",
    "build_admin_panel",
    "build_all",
    "build_env",
    "build_framework",
    "build_project",
    "check_required",
    "clean_all",
    "clean_project",
    "launch_editor",
    "project_roots",
    "resolve_root",
]
