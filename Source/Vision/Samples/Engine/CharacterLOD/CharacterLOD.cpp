/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Character LOD Sample
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how to use characters with multiple models at different quality levels (LOD),
// which automatically change according to distance of the camera.
// ***********************************************************************************************
#include <Vision/Samples/Engine/CharacterLOD/CharacterLODPCH.h>
#include <Vision/Samples/Engine/CharacterLOD/LODObject.h>
#include <Vision/Samples/Engine/Common/Entities/StaticCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/CharacterLOD/GUI/MenuSystem.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>
#include <Vision/Runtime/Base/System/IO/Clipboard/VClipboard.hpp>
#include <Vision/Samples/Engine/CharacterLOD/DemoCamera.hpp>

#define DONT_SHOW_DEFAULT_COMPANY_LOGO

VisSampleAppPtr spApp;
VGUIMainContextPtr spGUIContext;
VDialogPtr spMainDlg;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // include the vision engine plugin
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();
#ifdef _VR_GLES2
  Vision::Renderer.SetUseSingleBufferedStaticMeshes(false);
#endif

  // Create and init an application
  spApp = new VisSampleApp();

#ifdef _VISION_PS3
  // Ensure that we have enough mapped main memory available (to avoid warnings)
  spApp->m_appConfig.m_gcmConfig.uiMappedMainMemorySize = 10 * 1024 * 1024;
#endif
  
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground_mobile" /*SampleScene*/, (VSAMPLE_INIT_DEFAULTS & ~VSAMPLE_HAVOKLOGO) | VSAMPLE_FORCEMOBILEMODE ))
#else
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS&~VSAMPLE_HAVOKLOGO ))
#endif
    return false;
  
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  spApp->AddSampleDataDir("Common");
  
  // setup dynamic light
  hkvVec3 lightPos( 0.f, 0.f, 300.f );
  VisLightSource_cl *pLight = Vision::Game.CreateLight( lightPos, VIS_LIGHT_POINT, 15000.f );
  
  pLight->SetMultiplier( 1.1f );
  pLight->SetPosition( lightPos );

  //Create the entities
  LODObject *pLODObject, *pLookatEntity=NULL;
  for (int i=0;i<ENTITY_COUNT;i++)
  {
    char szKey[64];
    hkvVec3 origin(((float)(i%3)-1.f)*400.f,((float)(i/3)-1.f)*400.f, 80.f);
    pLODObject = (LODObject *) Vision::Game.CreateEntity("LODObject",origin);
    if (i==ENTITY_COUNT/2)
      pLookatEntity = pLODObject;
    sprintf(szKey,"Soldier_%i",i);
    pLODObject->m_LOD.SetLODLevel(LOD_AUTO);
    pLODObject->SetEntityKey(szKey);
  }

  // Setup the camera
  hkvVec3 cameraOrigin( 0, 0, 0 );
  VisDemoCamera_cl *pCamera = (VisDemoCamera_cl *)Vision::Game.CreateEntity("VisDemoCamera_cl", cameraOrigin );
  pCamera->SetTargetEntity( pLookatEntity );

  // load some GUI resources
  VGUIManager::GlobalManager().LoadResourceFile("CharacterLOD\\Dialogs\\MenuSystem.xml");
  spGUIContext = new VGUIMainContext(NULL);
  spGUIContext->SetRenderHookConstant(VRH_PRE_SCREENMASKS);
  spGUIContext->SetActivate(true);

  // start the main menu
  spMainDlg = spGUIContext->ShowDialog("CharacterLOD\\Dialogs\\MainMenu.xml");
  VASSERT(spMainDlg);

    // define help text
  spApp->AddHelpText("");
  spApp->AddHelpText("How to use this demo :");

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  spApp->AddHelpText("PAGE UP                    : Zoom in");
  spApp->AddHelpText("PAGE DOWN                  : Zoom Out");
#endif

#ifdef SUPPORTS_MOUSE
  spApp->AddHelpText("MOUSE - WHEEL              : Zoom in/out");
  spApp->AddHelpText("MOUSE - RIGHT MOUSE BUTTON : Rotate scene");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  spApp->AddHelpText(" PAD 1 - LEFT/RIGHT TRIGGER  : Zoom in/out");
  spApp->AddHelpText(" PAD 1 - DIGITAL CURSOR-KEYS : Rotate scene");
#elif defined(_VISION_PS3)
  spApp->AddHelpText(" PAD 1 - LEFT/RIGHT TRIGGER  : Zoom in/out");
  spApp->AddHelpText(" PAD 1 - DIGITAL CURSOR-KEYS : Rotate scene");
#elif defined(_VISION_WIIU)
  spApp->AddHelpText(" DRC - LEFT/RIGHT TRIGGER  : Zoom in/out");
  spApp->AddHelpText(" DRC - DIGITAL CURSOR-KEYS : Rotate scene");
#endif

  spApp->AddHelpText("");
  spApp->AddHelpText("Watch the soldiers (+ the frame rate)");
  spApp->AddHelpText("and how each model changes according");
  spApp->AddHelpText("to the distance of the camera. The");
  spApp->AddHelpText("letters above the soldiers indicate");
  spApp->AddHelpText("the selected model.");
  spApp->AddHelpText("");
  spApp->AddHelpText("(You can also switch to manual model");
  spApp->AddHelpText("selection via the side menu)");
  spApp->AddHelpText("");
  spApp->AddHelpText("Models (C) by Rocketbox Studios GmbH");
  spApp->AddHelpText("www.rocketbox-libraries.com");

  //show fps
  spApp->SetShowFrameRate(true);

  // deactivate automatic use of @2x resources on high-resolution displays
  bool bAllow2x = Vision::Video.GetAllowAutomaticUseOf2xAssets();
  Vision::Video.SetAllowAutomaticUseOf2xAssets( false );

  //place the overlay
  VisScreenMask_cl *pOverlay = new VisScreenMask_cl("Models/Soldier/Textures/rb_cs_logo.dds");
    
  pOverlay->SetTransparency(VIS_TRANSP_ALPHA);
  pOverlay->SetFiltering(FALSE);
  pOverlay->SetPos(14.f, (float)(Vision::Video.GetYRes() - 95));

#if !defined(HK_ANARCHY)
  pOverlay = new VisScreenMask_cl("Textures\\Havok_Logo_128x64.dds");
    
  pOverlay->SetTransparency(VIS_TRANSP_ALPHA);
  pOverlay->SetFiltering(FALSE);
  pOverlay->SetPos(14.f, (float)(Vision::Video.GetYRes() - 156));
#endif

  Vision::Video.SetAllowAutomaticUseOf2xAssets( bAllow2x );

  //bool bShadow = false;
}

VISION_SAMPLEAPP_RUN
{
#if defined(_VISION_MOBILE)
  if (spMainDlg->GetDialogResult())
    return false;

  // main loop
  return spApp->Run();
#else
  // main loop
  return spApp->Run() && !spMainDlg->GetDialogResult();
#endif
}

VISION_DEINIT
{
  // Deinit GUI
  spMainDlg = NULL;
  spGUIContext->SetActivate(false);
  spGUIContext = NULL;
  VGUIManager::GlobalManager().CleanupResources();

  // Deinit our application
  spApp->DeInitSample();
  spApp = NULL;

  Vision::Plugins.UnloadAllEnginePlugins();
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
