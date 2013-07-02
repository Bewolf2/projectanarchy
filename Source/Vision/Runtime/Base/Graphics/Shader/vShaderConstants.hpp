/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderConstants.hpp

#ifndef VSHADERCONSTANTS_HPP_INCLUDED
#define VSHADERCONSTANTS_HPP_INCLUDED

#define VSHADER_TRACKING_LIGHTGRID_PS               0x00000001
#define VSHADER_TRACKING_LIGHTGRID_GS               0x00000002
#define VSHADER_TRACKING_LIGHTGRID_VS               0x00000004
#define VSHADER_TRACKING_LIGHTMASK_MATRIX           0x00000008
#define VSHADER_TRACKING_BONEMATRICES               0x00000010
#define VSHADER_TRACKING_WII_DYNAMICLIGHTS          0x00000020
#define VSHADER_TRACKING_MODELVIEWMATRIX            0x00000100
#define VSHADER_TRACKING_MODELVIEWPROJECTIONMATRIX  0x00000200
#define VSHADER_TRACKING_MODELTOWORLDSPACEMATRIX    0x00000400
#define VSHADER_TRACKING_MODELVIEWMATRIX_PS         0x00000800
#define VSHADER_TRACKING_USERBIT0                   0x00010000
#define VSHADER_TRACKING_USERBIT1                   0x00020000
#define VSHADER_TRACKING_USERBIT2                   0x00040000
#define VSHADER_TRACKING_USERBIT3                   0x00080000
#define VSHADER_TRACKING_DEFAULT                    (VSHADER_TRACKING_MODELVIEWMATRIX|VSHADER_TRACKING_MODELVIEWPROJECTIONMATRIX|VSHADER_TRACKING_MODELTOWORLDSPACEMATRIX)

#if defined(_VR_DX11)
  // suggested names:
  #define VCONSTANTBUFFERNAME_USER        "g_GlobalConstantBufferUser"
  #define VCONSTANTBUFFERNAME_OBJECT      "g_GlobalConstantBufferObject"
  #define VCONSTANTBUFFERNAME_LIGHTGRID   "g_GlobalConstantBufferLightGrid"
  #define VCONSTANTBUFFERNAME_SKY         "g_GlobalConstantBufferSky"

  // slots:
  #define VCONSTANTBUFFERINDEX_PERFRAME         0
  #define VCONSTANTBUFFERINDEX_PEROBJECT        1
  #define VCONSTANTBUFFERINDEX_USER             2
  #define VCONSTANTBUFFERINDEX_LIGHTGRID        3
  #define VCONSTANTBUFFERINDEX_SKY              3
  #define VCONSTANTBUFFERINDEX_HOQ_BOXSCALE     3
  #define VCONSTANTBUFFERINDEX_PARTICLES        3
  #define VCONSTANTBUFFERINDEX_STENCILLSHADOWS  3
  #define VCONSTANTBUFFERINDEX_SKINNING         4

  #define VSHADER_VS_REGISTER_MODELVIEWMATRIX            0
  #define VSHADER_VS_REGISTER_PROJECTIONMATRIX           0
  #define VSHADER_VS_REGISTER_MODELVIEWPROJECTIONMATRIX  4
  #define VSHADER_VS_REGISTER_EYEMATRIX                  4
  #define VSHADER_VS_REGISTER_INVERSEEYEMATRIX           8
  #define VSHADER_VS_REGISTER_MODELTOWORLDSPACEMATRIX    8
  #define VSHADER_VS_REGISTER_LIGHTMAPTOLIGHTMASK        12
  #define VSHADER_VS_REGISTER_LIGHTGRID                  0
  #define VSHADER_VS_REGISTER_LIGHTGRID_LAST             5

  #define VSHADER_PS_REGISTER_LIGHTGRID                  0
  #define VSHADER_PS_REGISTER_LIGHTGRID_LAST             5
  #define VSHADER_PS_REGISTER_MODELVIEWMATRIX            0
#else
  #define VSHADER_VS_REGISTER_MODELVIEWMATRIX            0
  #define VSHADER_VS_REGISTER_PROJECTIONMATRIX           4
  #define VSHADER_VS_REGISTER_MODELVIEWPROJECTIONMATRIX  8
  #define VSHADER_VS_REGISTER_EYEMATRIX                  12
  #define VSHADER_VS_REGISTER_INVERSEEYEMATRIX           16
  #define VSHADER_VS_REGISTER_MODELTOWORLDSPACEMATRIX    20
  #define VSHADER_VS_REGISTER_LIGHTMAPTOLIGHTMASK        28
  #define VSHADER_VS_REGISTER_LIGHTGRID                  32
  #define VSHADER_VS_REGISTER_LIGHTGRID_LAST             37

  #define VSHADER_PS_REGISTER_LIGHTGRID                  26
  #define VSHADER_PS_REGISTER_LIGHTGRID_LAST             31
  #define VSHADER_PS_REGISTER_MODELVIEWMATRIX            22
#endif

#if defined(_VR_GLES2) || defined(_VPLATFORMHANDLERGLES2)

enum VisGLESVertexAttrib
{
  VIS_GLES_ATTRIB_ARRAY_POSITION,
  VIS_GLES_ATTRIB_ARRAY_NORMAL,
  VIS_GLES_ATTRIB_ARRAY_TANGENT,
  VIS_GLES_ATTRIB_ARRAY_COLOR0,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD0,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD1,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD2,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD3,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD4,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD5,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD6,
  VIS_GLES_ATTRIB_ARRAY_TEXCOORD7,
  VIS_GLES_NUM_ATTRIBS
};

static const char* VisGLESVertexAttribNames[] = {
  "position",
  "normal",
  "tangent",
  "color",
  "tex0",
  "tex1",
  "tex2",
  "tex3",
  "tex4",
  "tex5",
  "tex6",
  "tex7"
};

#endif

#define VSHADER_RASTERIZERSTATEBIT_FRONTCCW     V_BIT(0)
#define VSHADER_RASTERIZERSTATEBIT_NODEPTHCLIP  V_BIT(1)
#define VSHADER_RASTERIZERSTATEBIT_SCISSORTEST  V_BIT(2)
#define VSHADER_RASTERIZERSTATEBIT_MULTISAMPLE  V_BIT(3)



/// \brief
///   Enum that represents a target platform. To bitwise combine platforms, use
///   e.g. (1<<TARGETPLATFORM_DX9).
enum VTargetPlatform_e
{
  TARGETPLATFORM_DX9      = 0,
  TARGETPLATFORM_XBOX360  = 1,
  TARGETPLATFORM_PS3      = 2,
  TARGETPLATFORM_WII      = 3,
  TARGETPLATFORM_DX10     = 4,
  TARGETPLATFORM_DX11     = 5,
  TARGETPLATFORM_PSP2     = 6,
  TARGETPLATFORM_GLES2    = 7,
  TARGETPLATFORM_LINUX    = 8,
  TARGETPLATFORM_WIIU     = 9,
  TARGETPLATFORM_COUNT    = 10,

  #if defined (WIN32)
    #if defined (_VR_DX9)
      TARGETPLATFORM_THIS = TARGETPLATFORM_DX9
    #elif defined (_VR_DX11)
      TARGETPLATFORM_THIS = TARGETPLATFORM_DX11
    #endif
  #elif defined(_VISION_XENON)
      TARGETPLATFORM_THIS = TARGETPLATFORM_XBOX360
  #elif defined(_VISION_PS3)
      TARGETPLATFORM_THIS = TARGETPLATFORM_PS3
  #elif defined(_VISION_PSP2)
      TARGETPLATFORM_THIS = TARGETPLATFORM_PSP2
  #elif defined(_VISION_IOS)
      TARGETPLATFORM_THIS = TARGETPLATFORM_GLES2
  #elif defined(_VISION_ANDROID)
      TARGETPLATFORM_THIS = TARGETPLATFORM_GLES2
  #elif defined(_VISION_WIIU)
      TARGETPLATFORM_THIS = TARGETPLATFORM_WIIU
  #else
    #error Undefined platform!
  #endif
};


/// \brief
///   Enum that represents the different devices supported by the engine.
enum VTargetDevice_e
{
#ifndef _VISION_DOC
  // NOTE: When adding a new platform, make sure that you propagate your changes into
  // scene export code in vForge. This includes CSharpFramework.AssetManagement.TargetDevice_e,
  // CSharpFramework.Shapes.TargetPlatformSupport, and Editor.EditorScene._ExportSceneNotSaveSettings(). 
#endif
  TARGETDEVICE_DX9      = 0,
  TARGETDEVICE_DX11     = 1,
  TARGETDEVICE_XBOX360  = 2,
  TARGETDEVICE_PS3      = 3,
  TARGETDEVICE_PSP2     = 4,
  TARGETDEVICE_IOS      = 5,
  TARGETDEVICE_ANDROID  = 6,
  TARGETDEVICE_WIIU     = 7,
  TARGETDEVICE_COUNT    = 8,

  #if defined (WIN32) && defined (_VR_DX9)
    TARGETDEVICE_THIS = TARGETDEVICE_DX9
  #elif defined (WIN32) && defined (_VR_DX11)
    TARGETDEVICE_THIS = TARGETDEVICE_DX11
  #elif defined(_VISION_XENON)
    TARGETDEVICE_THIS = TARGETDEVICE_XBOX360
  #elif defined(_VISION_PS3)
    TARGETDEVICE_THIS = TARGETDEVICE_PS3
  #elif defined(_VISION_PSP2)
    TARGETDEVICE_THIS = TARGETDEVICE_PSP2
  #elif defined(_VISION_IOS)
    TARGETDEVICE_THIS = TARGETDEVICE_IOS
  #elif defined(_VISION_ANDROID)
    TARGETDEVICE_THIS = TARGETDEVICE_ANDROID
  #elif defined(_VISION_WIIU)
    TARGETDEVICE_THIS = TARGETDEVICE_WIIU
  #else
    #error Undefined platform!
  #endif
};

static const char* VTargetDeviceName[TARGETDEVICE_COUNT] =
{
  "pcdx9",  
  "pcdx11", 
  "xbox360",
  "ps3",    
  "psvita", 
  "ios",    
  "android",
  "wiiu",   
};

enum VTargetPlatformMask_e
{
  TARGETPLATFORM_MASK_DX9     = 1 << TARGETPLATFORM_DX9,
  TARGETPLATFORM_MASK_XBOX360 = 1 << TARGETPLATFORM_XBOX360,
  TARGETPLATFORM_MASK_PS3     = 1 << TARGETPLATFORM_PS3,
  TARGETPLATFORM_MASK_WII     = 1 << TARGETPLATFORM_WII,
  TARGETPLATFORM_MASK_DX10    = 1 << TARGETPLATFORM_DX10,
  TARGETPLATFORM_MASK_DX11    = 1 << TARGETPLATFORM_DX11,
  TARGETPLATFORM_MASK_PSP2    = 1 << TARGETPLATFORM_PSP2,
  TARGETPLATFORM_MASK_GLES2   = 1 << TARGETPLATFORM_GLES2,
  TARGETPLATFORM_MASK_LINUX   = 1 << TARGETPLATFORM_LINUX,
  TARGETPLATFORM_MASK_WIIU    = 1 << TARGETPLATFORM_WIIU,
  TARGETPLATFORM_MASK_ALL     = 0xFFFFFFFF,

  TARGETPLATFORM_MASK_THIS    = 1 << TARGETPLATFORM_THIS,
};

/// \brief
///   Enum that represents a blending operation to combine source and destination term
enum VBlendOp
{
  BLEND_OP_ADD = 1,
  BLEND_OP_SUBTRACT = 2,
  BLEND_OP_REV_SUBTRACT = 3,
  BLEND_OP_MIN = 4,
  BLEND_OP_MAX = 5
};

/// \brief
///   Enum that represents a blending operation function
enum VBlendMode
{
  BLEND_ZERO = 1,
  BLEND_ONE = 2,
  BLEND_SRC_COLOR = 3,
  BLEND_INV_SRC_COLOR = 4,
  BLEND_SRC_ALPHA = 5,
  BLEND_INV_SRC_ALPHA = 6,
  BLEND_DEST_ALPHA = 7,
  BLEND_INV_DEST_ALPHA = 8,
  BLEND_DEST_COLOR = 9,
  BLEND_INV_DEST_COLOR = 10,
  BLEND_SRC_ALPHA_SAT = 11,
  BLEND_BLEND_FACTOR = 12,
  BLEND_INV_BLEND_FACTOR = 13,
  BLEND_SRC1_COLOR = 14,
  BLEND_INV_SRC1_COLOR = 15,
  BLEND_SRC1_ALPHA = 16,
  BLEND_INV_SRC1_ALPHA = 17
};


/// \brief
///   Enum that represents a texture sampler filter mode
enum VFilterMode
{
  FILTER_MIN_MAG_MIP_POINT = 0,
  FILTER_MIN_MAG_POINT_MIP_LINEAR = 1,
  FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT = 2,
  FILTER_MIN_POINT_MAG_MIP_LINEAR = 3,
  FILTER_MIN_LINEAR_MAG_MIP_POINT = 4,
  FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 5,
  FILTER_MIN_MAG_LINEAR_MIP_POINT = 6,
  FILTER_MIN_MAG_MIP_LINEAR = 7,
  FILTER_ANISOTROPIC = 8,
  FILTER_COMPARISON_MIN_MAG_MIP_POINT = 9,
  FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR = 10,
  FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT = 11,
  FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR = 12,
  FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT = 13,
  FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR = 14,
  FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT = 15,
  FILTER_COMPARISON_MIN_MAG_MIP_LINEAR = 16,
  FILTER_COMPARISON_ANISOTROPIC = 17,
  FILTER_DEFAULT = 18
};

/// \brief
///   Enum that represents a texture sampling addressing mode
enum VTextureAddressMode
{
  TEXTURE_ADDRESS_WRAP = 1,
  TEXTURE_ADDRESS_MIRROR = 2,
  TEXTURE_ADDRESS_CLAMP = 3,
  TEXTURE_ADDRESS_BORDER = 4,
  TEXTURE_ADDRESS_MIRROR_ONCE = 5
};

/// \brief
///   Enum that defines the texture binding of a sampler stage, e.g. base texture, lightmap etc.
enum VTextureType
{
  TEXTURETYPE_BASE_TEXTURE = 1,
  TEXTURETYPE_LIGHTMAP     = 2,
  TEXTURETYPE_LIGHTMASK    = 3,
  TEXTURETYPE_AUXILIARY_TEXTURE = 4,
  TEXTURETYPE_CUSTOM_TEXTURE    = 5,
  TEXTURETYPE_CUBEMAP      = 6,
  TEXTURETYPE_TEXTURE3D    = 7,
  TEXTURETYPE_REFMAP       = 8,
  TEXTURETYPE_NORMALMAP    = 9,
  TEXTURETYPE_ACCUMULATIONBUFFER  = 10,
  TEXTURETYPE_DIFFUSECOLORBUFFER  = 11,
  TEXTURETYPE_NORMALBUFFER        = 12,
  TEXTURETYPE_DEPTHBUFFER         = 13,
  TEXTURETYPE_TEXTURE2DARRAY      = 14
};


/// \brief
///   Enum that represents a generic comparison function, e.g. for depth comparison
enum VComparisonFunc
{
  COMPARISON_NEVER = 1,
  COMPARISON_LESS = 2,
  COMPARISON_EQUAL = 3,
  COMPARISON_LESS_EQUAL = 4,
  COMPARISON_GREATER = 5,
  COMPARISON_NOT_EQUAL = 6,
  COMPARISON_GREATER_EQUAL = 7,
  COMPARISON_ALWAYS = 8
};


/// \brief
///   Enum that represents a stencil test operation
enum VStencilOp
{
  STENCIL_OP_KEEP = 1,
  STENCIL_OP_ZERO = 2,
  STENCIL_OP_REPLACE = 3,
  STENCIL_OP_INCR_SAT = 4,
  STENCIL_OP_DECR_SAT = 5,
  STENCIL_OP_INVERT = 6,
  STENCIL_OP_INCR = 7,
  STENCIL_OP_DECR = 8
};


/// \brief
///   Enum that defines the rasterizer's fill mode for triangles
enum VFillMode
{
  FILL_WIREFRAME = 2,
  FILL_SOLID = 3
};


/// \brief
///   Enum that defines the rasterizer's face culling mode for triangles
enum VCullMode
{
  CULL_NONE = 1,
  CULL_FRONT = 2,
  CULL_BACK = 3
};


/// \brief
///   Enum that defines a simple render order of a technique, the values map to unique bits
enum VPassType_e
{
  VPT_AUTO          = 0,
  VPT_Undefined     = 0,
  VPT_PrimaryOpaquePass  = 1, ///< gets rendered at the beginning of the intial pass, usually used to render opaque geometry
  VPT_SecondaryOpaquePass  = 4, ///< gets rendered at the end of the inital pass, usually used to do special effects like stencil operations
  VPT_TransparentPass = 2 ///< gets rendered in the transparent pass, use to render transparent geometry
};

/// \brief
///   Enum that defines a tessellation mode
enum VTesselationMode
{
  TESSELATION_DISCRETE = 0,
  TESSELATION_CONTINUOUS = 1,
  TESSELATION_PEREDGE = 2
};

/// \brief
///   Enum that defines base variable types
enum VValueType_e
{
  VALUETYPE_UNKNOWN = -1,
  VALUETYPE_BOOL = 0,
  VALUETYPE_INT,
  VALUETYPE_FLOAT,
  VALUETYPE_FLOAT2,
  VALUETYPE_FLOAT3,
  VALUETYPE_FLOAT4,
  VALUETYPE_FLOAT3x3,
  VALUETYPE_FLOAT4x4,
  VALUETYPE_STRING,
  VALUETYPE_TEXTURE,
  VALUETYPE_CUBEMAP,
  VALUETYPE_TEXTURE3D,
  VALUETYPE_BOOL2,
  VALUETYPE_BOOL3,
  VALUETYPE_BOOL4,
  VALUETYPE_INT2,
  VALUETYPE_INT3,
  VALUETYPE_INT4,
  VALUETYPE_UINT,
  VALUETYPE_TEXTURE2DARRAY,
  VALUETYPE_FLOAT4x3,
  VALUETYPE_INCLUSION_TAG
};


/// \brief
///   Bitflag hints for displaying variables in a property grid
enum VDisplayHintFlags_e
{
  DISPLAY_HINT_NONE              = 0x00000000, ///< No specific hint flags
  DISPLAY_HINT_COLOR             = 0x00000001, ///< Add a color picker control, i.e. a vector should be interpreted as color
  DISPLAY_HINT_TEXTUREFILE       = 0x00000002, ///< Add a texture file browser control
  DISPLAY_HINT_MODELFILE         = 0x00000004, ///< Add a .model file browser control
  DISPLAY_HINT_CUSTOMFILE        = 0x00000008, ///< Add a file browser control for arbitrary files (*.*)
  DISPLAY_HINT_CUBEMAPKEY        = 0x00000010, ///< Add a control to select cubemap keys
  DISPLAY_HINT_GLOBALUNITSCALED  = 0x00000020  ///< Add a hint that the value should be multiplied by global unit scale value on init
};


/// \brief
///   Enum that defines the distinct state groups
enum VStateGroupFlags_e
{
  STATEGROUP_BLEND = 1,
  STATEGROUP_RASTERIZER = 2,
  STATEGROUP_DEPTHSTENCIL = 4,
  STATEGROUP_TESSELATION = 8,
  STATEGROUP_SAMPLERS = 16,
  STATEGROUP_TEXTURES = 32,
  STATEGROUP_ALL = 0xFF
};



/// \brief
///   enum values for loading and saving shader effect libraries (ShaderLib files)
enum VShaderCompileFlags_e
{
  VSF_NONE                                      = 0,
  VSF_LOAD_IGNOREDESCRIPTIONS                   = 1 << 0,   ///< Ignore shader & effect descriptions when loading
  VSF_LOAD_ALLBYTECODEPLATFORMS                 = 1 << 1,   ///< If specified, bytecode versions for all platforms are loaded (otherwise only for this platform)
  VSF_LOAD_IGNOREPLATFORMBINARIES               = 1 << 4,   ///< Forces loading the .ShaderLib file rather than platform specific. Inside the editor, this is always overridden to true

  VSF_SAVE_ALL                                  = 1 << 0,   ///< saves also non default values into lib file
  VSF_SAVE_COMPILED                             = 1 << 1,   ///< compiles everything before saving to generate latest bytecode and mapping table
  VSF_SAVE_IGNORE_COMPILING_FAILED              = 1 << 2,   ///< if specified, the lib will be saved also when compiling of the shaders failed
  VSF_SAVE_PLATFORMBINARIES                     = 1 << 3,   ///< if specified, platform specific binary files are saved out
  VSF_COMPILE_MISSING_PLATFORMHANDLER_AS_ERROR  = 1 << 4,   ///< Compilation treats missing platform handlers as errors, otherwise ignores these platforms

  // compile relevant flags
  VSF_COMPILE_INCLUDE_DEBUG_INFO                = 1 << 5,   ///< Shaders are compiled with debug info
  VSF_COMPILE_NO_OPTIMIZATION                   = 1 << 6,   ///< Shaders are compiled without optimization
  VSF_COMPILE_FORCE_PARTIAL_PRECISION           = 1 << 7,   ///< Uses reduced precision for floating point computation
  VSF_COMPILE_PREFER_FLOW_CONTROL               = 1 << 8,   ///< Hint flag that determines whether flow control should be preferred rather than unrolling

  VSF_FORCE_RECOMPILATION                       = 1 << 9,   ///< If this flag is set, a shader is always recompiled, even if its source-code hash has not changed recently.
  VSF_BREAK_ON_FIRST_ERROR                      = 1 << 10,  ///< If set, shader compilation will be interrupted when one shader reports an error
  VSF_IGNORE_SHADER                             = 1 << 11,  ///< [internal] Used by "ValidateShader" to mask out shaders that should not be compiled
  VSF_KEEP_PERMUTATION_SOURCES                  = 1 << 12,  ///< If set the source code of auto-generated shaders won't be deleted at the end

  VSF_COMPRESS_BYTECODE                         = 1 << 13,  ///< If set the shader byte code will be stored in a compressed format if the compression saves at least 20% (currently always internally set)

  // Currently we always enable shader compression (since tests showed that loading times benefit)
  // However if a customer has problems with the additional overhead of the decompression we may need to expose this flag in vForge
  VSF_DEFAULT_FLAGS = 
    VSF_SAVE_COMPILED | 
    VSF_SAVE_IGNORE_COMPILING_FAILED | 
    VSF_SAVE_PLATFORMBINARIES |
    VSF_COMPRESS_BYTECODE,                   ///< These flags are set by default when compiling shaders in vForge or the standalone shader compiler

  VSF_FLAGS_THAT_REQUIRE_RECOMPILATION = 
    VSF_COMPILE_INCLUDE_DEBUG_INFO | 
    VSF_COMPILE_NO_OPTIMIZATION | 
    VSF_COMPILE_FORCE_PARTIAL_PRECISION | 
    VSF_COMPILE_PREFER_FLOW_CONTROL |
    VSF_COMPRESS_BYTECODE |
    VSF_KEEP_PERMUTATION_SOURCES,            ///< If any of these flags is different on a stored shader and a to-be-compiled shader, the shader should definitely be recompiled
};


/// \brief
///   All available transparency type for surfaces/primitives
enum VIS_TransparencyType 
{
  VIS_TRANSP_NONE = 0,              ///< no transparency
  VIS_TRANSP_MULTIPLICATIVE = 1,    ///< multiplicative transparency
  VIS_TRANSP_ALPHA = 2,             ///< regular alpha blending
  VIS_TRANSP_ADDITIVE = 3,          ///< additive transparency
  VIS_TRANSP_COLORKEY = 4,          ///< deprecated, renamed to VIS_TRANSP_ALPHATEST
  VIS_TRANSP_ALPHATEST = 4,         ///< replaces 'VIS_TRANSP_COLORKEY' : no transparency, only alpha test
  VIS_TRANSP_ADD_MODULATE = 5,      ///< add the modulated result (dest=dest*(1+src))
  VIS_TRANSP_ADDITIVE_NOALPHA = 6,  ///< additive transparency, ignoring source and destination alpha
  VIS_TRANSP_NOCOLORWRITE = 7,      ///< no blend, no color write, no alpha test
  VIS_TRANSP_MODULATE2X = 8,        ///< modulate and multiply by two
  VIS_TRANSP_SUBTRACTIVE = 9,       ///< subtractive blending (dest-src*src.a)
  VIS_TRANSP_PREMULTIPLIEDALPHA = 10, ///< subtractive blending (src+(dest*(1-src.a))
  VIS_TRANSP_ALPHA_NOALPHATEST_PARTICLES = 11, ///< same as alpha without alphatest but selects special states for particles (internal use)
  VIS_TRANSP_ALPHA_PARTICLES = 12,  ///< same as alpha but selects special states for particles (internal use)
  VIS_TRANSP_ALPHA_NOALPHATEST = 15, ///< same as alpha, but without alpha test (internal use)
};

/// \brief
///   Enumeration for supported static lighting methods used for materials
enum VisLightingMethod_e
{
  VIS_LIGHTING_FULLBRIGHT = 0,
  VIS_LIGHTING_LIGHTMAPPING = 1,
  VIS_LIGHTING_LIGHTGRID = 2,
  VIS_LIGHTING_DYNAMIC_ONLY = 3
};



#endif //VSHADERCONSTANTS_HPP_INCLUDED

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
