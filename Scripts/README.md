# OpenTwin Scripts

Contents

    1. Directory guide
    2. Environment Setup Guide
         SetupEnvironment.py
         DeploymentManifest.py
         BuildOrder.py
         Project batches
    3. Project Setup Guide


## 1. Directory guide

```
Scripts/
  SetupEnvironment.py     all development environment variables
  BuildOrder.py           build order of BuildAll/RebuildAll
  DeploymentManifest.py   what the deployments copy
  Python/                 the python system: entry scripts (*.py) and the ot_dev package
    set_python.bat        sets OT_PYTHON, call it before any entry script
  BuildAndTest/           RebuildAll, TestAll, CleanAll, deployments, build server jobs, build logs
  CMake/                  shared CMake modules and presets (OT_CMAKE_DIR)
  Installer/              NSIS installers
  Launcher/               start scripts that ship with the deployment
  Other/                  helpers: compress/decompress, file headers, URL scheme, cmake-gui/VS with env
```

Every project folder has its own build.bat, clean.bat, test.bat and edit.bat (see Project batches).


## 2. Environment Setup Guide

### SetupEnvironment.py

This single script module is the central environment setter.
In there you can set and change or delete various development related environment variables.
Each development 'Section' is divided by a **python dictionary**.

**To correctly set a new variable:**

1. As the key, set the variable name.
2. As the value, set the path or the name of the Service/Library/Tools directory (The system automatically resolves the path for these cases).

E.g. for the Service/Library/Tool variables:

```python
SERVICES = {
    "OT_MODEL_SERVICE_ROOT": "Model",
}

LIBRARIES = {
    "OT_GUI_ROOT": "OTGui",
}

TOOLS = {
    "OT_OTOOLKIT_ROOT": "OToolkit",
}
```

> [!IMPORTANT]
> For full paths with reference to other set variables, please use python raw strings (`r"..."`).
> To reference a different variable: `%VARIABLE%`
>
> ```python
> r"%OPENTWIN_DEV_ROOT%\Deployment",
> ```


### DeploymentManifest.py

The central DeploymentManifest to plan and set various deployment files.

This module contains the `UpdateDeployment` setup, the `CreateDeployment` setup, the `CreateFrontendInstaller` and the `CreateDebugFiles` setup.
Please check to write your changes to the correct functions accordingly:

```python
def update_libraries(plan) -> None:


def create_deployment(plan) -> None:


def create_frontend_installer(plan) -> None:


def create_debug_files(plan) -> None:
```

To add content to the deployment manifest edit the file with these new functions:

    plan.copy(source, target)          COPY, wildcards allowed
    plan.tree(source, target)          XCOPY /S, add empty=True for /E
    plan.mkdir(target)                 MKDIR
    plan.rmtree(target)                RMDIR /S /Q
    plan.remove(target)                DEL, wildcards allowed
    plan.remove_glob(folder, pattern)  recursive DEL
    plan.write(target, lines)          ECHO > file
    plan.rename(target, name)          REN
    plan.move(source, target)          MOVE
    plan.run(label, command, cwd)      runs a program (e.g. makensis)
    plan.action(name)                  a step ot_dev runs (see ACTIONS)

This module resembles the batch functions of the old batch system.


### BuildOrder.py

The planned build order of the `BuildAll` script.

Add or change the build order by changing the `BUILD_ORDER` list.
A correct list entry should only be the key of an environment variable:

E.g. OT_FOO_ROOT -> `FOO`

You can override the configurations of certain builds by inserting the project name into the: `BUILD_OVERRIDES`.

The tuple is (configurations, rebuild). In this example only the release configuration is built, and always as a full rebuild.

```python
BUILD_OVERRIDES = {
    "KEYGENERATOR": (["release"], True),
}
```

> [!TIP]
> FRAMEWORK, ADMINPANEL, KEYGENERATOR arent plain CMake builds and rely on different toolchains to be built.

### Project batches

We use the build/clean/test/edit.bat batch files that wrap our python system entry points of the same name: build/clean/test/edit.py.

To override a certain editor to start with the launching of the edit.bat, use an argument of our allowed editors:
- VS (Visual Studio)
- CODE (Visual Studio Code)
- NVIM (Neovim)

E.g.
```bat
"%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\edit.py" VISUALIZATION_SERVICE CODE
```

You can also start the edit.bat by just supplying the argument directly as a command line.

E.g.
```ps1
.\edit.bat CODE
```

To always start a different editor, set the `OT_DEFAULT_EDITOR` environment variable to VS, CODE or NVIM.
Without it, VS is used. An argument given to the edit.bat still wins over the variable.

The python interpreter is set via the `OT_PYTHON` variable, that is set by the /Scripts/Python/set_python.bat.
This batch script is central for the actual usage of the entry points of each function of the /Scripts/Python/*.py

> [!WARNING]
> To correctly use the python environment system, ALWAYS(!) keep the set_python.bat call, before using the OT_PYTHON variable.

---

## 3. Project Setup Guide

To set up a new project with the new environment system, follow these steps.

1. SetupEnvironment.py
   Add the project to SERVICES, LIBRARIES or TOOLS, depending on its folder.
   E.g. Libraries/OTFoo  ->  LIBRARIES = { ..., "OT_FOO_ROOT": "OTFoo" }

2. BuildOrder.py (optional)
   Add the key to BUILD_ORDER to build it with BuildAll/RebuildAll.
   Put it after the projects it depends on.
   E.g. OT_FOO_ROOT  ->  "FOO"

3. Project batches
   Copy build.bat, clean.bat, test.bat and edit.bat from a sibling project.
   In each, replace the project key after the script name:

       "%OT_PYTHON%" "%OPENTWIN_DEV_ROOT%\Scripts\Python\build.py" FOO %1 %2

4. CMake
   Copy the CMakePresets.json from a sibling project. It only includes the shared
   presets from Scripts/CMake, so it needs no changes.

5. Tests (optional)
   A tests/CMakeLists.txt in the project is enough, TestAll picks it up by itself.

6. Deployment (optional)
   If the project ships with OpenTwin, add its binaries to update_libraries
   in DeploymentManifest.py. CreateDeployment runs update_libraries as well.
