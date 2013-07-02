/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// Ball_cl Class
//***********************************************************************

#ifndef _Ball_cl_HEADER_
#define _Ball_cl_HEADER_

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>

// defines
#define BALL_MASS 10000.0f

//
// *** Ball_cl  ***
//
// Synopsis:
//    - Entity class that holds the ball
//
class Ball_cl : public VisBaseEntity_cl
{
public:
  // constructor
  Ball_cl();

  // destructor
  ~Ball_cl();

  // is called upon entity initialization
  virtual void InitFunction();

  // is called upon entity de-initialization
  virtual void DeInitFunction();

  // is called each frame
  virtual void ThinkFunction();

  void ApplyForce(hkvVec3 &vForce, float deltaT);

  void SetPosition(const hkvVec3 &vPosition);

private:
  // private variables
  vHavokRigidBody *m_pRigidBody;
  

V_DECLARE_SERIAL( Ball_cl, )
IMPLEMENT_OBJ_CLASS(Ball_cl);
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
