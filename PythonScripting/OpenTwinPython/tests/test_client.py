from opentwin import OpenTwinClient

def test_login_does_nothing():
    client = OpenTwinClient()

    result = client.login()

    assert result is None