import re

_VAR = re.compile(r"%([^%]+)%")

DEFAULTS = {
    "OPEN_TWIN_MONGODB_ADDRESS": "127.0.0.1:27017",
    "OPEN_TWIN_SERVICES_ADDRESS": "127.0.0.1",
    "OPEN_TWIN_TOOLKIT_PORT": "8001",
    "OPEN_TWIN_LOG_PORT": "8090",
    "OPEN_TWIN_GSS_PORT": "8091",
    "OPEN_TWIN_LSS_PORT": "8093",
    "OPEN_TWIN_AUTH_PORT": "8092",
    "OPEN_TWIN_GDS_PORT": "9094",
    "OPEN_TWIN_LDS_PORT": "9095",
    "OPEN_TWIN_ADMIN_PORT": "8000",
    "OPEN_TWIN_LMS_PORT": "8002",
    "OPEN_TWIN_SITE_ID": "1",
    "OPEN_TWIN_DOWNLOAD_PORT": "80",
    "OPEN_TWIN_LOGGING_URL": r"%OPEN_TWIN_SERVICES_ADDRESS%:%OPEN_TWIN_LOG_PORT%",
    "OPEN_TWIN_LOGGING_MODE": '"WARNING_LOG|ERROR_LOG"',
}


def apply(env):
    for name, template in DEFAULTS.items():
        if name not in env:
            env[name] = _VAR.sub(lambda m: env.get(m.group(1), ""), template)
    return env
