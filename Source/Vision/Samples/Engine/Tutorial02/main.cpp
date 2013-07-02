/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 02 : Entities
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows how to create and rotate an entity using code
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial02/Tutorial02PCH.h>

VisSampleAppPtr spApp;
VisBaseEntity_cl* pEntity = NULL;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // Create an application
  spApp = new VisSampleApp();
  
  // Init the application
  if (!spApp->InitSample("Maps\\ViewerMap" /*DataDir*/, "ViewerMap" /*SampleScene*/ ))
    return false;
  
  return true;
}
 
VISION_SAMPLEAPP_AFTER_LOADING
{
  // Create a mouse controlled camera
  spApp->EnableMouseCamera();

  // Create an entity of class VisBaseEntity_cl infront of the camera with a model
  pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3 (250,0,-120), "Models\\Warrior\\Warrior.MODEL");
  pEntity->SetCastShadows(true);
}

VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
  if (spApp->Run())
  {
    //Rotate the entity 20 degrees per second using this simulation frame's time delta
    float fTime = Vision::GetTimer()->GetTimeDifference();
    pEntity->IncOrientation( hkvVec3 (20.f * fTime,0,0) );
    return true;
  }
  return false;
}

VISION_DEINIT
{
  // Deinit the application
  spApp->DeInitSample();
  spApp = NULL;
  return 0;
}

VISION_MAIN_DEFAULT

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
