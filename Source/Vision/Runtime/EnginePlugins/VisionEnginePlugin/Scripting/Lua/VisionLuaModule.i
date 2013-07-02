/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ===========================================================
// VisionLua Module - SWIG Interface
// ===========================================================
// This interface file generates VisionLuaModule_wrapper.cpp
// and is represented in Lua as the "Vision" module.

#ifndef VLUA_APIDOC

//definitions for types like UINT, VBool, etc...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>
%include <typemaps.i>

//custom headers for generated source file
%module Vision
%{
  #include <Vision/Runtime/Base/VGL/VGL.hpp>
  #include <Vision/Runtime/Engine/SceneElements/VisApiPathNode.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionManager.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Components/VAnimationComponent.hpp>
//  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Components/VTimedValueComponent.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptConstants.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDraw_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptInput_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDebug_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptScreen_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptRenderer_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptUtil_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptApp_wrapper.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VEnginePluginElementManager.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RendererNode/VRendererNodeCommon.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/PostProcessBase.hpp>  
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/CubeMapHandle.hpp>
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>
%}

//common used constants
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptConstants.hpp>;

////////////////////////////////
//       Primitives           //
////////////////////////////////
//primitives
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/hkvVec3.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/hkvMat3.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/hkvAlignedBBox.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.i>;

/////////////////////////////////////////////////////////
//       Typed engine objects or ref counted           //
//(types without a constructor in the public interface)//
/////////////////////////////////////////////////////////
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObject3D.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObjectComponent.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiBaseEntity.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/ParticleGroupBase.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiDynamicMesh.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiLightSource.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiStaticMeshInstance.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiSurface.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisPath_cl.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisPathNode_cl.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTextureObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiContextCamera.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiRenderer.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/TriggerBoxEntity.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiScreenMask.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/IVRendererNode.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/PostProcessBase.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/CubeMapHandle.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/PathCameraEntity.i>

//components
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTriggerComponent.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTransitionStateMachine.i>;

//pseudo components (created for scripting purpose only)
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VAnimationComponent.i>
//%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTimedValueComponent.i>

/////////////////////////////////
//    GUI and Input classes    //
/////////////////////////////////
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VGUIManager.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VWindowBase.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VDialog.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VStringInputMap.i>

////////////////////////////////
//       Static classes       //
////////////////////////////////
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiGame.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionTimer.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisionConsoleManager.i>

//static wrappers (created only for scripting)
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDraw_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptDebug_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptInput_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptUtil_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptRenderer_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptScreen_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptFileSystem_wrapper.i>;
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptApp_wrapper.i>;

%native(Assert) int Vision_Assert(lua_State *L);
%{
  int Vision_Assert(lua_State *L)
  {
    #ifdef _DEBUG
      DECLARE_ARGS_OK;
      
      GET_ARG(1, bool, bCondition);
      GET_OPT_ARG(2, const char *, szMessage, "Lua script assertion failed!");

      if (ARGS_OK)
      {
        VASSERT_MSG(bCondition, szMessage);
      }
      else
      {
        VASSERT_MSG(bCondition, "Invalid Assertion, use: Vision.Assert(condition, message)");
      }
    #endif
    return 0;
  }
%}
#else

/// \brief MODULE: The Lua \b Vision module contains all generated wrapper classes of the VisionEnginePlugin.
/// Since all classes, which are derrived from VisTypedEngineObject_cl have no constructor you can only access
/// primitive objects and constants via the Vision module. For all other objects use the global \b Game
/// instance (class VisGame_cl) to create or search them.
/// \par Example
///   \code
///     -- primitve objects
///     local vector = Vision.hkvVec3(3,4,5)
///     local matrix = Vision.hkvMat3()
///     local color = Vision.VColorRef(255,0,0)
///     local bitmask = Vision.VBitmask()
///     local box = Vision.hkvAlignedBBox()
///
///     -- constants
///     local runInEditor = Vision.EDITOR_RUN
///     local spotLight = Vision.LIGHT_SPOT
///
///     -- VisTypedEngineObject_cl derrived objects (accessed via the Game instance,
///     -- therfore you don`t need to care about the module)
///     local entity = Game:GetEntity("Warrior")
///     local light = Game:CreateLight(Vision.hkvVec3(300,0,300), Vision.LIGHT_POINT, 400)
///   \endcode
class Vision {
public:

  /// @name Static Module Function
  /// @{
  
  /// \brief Static function to perform an assertion - \b only \b in \b DEBUG \b build.
  /// \param condition The boolean condition to proof.
  /// \param message (\b optional) A message giving a hint for the assertion.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       Vision.Assert(self:GetMesh()~=nil, "Expected a mesh!")
  ///     end
  ///   \endcode
  static void Assert(boolean condition, string message = nil);
  
  /// @}
  /// @name Callbacks for Scene- and GameScripts
  /// @{
  
  /// \brief Overridable callback function. Called after a scene has been loaded.
  /// \details All scene objects already have been created.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded()
  ///       Debug:PrintLine("Scene loaded...")
  ///     end
  ///   \endcode
  void OnAfterSceneLoaded();
  
  /// \brief Overridable callback function. Called before the scene is loaded.
  /// \details No shapes are present at this moment.
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded()
  ///       G.MaxTime = 120
  ///       G.Difficulty = ReadDifficultyFromConfig()
  ///     end
  ///   \endcode
  void OnBeforeSceneLoaded();
  
  /// \brief Overridable callback function. Called before the scene is unloaded.
  /// \details All shapes are still present.
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneUnloaded()
  ///       CleanupCreatedResources()
  ///     end
  ///   \endcode
  void OnBeforeSceneUnloaded();

  /// \brief Overridable callback function. Called after the scene is unloaded.
  /// \details All scene objects already have been deleted.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneUnloaded()
  ///       Debug:Log("Executing Lua callback OnAfterSceneUnloaded")
  ///     end
  ///   \endcode
  void OnAfterSceneUnloaded();

  /// @}
  /// @name Primitive Types (having a Constructor in Lua)
  /// @{
  
  static hkvVec3 hkvVec3(...);
  static hkvMat3 hkvMat3(...);
  static VColorRef VColorRef(...);
  static VBitmask VBitmask(...);
  static hkvAlignedBBox hkvAlignedBBox(...);

  /// @}
  /// @name Named Colors
  /// @{
  
  VColorRef  V_RGBA_WHITE;
  VColorRef  V_RGBA_GREY;
  VColorRef  V_RGBA_BLACK;
  VColorRef  V_RGBA_RED;
  VColorRef  V_RGBA_YELLOW;
  VColorRef  V_RGBA_GREEN;
  VColorRef  V_RGBA_CYAN;
  VColorRef  V_RGBA_BLUE;
  VColorRef  V_RGBA_PURPLE;

  /// @}
  /// @name Transition State Events
  /// @{
  
  number EVENT_TRANSITION_STARTED = 1;
  number EVENT_TRANSITION_FINISHED = 2;
  number EVENT_SEQUENCE_STARTED = 3;
  number EVENT_SEQUENCE_FINISHED = 4;
  number EVENT_TRANSITION_START_REACHED = 5;

  /// @}
  /// @name PC Keyboard and Mouse Input (not intended to be used with the Input Trigger System!)
  /// @{
    
  number KEY_UNKNOWN = VGLK_UNKNOWN;
  number KEY_LSHIFT = VGLK_LSHIFT;
  number KEY_RSHIFT = VGLK_RSHIFT;
  number KEY_LCTRL = VGLK_LCTRL;
  number KEY_RCTRL = VGLK_RCTRL;

  number KEY_LALT = VGLK_LALT;
  number KEY_RALT  = VGLK_RALT;
  number KEY_ALTGR = VGLK_ALTGR;

  number KEY_BACKSPACE = VGLK_BACKSP;
  number KEY_TAB = VGLK_TAB;

  number KEY_LWIN = VGLK_LWIN;
  number KEY_RWIN = VGLK_RWIN;
  number KEY_APPS = VGLK_APPS;
  number KEY_ENTER = VGLK_ENTER;
  number KEY_LCOM = VGLK_LCOM;
  number KEY_RCOM = VGLK_RCOM;
  number KEY_MAC = VGLK_MAC;
  number KEY_LMETA = VGLK_LMETA;
  number KEY_RMETA = VGLK_RMETA;

  number KEY_INS = VGLK_INS;
  number KEY_DEL = VGLK_DEL;
  number KEY_HOME = VGLK_HOME;
  number KEY_END = VGLK_END;
  number KEY_PAGEUP = VGLK_PGUP;
  number KEY_PAGEDOWN = VGLK_PGDN;

  number KEY_ESC = VGLK_ESC;

  number KEY_UP = VGLK_UP;
  number KEY_DOWN = VGLK_DOWN;
  number KEY_LEFT = VGLK_LEFT;
  number KEY_RIGHT = VGLK_RIGHT;

  number KEY_SPACE = VGLK_SPACE;

  number KEY_COMMA = VGLK_COMMA;
  number KEY_PERIOD = VGLK_PERIOD;
  number KEY_MINUS = VGLK_MINUS;

  number KEY_GRAVE = VGLK_GRAVE;

  number KEY_0 = VGLK_0;
  number KEY_1 = VGLK_1;
  number KEY_2 = VGLK_2;
  number KEY_3 = VGLK_3;
  number KEY_4 = VGLK_4;
  number KEY_5 = VGLK_5;
  number KEY_6 = VGLK_6;
  number KEY_7 = VGLK_7;
  number KEY_8 = VGLK_8;
  number KEY_9 = VGLK_9;

  number KEY_CAPS = VGLK_CAPS;
  number KEY_SCROLL = VGLK_SCROLL;
  number KEY_NUM = VGLK_NUM;
  number KEY_PRINTSCREEN = VGLK_PRSCR;
  number KEY_PAUSE = VGLK_PAUSE;

  number KEY_A = VGLK_A;
  number KEY_B = VGLK_B;
  number KEY_C = VGLK_C;
  number KEY_D = VGLK_D;
  number KEY_E = VGLK_E;
  number KEY_F = VGLK_F;
  number KEY_G = VGLK_G;
  number KEY_H = VGLK_H;
  number KEY_I = VGLK_I;
  number KEY_J = VGLK_J;
  number KEY_K = VGLK_K;
  number KEY_L = VGLK_L;
  number KEY_M = VGLK_M;
  number KEY_N = VGLK_N;
  number KEY_O = VGLK_O;
  number KEY_P = VGLK_P;
  number KEY_Q = VGLK_Q;
  number KEY_R = VGLK_R;
  number KEY_S = VGLK_S;
  number KEY_T = VGLK_T;
  number KEY_U = VGLK_U;
  number KEY_V = VGLK_V;
  number KEY_W = VGLK_W;
  number KEY_X = VGLK_X;
  number KEY_Y = VGLK_Y;
  number KEY_Z = VGLK_Z;

  number KEYPAD_SLASH = VGLK_KP_SLASH;
  number KEYPAD_MUL = VGLK_KP_MUL;
  number KEYPAD_MINUS = VGLK_KP_MINUS;
  number KEYPAD_PLUS = VGLK_KP_PLUS;
  number KEYPAD_ENTER = VGLK_KP_ENTER;
  number KEYPAD_PERIOD = VGLK_KP_PERIOD;
  number KEYPAD_DEL = VGLK_KP_DEL;
  number KEYPAD_0 = VGLK_KP_0;
  number KEYPAD_INS = VGLK_KP_INS;
  number KEYPAD_1 = VGLK_KP_1;
  number KEYPAD_END = VGLK_KP_END;
  number KEYPAD_2 = VGLK_KP_2;
  number KEYPAD_DOWN = VGLK_KP_DOWN;
  number KEYPAD_3 = VGLK_KP_3;
  number KEYPAD_PAGEDOWN = VGLK_KP_PGDN;
  number KEYPAD_4 = VGLK_KP_4;
  number KEYPAD_LEFT = VGLK_KP_LEFT;
  number KEYPAD_5 = VGLK_KP_5;
  number KEYPAD_6 = VGLK_KP_6;
  number KEYPAD_RIGHT = VGLK_KP_RIGHT;
  number KEYPAD_7 = VGLK_KP_7;
  number KEYPAD_HOME = VGLK_KP_HOME;
  number KEYPAD_8 = VGLK_KP_8;
  number KEYPAD_UP = VGLK_KP_UP;
  number KEYPAD_9 = VGLK_KP_9;
  number KEYPAD_PGAGEUP = VGLK_KP_PGUP;

  number KEY_F1 = VGLK_F1;
  number KEY_F2 = VGLK_F2;
  number KEY_F3 = VGLK_F3;
  number KEY_F4 = VGLK_F4;
  number KEY_F5 = VGLK_F5;
  number KEY_F6 = VGLK_F6;
  number KEY_F7 = VGLK_F7;
  number KEY_F8 = VGLK_F8;
  number KEY_F9 = VGLK_F9;
  number KEY_F10 = VGLK_F10;
  number KEY_F11 = VGLK_F11;
  number KEY_F12 = VGLK_F12;

  number KEY_EQUAL = VGLK_EQUAL;
  number KEY_LSQBRK = VGLK_LSQBRK;
  number KEY_RSQBRK = VGLK_RSQBRK;
  number KEY_SEMICL = VGLK_SEMICL;
  number KEY_APOSTR = VGLK_APOSTR;
  number KEY_BACKSL = VGLK_BACKSL;
  number KEY_SLASH = VGLK_SLASH;

  number KEY_DE_SS = VGLK_DE_SS;
  number KEY_DE_ACCENT = VGLK_DE_ACCENT;
  number KEY_DE_UE  = VGLK_DE_UE;
  number KEY_DE_PLUS = VGLK_DE_PLUS;
  number KEY_DE_OE = VGLK_DE_OE;
  number KEY_DE_AE = VGLK_DE_AE;
  number KEY_DE_HASH = VGLK_DE_HASH;
  number KEY_DE_LT  = VGLK_DE_LT;
  number KEY_DE_CIRC = VGLK_DE_CIRC;

  number KEY_FR_UGRAVE = VGLK_FR_UGRAVE;
  number KEY_FR_MULTIPLY = VGLK_FR_MULTIPLY;
  number KEY_FR_COMMA = VGLK_FR_COMMA;
  number KEY_FR_SEMICL = VGLK_FR_SEMICL;
  number KEY_FR_COLON = VGLK_FR_COLON;
  number KEY_FR_SS = VGLK_FR_SS;
  number KEY_FR_RBRACKET = VGLK_FR_RBRACKET;
  number KEY_FR_EQUAL = VGLK_FR_EQUAL;
  number KEY_FR_DOLL = VGLK_FR_DOLL;
  
  number BUTTON_LEFT = 0;
  number BUTTON_MIDDLE = 1;
  number BUTTON_RIGHT = 2;

  /// @}
  /// @name Light Source Types
  /// @{
  
  number LIGHT_POINT    = VIS_LIGHT_POINT;
  number LIGHT_DIRECTED = VIS_LIGHT_DIRECTED;
  number LIGHT_SPOT     = VIS_LIGHT_SPOTLIGHT;

  /// @}
  /// @name Corona Scaling
  /// @{
  
  number CORONA_NO_SCALE       = VIS_CORONASCALE_NONE;
  number CORONA_DISTANCE_SCALE = VIS_CORONASCALE_DISTANCE;
  number CORONA_AREA_SCALE     = VIS_CORONASCALE_VISIBLEAREA;
  number CORONA_FADEOUT_SCALE  = VIS_CORONASCALE_USEFADEOUT;
  number CORONA_ROTATING_SCALE = VIS_CORONASCALE_ROTATING;

  /// @}
  /// @name Trace Accuracy
  /// @{

  number TRACE_IGNORE       = VIS_TRACEACC_NOCOLLISION;
  number TRACE_AABOX        = VIS_TRACEACC_AABOX;
  number TRACE_OBOX         = VIS_TRACEACC_OBOX;
  number TRACE_SPHERE       = VIS_TRACEACC_SPHERE;
  number TRACE_POLYGON      = VIS_TRACEACC_POLYGONS;

  /// @}
  /// @name Editor Modes
  /// @{
  
  number EDITOR_NONE        = VisEditorManager_cl::EDITORMODE_NONE;
  number EDITOR_ANIMATING   = VisEditorManager_cl::EDITORMODE_ANIMATING;
  number EDITOR_RUN         = VisEditorManager_cl::EDITORMODE_PLAYING_IN_EDITOR;
  number EDITOR_PLAY        = VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME;
  
  /// @}
  /// @name Surface Texture Types
  /// @{
  
  number TEXTURE_TYPE_DIFFUSE    = VisSurfaceTextures_cl::VTT_Diffuse;
  number TEXTURE_TYPE_NORMAL_MAP = VisSurfaceTextures_cl::VTT_NormalMap;
  number TEXTURE_TYPE_SPEC_MAP   = VisSurfaceTextures_cl::VTT_SpecularMap;
    
  number BLEND_OPAQUE           = VIS_TRANSP_NONE;
  number BLEND_MULTIPLY         = VIS_TRANSP_MULTIPLICATIVE;
  number BLEND_ADDITIVE         = VIS_TRANSP_ADDITIVE;
  number BLEND_ADDITIVE_NOALPHA = VIS_TRANSP_ADDITIVE_NOALPHA;
  number BLEND_ALPHA            = VIS_TRANSP_ALPHA;
  number BLEND_ALPHA_TEST       = VIS_TRANSP_ALPHATEST;
  number BLEND_ADD_MODULATE     = VIS_TRANSP_ADD_MODULATE;
    
  /// @}
  /// @name Math Constants
  /// @{ 
    
  number MATH_SMALL_EPSILON     = HKVMATH_SMALL_EPSILON;
  number MATH_DEFAULT_EPSILON   = HKVMATH_DEFAULT_EPSILON;
  number MATH_LARGE_EPSILON     = HKVMATH_LARGE_EPSILON;
  number MATH_HUGE_EPSILON      = HKVMATH_HUGE_EPSILON;
  number MATH_GIGANTIC_EPSILON  = HKVMATH_GIGANTIC_EPSILON;
    
  /// @}
};

#endif

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
