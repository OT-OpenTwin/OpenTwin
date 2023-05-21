########################################
#
#	Setup the environment
#
########################################

Read the README.md file in the OpenTwin repository

########################################
#
#	Setup the batch files
#
########################################

1) build.bat
In the line after: ECHO %TYPE_NAME% DEBUG, change the file path to your service project path
In the line after: ECHO %TYPE_NAME% RELEASE, change the file path to your service project path

2) edit.bat
In the line after: REM Open project, change the file path to your service project path

########################################
#
#	Edit your project
#
########################################

Use the edit.bat file to start the VisualStudio2017 development environment with all environment variables set

########################################
#
#	Setup the project		
#
########################################

Project Properties:
Configuration Properties->Debugging->Command Argument:	Set the arguments according to your requirements (...)
