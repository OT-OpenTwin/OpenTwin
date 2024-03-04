# CHANGE PERMISSIONS FOR CUSTOM DB AND LOG PATHS FOR MONGODB
# MongoDB's service needs full access to the folder that get created by
# the storage scripts and this script takes care of that

import subprocess
import sys

def set_permissions(folder_path):
    command = ['icacls', folder_path, '/grant', 'NETWORK SERVICE:(OI)(CI)F', '/T']
    try:
        subprocess.run(command, check=True)
        print(f"Successfully set permissions for NETWORK SERVICE on {folder_path}")
    except subprocess.CalledProcessError as e:
        print(f"Failed to set permissions for {folder_path}: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: <SCRIPTNAME>.py <db_path> <log_path>")
        sys.exit(1)

    db_path = sys.argv[1]
    log_path = sys.argv[2]

    #function call
    set_permissions(db_path)
    set_permissions(log_path)
