#!/bin/sh

mkdir -p ./Data/Vision
cp -R -f "$VISION_SDK/Data/Vision/Base" ./Data/Vision

mkdir -p ./Data/Common/iOS
cp -R -f "$VISION_SDK/Data/Common/iOS" ./Data/Common

mkdir -p ./Source/GameApplication/iOS
cp -f "$VISION_SDK/Source/Vision/Runtime/Common/VisSampleApp.cpp" ./Source/GameApplication
cp -f "$VISION_SDK/Source/Vision/Runtime/Common/VisSampleAppCallbacks.cpp" ./Source/GameApplication
cp -R -f "$VISION_SDK/Source/Vision/Runtime/Common/iOS" ./Source/GameApplication