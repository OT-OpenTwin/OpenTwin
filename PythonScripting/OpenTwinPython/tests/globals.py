import logging
from opentwin.logging import logging_config

def ot_initialize_tests():
    # Ensure that initialization is done only once
    if hasattr(ot_initialize_tests, "initialized"):
        return
    
    ot_initialize_tests.initialized = True

    logging_config.configure_logging(console=True, file=False, level=logging.DEBUG)

