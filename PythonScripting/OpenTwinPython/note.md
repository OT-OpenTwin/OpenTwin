# OpenTwin Python Client

A Python client library for interacting with the OpenTwin platform.

## Development

Create and activate a virtual environment, then install the
package in editable mode:

```powershell
py -m pip install -e .
```

## Usage

```python
from opentwin import OpenTwinClient

client = OpenTwinClient()
client.login()
```

The login method is currently a placeholder and does not
perform authentication.


## Install
From the project root, with your virtual environment active, run:

```
python -m pip install --upgrade pip
python -m pip install -e .
```

Editable installation means changes to your Python source files are available without rebuilding and reinstalling the package after each modification.