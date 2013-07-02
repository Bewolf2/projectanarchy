/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFpsCameraEntity.cpp


#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h> // precompiled header
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VFpsCameraEntity.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VMouseCamera.hpp>

// speed things
#define FPSPLAYER_MOVE_SPEED            350.0f * Vision::World.GetGlobalUnitScaling()
#define FPSPLAYER_LOOK_SENSITIVITY      200.0f

// bounding box related defines
#define FPSPLAYER_BBOX_WIDTH            36.0f
#define FPSPLAYER_BBOX_BREADTH          20.0f
#define FPSPLAYER_BBOX_HEIGHT_STAND     90.0f // low due to Hangar scene.
#define FPSPLAYER_BBOX_TOP_OFFSET       5.0f  // dont want eye height too low too..

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class VFpsCameraEntity
//////////////////////////////////////////////////////////////////////////////////////////////////////////


VFpsCameraEntity::VFpsCameraEntity()
  : m_pCamera( NULL )
  , m_pMouseCamInputMap( NULL )
  , m_pPhys( NULL )
  , m_fFov( 0.0f )
  , m_fTimeDiff( 0.0f )
  , m_fHalfScreenSizeX( 0.0f )
  , m_fHalfScreenSizeY( 0.0f )
  , m_bbStand()
{
}

void VFpsCameraEntity::InitFunction()
{
  CommonInit();
}


void VFpsCameraEntity::ThinkFunction()
{
  // get time difference from last frame
  m_fTimeDiff = Vision::GetTimer()->GetTimeDifference();

  // get current field of view (to adjust mouse sensitivity)
  float t;
  Vision::Contexts.GetMainRenderContext()->GetFOV(m_fFov,t);

  //// handle movement input
  Vision::Camera.AttachToEntity(m_pCamera, hkvVec3(0, 0, 0));
  HandleInput();
}

void VFpsCameraEntity::DeInitFunction()
{
}

//void VFpsCameraEntity::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
//{
//  VisBaseEntity_cl::MessageFunction(iID,iParamA,iParamB);
//}

V_IMPLEMENT_SERIAL( VFpsCameraEntity, VisBaseEntity_cl, 0, &g_vHavokModule );
void VFpsCameraEntity::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    // anything to deserialize...?
    CommonInit();
  } 
  else
  {
    // anything to serialize...?
  }
}

void VFpsCameraEntity::SetupPhysics()
{
  // set up bounding boxes
  m_bbStand.m_vMin.x = -(FPSPLAYER_BBOX_BREADTH/2);
  m_bbStand.m_vMax.x =  (FPSPLAYER_BBOX_BREADTH/2);
  m_bbStand.m_vMin.y = -(FPSPLAYER_BBOX_WIDTH  /2);
  m_bbStand.m_vMax.y =  (FPSPLAYER_BBOX_WIDTH  /2);
  m_bbStand.m_vMin.z = -(FPSPLAYER_BBOX_HEIGHT_STAND - FPSPLAYER_BBOX_TOP_OFFSET);
  m_bbStand.m_vMax.z =  (FPSPLAYER_BBOX_TOP_OFFSET);

  m_pPhys = new vHavokCharacterController();
  m_pPhys->Initialize();
  m_pPhys->Capsule_Radius = FPSPLAYER_BBOX_WIDTH;
  m_pPhys->Character_Top    = hkvVec3(0,0,  m_bbStand.getSizeZ()*0.1f);
  m_pPhys->Character_Bottom = hkvVec3(0,0, -m_bbStand.getSizeZ()*0.9f);
  m_pPhys->Max_Slope = 75.0f;
  m_pPhys->SetPosition(GetPosition());
  //m_pPhys->SetPivotOffset(hkvVec3(0,0,6000));

  AddComponent(m_pPhys);
}

void VFpsCameraEntity::HandleInput()
{
  ////////////////////////////////////////
  // look around (mouse and analog pad)
  float dx = m_pMouseCamInputMap->GetTrigger(API_CAMERA_HORIZONTAL_LOOK);
  float dy = m_pMouseCamInputMap->GetTrigger(API_CAMERA_VERTICAL_LOOK);
  float factor = m_fInitialFov / m_fFov;

  m_pCamera->IncOrientation(-dx * FPSPLAYER_LOOK_SENSITIVITY / factor, dy * FPSPLAYER_LOOK_SENSITIVITY / factor, 0);

  const float fMaxSpeed = FPSPLAYER_MOVE_SPEED;

  // constrain movement to the ground plane by setting the Z axis to 0
  hkvVec3 vForward = m_pCamera->GetObjDir();
  vForward.z = 0.0f;
  vForward.normalize();

  // Accumulate move directions (multiply in order to take analog input into account).
  hkvVec3 vMove(hkvVec3::ZeroVector());
  vMove += vForward * m_pMouseCamInputMap->GetTrigger(API_CAMERA_MOVE_FORWARD);
  vMove -= vForward * m_pMouseCamInputMap->GetTrigger(API_CAMERA_MOVE_BACKWARD);
  vMove -= m_pCamera->GetObjDir_Right() * m_pMouseCamInputMap->GetTrigger(API_CAMERA_MOVE_RIGHT);
  vMove += m_pCamera->GetObjDir_Right() * m_pMouseCamInputMap->GetTrigger(API_CAMERA_MOVE_LEFT);
  vMove *= fMaxSpeed;

  // Clamp movement, so that moving diagonally is not faster than moving straight 
  // when using digital input.
  const float fSpeed = vMove.getLength();
  if (fSpeed > fMaxSpeed)
    vMove.setLength(fMaxSpeed);
  vMove *= Vision::GetTimer()->GetTimeDifference();

  SetMotionDeltaLocalSpace(vMove);
}

bool VFpsCameraEntity::ContactsGround()
{
  class CheckGroundRaycastFirstResult : public VisPhysicsRaycastBase_cl
  {
  public:
    VFpsCameraEntity* const pPlayerEntity;
    bool bHit;

    virtual void Reset()
    {
      VisPhysicsRaycastBase_cl::Reset();
      bHit = false;
    }

    bool onHit(VisPhysicsHit_t &hit)
    {
      if(!bHit && hit.pHitObject!=pPlayerEntity)
        bHit  = true;
      return !bHit;
    }

    bool allHits()
    {
      return false;
    }

    CheckGroundRaycastFirstResult(VFpsCameraEntity* PlayerEntity) : pPlayerEntity(PlayerEntity)
    {
      bHit  = false;
    }
  };

  float BBoxHeight;
  CheckGroundRaycastFirstResult Result(this);
  Result.Reset();

  BBoxHeight  = FPSPLAYER_BBOX_HEIGHT_STAND;

  Result.vRayStart = m_pPhys->GetPosition();// + hkvVec3(0.0f, 0.0f, -(BBoxHeight+FPSPLAYER_BBOX_EPSILON) * 0.5f );
  Result.vRayEnd = Result.vRayStart - hkvVec3( 0.0f, 0.0f, BBoxHeight*0.70f);

  Vision::GetApplication()->GetPhysicsModule()->PerformRaycast(&Result);
  return Result.bHit;
}

void VFpsCameraEntity::CommonInit()
{
  // initialise values
  float t;
  Vision::Contexts.GetMainRenderContext()->GetFOV(m_fInitialFov,t);
  m_fFov = 0;
  m_fTimeDiff = 0.0f;

  m_fHalfScreenSizeX = (float)(Vision::Video.GetXRes()>>1);
  m_fHalfScreenSizeY = (float)(Vision::Video.GetYRes()>>1);

  // create and attach a CameraEntity_cl object if we are playing the game (not in editing mode)
  if (Vision::Editor.IsPlayingTheGame())
  {
    hkvVec3 v3Pos = GetPosition();

    // grab camera
    m_pCamera = Vision::Game.CreateEntity("VisBaseEntity_cl", v3Pos);
    m_pCamera->AttachToParent(this);
    m_pCamera->ResetLocalTransformation();    
  }

#if defined( _VISION_MOBILE )
  // as screen resolution is incorporated twice into camera movement
  // we divide our screen factor by the square of the content scale
  // factor (which is 2 if we are running on a retina display and
  // 1 otherwise)
  float fContentScaleFactorSquared = hkvMath::Max( 1.0f, hkvMath::floor( Vision::Video.GetVideoConfig()->DisplayDensity ) );
  fContentScaleFactorSquared *= fContentScaleFactorSquared;
  m_fHalfScreenSizeX /= fContentScaleFactorSquared;
  m_fHalfScreenSizeY /= fContentScaleFactorSquared;
#endif

  // search for a mouse camera to hijack its input map
  for ( unsigned int i = 0; i < VisBaseEntity_cl::ElementManagerGetSize(); ++i )
  {
    VisBaseEntity_cl *pEntity = VisBaseEntity_cl::ElementManagerGetAt( i );
    if ( pEntity->GetTypeId() == VisMouseCamera_cl::GetClassTypeId() )
    {
      VisMouseCamera_cl *pMouseCam = static_cast< VisMouseCamera_cl* >( pEntity );
      m_pMouseCamInputMap = pMouseCam->GetInputMap();
      break;
    }
  }

  // HACK!!! Retrieve our initial position via a specific entity
  VisBaseEntity_cl *pEntity = Vision::Game.SearchEntity( "FpsCamInitPos" );
  if ( pEntity )
  {
    SetPosition( pEntity->GetPosition() );
    if ( m_pCamera )
      m_pCamera->SetOrientation( pEntity->GetOrientation() );
  }

  SetupPhysics();
}


START_VAR_TABLE(VFpsCameraEntity, VisBaseEntity_cl, "VFpsCameraEntity", 0, "")  
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
