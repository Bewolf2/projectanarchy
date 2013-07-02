// ===========================================================
// Havok Behavior Lua Module - SWIG Interface
// ===========================================================
// This interface file generates BehaviorLuaModule_wrapper.cpp
// and is represented in Lua as the "Behavior" module.
 
#ifndef VLUA_APIDOC
 
//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

//dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObjectComponent.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObject3D.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiBaseEntity.i>

//custom headers for generated source file
%module Behavior
%{
	#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptRenderer_wrapper.hpp>
	#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>
%}

%immutable;

%mutable;

%include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/Lua/vHavokBehaviorComponent.i>;

#else

/// DOCS

#endif