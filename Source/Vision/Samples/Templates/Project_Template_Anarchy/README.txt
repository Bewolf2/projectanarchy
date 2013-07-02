Project Template

A great place to start developing your own projects from.

The project is a basic template, complete with plugin; demo component,
demo action. It will also run in vForge’s “play the game” mode.


This project can be copied to any location outside of the Vision SDK folder. 

To get this project up and running:

The project contains templates for console actions and a project plugin
component example.

------------------------------------
Instructions for Windows and Android
------------------------------------

(1) Copy to folder to your new location.



(2) Run the included batch file RUN_ONCE.bat to copy the SDK shared files to your project.



(3) Open the visual studio *.sln file located in the Source folder. If some files or projects cannot be found, check if 
- the environment variable VISION_SDK is set to your Vision root directory
- the correct Android SDK and NDK is installed
- the environment variable ANDROID_NDK_ROOT is set to your Android NDK root


(4) Build and Run 

(5) (Only relevant on Windows and if you want to load the scene in vForge) For windows the project plugin can be located in the respective Bin folders. Copy this plugin to the bin folder with the same configuration where vForge is located (%VISION_SDK%\Bin). Once this is done make sure you start vForge on the same configuration (x86/x64, DX9/DX11, MSVS 2010/2012).

Debug version:   GamePlugin.vPluginD
Release version: GamePlugin.vPlugin



--------------------
Instructions for iOS
--------------------

(1) Copy to folder to your new location.



(2) Open the terminal, go to your copied project template folder and execute RUN_ONCE_IOS.sh file (e.g. /.RUN_ONCE_IOS.sh). The script will copy SDK shared files to your project template folder.


(3) Open the ProjectTemplate_ios.xcodeproj file located in the Source/GameApplication folder.


(4) Build and Run 



