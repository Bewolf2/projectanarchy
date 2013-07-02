/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Tutorial 01 : Getting started
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows the basic steps needed to initialize the Vision Engine.
// This sample uses Source/Vision/Runtime/Common/VisSampleApp.cpp to initialize the engine
// ***********************************************************************************************
#include <Vision/Samples/Engine/Tutorial01/Tutorial01PCH.h>

VisSampleAppPtr spApp;

VISION_INIT
{
  // setup directories, does nothing on platforms other than iOS, 
  // pass true if you want load from the documents directory
  VISION_SET_DIRECTORIES(false);
  
  // Create an application
  spApp = new VisSampleApp(); 
  spApp->LoadVisionEnginePlugin();
  
  // Init the application
  if (!spApp->InitSample("Maps\\ViewerMap" /*DataDir*/, "ViewerMap" /*SampleScene*/ ))
    return false;
  
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  spApp->SetShowFrameRate(true);
  // Create a mouse controlled camera (optionally with gravity)
  VisBaseEntity_cl *pCamera = spApp->EnableMouseCamera();
}

VISION_SAMPLEAPP_RUN
{
  return spApp->Run();
}

VISION_DEINIT
{
  // Deinit the application
  spApp->DeInitSample();
  spApp = NULL;
  return true;
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
