/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VPlayerCamera.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#if !defined( HK_ANARCHY )
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Postprocessing/VPostProcessDepthOfField.hpp>
#endif

/// =========================================================================== ///
/// CAMERA CONSTANTS                                                            ///
/// =========================================================================== ///

#define ANGLE_PITCH_MIN 0.0f
#define ANGLE_PITCH_MAX 89.0f


/// =========================================================================== ///
/// MOUSE CONTROL                                                               ///
/// =========================================================================== ///

enum CAMERA_CONTROL 
{
  CAMERA_HORIZONTAL_MOVEMENT,
  CAMERA_VERTICAL_MOVEMENT,
  CAMERA_ZOOM,

  CAMERA_NUM_ELEMENTS
};

#define INPUTMAP_CAMERA_ALTERNATIVES   4

VPlayerCamera::VPlayerCamera(int iComponentFlags) : IVObjectComponent(0, iComponentFlags)
{
  VisionApp_cl::OnUpdatePhysicsFinished += this;
  Vision::Callbacks.OnEditorModeChanged += this;

  // Initial values
  Enabled = true;
  Zoom = true;
  Follow = true;
  Collides = true;
  DepthOfField = false;
  FollowFixed = false;

  InitialYaw = 0.0f;
  InitialPitch = 30.0f;
  MinimalDistance = 200.f;
  MaximalDistance = 1000.f;
  CameraDistance = 500.f;
  RelativeLookAtHeight = 0.5;

  CameraSensitivity = 200.0f;
  MoveSmoothness = 1.0f;


  // Create input map
  m_pInputMap = new VInputMap(CAMERA_NUM_ELEMENTS, INPUTMAP_CAMERA_ALTERNATIVES);
  
  // Map mouse and touch input depending on physical touch/cursor distance.

#if defined( SUPPORTS_MOUSE )

  m_pInputMap->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, V_MOUSE, CT_MOUSE_ABS_DELTA_X,   VInputOptions::Sensitivity(0.18f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   V_MOUSE, CT_MOUSE_ABS_DELTA_Y,   VInputOptions::Sensitivity(0.18f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_ZOOM,                V_MOUSE, CT_MOUSE_WHEEL,         VInputOptions::Sensitivity(MOUSE_WHEEL_SCALE_FACTOR / 2));

#endif

#if defined( _VISION_MOBILE )

  VTouchArea* pTouchArea = new VTouchArea(VInputManager::GetTouchScreen(), VRectanglef(), -2000.0f);
  m_pInputMap->MapTrigger(CAMERA_HORIZONTAL_MOVEMENT, pTouchArea, CT_TOUCH_ABS_DELTA_X, VInputOptions::Sensitivity(0.3f / Vision::Video.GetDeviceDpi()));
  m_pInputMap->MapTrigger(CAMERA_VERTICAL_MOVEMENT,   pTouchArea, CT_TOUCH_ABS_DELTA_Y, VInputOptions::Sensitivity(0.3f / Vision::Video.GetDeviceDpi()));

#endif

  // Map button/key/joystick based input - since we accumulate the values every frame, we need to timescale these inputs to achieve a frame rate independent behavior.

  VInputOptions camMoveOptions;
  camMoveOptions.m_bTimeScaled = true;
  camMoveOptions.m_fSensitivity = 0.5f;
  camMoveOptions.m_fDeadZone = 0.2f;

  VInputOptions camZoomOptions;
  camZoomOptions.m_bTimeScaled = true;
  camZoomOptions.m_fSensitivity = 3.0f;

#if defined ( SUPPORTS_KEYBOARD )

  m_pInputMap->MapTriggerAxis(CAMERA_VERTICAL_MOVEMENT,   V_KEYBOARD, CT_KB_KP_8, CT_KB_KP_2, camMoveOptions);
  m_pInputMap->MapTriggerAxis(CAMERA_HORIZONTAL_MOVEMENT, V_KEYBOARD, CT_KB_KP_4, CT_KB_KP_6, camMoveOptions);
  m_pInputMap->MapTriggerAxis(CAMERA_ZOOM,                V_KEYBOARD, CT_KB_PGDN, CT_KB_PGUP, camZoomOptions);

#endif

  // Map the same input for all pad-like devices
  IVInputDevice* padDevices[] =
  {
#if defined (_VISION_XENON) || defined (_VISION_PS3) || defined (_VISION_PSP2) || defined(WIN32)
    VInputManager::IsInitialized() ? &V_PAD(0) : NULL,
#elif defined(_VISION_WIIU)
    &VInputManagerWiiU::GetDRC(V_DRC_FIRST),
    &VInputManagerWiiU::GetRemote(0),
#endif
    NULL
  };

  for(int iDeviceIdx = 0; iDeviceIdx < V_ARRAY_SIZE(padDevices) - 1; iDeviceIdx++)
  {
    if(padDevices[iDeviceIdx] == NULL)
    {
      continue;
    }

    m_pInputMap->MapTriggerAxis(CAMERA_VERTICAL_MOVEMENT,   *padDevices[iDeviceIdx], CT_PAD_RIGHT_THUMB_STICK_UP,   CT_PAD_RIGHT_THUMB_STICK_DOWN,  camMoveOptions);
    m_pInputMap->MapTriggerAxis(CAMERA_HORIZONTAL_MOVEMENT, *padDevices[iDeviceIdx], CT_PAD_RIGHT_THUMB_STICK_LEFT, CT_PAD_RIGHT_THUMB_STICK_RIGHT, camMoveOptions);
    m_pInputMap->MapTriggerAxis(CAMERA_ZOOM,                *padDevices[iDeviceIdx], CT_PAD_LEFT_TRIGGER,           CT_PAD_RIGHT_TRIGGER,           camZoomOptions);
  }
}

VPlayerCamera::~VPlayerCamera()
{
  Vision::Callbacks.OnEditorModeChanged -= this;
  VisionApp_cl::OnUpdatePhysicsFinished -= this;

  V_SAFE_DELETE(m_pInputMap);

  Enabled = false;
  UpdateAttachment();
}

void VPlayerCamera::SetEnabled(BOOL bStatus) 
{
	Enabled = bStatus;
	UpdateAttachment(); 
}

void VPlayerCamera::UpdateAttachment()
{
  VisContextCamera_cl* pMainCamera = Vision::Camera.GetMainCamera();
  if(Enabled && Vision::Editor.IsPlayingTheGame() && m_pOwner)
  {
    if(!m_spCameraProxy)
    {
      m_spCameraProxy = Vision::Game.CreateEntity("VisBaseEntity_cl", hkvVec3 (0.f, 0.f, 0.f));
      m_spCameraProxy->SetEntityKey("<VPlayerCamera:CameraProxy>");

      pMainCamera->AttachToEntity(m_spCameraProxy, hkvVec3(0.0f, 0.0f, 0.0f));

      SetCamera(InitialYaw, InitialPitch, CameraDistance, RelativeLookAtHeight);
    }
  }
  else
  {
    if(m_spCameraProxy)
    {
      if(pMainCamera->GetParent() == m_spCameraProxy)
      {
        pMainCamera->AttachToEntity(NULL, hkvVec3(0.0f, 0.0f, 0.0f));
      }
      m_spCameraProxy = NULL;
    }
  }
}

BOOL VPlayerCamera::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
  {
    return false;
  }

  if (!pObject->IsOfType(VisBaseEntity_cl::GetClassTypeId()))
  {
    sErrorMsgOut = "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VPlayerCamera::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  UpdateAttachment();
}

void VPlayerCamera::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if(strcmp(pVar->GetName(), "Enabled") == 0)
  {
    UpdateAttachment();
  }
}

V_IMPLEMENT_SERIAL(VPlayerCamera,IVObjectComponent,0,&g_VisionEngineModule);

void VPlayerCamera::Serialize( VArchive &ar )
{
  char iLocalVersion = PLAYERCAMERACOMPONENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion <= PLAYERCAMERACOMPONENT_VERSION_CURRENT && iLocalVersion >= PLAYERCAMERACOMPONENT_VERSION_0, "Invalid local version. Please re-export");

    ar >> Enabled;
    ar >> Collides;
    ar >> Follow;
    ar >> Zoom;

    ar >> InitialYaw;
    ar >> InitialPitch;
    ar >> CameraDistance;
    ar >> MinimalDistance;
    ar >> MaximalDistance;
    ar >> RelativeLookAtHeight;

    ar >> CameraSensitivity;
    ar >> MoveSmoothness;

    if (iLocalVersion >= PLAYERCAMERACOMPONENT_VERSION_3)
      ar >> FollowFixed;
  } 
  else
  {
    ar << iLocalVersion;

    ar << Enabled;
    ar << Collides;
    ar << Follow;
    ar << Zoom;

    ar << InitialYaw;
    ar << InitialPitch;
    ar << CameraDistance;
    ar << MinimalDistance;
    ar << MaximalDistance;
    ar << RelativeLookAtHeight;

    ar << CameraSensitivity;
    ar << MoveSmoothness;
    ar << FollowFixed;
  }
}


void VPlayerCamera::OnDeserializationCallback(const VSerializationContext &context)
{
  UpdateAttachment();
}

/// =========================================================================== ///
/// CALLBACK HANDLER OVERRIDES                                                  ///
/// =========================================================================== ///

void VPlayerCamera::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if(pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
  {
    UpdateAttachment();
    return;
  }

  if(pData->m_pSender == &VisionApp_cl::OnUpdatePhysicsFinished)
  {
    if (!m_spCameraProxy)
    {
      return;
    }

    float fStepX = CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_HORIZONTAL_MOVEMENT);
    float fStepY = CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_VERTICAL_MOVEMENT);
    float fStepZoom = (Zoom ? 1.0f : 0.0f) * CameraSensitivity * m_pInputMap->GetTrigger(CAMERA_ZOOM);

    // Step camera move
    SetCameraStep(fStepX, fStepY, fStepZoom, 0.f); 
  }
}


/// =========================================================================== ///
/// PLAYER CAMERA COMPONENT                                                     ///
/// =========================================================================== ///

void VPlayerCamera::SetCamera(float fYaw, float fPitch, float fDistance, float fHeightRelative)
{
  if (!m_pOwner || !Enabled)
    return;

  // Set initial camera position and orientation
  InitialYaw = fYaw;
  InitialPitch = fPitch;

  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(m_pOwner);

  // Get position and bounding box size of entity
  VDynamicMesh *pMesh = pOwnerEntity->GetMesh();
  if (pMesh != NULL)
  {
    hkvAlignedBBox pVisBoundingBox;
    pMesh->GetCollisionBoundingBox(pVisBoundingBox);

    // Some models have an invalid collision bbox
    if (!pVisBoundingBox.isValid())
    {
      pMesh->GetVisibilityBoundingBox(pVisBoundingBox);
    }

    m_fRelativeLookAtHeight = pVisBoundingBox.getSizeZ() * hkvMath::saturate(fHeightRelative);
  }
  else
  {
    m_fRelativeLookAtHeight = 0.f;
  }

  // Set camera position and orientation

  // Get orientation from owner
  hkvVec3 vCamOri = pOwnerEntity->GetOrientation();
  vCamOri.x -= InitialYaw;
  vCamOri.y += InitialPitch;

  // Get position of entity
  hkvVec3 vPos = pOwnerEntity->GetPosition();

  // Set position of entity
  hkvVec3 vCamPos = hkvVec3 (vPos.x, vPos.y, vPos.z + m_fRelativeLookAtHeight);
  m_vOldPos = vCamPos;

  // Set direction of camera
  m_spCameraProxy->SetOrientation(vCamOri.x, vCamOri.y, 0.f); 
  hkvVec3 vCamDir = m_spCameraProxy->GetDirection() * CameraDistance;

  // Set final camera position
  m_spCameraProxy->SetPosition(vCamPos - vCamDir); 
}

void VPlayerCamera::SetCameraStep(float fYawStep, float fPitchStep, float fDistanceStep, float fHeightStep)
{
  if (!m_pOwner)
    return;

  // update camera position and orientation
  hkvVec3 vCamPos, vCamOri, vCamDir;

  VisBaseEntity_cl* pOwnerEntity = vstatic_cast<VisBaseEntity_cl*>(m_pOwner);

  if (Follow)
  {
    // Get orientation from owner
    vCamOri = pOwnerEntity->GetActualOrientation();
    vCamOri.x -= InitialYaw;
    vCamOri.y += InitialPitch;
  }
  else
  {
    // Get orientation from camera
    m_spCameraProxy->GetOrientation(vCamOri);
    vCamOri.x -= fYawStep;
    vCamOri.y += fPitchStep;
  }

  // Check angle limits
  if (vCamOri.y < ANGLE_PITCH_MIN)
    vCamOri.y = ANGLE_PITCH_MIN;
  else if (vCamOri.y > ANGLE_PITCH_MAX)
    vCamOri.y = ANGLE_PITCH_MAX;

  // Get position of entity
  hkvVec3 vPos = pOwnerEntity->GetPosition();
  vPos.z += m_fRelativeLookAtHeight;

  // Get orientation (from entity or custom)
  if (Follow)
  {
    // Calculate current blending weight
    float weight = Vision::GetTimer()->GetTimeDifference() / MoveSmoothness;  // Use Vision::GetUITimer() here?

    // apparently it is possible to have negative values in here, in some obscure configurations
    // not sure what would happen, if we were to clamp that to 0 then.
    weight = hkvMath::clamp(weight, HKVMATH_SMALL_EPSILON, 1.0f);

    hkvVec3 vOriBlend =  m_spCameraProxy->GetOrientation();
    hkvVec3 vPosBlend =  m_vOldPos;

    // Set blending position of camera
    vPosBlend += (vPos - vPosBlend) * weight;
    vCamPos = hkvVec3 (vPosBlend.x, vPosBlend.y, vPosBlend.z);
    m_vOldPos = vCamPos;

    // Set blending orientation of camera (YAW)
    if (hkvMath::Abs (vCamOri.x - vOriBlend.x) < 180.f)
      vOriBlend.x += (vCamOri.x - vOriBlend.x) * weight;
    else if ((vCamOri.x - vOriBlend.x) < 0.f)
      vOriBlend.x += (360.f - hkvMath::Abs (vCamOri.x - vOriBlend.x)) * weight;
    else if ((vCamOri.x - vOriBlend.x) > 0.f)
      vOriBlend.x -= (360.f - hkvMath::Abs (vCamOri.x - vOriBlend.x)) * weight;

    // Set blending orientation of camera (PITCH)
    vOriBlend.y += (vCamOri.y - vOriBlend.y) * weight;

    // Set direction of camera
    if (!FollowFixed)
      m_spCameraProxy->SetOrientation(vOriBlend.x, vOriBlend.y, vCamOri.z); 

    // Increment Distance
    if (Zoom)
      CameraDistance += fDistanceStep;
    else
      CameraDistance += (MinimalDistance - CameraDistance) * weight;

    // Check for valid input
    if (CameraDistance < MinimalDistance)
      CameraDistance = MinimalDistance;
    else if (CameraDistance > MaximalDistance)
      CameraDistance = MaximalDistance;

    vCamDir = m_spCameraProxy->GetDirection() * CameraDistance;
  }
  else
  {
    // Set position of entity
    vCamPos = hkvVec3 (vPos.x, vPos.y, vPos.z + m_fRelativeLookAtHeight);

    // Set direction of camera
    m_spCameraProxy->SetOrientation(vCamOri.x, vCamOri.y, 0.f); 

    // Increment Distance
    CameraDistance += fDistanceStep;

    // Check for valid input
    if (CameraDistance < MinimalDistance)
      CameraDistance = MinimalDistance;
    else if (CameraDistance > MaximalDistance)
      CameraDistance = MaximalDistance;

    vCamDir = m_spCameraProxy->GetDirection() * CameraDistance;
  }

  // Check for collision (camera with geometry)
  if (Collides)
  {
    VisPhysicsHit_t hitInfo;
    IVisPhysicsModule_cl* physicsModule = Vision::GetApplication()->GetPhysicsModule();
    if (physicsModule != NULL && physicsModule->Raycast(vCamPos, vCamPos - vCamDir, hitInfo))
    {
      bool bTouchedEntity = (hitInfo.eHitType == VIS_TRACETYPE_ENTITYBOX || hitInfo.eHitType == VIS_TRACETYPE_ENTITYPOLY);
      if (!bTouchedEntity)
      {
        vCamDir = vCamPos - hitInfo.vImpactPoint;
      }
    }
  }

  // Set final camera position
  m_spCameraProxy->SetPosition(vCamPos - vCamDir); 

#if defined( SUPPORTS_POSTPROCESSING ) && !defined( HK_ANARCHY )

  // Update Depth of Field focus
  if (DepthOfField && Vision::Renderer.GetRendererNode(0) != NULL)
  {
    VPostProcessDepthOfField *pDepthOfField = Vision::Renderer.GetRendererNode(0)->Components().GetComponentOfType<VPostProcessDepthOfField>();
    if (pDepthOfField)
      pDepthOfField->SetParams(CameraDistance, pDepthOfField->FocusDepth, pDepthOfField->BlurRadius);
  }

#endif
}

START_VAR_TABLE(VPlayerCamera,IVObjectComponent, "Player Camera Component. Can be attached to entities to observe their movement.", VVARIABLELIST_FLAGS_NONE, "Player Camera" )
  DEFINE_VAR_BOOL(VPlayerCamera, Enabled, "If enabled, camera is active when running the game", "TRUE", 0, NULL);
DEFINE_VAR_BOOL(VPlayerCamera, Collides, "If enabled, camera collides with the geometry", "FALSE", 0, NULL);
DEFINE_VAR_BOOL(VPlayerCamera, Follow, "If enabled, camera is aligned to the orientation of the parent entity", "FALSE", 0, NULL);
DEFINE_VAR_BOOL(VPlayerCamera, FollowFixed, "If enabled, camera is following the parent object with a fixed orientation", "FALSE", 0, NULL);
DEFINE_VAR_BOOL(VPlayerCamera, Zoom, "If enabled, camera can be zoomed with the mouse wheel", "TRUE", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, InitialYaw, "Initial yaw angle of the camera", "0.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, InitialPitch, "Initial pitch angle of the camera", "30.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, CameraDistance, "Initial distance of the camera to the parent entity", "500.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, MinimalDistance, "Minimal distance of the camera to the parent entity", "100.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, MaximalDistance, "Maximal distance of the camera to the parent entity", "1000.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, RelativeLookAtHeight, "Relative look-at height of the parent entity the camera looks at", "0.5", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, CameraSensitivity, "Camera movement sensitivity (does not affect follower camera)", "200.0", 0, NULL);
DEFINE_VAR_FLOAT(VPlayerCamera, MoveSmoothness, "Camera movement ease in/out (affects only follower camera)", "1.0", 0, NULL);
END_VAR_TABLE

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
