/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 04 : Custom Entities
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// How to create a custom entity class
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial04/Tutorial04PCH.h>


V_IMPLEMENT_SERIAL( MyEntity_cl, VisBaseEntity_cl /*parent class*/ , 0, &g_MyModule );


MyEntity_cl::MyEntity_cl()
{
  RotateSpeed = 10.f;
}

MyEntity_cl::~MyEntity_cl()
{
}



//Called when the entity is created
void MyEntity_cl::InitFunction()
{
}

//Called every frame to update the simulation
void MyEntity_cl::ThinkFunction()
{
  float fTime = Vision::GetTimer()->GetTimeDifference();  // find out how much time has passed since last time
  IncOrientation( hkvVec3 ( RotateSpeed * fTime,0,0) ); // update this entity's rotation
}

// *** The variable table is used by the plugin system of the engine and editor. 
START_VAR_TABLE(MyEntity_cl,VisBaseEntity_cl,"Rotating entity",0, NULL )
  DEFINE_VAR_FLOAT(MyEntity_cl, RotateSpeed, "Rotation speed (degrees per second)", "10.0", 0, 0);
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
