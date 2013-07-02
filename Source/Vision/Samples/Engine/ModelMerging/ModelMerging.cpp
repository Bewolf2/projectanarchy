/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// ModelMerging
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// This demo shows how you can use the model merging feature in Vision to customize a character
// by attaching various parts of an armor to a model and merging them to one single object.
//
// ***********************************************************************************************

#include <Vision/Samples/Engine/ModelMerging/ModelMergingPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/ModelMerging/GUI/MenuSystem.hpp>
#include <Vision/Samples/Engine/ModelMerging/MergedModelFactory.hpp>
#include <Vision/Samples/Engine/ModelMerging/KeyControlledTransitionBarbarian.hpp>


VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();

VisSampleAppPtr spApp;
VSmartPtr<MergeModelsMainMenu> spMainDlg;
VGUIMainContextPtr spGUIContext;
MergedModelFactory_cl *pMergeModelFactory = NULL;

enum MERGE_MODEL_CONTROL 
{
  MERGE_MODEL_SHOW_MENU = CHARACTER_CONTROL_LAST_ELEMENT+1
};


VISION_INIT
{
  VISION_SET_DIRECTORIES( false );

  // Include the vision engine plugin
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();
  VISION_PLUGIN_ENSURE_LOADED(vHavok);

  // Create an application
  spApp = new VisSampleApp();

  int iX = VVIDEO_DEFAULTWIDTH, iY = VVIDEO_DEFAULTHEIGHT;

#ifdef _VISION_PSP2
  // Use double resolution, since otherwise the menu doesn't fit
  iX *= 2, iY *= 2;
#endif

#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
  const char* szSceneFile = "ground_mobile";
#else
  const char* szSceneFile = "ground";
#endif

  // Init the application
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, szSceneFile /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_ALIGNLOGOALTERNATIVE, iX, iY ))
    return false;

  return true;
}
 
VISION_SAMPLEAPP_AFTER_LOADING
{
  // Add Barbarian Model directory
  spApp->AddSampleDataDir("Models\\Barbarian");

  // Define help text
  spApp->AddHelpText("");
  spApp->AddHelpText("How to use this demo :");
  spApp->AddHelpText("");

#ifdef SUPPORTS_KEYBOARD

  VisSampleApp::GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_KEYBOARD, CT_KB_SPACE, VInputOptions::Once());

  spApp->AddHelpText("KEYBOARD - ARROW UP: Move Forward");
  spApp->AddHelpText("KEYBOARD - ARROW UP + SHIFT: Run Forward");
  spApp->AddHelpText("KEYBOARD - ARROW LEFT: Rotate Left");
  spApp->AddHelpText("KEYBOARD - ARROW RIGHT: Rotate Right");
  spApp->AddHelpText("KEYBOARD - SPACE : Toggle the menu");
  spApp->AddHelpText("");

#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  VisSampleApp::GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_XENON_PAD(0), CT_PAD_Y, VInputOptions::Once());

  spApp->AddHelpText("PAD1 - Left Thumbstick : Move character");
  spApp->AddHelpText("PAD1 - Left Shoulder  : Run");
  spApp->AddHelpText("PAD1 - Y               : Toggle the menu");

#elif defined (_VISION_PS3)
  VisSampleApp::GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_PS3_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());

  spApp->AddHelpText("PAD1 - Left Thumbstick : Move character");
  spApp->AddHelpText("PAD1 - Left Shoulder  : Run");
  spApp->AddHelpText("PAD1 - TRIANGLE        : Toggle the menu");

#elif defined (_VISION_PSP2)
  VisSampleApp::GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, V_PSP2_PAD(0), CT_PAD_TRIANGLE, VInputOptions::Once());

  spApp->AddHelpText("Left Thumbstick : Move character");
  spApp->AddHelpText("Left Shoulder   : Run");
  spApp->AddHelpText("TRIANGLE        : Toggle the menu");

#elif defined (_VISION_WIIU)
  VisSampleApp::GetInputMap()->MapTrigger(MERGE_MODEL_SHOW_MENU, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_X, VInputOptions::Once());

  spApp->AddHelpText("DRC - Left Thumbstick : Move character");
  spApp->AddHelpText("DRC - Left Shoulder   : Run");
  spApp->AddHelpText("DRC - X               : Toggle the menu");

#endif


  // Create the model merged model factory
  pMergeModelFactory = new MergedModelFactory_cl();
  pMergeModelFactory->MergeModel();

  // Load some GUI resources
  VGUIManager::GlobalManager().LoadResourceFile("ModelMerging\\GUI\\MenuSystem.xml");
  spGUIContext = new VGUIMainContext(NULL);

  // Start the main menu
  spMainDlg = (MergeModelsMainMenu *)spGUIContext->ShowDialog("ModelMerging\\GUI\\MainMenu.xml");
  VASSERT(spMainDlg);
  spMainDlg->SetMergeModelFactory(pMergeModelFactory);

#if defined( _VISION_MOBILE )
  // Activate GUI
  spGUIContext->SetActivate(true);
  spGUIContext->ShowDialog( spMainDlg );
#else
  // Deactivate GUI
  spGUIContext->SetActivate(false);
#endif
}

VISION_SAMPLEAPP_RUN
{
  if (spApp->Run())
  {
    // Toggle GUI and camera mode
    if (VisSampleApp::GetInputMap()->GetTrigger(MERGE_MODEL_SHOW_MENU))
    {
      bool bShowGUI = !spGUIContext->IsActive();
      spGUIContext->SetActivate(bShowGUI);
      if (bShowGUI)
        spGUIContext->ShowDialog(spMainDlg);
    }

    return true;
  }
  return false;
}

VISION_DEINIT
{
  // Deinit GUI
  spMainDlg = NULL;
  spGUIContext->SetActivate(false);
  spGUIContext = NULL;
  VGUIManager::GlobalManager().CleanupResources();

  // Delete Factory
  V_SAFE_DELETE(pMergeModelFactory);

  // Deinit the application
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
