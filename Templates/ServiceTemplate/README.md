# Service Template

Copy this folder into `Services/`, rename it, and work through the steps below.
It builds with CMake and the Python scripts under `Scripts/Python`.

## 1. Rename

| file | what to change |
| --- | --- |
| folder | `Services/<YourService>` |
| `CMakeLists.txt` | `project()` and every `ServiceTemplate` occurrence |
| `tests/CMakeLists.txt` | `project()` and `ot_initialize_test` |
| `build.bat` `clean.bat` `edit.bat` `test.bat` | replace `SERVICE_TEMPLATE` with your key |

## 2. Register the service

`Scripts/SetupEnvironment.py` - add the root to `SERVICES`:

    "OT_MY_SERVICE_ROOT": "MyService",

The key used by the batch files is that name without `OT_` and `_ROOT`,
so `OT_MY_SERVICE_ROOT` is driven as `MY_SERVICE`.

`Scripts/BuildOrder.py` - add the key to `BUILD_ORDER` in dependency order.

## 3. Build, test and edit

    build.bat  [DEBUG|RELEASE|BOTH] [BUILD|REBUILD]
    test.bat   [DEBUG|RELEASE|BOTH]
    clean.bat
    edit.bat

## 4. Dependencies

Add tokens to `ot_add_dependency` in `CMakeLists.txt`. The tokens are defined in
`Scripts/CMake/OTProject.cmake`, and third party paths come from the ThirdParty
`SetupEnvironment` - never `find_package`.

## 5. Optional

Services that need a debug launch entry add `ot_service_debug_launch` with their
own port; see `Services/LoggerService/CMakeLists.txt`.
