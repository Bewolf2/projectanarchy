/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vCompiledShaderPass.hpp

#ifndef VSTATEGROUPS_HPP_INCLUDED
#define VSTATEGROUPS_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSampler.hpp>

/// \brief
///   Enum that is passed as a geometry type to the per-primitive callback function. The void
///   pointer argument to the callback function can be cast accordingly
enum VCallbackGeometryType_e
{
  GEOMETRYTYPE_ENTITY = 1,          ///< the passed object is an entity (deprecated, replaced by GEOMETRYTYPE_ENTITYSUBMESH)
  GEOMETRYTYPE_PARTICLEGROUP = 2,   ///< the passed object is a VisParticleGroup_cl instance
  GEOMETRYTYPE_SCREENMASK = 3,      ///< the passed object is a VisScreenMask_cl instance
  GEOMETRYTYPE_MESHBUFFEROBJECT = 4,///< the passed object is a VisMeshBufferObject_cl instance
  GEOMETRYTYPE_MODEL = 5,           ///< the passed object is a VDynamicMesh instance
  GEOMETRYTYPE_STATICGEOMETRY = 6,  ///< the passed object is a VisStaticGeometryInstance_cl instance
  GEOMETRYTYPE_ENTITYSUBMESH = 7,   ///< the passed object is a pointer to struct VEntitySubmeshInfo_t which holds information about the entity and a submesh instance
};


/// \brief
///   Enum that defines the callback behaviour of per-primitive callback functions
enum VCallbackRetVal_e
{
  CALLBACK_CONTINUE    = 0,     ///< Continue rendering as normally
  CALLBACK_DONTCALL    = 4,     ///< Cancel rendering for the current object
  CALLBACK_CALLAGAIN   = 8      ///< Render the object again (typically only makes sense if the callback modifies the shader or shader constants)
};

#if defined _VISION_DOC

/// \brief
///   Function prototype of a per-primitive callback function
/// 
/// Callback functions of this type can be registered in the engine and used by shaders for
/// multipass rendering and per-instance manipulations of shaders.
/// 
/// \param sendertype
///   Enumeration defining the geometry type of the rendered instance
/// 
/// \param pElement
///   Pointer to the instance. According to the sender type, this pointer can be cast to:
///   \li VisBaseEntity_cl* for GEOMETRYTYPE_ENTITY (deprecated),
///   \li VisParticleGroup_cl* for GEOMETRYTYPE_PARTICLEGROUP,
///   \li VisScreenMask_cl* for GEOMETRYTYPE_SCREENMASK,
///   \li VisMeshBufferObject_cl* for GEOMETRYTYPE_MESHBUFFEROBJECT,
///   \li VDynamicMesh* for GEOMETRYTYPE_MODEL,
///   \li VisStaticGeometryInstance_cl* for GEOMETRYTYPE_STATICGEOMETRY.
///   \li VEntitySubmeshInfo_t* for GEOMETRYTYPE_ENTITYSUBMESH. The VEntitySubmeshInfo_t structure holds information about the entity and the sub mesh index
/// 
/// \param shader
///   The shader pass instance that is used for rendering
/// 
/// \return
///   VCallbackRetVal_e behavior:
///   \li CALLBACK_CONTINUE: Continue with the current shader setup, but do not call this callback
///     again for this render batch
/// 
///   \li CALLBACK_DONTCALL: Abort. The render batch will not be rendered
/// 
///   \li CALLBACK_CALLAGAIN: Render the geometry and call this callback again. This can be used
///     for multipass rendering effects such as fur shells.
///
/// \note
///   The function pointer is declared as (VISION_FASTCALL *VISSHADERCALLBACKFUNC), however doxygen
///   does not like the __fastcall in this declaration 
typedef VCallbackRetVal_e (*VISSHADERCALLBACKFUNC) (VCallbackGeometryType_e sendertype, void *pElement, VCompiledShaderPass *shader);
#else
// this is necessary since doxygen does not like the __fastcall in the function pointer declaration
typedef VCallbackRetVal_e (VISION_FASTCALL *VISSHADERCALLBACKFUNC) (VCallbackGeometryType_e sendertype, void *pElement, VCompiledShaderPass *shader);
#endif

/// \brief
///   State group that controls the render states for blending
///
/// Note that not all states which are part of this state group are actually supported by all platforms. Please
/// refer to each platform's graphics documentation for information on which render states the hardware and graphics API
/// of that platform support.
class VStateGroupBlendBase
{
  public:

    /// \brief
    ///   Constructor; Sets defaults
    inline VStateGroupBlendBase() 
    {
      memset(this, 0, sizeof(VStateGroupBlendBase)); ///< turn off everything by default
      m_cBlendOp = BLEND_OP_ADD;
      m_cSrcBlend = BLEND_ONE;
      m_cDestBlend = BLEND_ONE;
      m_cBlendOpAlpha = BLEND_OP_ADD;
      m_cSrcBlendAlpha = BLEND_ONE;
      m_cDestBlendAlpha = BLEND_ONE;
      m_iRenderTargetWriteMask = 0xffffffff;
      m_cAlphaTestFunc = COMPARISON_GREATER_EQUAL;
      m_cAlphaTestRef = 128;
      m_blendFactor = VColorRef(255,255,255,255);
    }

    /// \brief
    ///   Assignment operator
    inline VStateGroupBlendBase &operator = (const VStateGroupBlendBase &other)
    {
      memcpy(this, &other, sizeof(VStateGroupBlendBase));
      return *this;
    }

    /// \brief
    ///   Comparison operator
    inline bool operator == (const VStateGroupBlendBase& other) const
    {
      if (m_iHash!=other.m_iHash) return false;
      return memcmp(this, &other, sizeof(VStateGroupBlendBase))==0;
    }

    /// \brief
    ///   Comparison operator
    inline bool operator != (const VStateGroupBlendBase& other) const
    {
      return !(*this == other);
    }

    /// \brief
    ///   Cleans up internally allocated data. 
    inline void Cleanup()
    {
    }

    /// \brief
    ///   Legacy function. Still present for compatibility reasons. Do not use.
    inline void SetOldWriteMask(int iOldmask)
    {
      m_iRenderTargetWriteMask = 0;
      for (int i=0;i<8;i++) 
        if (iOldmask&(1<<i))
          m_iRenderTargetWriteMask |= (15<<(i*4)); // every render target (0..7) has 4bits in this 32bit mask
    }

    /// \brief
    ///   Computes the hash value for this state group. This function has to be called prior to using the state group.
    ///
    /// This function always has to be called after setting up a new state group or after modifying an existing state group.
    /// It computes a hash which is internally used for optimization purposes, and additionally generates platforms-specific
    /// data which is critical to the use of the state group.
    inline void ComputeHash()
    {
      m_iHash = ComputeHashInternal();
    }

    /// \brief
    ///   Invalidates all states (for internal use)
    inline void Invalidate()
    {
      ::InvalidateStates(this, sizeof(VStateGroupBlendBase));
    }

    /// \brief
    ///   Verifies the correctness of the hash. Only used for debugging purposes.
    inline bool IsHashValid() const
    {
      return m_iHash == ComputeHashInternal();
    }

    /// \brief
    ///   Helper function to dump the current states into a text file in a readable format
    VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;

    /// \brief
    ///   Binary file serialization
    VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  private:
    inline unsigned int ComputeHashInternal() const
    {
      return ::ComputeHash(this, offsetof(VStateGroupBlendBase, m_iHash));
    }

  public:

    unsigned char m_cBlendEnable;             ///< Blending enabled/disabled (8 Bits, corresponding to 8 Render Targets)
    unsigned char m_bSeparateAlphaBlend;      ///< Separate alpha blend enabled/disabled
    unsigned char m_cBlendOp;                 ///< Blend operation for R,G, and B channels. One of the members of the VBlendOp enumeration
    unsigned char m_cSrcBlend;                ///< Source Blend mode for R,G, and B channels. One of the members of the VBlendMode enumeration
    unsigned char m_cDestBlend;               ///< Destination Blend mode for R,G and B channels. One of the members of the VBlendMode enumeration
    unsigned char m_cBlendOpAlpha;            ///< Blend operation for alpha channel. One of the members of the VBlendOp enumeration
    unsigned char m_cSrcBlendAlpha;           ///< Source Blend mode for alpha channel. One of the members of the VBlendMode enumeration
    unsigned char m_cDestBlendAlpha;          ///< Destination Blend mode for alpha channel. One of the members of the VBlendMode enumeration
    unsigned char m_bAlphaTestEnabled;        ///< Alpha test enabled/disabled. (PC DX9, Xbox360, PS3)
    unsigned char m_cAlphaTestFunc;           ///< Alpha test comparison function. One of the members of the VComparisonFunc enumeration. (PC DX9, Xbox360, PS3)
    unsigned char m_cAlphaTestRef;            ///< Reference value for alpha test. (PC DX9, Xbox360, PS3)
    unsigned char m_cHighPrecisionBlendEnabled;  ///< High-precision blending enabled/disabled (8 Bits, corresponding to 8 Render Targets). (Xbox360)
    unsigned char m_bAlphaToCoverageEnabled;  ///< Alpha To Coverage (A2C) enabled/disabled.
    unsigned int m_iRenderTargetWriteMask;    ///< 8*4bit: Each render target (0..7) reserves 4 bits for color write masks: (16<<ContextIndex) | (1<<iColorComponent)
    VColorRef m_blendFactor;                  ///< Blend factor

    unsigned int m_iHash;
};


/// \brief
///   State group that controls the render states for depth and stencil buffer
///
/// Note that not all states which are part of this state group are actually supported by all platforms. Please
/// refer to each platform's graphics documentation for information on which render states the hardware and graphics API
/// of that platform support.
class VStateGroupDepthStencilBase
{
  public:
    
  /// \brief
  ///   Constructor; Sets defaults
  inline VStateGroupDepthStencilBase()
  {
    memset(this, 0, sizeof(VStateGroupDepthStencilBase)); ///< turn off everything by default
    m_bDepthTestEnabled = true;
    m_bDepthWriteEnabled = true;
    m_cDepthComparisonFunc = COMPARISON_LESS_EQUAL;
    m_iStencilReadMask = 0xFFFFFFFF;
    m_iStencilWriteMask = 0xFFFFFFFF;
    m_iStencilRef = 0;
    m_cStencilFailOp[0] = m_cStencilFailOp[1] = STENCIL_OP_KEEP;
    m_cStencilDepthFailOp[0] = m_cStencilDepthFailOp[1] = STENCIL_OP_KEEP;
    m_cStencilPassOp[0] = m_cStencilPassOp[1] = STENCIL_OP_KEEP;
    m_cStencilComparisonFunc[0] = m_cStencilComparisonFunc[1] = COMPARISON_GREATER;
  }

  /// \brief
  ///   Assignment operator
  inline VStateGroupDepthStencilBase& operator = (const VStateGroupDepthStencilBase& other)
  {
    memcpy(this, &other, sizeof(VStateGroupDepthStencilBase));
    return *this;
  }

  /// \brief
  ///   Comparison operator
  ///
  /// \param other
  ///   depth stencil state group to compare with 
  inline bool operator == (const VStateGroupDepthStencilBase& other) const
  {
    if (m_iHash!=other.m_iHash)
      return false;
    else
      return memcmp(this, &other, sizeof(VStateGroupDepthStencilBase))==0;
  }

  /// \brief
  ///   Comparison operator
  ///
  /// \param other
  ///   depth stencil state group to compare with 
  inline bool operator != (const VStateGroupDepthStencilBase& other) const
  {
    return !(*this == other);
  }  

  /// \brief
  ///   Cleans up internally allocated data. 
  inline void Cleanup()
  {
  }

  /// \brief
  ///   Computes the hash value for this state group. This function has to be called prior to using the state group.
  ///
  /// This function always has to be called after setting up a new state group or after modifying an existing state group.
  /// It computes a hash which is internally used for optimization purposes, and additionally generates platforms-specific
  /// data which is critical to the use of the state group.
  inline void ComputeHash()
  {
    m_iHash = ComputeHashInternal();
  }

  /// \brief
  ///   Invalidates all states (for internal use)
  inline void Invalidate()
  {
    ::InvalidateStates(this, sizeof(VStateGroupDepthStencilBase));
  }

  inline bool IsHashValid() const
  {
    return m_iHash == ComputeHashInternal();
  }

  /// \brief
  ///   Helper function to dump the current states into a text file in a readable format
  VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;
  
  /// \brief
  ///   Binary file serialization
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

private:
  inline unsigned int ComputeHashInternal() const
  {
    return ::ComputeHash(this, offsetof(VStateGroupDepthStencilBase, m_iHash));
  }

public:
  unsigned char m_bDepthTestEnabled;            ///< true to enable depth test, false to disable it
  unsigned char m_bDepthWriteEnabled;           ///< true to enable depth write, false to disable it
  unsigned char m_cDepthComparisonFunc;         ///< Depth comparison function. One of the members of the VComparisonFunc enumeration.
  unsigned char m_bStencilTestEnabled;          ///< true to enable the stencil test, false to disable it
  unsigned int m_iStencilReadMask;              ///< Stencil read mask
  unsigned int m_iStencilWriteMask;             ///< Stencil write mask
  unsigned int m_iStencilRef;                   ///< Stencil test reference value
  unsigned char m_cStencilFailOp[2];            ///< Stencil operation to perform when a stencil test fails. One of the members of the VStencilOp enumeration.
  unsigned char m_cStencilDepthFailOp[2];       ///< Stencil operation to perform when a depth test fails. One of the members of the VStencilOp enumeration.
  unsigned char m_cStencilPassOp[2];            ///< Stencil operation to perform when a stencil test passes. One of the members of the VStencilOp enumeration.
  unsigned char m_cStencilComparisonFunc[2];    ///< Comparison function for the stencil test. One of the members of the VComparisonFunc enumeration.
  unsigned char m_bTwoSidedStencil;             ///< Enable/disable two-sided stencil tests. If true, both values in the stencil op and comparison function arrays have to be set up (0 for front-facing geometry, 1 for back-facing geometry)
  unsigned int m_iHash;
};




/// \brief
///   State group that controls the rasterization properties
///
/// Note that not all states which are part of this state group are actually supported by all platforms. Please
/// refer to each platform's graphics documentation for information on which render states the hardware and graphics API
/// of that platform support.
class VStateGroupRasterizerBase
{

  public:

  /// \brief
  ///   Constructor; Sets defaults
  inline VStateGroupRasterizerBase() 
  {
    memset(this, 0, sizeof(VStateGroupRasterizerBase)); ///< turn off everything by default
    m_cFillMode = FILL_SOLID;
    m_cCullMode = CULL_BACK;
    m_cFlags = VSHADER_RASTERIZERSTATEBIT_MULTISAMPLE;
  }

  /// \brief
  ///   Assignment operator
  inline VStateGroupRasterizerBase& operator = (const VStateGroupRasterizerBase& other)
  {
    memcpy(this, &other, sizeof(VStateGroupRasterizerBase));
    return *this;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator == (const VStateGroupRasterizerBase& other) const
  {
    if (m_iHash != other.m_iHash)
      return false;
    else
      return memcmp(this, &other,sizeof(VStateGroupRasterizerBase))==0;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator != (const VStateGroupRasterizerBase &other) const
  {
    return !(*this == other);
  }  

  /// \brief
  ///   Cleans up internally allocated data. 
  inline void Cleanup()
  {
  }

  /// \brief
  ///   Computes the hash value for this state group. This function has to be called prior to using the state group.
  ///
  /// This function always has to be called after setting up a new state group or after modifying an existing state group.
  /// It computes a hash which is internally used for optimization purposes, and additionally generates platforms-specific
  /// data which is critical to the use of the state group.
  inline void ComputeHash()
  {
    m_iHash = ComputeHashInternal();
  }

  /// \brief
  ///   Invalidates all states (for internal use)
  inline void Invalidate()
  {
    ::InvalidateStates(this, sizeof(VStateGroupRasterizerBase));
  }

  inline bool IsHashValid() const
  {
    return m_iHash == ComputeHashInternal();
  }

  /// \brief
  ///   Helper function to dump the current states into a text file in a readable format
  VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;

  /// \brief
  ///   Binary file serialization
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

private:
  inline unsigned int ComputeHashInternal() const
  {
    return ::ComputeHash(this, offsetof(VStateGroupRasterizerBase, m_iHash));
  }

public:
  unsigned char m_cFillMode;            ///< Fill mode. One of the members of the VFillMode enumeration.
  unsigned char m_cCullMode;            ///< Cull mode. One of the members of the VCullMode enumeration.
  float m_fDepthBias;                   ///< Depth Bias
  float m_fDepthBiasClamp;              ///< Depth Bias Clamp
  float m_fSlopeScaledDepthBias;        ///< Slope-scaled Depth Bias
  unsigned char m_cFlags;               ///< Bits: 1=FrontCounterClockwise, 2=DepthClipDisable, 4=ScissorEnable, 8=MultisamplingEnable
  unsigned int m_iHash;
};


#if defined(_VR_DX11)

// Special code for DX11 state groups (creating state objects etc.)
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupsDX11.inl>

typedef VStateGroupBlendDX11 VStateGroupBlend;
typedef VStateGroupDepthStencilDX11 VStateGroupDepthStencil;
typedef VStateGroupRasterizerDX11 VStateGroupRasterizer;

#elif defined(_VR_GX2)

// Special code for GX2 state groups (creating state objects etc.)
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupsGX2.inl>

typedef VStateGroupBlendGX2 VStateGroupBlend;
typedef VStateGroupDepthStencilGX2 VStateGroupDepthStencil;
typedef VStateGroupRasterizerGX2 VStateGroupRasterizer;

#else
typedef VStateGroupBlendBase VStateGroupBlend;
typedef VStateGroupDepthStencilBase VStateGroupDepthStencil;
typedef VStateGroupRasterizerBase VStateGroupRasterizer;
#endif


/// \brief
///   State group that controls the tessellation states
///
/// Note that not all states which are part of this state group are actually supported by all platforms. Please
/// refer to each platform's graphics documentation for information on which render states the hardware and graphics API
/// of that platform support.
struct VStateGroupTesselation
{

  /// \brief
  ///   Constructor; Sets defaults
  inline VStateGroupTesselation()
  {
    memset(this, 0, sizeof(VStateGroupTesselation));
    m_cTesselationMode = TESSELATION_DISCRETE;
    m_fMinTesselationLevel = 0.f;
    m_fMaxTesselationLevel = 0.f;
  }

  /// \brief
  ///   Assignment operator
  inline VStateGroupTesselation &operator = (const VStateGroupTesselation& other)
  {
    memcpy(this, &other, sizeof(VStateGroupTesselation));
    return *this;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator == (const VStateGroupTesselation &other) const
  {
    if (m_iHash!=other.m_iHash)
      return false;
    else
      return memcmp(this, &other, sizeof(VStateGroupTesselation))==0;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator != (const VStateGroupTesselation &other) const
  {
    return !(*this == other);
  }  

  /// \brief
  ///   Cleans up internally allocated data. 
  inline void Cleanup()
  {
  }

  /// \brief
  ///   Computes the hash value for this state group. This function has to be called prior to using the state group.
  ///
  /// This function always has to be called after setting up a new state group or after modifying an existing state group.
  /// It computes a hash which is internally used for optimization purposes, and additionally generates platforms-specific
  /// data which is critical to the use of the state group.
  inline void ComputeHash() 
  {
    m_iHash=0;
    m_iHash=::ComputeHash(this, sizeof(VStateGroupTesselation));
  }

  /// \brief
  ///   Invalidates all states (for internal use)
  inline void Invalidate() 
  {
    ::InvalidateStates(this, sizeof(VStateGroupTesselation));
  }

  /// \brief
  ///   Helper function to dump the current states into a text file in a readable format
  VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;

  /// \brief
  ///   Binary file serialization
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  unsigned char m_bTesselationEnabled;            ///< Enable/disable tessellation
  unsigned char m_cTesselationMode;               ///< Tessellation mode. One of the members of the VTesselationMode enumeration.
  float m_fMinTesselationLevel;                   ///< Minimum tessellation level
  float m_fMaxTesselationLevel;                   ///< Maximum tessellation level
  unsigned int m_iHash;
};




/// \brief
///   State group that defines texture types for a sampler stage. In a shader, there is a 1:1
///   mapping between VStateGroupTexture and VStateGroupSampler
///
/// Note that not all states which are part of this state group are actually supported by all platforms. Please
/// refer to each platform's graphics documentation for information on which render states the hardware and graphics API
/// of that platform support.
struct VStateGroupTexture
{

  /// \brief
  ///   Constructor; Sets defaults
  inline VStateGroupTexture() 
  {
    m_cTextureType=TEXTURETYPE_BASE_TEXTURE;
    m_iTextureIndex=0;
  }

  /// \brief
  ///   Assignment operator
  inline VStateGroupTexture &operator = (const VStateGroupTexture &other)
  {
    m_cTextureType = other.m_cTextureType;
    m_iTextureIndex = other.m_iTextureIndex;
    m_spCustomTex = other.m_spCustomTex;
    return *this;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator == (const VStateGroupTexture &other) const
  {
    if (m_cTextureType!=other.m_cTextureType) return false;
    if (m_iTextureIndex!=other.m_iTextureIndex) return false;
    if (m_spCustomTex!=other.m_spCustomTex) return false;
    return true;
  }

  /// \brief
  ///   Comparison operator
  inline bool operator != (const VStateGroupTexture &other) const
  {
    return !(*this == other);
  }  


  /// \brief
  ///   Helper function to dump the current states into a text file in a readable format
  VBASE_IMPEXP void DumpToFile(IVFileOutStream *pOut) const;

  /// \brief
  ///   Binary file serialization
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file);

  unsigned char m_cTextureType;               ///< Texture type. One of the members of the VTextureType enumeration.
  unsigned int m_iTextureIndex;               ///< Texture index. For lightmaps, this references the lightmap index (0 = diffuse lightmap, 1-3 = dot3 lightmaps)
  VTextureObjectPtr m_spCustomTex;            ///< For texture types that require a custom texture, this member has to point to the texture to use.
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
