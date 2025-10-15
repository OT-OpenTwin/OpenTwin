# This script will go trough all OpenTwin header and source files and print lines matching the following criteria:
# - The line contains the virtual keyword but does not contain either "= 0", "~" or "override"

import os
import sys

def find_virtual_lines(root_dir):
    """
    Search through specified directories for files with .h, .cpp, .hpp extensions
    and find lines containing 'virtual' but not containing '= 0' or 'override'
    """
    
    # Target directories to search (direct children of root)
    target_dirs = {"Libraries", "Services", "Tools"}
    
    # Directories to skip at any level
    skip_dirs = {".vs", "x64", "packages", "AdminPanel"}
    
    # File extensions to process
    target_extensions = {".h", ".cpp", ".hpp"}
    
    # Check if root directory exists
    if not os.path.exists(root_dir):
        print(f"Error: Directory '{root_dir}' does not exist.")
        return
    
    # Get immediate subdirectories of root
    try:
        root_contents = os.listdir(root_dir)
    except PermissionError:
        print(f"Error: Permission denied accessing '{root_dir}'")
        return
    
    # Find target directories that exist
    found_target_dirs = []
    for item in root_contents:
        item_path = os.path.join(root_dir, item)
        if item in target_dirs and os.path.isdir(item_path):
            found_target_dirs.append(item_path)
    
    if not found_target_dirs:
        print(f"No target directories ({', '.join(target_dirs)}) found in '{root_dir}'")
        return
    
    print(f"Searching in directories: {', '.join([os.path.basename(d) for d in found_target_dirs])}")
    print("-" * 80)
    
    # Process each target directory
    for target_dir in found_target_dirs:
        print(f"Processing: {target_dir}")
        process_directory(target_dir, skip_dirs, target_extensions)

def process_directory(directory, skip_dirs, target_extensions):
    """
    Recursively process a directory and its subdirectories
    """
    try:
        for item in os.listdir(directory):
            item_path = os.path.join(directory, item)
            
            # Skip if it's in our skip list
            if item in skip_dirs:
                continue
                
            if os.path.isdir(item_path):
                # Recursively process subdirectories
                process_directory(item_path, skip_dirs, target_extensions)
            elif os.path.isfile(item_path):
                # Check if file has target extension
                _, ext = os.path.splitext(item)
                if ext.lower() in target_extensions:
                    process_file(item_path)
    except PermissionError:
        print(f"Permission denied: {directory}")
    except Exception as e:
        print(f"Error processing {directory}: {e}")

def process_file(file_path):
    """
    Process a single file and find matching lines
    """
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
            for line_num, line in enumerate(file, 1):
                line = line.rstrip('\n\r')
                line = line.lstrip()
                
                # Check if line contains 'virtual' but not '= 0' or 'override'
                if (line.startswith("virtual") and 
                    "= 0" not in line and 
                    "~" not in line and 
                    "override" not in line):
                    print(f"{file_path}:{line_num}:   {line}")
                    
    except UnicodeDecodeError:
        # Try with different encoding if UTF-8 fails
        try:
            with open(file_path, 'r', encoding='latin-1', errors='ignore') as file:
                for line_num, line in enumerate(file, 1):
                    line = line.rstrip('\n\r')
                    line = line.lstrip()
                    if (line.startswith("virtual") and 
                        "= 0" not in line and 
                        "~" not in line and 
                        "override" not in line):
                        print(f"{file_path}:{line_num}:   {line}")
        except Exception as e:
            print(f"Error reading file {file_path}: {e}")
    except Exception as e:
        print(f"Error processing file {file_path}: {e}")

def main():
    """
    Main function to read root directory from environment variable and start the search
    """
    root_directory = os.environ.get("OPENTWIN_DEV_ROOT")
    
    if not root_directory:
        print("Error: Environment variable OPENTWIN_DEV_ROOT is not set.")
        print("Please set the environment variable and try again.")
        sys.exit(1)
    
    if not os.path.exists(root_directory):
        print(f"Error: Directory '{root_directory}' specified in OPENTWIN_DEV_ROOT does not exist.")
        sys.exit(1)
    
    print(f"Using root directory from OPENTWIN_DEV_ROOT: {root_directory}")
    find_virtual_lines(root_directory)

if __name__ == "__main__":
    main()