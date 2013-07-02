/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file EntityLODComponent.hpp

#ifndef ENTITYLODCOMPONENT_HPP_INCLUDED
#define ENTITYLODCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>

/// \brief
///   Enum that represents commonly used LOD level indices
enum LODLevel_e
{
  LOD_NONE      = -2,

  // switches LOD by distance
  LOD_AUTO      = -1,

  // the following are pre-defined, but there can be an arbitrary number
  // However, 0 is always the highest level.
  LOD_HIGH      = 0,
  LOD_MEDIUM    = 1,
  LOD_LOW       = 2,
  LOD_ULTRALOW  = 3,
  // [...]
};


/// \brief
///   Class that represents information about one LOD level. The EntityLODComponent class has an
///   array of it.
class VLODLevelInfo
{
public:
  VLODLevelInfo()
  {
    m_fSwitchLevel = 0.f;
  }

  /// \brief
  ///   Associates a model file with this level
  bool SetModelFile(const char *szFilename, bool bAppyMotionDelta=false);

  /// \brief
  ///   Sets a distance at which the level switches to next level. So the LOD level will be visible
  ///   in the range [this->m_fSwitchLevel .. next->m_fSwitchLevel]
  inline void SetSwitchDistance(float fDist)
  {
    m_fSwitchLevel = fDist;
  }

  /// \brief
  ///   Extracts relevant information from the node
  inline void ParseXMLNode(TiXmlElement *pLevelNode)
  {
    VASSERT(pLevelNode);
    const char *szFilename = XMLHelper::Exchange_String(pLevelNode,"model",NULL,false);
    SetModelFile(szFilename);
    XMLHelper::Exchange_Float(pLevelNode,"distance",m_fSwitchLevel,false);
  }

  VDynamicMeshPtr m_spMesh;
  VisAnimConfigPtr m_spAnimConfig;

  float m_fSwitchLevel;
  VSmartPtr<VisAnimFinalSkeletalResult_cl> m_spFinalSkeletalResult;
};



/// \brief
///   Component that handles the LOD changes of an entity. For implementation, please refer to the
///   engine sample
class EntityLODComponent
{
public:

  /// \brief
  ///   Constructor
  EntityLODComponent()
  {
    m_iCurrentLevel = LOD_NONE;
    m_iLevelCount = 0;
    m_pLevels = NULL;
    m_pOwner = NULL;
    m_bAutoLOD = false;
	 m_hysteresisZoneHalfWidth = 0.0f;
  }

  /// \brief
  ///   Destructor
  ~EntityLODComponent()
  {
    V_SAFE_DELETE_ARRAY(m_pLevels);
  }

  /// \brief
  ///   Associates this component with an entity. Should for instance be called in the entity's
  ///   constructor.
  inline void Init(VisBaseEntity_cl *pOwner)
  {
    m_pOwner = pOwner;
  }

  bool LoadFromFile(const char *szFilename);
  bool ParseXMLNode(TiXmlElement *pLODNode);

  /// \brief
  ///   Allocates iCount LOD levels
  inline void AllocateLevels(int iCount)
  {
    if (m_iLevelCount == iCount)
      return;
    V_SAFE_DELETE_ARRAY(m_pLevels);
    m_iLevelCount = iCount;
    if (iCount>0)
      m_pLevels = new VLODLevelInfo[iCount];

  }

  /// \brief
  ///   Accesses the LOD level info of specified index, where the index must be in the allocated
  ///   range
  inline VLODLevelInfo &GetLevelInfo(int iIndex)
  {
    VASSERT(iIndex>=0 && iIndex<m_iLevelCount);
    return m_pLevels[iIndex];
  }

  /// \brief
  ///   Updates the skeletal animation root node on all animation configs
  void SetSkeletalAnimRootNode(IVisAnimResultGenerator_cl *pRoot)
  {
    for (int i=0;i<m_iLevelCount;i++)
      m_pLevels[i].m_spFinalSkeletalResult->SetSkeletalAnimInput(pRoot);
  }

  /// \brief
  /// Sets a custom animation config on the conmponent.
  void SetAnimConfigs( VisAnimConfigPtr* arrAnimConfig, int lodsCount );


  /// \brief
  ///   Forwards the new frozen state to all animation configs involved
  void SetFrozen(bool bNewState)
  {
    for (int i=0;i<m_iLevelCount;i++)
      m_pLevels[i].m_spAnimConfig->SetFrozen(bNewState);
  }

  /// \brief
  ///   Sets a LOD index. Can be LOD_AUTO to detect according to distance in the Update() function
  void SetLODLevel(LODLevel_e newLevel);

  /// \brief
  ///   Returns the current LOD level, which is always >=0
  inline int GetCurrentLODLevel() const {return m_iCurrentLevel;}

  /// \brief
  ///   Evaluates the current distance of the owner's entity to the camera
  inline float GetDistanceToCamera() const {return (m_pOwner->GetPosition()-Vision::Camera.GetCurrentCameraPosition()).getLength();}

  /// \brief
  ///   Returns (but does not set) the LOD level determined by the passed distance
  int ComputeLODLevel(float fDistance) const;


  /// \brief
  ///   Update function that has to be called once a frame, e.g. in the entity's ThinkFunction
  inline void Update()
  {
    if (m_bAutoLOD && m_pOwner->WasVisibleInAnyLastFrame())
      UpdateLODLevel(ComputeLODLevel(GetDistanceToCamera()));
  }

protected:
  void UpdateLODLevel(int newLevel);

public:
  float m_hysteresisZoneHalfWidth;

  VisBaseEntity_cl *m_pOwner;
  int m_iCurrentLevel;
  bool m_bAutoLOD;

  int m_iLevelCount;
  VLODLevelInfo *m_pLevels;
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
