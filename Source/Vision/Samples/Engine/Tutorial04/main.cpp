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

#define MY_VERSION_MAJOR 0x01
#define MY_VERSION_MINOR 0x00

//Module to register our classes with
DECLARE_THIS_MODULE( g_MyModule, MAKE_VERSION(MY_VERSION_MAJOR, MY_VERSION_MINOR),
                     "Engine Sample", "Havok", "Vision Engine Sample", NULL );

VisSampleAppPtr spApp;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // *********************************************************
  // Register the module so the engine knows about the classes
  // *********************************************************
  Vision::RegisterModule(&g_MyModule);
    
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

  // Create entities of type MyEntity_cl in front of the camera
  for (int i=-1; i<=1; i++)
  {
    MyEntity_cl * pEntity = (MyEntity_cl*) Vision::Game.CreateEntity("MyEntity_cl", 
                                hkvVec3 (250.f, (float)i * 150.f ,-120), "Models\\Warrior\\Warrior.MODEL");
    VASSERT(pEntity);
    pEntity->RotateSpeed = 20.f + (float)i * 10.f;    //each entity has a different speed
    pEntity->SetCastShadows(true);
  }
}
 
VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
  return spApp->Run();
  //Entities will automatically rotate
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
