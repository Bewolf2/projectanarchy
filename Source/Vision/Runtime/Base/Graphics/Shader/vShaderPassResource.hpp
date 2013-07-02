/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vShaderPassResource.hpp

#ifndef VSHADERPASSRESOURCE_HPP_INCLUDED
#define VSHADERPASSRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderProgramResource.hpp>


/// \brief
///   Represents the uncompiled version of a shader pass.
/// 
/// A shader resource holds the source code for vertex and pixel shaders and prototypes of all
/// relevant state groups and texture samplers.
class VShaderPassResource : public VRefCounter, public VUserDataObj
{
public:

  /// \brief Enum to describe the file version of a stored VShaderPassResource
  enum VShaderPassResourceVersion_e
  {
    VSPR_VERSION_1 = 1,
    VSPR_VERSION_2 = 2,
    VSPR_VERSION_3 = 3,
    VSPR_VERSION_4 = 4,   ///< (8.2.0): Auto-Generated Shaders / Techniques
    VSPR_VERSION_5 = 5,   ///< (8.2.0): Shared Shader States
    VSPR_VERSION_6 = 6,   ///< (8.2.0): Shader Source Hashes to prevent recompilation
    VSPR_VERSION_7 = 7,   ///< (8.2.0): Shader Source Hashes for the remaining shader types
    VSPR_VERSION_8 = 8,   ///< (8.2.0): Stream-Mask moved back into the Compiled Shader
    VSPR_VERSION_9 = 9,   ///< (2012.1) : Shaders can now have an individual compiled class
    VSPR_VERSION_CURRENT = VSPR_VERSION_9,
  };

  ///
  /// @name Constructor/destructor
  /// @{
  ///

public:

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VShaderPassResource(VShaderEffectLib *pOwnerLib);

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VShaderPassResource();

  ///
  /// @}
  ///

  ///
  /// @name Accessing properties of the Shader Pass Resource
  /// @{
  ///

  /// \brief
  ///   Returns the compiled shader program object for the passed shader stage.
  ///
  /// Cast the result to VVertexShaderObject*, VPixelShaderObject*, VGeometryShaderObject*, 
  /// VHullShaderObject*, VDomainShaderObject* or VComputeShaderObject*, depending on the requested shader stage.
  ///
  /// \param eShaderStage
  ///   The shader stage of the shader.
  ///
  /// \param eOutDstModel
  ///   The shader model to compile for.
  ///
  /// \param pLog
  ///   Optional log object.
  VBASE_IMPEXP void* GetCachedShader (VShaderStage_e eShaderStage, VShaderModel_e& eOutDstModel, IVLog *pLog = NULL);

  /// \brief
  ///   Releases all the cached shader objects.
  VBASE_IMPEXP void FreeCacheShaders ();

  /// \brief
  ///   Returns the state container holding all the state groups for this shader pass resource.
  VBASE_IMPEXP VRenderStateContainer* GetRenderState () const;

  /// \brief 
  ///   Returns a bit-mask of or'ed VTargetPlatformMask_e values that represents which platforms are supported.
  VBASE_IMPEXP unsigned int GetSupportedTargetPlatforms (void) const { return (m_uiSupportedTargetPlatforms); }

  /// \brief 
  ///   Returns whether a certain platform is included in the target platform bitmask.
  VBASE_IMPEXP bool IsSupportedTargetPlatform (VTargetPlatform_e Platform) const { return ((m_uiSupportedTargetPlatforms & (1 << Platform)) != 0); }

  /// \brief 
  ///   Sets a bit-mask of or'ed VTargetPlatformMask_e values that represents which platforms are supported.
  VBASE_IMPEXP void SetSupportedTargetPlatforms (unsigned int platforms) { m_uiSupportedTargetPlatforms = platforms; }

  /// \brief 
  /// Adds a platform to the list of supported platforms.
  VBASE_IMPEXP void AddSupportedTargetPlatform (VTargetPlatform_e Platform) { m_uiSupportedTargetPlatforms |= (1 << Platform); }

  /// \brief 
  ///   Removes a platform from the list of supported platforms.
  VBASE_IMPEXP void RemoveSupportedTargetPlatform (VTargetPlatform_e Platform) { m_uiSupportedTargetPlatforms &= ~(1 << Platform); }

  /// \brief
  ///   Returns the parent shader library this shader belongs to.
  VBASE_IMPEXP VShaderEffectLib* GetOwnerEffectLib () const {return m_pOwnerLib;}

  /// \brief
  ///   Returns the name of this shader pass.
  VBASE_IMPEXP const VString& GetName (void) const { return (m_sName); }

  /// \brief
  ///   Sets the name of this shader pass.
  VBASE_IMPEXP void SetName (const char* szName) { m_sName = szName; }

  /// \brief
  ///   Returns the description of this shader pass.
  VBASE_IMPEXP const VString& GetDescription (void) const { return (m_sDescription); }

  /// \brief
  ///   Sets the name of this shader pass.
  VBASE_IMPEXP void SetDescription (const char* szDesc) { m_sDescription = szDesc; }

  /// \brief
  ///   Allows read-access to the underlying Shader source
  VBASE_IMPEXP const VShaderProgramResource& GetShaderProgram (VShaderStage_e Shader) const { return (m_ShaderProgram[Shader]); }

  /// \brief
  ///   Returns whether this shader was automatically generated from a shader permutation.
  VBASE_IMPEXP bool IsAutoGenerated (void) const { return (m_bIsAutoGenerated); }

  /// \brief
  ///   Sets a flag indicating that this shader was auto-generated by the permutation system.
  VBASE_IMPEXP void SetIsAutoGenerated (bool b) { m_bIsAutoGenerated = b; }

  /// \brief
  ///   Sets the shader class name for this shader pass
  VBASE_IMPEXP void SetCompiledClassName(const char* pName) { m_sCompiledClassName = pName; }

  /// \brief
  ///   Returns the shader class name for this shader pass
  VBASE_IMPEXP const char* GetCompiledClassName() const { return m_sCompiledClassName.IsEmpty() ? NULL : m_sCompiledClassName.GetSafeStr(); }

  /// \brief
  ///   Returns whether this shader contains any permutation variables and thus should not be used directly (but as a source for generating shader permutations)
  VBASE_IMPEXP bool IsUsingPermutationVariables (void) const { return (m_bShaderUsesPermutationVariables); }

  /// \brief
  ///   Sets the flag whether this shader uses permutation variables.
  VBASE_IMPEXP void SetIsUsingPermutationVariables (bool b) { m_bShaderUsesPermutationVariables = b; }

  /// \brief
  ///   Returns the name of the per-primitive callback function.
  VBASE_IMPEXP const VString& GetPerPrimitiveCallbackName (void) const { return (m_sPerPrimitiveCallback); }

  /// \brief
  ///   Sets the name of the per-primitive callback function.
  VBASE_IMPEXP void SetPerPrimitiveCallbackName (const char* szName) { m_sPerPrimitiveCallback = szName; }

  /// \brief
  ///   Returns the flags used for compiling the shader. The returned flags are enumerated in VShaderCompileFlags_e.
  VBASE_IMPEXP unsigned int GetCompileFlags (void) const { return (m_uiCompileFlags); }

  /// \brief
  ///   Sets the flags used for compiling the shader. The  flags are enumerated in VShaderCompileFlags_e.
  VBASE_IMPEXP void SetCompileFlags (unsigned int flags) { m_uiCompileFlags = flags; }

  /// \brief
  ///   Returns number of control points the hull shader uses.
  VBASE_IMPEXP unsigned int GetNumHullShaderControlPoints (void) const { return (m_iNumHSControlPoints); }

  /// \brief
  ///   Sets the number of control points the hull shader uses.
  VBASE_IMPEXP void SetNumHullShaderControlPoints (unsigned int points) { m_iNumHSControlPoints = points; }

  /// \brief
  ///   Returns the hash of the shader source.
  VBASE_IMPEXP unsigned int GetShaderHash (VTargetPlatform_e Platform, VShaderStage_e Shader) const { return (m_uiCompiledShaderSourceHash[Shader][Platform]); }

  /// \brief
  ///   Sets the hash of the shader source.
  VBASE_IMPEXP void SetShaderHash (VTargetPlatform_e Platform, VShaderStage_e Shader, unsigned int uiHash) { m_uiCompiledShaderSourceHash[Shader][Platform] = uiHash; }

  /// \brief Returns the vertex stream mask.
  unsigned int GetStreamMask (void) const { return (m_uiStreamMask); }

  /// \brief Sets the vertex stream mask.
  void SetStreamMask (unsigned int ui) { m_uiStreamMask = ui; }

  /// \brief Sets the platforms on which the shader needs to be recompiled (due to configuration changes).
  void SetRequiresRecompilationFlags (unsigned int uiPlatformMask = TARGETPLATFORM_MASK_ALL) { m_uiRequiresRecompilation = uiPlatformMask; }

  /// \brief Returns the platforms on which the shader needs to be recompiled (due to configuration changes).
  bool IsRecompilationRequired (VTargetPlatform_e Platform) const { return ((m_uiRequiresRecompilation & (1 << Platform)) != 0); }

  /// \brief Clears the flag indicating whether the shader needs to be recompiled for one platform. Called after successful shader compilation.
  void ResetRequiresRecompilationFlag (VTargetPlatform_e Platform) { m_uiRequiresRecompilation &= ~(1 << Platform); }

  /// \brief Returns the mask on which platform a shader needs to be recompiled.
  unsigned int GetRequiresRecompilationFlags (void) const { return (m_uiRequiresRecompilation); }

  /// \brief 
  ///   Returns the desired Shader Program pointer as a void* pointer.
  void* GetCachedShaderPointer (VShaderStage_e Shader) const;

  ///
  /// @}
  ///

  ///
  /// @name Loading / Saving
  /// @{
  ///

  /// \brief
  ///   Internal function for binary file data exchange (load and save).
  VBASE_IMPEXP void ChunkFileExchange(VChunkFile &file, VTargetPlatform_e targetPlatform, char iLocalVersion = VShaderPassResource::VSPR_VERSION_CURRENT, bool bByteCodeOnly=false);

  ///
  /// @}
  ///


  ///
  /// @name Internal methods
  /// @{
  ///

  /// \brief
  ///   Allows write-access to the underlying Shader source. For internal use.
  VBASE_IMPEXP VShaderProgramResource& GetShaderProgram_WriteAccess (VShaderStage_e Shader) { return (m_ShaderProgram[Shader]); }

  #ifdef WIN32
    /// \brief Used internally while saving.
    int m_iShaderNum;
    bool m_bIsReferenced;
  #endif

  /// \brief
  ///   Internal function
  VBASE_IMPEXP void OnDetachFromLib();

  ///
  /// @}
  ///

private:

  // Member variables
  VShaderEffectLib* m_pOwnerLib;          ///< Library that "owns" (contains) this shader pass resource
  VString m_sName;                        ///< Name of the shader pass resource
  VString m_sDescription;                 ///< Description of the shader pass resource

  bool m_bIsAutoGenerated;                ///< Set to true for shaders that are generated through the permutation system
  bool m_bShaderUsesPermutationVariables; ///< Set to true for shaders that use $ifdef etc.
  unsigned int m_uiStreamMask;            ///< Input stream mask for the vertex shader
  VString m_sCompiledClassName;           ///< Per pass overwrite for the compiled shader class name

  unsigned int m_uiCompiledShaderSourceHash[VSS_MaxStageCount][TARGETPLATFORM_COUNT];        ///< For every platform we store the shader code hash with which it was compiled.
  unsigned int m_uiCompileFlags;  ///< Flags used in the shader compiler (VShaderCompileFlags_e)
  int m_iNumHSControlPoints;      ///< DX11 specific: Number of tessellation control points in HS stage
  unsigned int m_uiSupportedTargetPlatforms;  ///< Each bit represents a VTargetPlatformMask_e value
  VString m_sPerPrimitiveCallback;    ///< callback function name
  
  #ifdef WIN32
   friend bool ParseFXLib (char *text, int numChars, VShaderCompileFlags_e flags, VShaderEffectLib &fxlib);
  #endif
  
  mutable VRenderStateContainerPtr m_spRenderStateContainer;      ///< The container that stores all render-states.
  VShaderProgramResource m_ShaderProgram[VSS_MaxStageCount];      ///< Source of each shader stage.

  // All the different shader types that are supported.
  VVertexShaderObjectPtr      m_spCachedSkinningShader;
  VVertexShaderObjectPtr      m_spCachedVertexShader;
  VPixelShaderObjectPtr       m_spCachedPixelShader;
  #ifdef _VR_DX11
    VGeometryShaderObjectPtr  m_spCachedGeometryShader;
    VHullShaderObjectPtr      m_spCachedHullShader;
    VDomainShaderObjectPtr    m_spCachedDomainShader;
    VComputeShaderObjectPtr   m_spCachedComputeShader;
  #endif

  unsigned int m_uiRequiresRecompilation;

  #ifdef _VR_GLES2
    // we need to have access to CreatePixelShader_Internal() on OpenGL ES in order to perform lazy shader compilation
    friend class VCompiledShaderPass;
  #endif
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
