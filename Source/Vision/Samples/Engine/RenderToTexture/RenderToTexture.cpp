/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Render-To-Texture
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This demo shows the "RenderToTexture" feature of the Vision Engine
// It loads a map with two "security camera" entities in it. Security cameras are a typical example for using 
// rendering to a texture.
// The security camera entities each reference a destination texture which is their render target. The destination 
// textures are mapped onto monitors so that the monitors display the security camera's view. The engine
// has a powerful optimisation, i.e. that the "render to texture" operation is only performed if the destination
// texture is actually visible!
// Please have a look at the security camera entity code to see how the rendering is performed in detail.
//
// Other examples for "render to texture":
//  - rearview mirror in a racing game
//  - location dependent environment map for a envmap shader (e.g. on a car)
//  - terminal dialog with a person at ground control
// ***********************************************************************************************
#include <Vision/Samples/Engine/RenderToTexture/RenderToTexturePCH.h>

VisSampleAppPtr spApp;
SecurityCamEntity_cl* pSecCam1;
SecurityCamEntity_cl* pSecCam2;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // include the vision engine plugin (required for some of the scene elements)
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();

  // Create and init an application
  spApp = new VisSampleApp();
#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
  if (!spApp->InitSample("Maps\\SciFi" /*DataDir*/, NULL /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_FORCEMOBILEMODE))
#elif defined(_VISION_PSP2)
  if (!spApp->InitSample("Maps\\SciFi" /*DataDir*/, NULL /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_WAITRETRACE))
#else
  if (!spApp->InitSample("Maps\\SciFi" /*DataDir*/, NULL /*SampleScene*/))
#endif
    return false;


  spApp->LoadScene("Crossing");

  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // define help text
  spApp->AddHelpText("");
  spApp->AddHelpText("How to use this demo :");
  spApp->AddHelpText("");
  spApp->AddHelpText("You can relocate the security cameras!");
  spApp->AddHelpText("Move very close to a security camera to pick it");
  spApp->AddHelpText("Drop it where ever you want");
  spApp->AddHelpText("");

#ifdef SUPPORTS_KEYBOARD
  spApp->AddHelpText("KEYBOARD - ENTER : Pick/Drop security camera");
  spApp->AddHelpText("");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  spApp->AddHelpText("PAD1 - A   : Pick/Drop security camera");
#elif defined (_VISION_PS3)
  spApp->AddHelpText("PAD1 - CROSS : Pick/Drop security camera");
#elif defined (_VISION_PSP2)
  spApp->AddHelpText("PAD - CROSS : Pick/Drop security camera");
#elif defined (_VISION_MOBILE)
  spApp->AddHelpText("Upper right screen corner : Pick/Drop security camera");
  spApp->SetShowHelpText(true);
#elif defined (_VISION_WIIU)
  spApp->AddHelpText("DRC - B : Pick/Drop security camera");
#endif

  // Create two security cameras.
  pSecCam1 = (SecurityCamEntity_cl*)Vision::Game.CreateEntity("SecurityCamEntity_cl", hkvVec3(-520,300,20), "Models\\securitycam.model");
  pSecCam1->SetYawSpeed(0.05f * hkvMath::pi () * 2.0f);
  pSecCam1->SetMonitorSurfaceName("MonitorScreen03_Mat");

  pSecCam2 = (SecurityCamEntity_cl*)Vision::Game.CreateEntity("SecurityCamEntity_cl", hkvVec3(-720,0,390), "Models\\securitycam.model", "m_bUseThermalImage=TRUE");
  pSecCam2->SetYawSpeed(0.07f * hkvMath::pi () * 2.0f);
  pSecCam2->SetMonitorSurfaceName("MonitorScreen01_Mat");



  VisBaseEntity_cl *pCam = spApp->EnableMouseCamera();

  VisBaseEntity_cl *pSpawnPos = Vision::Game.SearchEntity("spawnpos");
  if (pSpawnPos)
  {
    pCam->SetPosition(pSpawnPos->GetPosition());
    Vision::Camera.ReComputeVisibility();
  } else
  {
    pCam->SetPosition(150, 0, 200);
    pCam->SetOrientation(180, -15, 0);
  }
}

VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
  // everything is done by the security camera entities
  return spApp->Run();
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
