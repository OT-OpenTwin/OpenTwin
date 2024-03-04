# MONGODB STORAGE SCRIPT NO AUTHORIZATION
# This script sets custom DB and log paths within the mongod.cfg
# it also sets any custom bindIP IP's the user has specified
# it gets all its values through command line args (sys.argv[n])

import os
import sys

#check if the specified folders exist
def check_folder_existence(path):
    if not os.path.exists(path): #if they don't exist > create
        os.makedirs(path)
        print(f"'{path}' does not exist yet. Creating directory: {path}")

def modify_mongod_cfg(file_path, new_db_path, new_log_path, new_bindip):
    check_folder_existence(new_db_path)
    check_folder_existence(new_log_path)

    with open(file_path, 'r') as file:
        lines = file.readlines() #read line for line - formatting is according to the .append function now
    
    #"path" under systemLog is a very vague and non unique tag descriptor
    #therefore, we would need an extra condition before changing a tag that's
    #just called "path" - hence the systemLog indicator
    #same goes with the security token, since it's commented out in the original
    #mongod.cfg 
    systemLog_indicator = False     #indicator if we're under systemLog or not
    security_indicator = False

    #empty "new lines"
    new_lines = []
    for line in lines:
        stripped_line = line.strip()        
        if stripped_line.startswith('dbPath:'):
            systemLog_indicator = False
            new_lines.append(f"  dbPath: {new_db_path}\n")

        elif stripped_line.startswith('systemLog:'):
            systemLog_indicator = True #set systemLog indicator
            new_lines.append(line)
             
        elif systemLog_indicator and stripped_line.startswith('path:'):
            if not new_log_path.endswith('\\'):
               new_log_path += '\\'
            new_log_path += 'mongodb.log'
            new_lines.append(f"  path: {new_log_path}\n") #spaces for intendation

        elif stripped_line.startswith('bindIp:'):
            new_lines.append(f"  bindIp: {new_bindip}\n")
            systemLog_indicator = False  # Assuming bindIp is not under systemLog

        elif stripped_line.startswith('#security:'):
            new_lines.append(line.replace('#security:', 'security:'))
            security_indicator = True  #set flag for finding and uncommenting security section
        elif stripped_line.startswith('security:'):
            new_lines.append(line)
            security_indicator = True
        elif security_indicator and (stripped_line == '' or line.startswith(' ')):  # Check for an empty line or indentation indicating the next line within security
            new_lines.append(f"  authorization: {new_authorization}\n\n")
            security_indicator = False  #reset indicator after adding authorization
        else:
            new_lines.append(line)
            if line.strip() == '':  #indicator reset if an empty line is scanned, for safety
                systemLog_indicator = False

    #write everything back into .cfg
    with open(file_path, 'w') as file:
        file.writelines(new_lines)

if __name__ == "__main__":
    if len(sys.argv) < 6:
        print("Not enough arguments. Usage: <SCRIPTNAME>.py <cfg_file_path> <new_db_path> <new_log_path> <new_bindip>")
    else:
        file_path = sys.argv[1]
        new_db_path = sys.argv[2]
        new_log_path = sys.argv[3]
        new_bindip = sys.argv[4]
        new_authorization = sys.argv[5]
        #function call
        modify_mongod_cfg(file_path, new_db_path, new_log_path, new_bindip)