#Requires that the BAT Nr selection is updated first.
import OpenTwin
import os

def __main__(this):
    tableName = OpenTwin.GetPropertyValue(this,"Table name")

    print(tableName)
    directory, filename = os.path.split(tableName)
    
    # Split filename by underscores
    parts = filename.split("_")

    # Target part is index 6 (between 7th and 8th underscores)
    num_str = parts[7]

    # Handle optional '#' prefix
    if num_str.startswith("#"):
        prefix = "#"
        digits = num_str[1:]
    else:
        prefix = ""
        digits = num_str
    print(num_str)
    # Increment while preserving zero padding
    num = int(digits) + 1
    new_num_str = prefix + str(num).zfill(len(digits))

    # Replace the part
    parts[7] = new_num_str

    # Reconstruct the filename
    newTableName = "_".join(parts)
    newTableName = directory + "/" + newTableName
    setSuccess = OpenTwin.SetPropertyValue(this,"Table name",newTableName)
    if not setSuccess:    
        raise Exception('Failed to update property of ', this)

