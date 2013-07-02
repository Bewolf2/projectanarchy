/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_PLAYER_CHARACTER_SYNC_GROUP_H__
#define RPG_PLUGIN_PLAYER_CHARACTER_SYNC_GROUP_H__

#include "Engine/IVisApiNetworkManager.hpp"

class RPG_PlayerCharacterSynchronizationGroup : public IVNetworkSynchronizationGroup
{
public:
  /// \brief
  ///   Data history for position and rotation
  class VHistoryData : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VHistoryData() : m_iChangedDataFlags(0), m_iDataFlagsToSend(0)
    {
    }

    VisDataHistory_cl<hkvVec3, 3> m_positionHistory;
    VisDataHistory_cl<hkvVec3, 3, VLerpEulerAngles> m_rotationHistory;
    BYTE m_iChangedDataFlags;
    BYTE m_iDataFlagsToSend;
  };

  /// \brief
  ///   Flags defining which data is being sent
  enum VChangedDataFlags
  {
    VCD_POSITION = V_BIT(0),
    VCD_ROTATION = V_BIT(1),
    VCD_DataFlags = V_BIT(0) | V_BIT(1),
  };

  VOVERRIDE const char *GetName() const {return "RPG_PlayerCharacter";}
  VOVERRIDE VGroupTypeFlags_e GetUsageTypeFlags() const {return VGroupTypeBit_Transformation;}
  VOVERRIDE IVNetworkSynchronizationGroupInstanceData* CreatePerInstanceData() const {return (new VHistoryData);}
  VOVERRIDE bool QuerySynchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VMessageSettings& out_paketSettings);
  VOVERRIDE void Synchronize(const VNetworkViewContext& context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, VArchive &ar);
  VOVERRIDE void TickFunction(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t &instanceInfo, float fTimeDelta);

  static RPG_PlayerCharacterSynchronizationGroup g_Instance;
};

#endif // RPG_PLUGIN_PLAYER_CHARACTER_SYNC_GROUP_H__

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
