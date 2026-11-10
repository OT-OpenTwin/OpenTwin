# @otlicense

from pathlib import Path

from platformdirs import user_data_dir as platformdirs_user_data_dir
from platformdirs import user_config_dir as platformdirs_user_config_dir
from platformdirs import user_cache_dir as platformdirs_user_cache_dir
 
class DirectoryManager:
    @staticmethod
    def get_local_user_data_directory() -> Path:
        return Path(platformdirs_user_data_dir("OpenTwin", appauthor=False))

    @staticmethod
    def get_local_user_config_directory() -> Path:
        return Path(platformdirs_user_config_dir("OpenTwin", appauthor=False))

    @staticmethod
    def get_local_user_cache_directory() -> Path:
        return Path(platformdirs_user_cache_dir("OpenTwin", appauthor=False))