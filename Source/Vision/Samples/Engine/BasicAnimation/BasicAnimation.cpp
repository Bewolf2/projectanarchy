/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Simple Animation Sample
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// The Simple Animation Sample has been designed to give a brief introduction into the various
// parts of the animation system. The sample code snippets are rather simple and thus easy to
// understand.
//
// The SimpleSkeletalAnimatedObject_cl class covers the skeletal animation topic and shows you how
// to start skeletal animations, blend skeletal animation, layer skeletal animations, listen to
// animation events, set the animation time manually and apply forward kinematics to a bone.
//
// The SimpleVertexAnimatedObject_cl class is about vertex animations and shows you how to start
// a vertex animation.
//
// ***********************************************************************************************
#include <Vision/Samples/Engine/BasicAnimation/BasicAnimationPCH.h>
#include <Vision/Samples/Engine/BasicAnimation/BasicAnimation.h>
#include <Vision/Samples/Engine/BasicAnimation/SimpleSkeletalAnimatedObject.h>
#include <Vision/Samples/Engine/Common/Entities/StaticCamera.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/BasicAnimation/GUI/MenuSystem.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>
#include <Vision/Runtime/Base/System/IO/Clipboard/VClipboard.hpp>
#if !defined (_VISION_MOBILE ) && !defined( HK_ANARCHY )
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapComponentSpotDirectional.hpp>
#else
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VMobileShadowMapComponentSpotDirectional.hpp>
#endif

void TestSerialize(VisBaseEntity_cl *pEntity)
{
  // create a clone of the entity via serializing it to a memory stream
  VClipboardPtr m_spClipboard = new VClipboard(); // for memory streams
  {
    // write the entity into a memory clipboard
    IVFileOutStream *pMemStream = m_spClipboard->Create("MemoryStream");
    VArchive ar("TestArchive",pMemStream,Vision::GetTypeManager());
      ar << Vision::GetArchiveVersion();
      ar << pEntity;
    ar.Close();
    pMemStream->Close();
  }
  {
    // ...and load again as a separate instance running parallel
    IVFileInStream *pMemStream = m_spClipboard->Open("MemoryStream");
    VArchive ar("TestArchive",pMemStream,Vision::GetTypeManager());
      int iVers;
      ar >> iVers;
      ar.SetLoadingVersion(iVers);
      VisBaseEntity_cl *pEntity1 = (VisBaseEntity_cl *)ar.ReadObject(NULL);
    ar.Close();
    pMemStream->Close();
    pEntity1->IncPosition(10.f,0,0); // shift to the side so we can see whether both are synchronous
  }
}

VisSampleAppPtr spApp;
VGUIMainContextPtr spGUIContext;
VDialogPtr spMainDlg;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  // include the vision engine plugin
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();

  // Create and init an application
  spApp = new VisSampleApp();
#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground_mobile" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_ALIGNLOGOALTERNATIVE | VSAMPLE_FORCEMOBILEMODE))
#elif defined(_VISION_PSP2)
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_ALIGNLOGOALTERNATIVE, 960, 544 ))
#else
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_ALIGNLOGOALTERNATIVE ))
#endif
    return false;

  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // We need a forward renderer node to cast shadows (will do nothing on platforms that don't support renderer nodes)
  VisSampleApp::CreateForwardRenderer();

  // define help text
  spApp->AddHelpText("");
  spApp->AddHelpText("How to use this demo :");
  spApp->AddHelpText("");

#if defined(WIN32) && (!defined(_VISION_WINRT) || defined(_VISION_METRO))
  #ifdef SUPPORTS_KEYBOARD
    spApp->AddHelpText("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
    spApp->AddHelpText("- When playing skeletal animations, select the animation mode from the list box below");
    spApp->AddHelpText("- Depending on the selected animation type and mode, additional controls may be shown");
    Vision::Message.Add("Use the sliders to control the animations. Press F1 for more details.");
  #endif
#elif defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO) )
  spApp->AddHelpText("- Move the cursor with the left thumbstick of Pad1 and click with the A button");
  spApp->AddHelpText("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  spApp->AddHelpText("- When playing skeletal animations, select the animation mode from the list box below");
  spApp->AddHelpText("- Depending on the selected animation type and mode, additional controls may be shown");
  Vision::Message.Add("Use the sliders to control the animations. Press PAD1 - BACK for more details.");
#elif defined (_VISION_PS3)
  spApp->AddHelpText("- Move the mouse cursor with the left thumbstick of Pad1 and click with the CROSS button");
  spApp->AddHelpText("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  spApp->AddHelpText("- When playing skeletal animations, select the animation mode from the list box below");
  spApp->AddHelpText("- Depending on the selected animation type and mode, additional controls may be shown");
  Vision::Message.Add("Use the sliders to control the animations. Press PAD1 - SELECT for more details.");
#elif defined (_VISION_PSP2)
  spApp->AddHelpText("- Move the mouse cursor with the left thumbstick of Pad1 and click with the CROSS button");
  spApp->AddHelpText("- Select the animation type by clicking on the Skeletal Animation or Vertex Animation labels");
  spApp->AddHelpText("- When playing skeletal animations, select the animation mode from the list box below");
  spApp->AddHelpText("- Depending on the selected animation type and mode, additional controls may be shown");
  Vision::Message.Add("Use the sliders to control the animations. Press PAD1 - SELECT for more details.");
#endif

  const hkvVec3 characterOrigin( -150.0f, 150.f, -20.f );
  const hkvVec3 bubbleOrigin( -150.0f, 150.f, 120.f );
  const hkvVec3 lightPos( 200.f, 100.f, 500.f );
  const hkvVec3 lightTarget = characterOrigin + hkvVec3( -150.0f, 100.0f, -20.f );

  // setup dynamic light
  VisLightSource_cl *pLight = new VisLightSource_cl(VIS_LIGHT_SPOTLIGHT, 2000.f);
  pLight->SetPosition(lightPos);
  pLight->SetDirection(lightTarget - lightPos);

#if defined(SUPPORTS_SHADOW_MAPS)

  // create shadow map component
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  VMobileShadowMapComponentSpotDirectional *pComponent = new VMobileShadowMapComponentSpotDirectional(0);
  pComponent->SetShadowMapSize(512);
  pComponent->SetNearClip( 100.0f );
#else
  VShadowMapComponentSpotDirectional *pComponent = new VShadowMapComponentSpotDirectional(0);
  pComponent->SetShadowMapSize(1024);
  pComponent->SetCascadeCount(1);
  pComponent->SetCascadeRange(0, 400.0f);
  pComponent->SetCascadeSelection(VShadowMapComponentSpotDirectional::CSM_SELECT_BY_BOUNDINGBOX);
  pComponent->SetShadowMappingMode(SHADOW_MAPPING_MODE_PCF8);
#endif
  pLight->AddComponent(pComponent);

#endif

  // Create two entities
  SimpleSkeletalAnimatedObject_cl *pSkeletalObj = (SimpleSkeletalAnimatedObject_cl *) 
    Vision::Game.CreateEntity("SimpleSkeletalAnimatedObject_cl",characterOrigin);
  VisBaseEntity_cl *pCharacter = Vision::Game.CreateEntity("SimpleVertexAnimatedObject_cl",bubbleOrigin);
  // set enity keys so the GUI can find them
  pSkeletalObj->SetEntityKey("SkeletalAnimEntity");
  pCharacter->SetEntityKey("VertexAnimEntity");

  // Setup the camera
  hkvVec3 cameraOrigin( 0.f, 0.f, 200.f );
  StaticCamera_cl *pCamera = (StaticCamera_cl *) Vision::Game.CreateEntity( "StaticCamera_cl", cameraOrigin );
  pCamera->SetTarget( pSkeletalObj );
  hkvVec3 vCamTarget(0.f, 100.f, 50.f);
  pCamera->SetTargetOffset( vCamTarget ); //look at the center of the character

  // load some GUI resources
  VGUIManager::GlobalManager().LoadResourceFile("BasicAnimation\\MenuSystem.xml");
  spGUIContext = new VGUIMainContext(NULL);
  spGUIContext->SetActivate(true);

  // start the main menu
  spMainDlg = spGUIContext->ShowDialog("BasicAnimation\\MainMenu.xml");
  VASSERT(spMainDlg);
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
