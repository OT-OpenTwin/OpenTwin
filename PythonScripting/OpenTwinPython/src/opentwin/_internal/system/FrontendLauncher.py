# @otlicense

# Python standard
import os
import sys
import subprocess
from pathlib import Path

class FrontendLauncher:
    
    def __init__(self, frontend_executable_path: Path) -> None:
        self._frontend_executable_path = Path(frontend_executable_path).resolve()

        if not self._frontend_executable_path.is_file():
            raise FileNotFoundError(f"Frontend executable not found: {self._frontend_executable_path}")

        if sys.platform != "win32" and not os.access(self._frontend_executable_path, os.X_OK):
            raise PermissionError(f"Frontend executable is not executable: {self._frontend_executable_path}")

    def launch(self, args: list[str]) -> None:
        if not isinstance(args, list) or not all(isinstance(arg, str) for arg in args):
            raise TypeError("args must be a list of strings")

        command = [str(self._frontend_executable_path), *args]

        try:
            if sys.platform == "win32":
                subprocess.Popen(
                    command,
                    creationflags=(subprocess.DETACHED_PROCESS 
                                   | subprocess.CREATE_NEW_PROCESS_GROUP
                    ),
                    close_fds=True,
                    cwd=self._frontend_executable_path.parent,
                )

            elif sys.platform in ("linux", "darwin"):
                subprocess.Popen(
                    command,
                    start_new_session=True,
                    close_fds=True,
                    cwd=self._frontend_executable_path.parent,
                )

            else:
                raise NotImplementedError(f"Unsupported operating system: {sys.platform}")

        except OSError as exc:
            raise RuntimeError(f"Failed to launch OpenTwin frontend: {self._frontend_executable_path}") from exc
