# MONGODB STORAGE SCRIPT WITH AUTHORIZATION
# This script skips the steps from the noAuth script and just appends
# a few lines for the TLS tag in the cfg and changes the previously
# set security -> authorization tag to whatever sys.argv[2] is

import os
import sys

#check if the specified folders exist
def check_folder_existence(path):
    if not os.path.exists(path): #if they don't exist > create
        os.makedirs(path)
        print(f"'{path}' does not exist yet. Creating directory: {path}")

def modify_mongod_cfg(file_path, new_authorization, tls_certificate_key_file_path, new_port):

    with open(file_path, 'r') as file:
        lines = file.readlines() #read line for line - formatting is according to the .append function now
    
    #using 'indicators' to determine if we're at a specific part of the cfg or not
    security_indicator = False      #indicator if we found the security tag or not
    net_indicator = False           #indicator for net section

    #empty "new lines"
    new_lines = []
    for line in lines:
        stripped_line = line.strip()        
        #check for commented-out security section
        if stripped_line.startswith('security:'):
            new_lines.append(line)
            security_indicator = True
        # now that security_indicator is set to true > look for the next line to insert authorization
        elif security_indicator and (stripped_line == '' or line.startswith(' ')):  # Check for an empty line or indentation indicating the next line within security
            new_lines.append(f"  authorization: {new_authorization}\n")
            security_indicator = False  #reset indicator after adding authorization

        elif stripped_line.startswith('port:'):
            new_lines.append(f"  port: {new_port}\n")

        elif stripped_line.startswith('net:'):
            net_indicator = True
            new_lines.append(line)

        elif net_indicator and stripped_line == '':
            new_lines.append("  tls:\n")
            new_lines.append(f"    mode: requireTLS\n")
            new_lines.append(f"    certificateKeyFile: {tls_certificate_key_file_path}\n")
            net_indicator = False

        else:
            new_lines.append(line)
            if line.strip() == '':  #indicator reset if an empty line is scanned, for safety
                security_indicator = False
                net_indicator = False

    #write everything back into .cfg
    with open(file_path, 'w') as file:
        file.writelines(new_lines)

if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("Not enough arguments. Usage: <SCRIPTNAME>.py <cfg_file_path> <new_authorization> <certificate_path> <port>")
    
    else:
        file_path = sys.argv[1]
        new_authorization = sys.argv[2]
        tls_certificate_key_file_path = sys.argv[3]
        new_port = sys.argv[4]
        #function call
        modify_mongod_cfg(file_path, new_authorization, tls_certificate_key_file_path, new_port)
