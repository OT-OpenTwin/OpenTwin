# logging_config.py

import logging
from logging.handlers import RotatingFileHandler
from pathlib import Path

def configure_logging(console: bool = False,
                      file: bool = False,
                      level: int = logging.INFO,
                      log_file_path: Path | None = None
                      ) -> None:

    logger = logging.getLogger("OpenTwin")
    logger.setLevel(level)
    logger.propagate = False

    # Remove existing handlers to avoid duplicates.
    for handler in logger.handlers[:]:
        logger.removeHandler(handler)
        handler.close()

    formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(name)s: %(message)s")

    if console:
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)

    if file:
        if log_file_path is None:
            raise ValueError("A log file path must be provided.")

        file_handler = RotatingFileHandler(
            log_file_path,
            maxBytes = 5 * 1024 * 1024,
            backupCount = 3,
            encoding = "utf-8"
        )
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)