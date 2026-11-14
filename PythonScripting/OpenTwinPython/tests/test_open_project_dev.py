import os
from opentwin import *
from tests.globals import ot_initialize_tests as ini

def test_login_does_nothing():
    ini()

    print("Running test...")
 
    # first read token from env variable OPEN_TWIN_ACCESS_TOKEN
    token = os.environ.get("OPEN_TWIN_ACCESS_TOKEN")
    if token is None:
        raise RuntimeError("OPEN_TWIN_ACCESS_TOKEN environment variable is not set.")

    if not isinstance(token, str):
        raise TypeError("OPEN_TWIN_ACCESS_TOKEN must be a string.")

    if token == "":
        raise ValueError("OPEN_TWIN_ACCESS_TOKEN cannot be an empty string.")

    client = OpenTwinClient("https://127.0.0.1:8091", token)

    assert client is not None