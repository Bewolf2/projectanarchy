/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiDefs.hpp

#ifndef DEFINE_VISAPIDEFS
#define DEFINE_VISAPIDEFS

#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <Vision/Runtime/Base/Math/BoundingVolume/hkvBoundingSphere.h>

// CLASS AND STRUCTURE DECLARATIONS
class VisSurface_cl;
class VisBaseEntity_cl;
class EntityData_cl;
class VisLightSource_cl;
class VisConvexVolume_cl;
struct VisParticleCustomData_t;
struct VisParticleCustomInfo_t;
struct VisPrimitiveProperty_t;
struct ModelVertex_t;
struct ModelPolygon_t;

typedef signed short int      SWORD;  ///< 16 bit signed.
typedef unsigned short int    UWORD;  ///< 16 bit unsigned.
typedef signed char           SBYTE;  ///< 8 bit signed.
typedef unsigned char         UBYTE;  ///< 8 bit unsigned.

// Do we perform immediate context switches?
#ifdef _VR_OPENGL
  #define IMMEDIATE_CONTEXT_SWITCH
#endif


#ifdef _VR_GCM
#define IMMEDIATE_CONTEXT_SWITCH
#endif


#if defined(_VR_DX11)
  #define IMMEDIATE_CONTEXT_SWITCH
#endif

#if defined(_VR_DX9) && defined(WIN32)
  #define IMMEDIATE_CONTEXT_SWITCH
#endif

//Renderstate flags
#define VIS_API_SORTING_RENDERSTATE         1           ///< items are sorted by renderstates
#define VIS_API_SORTING_DISTANCE            2           ///< items are sorted by distance (closest object first)
#define VIS_API_SORTING_PROXIMITY           4           ///< items are sorted by proximity (closest object last)
#define VIS_API_SORTING_OBJECTKEY           8           ///< items are sorted by object priority
#define VIS_API_SORTING_POLYGONCOUNT        16          ///< items are sorted by poly count
#define VIS_API_SORTING_VERTEXBUFFER        32          ///< items are sorted by vertex buffer (static geometry instances only)

//Load Scene Status callback flags
#define VIS_API_LOADMODELSTATUS_START       0x00000008           ///< always called at beginning
#define VIS_API_LOADMODELSTATUS_PROGRESS    0x00000010           ///< called in between
#define VIS_API_LOADMODELSTATUS_FINISHED    0x00000020           ///< the last call of callback

#define VIS_API_LOADSCENESTATUS_START       0x00000100           ///< called when scene loading starts
#define VIS_API_LOADSCENESTATUS_PROGRESS    0x00000200           ///< called while loading a scene
#define VIS_API_LOADSCENESTATUS_FINISHED    0x00000400           ///< called when scene loading finished


// Occlusion Query Settings 

#define HWOCCTEST_CUSTOM_GEOMETRY               0
#define HWOCCTEST_BOUNDINGBOX                   1
#define HWOCCTEST_BILLBOARD                     2

#define MIN_HWOCCTEST_VISIBLE_PIXELS            5
#define MIN_HWOCCTEST_OBJECT_POLYCOUNT         50
#define HWOCCTEST_INVALID_INDEX             65535

#if defined(_VR_DX9) && defined(WIN32)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES 16384
#elif defined(_VR_DX11)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES 16384
#elif defined(_VISION_XENON)
  // 0: Untiled, 1-15: number of tiles when using predicated tiling.
  #define HWOCCTEST_NUM_QUERY_QUEUES 16
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES 16384
#elif defined(_VISION_PS3)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES  8192 ///< 1Mb buffer for queries with 128 bytes per query = 8192 queries at max
#elif defined(_VR_GLES2)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES  0
#elif defined(_VISION_PSP2)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES  16384 ///< hardware limit
#elif defined(_VISION_WIIU)
  #define HWOCCTEST_NUM_QUERY_QUEUES 1
  #define MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES 2048
#else
  #error "Missing Platform"
#endif

#define VIS_MAX_IM_INDEXES 8192

// Render Context Priority Default Values
#define VIS_RENDERCONTEXTPRIORITY_DISPLAY                     100000000.0f
#define VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR_RESOLVED      20000000.0f
#define VIS_RENDERCONTEXTPRIORITY_POSTPROCESSOR               10000000.0f
#define VIS_RENDERCONTEXTPRIORITY_SCENE                       1000000.0f
#define VIS_RENDERCONTEXTPRIORITY_MIRROR                      100000.0f
#define VIS_RENDERCONTEXTPRIORITY_SHADOWS                     10000.0f


/// \brief Render Context Flags
enum VRenderContextFlags
{
  VIS_RENDERCONTEXT_FLAG_NONE                  = 0,
  VIS_RENDERCONTEXT_FLAG_NO_WORLDGEOM          = V_BIT(0),    ///< If set, no static geometry instances are rendered.

  VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS        = V_BIT(2),    ///< If set, coronas and are rendered.
  VIS_RENDERCONTEXT_FLAG_REVERSE_CULLMODE      = V_BIT(3),    ///< If set, reverse cull-mode is used (e.g. for mirrors and other cases where the coordinate system is flipped).
  VIS_RENDERCONTEXT_FLAG_NO_CLEARSCREEN        = V_BIT(4),    ///< The render-loop should not call ClearScreen at the beginning. This flag can be useful when layering contexts without changing the render-loop. This flag is considered by the default render-loop and affects clearing status for all buffers (color, stencil,..).
  VIS_RENDERCONTEXT_FLAG_RENDER_LENSFLARES     = V_BIT(5),    ///< If set, lens flares are rendered.

  VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER      = V_BIT(9),    ///< If set, hardware occlusion queries are used in this context for pixel count queries (e.g. corona / lens flare visibility). This flag only makes sense for contexts that survive more than one frame.
  VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY    = V_BIT(10),   ///< If set, hardware occlusion queries are used in this context for visibility computations. This flag only makes sense for contexts that survive more than one frame.

  VIS_RENDERCONTEXT_FLAG_NO_LOD_SCALING        = V_BIT(12),   ///< If set, then the LOD correction factor (from FOV) is not considered for near and far clipping of geometry.
  VIS_RENDERCONTEXT_FLAG_SHOW_DEBUGOUTPUT      = V_BIT(13),
  VIS_RENDERCONTEXT_FLAG_NO_ENTITIES           = V_BIT(14),   ///< If set, no entities are rendered.
  VIS_RENDERCONTEXT_FLAG_NO_LIGHTS             = V_BIT(15),   ///< Set this flag if you don't need feedback about which lights are visible in this context.
  VIS_RENDERCONTEXT_FLAG_NO_VISOBJECTS         = V_BIT(16),   ///< If set, no visibility objects are rendered.
  VIS_RENDERCONTEXT_FLAG_NO_FARCLIP            = V_BIT(17),   ///< If set, this context does not perform far clip tests for entities and visibility objects, i.e. it ignores the distances that are set via VisBaseEntity_cl::SetFarClipDistance resp. VisVisibilityObject_cl::SetFarClipDistance. Useful for shadow contexts that render the scene from far away.
  VIS_RENDERCONTEXT_FLAG_NO_ENTITYPREPARATION  = V_BIT(18),
  VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT           = V_BIT(19),   ///< This flag marks the context to be a view context, e.g. a context that renders all scene elements. The main context (and all secondary contexts) are typically view contexts, but helper contexts such as shadow contexts are not view contexts. OnContextSwitching listeners might check this flag to determine whether their own contexts have to be rendered before this one.
  VIS_RENDERCONTEXT_FLAG_NOMIRRORS             = V_BIT(20),

  /// \brief Set this flag if you don't plan to render any scene elements (e.g. for post-processing effects)
  VIS_RENDERCONTEXT_FLAG_NO_SCENEELEMENTS      = (VIS_RENDERCONTEXT_FLAG_NO_WORLDGEOM|VIS_RENDERCONTEXT_FLAG_NO_ENTITIES|VIS_RENDERCONTEXT_FLAG_NO_VISOBJECTS|VIS_RENDERCONTEXT_FLAG_NOMIRRORS),

  /// \brief A combination of flags useful for primary contexts. It basically enables all flags except for VIS_RENDERCONTEXT_FLAG_NO_WORLDGEOM and VIS_RENDERCONTEXT_FLAG_REVERSE_CULLMODE.
  VIS_RENDERCONTEXT_FLAGS_ALLFEATURES          = (VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT|VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS|VIS_RENDERCONTEXT_FLAG_RENDER_LENSFLARES|VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER|VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY|VIS_RENDERCONTEXT_FLAG_SHOW_DEBUGOUTPUT),

  /// \brief A combination of flags useful for secondary contexts. It doesn't perform LOD changes.
  VIS_RENDERCONTEXT_FLAGS_SECONDARYCONTEXT     = (VIS_RENDERCONTEXT_FLAG_VIEWCONTEXT|VIS_RENDERCONTEXT_FLAG_RENDER_CORONAS|VIS_RENDERCONTEXT_FLAG_USE_PIXELCOUNTER|VIS_RENDERCONTEXT_FLAG_USE_OCCLUSIONQUERY),

  /// \brief This flag has no use at the moment (everything is rendered to a texture anyway)
  VIS_RENDERCONTEXT_FLAGS_RENDERTOTEXTURE      = VIS_RENDERCONTEXT_FLAG_NONE
};

// default context bit mask for shader rendering
#define VIS_RENDERSHADER_ALL_SCENE      0xffff0001
#define VIS_RENDERSHADER_ALL_TEXTURE    0xffff0002

// Entity visibility default bit masks (VisBaseEntity_cl::SetVisibleBitmask)
#define VIS_ENTITY_INVISIBLE            0x00000000
#define VIS_ENTITY_VISIBLE_IN_WORLD     0x00000001
#define VIS_ENTITY_VISIBLE_IN_TEXTURE   0x00000002
#define VIS_ENTITY_VISIBLE_IN_CUBEMAP   0x00000004
#define VIS_ENTITY_VISIBLE_IN_MIRROR    0x00000008
#define VIS_ENTITY_VISIBLE              0xffffffff


// Currently reserved memory dumping flags
#define VIS_DUMPMEM_SORTBYFILE          1
#define VIS_DUMPMEM_SORTBYSIZE          2
#define VIS_DUMPMEM_VISMESSAGE          4

// Memory Usage Flags
#define VIS_MEMUSAGE_STATIC               0
#define VIS_MEMUSAGE_DYNAMIC              1
#define VIS_MEMUSAGE_STREAM               2
#define VIS_MEMUSAGE_STATIC_MANAGED       0
#define VIS_MEMUSAGE_STATIC_UNMANAGED     3
#define VIS_MEMUSAGE_FORCEMAINMEMORY      4           ///< PS3 only
#define VIS_MEMUSAGE_FORCELOCALMEMORY     5           ///< PS3 only
#define VIS_MEMUSAGE_UAV_BYTEADDRESS     0x40  ///< DX11 only
#define VIS_MEMUSAGE_UAV_STRUCTURED      0x80  ///< DX11 only (VB, IB, & RB only)
#define VIS_MEMUSAGE_UAV_DRAWINDIRECT    0x100 ///< DX11 only (VB, IB, & RB only)
#define VIS_MEMUSAGE_UAV_APPEND          0x200 ///< DX11 only (VB, IB, & RB only)
#define VIS_MEMUSAGE_UAV_COUNT           0x400 ///< DX11 only (VB, IB, & RB only)
#define VIS_MEMUSAGE_USAGE_BITS           0xF

// Resource Bind Flags
#define VIS_BIND_INDEX_BUFFER         V_BIT( 0 )
#define VIS_BIND_VERTEX_BUFFER        V_BIT( 1 )
#define VIS_BIND_SHADER_RESOURCE      V_BIT( 2 )  ///< DX11 only
#define VIS_BIND_UNORDERED_ACCESS     V_BIT( 3 )  ///< DX11 only
#define VIS_BIND_STREAM_OUTPUT        V_BIT( 4 )  ///< DX11 only
//#define VIS_BIND_CONSTANT_BUFFER      V_BIT( 5 )


// \brief
//   Index Buffer Formats
enum VIndexFormat
{
    VIS_INDEXFORMAT_32 = 32,
    VIS_INDEXFORMAT_16 = 16,
    VIS_INDEXFORMAT_INVALID = 0
};


/// \brief
///   Constants for defining when a render hook should be triggered
enum VRenderHook_e {
  VRH_INVALID                                                              = 0x00000000,
  VRH_PRE_RENDERING                                                        = 0x00000001,
  VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY                                     = 0x00000002,
  VRH_PRE_PRIMARY_OPAQUE_PASS_ENTITIES                                     = 0x00000004,
  VRH_PRE_SECONDARY_OPAQUE_PASS_GEOMETRY                                   = 0x00000008,
  VRH_PRE_SECONDARY_OPAQUE_PASS_ENTITIES                                   = 0x00000010,
  VRH_PRE_OCCLUSION_TESTS                                                  = 0x00000020,
  VRH_POST_OCCLUSION_TESTS                                                 = 0x00000040,
  VRH_PRE_TRANSPARENT_PASS_GEOMETRY                                        = 0x00000080,
  VRH_PRE_TRANSPARENT_PASS_ENTITIES                                        = 0x00000100,
  VRH_POST_TRANSPARENT_PASS_GEOMETRY                                       = 0x00000200,
  VRH_DECALS                                                               = 0x00000400,
  VRH_PARTICLES                                                            = 0x00000800,
  VRH_ADDITIVE_PARTICLES                                                   = 0x00001000,
  VRH_TRANSLUCENT_VOLUMES                                                  = 0x00002000,
  VRH_CORONAS_AND_FLARES                                                   = 0x00004000,
  VRH_PRE_SCREENMASKS                                                      = 0x00008000,
  VRH_GUI                                                                  = 0x00010000,
  VRH_AFTER_RENDERING                                                      = 0x00020000,
  VRH_CUSTOM                                                               = 0x00040000,
};

// internal flags passed to the VisMeshBufferObject_cl::OnRender function
#define VIS_MBONRENDERFLAG_NONE           0x00000000
#define VIS_MBONRENDERFLAG_NOSTATESETUP   0x00010000
#define VIS_MBONRENDERFLAG_NOSTREAMSETUP  0x00020000

// flags for visibility testing
#define VIS_LOD_TEST_NONE                 0
#define VIS_LOD_TEST_CLIPPOSITION         32
#define VIS_LOD_TEST_BOUNDINGBOX          64
#define VIS_LOD_TEST_APPLYLODSCALING      128

#define VIS_HAS_NO_MODEL                  1
#define VIS_EXCLUDED_FROM_VISTEST         2
#define VIS_IS_INACTIVE                   4
#define VIS_PERFORM_LODTEST_DEPRECATED    8
#define VIS_PERFORM_LODTEST               (VIS_LOD_TEST_CLIPPOSITION|VIS_LOD_TEST_BOUNDINGBOX|VIS_LOD_TEST_APPLYLODSCALING)
#define VIS_EXCLUDED_FROM_OCCLUSIONQUERY  16


//#ifdef HK_DEBUG_SLOW
#if 1  
  #define VULPERROR_0(expression,message)             if (!(expression)) Vision::Error.FatalError(message);
  #define VULPERROR_1(expression,message,p1)          if (!(expression)) Vision::Error.FatalError(message, p1);
  #define VULPERROR_2(expression,message,p1, p2)      if (!(expression)) Vision::Error.FatalError(message, p1, p2);
  #define VULPAPIWARNING_0(expression,message)        if (!(expression)) Vision::Error.Warning(message);
  #define VULPAPIWARNING_1(expression,message,p1)     if (!(expression)) Vision::Error.Warning(message, p1);
  #define VULPAPIWARNING_2(expression,message,p1,p2)  if (!(expression)) Vision::Error.Warning(message, p1, p2);
  #define VULPAPIWARNING_3(expression,message,p1,p2,p3)  if (!(expression)) Vision::Error.Warning(message, p1, p2, p3);
  #define VULPCRITICALWARNING_0(expression, message)  if (!(expression)) Vision::Error.Warning(message);
  #define VULPSTATUSMESSAGE_0(message)                Vision::Error.SystemMessage(message);
  #define VULPSTATUSMESSAGE_1(message,p1)             Vision::Error.SystemMessage(message, p1);
  #define VULPSTATUSMESSAGE_2(message,p1,p2)          Vision::Error.SystemMessage(message, p1, p2);
  #define VULPSTATUSMESSAGE_3(message,p1,p2,p3)       Vision::Error.SystemMessage(message, p1, p2, p3);
  #define VULPSTATUSMESSAGE_4(message,p1,p2,p3,p4)    Vision::Error.SystemMessage(message, p1, p2, p3, p4);
  #define VULPSTATUSMESSAGE_5(message,p1,p2,p3,p4,p5) Vision::Error.SystemMessage(message, p1, p2, p3, p4, p5);
  #define VULPDEPRECATED(message)                     Vision::Error.Warning(message);

#else
  #define VULPERROR_0(expression,message)                 ;    
  #define VULPERROR_1(expression,message,p1)              ;
  #define VULPERROR_2(expression,message,p1, p2)          ;
  #define VULPAPIWARNING_0(expression,message)            ;
  #define VULPAPIWARNING_1(expression,message,p1)         ;
  #define VULPAPIWARNING_2(expression,message,p1,p2)      ;
  #define VULPCRITICALWARNING_0(expression, message)      ;
  #define VULPSTATUSMESSAGE_0(message)                    ;
  #define VULPSTATUSMESSAGE_1(message,p1)                 ;
  #define VULPSTATUSMESSAGE_2(message,p1,p2)              ;
  #define VULPSTATUSMESSAGE_3(message,p1,p2,p3)           ;
  #define VULPSTATUSMESSAGE_4(message,p1,p2,p3,p4)        ;
  #define VULPSTATUSMESSAGE_5(message,p1,p2,p3,p4,p5)     ;

  #define VULPCALLHISTORY_START(number)                   ;
  #define VULPCALLHISTORY_END(number)                     ;

#endif

// in debug mode, activate profiling
#ifdef HK_DEBUG_SLOW
  #ifndef PROFILING
    #define PROFILING
  #endif
#endif


#ifdef PROFILING

  #define VISION_START_PROFILING(elementID)               Vision::Profiling.StartElementProfiling(elementID);
  #define VISION_STOP_PROFILING(elementID)                Vision::Profiling.StopElementProfiling(elementID);

  #define VISION_PROFILE_FUNCTION__(elementID,line) \
      struct VisProfileFunctionHelper_t##line \
      { \
        VisProfileFunctionHelper_t##line(int iElementID) { m_iID = iElementID; VISION_START_PROFILING(m_iID); } \
        ~VisProfileFunctionHelper_t##line( ) { VISION_STOP_PROFILING(m_iID); } \
        int m_iID; \
      } profileFunctionHelper##line(elementID);

  #define VISION_PROFILE_FUNCTION_(elementID,line) VISION_PROFILE_FUNCTION__(elementID, line)

  /// \brief
  ///   Helper class for profiling a function. If a macro is put somewhere into the code, the scope of the macro position will be profiled (e.g. inside curly brackets)
  #define VISION_PROFILE_FUNCTION(elementID) VISION_PROFILE_FUNCTION_(elementID, __LINE__)

#else
  
  /// \brief
  ///   Start to profile the code and use the given element to save the current timing The element
  ///   IDs 0 to 2047 are reserved, you have to use higher values for your own profiling elements.
  #define VISION_START_PROFILING(elementID)

  /// \brief
  ///   Stop to profile the code and store the results in the given element
  #define VISION_STOP_PROFILING(elementID)
  
  #define VISION_PROFILE_FUNCTION(elementID) 

#endif

#ifdef WIN32

  #if defined(VISIONDLL_LIB) || defined(_VISION_WINRT)
    #define VISION_APIFUNC __declspec()
    #define VISION_APIDATA __declspec()
  #endif

  #ifdef VISIONDLL_EXPORTS
    #define VISION_APIFUNC __declspec(dllexport)
    #define VISION_APIDATA __declspec(dllexport)
  #endif

  #ifndef VISION_APIFUNC
    #define VISION_APIFUNC
    #define VISION_APIDATA __declspec(dllimport)
  #endif

#elif defined(_VISION_XENON) 
  #define VISION_APIFUNC __declspec()
  #define VISION_APIDATA __declspec()

#elif defined(_VISION_PS3) || defined(_VISION_IOS) || defined(_VISION_ANDROID) ||  defined(_VISION_PSP2) || defined(_VISION_WIIU) || defined(_VISION_WINRT)
  #define VISION_APIFUNC
  #define VISION_APIDATA

#else
  #error "Missing Platform!"
#endif

// message IDs for the virtual VisTypedEngineObject_cl::MessageFunction
const unsigned int VIS_MSG_INTERNAL = 0xFFFF,                             ///< internal messages start at 0xFFFF
                   VIS_MSG_EVENT    = VIS_MSG_INTERNAL + 1,               ///< event caused by animation system : iParamA contains the event id iParamB the VisAnimControl_cl
                   VIS_MSG_TRIGGER  = VIS_MSG_INTERNAL + 2,               ///< Called by trigger components, see VisTriggerTargetComponent_cl
                   VIS_MSG_DATABLOCK = VIS_MSG_INTERNAL + 3,              ///< Network message that can be used to send data blocks. iParamA is a pointer to a VDataBlockMessageInfo struct and iParamB is not used.
                   VIS_MSG_ACTIVATE = VIS_MSG_INTERNAL + 4,               ///< Reserved for activation messages. iParamA is a (0/1) flag that defines the activation status and iParamB can be casted to a string that defines the name of the activator
                   VIS_MSG_VISIBILITY = VIS_MSG_INTERNAL + 5,             ///< Sent to components when the owner object changed its visibility through SetVisibleBitmask. iParamA is the new 32bit value as retrieved via GetVisibleBitmask. 
                   VIS_MSG_RESOURCE_BEFORE_FILEMODIFIED = VIS_MSG_INTERNAL + 10,  ///< Sent before a resource has been reloaded because its file time stamp has been modified. The resource pointer is passed as iParamA and the manager passed as iParamB. Called by VisResourceSystem_cl::ReloadModifiedResourceFiles
                   VIS_MSG_RESOURCE_AFTER_FILEMODIFIED = VIS_MSG_INTERNAL + 11,   ///< Sent after a resource has been reloaded because its file time stamp has been modified. The resource pointer is passed as iParamA and the manager passed as iParamB. Called by VisResourceSystem_cl::ReloadModifiedResourceFiles
                   VIS_MSG_REQUEST_IR_PARAMS = VIS_MSG_INTERNAL + 12,     ///< Used by external renderers to retrieve material params from IR renderer (internal use)
                   VIS_MSG_REPLAY_EVENT = VIS_MSG_INTERNAL + 13,          ///< Event used by record and replay implementations. iParamA and iParamB are implementation specific
                   VIS_MSG_USER = 0,                                      ///< user messages have the range 0 - 9999
                   VIS_MSG_LASTUSER = 9999,                               ///< last user message
                   VIS_MSG_PHYSICS = 10000,                               ///< physics module messages have the range 10000 - 10999
                   VIS_MSG_LASTPHYSICS = 10999,                           ///< last physics module message
                   VIS_MSG_EDITOR = 11000,                                ///< editor messages have the range 11000 - 11999
                   VIS_MSG_EDITOR_PROPERTYCHANGED = VIS_MSG_EDITOR + 1,   ///< entity property has been changed by the editor: iParamA contains the property name, iParamB the property value (always as a string)
                   VIS_MSG_EDITOR_PROPERTYCHANGING = VIS_MSG_EDITOR + 2,  ///< Some controls send this message while the value is changing, e.g. a slider control
                   VIS_MSG_EDITOR_GETLINKS  = VIS_MSG_EDITOR + 3,         ///< Called by vForge to gather information about custom link types. iParamA can be casted to (VShapeLinkConfig *) which can receive information about an arbitrary number of shape links/targets
                   VIS_MSG_EDITOR_CANLINK   = VIS_MSG_EDITOR + 4,         ///< Called by vForge to query whether it is possible to link this entity to the passed other VisObject3D_cl. iParamA can be casted to (VShapeLink *) which in turn provides information about the link type and the other object. The entity code should set the VShapeLink::m_bResult member
                   VIS_MSG_EDITOR_ONLINK    = VIS_MSG_EDITOR + 5,         ///< Called by vForge to perform the actual link. iParamA can be casted to (VShapeLink *) which provides information about the other object that should be linked. The entity code is responsible for establishing the link and also saving link information to archives at export time.
                   VIS_MSG_EDITOR_ONUNLINK  = VIS_MSG_EDITOR + 6,         ///< Called by vForge to un-link another object. See VIS_MSG_EDITOR_ONLINK
                   VIS_MSG_TRANSITIONSTATEMACHINE = VIS_MSG_EDITOR + 7,   ///< Used by the transition state machine to propagate its events to external listeners. iParamA contains the event id iParamB the VTransitionStateMachine
                   VIS_MSG_EDITOR_CLONESOURCE = VIS_MSG_EDITOR + 8,       ///< Called by vForge to indicate that this entity is a clone of another entity. The original source entity pointer is passed in iParamA. That pointer is NULL if this entity is the original (this message is called anyway)
                   VIS_MSG_EDITOR_GETSTANDARDVALUES = VIS_MSG_EDITOR + 9, ///< Called by vForge to retrieve standard values for a dropdown. iParamA can be casted to const char* which defines the key for the dropdown. iParamB can be casted to (VStrList *) which can be filled with values for the dropdown.
                   VIS_MSG_EDITOR_TRACE_SHAPE	= VIS_MSG_EDITOR + 10,      ///< Called by vForge to propagate shape picking to components set on an entity.
                   VIS_MSG_LASTEDITOR = 11999,                            ///< last editor message
                   VIS_MSG_FIRST3RDPARTY_1 = 0x00021000,                  ///< Reserved for 3rd party messages (company #1)
                   VIS_MSG_FIRST3RDPARTY_2 = 0x00022000,                  ///< Reserved for 3rd party messages (company #2)
                   VIS_MSG_FIRST3RDPARTY_3 = 0x00023000,                  ///< Reserved for 3rd party messages (company #3)
                   VIS_MSG_FIRST3RDPARTY_4 = 0x00024000;                  ///< Reserved for 3rd party messages (company #4)


// entity status flags
const int 
          VIS_STATUS_SKELETALANIMATION_CHANGED = 0x8000,                 ///< entity animation changed in this frame
          VIS_STATUS_ANIMATION_CHANGED         = 0x10000 + 0x8000,       ///< entity skeletal or muscle animation changed in this frame
          VIS_STATUS_DISABLED_THINKFUNCTION    = 0x40000,                ///< thinkfunction is disabled, won't be called anymore
          VIS_STATUS_DISABLED_ANY_SUBMESH      = 0x80000,                ///< the entity has any submesh that has been disabled via SetSubmeshVisibleState
          VIS_STATUS_FORCE_COLMESH_UPDATE      = 0x800000,               ///< if set then the collision mesh needs force to be updated on the next request
          VIS_STATUS_DISABLED_PRETHINKFUNCTION = 0x1000000,              ///< prethinkfunction is disabled, won't be called anymore
          VIS_STATUS_ALL                       = 0xFFFFFFFF;



const int VIS_CFG_SHADOWS_ON_PATCHES        = 1, 
          VIS_CFG_SHADOWS_ON_WORLDPOLYGONS  = 2, 
          VIS_CFG_SHADOWS_ON_OBJECTS        = 4;

// traceline flags
const int VIS_TRACE_NONE                 = 0,             ///< don't check against scene elements
          VIS_TRACE_ALL                  = 0xFFFFFFFF;    ///< check against all scene elements

const int VIS_TRACE_STORENOTHING         = 0,             ///< don't do anything special
          VIS_TRACE_STOREENTITIES        = 1,             ///< store all the entities which the ray intersects in the collision info table, sorted by distance
          VIS_TRACE_STOREPRIMITIVES      = 2,             ///< store all static geometry which the ray intersects in the collision info table, sorted by distance
          VIS_TRACE_STOREALL             = 0xFFFFFFFF;    ///< store all the static geometry and entities which the ray intersects in the collision info table, sorted by distance


/// \brief Traceline settings flags.
enum VTraceSettings {
          VIS_TRACESETTINGS_NONE                = 0x00000000,  ///< No settings.
          VIS_TRACESETTINGS_IGNORETRANSP        = 0x00000002,  ///< Ignore transparent geometry during tracelines.
          VIS_TRACESETTINGS_IGNOREINVISIBLE     = 0x00000004,  ///< Ignore objects with a visibility bitmask of 0.
          VIS_TRACESETTINGS_COLLISIONBBOX       = 0x00000008,  ///< Use the model collision bounding box instead of the visibility bounding box for entities.
          VIS_TRACESETTINGS_STOREBASEUV         = 0x00000010,  ///< If specified, the base texture uv texture coordinates can be retrieved from the traceline info.
          VIS_TRACESETTINGS_IGNORECOLLISIONFLAG = 0x00000020,  ///< Ignore the collision flag, i.e. colliding and non-colliding geometry is potentially traced.
          VIS_TRACESETTINGS_NONCOLLIDINGONLY    = 0x00000040,  ///< Use *only* polygons flagged as non-colliding (mutally exclusive to VIS_TRACESETTINGS_IGNORECOLLISIONFLAG flag).
          VIS_TRACESETTINGS_STORESURFACEINFO    = 0x00000080,  ///< If specified, the base texture uv texture coordinates can be retrieved from the traceline info.
          VIS_TRACESETTINGS_FORCEOBJ_AABOXACC   = 0x00000100,  ///< Forces to use entity axis aligned bounding box accuracy regardless of individual entity setting.
          VIS_TRACESETTINGS_FORCEOBJ_OBOXACC    = 0x00000200,  ///< Forces to use entity oriented bounding box accuracy regardless of individual entity setting.
          VIS_TRACESETTINGS_FORCEOBJ_POLYGONACC = 0x00000400,  ///< Forces to use entity polygon accuracy regardless of individual entity setting.
};

const VTraceSettings VIS_TRACESETTINGS_DEFAULT = VIS_TRACESETTINGS_STORESURFACEINFO;

/// \brief
///   Controls handling of tagged entries (e.g. static meshes/ entities).
enum VTagFilter_e
{
  VTF_IGNORE_NONE=0,
  VTF_IGNORE_TAGGED_ENTRIES=1
};

/// \brief
///   Sets the skinning mode (used for animation configs)
enum VisSkinningMode_e
{
  VIS_SKINNINGMODE_SOFTWARE = 0,              ///< Software (CPU-based) skinning
  VIS_SKINNINGMODE_HARDWARE = 1,              ///< Hardware skinning
  VIS_SKINNINGMODE_HARDWARE_MEMEXPORT = 2,    ///< Hardware skinning with memory export/stream out (Xbox360 and DX10 only)
  VIS_SKINNINGMODE_HARDWARE_COMPUTE = 2,      ///< Compute shader skinning (DX11 only)
  VIS_SKINNINGMODE_SPU = 2,                   ///< SPU skinning (PS3 only)

  VIS_SKINNINGMODE_INVALID = -1               ///< Invalid skinning mode (used internally to differentiate between cached skinningmodes and uncomputed skinningmodes)
};

/// \brief
///   Sets the scale mode for coronas
enum VisCoronaScaleMode_e
{
  VIS_CORONASCALE_NONE = 0,
  VIS_CORONASCALE_DISTANCE = 1,
  VIS_CORONASCALE_VISIBLEAREA = 2,
  VIS_CORONASCALE_USEFADEOUT = 64,
  VIS_CORONASCALE_ROTATING = 128
};


/// \brief
///   Scene element types for classes derived from VisObject3D_cl.
/// 
/// These types are used to identify scene element types during visibility determination.
/// 
/// Custom VisObject3D_cl types with visibility information should have the element type set to
/// VIS_SCENEELEMENTTYPE_CUSTOM.
enum VisSceneElementType_e
{
  VIS_SCENEELEMENTTYPE_UNKNOWN = 0,
  VIS_SCENEELEMENTTYPE_ENTITY = 1,
  VIS_SCENEELEMENTTYPE_LIGHT = 2,
  VIS_SCENEELEMENTTYPE_VISOBJECT = 3,
  VIS_SCENEELEMENTTYPE_MESHBUFFEROBJECT = 4,
  VIS_SCENEELEMENTTYPE_PARTICLESYSTEM = 5,
  VIS_SCENEELEMENTTYPE_CAMERA = 6,
  VIS_SCENEELEMENTTYPE_CUSTOM = 16
};


/// \brief
///   Enum that determines the projection mode for render contexts, see
///   VisRenderContext_cl::SetProjectionType
enum VisProjectionType_e
{
  VIS_PROJECTIONTYPE_PERSPECTIVE  = 0,             ///< perspective projection matrix
  VIS_PROJECTIONTYPE_ORTHOGRAPHIC = 1,             ///< othographic projection matrix
  VIS_PROJECTIONTYPE_CUSTOM       = 2,             ///< custom projection matrix
  VIS_PROJECTIONTYPE_PERSPECTIVE_AUTOFRUSTUM = 3   ///< custom perspective projection matrix with automatic frustum extraction
};

/// \brief
///   Enum passed to VisRenderContext_cl::SetUsageHint / VisRenderContext_cl::GetUsageHint
enum VisContextUsageHint_e
{
  // context types:
  VIS_CONTEXTUSAGE_NONE = 0,            ///< used for temporary contexts
  VIS_CONTEXTUSAGE_VIEWCONTEXT = 1,     ///< render the scene
  VIS_CONTEXTUSAGE_DEPTHSHADOW = 2,     ///< used for shadow contexts
  VIS_CONTEXTUSAGE_DEPTHFILL = 3,       ///< used for depth fill contexts
  VIS_CONTEXTUSAGE_MIRROR = 4,          ///< used for rendering mirror reflections
  
  VIS_CONTEXTUSAGE_TYPEENUMMASK = 255,  ///< masks out the different types (view/depth/mirror...)

  // additional hint bits:
  VIS_CONTEXTUSAGEFLAG_DEFERRED           = V_BIT(10),  ///< bit flag hint that can be combined with VIS_CONTEXTUSAGE_VIEWCONTEXT
  VIS_CONTEXTUSAGEFLAG_DEPTHSTENCILONLY   = V_BIT(11),  ///< bit flag hint indicating that this context will only use a depth stencil target, but no color render target
  VIS_CONTEXTUSAGEFLAG_DEFERRED_MSAA      = V_BIT(12),  ///< bit flag hint that can be combined with VIS_CONTEXTUSAGEFLAG_DEFERRED to indicate that MSAA is used (DX11 only)
  VIS_CONTEXTUSAGEFLAG_INFRARED           = V_BIT(13),  ///< bit flag hint that can be combined with VIS_CONTEXTUSAGE_VIEWCONTEXT
};


/// \brief
///   Structure holding the different available collision mesh types which can be queried through
///   the VisPhysics_cl class
enum VisColMeshType_e
{
  VIS_COLMESHTYPE_INVALID = 0,              ///< invalid collision mesh
  VIS_COLMESHTYPE_POLYGON = 1,              ///< polygonal collision mesh (single polygons)
  VIS_COLMESHTYPE_TRIANGLEARRAY = 2,        ///< triangle array collision mesh (bezier patches, heightfields, triangle lists
  VIS_COLMESHTYPE_INDEXED_TRIANGLEARRAY = 3 ///< indexed triangle array collision mesh. Currently only used for entities
};

/// \brief
///   Enumeration for static geometry types. Each static geometry instance can be of one of these
///   types.
enum VisStaticGeometryType_e
{
  STATIC_GEOMETRY_TYPE_MESHINSTANCE = 1,      ///< Geometry is a VisStaticSubmeshInstance_cl
  STATIC_GEOMETRY_TYPE_TERRAIN = 2,           ///< Geometry is a Vision terrain sector
  STATIC_GEOMETRY_TYPE_FIRSTCUSTOM = 3        ///< Geometry has a custom type
};


/// \brief
///   Defines which profiling method to use (either the CPU time stamp counter or
///   QueryPerformanceCounter)
enum VisProfilingMethod_e
{
  VIS_PROFILINGMETHOD_RDTSC = 0,
  VIS_PROFILINGMETHOD_QPC = 1,

#ifdef _VISION_PSP2
  VIS_PROFILINGMETHOD_PROCESSTIME = 0,
  VIS_PROFILINGMETHOD_PERFLIB = 1,
#endif

  VIS_PROFILINGMETHOD_DISABLED = 0xFFFFFFFF
};

/// \brief
///   "Welding" is the term used to address the problem of objects bouncing as new contact points are created while transitioning from a collision
///   with one shape to a collision with a neighboring shape. 
enum VisWeldingType_e
{
  VIS_WELDING_TYPE_NONE = 0,
  VIS_WELDING_TYPE_ANTICLOCKWISE = 1,
  VIS_WELDING_TYPE_CLOCKWISE = 2,
  VIS_WELDING_TYPE_TWO_SIDED = 3,
};

/// \brief
///   This enumeration is both used for the traceline accuracy variable and the corona block
///   accuracy variable of entities Note: This enum does always contain the same (integer-)values
///   as VisCollisionAccuracy_e plus an additional default value
enum VisTraceAccuracy_e
{
  VIS_TRACEACC_NOCOLLISION = 0,     ///< ignore entity during traceline
  VIS_TRACEACC_AABOX = 1,           ///< use axis aligned bounding boxes for a traceline/corona check against the entity
  VIS_TRACEACC_OBOX = 2,            ///< use orientied bounding boxes for a traceline/corona check against the entity
  VIS_TRACEACC_SPHERE = 3,          ///< use a sphere for tracing. The radius and center of the trace bounding box will be used
  VIS_TRACEACC_POLYGONS = 7         ///< do polygon accurate traceline/corona checks against the entity (slow!). If the entity has no model assigned then AABOX accuracy will be used instead.
};





/// \brief
///   The various light source types supported by the Vision engine
enum VisLightSourceType_e 
{
  VIS_LIGHT_POINT = 1,              ///< standard point lightsource
  VIS_LIGHT_DIRECTED = 3,           ///< directed lightsource
  VIS_LIGHT_SPOTLIGHT= 5            ///< spotlight or projected light 
};


/// \brief
///   Values for the light attenuation mode
/// 
/// Dynamic lights have two different attenuation modes
/// 
/// For VIS_LIGHT_ATTEN_CUSTOM
/// \li The new technique that is more suitable for making dynamic lights with a custom fall-of
///   curve
/// 
/// \li You can control the intensity and radius separately
/// 
/// \li Typical intensity values are 1,2,3,4...
/// 
/// \li The intensities act as a multiplier and can create overbright effects (e.g. 2x)
/// 
/// For VIS_LIGHT_ATTEN_1OVERX
/// \li Old technique with 1/x falloff which gives it a large fallof distance 
/// 
/// \li Not supported anymore
enum VisLightAttenuationType_e
{
  VIS_LIGHT_ATTEN_1OVERX = 0,   ///< Old version with 1/x attenuation; radius depends on intensity; final pixel intensity is clamped to a max of 1
  VIS_LIGHT_ATTEN_CUSTOM = 1    ///< New technique. Separate radius and intensity, overbright and custom fall-off curve 
};


/// \brief
///   All available rasterizer style types for surfaces/primitives
enum VIS_RasterizerStyle 
{
  VIS_RASTERIZER_CULL_FRONT = 0,
  VIS_RASTERIZER_CULL_BACK = 1,
  VIS_RASTERIZER_CULL_NONE = 2,
  VIS_RASTERIZER_SCISSOR_BIT = 4, ///< bit flag that can be combined with all other modes 
  VIS_RASTERIZER_CULL_FRONT_SCISSOR = 4,
  VIS_RASTERIZER_CULL_BACK_SCISSOR = 5,
  VIS_RASTERIZER_CULL_NONE_SCISSOR = 6,
  VIS_RASTERIZER_WIREFRAME = 7,

  VIS_RASTERIZER_NO_MULTISAMPLE_BIT = 8 ///< bit flag that can be combined with all other modes 
};

/// \brief
///   Standard stencil states
enum VisStencilState_e
{
  VIS_DEPTHSTENCIL_NODEPTHWRITE = 5,
  VIS_DEPTHSTENCIL_NODEPTHTEST = 6,
  VIS_DEPTHSTENCIL_NODEPTHTEST_NODEPTHWRITE = 7,
  VIS_DEPTHSTENCIL_RENDEREQUALDEPTH = 8,
  VIS_DEPTHSTENCIL_SHADOWS_TWOSIDED_REVERSE = 9
};


/// \brief
///   Standard sampler states
enum VisSamplerState_e
{
  VIS_SAMPLER_CLAMP  = 0,
  VIS_SAMPLER_REPEAT = 1,
  VIS_SAMPLER_FILTERING = 0,
  VIS_SAMPLER_NEAREST   = 2
};


/// \brief
///   Describes the sky layer mapping type
enum VIS_SKY_MappingType 
{
  VIS_SKYMAPPING_SPHERICAL = 0,       ///< spherical mapping, normally used for the sky, the clouds, ...
  VIS_SKYMAPPING_CUBEMAP = 1,         ///< cubemap, the layer consists of 6 sides with arbitrary textures (to be set with VisSkyLayer_cl::SetCubemapTextures)
  VIS_SKYMAPPING_CYLINDRICAL = 2,     ///< cylindrical mapping; The texture performs a full wrap around the horizon
};




/// \brief
///   Enumeration which contains the available primitive types of the engine
enum VIS_PRIMITIVE_Type 
{
  VIS_PRIMITIVETYPE_INVALID = 0,            ///< Invalid primitive type
  VIS_PRIMITIVETYPE_POLYGON = 1,            ///< Primitive Type: Single Polygon
  VIS_PRIMITIVETYPE_PATCH = 2,              ///< Primitive Type: Bezier Patch
  VIS_PRIMITIVETYPE_TERRAIN = 3,            ///< Primitive Type: Terrain
  VIS_PRIMITIVETYPE_LIST = 4,               ///< Primitive Type: Triangle List or Strip
  VIS_PRIMITIVETYPE_ENTITY = 6,             ///< Primitive Type: Entity, for instance used for collision meshes
  VIS_PRIMITIVETYPE_STATICMESH = 7,         ///< Primitive Type: Static Mesh
  VIS_PRIMITIVETYPE_CUSTOM = 8              ///< Primitive Type: Custom Geometry
};



/// \brief
///   Collision Mesh Geometry type. 
enum VisCollisionMeshGeoType_e
{
  VIS_COLMESH_GEOTYPE_MESH = 0,             ///< Collision Mesh may be arbitrary mesh
  VIS_COLMESH_GEOTYPE_CONVEXHULL = 1        ///< Collision Mesh is convex/a convex hull. Used for optimizations in physics code.
};


/// \brief
///   Configuration Structure enums
enum VIS_CFG_OnOff 
{
  VIS_ENABLED = 1,                        ///< Feature/Setting is enabled
  VIS_DISABLED = 0                        ///< Feature/Setting
};


/// \brief
///   Configuration Structure enumeration which contains six different detail levels
enum VIS_CFG_Detail6Steps 
{
  VIS_D6_OFF = 0,             ///< setting is off
  VIS_D6_VERYLOW = 1,         ///< very low detail 
  VIS_D6_LOW = 2,             ///< low detail
  VIS_D6_MEDIUM = 3,          ///< medium detail
  VIS_D6_HIGH = 4,            ///< high detail
  VIS_D6_VERYHIGH = 5         ///< very high detail
};


/// \brief
///   Configuration Structure enumeration which contains five different detail levels
enum VIS_CFG_Detail5Steps 
{
  VIS_D5_VERYLOW = 0,         ///< very low detail 
  VIS_D5_LOW = 1,             ///< low detail
  VIS_D5_MEDIUM = 2,          ///< medium detail
  VIS_D5_HIGH = 3,            ///< high detail
  VIS_D5_VERYHIGH = 4         ///< very high detail
};


/// \brief
///   Configuration Structure enumeration which contains four different detail levels
enum VIS_CFG_Detail4Steps
{
  VIS_D4_LOW = 0,             ///< low detail
  VIS_D4_MEDIUM = 1,          ///< medium detail
  VIS_D4_HIGH = 2,            ///< high detail
  VIS_D4_VERYHIGH = 3         ///< very high detail
};


/// \brief
///   Configuration Structure enumeration which contains three different detail levels
enum VIS_CFG_Detail3Steps 
{
  VIS_D3_LOW = 0,             ///< low detail
  VIS_D3_MEDIUM = 1,          ///< medium detail
  VIS_D3_HIGH = 2             ///< high detail
};


/// \brief
///   Configuration Structure enumeration which contains two different detail levels
enum VIS_CFG_Detail2Steps 
{
  VIS_D2_LOW = 0,             ///< low detail
  VIS_D2_HIGH = 1             ///< high detail
};


/// \brief
///   lighting mode to be used for static light-mapping
enum VIS_CFG_LightingMode 
{
  VIS_LIGHTING_NOLIGHTMAPS = -1, ///< No lightmaps in the scene
  VIS_LIGHTING_DEFAULT = 0,      ///< Default, use the settings from the litfile (usually modulate, but also modulate2x)
  VIS_LIGHTING_MODULATE = 1,     ///< use color modulation for lightmap (lightmap*base texture) 
  VIS_LIGHTING_MODULATE2X = 2,   ///< use color modulation 2x for overbright effects (2*lightmap*base texture) 
  VIS_LIGHTING_MODULATE4X = 4,   ///< use color modulation 4x for overbright effects (4*lightmap*base texture) 
  VIS_LIGHTING_BUMPLIGHTMAPS = 8 ///< use 3 lightmaps for static pixel lighting
};


/// \brief
///   Configuration Structure enumeration for the texture detail
enum VIS_CFG_TextureDetail 
{
  VIS_TEXDETAIL_FULL = 2,                   ///< Full Texture Detail
  VIS_TEXDETAIL_REDUCED = 1,                ///< Reduced Texture Detail
  VIS_TEXDETAIL_LOW = 0                     ///< Low Texture Detail
};


/// \brief
///   Enumeration which contains the available model animation types
enum VisModelAnimType_e
{
  VIS_MODELANIM_UNKNOWN = 0,  ///< not used
  VIS_MODELANIM_VERTEX = 1,   ///< vertex animation
  VIS_MODELANIM_MUSCLE = 1,   ///< vertex animation
  VIS_MODELANIM_SKELETAL = 2  ///< skeletal
};


/// \brief
///   Enumeration which contains the available report types for errors
enum VisReportEntryType_e
{
  VIS_REPORTENTRY_INFO = 0,   ///< information
  VIS_REPORTENTRY_WARNING,    ///< warning
  VIS_REPORTENTRY_ERROR       ///< error
};



/// \brief
///   Enumeration which contains the available report group types for errors
enum VisReportGroupType_e
{
  // errors
  VIS_REPORTGROUPTYPE_FILE_NOT_FOUND = 0,                     ///< Error: file not found
  VIS_REPORTGROUPTYPE_MISSING_TEXTURE,                        ///< Error: missing texture
  VIS_REPORTGROUPTYPE_MISSING_ENTITY_CLASS,                   ///< Error: missing entity class
  VIS_REPORTGROUPTYPE_MISSING_COMPONENT_CLASS,                ///< Error: missing component class
  VIS_REPORTGROUPTYPE_MISSING_FXLIB,                          ///< Error: missing shader effect library
  VIS_REPORTGROUPTYPE_MISSING_PROJECTEDLIGHT_TEXTURE,         ///< Error: missing projected light texture
  VIS_REPORTGROUPTYPE_MISSING_MODELFILE,                      ///< Error: missing model file
  VIS_REPORTGROUPTYPE_OLD_MODELFILE,                          ///< Warning: 8.1 reuires new model format
  VIS_REPORTGROUPTYPE_MISSING_BITMAP,                         ///< Error: missing bitmap file (VisBitmap_cl class, usually used for color curves)
  VIS_REPORTGROUPTYPE_MISSING_ANIMATION_FILE,                 ///< Error: missing animation file
  VIS_REPORTGROUPTYPE_MISSING_FONT,                           ///< Error: missing font file
  VIS_REPORTGROUPTYPE_MISSING_LIGHTGRID,                      ///< Error: missing lightgrid file
  VIS_REPORTGROUPTYPE_MISSING_MATERIAL_TEMPLATE,              ///< Error: missing material template file
  VIS_REPORTGROUPTYPE_UNSUPPORTED_VERSION,                    ///< Error: the version is not supported by this build of the engine
  VIS_REPORTGROUPTYPE_COMPRESSED_BUMPMAP,                     ///< Error: A compressed dds file was tried to convert from bumpmap to normalmap
  VIS_REPORTGROUPTYPE_NOLIGHTMAPSTREAM,                       ///< Error: A model file does not provide the necessary lightmap streams

  // warnings
  VIS_REPORTGROUPTYPE_MISSING_EFFECT,                         ///< Warning: missing shader effect
  VIS_REPORTGROUPTYPE_CUBEMAP_ENTITIY_NOT_FOUND,              ///< Warning: a cubemap entity referenced by a shader not found
  VIS_REPORTGROUPTYPE_TEXTURE_INVALID_SIZE,                   ///< Warning: texture has invalid size (not power of 2)
  VIS_REPORTGROUPTYPE_FILENAME_INVALID_CHAR,                  ///< Warning: a (texture-)filename contains an invalid character, e.g. '%' (will be replaced by '_')
  VIS_REPORTGROUPTYPE_FILENAME_MISSING_CALLBACK,              ///< Warning: a shader callback function (texture/perframe/perpolygon) is missing
  VIS_REPORTGROUPTYPE_BUMPINESS,                              ///< Warning: a surface uses bumpmaps instead of normalmaps
  VIS_REPORTGROUPTYPE_OLD_SHADERLIB,                          ///< Warning: the shader library is of an old format and should be recompiled

  // information
  VIS_REPORTGROUPTYPE_RESOURCE_SURVIVED                       ///< Information: Resources survived unloading a world
};

/// \brief
///   Enumeration used to describe the results of a portal clipping operation.
/// 
/// VIS_CLIPPINGRESULT_ALLCLIPPED means that the portal has been completely clipped away. There is
/// no overlap between the portal and the frustum.
/// 
/// VIS_CLIPPINGRESULT_UNCHANGED means that the portal is fully inside the frustum. No clipping has
/// occurred.
/// 
/// VIS_CLIPPINGRESULT_CLIPPED means that the portal has been partially clipped away by the
/// frustum.
/// 
/// VIS_CLIPPINGRESULT_TOOCLOSE means that no clipping has been performed because the portal is
/// very close to the origin of the frustum.
enum VisClippingResult_e
{
  VIS_CLIPPINGRESULT_ALLCLIPPED = 0,
  VIS_CLIPPINGRESULT_UNCHANGED = 1,
  VIS_CLIPPINGRESULT_CLIPPED = 2,
  VIS_CLIPPINGRESULT_TOOCLOSE = 3
};


#ifndef SPU

/// \brief
///   Structure which contains the data for one single particle
struct Particle_t
{
  float      pos[3];                ///< x,y,z position of the particle in world space
  float      size;                  ///< size of the particle
  float      normal[3];             ///< normal vector of particle
  VColorRef  color;                 ///< RGBA color value of the particle (0-255, red, green, blue, alpha)      
  union
  {
    float      distortion[3];       ///< distortion vector of the particle in world space, the particle will be stretched in that direction if distortion is enabled
    struct     
    {   
      float angle;                  ///< angle (in RAD) for particle rotation
      float dummySize;              ///< dummy value. Currently not used.
      float dummyMustBeOne;         ///< dummy value. Currently must be 1.0f.
    };
  };

  // This is necessary, as the DXGI_FORMAT_R8G8B8A8_* vertex attribute formats on the xbox360 are read in reverse order.
#ifdef _VISION_XENON
  char       valid;                 ///< if TRUE then this particle will be drawn
  UBYTE      blend_weight;          ///< in case the texture uses an animation subdivision, this member specifies the blend factor between this frame and the next frame
  UBYTE      animation_next_frame;  ///< in case the texture uses an animation subdivision, this member specifies the frame index in the texture to blend with
  UBYTE      animation_frame;       ///< in case the texture uses an animation subdivision, this member specifies the frame index in the texture
#else
  UBYTE      animation_frame;       ///< in case the texture uses an animation subdivision, this member specifies the frame index in the texture
  UBYTE      animation_next_frame;  ///< in case the texture uses an animation subdivision, this member specifies the frame index in the texture to blend with
  UBYTE      blend_weight;          ///< in case the texture uses an animation subdivision, this member specifies the blend factor between this frame and the next frame
  char       valid;                 ///< if TRUE then this particle will be drawn
#endif

  float      velocity[3];           ///< custom variable for the programmer
};


/// \brief
///   Structure used to store and cache light grid data.
struct LightGridData_t 
{
  hkvVec3 vColors[6];  ///< Light values from six directions
  unsigned int iLastUpdate;         ///< Last frame in which this light grid data structure was updated
};


/// \brief
///   Structure that is used for sorting particles
struct ParticleSort_t
{
  unsigned short  index;            ///< index of the particle in the particle list that this structure is referencing to
  unsigned short  sortkey;          ///< key that determines the sort order of the particle that is referenced (e.g. camera distance)
};


/// \brief
///   Structure which contains the position and normal for one vertex. This structure is used for
///   custom muscle animations.
struct VisObjectVertexDelta_t {
  int           vertex;             ///< number of the vertex, beginning with #1
  float         delta[3];           ///< vertex position(!) x,y,z in object space
  signed char   normal[4];          ///< vertex normal information in object space       
};


class  VisSurface_cl;
class VCompiledShaderPass;
class VBaseSubmesh;


/// \brief
///   Structure that represents information about a single mesh draw call
///
/// An array of this structure can be passed to function VisRenderLoopHelper_cl::RenderEntityWithSurfaceShaderList for fine grained control of the
/// rendered geometry. A single draw call consists of 3 important members:
///   \li The submesh that should be rendered
///   \li The surface that provides the material states (can be a different pointer than the submesh's own surface)
///   \li The shader pass that should be used for rendering.
///
/// Default rendering of dynamic meshes is performed by building a list of submeshes, the respective surfaces and the flattened list of shader passes
/// of the material techniques.
struct VisDrawCallInfo_t
{
  inline VisDrawCallInfo_t() : m_uiResolvedPassType(VPT_Undefined) {}

  /// \brief
  ///   Convenience function to set all members
  VISION_APIFUNC void Set(VBaseSubmesh *pSubmesh, VisSurface_cl *pSurface, VCompiledShaderPass *pShader);

  /// \brief returns the submesh
  inline VBaseSubmesh* GetSubmesh() const { return m_pSubmesh; }

  /// \brief returns the surface
  inline VisSurface_cl* GetSurface() const { return m_pSurface; }

  /// \brief returns the shader
  inline VCompiledShaderPass* GetShader() const { return m_pShader; }

  /// \brief returns the resolved pass type
  inline UBYTE GetResolvedPassType() const { return m_uiResolvedPassType; }

private:
  VBaseSubmesh *m_pSubmesh;       ///< The submesh that should be rendered
  VisSurface_cl *m_pSurface;      ///< The surface that provides material settings (textures etc.)
  VCompiledShaderPass *m_pShader; ///< The shader pass used for rendering
  UBYTE m_uiResolvedPassType;     ///< The resolved pass type
};

/// \brief
///   Helper structure that is passed to a shader callback when the sender type is GEOMETRYTYPE_ENTITYSUBMESH
struct VEntitySubmeshInfo_t
{
  VEntitySubmeshInfo_t(VisBaseEntity_cl *pEntity, int iSubmeshIndex) 
    : m_pEntity(pEntity), m_iSubmeshIndex(iSubmeshIndex)
  {
  }

  VisBaseEntity_cl *m_pEntity;  ///< The entity that is currently being rendered
  int m_iSubmeshIndex;          ///< The 0-based index of the submesh that is currently being rendered
};


#ifndef _VISION_DOC

struct ColoredLine_t {
  hkvVec3 start;
  hkvVec3 end;
  VColorRef iColor;
  VSimpleRenderState_t state;
};

struct GLTriangle_t 
{
  hkvVec3 vPos[3];
  VColorRef iColor;
  VSimpleRenderState_t iRenderState;
};


struct GLSprite_t
{
  hkvVec3 vCenter;
  VTextureObject *pTexture;
  VColorRef iColor;
  VSimpleRenderState_t iRenderState;
  float fSizeX,fSizeY;
  hkvVec2 uv0,uv1;
};



class VisDebugRenderQueue_cl
{
public:
  inline VisDebugRenderQueue_cl()
  {
    m_iLineCount = m_iTriCount = m_iSpriteCount = 0;
  }

  inline ~VisDebugRenderQueue_cl()
  {
    Reset();
  }

  void Render(char mode);

  inline void Reset()
  {
    GLSprite_t *pSprite = m_SpriteBuffer.GetDataPtr();
    for (int i=0;i<m_iSpriteCount;i++,pSprite++)
      V_SAFE_RELEASE(pSprite->pTexture);

    m_iLineCount = m_iTriCount = m_iSpriteCount = 0;
  }

  inline void PurgeMemory()
  {
    Reset();
    m_LineBuffer.Reset();
    m_TriBuffer.Reset();
    m_SpriteBuffer.Reset();
  }

  inline ColoredLine_t &CreateLine(const hkvVec3& startPoint, const hkvVec3& endPoint, VColorRef color, const VSimpleRenderState_t &state)
  {
    VASSERT(m_iLineCount<10000000); // sanity
    ColoredLine_t &line(m_LineBuffer[m_iLineCount++]);
    line.start = startPoint;
    line.end = endPoint;
    line.iColor = color;
    line.state = state;

    return line;
  }

  inline GLTriangle_t &CreateTriangle(const hkvVec3& vPos0, const hkvVec3& vPos1, const hkvVec3& vPos2, VColorRef iColor, const VSimpleRenderState_t &state)
  {
    VASSERT(m_iTriCount<1000000); // sanity
    GLTriangle_t &tri(m_TriBuffer[m_iTriCount++]);
    tri.vPos[0] = vPos0;
    tri.vPos[1] = vPos1;
    tri.vPos[2] = vPos2;
    tri.iColor = iColor;
    tri.iRenderState = state;
    return tri;
  }

  void CreateSprite(const hkvVec3 &vCenter, VTextureObject *pTexture, VColorRef iColor, const VSimpleRenderState_t &iProperties, float fSizeX, float fSizeY, const hkvVec2& uv0=hkvVec2(0,0), const hkvVec2& uv1=hkvVec2(1,1) )
  {
    VASSERT(m_iSpriteCount<1000000); // sanity
    GLSprite_t *pSrite = &m_SpriteBuffer[m_iSpriteCount++];
    if (pTexture) pTexture->AddRef();
    pSrite->pTexture = pTexture;
    pSrite->vCenter = vCenter;
    pSrite->iColor = iColor;
    pSrite->iRenderState = iProperties;
    pSrite->iRenderState.iRenderFlags |= RENDERSTATEFLAG_DOUBLESIDED;
    pSrite->fSizeX = fSizeX;
    pSrite->fSizeY = fSizeY;
    pSrite->uv0 = uv0;
    pSrite->uv1 = uv1;
  }

  int m_iLineCount;
  DynArray_cl<ColoredLine_t> m_LineBuffer;

  int m_iTriCount;
  DynArray_cl<GLTriangle_t> m_TriBuffer;

  int m_iSpriteCount;
  DynArray_cl<GLSprite_t> m_SpriteBuffer;
};




#endif


#ifndef _VISION_DOC

struct GlobalWorldArrayDimensions_t {
  unsigned int     lights;
  unsigned int     objects;
};

#endif



/// \brief
///   Structure which defines a single frame for a screenmask in frame mode
/// 
/// This structure provides 4 pairs of float coordinates (source position and size within
/// screenmask texture, and target position and size on screen). For screenmask operations,
/// coordinates are specified in actual texel/pixel coordinates rather than the texture
/// coordinates' 0 to 1 range.
/// 
/// If the use_color flag is set (use_color=1) the frame is rendered with the  modulation color
/// defined by the color member (RGB and alpha).
/// 
/// If the use_color flag is not set (use_color=0) the frame is rendered with the  screenmask's
/// modulation color and transparency.
/// 
/// The key member is for arbitrary use and will be ignored by the engine. 
/// 
/// \sa VisScreenMask_cl::SetFrames
/// \sa VisScreenMask_cl::DeleteFrames
/// \sa struct VisMaskFrameList_t
/// 
/// \example
///   \code
///   VisMaskFrame_t frame;
///   frame.sourcepos[0] = 0; // source rect is (0,0)-(256,256)
///   frame.sourcepos[1] = 0;
///   frame.sourcesize[0] = 256.f;
///   frame.sourcesize[1] = 256.f;
///   frame.targetpos[0] = 0; // cover whole screen (screen res 640x480)
///   frame.targetpos[1] = 0;
///   frame.targetsize[0] = 640.f;
///   frame.targetsize[1] = 480.f;
///   \endcode
struct VisMaskFrame_t {
  VisMaskFrame_t() ///< constructor
  {
    use_color=0;
    color=V_RGBA_WHITE;
    use_rotation=0;
    fRotationAngle=0.f;
    fRotationCenter[0]=fRotationCenter[1]=0.f;
    iVisibleBitmask = 0xffffffff;
  }
  float sourcepos[2];           ///< Source position
  float sourcesize[2];          ///< Source size
  float targetpos[2];           ///< Target position
  float targetsize[2];          ///< Target size
  VColorRef color;              ///< Color (RGBA)
  short key;                    ///< User definable key (not used by the engine)
  char use_color;               ///< Use color
  char use_rotation;            ///< Use rotation (fRotationAngle and fRotationOffset has to be set)
  float fRotationAngle;         ///< Frame rotation angle in degree (see also screenmask rotation)
  float fRotationCenter[2];     ///< The relative rotation center (0,0) means rotating around targetpos, (targetsize[0]*0.5f, targetsize[1]*0.5f) means rotating around the frame's center
  unsigned int iVisibleBitmask; ///< Used to AND with the context's render filter
};

/// \brief
///   Structure which defines a list of frames for screenmasks in frame mode
/// 
///  The numFrames member defines the number of frames in the list, pMasks points to an array of
/// numFrames elements of type VisMaskFrame_t, each defining a frame.
/// 
/// \sa VisScreenMask_cl::SetFrames
/// \sa VisScreenMask_cl::DeleteFrames
/// \sa struct VisMaskFrame_t
struct VisMaskFrameList_t {
  int numFrames;                ///< Number of frames
  VisMaskFrame_t *pMasks;       ///< Pointer to array of frames
};

 
#endif





const int VIS_PROFILE_TRACELINE_PORTALS = 1,
          VIS_PROFILE_TRACELINE_API = 2,
          VIS_PROFILE_TRACELINE_CORONAS = 3,
          
          VIS_PROFILE_PHYSICS_RUNSIMULATION = 4,
          VIS_PROFILE_PARTICLES_HANDLE = 5,
          VIS_PROFILE_GAMELOOP_ANIMATION = 6,
          VIS_PROFILE_GAMELOOP_THINKFUNCTION = 7,
          VIS_PROFILE_GAMELOOP_PRETHINKFUNCTION = 8,
          VIS_PROFILE_GAMELOOP_UPDATELOOP = 9,
          VIS_PROFILE_GAMELOOP_MODULESYSHANDLE = 10,
          VIS_PROFILE_PHYSICS_FETCHRESULTS = 11;

const int VIS_BONE_MATRIX_LOCAL = 1,        ///< The matrix of the bone in parent bone space
          VIS_BONE_MATRIX_MODEL = 2,        ///< The matrix of the bone in model space
          VIS_BONE_MATRIX_WORLD = 4,        ///< The matrix of the bone in world space
          VIS_BONE_MATRIX_USE_CUSTOM = 16,  ///< Use the supplied custom matrix to modify the bone (typically used)
          VIS_BONE_MATRIX_NORMALIZE = 32;   ///< Normalize the results (Only needed if you turn normalization off for the entity)



#ifndef _VISION_DOC

  struct SkinningVertex_t
  {
    float pos[3];
    float normal[3];

    #ifdef SUPPORTS_SKINNED_TANGENTS
      float tangent[3];
    #endif
  };

#endif

/// \brief
///   Check if a version is higher than the max version
/// 
/// Checks if the given version is above the max version. Is this the case, there will be a system
/// meassage with the description and the versions as well as a report with this error.
///
/// \param pszDescr
///   Name of the file - will be part of the output string
///
/// \param iVersion
///   Version number to check
///
/// \param iMaxVersion
///   Maximum allowed version number.
/// 
/// \return
///   False if the given version is above the max version, else true.
/// 
/// \example
///   \code
///   if (!CheckVersion("models/character.model", 1, 10)) return false;
///   \endcode
bool CheckVersion(const char *pszDescr, int iVersion, int iMaxVersion);




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
