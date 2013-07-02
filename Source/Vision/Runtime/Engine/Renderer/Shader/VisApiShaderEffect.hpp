/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiShaderEffect.hpp

#ifndef _VISAPISHADEREFFECT_HPP_INCLUDED_
#define _VISAPISHADEREFFECT_HPP_INCLUDED_

class VChunkFile;

// bit masks for VisShaders_cl::CreateEffect function
#define EFFECTFLAGS_NONE          0x00000000
#define EFFECTFLAGS_FORMODELS     0x00000001
#define EFFECTFLAGS_FORCEUNIQUE   0x00000002


/// \brief
///   Helper class to store and serialize an effect assignment
/// 
/// Each instance of this class represents an effect assignment, e.g. effect assignment to a world
/// surface .
/// 
/// \sa VisShaders_cl
/// \sa VCompiledTechnique
class VisEffectConfig_cl
{
public:
  V_DECLARE_SERIALX( VisEffectConfig_cl, VISION_APIFUNC )


  /// \brief
  ///   Constructor
  VisEffectConfig_cl()
  {
  }

  /// \brief
  ///   Clears the effect config, i.e. removes the effect
  inline void Clear()
  {
    m_spEffect=NULL;
  }

  /// \brief
  ///   Indicates whether an effect is assigned
  inline bool HasEffects() const
  {
    return m_spEffect!=NULL;
  }

  /// \brief
  ///   Returns the number of effects in this config. Currently only 0 or 1 are possible return
  ///   values.
  inline int GetNumEffects() const
  {
    return (m_spEffect!=NULL)?1:0;
  }

  /// \brief
  ///   Returns the effect instance. Index must be 0 since only 1 effect can currently be assigned 
  inline VCompiledEffect* GetEffect(int index=0) const
  {
    return m_spEffect;
  }

  /// \brief
  ///   Adds an effect to this config. Currently the same as SetEffect.
  inline void AddEffect(VCompiledEffect* pFX)
  {
    SetEffect(pFX);
  }

  /// \brief
  ///   Sets the effect for this effect config
  inline void SetEffect(VCompiledEffect* pFX)
  {
    m_spEffect = pFX;
    if (pFX!=NULL)
    {
      m_spEffectRes = pFX->m_pSourceEffect;
      m_spShaderLib = (m_spEffectRes!=NULL) ? m_spEffectRes->m_pOwnerLib : NULL;
    } else
    {
      m_spEffectRes = NULL;
      m_spShaderLib = NULL;
    }
  }

  /// \brief
  ///   Copies the effect config from another config
  inline void CopyFrom(const VisEffectConfig_cl &other)
  {
    m_spEffect = other.m_spEffect;
    m_spEffectRes = other.m_spEffectRes;
    m_spShaderLib = other.m_spShaderLib;
  }

  /// \brief
  ///   Re-creates the shader effect in case the library has changed for instance. It uses the same effect lib, effect name, parameter string and creation flags as before
  VISION_APIFUNC void ReAssignEffect();

  /// \brief
  ///   Writes the effect config (name and parameter string) to a file stream
  VISION_APIFUNC BOOL WriteToStream(IVFileOutStream *pOutStream) const;

  /// \brief
  ///   Loads the effect config from file stream, i.e. load the effect name and param string and
  ///   tries to create an effect through API functions
  VISION_APIFUNC BOOL ReadFromStream(IVFileInStream *pInStream);

  /// \brief
  ///   Serializes an effect config, i.e. load/write effect name and param string
  VISION_APIFUNC void SerializeX(VArchive &ar);

  /// \brief
  ///   Serializes an effect config into a chunk file, i.e. load/write effect name and param string
  VISION_APIFUNC BOOL ChunkFileSerialize(VChunkFile &ar);

  /// \brief
  ///   Helper function to read the effect from or write to an XML node
  /// 
  /// In write mode, this function applies a "Shader" sub-node to the parent node
  /// 
  /// \param pParent
  ///   The parent XML node
  /// 
  /// \param bWrite
  ///   In write mode (bWrite=true), this effect is added to the parent node as a sub-node. If
  ///   bWrite=false, this function tries to read the effect from the sub-node.
  /// 
  /// \param szRelLibPath
  ///   If specified, tihs path is pre-pended to the library filename.
  /// 
  /// \return
  ///   BOOL bResult: TRUE, if writing (resp. reading was successful). In read mode, this function
  ///   returns FALSE if there is no "Shader" sub-node.
  VISION_APIFUNC BOOL XMLExchange(TiXmlElement *pParent, bool bWrite, const char *szRelLibPath);
private:
  VCompiledEffectPtr m_spEffect;
  VShaderEffectResourcePtr m_spEffectRes; // store the owner effect as well so we can retrieve the effect name
  VShaderEffectLibPtr m_spShaderLib;
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
