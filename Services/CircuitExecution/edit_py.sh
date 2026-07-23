#!/usr/bin/env bash
. "$OPENTWIN_DEV_ROOT/Scripts/set_python.sh"
"$OT_PYTHON" "$OPENTWIN_DEV_ROOT/Scripts/edit.py" CIRCUIT_EXECUTION || read -rp "Press Enter to continue... "
