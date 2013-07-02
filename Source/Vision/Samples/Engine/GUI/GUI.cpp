/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// GUI Demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This is a demo which provides a completely object oriented Graphical User Interface system
// based on Vision screenmasks.
// This system is reusable and extendable. For instance, a new class can be derived from the item class to
// make a custom type of menu items, such as scrollbars, etc.
// ***********************************************************************************************
#include <Vision/Samples/Engine/GUI/GUIPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Samples/Engine/GUI/MonitorContext.hpp>
#include <Vision/Runtime/Common/VisSampleApp.hpp>

#if defined(_VISION_ANDROID)
#include <Vision/Runtime/Common/VisMobileExitDialog.hpp> 
#endif

enum GUIState
{
  GS_INIT,
  GS_MAIN_MENU,
  GS_GAME
};

GUIState g_state = GS_INIT;

VGUIMainContextPtr g_spGUIContext = NULL;
VMonitorGUIContextPtr g_spMonitor = NULL;

VDialog* g_pMainMenuDialog = NULL;
VDialog* g_pMessageBoxInGame = NULL;

class GUISampleApp : public VisSampleApp
{
public:
  virtual void OnUpdateScene() HKV_OVERRIDE
  {
    if ((g_spGUIContext != NULL) && g_spGUIContext->IsActive())
    {
      // Timer is not longer updated here, because it needs to be updated right after the frame flip
      Vision::Callbacks.OnUpdateSceneBegin.TriggerCallbacks();
      const float dtime = Vision::GetTimer()->GetTimeDifference();

      // Handle render contexts
      VisRenderContext_cl::HandleAllRenderContexts(dtime);

      Vision::Profiling.Update();
    }
    else
    {
      VisSampleApp::OnUpdateScene();
    }
  }

  virtual bool Run() HKV_OVERRIDE
  {
    if ((g_spGUIContext != NULL) && g_spGUIContext->IsActive())
    {
#if defined(_VISION_ANDROID)
      if (m_spGUIContext != NULL && m_spGUIContext->IsActive() && m_spExitDlg != NULL && static_cast<VisMobileExitDialog*>(m_spExitDlg.GetPtr())->IsExitTriggered())
      {
        return false;
      }
#endif

      return VisionApp_cl::Run();
    }
    else
    {
      return VisSampleApp::Run();
    }
  }

#if defined(_VISION_ANDROID)
  virtual bool OnAndroidBackButtonPressed() HKV_OVERRIDE
  {
    // Disable back button default behaviour
    return true;
  }
#endif
};

VSmartPtr<GUISampleApp> spApp = NULL;

// Sample flags, and filename configurations
#if defined(_VISION_MOBILE) || defined(HK_ANARCHY)
  int iSampleFlags = VSAMPLE_INIT_DEFAULTS | VSAMPLE_FORCEMOBILEMODE;
  int iVideoWidth = VVIDEO_DEFAULTWIDTH;
  int iVideoHeight = VVIDEO_DEFAULTHEIGHT;

  #define MENU_SYSTEM_FILENAME "GUI\\Dialogs\\MenuSystemMobile.xml"
  #define MAIN_MENU_FILENAME "GUI\\Dialogs\\MainMenuMobile.xml"

  #define SCENE_NAME "Crossing"
  #define MESH_NAME "Crossing.vmesh"

  VisScreenMaskPtr g_spExit;

#elif defined(_VISION_PSP2)
  int iSampleFlags = VSAMPLE_INIT_DEFAULTS | VSAMPLE_WAITRETRACE;
  int iVideoWidth = 960;
  int iVideoHeight = 544;

  #define MENU_SYSTEM_FILENAME "GUI\\Dialogs\\MenuSystemMobile.xml"
  #define MAIN_MENU_FILENAME "GUI\\Dialogs\\MainMenuMobile.xml"

  #define SCENE_NAME "Crossing"
  #define MESH_NAME "Crossing.vmesh"

#else
  int iSampleFlags = VSAMPLE_INIT_DEFAULTS;
  int iVideoWidth = VVIDEO_DEFAULTWIDTH;
  int iVideoHeight = VVIDEO_DEFAULTHEIGHT;

  #define MENU_SYSTEM_FILENAME "GUI\\Dialogs\\MenuSystem.xml"
  #define MAIN_MENU_FILENAME "GUI\\Dialogs\\MainMenu.xml"

  #define SCENE_NAME "Crossing"
  #define MESH_NAME "Crossing.vmesh"

#endif

#define MAP_DATA_DIR  "Maps"VFILE_STR_SEPARATOR"SciFi"

// Optional callback listener that can translate all text labels in the GUI.
// Note: Does not do anything in this version.
class TextLabelTranslator : public IVisCallbackHandler_cl
{
public:
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData)
  {
    VTextLabelDataObject *pTextData = (VTextLabelDataObject *)pData;
    if (!pTextData->m_szLabel)
      return;

    // Optionally do some localization here...
    //pTextData->m_bTranslated = true;
    //pTextData->m_szLabel = "Translated version";
  }
};

TextLabelTranslator translator;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);

  // Include the vision engine plugin
  VisionAppHelpers::MakeEXEDirCurrent();
  VisSampleApp::LoadVisionEnginePlugin();

#if defined(_VISION_MOBILE) || defined(HK_ANARCHY)
  Vision::Renderer.SetUseSingleBufferedStaticMeshes(false);
#endif

#if defined(_VISION_WIIU)
  iSampleFlags &= ~VSAMPLE_WIIU_DRCDEMO;
#endif

  // Create and init an application
  spApp = new GUISampleApp();
  if (!spApp->InitSample(MAP_DATA_DIR /*DataDir*/, NULL,  iSampleFlags&~VSAMPLE_SPLASHSCREEN&~VSAMPLE_HAVOKLOGO, iVideoWidth, iVideoHeight ))
    return false;

  g_state = GS_INIT;

  g_spGUIContext = NULL;
  g_spMonitor = NULL;

  g_pMainMenuDialog = NULL;
  g_pMessageBoxInGame = NULL;

  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  if (g_state == GS_INIT)
  {
    // Define help text
    spApp->AddHelpText("");
    spApp->AddHelpText("How to use this demo :");
    spApp->AddHelpText("");

#ifdef SUPPORTS_KEYBOARD
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT, V_KEYBOARD, CT_KB_ESC);
    spApp->AddHelpText("KEYBOARD - ESC : Go to main menu");
    spApp->AddHelpText("");
#endif

#ifdef SUPPORTS_MOUSE
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_MOUSE, CT_MOUSE_LEFT_BUTTON);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_MOUSE, CT_MOUSE_RIGHT_BUTTON);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_MOUSE, CT_MOUSE_MIDDLE_BUTTON);
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_XENON_PAD(0), CT_PAD_A);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_XENON_PAD(0), CT_PAD_B);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_XENON_PAD(0), CT_PAD_X);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT,     V_XENON_PAD(0), CT_PAD_Y);

    spApp->AddHelpText("PAD1 - A : Click with cursor");
    spApp->AddHelpText("PAD1 - Y : Go to main menu");

#elif defined (_VISION_PS3)
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_PS3_PAD(0), CT_PAD_CROSS);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_PS3_PAD(0), CT_PAD_CIRCLE);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_PS3_PAD(0), CT_PAD_SQUARE);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT,     V_PS3_PAD(0), CT_PAD_TRIANGLE);

    spApp->AddHelpText("PAD1 - CROSS : Click with cursor");
    spApp->AddHelpText("PAD1 - TRIANGLE : Go to main menu");

#elif defined (_VISION_PSP2)
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, V_PSP2_PAD(0), CT_PAD_CROSS);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, V_PSP2_PAD(0), CT_PAD_CIRCLE);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, V_PSP2_PAD(0), CT_PAD_SQUARE);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT, V_PSP2_PAD(0), CT_PAD_TRIANGLE);

    spApp->AddHelpText("PAD1 - CROSS : Click with cursor");
    spApp->AddHelpText("PAD1 - TRIANGLE : Go to main menu");

#elif defined (_VISION_WIIU)
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_1, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_B);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_2, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_A);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_ACTION_3, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_Y);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT,     VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_X);

    spApp->AddHelpText("PAD1 - B : Click with cursor");
    spApp->AddHelpText("PAD1 - X : Go to main menu");

#elif defined(_VISION_MOBILE)
    int iResX = Vision::Video.GetXRes();
    int iResY = Vision::Video.GetYRes();
    int iWidth, iHeight;

    g_spExit = new VisScreenMask_cl("GUI\\Dialogs\\ButtonQuit_Normal.TGA");
    g_spExit->GetTextureSize(iWidth, iHeight);
    g_spExit->SetPos(iResX - iWidth - 16.0f, 16.0f);
    g_spExit->SetTransparency(VIS_TRANSP_ALPHA);
    g_spExit->SetVisible(TRUE);

    VTouchArea* pDemoExitArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(iResX - iWidth - 16.0f, 0.0f, float(iResX), 16.0f + iHeight), 0.0f);
    VisSampleApp::GetInputMap()->MapTrigger(GUI_DEMO_EXIT, pDemoExitArea, CT_TOUCH_ANY);

#endif

    Vision::Error.SetReportStatus(FALSE);
    Vision::ResourceSystem.SetReportSurvivingResources(FALSE);

    // Create a GUI context and load the GUI resources
    g_spGUIContext = new VGUIMainContext(NULL); // NULL means using the VGUIManager::GlobalManager
    g_spGUIContext->SetActivate(true);

    // Load some GUI resources:
    VGUIManager::GlobalManager().LoadResourceFile(MENU_SYSTEM_FILENAME);

    VGUIManager::OnTextLabelCallback += translator;

    // Use a modified default tooltip
    VTooltip *pCustomTooltip = new VTooltip();
    pCustomTooltip->SetDelay(0.5f); // shorter delay than usual
    g_spGUIContext->SetTooltipTemplate(pCustomTooltip);

    g_spGUIContext->SetActivate(true);
    g_pMainMenuDialog = g_spGUIContext->ShowDialog(MAIN_MENU_FILENAME);

    // Prepare the world in order to have skinning shaders for the render to texture menu
    Vision::InitWorld();

    // Start in main menu after initialization
    g_state = GS_MAIN_MENU;
  }
  else if (g_state == GS_GAME)
  {
    VisBaseEntity_cl *pCamera = spApp->EnableMouseCamera();
    pCamera->SetPosition(150, 0, 200);
    pCamera->SetOrientation(180, -15, 0);

    VisSurface_cl *pScreenSrf = NULL;
    VMeshManager* MeshManager = VMeshManager::GetMeshManager();
    VisStaticMesh_cl* CinemaMesh = (VisStaticMesh_cl*)MeshManager->GetResourceByName(MESH_NAME);
    
    if( CinemaMesh )
      pScreenSrf = CinemaMesh->GetSurfaceByName("MonitorScreen01_Mat");

    // Put GUI on one of the scene's monitors.
    g_spMonitor = new VMonitorGUIContext(NULL, pScreenSrf);
    g_spMonitor->SetActivate(true);
    g_spMonitor->ShowDialog("GUI\\Dialogs\\MonitorDialog.xml");
  }
}

VISION_SAMPLEAPP_RUN
{
  switch (g_state)
  {
    case GS_MAIN_MENU:
      {
        VASSERT(g_pMainMenuDialog);

        int iDlgResult = g_pMainMenuDialog->GetDialogResult();
        if (iDlgResult)
        {
          if (iDlgResult == VGUIManager::GetID("CANCEL"))
          {
#if defined(_VISION_ANDROID)
            g_pMainMenuDialog->SetDialogResult(0);

            // On android use the build-in exit dialog
            spApp->ShowExitDialog();
#else
            g_spGUIContext->CloseDialog(g_pMainMenuDialog);
            g_pMainMenuDialog = NULL;
            return false;
#endif
          }
          else if (iDlgResult == VGUIManager::GetID("NEWGAME"))
          {
            g_spGUIContext->CloseDialog(g_pMainMenuDialog);
            g_pMainMenuDialog = NULL;

            g_spGUIContext->SetActivate(false);

            g_state = GS_GAME;

            // Load a map with free camera...
            spApp->LoadScene(SCENE_NAME);

            return true;
          }
        }

#if defined(_VISION_ANDROID)
        if (spApp->GetInputMap()->GetTrigger(EXIT)) 
        {
          // Show exit dialog when back button is pressed in main menu
          spApp->ShowExitDialog();
        }
#endif
        
        return spApp->Run();        
      }

    case GS_GAME:
      {
        if (VisSampleApp::GetInputMap()->GetTrigger(GUI_DEMO_EXIT)
#if defined (_VISION_XENON) || defined (_VISION_PS3) || defined (_VISION_PSP2) || defined(_VISION_WIIU) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
          || (VisSampleApp::GetInputMap()->GetTrigger(EXIT_COMBO) && VisSampleApp::GetInputMap()->GetTrigger(EXIT))
#endif
          )
        {
          // Show in-game menu, allowing the user to exit to main menu
          g_spGUIContext->SetActivate(true);
          g_pMessageBoxInGame = g_spGUIContext->ShowDialog("GUI\\Dialogs\\MessageBox_InGame.xml");
        }

        spApp->Run();

        if (g_pMessageBoxInGame)
        {
          int iDlgResult = g_pMessageBoxInGame->GetDialogResult();
          if (iDlgResult)
          {
            // Close the dialog when a result is available
            g_spGUIContext->CloseDialog(g_pMessageBoxInGame);
            g_pMessageBoxInGame = NULL;

            g_spGUIContext->SetActivate(false);

            // In case of MAINMENU, unload scene and go to main menu again
            if (iDlgResult == VGUIManager::GetID("MAINMENU"))
            {
              g_spMonitor->SetActivate(false);
              g_spMonitor = NULL;

              Vision::InitWorld();
              g_state = GS_MAIN_MENU;

              g_spGUIContext->SetActivate(true);
              g_pMainMenuDialog = g_spGUIContext->ShowDialog(MAIN_MENU_FILENAME);
            }
          }
        }

        return true;
      }

    default:
      VASSERT(false);
      return false;
  }
}

VISION_DEINIT
{
  // Cleanup GUI related stuff just before sample de-initialization
  g_spGUIContext->SetActivate(false);
  g_spGUIContext = NULL;

  VGUIManager::GlobalManager().CleanupResources();
  VGUIManager::OnTextLabelCallback -= translator;

  if (g_spMonitor)
  {
    g_spMonitor->SetActivate(false);
    g_spMonitor = NULL;
  }

  g_pMainMenuDialog = NULL;
  g_pMessageBoxInGame = NULL;

#if defined(_VISION_MOBILE)
  g_spExit = NULL;
#endif

  // Deinit our application
  spApp->DeInitSample();
  spApp = NULL;
  Vision::Plugins.UnloadAllEnginePlugins();

  return 0;
}

#if !defined( _VISION_IOS )
VISION_MAIN_DEFAULT
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
