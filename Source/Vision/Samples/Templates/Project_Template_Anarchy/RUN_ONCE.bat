::@ECHO OFF
echo off

REM - Set root directory

set CURR_DIR=%CD%\
set ROOT_DIR=%CD%\
echo Files will be copied from:
echo %VISION_SDK%
echo to:
echo %CURR_DIR%

REM - Start copy (MSVS 2010:x86:DX9/DX11)

set ARCH=win32_vs2010_anarchy


set CONF=Bin\%ARCH%\dev_dll\DX9
set SRC_DIR=%VISION_SDK%\%CONF%
md %CONF%
xcopy "%SRC_DIR%\fmodex.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\lua100.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\Base.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\BaseUI.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\Vision.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\VisionEnginePlugin.vPlugin" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\vHavok.vPlugin" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\vFmodEnginePlugin.vPlugin" "%ROOT_DIR%\%CONF%" /Y


set CONF=Bin\%ARCH%\debug_dll\DX9
set SRC_DIR=%VISION_SDK%\%CONF%
md %CONF%
xcopy "%SRC_DIR%\fmodexL.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\lua100D.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\BaseD.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\BaseUID.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\VisionD.dll" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\VisionEnginePlugin.vPluginD" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\vHavok.vPluginD" "%ROOT_DIR%\%CONF%" /Y
xcopy "%SRC_DIR%\vFmodEnginePlugin.vPluginD" "%ROOT_DIR%\%CONF%" /Y


REM - Start copy (Base data)

set SRC_DIR=%VISION_SDK%\Data\Vision\Base
md Data\Vision\Base
xcopy "%SRC_DIR%" /S "%ROOT_DIR%\Data\Vision\Base" /Y

xcopy "%VISION_SDK%\Source\Vision\Runtime\Common\VisSampleApp.cpp" /S %ROOT_DIR%\Source\GameApplication /Y
xcopy "%VISION_SDK%\Source\Vision\Runtime\Common\VisSampleAppCallbacks.cpp" /S %ROOT_DIR%\Source\GameApplication /Y