import os, shutil, subprocess
from pathlib import Path

from SetupEnvironment import build_env, LIBRARIES, SERVICES, SERVICES_EXTRA, TOOLS

CLEAN_DIRS = [".vs", "build", "x64", "packages", "test"]
CMAKE_SUBPATH = Path("CommonExtensions") / "Microsoft" / "CMake" / "CMake" / "bin" / "cmake.exe"
BUILD_ENV = {"windows": {"VSLANG": "1033"}, "linux": {}}


def project_roots():
    explicit = {}
    for group in (SERVICES_EXTRA, LIBRARIES, TOOLS):
        explicit.update({var.removeprefix("OT_").removesuffix("_ROOT"): var for var in group})
    roots = dict(explicit)
    for key in SERVICES:
        roots[f"{key}_SERVICE" if key in explicit else key] = f"OT_{key}_SERVICE_ROOT"
    return roots


def resolve_root(env, key):
    roots = project_roots()
    var = roots.get(key.upper())
    if var not in env:
        raise SystemExit(f"Unknown project '{key}'. Known: " + ", ".join(sorted(roots)))
    return env[var]


def launch_devenv(env, target):
    devenv = Path(env["DEVENV_ROOT_2022"]) / "devenv.exe"
    if not devenv.is_file():
        raise SystemExit(f"devenv.exe not found: {devenv}")
    if not Path(target).exists():
        raise SystemExit(f"path does not exist: {target}")

    print("Launching development environment")
    subprocess.Popen([str(devenv), target], env=env)
    return 0


def build_project(env, target, configs, rebuild):
    cmake = Path(env["DEVENV_ROOT_2022"]) / CMAKE_SUBPATH
    if not cmake.is_file():
        raise SystemExit(f"cmake.exe not found: {cmake}")

    env = dict(env)
    system = "windows" if os.name == "nt" else "linux"
    env.update(BUILD_ENV[system])
    parallel = ["--parallel"] if env.get("OPENTWIN_DEV_PARALLEL_BUILDS") else []
    log_dir = Path.cwd()
    rc = 0

    print(f"Building Project {target}", flush=True)
    for cfg in configs:
        print(cfg.upper(), flush=True)
        log = log_dir / f"buildlog_{cfg.capitalize()}.txt"
        with open(log, "a", encoding="utf-8") as out:
            out.write("=" * 88 + "\n")
            out.write(f"Building project: {target}\n")
            out.write("=" * 88 + "\n")
            out.flush()

            def run(args):
                return subprocess.run(args, cwd=target, env=env, stdout=out, stderr=subprocess.STDOUT)

            run([str(cmake), "--preset", f"{system}-{cfg}"])
            if rebuild:
                run([str(cmake), "--build", "--preset", f"build-{system}-{cfg}", "--target", "clean"])
            result = run([str(cmake), "--build", "--preset", f"build-{system}-{cfg}"] + parallel)

            ok = result.returncode == 0
            out.write(f"--- Build {'successful' if ok else 'failed'}: {target} ---\n")

        if not ok:
            rc = result.returncode

    print("---", flush=True)
    print("SUCCESS" if rc == 0 else "FAILED", flush=True)
    return rc


def clean_project(target):
    if not Path(target).is_dir():
        raise SystemExit(f"path does not exist: {target}")

    print(f"Cleaning Project {target}", flush=True)
    failed = []
    for name in CLEAN_DIRS:
        path = Path(target) / name
        if not path.is_dir():
            continue
        shutil.rmtree(path, ignore_errors=True)
        if path.exists():
            failed.append(name)
        print(f"{name}{' (locked)' if path.exists() else ''}", flush=True)

    print("---", flush=True)
    print("FAILED" if failed else "SUCCESS", flush=True)
    return 1 if failed else 0
