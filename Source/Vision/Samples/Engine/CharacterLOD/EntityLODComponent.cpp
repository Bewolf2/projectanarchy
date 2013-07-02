/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>
#include <Vision/Samples/Engine/CharacterLOD/EntityLODComponent.hpp>



bool VLODLevelInfo::SetModelFile(const char *szFilename, bool bAppyMotionDelta)
{
  m_spMesh = Vision::Game.LoadDynamicMesh(szFilename, true);
  m_spAnimConfig = NULL;
  if (!m_spMesh)
    return false;

  // create a new final result for this config
  VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
  m_spAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(m_spMesh, &pFinalSkeletalResult);
  if (!bAppyMotionDelta)
    m_spAnimConfig->SetFlags(m_spAnimConfig->GetFlags() & (~APPLY_MOTION_DELTA));

  m_spFinalSkeletalResult = pFinalSkeletalResult;
  return true;
}





bool EntityLODComponent::LoadFromFile(const char *szFilename)
{
  TiXmlDocument doc;
  if (!doc.LoadFile(szFilename,Vision::File.GetManager()))
    return false;

  return ParseXMLNode(XMLHelper::SubNode(doc.RootElement(),"LOD",false));
}


bool EntityLODComponent::ParseXMLNode(TiXmlElement *pLODNode)
{
  if (!pLODNode)
    return false;

  // first count the number of elements
  TiXmlElement* pElement;

  // load constraints
  TiXmlNode *pFirstNode = pLODNode->FirstChild("LODLevel");
  if (!pFirstNode)
    return false;

  int iCount = 0;
  // iterate through all sub-nodes and count them
	for (pElement=pFirstNode->ToElement(); pElement!=NULL; pElement=pElement->NextSiblingElement("LODLevel"))
	  iCount++;
  AllocateLevels(iCount);

  // iterate again and parse
  iCount = 0;
  for (pElement=pFirstNode->ToElement(); pElement!=NULL; pElement=pElement->NextSiblingElement("LODLevel"))
    GetLevelInfo(iCount++).ParseXMLNode(pElement);


  return true;
}

void EntityLODComponent::SetAnimConfigs( VisAnimConfigPtr* arrAnimConfig, int lodsCount )
{
	for ( int i = 0; i < lodsCount; ++i )
	{
		VLODLevelInfo& lodInfo = GetLevelInfo( i );
		if ( lodInfo.m_spMesh != NULL )
		{
			// create a new final result for this config
			lodInfo.m_spAnimConfig = arrAnimConfig[i];
			lodInfo.m_spFinalSkeletalResult = arrAnimConfig[i]->GetFinalResult();
		}

		if ( i == m_iCurrentLevel )
		{
			// make the animation play
			m_pOwner->SetMesh( lodInfo.m_spMesh );
			m_pOwner->SetAnimConfig( lodInfo.m_spAnimConfig );
		}
	}
}

void EntityLODComponent::SetLODLevel(LODLevel_e newLevel)
{
  m_bAutoLOD = newLevel==LOD_AUTO;
  if(!m_bAutoLOD)
    UpdateLODLevel(newLevel);
}

  
void EntityLODComponent::UpdateLODLevel(int newLevel)
{
  if (m_iCurrentLevel==newLevel)
  {
    return;
  }

  // change the mesh and the animation config used
  int oldLevel = m_iCurrentLevel;
  VLODLevelInfo &newLODInfo = GetLevelInfo(newLevel);
  m_iCurrentLevel=newLevel;
  m_pOwner->SetMesh(newLODInfo.m_spMesh);
  m_pOwner->SetAnimConfig(newLODInfo.m_spAnimConfig);

  // synchronize animation tracks of both LODs
  if (oldLevel>=0 && oldLevel<m_iLevelCount)
  {
	  VLODLevelInfo &oldLODInfo = GetLevelInfo(oldLevel);
	  IVisAnimResultGenerator_cl* oldLODAnimInput = oldLODInfo.m_spAnimConfig->GetFinalResult()->GetSkeletalAnimInput();

	  float currAnimTime = 0.0f;
	  if ( oldLODAnimInput->IsOfType( V_RUNTIME_CLASS(VisSkeletalAnimControl_cl) ) )
	  {
        VisSkeletalAnimControl_cl* oldLODAanimController = static_cast< VisSkeletalAnimControl_cl* >( oldLODAnimInput );
	     currAnimTime = oldLODAanimController->GetCurrentSequenceTime();
	  }

	  IVisAnimResultGenerator_cl* newLODAnimInput = newLODInfo.m_spAnimConfig->GetFinalResult()->GetSkeletalAnimInput();
	  if ( newLODAnimInput->IsOfType( V_RUNTIME_CLASS(VisSkeletalAnimControl_cl) ) )
	  {
	     VisSkeletalAnimControl_cl* newLODAanimController = static_cast< VisSkeletalAnimControl_cl* >( newLODAnimInput );
	     newLODAanimController->SetCurrentSequenceTime( currAnimTime );
	  }
  }
}


int EntityLODComponent::ComputeLODLevel(float fDistance) const
{
	if ( m_hysteresisZoneHalfWidth <= 0.0f )
	{
		for (int i=1;i<m_iLevelCount;i++)
			if (m_pLevels[i].m_fSwitchLevel>fDistance)
				return i-1;

		return m_iLevelCount-1;
	}
	else
	{
		int iNewLevel = m_iLevelCount-1;
		for (int i=1;i<m_iLevelCount;i++)
		{
			if (m_pLevels[i].m_fSwitchLevel>fDistance)
			{
				iNewLevel = i-1;
				break;
			}
		}

		if (m_iCurrentLevel < iNewLevel && ( fDistance - m_pLevels[m_iCurrentLevel].m_fSwitchLevel ) <= m_hysteresisZoneHalfWidth )
		{
			// we're not out of the hysteresis zone yet to switch the LOD
			iNewLevel = m_iCurrentLevel;
		}
		else if (m_iCurrentLevel > iNewLevel && ( m_pLevels[m_iCurrentLevel].m_fSwitchLevel - fDistance ) <= m_hysteresisZoneHalfWidth )
		{
			// we're not out of the hysteresis zone yet to switch the LOD
			iNewLevel = m_iCurrentLevel;
		}
		return iNewLevel;
	}
}

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
