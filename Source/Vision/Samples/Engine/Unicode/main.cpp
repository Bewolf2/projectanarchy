/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Unicode demo
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// This sample shows how to print unicode text in UTF8 encoding. The font data
// has been created using the font generator tool of the SDK. For
// A simple font implementation can also be found in the SimpleFont demo of the SDK.
// ***********************************************************************************************

#include <Vision/Samples/Engine/Unicode/UnicodePCH.h>
#include <Vision/Samples/Engine/Unicode/WorldLanguages.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

// language entity
VisSampleAppPtr   g_spApp;
WorldLanguages_cl *g_pLangEnt = NULL;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);

  // Create and init an application
  g_spApp = new VisSampleApp();

#if defined(_VISION_PSP2)
  if (!g_spApp->InitSample("Unicode" /*DataDir*/, "worldmap" /*SampleScene*/, 
    (VSAMPLE_INIT_DEFAULTS & ~VSAMPLE_HAVOKLOGO), 960, 544 ))
#else
  if (!g_spApp->InitSample("Unicode" /*DataDir*/, "worldmap" /*SampleScene*/, 
    (VSAMPLE_INIT_DEFAULTS & ~VSAMPLE_HAVOKLOGO) ))
#endif
    return false;

  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // define help text
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("How to use this demo :");
  g_spApp->AddHelpText("");
  g_spApp->AddHelpText("Switch places to see text in different languages.");
  g_spApp->AddHelpText("Note: Of course, most of the texts don't mean anything");
  g_spApp->AddHelpText("really, they are just a string of characters.");
  g_spApp->AddHelpText("");

#if defined(SUPPORTS_KEYBOARD)
  g_spApp->AddHelpText("KEYBOARD - SPACE : Switches to the next place");
  g_spApp->AddHelpText("KEYBOARD - BACKSPACE : Switches to the previous place");
  g_spApp->AddHelpText("");
#elif defined(SUPPORTS_MULTITOUCH)
  g_spApp->AddHelpText("SINGLE TOUCH : Switches to the next place");
  g_spApp->AddHelpText("");
#endif

#if defined (_VISION_XENON) || (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))
  g_spApp->AddHelpText("PAD1 - A : Switches to the next place");
  g_spApp->AddHelpText("PAD1 - B : Switches to the previous place");
#elif defined (_VISION_PS3)
  g_spApp->AddHelpText("PAD1 - CROSS : Switches to the next place");
  g_spApp->AddHelpText("PAD1 - CIRCLE : Switches to the previous place");
#elif defined (_VISION_WIIU)
  g_spApp->AddHelpText("DRC - B : Switches to the next place");
  g_spApp->AddHelpText("DRC - A : Switches to the previous place");
#elif defined(_VISION_MOBILE)
  g_spApp->AddHelpText("RIGHT SCREEN BORDER : Switches to the next place");
  g_spApp->AddHelpText("LEFT SCREEN BORDER : Switches to the previous place");  
  g_spApp->SetShowHelpText(true);
#endif

  // create world languages entity
  VASSERT(g_pLangEnt == NULL);
  g_pLangEnt = static_cast<WorldLanguages_cl*>(Vision::Game.CreateEntity("WorldLanguages_cl", hkvVec3 (0,0,0)));
  g_pLangEnt->SetEntityKey("languages");
} 

VISION_SAMPLEAPP_RUN
{
  // main loop
  return g_spApp->Run();
}

VISION_DEINIT
{
  // clean up
  Vision::Game.RemoveEntity(g_pLangEnt);
  g_pLangEnt = NULL;

  // deinit the App
  g_spApp->DeInitSample();
  g_spApp = NULL;
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
