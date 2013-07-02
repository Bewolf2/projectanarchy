/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/PathCameraEntity.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VisCallback_cl PathCameraAction::OnTriggerEvent;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class PathParameter
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void PathParameter::Handle(float dtime)
{
  if (!m_pPath) {m_bFinished=true;return;}
  m_fCurrentTime += dtime;

  // move forward:
  if (m_fMaxParam>m_fMinParam)
  {
    m_fCurrentParam += dtime*m_fTimeScale;
    if (m_fCurrentParam>=m_fMaxParam) {m_fCurrentParam=m_fMaxParam;m_bFinished=true;return;}
  }
  else // backwards
  {
    m_fCurrentParam -= dtime*m_fTimeScale;
    if (m_fCurrentParam<=m_fMaxParam) {m_fCurrentParam=m_fMaxParam;m_bFinished=true;return;}
  }
}

        

void PathParameter::EvaluatePosition(hkvVec3& vPos, hkvMat3 *pCamRot)
{
  if (!m_pPath)
    return;
  if (pCamRot)
  {
    hkvVec3 vDirection(hkvNoInitialization), vRight(hkvNoInitialization);
    m_pPath->EvalPointSmooth(m_fCurrentParam, vPos,&vDirection);
    if (!vDirection.isZero ())
    {
      vDirection.normalizeIfNotZero(); // needed for next operation
      hkvVec3 vUp(0,0,1);
      if (hkvMath::isZero (vDirection.x) && hkvMath::isZero (vDirection.y)) // direction points into up direction, so make an alternative coordinate system
        vUp.set(1,0,0);

      if (m_fRolliness!=0.f)
      {
        float fPathStep = 0.1f / m_fTime; // Speed, basically
        hkvVec3 vNextDirection(hkvNoInitialization);
        m_pPath->EvalPointSmooth(hkvMath::mod (m_fCurrentParam+fPathStep,1.0f), (hkvVec3&) vPos,&vNextDirection);
        vNextDirection.normalizeIfNotZero();
        vUp += (vNextDirection - vDirection) * m_fRolliness;
        vRight = vUp.cross(vDirection);
        vRight.normalizeIfNotZero();
        vUp = vDirection.cross(vRight);
        vUp.normalizeIfNotZero();

        pCamRot->setAxisXYZ (vDirection,vRight,vUp);
      } 
      else
      {
        ANALYSIS_IGNORE_WARNING_ONCE(6246)
        hkvVec3 vRight = vUp.cross(vDirection);
        vRight.normalizeIfNotZero();
        vUp = vDirection.cross(vRight);
        vUp.normalizeIfNotZero();
        pCamRot->setAxisXYZ (vDirection,vRight,vUp);
      }
    }
  }
  else
  {
    m_pPath->EvalPointSmooth(m_fCurrentParam, (hkvVec3&) vPos,NULL,NULL);
  }
}


void PathParameter::FromXMLNode(TiXmlElement *pNode)
{
  Clear();
  if (!pNode)
    return;

  XMLHelper::Exchange_Float(pNode,"start",m_fMinParam,false);
  XMLHelper::Exchange_Float(pNode,"end",m_fMaxParam,false);
  XMLHelper::Exchange_Float(pNode,"time",m_fTime,false);
  XMLHelper::Exchange_Float(pNode,"rolliness",m_fRolliness,false);
  
  m_fTimeScale = 1.f/m_fTime;
  m_fStartParam = m_fMinParam;
  XMLHelper::Exchange_Float(pNode,"position",m_fStartParam,false);
  const char *szPathKey = XMLHelper::Exchange_String(pNode,"key",NULL,false);
  if (szPathKey)
    m_pPath = Vision::Game.SearchPath(szPathKey);

  m_fCurrentParam = m_fStartParam;
  m_fCurrentTime = 0.f;
}



PathCameraAction::~PathCameraAction()
{
  V_SAFE_DELETE_ARRAY(m_pEventTime);
  for (int i=0;i<m_iEventCount;i++)
    V_SAFE_DELETE(m_pEventNode[i]);
  V_SAFE_DELETE_ARRAY(m_pEventTime);
  V_SAFE_DELETE_ARRAY(m_pEventNode);
}

void PathCameraAction::FromXMLNode(TiXmlElement *pNode)
{
  if (!pNode)
    return;
  m_PositionPath.FromXMLNode(XMLHelper::SubNode(pNode,"positionpath",false));
  m_LookatPath.FromXMLNode(XMLHelper::SubNode(pNode,"lookatpath",false));
  XMLHelper::Exchange_Float(pNode,"fadeintime",m_fFadeInTime,false);
  XMLHelper::Exchange_Float(pNode,"fadeouttime",m_fFadeOutTime,false);

  m_spFOVCurve = VCurve2D::Exchange_Curve(pNode,"fovcurve",NULL,false);
  if (m_spFOVCurve)
    m_spFOVCurve->CreateLookup(1024);
  XMLHelper::Exchange_Float(pNode,"fov",m_fDefaultFOV,false); // supersedes the global properties fov attribute

  // count events
  const char *szEventNode = "event";
  m_iEventCount = 0;
  for (TiXmlElement *pEventNode=pNode->FirstChildElement(szEventNode); pEventNode; pEventNode=pEventNode->NextSiblingElement(szEventNode) )
    m_iEventCount++;

  if (m_iEventCount>0)
  {
    // allocate the actions
    m_pEventTime = new float[m_iEventCount];
    m_pEventNode = new TiXmlElement *[m_iEventCount];

    // and parse them
    m_iEventCount=0;
    for (TiXmlElement *pEventNode=pNode->FirstChildElement(szEventNode); pEventNode; pEventNode=pEventNode->NextSiblingElement(szEventNode))
    {
      float fPos = -1.f;
      XMLHelper::Exchange_Float(pEventNode,"pos",fPos,false);
      if (fPos<0.f)
        continue;
      m_pEventTime[m_iEventCount] = fPos;
      m_pEventNode[m_iEventCount] = pEventNode->Clone()->ToElement();
      m_iEventCount++;
    }
  }
}


void PathCameraAction::EvaluatePosition(hkvVec3& vPos, hkvMat3 &camRot)
{
  if (m_LookatPath.m_pPath) // look at the other path
  {
    hkvVec3 vLookAt;
    m_PositionPath.EvaluatePosition(vPos,NULL);
    m_LookatPath.EvaluatePosition(vLookAt,NULL);
    camRot.setLookInDirectionMatrix (vLookAt - vPos);
  } 
  else // look in flight direction
  {
    m_PositionPath.EvaluatePosition(vPos,camRot.getPointer ());
  }
}

void PathCameraAction::Handle(PathCameraEntity *pOwner, float dtime)
{
  IVRendererNode* pRendererNode = Vision::Renderer.GetRendererNode(pOwner->m_iRendererNodeIndex);
  VASSERT(pRendererNode != NULL);

  const float fOldPos = m_PositionPath.m_fCurrentParam;
  m_PositionPath.Handle(dtime);
  m_LookatPath.Handle(dtime);

  // use the current path parameter for FOV curve lookup
  if (m_spFOVCurve)
  {
    float fFOVX = m_spFOVCurve->GetValueFast(m_PositionPath.m_fCurrentParam);

    pRendererNode->GetViewProperties()->setFov(fFOVX, 0.0f);
  }
  else if (m_fDefaultFOV>0.f)
  {
    pRendererNode->GetViewProperties()->setFov(m_fDefaultFOV, 0.0f);
  }

  pRendererNode->OnViewPropertiesChanged();

  // trigger events (forward and backwards
  for (int i=0;i<m_iEventCount;i++)
  {
    const float fStart = hkvMath::Min(fOldPos,m_PositionPath.m_fCurrentParam);
    const float fEnd = hkvMath::Max(fOldPos,m_PositionPath.m_fCurrentParam);
    if (fStart<m_pEventTime[i] && fEnd>=m_pEventTime[i])
    {
      VPathEventCallbackDataObject data(m_pEventNode[i],m_pEventTime[i],this);
      PathCameraAction::OnTriggerEvent.TriggerCallbacks(&data);
    }
  }

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class PathCameraEntity
//////////////////////////////////////////////////////////////////////////////////////////////////////////


PathCameraEntity::PathCameraEntity()
{
  m_fTimeScaling = 1.f;
  m_fStoreFOV[0] = m_fStoreFOV[1] = -1.f;
  m_iActionCount = 0;
  m_pActions = NULL;
  m_iCurrentAction = -1;
  m_bPaused = m_bPlayLooped = false;
  AttachToRendererNode(0);
  
}

void PathCameraEntity::InitFunction()
{
  SetUseEulerAngles(false);
}

void PathCameraEntity::AttachToRendererNode(int iRendererNodeIndex)
{
  m_iRendererNodeIndex = iRendererNodeIndex;
}

void PathCameraEntity::TickFunction(float dtime)
{
  if (m_iCurrentAction < 0)
  {
    SetThinkFunctionStatus(false);
    return;
  }

  dtime *= m_fTimeScaling;
  VASSERT(m_pActions && m_iCurrentAction < m_iActionCount);
  PathCameraAction &action = m_pActions[m_iCurrentAction];
  action.Handle(this, dtime);
  float fBlendWeight = action.GetBlendWeight();
  if (fBlendWeight<1.f)
  {
    int its = (int)(fBlendWeight*255.99f);
    m_spFadeMask->SetColor(VColorRef(its,its,its,255-its));
    m_spFadeMask->SetVisible(true);
  } 
  else
  {
    m_spFadeMask->SetVisible(false);
  }

  // evaluate and set new camera position
  hkvVec3 vPos = GetPosition();
  hkvMat3 camRot = GetRotationMatrix();
  action.EvaluatePosition(vPos,camRot);
  SetPosition(vPos);
  SetRotationMatrix(camRot);

  if (action.IsFinished())
  {
    m_iCurrentAction++;
    if (m_iCurrentAction>=m_iActionCount)
    {
      if (m_bPlayLooped)
        Start();
      else
        Stop();
    }

    Vision::Camera.ReComputeVisibility(); // teleport
  }
}


void PathCameraEntity::ThinkFunction()
{
  float dtime = Vision::GetTimer()->GetTimeDifference();
  TickFunction(dtime);
}

void PathCameraEntity::DeInitFunction()
{
  FreeScriptFile();

  // restore the old FOV
  IVRendererNode* pNode = Vision::Renderer.GetRendererNode(m_iRendererNodeIndex);
  if(pNode && m_fStoreFOV[0] > 0.0f)
  {
    pNode->GetViewProperties()->setFov(m_fStoreFOV[0], m_fStoreFOV[1]);
    pNode->OnViewPropertiesChanged();
  }
}


bool PathCameraEntity::LoadScriptFile(const char *szFilename)
{
  if (VStringUtil::IsEmpty(szFilename))
    szFilename = m_sScriptFile;
  else
    m_sScriptFile = szFilename;

  FreeScriptFile(); // disable think functions

  if (VStringUtil::IsEmpty(szFilename))
    return false;
  
  TiXmlDocument doc;
  if (!doc.LoadFile(m_sScriptFile,Vision::File.GetManager()))
    return false;

  if (!doc.RootElement())
    return false;

  // base properties
  float fDefaultFOV = -1.f;
  TiXmlElement *pProperties = XMLHelper::SubNode(doc.RootElement(),"properties",false);
  if (pProperties)
  {
    XMLHelper::Exchange_Bool(pProperties,"looped",m_bPlayLooped,false);
    XMLHelper::Exchange_Bool(pProperties,"paused",m_bPaused,false);
    XMLHelper::Exchange_Float(pProperties,"fov",fDefaultFOV,false);
  }

  const char *szActionNode = "cameraaction";
  // count the camera actions in the XML
  for (TiXmlElement *pNode=doc.RootElement()->FirstChildElement(szActionNode); pNode; pNode=pNode->NextSiblingElement(szActionNode) )
    m_iActionCount++;

  if (!m_iActionCount)
    return false;

  // allocate the actions
  m_pActions = new PathCameraAction[m_iActionCount];
  // and parse them
  int i=0;
  m_iActionCount = 0;
  for (TiXmlElement *pNode=doc.RootElement()->FirstChildElement(szActionNode); pNode; pNode=pNode->NextSiblingElement(szActionNode),i++ )
  {
    PathCameraAction &action = m_pActions[i];
    action.FromXMLNode(pNode);
    if (action.m_fDefaultFOV<0.f && fDefaultFOV>0.f)
      action.m_fDefaultFOV = fDefaultFOV;
    if (action.IsValid()) // only count valid actions
      m_iActionCount++;
  }

  // all actions sorted out?
  if (m_iActionCount<1)
  {
    Vision::Error.Warning("Camera script file '%s' does not contain valid camera actions. Please check path keys.", m_sScriptFile.AsChar());
    return false;
  }

  if (Vision::Editor.IsInEditor() && Vision::Editor.IsPlayingTheGame() && !IsPaused())
    Start();

  return true;
}


void PathCameraEntity::FreeScriptFile()
{
  SetThinkFunctionStatus(false);
  m_iCurrentAction = -1;
  m_iActionCount = 0;
  m_bPlayLooped = false;
  V_SAFE_DELETE_ARRAY(m_pActions);
}


bool PathCameraEntity::Start()
{
  if (m_iActionCount<=0)
  {
    SetThinkFunctionStatus(false);
    return false;
  }

  // store the FOV so we can restore it
  IVRendererNode* pNode = Vision::Renderer.GetRendererNode(m_iRendererNodeIndex);
  VASSERT(pNode != NULL);

  m_fStoreFOV[0] = pNode->GetViewProperties()->getFovX();
  m_fStoreFOV[1] = pNode->GetViewProperties()->getFovY();
  
  VisContextCamera_cl *pCamera = pNode->GetReferenceContext()->GetCamera();

  if (!m_spFadeMask)
  {
    m_spFadeMask = new VisScreenMask_cl("plainwhite.dds");
    m_spFadeMask->SetTargetSize((float)Vision::Video.GetXRes(),(float)Vision::Video.GetYRes());
    m_spFadeMask->SetPos(0,0);
    m_spFadeMask->SetDepthWrite(FALSE);
    m_spFadeMask->SetTransparency(VIS_TRANSP_MULTIPLICATIVE);
    m_spFadeMask->SetVisible(FALSE);
    m_spFadeMask->SetUserData(m_spFadeMask); // render in main scene [HACK]
  }

  for (int i=0;i<m_iActionCount;i++)
    m_pActions[i].Reset();

  m_iCurrentAction = 0;
  SetThinkFunctionStatus(true);
  m_bPaused = false;

  pCamera->AttachToEntity(this, hkvVec3::ZeroVector ());
  TickFunction(0.f); // get current position and rotation
  return true;
}

void PathCameraEntity::Stop()
{
  m_iCurrentAction = -1;
  SetThinkFunctionStatus(false);
  if (m_spFadeMask)
    m_spFadeMask->SetVisible(FALSE);
}

void PathCameraEntity::SetPause(bool bStatus)
{
  SetThinkFunctionStatus(!bStatus);
  m_bPaused = bStatus;
}




V_IMPLEMENT_SERIAL( PathCameraEntity, VisBaseEntity_cl, 0, &g_VisionEngineModule );
void PathCameraEntity::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    SetUseEulerAngles(false);
    SetThinkFunctionStatus(false);
    int iVersion;
    ar >> iVersion; VASSERT(iVersion==0 && "Unsupported version number");
    ar >> m_sScriptFile;
    bool bStart;
    ar >> bStart;
  } else
  {
    ar << (int)0; // version
    ar.WriteStringBinary(m_sScriptFile);
    ar << true;
  }
}


void PathCameraEntity::OnDeserializationCallback(const VSerializationContext &context)
{
  VisBaseEntity_cl::OnDeserializationCallback(context);

  LoadScriptFile(NULL);
  if (Vision::Editor.IsPlayingTheGame() && !IsPaused())
    Start();
}


// hide this entity class in vForge because there is a dedicated C# shape type
START_VAR_TABLE(PathCameraEntity, VisBaseEntity_cl, "PathCameraEntity", VFORGE_HIDECLASS, "")  
END_VAR_TABLE


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class CameraPositionEntity
//////////////////////////////////////////////////////////////////////////////////////////////////////////


V_IMPLEMENT_SERIAL( CameraPositionEntity, VisBaseEntity_cl, 0, &g_VisionEngineModule );
void CameraPositionEntity::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT(iLocalVersion==0 && "Unsupported version number");
    ar >> NearClipDistance >> FarClipDistance >> FovX;
  } else
  {
    ar << iLocalVersion; // version
    ar << NearClipDistance << FarClipDistance << FovX;
  }
}

void CameraPositionEntity::ApplyToContext(VisRenderContext_cl *pContext)
{
  VASSERT(pContext!=NULL && pContext->GetCamera()!=NULL);

  // apply position and transformation
  pContext->GetCamera()->AttachToEntity(this,hkvVec3::ZeroVector ());

  // apply view settings
  float fNear,fFar;
  pContext->GetClipPlanes(fNear,fFar);
  if (NearClipDistance>0.f)
    fNear = NearClipDistance;
  if (FarClipDistance>0.f)
    fFar = FarClipDistance;
  if (fFar<=fNear) fFar = fNear+0.001f;
  pContext->SetClipPlanes(fNear,fFar);
  if (FovX>0.f)
  {
    float FovY = 0.0f;
    int iWidth, iHeight;
    pContext->GetSize(iWidth, iHeight);

    hkvMathHelpers::adjustFovsForAspectRatio(FovX, FovY, float(iWidth) / float(iHeight));

    pContext->SetFOV(FovX, FovY);
  }

}

CameraPositionEntity* CameraPositionEntity::ApplyToContext(VisRenderContext_cl *pContext, const char *szKey)
{
  VisBaseEntity_cl *pEnt = Vision::Game.SearchEntity(szKey);
  if (pEnt==NULL || !pEnt->IsOfType(V_RUNTIME_CLASS(CameraPositionEntity)))
    return NULL;
  CameraPositionEntity *pCam = (CameraPositionEntity *)pEnt;
  pCam->ApplyToContext(pContext);
  return pCam;
}


// hide this entity class in vForge because there is a dedicated C# shape type
START_VAR_TABLE(CameraPositionEntity, VisBaseEntity_cl, "CameraPositionEntity", VFORGE_HIDECLASS, "")  
  DEFINE_VAR_FLOAT(CameraPositionEntity, NearClipDistance, "Custom near clip distance (or 0)", "0.0", 0, 0);
  DEFINE_VAR_FLOAT(CameraPositionEntity, FarClipDistance, "Custom far clip distance (or 0)", "0.0", 0, 0);
  DEFINE_VAR_FLOAT(CameraPositionEntity, FovX, "Custom fov (or 0)", "0.0", 0, 0);
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
