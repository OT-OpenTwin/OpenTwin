# Environment Setup Guide:

## SetupEnvironment.py:

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

[!IMPORTANT] For full paths with reference to other set variables, please use pythons r-strings:
[!IMPORTANT] To reference a different variable: `%VARIABLE%`

```python
    r"%OPENTWIN_DEV_ROOT%\Deployment",
```


## DeploymentManifest.py

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
    plan.action(name)                  a step ot_dev runs (see ACTIONS)

This module resembles the batch functions of the old batch system.


## BuildOrder.py

The planned build order of the `BuildAll` script.

Add or change the build order by changing the `BUILD_ORDER` list.
A correct list entry should only be the key of a environment variable:

E.g. OT_FOO_ROOT -> FOO
