/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vCompiledEffect.hpp

#ifndef VCOMPILEDEFFECT_HPP_INCLUDED
#define VCOMPILEDEFFECT_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vCompiledShaderPass.hpp>
#include <Vision/Runtime/Base/Graphics/Shader/vTechniqueConfig.hpp>


// Forwards
class VShaderEffectLib;
class VShaderEffectTechnique;


/// \brief
///   VCompiledTechnique represents a shader technique, typically consisting of one or multiple VCompiledShaderPass instances.
/// 
/// VCompiledTechnique and its VCompiledShaderPass instances can be passed to many rendering functions. Techniques are primarily
/// containers for shader passes and can contain multiple shader passes for multi-pass rendering. VCompiledTechnique instances
/// are generated from VShaderEffectTechnique objects and are specific for one parameter setup (in other words, there is a separate
/// VCompiledTechnique for each combination of shader parameters and VShaderEffectTechnique).
/// 
/// The VCompiledEffect class contains multiple VCompiledTechnique objects. The shader technique to be used for rendering is chosen
/// through a set of inclusion and exclusion tags.
class VCompiledTechnique : public VRefCounter, public VUserDataObj
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor. For internal use.
  inline VCompiledTechnique(VCompiledEffect *pOwnerFX, const char *szInclusionTags, const char *szExclusionTags) 
    : m_pOwnerFX(pOwnerFX)
    , m_Shaders()
    , m_Config(szInclusionTags,szExclusionTags)
    , m_ePassType(VPT_Undefined)
  {
  }

  /// \brief
  ///   Constructor. For internal use.
  inline VCompiledTechnique(VCompiledEffect *pOwnerFX, VTechniqueConfig *pConfig, 
    int iPassCount=0, VCompiledShaderPass **pPasses=NULL)
  {
    m_pOwnerFX = pOwnerFX;
    if (pConfig)
      m_Config = *pConfig; ///< use assignment operator

    m_Shaders.EnsureCapacity(iPassCount);
    for (int i=0;i<iPassCount;i++)
    {
      pPasses[i]->m_bMayDeleteUnique = true;
      m_Shaders.Add(pPasses[i]);
    }
    EvaluatePassTypeFromPasses();
  }

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VCompiledTechnique();

  /// \brief
  ///   Non-virtual override of the release function. In case refcounter reaches 1 and the force unique flag is set, the technique is deleted from the cache
  VBASE_IMPEXP void Release();

  ///
  /// @}
  ///

  ///
  /// @name Accessing Shader Passes
  /// @{
  ///


  /// \brief
  ///   Returns the number of shader passes in this technique
  /// 
  /// This number can be used to iterate over the shader passes using GetShader(i)
  inline int GetShaderCount() const {return m_Shaders.Count();}

  /// \brief
  ///   Returns a pointer to a list of pointers to all shader pass objects in this technique.
  inline VCompiledShaderPass **GetShaderList() {return m_Shaders.GetPtrs();}

  /// \brief
  ///   Returns the n-th shader pass instance in this technique
  /// 
  /// The index is 0-based and must be in valid range [0..GetShaderCount()-1]
  /// 
  /// \param iIndex
  ///   0-based index
  /// 
  /// \return
  ///   VCompiledShaderPass *pPass : Instance that represents the shader pass with index iIndex
  inline VCompiledShaderPass *GetShader(int iIndex) const {return m_Shaders.GetAt(iIndex);}

  /// \brief
  ///   Sets the state group mask for all passes.
  ///
  /// The state group mask of a shader defines which state groups (e.g. rasterizer state, blend state) should be set when
  /// enabling the shader pass. By excluding certains state groups, it becomes possible to override them using the functions in
  /// the VisRenderStates_cl class.
  ///
  /// \param iStateGroupMask
  ///   The state group mask to set.
  VBASE_IMPEXP void SetStateGroupMaskForAllPasses(int iStateGroupMask);

  ///
  /// @}
  ///

  ///
  /// @name Accessing Effect and Library
  /// @{
  ///

  /// \brief
  ///   Access the effect resource that has been used to create this compiled technique
  VBASE_IMPEXP VShaderEffectResource *GetSourceEffect() const;

  /// \brief
  ///   Access the effect library resource that has been used to create this compiled technique
  VBASE_IMPEXP VShaderEffectLib *GetOwnerEffectLib() const;

  /// \brief
  ///   Returns the parameter string that has been used to create this technique. Same as the parameter string of the source effect. 
  VBASE_IMPEXP const char *GetParameterString() const;

  ///
  /// @}
  ///

  ///
  /// @name Internal Functions
  /// @{
  ///

  /// \brief
  ///   Loops through all shaders and evaluates the common pass type bitmask.
  VBASE_IMPEXP void EvaluatePassTypeFromPasses();

  /// \brief
  ///   Returns all resources this compiled technique depends on.
  VBASE_IMPEXP void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner);

  ///
  /// @}
  ///

public:
  VCompiledEffect *m_pOwnerFX;
  VRefCountedCollection<VCompiledShaderPass> m_Shaders;
  VTechniqueConfig m_Config;
  VPassType_e m_ePassType;
};



/// \brief
///   VCompiledEffect has a collection of shader techniques and provides a mechanism of choosing a specific technique using inclusion and exclusion tags.
/// 
/// A VCompiledEffect is created from a VShaderEffectResource (just like a VCompiledTechnique is created from a VShaderEffectTechnique). While VCompiledEffect
/// may, for instance, be used to represent a specific material type (e.g. wood), you might need different ways to render this material type under different
/// conditions (e.g. using directional lighting, using lightmaps, using a point light, etc.). Each of these conditions could be represented as a separate
/// VCompiledTechnique inside the effect, each using different inclusion tags. At runtime, inclusion tags could be used to identify the appropriate technique
/// and render the scene geometry with it.
class VCompiledEffect : public VRefCounter, public VUserDataObj
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor. For internal use.
  VBASE_IMPEXP VCompiledEffect(VShaderEffectResource* pSrcFX, VCompiledShaderManager &instanceManager, 
    const char *szParamStr, int iCreationFlags);

  /// \brief
  ///  Destructor
  VBASE_IMPEXP virtual ~VCompiledEffect();

  ///
  /// @}
  ///

  ///
  /// @name Access Compiled Techniques
  /// @{
  ///
  
  /// \brief
  ///   Find a compiled technique inside this effect that matches the specified configuration
  /// 
  /// This function loops through all compiled techniques in this effect and compares the
  /// inclusion/exclusion flags provided in the passed structure with each technique's flags.
  /// 
  /// The first matching technique is returned.
  /// 
  /// This feature is useful for providing variants of the same effect (e.g. with fog/without fog) and
  /// accessing these variants at runtime.
  /// 
  /// \param pConfig
  ///   The config that is used to test compatibility
  /// 
  /// \param pGlobalConfig
  ///   Optional second config, can be NULL. If specified, both configs must be compatible.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique : The first technique that has a compatible config or NULL
  VBASE_IMPEXP VCompiledTechnique *FindCompatibleTechnique(const VTechniqueConfig *pConfig, const VTechniqueConfig *pGlobalConfig=NULL);

  /// \brief
  ///   Returns the first valid technique in this effect. Commonly used for effects that do not require variants (i.e., only have one technique).
  VBASE_IMPEXP VCompiledTechnique *GetDefaultTechnique();

  /// \brief
  ///   Gather relevant resource information that pOwner relies on
  VBASE_IMPEXP void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner);

  /// \brief
  ///   Returns the source effect resource object.
  inline VShaderEffectResource *GetSourceEffect() const
  {
    return m_pSourceEffect;
  }

  /// \brief
  ///   Returns the parameter string.
  inline const char* GetParameterString() const
  {
    return m_sParamStr;
  }

  /// \brief
  ///   Static helper function to concatenate multiple parameter strings into one, removing duplicates and whitespaces
  ///
  /// This funciton generates a new parameter string, that contains every unique parameter name once. If contained more
  /// than once inside one string or in distinct strings, the latter value will be used
  ///
  /// \param szTarget
  ///   The target string buffer. Must hold enough bytes for the the sum of all szParamList strings
  ///
  /// \param szParamList
  ///   The list of parameter string pointers (iListCount elements)
  ///
  /// \param iListCount
  ///   The number of list elements in szParamList
  VBASE_IMPEXP static void ConcatAndNormalizeParameterString(char *szTarget, const char **szParamList, int iListCount);

  /// \brief
  ///   Helper function to parse a single parameter
  ///
  /// \param szParamStr
  ///   The parameter string.
  ///
  /// \param param
  ///   Parsed result.
  ///
  /// \return The remaining parameter string (or NULL if done).
  VBASE_IMPEXP const char *ParseNextParameter(const char *szParamStr, VShaderParam &param) const;

  ///
  /// @}
  ///

  ///
  /// @name Internal methods
  /// @{
  ///

  /// \brief
  ///   Internal function
  VBASE_IMPEXP void RemoveCachedVersion(VCompiledTechnique *pTechnique);

  /// \brief
  ///   Internal function
  VBASE_IMPEXP VCompiledTechnique *CompileTechnique(VShaderEffectTechnique *pTechnique, IVLog *pLog=NULL);

  /// \brief
  ///   Internal function
  VBASE_IMPEXP void RemoveEffectResourceReference();

  ///
  /// @}
  ///

public:
  VCompiledShaderManager &m_InstanceManager;    ///< reference used to create shader instances
  int m_iCachedTechniqueCount;                  ///< same as m_pSourceEffect->m_Techniques.Count();
  VCompiledTechniquePtr *m_spCachedTechniques;  ///< array of size m_iCachedTechniqueCount. Might contain NULL pointers 
  VShaderEffectResourcePtr m_pSourceEffect;     ///< points to the owner resource
  VString m_sParamStr;                          ///< parameter string that was used to create this effect instance
  int m_iCreationFlags;                         ///< flags used to create this compiled version
  bool m_bDestroying;                           ///< internal flag
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
