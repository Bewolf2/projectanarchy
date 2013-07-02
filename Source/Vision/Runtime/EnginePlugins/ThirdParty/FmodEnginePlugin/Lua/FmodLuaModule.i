// ===========================================================
// Fmod Lua Module - SWIG Interface
// ===========================================================
// This interface file generates FmodLuaModule_wrapper.cpp
// and is represented in Lua as the "Fmod" module.

#ifndef VLUA_APIDOC

//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

//dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObject3D.i>

//custom headers for generated source file
%module FireLight
%{
  #include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>
%}
  
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodManager.i>;
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodSoundObject.i>;
%include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/Lua/VFmodEvent.i>;

#endif