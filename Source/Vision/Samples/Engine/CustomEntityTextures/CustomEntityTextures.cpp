/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ***********************************************************************************************
// Custom Entity Textures
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// Shows how to replace textures of single entity instances without modifying the shader assignment
// ***********************************************************************************************
#include <Vision/Samples/Engine/CustomEntityTextures/CustomEntityTexturesPCH.h>

// helper function to replace a single base texture of an entity
void ReplaceEntityBaseTexture(VisBaseEntity_cl *pEntity, const char *szSurfacename, const char *szNewTexture)
{
  VisSurfaceTextureSet_cl *pSet = pEntity->GetCustomTextureSet();
  if (!pSet)
  {
    pSet = pEntity->CreateCustomTextureSet();
    pEntity->SetCustomTextureSet(pSet);
  }

  // replace the diffuse base texture in the array
  int iIndex = pEntity->GetMesh()->GetSurfaceIndexByName(szSurfacename);
  VASSERT(iIndex>=0);
  pSet->GetTextures(iIndex)->m_spDiffuseTexture = Vision::TextureManager.Load2DTexture(szNewTexture);

}

VisSampleAppPtr spApp;

VISION_INIT
{
  VISION_SET_DIRECTORIES(false);
  
  // Create and init an application
  spApp = new VisSampleApp();
#if defined( _VISION_MOBILE ) || defined( HK_ANARCHY )
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground_mobile" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_FORCEMOBILEMODE))
#else
  if (!spApp->InitSample("Maps\\SimpleGround" /*DataDir*/, "ground" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS))
#endif
    return false;
  
  return true;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  //We get all the soldier models from an encrypted file
  spApp->AddSampleDataDir("Common");

  spApp->AddHelpText("");
  spApp->AddHelpText("Models (C) by Rocketbox Studios GmbH");
  spApp->AddHelpText("www.rocketbox-libraries.com");

  // create 5 entities of the same model and animate them
  VisBaseEntity_cl *pEntity[3];
  for (int i=0;i<3;i++)
  {
    hkvVec3 vPos(300.f,(i-1.f)*120.f, 60.f);
    pEntity[i] = Vision::Game.CreateEntity("VisBaseEntity_cl",vPos,"Models/Soldier/soldier_high.MODEL");
    pEntity[i]->SetOrientation(270.f,0.f,0.f);
    pEntity[i]->SetCastShadows(true);

    if (i==0)
    {
      VisAnimConfig_cl::StartSkeletalAnimation(pEntity[i], "Walk", VSKELANIMCTRL_DEFAULTS|VANIMCTRL_LOOP, 1.f);

      VASSERT_MSG(pEntity[i]->GetAnimConfig() != NULL, "Entity animation was not loaded - missing animation file?");

      // set the HINT_FREQUENT_REUSE flag for the config as we reuse it for 5 entities. This improves performance drastically
      pEntity[i]->GetAnimConfig()->SetFlags(pEntity[i]->GetAnimConfig()->GetFlags() | HINT_FREQUENT_REUSE);
    }
    else // share the animation data
      pEntity[i]->SetAnimConfig(pEntity[0]->GetAnimConfig());
  }

  // texture variation #1
  ReplaceEntityBaseTexture(pEntity[0], "s01_w_us_jg", "Models/Soldier/Textures/s01_w_ru_ur.dds");
  ReplaceEntityBaseTexture(pEntity[0], "h6_us_jg",    "Models/Soldier/Textures/h06_ru_ur.dds"); // helmet

  // texture variation #2
  ReplaceEntityBaseTexture(pEntity[1], "s01_w_us_jg", "Models/Soldier/Textures/s01_w_uk_ds.dds");
  ReplaceEntityBaseTexture(pEntity[1], "h6_us_jg",    "Models/Soldier/Textures/h06_uk_de.dds"); // helmet

  // Create a mouse controlled camera (without gravity)
  VisBaseEntity_cl *pCamera = spApp->EnableMouseCamera();
  pCamera->IncPosition(0,0,100.f);

  // We need a forward renderer node to cast shadows (will do nothing on platforms that don't support renderer nodes)
  VisSampleApp::CreateForwardRenderer();

  hkvVec3 lightPos( 0.f, 0.f, 300.f );
  VisLightSource_cl *pLight = Vision::Game.CreateLight( lightPos, VIS_LIGHT_POINT, 1250.f );
  pLight->SetPosition( lightPos );

#ifndef _VISION_PSP2
  VisSampleApp::SetShadowsForLight(pLight, true);
#endif

  //place the overlay
  VisScreenMask_cl *pOverlay = new VisScreenMask_cl("Models/Soldier/Textures/rb_cs_logo.dds");
    
  pOverlay->SetTransparency (VIS_TRANSP_ALPHA);
  pOverlay->SetFiltering(FALSE);
#ifndef _VISION_MOBILE
  pOverlay->SetPos(14.f, (float)(Vision::Video.GetYRes()-85) );
#else
  pOverlay->SetPos((float)(Vision::Video.GetXRes()-185), (float)(Vision::Video.GetYRes()-146) );
#endif
}
  
VISION_SAMPLEAPP_RUN
{
  // Run the main loop of the application until we quit
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
