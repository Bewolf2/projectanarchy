/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// GameState_cl Class
//***********************************************************************

#ifndef _GameState_cl_HEADER_
#define _GameState_cl_HEADER_

enum GAME_STATE
{
  GAME_STATE_RUN,
  GAME_STATE_RESTART
};

//
// *** GameState_cl  ***
//
// Synopsis:
//    - Entity class that holds the game state information
//
class GameState_cl : public VisBaseEntity_cl
{
public:
  // is called upon entity initialization
  virtual void InitFunction();

  // is called upon entity de-initialization
  virtual void DeInitFunction();

  // is called each frame
  virtual void ThinkFunction();

  virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  void SetCurrentState(GAME_STATE eState);

  GAME_STATE GetCurrentState() const;

private:
  // private variables
  GAME_STATE m_eState;
  

V_DECLARE_SERIAL( GameState_cl, )
IMPLEMENT_OBJ_CLASS(GameState_cl);
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
