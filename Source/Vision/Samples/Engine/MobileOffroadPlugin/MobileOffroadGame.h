/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MOBILEOFFROAD_GAME_H
#define MOBILEOFFROAD_GAME_H

class MobileOffRoadGame : public IVisCallbackHandler_cl, public VisBaseEntity_cl
{
public:
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  // called when plugin is loaded/unloaded
  void OneTimeInit();
  void OneTimeDeInit();

  // switch to play-the-game mode. When not in vForge, this is default
  void SetPlayTheGame(bool bStatus);

  void RecordBodyState();
  void RestoreBodyState();
  void ClearBodyState();

  // update the game
  void Update();

  // global instance
  static MobileOffRoadGame& GetInstance() { return s_instance; }

private:
  static MobileOffRoadGame s_instance;

  VInputMap *m_inputMap;

  bool m_playingTheGame;

  struct BodyRecordState
  {
    hkpRigidBody* b;
    hkTransform transform;
    hkVector4 linVel;
    hkVector4 angVel;
  };
  hkArray<BodyRecordState> m_recordedState;
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
