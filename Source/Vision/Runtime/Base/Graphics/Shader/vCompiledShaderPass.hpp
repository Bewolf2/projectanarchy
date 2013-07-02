/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vCompiledShaderPass.hpp

#ifndef VCOMPILEDSHADERPASS_HPP_INCLUDED
#define VCOMPILEDSHADERPASS_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vConstantBuffer.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VVideo.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroupSampler.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vStateGroups.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vShaderDeclarations.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vRenderStateContainer.hpp>
#include <Vision/Runtime/Base/System/VNameValueListParser.hpp>

struct VStateGroupTexture;

/// \brief
///   Class that represents the compiled version of an individual shader pass.
/// 
/// VCompiledShaderPass keeps all state information for a render pass, including state groups, shaders,
/// shader constants, vertex stream setup, and texture setup. 
/// 
/// VCompiledShaderPass represents the base class of a compiled shader pass. You can derive
/// custom classes from it to provide additional functionality. For instance, to implement a
/// tick function that modifies this shader once per game tick, a custom shader class could additionally
/// implement the IVisCallbackHandler_cl interface and listen to the
/// Vision::Callbacks.OnUpdateSceneBegin callback.
/// 
/// Custom shader classes are automatically created by the shader system using the type manager. The
/// custom class name can be specified in the shader editor.
class VCompiledShaderPass : public VTypedObject, public VRefCounter, public VUserDataObj
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; Sets defaults
  VBASE_IMPEXP VCompiledShaderPass();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VCompiledShaderPass();

  /// \brief
  ///   Non-virtual override of the release function. In case refcounter reaches 1 and the force unique flag is set, we delete the shader from the cache
  VBASE_IMPEXP void Release();

  /// \brief
  ///   Clears the entire state of the object.
  VBASE_IMPEXP void Clear();

  ///
  /// @}
  ///


  ///
  /// @name Accessing Textures and Samplers
  /// @{
  ///
  
  /// \brief
  ///   Returns the number of active samplers for the specified shader stage.
  inline unsigned int GetActiveSamplerCount(VShaderStage_e eShaderStage) const
  {
    VASSERT(eShaderStage > VSS_Invalid && eShaderStage < VSS_MaxStageCount);
    return m_uiNumActiveSamplers[eShaderStage];
  }

  /// \brief
  ///   Returns the state group sampler for the specified shader stage and index. If sampler could not be retrieved,
  ///   it will return NULL.
  inline VStateGroupSampler* GetStateGroupSampler(VShaderStage_e eShaderStage, unsigned int iIndex) const
  {
    VASSERT(eShaderStage > VSS_Invalid && eShaderStage < VSS_MaxStageCount);
    return (m_pStateGroupSamplers[eShaderStage] != NULL && iIndex < m_uiNumActiveSamplers[eShaderStage]) ? &m_pStateGroupSamplers[eShaderStage][iIndex] : NULL;
  }

  /// \brief
  ///   Returns the state group texture for the specified shader stage and index. If texture could not be retrieved,
  ///   it will return NULL.
  inline VStateGroupTexture* GetStateGroupTexture(VShaderStage_e eShaderStage, unsigned int iIndex) const
  {
    VASSERT(eShaderStage > VSS_Invalid && eShaderStage < VSS_MaxStageCount);
    return (m_pStateGroupTextures[eShaderStage] != NULL && iIndex < m_uiNumActiveSamplers[eShaderStage]) ? &m_pStateGroupTextures[eShaderStage][iIndex] : NULL;
  }

  ///
  /// @}
  ///


  ///
  /// @name Accessing Constant Buffers
  /// @{
  ///

  /// \brief
  ///   Accesses one of the shader-specific constant buffers
  /// 
  /// The passed buffer index can either be VSS_VertexShader, VSS_PixelShader, 
  /// VSS_GeometryShader (DX10, DX11) or VSS_HullShader/VSS_DomainShader/VSS_ComputeShader (DX11).
  /// 
  /// The returned buffer provides a range of shader registers that are used by this shader
  /// instance.
  /// 
  /// The buffer object can be used to modify registers in the buffer. The constants are uploaded to the
  /// graphics card at shader bind time.
  /// 
  /// However, only constants inside the valid range are supported. For instance, if a shader uses
  /// vertex shader registers c65-c80, only these constants can be modified in the vertex shader
  /// constant buffer.
  /// 
  /// \param Stage
  ///   The following constants are currently supported:
  ///   \li VSS_VertexShader : access the vertex shader constant table
  ///   \li VSS_PixelShader : access the pixel shader constant table
  ///   \li VSS_GeometryShader : access the geometry shader constant table (DX10/11 only)
  ///   \li VSS_HullShader : access the hull shader constant table (DX11 only)
  ///   \li VSS_DomainShader : access the domain shader constant table (DX11 only)
  ///   \li VSS_ComputeShader : access the compute shader constant table (DX11 only)
  /// 
  /// \return
  ///   VShaderConstantBuffer* pBuffer: A contant buffer that provides the range of constants used
  ///   by this shader
  /// 
  /// \sa VShaderConstantBuffer
  VBASE_IMPEXP VShaderConstantBuffer* GetConstantBuffer (VShaderStage_e Stage);

  /// \brief
  ///   Helper function to get the texture sampler index associated with the passed sampler name
  /// 
  /// The name has to match a sampler name that is used in the vertex/pixel shader. If no sampler with the specified name exists in this shader,
  /// this function returns -1.
  /// 
  /// The index can be used for a sampler lookup in the shader's m_pStateGroupSamplers and m_pStateGroupTextures
  /// arrays.
  /// 
  /// To look up a sampler index independently for vertex-, geometry- or pixel shader, you can use
  /// VCompiledShaderPass::GetConstantBuffer(VSS_VertexShader)->GetSamplerIndexByName(szSamplerName)
  /// instead.
  ///
  /// \param Stage
  ///   The shader stage in which to query the sampler name.
  /// 
  /// \param szSamplerName
  ///   Name of the sampler to find. The comparison is case-insensitive.
  /// 
  /// \return
  ///   int iSamplerIndex: index of the sampler (or -1)
  VBASE_IMPEXP int GetSamplerIndexByName(VShaderStage_e Stage, const char *szSamplerName) const;

  ///
  /// @}
  ///

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
  ///   Virtual overridable that is called before the shader is compiled
  /// 
  /// Custom shader classes can implement this function to initialize custom variables right before
  /// the platform-specific shader object is created..
  /// 
  /// \param pSourceFX
  ///   The effect resource that is compiled. This resource owns the effect parameter table.
  /// 
  /// \param pSourceShader
  ///   The shader resource that is compiled. This resource owns the shader source code.
  VBASE_IMPEXP virtual void PreCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader) {}

  /// \brief
  ///   Virtual overridable that is called after the shader has been successfully compiled
  /// 
  /// Custom shader classes can implement this function to initialize custom variables after
  /// the platform-specific shader object has been created.
  /// 
  /// For instance, frequently used shader constant register indices can be looked up inside this
  /// function and stored in custom shader members. 
  /// 
  /// See e.g. VFloatConstantBuffer::GetRegisterByName.
  /// 
  /// \param pSourceFX
  ///   The effect resource that has been used to compile this shader. This resource owns the
  ///   effect parameter table.
  /// 
  /// \param pSourceShader
  ///   The shader resource that has been used to compile this shader. This resource owns the
  ///   shader source code.
  /// 
  /// \sa VFloatConstantBuffer
  /// \sa VFloatConstantBuffer::GetRegisterByName
  VBASE_IMPEXP virtual void PostCompileFunction(VShaderEffectResource *pSourceFX, VShaderPassResource *pSourceShader);


  /// \brief
  ///   Virtual overridable that is called for every parameter in the parameter string that
  ///   modifies this shader
  /// 
  /// The default implementation of this function maps the parameter to both vertex and pixel
  /// shader constant table. 
  /// 
  /// Custom implementations can use this function to map a parameter to any custom member, such as
  /// light source keys etc.
  /// 
  /// \param szVarname
  ///   The name of the parameter, e.g. "LightColor"
  /// 
  /// \param szValueStr
  ///   The value string. Vector components are separated by ','. The default implementation parses
  ///   this string and maps it to the constant buffer.
  /// 
  /// \return
  ///   bool bResult: true if updating this parameter was successful
  /// 
  /// \sa VFloatConstantBuffer
  /// \sa VFloatConstantBuffer::GetRegisterByName
  VBASE_IMPEXP virtual bool UpdateShaderParam(const char *szVarname, const char *szValueStr);

  ///
  /// @}
  ///

  ///
  /// @name Parameter String Helpers
  /// @{
  ///

  /// \brief
  ///   Gets called at compile time. Loops through all parameters in the effect and calls
  ///   UpdateShaderParam on each.
  VBASE_IMPEXP void MapDefaultParameterFromEffect(VShaderEffectResource *pEffectRes);

  /// \brief
  ///   Gets called at compile time. Parses the parameter string and calls UpdateShaderParam on
  ///   each "param=value" pair.
  VBASE_IMPEXP void ParseParameterString(VShaderPassResource *pSourceShader, const char *szParamStr);

  /// \brief
  ///   Gets called at compile time. Evaluates the register range for the constant buffers and clamps them to the range valid on the current platform.
  VBASE_IMPEXP void ClampConstantBufferRanges();

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Assignment operator; copies data from the passed compiled shader pass
  VBASE_IMPEXP VCompiledShaderPass &operator = (const VCompiledShaderPass &other);

  /// \brief
  ///   Comparison operator. Not virtual. Thus for custom comparison functions use the IsEqual
  ///   function
  VBASE_IMPEXP bool operator == (const VCompiledShaderPass &other) const;

  /// \brief
  ///   Comparison operator. Not virtual. Thus for custom comparison functions use the IsEqual
  ///   function
  bool operator != (const VCompiledShaderPass &other) const {return !(*this == other);}

  /// \brief
  ///   Creates a clone of this shader pass instance
  /// 
  /// The base implementation creates a class instance of the same type using the type manager.
  /// Then it copies all states of the VCompiledShaderPass class.
  /// 
  /// Custom shader classes can override this function to copy their own relevant members
  /// 
  /// \return
  ///   VCompiledShaderPass *pNewInstance: A new shader instance of the same class as this
  VBASE_IMPEXP virtual VCompiledShaderPass *Clone() const;

  /// \brief
  ///   Overridable to compare two shader instances
  /// 
  /// The base implementation of this function calls the '==' operator that in turn compares the
  /// class types and the base members.
  /// 
  /// Custom shader classes can override this function to compare their own relevant members
  /// 
  /// \param other
  ///   Other instance to compare
  /// 
  /// \return
  ///   bool bEqual: Comparison result
  VBASE_IMPEXP virtual bool IsEqual(const VCompiledShaderPass &other) const {return (*this)==other;}

  ///
  /// @}
  ///

  ///
  /// @name Internal Functions
  /// @{
  ///

  /// \brief
  ///   Compute the state hashes of all state group members
  VBASE_IMPEXP void ComputeStateHashes();

  /// \brief
  ///   Internal function; allocates samplers (m_pStateGroupSamplers and m_pStateGroupTextures[VSS_PixelShader] array)
  VBASE_IMPEXP void SetActiveSamplerCount (VShaderStage_e Stage, int iCount);

  /// \brief
  ///   Internal function to validate ranges according to device caps (e.g. anisotropy)
  VBASE_IMPEXP void ClampValuesToValidRange();

  /// \brief
  ///   Copies over all properties of the shader pass resource to the compiled shader pass.
  VBASE_IMPEXP void CopyPropertiesFromResource (VShaderPassResource &resource, short** out_pSamplerMapping);

  /// \brief
  ///   Copies over all shader programs from the shader pass resource to the compiled shader pass.
  VBASE_IMPEXP bool CopyShadersFromResource (VShaderPassResource& resource, IVLog* pLog);

  /// \brief
  ///   Gather resource dependencies of this compiled shader pass.
  ///
  /// \param snapshot
  ///   Resource snapshot to store the dependencies in.
  ///
  /// \param pOwner
  ///   Owner for which to gather the dependencies.
  /// 
  /// To avoid confusion, this is not intended to be a virtual function overload of
  /// VTypedObject::GetDependencies.
  VBASE_IMPEXP void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner);

  /// \brief
  ///   Dummy overload of VTypedObject::GetDependencies to avoid unnecessary compiler warnings of
  ///   GetDependencies shadowing a virtual function of the base class with the same name.
  virtual void GetDependencies(VResourceSnapshot &snapshot)
  {
    VTypedObject::GetDependencies(snapshot);
  }

  /// \brief
  ///   Returns the Render State Container for this compiled shader pass. Creates it if it doesn't exist yet.
  VBASE_IMPEXP VRenderStateContainer* GetRenderState () const;

  // internal functions
  VBASE_IMPEXP void LoadDefaultSamplerTextures(IVTextureLoader &TextureLoader, const VShaderProgramResource &pSrc, 
    const short *pSamplerMapping, VShaderStage_e eShaderStage);

  VBASE_IMPEXP void SetSpecificTexturesFlags();


  VBASE_IMPEXP VCompiledShaderManager* GetManager (void) const 
  { 
    return (m_pOwnerManager); 
  }

  VBASE_IMPEXP void SetManager (VCompiledShaderManager* pManager) 
  { 
    m_pOwnerManager = pManager; 
  }

  VBASE_IMPEXP const void* GetShader (VShaderStage_e Shader);

  #ifdef SUPPORTS_TESSELATION_SHADER
    VBASE_IMPEXP int GetNumHullShaderControlPoints (void) const 
    { 
      return (m_iNumHSControlPoints); 
    }
  #endif

  VBASE_IMPEXP void DisablePixelShader(void);

  inline VShaderModel_e GetShaderModel (VShaderStage_e Shader) const 
  { 
    return (m_ShaderModel[Shader]); 
  }

  /// \brief Returns the vertex-stream mask.
  inline unsigned int GetStreamMask (void) const 
  { 
    VASSERT(m_uiStreamMask != 0); 
    return m_uiStreamMask; 
  }

#if defined(_VR_GLES2)

  inline unsigned int GetProgramHandle() const
  { 
    return m_uiGLES2Program; 
  }  
  
  inline unsigned int GetProgramHandleSkinning() const
  { 
    return m_uiGLES2ProgramSkinning; 
  }

#endif

  inline void UpdatePrograms()
  {
    CopyShadersFromResource(*m_spOrigin.GetPtr(), NULL);
  }

private:
    // Private functions
#if defined(_VR_GLES2)

  // Function to create a program handle (if required).
  void CreateProgramHandle();
  void DestroyProgramHandle();

#endif

public:
  // Public member variables

  // compiled vertex and pixel shader
  VVertexShaderObjectPtr m_spVertexShader;                ///< Vertex Shader for this shader pass
  VVertexShaderObjectPtr m_spSkinningVertexShader;        ///< Skinning vertex shader for this shader pass (used to render skinned objects)
  VPixelShaderObjectPtr m_spPixelShader;                  ///< Pixel Shader for this shader pass
#if defined(SUPPORTS_GEOMETRY_SHADER)
  VGeometryShaderObjectPtr m_spGeometryShader;            ///< Geometry Shader for this shader pass (DX10 and up only)
#endif
#if defined(SUPPORTS_TESSELATION_SHADER)
  VHullShaderObjectPtr m_spHullShader;                    ///< Hull Shader for this shader pass (DX11 and up only)
  VDomainShaderObjectPtr m_spDomainShader;                ///< Domain Shader for this shader pass (DX11 and up only)
  int m_iNumHSControlPoints;                              ///< Number of hull shader control points (DX11 and up only)
#endif
#if defined(SUPPORTS_COMPUTE_SHADER)
  VComputeShaderObjectPtr m_spComputeShader;              ///< Compute Shader for this shader pass (DX11 and up only)
#endif

  unsigned char m_cStateGroupMask;                        ///< The state group mask of a shader defines which state groups (e.g. rasterizer state, blend state) should be set when enabling the shader pass. By excluding certains state groups, it becomes possible to override them using the functions in the VisRenderStates_cl class.
  bool m_bModified;                                       ///< Set this to true when modifying a shader pass. This will indicate to the engine that the shader will have to be set again even if it is already active.
  bool m_bNoPrimitiveSpecificTextures;                    ///< Performance optimization hint: Set this to true to indicate that the shader does not contain any primitive-specific textures (e.g. lightmap).
  bool m_bNoSurfaceSpecificTextures;                      ///< Performance optimization hint: Set this to true to indicate that the shader does not contain any surface-specific textures (e.g. base texture, surface normal map, etc.)
  bool m_bForceUnique;                                    ///< If set, never return an existing shader when registering this shader; always make sure this shader pass is unique.
  bool m_bMayDeleteUnique;
  bool m_bNeedsToPassTextureSize;                         ///< If set, this shader needs to automatically fill texture sizes into one or more constant registers
  VISSHADERCALLBACKFUNC m_pCallback;                      ///< Callback function that can be set for a shader.
  
  VPassType_e m_PassType;                                 ///< This is set per technique and only mirrored here

  #if defined (_VISION_XENON) || defined (_VR_DX11)
    /// Note: Just a copy of a pointer, never deallocate this!
    const int* m_pByteCodeBuffer[VSS_PlatformStageCount];
    int  m_iByteCodeBufferLen[VSS_PlatformStageCount];
  #endif

  VCompiledEffect *m_pCompiledEffect;                     ///< Compiled effect this shader pass belongs to
  VShaderPassResourcePtr m_spOrigin;                      ///< Shader Pass Resource this shader pass has been created from

  ///
  /// @}
  ///

private:
  // Private member variables
  mutable VRenderStateContainerPtr m_spRenderStateContainer;

  unsigned int m_uiNumActiveSamplers[VSS_PlatformStageCount];         ///< List of active sampler numbers
  VStateGroupSampler* m_pStateGroupSamplers[VSS_PlatformStageCount];  ///< List of sampler state groups in this shader pass
  VStateGroupTexture* m_pStateGroupTextures[VSS_PlatformStageCount];  ///< List of texture state groups in this shader pass

  VShaderConstantBuffer m_ConstBuffer[VSS_PlatformStageCount]; ///< Shader Constants for all Shader Stages. The Skinning-Shader Constants are not used, but instead the Vertex-Shader Constants are used.

  VCompiledShaderManager* m_pOwnerManager;

  // TODO: Can this be shared between Shaders???
  VShaderModel_e m_ShaderModel[VSS_PlatformStageCount];
  unsigned int m_uiStreamMask;

#if defined(_VR_GLES2)
  unsigned int m_uiGLES2Program;
  unsigned int m_uiGLES2ProgramSkinning;
#endif

#if defined( _VR_GLES2 )
  void CompileShaders_Internal();
#endif
private:
  void GetTextureSizeRegisterHelper(VShaderStage_e eStage);

public:
  //type management
  V_DECLARE_DYNCREATE_DLLEXP( VCompiledShaderPass,  VBASE_IMPEXP )
};

typedef VNameValueListParser<';', '=', 1024> VShaderParameterParser;

#include <Vision/Runtime/Base/Graphics/Shader/vConstantBufferRegister.hpp>

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
