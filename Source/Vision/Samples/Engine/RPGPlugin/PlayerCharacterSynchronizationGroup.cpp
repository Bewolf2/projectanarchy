/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>
#if 0
#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacterSynchronizationGroup.h>

#include <Vision/Samples/Engine/RPGPlugin/PlayerCharacter.h>

RPG_PlayerCharacterSynchronizationGroup RPG_PlayerCharacterSynchronizationGroup::g_Instance;

bool RPG_PlayerCharacterSynchronizationGroup::QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings)
{
  RPG_PlayerCharacter *pObj = (RPG_PlayerCharacter*) instanceInfo.m_pInstance;
  VHistoryData* pData = (VHistoryData*) instanceInfo.m_pCustomData;

  // Save previously changed data, will be compared against the current changes.
  BYTE iPreviousChangedDataFlags = pData->m_iChangedDataFlags;
  // Add data flags, will be removed if following extrapolation tests succeed.
  pData->m_iChangedDataFlags |= VCD_DataFlags;

  hkvVec3 vInterpolatedEntry(false);
  float fEpsilon = 0.5f * Vision::World.GetGlobalUnitScaling();
  // Extrapolate current position
  pData->m_positionHistory.Interpolate (&vInterpolatedEntry, context.m_iCurrentServerTimeMS);
  const hkvVec3 vCurrentPos = pObj->GetPosition(); 
  if (vCurrentPos.Equals(vInterpolatedEntry, fEpsilon))
    pData->m_iChangedDataFlags &= ~VCD_POSITION; // Remove position flag

  // Extrapolate current rotation
  pData->m_rotationHistory.Interpolate (&vInterpolatedEntry, context.m_iCurrentServerTimeMS);
  const hkvVec3 vCurrentOrientation = pObj->GetOrientation();
  if (vCurrentOrientation.Equals(vInterpolatedEntry, fEpsilon))
    pData->m_iChangedDataFlags &= ~VCD_ROTATION; // Remove rotation flag


  // Change from previous tick, send changes reliably
  if ((iPreviousChangedDataFlags^pData->m_iChangedDataFlags) != 0)
  {
    pData->m_iDataFlagsToSend = (iPreviousChangedDataFlags^pData->m_iChangedDataFlags) | pData->m_iChangedDataFlags;
    out_paketSettings.SetReliability (VMR_Reliable_Ordered);
    out_paketSettings.SetPriority (VMP_HighPriority);
    out_paketSettings.SetOrderingChannel (1);
    return true;
  }
  // No change from previous tick
  else
  {
    // Is there anything to send?
    if ((pData->m_iChangedDataFlags & VCD_DataFlags) != 0)
    {
      // Continuous changes are sent unreliably
      pData->m_iDataFlagsToSend = pData->m_iChangedDataFlags;
      out_paketSettings.SetReliability (VMR_Unreliable_Sequenced);
      out_paketSettings.SetPriority (VMP_MediumPriority);
      out_paketSettings.SetOrderingChannel (1);
      return true;
    }
    else
    {
      return false;
    }
  }
}

void RPG_PlayerCharacterSynchronizationGroup::Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar)
{
  RPG_PlayerCharacter *pObj = (RPG_PlayerCharacter*) instanceInfo.m_pInstance;
  VHistoryData* pData = (VHistoryData*) instanceInfo.m_pCustomData;

  if (ar.IsLoading())
  {
    // Get timing
    __int64 iTimeMS;
    ar >> iTimeMS; //8 byte
    ar >> pData->m_iDataFlagsToSend; // 1 byte

    //Vision::Error.Warning("Server time: %.5f", iTimeMS / 1000.0f);


    if (pData->m_iDataFlagsToSend & VCD_POSITION)
    {
      float fX, fY, fZ;

      ar >> fX; //4 byte
      ar >> fY; //4 byte
      ar >> fZ; //4 byte
      hkvVec3 vNewPosition (fX, fY, fZ);
      pData->m_positionHistory.Write (vNewPosition, iTimeMS);
    }

    if (pData->m_iDataFlagsToSend & VCD_ROTATION)
    {
      VFloat16 x, y, z;

      ar >> x; //2 byte
      ar >> y; //2 byte
      ar >> z; //2 byte
      hkvVec3 vNewRotation (x.ToFloat(), y.ToFloat(), z.ToFloat());
      pData->m_rotationHistory.Write (vNewRotation, iTimeMS);
    }
  }
  else
  { 
    __int64 iTimeMS = context.m_iCurrentServerTimeMS;

    //Vision::Error.Warning("Server time: %.5f", iTimeMS / 1000.0f);

    ar << iTimeMS;
    ar << pData->m_iDataFlagsToSend;

    if (pData->m_iDataFlagsToSend & VCD_POSITION)
    {
      ar << pObj->GetPosition().x;
      ar << pObj->GetPosition().y;
      ar << pObj->GetPosition().z;
      // Data is also written while sending, to compare against in the next QuerySynchronize call.
      pData->m_positionHistory.Write (pObj->GetPosition(), iTimeMS);
    }

    if (pData->m_iDataFlagsToSend & VCD_ROTATION)
    {
      ar << VFloat16 (pObj->GetOrientation().x);
      ar << VFloat16 (pObj->GetOrientation().y);
      ar << VFloat16 (pObj->GetOrientation().z);
      // Data is also written while sending, to compare against in the next QuerySynchronize call.
      pData->m_rotationHistory.Write (pObj->GetOrientation(), iTimeMS);
    }
  }
}

void RPG_PlayerCharacterSynchronizationGroup::TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta)
{
  if (context.m_eNetType == VNT_Client)
  {
    VSystemGUID iOwnerID = instanceInfo.m_pComponent->GetNetworkOwnerID();

    // Only interpolate and set the values, if this is not a player controlled entity
    if (iOwnerID != context.m_iUserID)
    {
      RPG_PlayerCharacter *pObj = (RPG_PlayerCharacter*) instanceInfo.m_pInstance;
      VHistoryData* pData = (VHistoryData*) instanceInfo.m_pCustomData;

      hkvVec3 vNewPosition(false);
      VReadResult_e res = pData->m_positionHistory.Interpolate (&vNewPosition, context.m_iCurrentServerTimeMS - context.m_iInterpolationDelayMS);
      if (res != VRR_None)
      {
        pObj->SetPosition (vNewPosition);
      }

      //VReadResult_e res;
      //const hkvVec3* vNewPosition = NULL;
      //const long long *time = NULL;
      //if( pData->m_positionHistory.Read(vNewPosition, time, 0))
      //{
      //	pObj->SetPosition(*vNewPosition);
      //}

      hkvVec3 vNewRotation(false);
      res = pData->m_rotationHistory.Interpolate (&vNewRotation, context.m_iCurrentServerTimeMS - context.m_iInterpolationDelayMS);
      if (res != VRR_None)
      {
        pObj->SetOrientation (vNewRotation);
      }
    }    
  }
}
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
