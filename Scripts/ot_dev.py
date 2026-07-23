import os, subprocess
from pathlib import Path

from SetupEnvironment import build_env, SERVICES


def resolve_root(env, key):
    key = key.upper()
    for var in (f"OT_{key}_SERVICE_ROOT", f"OT_{key}_ROOT"):
        if var in env:
            return env[var]
    raise SystemExit(f"Unknown service '{key}'. Known: " + ", ".join(sorted(SERVICES)))


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
    cmake = Path(env["DEVENV_ROOT_2022"]) / "CommonExtensions" / "Microsoft" / "CMake" / "CMake" / "bin" / "cmake.exe"
    if not cmake.is_file():
        raise SystemExit(f"cmake.exe not found: {cmake}")

    env = dict(env)
    system = "windows" if os.name == "nt" else "linux"
    if system == "windows":
        env["VSLANG"] = "1033"
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

        print(f"--- Build {'successful' if ok else 'failed'}: {cfg.upper()} ---", flush=True)
        if not ok:
            rc = result.returncode

    return rc
