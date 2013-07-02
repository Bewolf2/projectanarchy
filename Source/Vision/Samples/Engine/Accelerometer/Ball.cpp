/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************
// Ball_cl Source
//***********************************************************************

#include <Vision/Samples/Engine/Accelerometer/AccelerometerPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Samples/Engine/Accelerometer/Ball.hpp>

Ball_cl::Ball_cl()
{
  TriggerBoxEntity_cl::AddObservedEntity(this);
}

Ball_cl::~Ball_cl()
{
  TriggerBoxEntity_cl::RemoveObservedEntity(this);
}

// InitFunction: Is called upon entity initialization.
void Ball_cl::InitFunction()
{
  // create a rigid body and attach it to the entity
  m_pRigidBody = new vHavokRigidBody();
  m_pRigidBody->Initialize();
  vHavokRigidBody::InitTemplate initTempl;
  float fScale = 1.0f;
  hkvVec3 vScale(fScale,fScale,fScale);
  m_pRigidBody->InitConvexRb(GetMesh(),vScale,initTempl);
  AddComponent(m_pRigidBody);
}

// DeInitFunction:: Is called upon entity de-initialization.
void Ball_cl::DeInitFunction()
{
}

// ThinkFunction: Is called once per frame.
void Ball_cl::ThinkFunction()
{
}

void Ball_cl::ApplyForce(hkvVec3 &vForce, float deltaT)
{
  m_pRigidBody->ApplyForce(vForce, deltaT);
}

void Ball_cl::SetPosition(const hkvVec3 &vPosition)
{
  m_pRigidBody->SetPosition(vPosition);
}

V_IMPLEMENT_SERIAL( Ball_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(Ball_cl, VisBaseEntity_cl, "Ball_cl", 0, "")
END_VAR_TABLE

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
