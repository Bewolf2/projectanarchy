/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//***********************************************************************************************
// HavokAi : Havok AI Binding Sample
// Copyright (C) Havok.com Inc. All rights reserved.
//
//***********************************************************************************************
// 
//***********************************************************************************************
#include <Vision/Samples/Engine/HavokAi/HavokAiPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiModule.hpp>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResource.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshResourceManager.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshInstance.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.hpp>
#include <Vision/Runtime/EnginePlugins/EnginePluginsImport.hpp>

VisSampleAppPtr spApp = NULL;
VGUIMainContextPtr spGUIContext;
VDialogPtr spMainDlg;
VisBaseEntity_cl* pCamera = NULL;
vHavokAiNavMeshInstance* pNavMeshInstance = NULL;
bool bLastShowGUI = false;

const int NUM_RIGID_BODIES = 20;
VisBaseEntity_cl* pRigidBodies[NUM_RIGID_BODIES];
hkvVec3 vStartPoint(-500.f, -500.f, 0.f);
hkvVec3 vEndPoint(500.f, 500.f, 0.f);

/// --------------------------------------------------------------------------- /// 
/// Input Controls                                                              ///
/// --------------------------------------------------------------------------- ///
enum HAVOKAI_CONTROL 
{
	HAVOKAI_SHOW_MENU = VISION_INPUT_CONTROL_LAST_ELEMENT+1
};

/// --------------------------------------------------------------------------- /// 
/// Implement a simple GUI dialog class that handles the picking                ///
/// --------------------------------------------------------------------------- ///
class VEntityPickDlg : public VDialog
{
public:

	VOVERRIDE void OnSetFocus(bool bStatus)
	{
		m_spCursor = GetMenuManager()->LoadCursorResource("GUI\\mouseNorm.TGA");
	}

	VOVERRIDE VCursor* GetCurrentCursor(VGUIUserInfo_t &user)
	{
		return m_spCursor;
	}

	VOVERRIDE void OnClick(VMenuEventDataObject *pEvent) 
	{
		VDialog::OnClick(pEvent);

 		hkvVec2 vMousePos = GetContext()->GetCurrentMousePos();
 		hkvVec3 vDir;
 		Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(vMousePos.x, vMousePos.y, vDir, 10000.f);
 		hkvVec3 vFrom = Vision::Camera.GetCurrentCameraPosition();
 		hkvVec3 vTo = vFrom + vDir;

		const float v2h = vHavokConversionUtils::GetVision2HavokScale();
		hkVector4 from; from.set( vFrom.x*v2h, vFrom.y*v2h, vFrom.z*v2h );
		hkVector4 to; to.set( vTo.x*v2h, vTo.y*v2h, vTo.z*v2h );

		hkaiNavMeshQueryMediator::HitDetails hit;
		hit.m_hitFaceKey = HKAI_INVALID_PACKED_KEY;
		hit.m_hitFraction = -1.f;
		if (vHavokAiModule::GetInstance()->GetAiWorld()->getDynamicQueryMediator()->castRay(from, to, hit))
		{
			hkVector4 hitPoint; hitPoint.setInterpolate(from, to, hit.m_hitFraction);
			hitPoint.mul4( vHavokConversionUtils::GetHavok2VisionScale() );
			if (pEvent->m_iButtons & BUTTON_LMOUSE)
			{
				vStartPoint.set( hitPoint(0), hitPoint(1), hitPoint(2) );				
			}
			else
			{
				vEndPoint.set( hitPoint(0), hitPoint(1), hitPoint(2) );
			}
		}
	}

	VCursorPtr m_spCursor;
};

VISION_INIT
{
	for (int i = 0; i < NUM_RIGID_BODIES; i++)
		pRigidBodies[i] = NULL;

	VISION_SET_DIRECTORIES(false);
	VisSampleApp::LoadVisionEnginePlugin();
	VISION_PLUGIN_ENSURE_LOADED(vHavokAi);
	VISION_HAVOK_SYNC_ALL_STATICS();
  
	// Create an application
	spApp = new VisSampleApp();
 
#if defined(_VISION_MOBILE) || defined( HK_ANARCHY )
  Vision::Renderer.SetUseSingleBufferedStaticMeshes(false);
	if (!spApp->InitSample("HavokAi" /*DataDir*/, "vScenes\\HavokAi" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS | VSAMPLE_FORCEMOBILEMODE ))
#else
	if (!spApp->InitSample("HavokAi" /*DataDir*/, "vScenes\\HavokAi" /*SampleScene*/, VSAMPLE_INIT_DEFAULTS & ~VSAMPLE_WIIU_DRCDEMO ))
#endif
		return false;

#if !defined(_VISION_MOBILE) && !defined( HK_ANARCHY )
  spApp->AddSampleDataDir("Models");
#endif

  if (vHavokAiModule::GetInstance()->LoadNavMeshDeprecated("vScenes\\HavokAi.NavMeshData\\navmesh_0.hkt"))
  {
    return true;
  }

	return false;
}

VISION_SAMPLEAPP_AFTER_LOADING
{
  // Override setting of scene
  vHavokAiModule::GetInstance()->SetConnectToPhysicsWorld(true);

#ifdef SUPPORTS_KEYBOARD
	VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_KEYBOARD, CT_KB_LALT);
	VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_KEYBOARD, CT_KB_RALT);

	spApp->AddHelpText("");
	spApp->AddHelpText("KEYBOARD - ALT : Show the cursor");
	spApp->AddHelpText("LEFT MOUSE CLICK : Set start position");
	spApp->AddHelpText("RIGHT MOUSE CLICK : Set end position");
#endif

#if defined (_VISION_XENON)|| (defined(_VISION_WINRT) && !defined(_VISION_METRO) && !defined(_VISION_APOLLO))

	VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_XENON_PAD(0), CT_PAD_LEFT_SHOULDER);

	spApp->AddHelpText("");
	spApp->AddHelpText("PAD1 - Left Shoulder : Show the cursor");
	spApp->AddHelpText("PAD1 - Right Thumbstick : Move camera/cursor");
	spApp->AddHelpText("PAD1 - A : Set start position");
	spApp->AddHelpText("PAD1 - B : Set end position");

#elif defined (_VISION_PS3)

	VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_PS3_PAD(0), CT_PAD_LEFT_SHOULDER);

	spApp->AddHelpText("");
	spApp->AddHelpText("PAD1 - Left Shoulder : Show the cursor");
	spApp->AddHelpText("PAD1 - Right Thumbstick : Move camera/cursor");
	spApp->AddHelpText("PAD1 - CROSS : Set start position");
	spApp->AddHelpText("PAD1 - CIRCLE : Set end position");

#elif defined (_VISION_PSP2)

	VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, V_PSP2_PAD(0), CT_PAD_LEFT_SHOULDER);

	spApp->AddHelpText("");
	spApp->AddHelpText("PAD - Left Shoulder : Show the cursor");
	spApp->AddHelpText("PAD - Right Thumbstick : Move camera/cursor");
	spApp->AddHelpText("PAD - CROSS : Set start position");
	spApp->AddHelpText("PAD - CIRCLE : Set end position");

#elif defined (_VISION_WIIU)

  VisSampleApp::GetInputMap()->MapTrigger(HAVOKAI_SHOW_MENU, VInputManagerWiiU::GetDRC(V_DRC_FIRST), CT_PAD_LEFT_SHOULDER);

  spApp->AddHelpText("");
  spApp->AddHelpText("DRC - Left Shoulder : Show the cursor");
  spApp->AddHelpText("DRC - Right Thumbstick : Move camera/cursor");
  spApp->AddHelpText("DRC - B : Set start position");
  spApp->AddHelpText("DRC - A : Set end position");

#endif

	// Create a GUI context
	spGUIContext = new VGUIMainContext(NULL);
	spMainDlg = new VEntityPickDlg();
	spMainDlg->InitDialog(spGUIContext,NULL,NULL);
	spGUIContext->SetActivate(false);

	pCamera = spApp->EnableMouseCamera();
	pCamera->SetPosition(484.f, 753.8f, 920.7f);
	pCamera->SetOrientation(-118.5f, 49.5f, 0.f);
}

VISION_SAMPLEAPP_RUN
{
	// Run the main loop of the application until we quit
	if (spApp->Run())
	{
		// Show / Hide GUI
		bool bShowGUI = VisSampleApp::GetInputMap()->GetTrigger(HAVOKAI_SHOW_MENU)!=0;
		if (bLastShowGUI!=bShowGUI)
		{
			if (bShowGUI)
				spGUIContext->ShowDialog(spMainDlg);

			spGUIContext->SetActivate(bShowGUI);
			pCamera->SetThinkFunctionStatus(!bShowGUI);
			bLastShowGUI = bShowGUI;
		}

		// Process rigid bodies
		static int nextBodyIndex = 0;
		static float baseTime = Vision::GetTimer()->GetCurrentTime();
		float currTime = Vision::GetTimer()->GetCurrentTime();
		if ((currTime-baseTime) > 1.f)
		{
			hkvVec3 vInitialPos(0.f, 0.f, 1500.f);
			vHavokRigidBody *pComponent;
			if (!pRigidBodies[nextBodyIndex])
			{
				pComponent = new vHavokRigidBody;
				pComponent->Initialize();
				pRigidBodies[nextBodyIndex] = Vision::Game.CreateEntity("VisBaseEntity_cl", vInitialPos, "Models\\ammobox3.model");
				hkvVec3 scaleFactor(2.f, 2.f, 2.f);
				pRigidBodies[nextBodyIndex]->SetScaling(scaleFactor);
				pRigidBodies[nextBodyIndex]->AddComponent(pComponent);
				pRigidBodies[nextBodyIndex]->SetCastShadows(true);
				pComponent->SetRestitution(0.1f);
			}
			else
			{
				pComponent = (vHavokRigidBody*)pRigidBodies[nextBodyIndex]->Components().GetComponentOfType( vHavokRigidBody::GetClassTypeId() );
				pComponent->SetPosition(vInitialPos);
				pRigidBodies[nextBodyIndex]->SetPosition(vInitialPos);
			}
			hkvVec3 zero(0.f, 0.f, 0.f);
			pComponent->SetLinearVelocity(zero);
			pComponent->SetAngularVelocity(zero);

			baseTime = currTime;
			if (++nextBodyIndex == NUM_RIGID_BODIES)
				nextBodyIndex = 0;
		}

		vHavokAiModule::GetInstance()->DebugRender(0.1f, false);

		IVRenderInterface *pRI = Vision::Contexts.GetMainRenderContext()->GetRenderInterface();
		vHavokAiModule::GetInstance()->ComputeAndDrawPath(pRI, &vStartPoint, &vEndPoint, 20.f, 1.75f, 0.2f, hkColor::YELLOW);

		return true;
	}
	return false;
}

VISION_DEINIT
{
	// DeInit GUI
	spMainDlg = NULL;
	spGUIContext->SetActivate(false);
	spGUIContext = NULL;
	VGUIManager::GlobalManager().CleanupResources();

	// Deinit the application
	spApp->DeInitSample();
	spApp = NULL;

	// Make sure to unload the engine plugins before the app is closed
	Vision::Plugins.UnloadAllEnginePlugins();
 
	// This should be done after Havok Physics has been de-initialized
	VISION_HAVOK_UNSYNC_ALL_STATICS();

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
