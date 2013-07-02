/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL( VModelPreviewComponent, IVObjectComponent, 0, &g_VisionEngineModule );

VModelPreviewComponent::VModelPreviewComponent(int iId, int iComponentFlags) : IVObjectComponent(iId, iComponentFlags)
{
  m_fYawSpeed = 0.f;
}

VModelPreviewComponent::VModelPreviewComponent(const char * pszName, int iComponentFlags) : IVObjectComponent(pszName, iComponentFlags)
{
  m_fYawSpeed = 0.f;
}

VModelPreviewComponent::~VModelPreviewComponent()
{
  Vision::Contexts.RemoveContext(m_spContext);

  m_spEntity = NULL;
  m_spCamera = NULL;

  m_spRenderTarget = NULL;
  m_spDepthStencilTarget = NULL;
  m_spContext = NULL;
}

void VModelPreviewComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = 0;
  int iResX=0, iResY=0;
  float fFOVX=0.f, fFOVY=0.f;

  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");

    ar >> iResX >> iResY >> fFOVX >> fFOVY;
    if (iResX>0)
    {
      CreateRenderTarget(iResX,iResY);
      m_spContext->SetFOV(fFOVX,fFOVY);
    }
    
    VisBaseEntity_cl *pEntity = NULL;
    ar >> pEntity >> m_fYawSpeed;
    for (int i=0;i<6;i++)
      m_LightGridColors[i].SerializeAsVec3 (ar);

    SetPreviewEntity(pEntity);
  } else
  {
    ar << iLocalVersion;
    if (m_spContext)
    {
      m_spContext->GetSize(iResX,iResY);
      m_spContext->GetFinalFOV(fFOVX,fFOVY);
    }

    ar << iResX << iResY << fFOVX << fFOVY;

    ar << m_spEntity.GetPtr() << m_fYawSpeed;
    for (int i=0;i<6;i++)
      m_LightGridColors[i].SerializeAsVec3 (ar);

  }
}

bool VModelPreviewComponent::CreateRenderTarget(int iResX, int iResY)
{
  // create the renderable texture
  VisRenderableTextureConfig_t config;
  config.m_iWidth = iResX;
  config.m_iHeight = iResY;
  config.m_eFormat = VTextureLoader::DEFAULT_RENDERTARGET_FORMAT_32BPP;

  VString name = GetComponentName()==NULL ? "ModelPreviewComponent" : GetComponentName();
  VString texturePrefix = "<";

  m_spRenderTarget = Vision::TextureManager.CreateRenderableTexture(texturePrefix + name + ">",config);
  if (!m_spRenderTarget)
  {
    VASSERT(!"Failed to create render target");
    return false;
  }

  // create the depth stencil texture
  config.m_eFormat = VVideo::GetSupportedDepthStencilFormat(VTextureLoader::D24S8, *Vision::Video.GetCurrentConfig());
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  m_spDepthStencilTarget = Vision::TextureManager.CreateRenderableTexture(texturePrefix + name + "_ds>",config);
  if (!m_spDepthStencilTarget)
  {
    VASSERT(!"Failed to create depth stencil target");
    return false;
  }

  // register a render context
  m_spCamera = new VisContextCamera_cl();
  m_spContext = new VisRenderContext_cl();
  m_spContext->SetName(name);
  m_spContext->SetCamera(m_spCamera);
  m_spContext->SetRenderTarget(0, m_spRenderTarget);
  m_spContext->SetDepthStencilTarget(m_spDepthStencilTarget);

  if ( m_spEntity )
    m_spContext->SetVisibilityCollector(new VModelPreviewVisCollector(m_spEntity));
  int iRenderFlags = VIS_RENDERCONTEXT_FLAGS_SECONDARYCONTEXT;
  m_spContext->SetRenderFlags(iRenderFlags);
  VModelPreviewRenderLoop *pRenderLoop = new VModelPreviewRenderLoop(this);
  m_spContext->SetRenderLoop(pRenderLoop);

  m_spRenderTarget->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  // register the render context
  Vision::Contexts.AddContext(m_spContext);
  m_spContext->SetPriority(VIS_RENDERCONTEXTPRIORITY_MIRROR);

  return true;
}


bool VModelPreviewComponent::InitComponent(int iResX, int iResY, float fFovH, float fFovV)
{
  return InitComponent(iResX, iResY, iResX, iResY, fFovH, fFovV);
}

bool VModelPreviewComponent::InitComponent(int iResX, int iResY, int iPresentationSizeX, int PresentationSizeY, float fFovH, float fFovV)
{
#if !defined(_VISION_MOBILE)
  CreateEmptyLightGrid();
#endif

  // track directional lightgrid color
  hkvVec3 vLightDir(0.5f,-0.5f,0.5f);
  vLightDir.normalizeIfNotZero();
  VLightGrid_cl::GetDirectionalLightColors(m_LightGridColors, vLightDir, hkvVec3(1.f,1.f,1.f), hkvVec3(0.3f,0.3f,0.3f));

  if (!CreateRenderTarget(iResX, iResY))
    return false;

  // if one FOV angle is undefined, calculate FOV from the presentation size (rather than context resolution)
  if(fFovH == 0)
  {
    fFovH = hkvMathHelpers::getFovX(fFovV, float(iPresentationSizeX) / float(PresentationSizeY));
  } else if(fFovV == 0)
  {
    fFovV = hkvMathHelpers::getFovY(fFovH, float(iPresentationSizeX) / float(PresentationSizeY));
  }

  m_spContext->SetFOV(fFovH, fFovV);
  SetClipPlaneFromExtent();
  
  SetFitModel();

  return true;
}

void VModelPreviewComponent::SetPreviewEntity(VisBaseEntity_cl *pEntity) 
{
  VASSERT( pEntity );
  m_spEntity = pEntity;
  if ( m_spContext )
  {
    IVisVisibilityCollector_cl* pVisColl = m_spContext->GetVisibilityCollector();
    if ( ! pVisColl )
    {
      m_spContext->SetVisibilityCollector(new VModelPreviewVisCollector(m_spEntity));
    }else 
    {
      VModelPreviewVisCollector* pVisCollModel = (VModelPreviewVisCollector*) pVisColl;
      pVisCollModel->SetEntity( pEntity );
    }
  }
}

void VModelPreviewComponent::SetClipPlaneFromExtent()
{
  if (IsAvailable() == FALSE)
    return;

  VisBaseEntity_cl *pEnt = GetPreviewEntity();
  hkvAlignedBBox modelBBox;
  pEnt->GetMesh()->GetVisibilityBoundingBox(modelBBox);
  const float extent = modelBBox.getMaxExtent();
  const float farClipPlane = hkvMath::Max(extent * 100.f, 100.f);
  const float nearClipPlane = hkvMath::clamp(extent * 0.01f, 0.001f, 8.f);

  m_spContext->SetClipPlanes(nearClipPlane,farClipPlane);
}

void VModelPreviewComponent::SetFitModel()
{
  if (IsAvailable() == FALSE)
    return;

  VisBaseEntity_cl *pEnt = GetPreviewEntity();
  
  hkvAlignedBBox modelbox;
  pEnt->GetMesh()->GetVisibilityBoundingBox(modelbox);
  hkvVec3 vPos = -modelbox.getCenter();

  float fAngleH,fAngleV;
  m_spContext->GetFOV(fAngleH, fAngleV);

  float dx = hkvMath::Max(modelbox.getSizeX(),modelbox.getSizeY());
  float zx = 0.5f*dx / hkvMath::tanDeg (fAngleH*0.5f);
  float zy = 0.5f*modelbox.getSizeZ() / hkvMath::tanDeg (fAngleV*0.5f);
  vPos.x = hkvMath::Max(zx,zy) + dx*0.5f;
  pEnt->SetPosition(vPos);
}


void VModelPreviewComponent::Update(float dtime)
{
  if (m_spEntity)
  {
    // mimic some of the scene update stuff here:
    Vision::Game.SetUpdateSceneCount( Vision::Game.GetUpdateSceneCount() + 1 );
    m_spEntity->IncOrientation(dtime*m_fYawSpeed,0,0);
    m_spEntity->HandleAnimations(dtime);
  }
}

void VModelPreviewComponent::CreateEmptyLightGrid()
{
  // create a dummy light grid...
  if (!Vision::RenderLoopHelper.GetLightGrid())
  {
    float r = 1000.f;
    hkvAlignedBBox box(hkvVec3 (-r,-r,-r), hkvVec3 (r,r,r));
    VLightGrid_cl *pLG = VLightGrid_cl::CreateFullbrightLightGrid(box);
    Vision::RenderLoopHelper.SetLightGrid(pLG);
  }
}

BOOL VModelPreviewComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  if (pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component cannot be attached to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut);
}

//////////////////////////////////////////////////////////////////////////////////////
// render loop implementation
//////////////////////////////////////////////////////////////////////////////////////
void VModelPreviewRenderLoop::OnDoRenderLoop(void *pUserData)
{
  Vision::RenderLoopHelper.ClearScreen();
  VisBaseEntity_cl *pEntity = m_pOwnerComp->GetPreviewEntity();

  static VisEntityCollection_cl prepareEntity(1,1);
  
  if (pEntity != NULL && pEntity->GetMesh())
  {
    prepareEntity.Clear();
    prepareEntity.AppendEntry(pEntity);
    prepareEntity.PrepareForRendering();

    VisShaderSet_cl *pShaderSet = pEntity->GetActiveShaderSet();
    // either render entity with shader set
    if (pShaderSet)
    {
      const VisDrawCallInfo_t *pSurfaceShaderList = NULL;
      int iNumSurfaceShaders = pShaderSet->GetShaderAssignmentList(&pSurfaceShaderList);
	    
      if (iNumSurfaceShaders && pSurfaceShaderList)
      {
        Vision::RenderLoopHelper.TrackLightGridInfo(m_pOwnerComp->GetLightGridColors()); // alternatively track constant directional light here
        Vision::RenderLoopHelper.RenderEntityWithSurfaceShaderList(pEntity, iNumSurfaceShaders, pSurfaceShaderList);
      }
    }
    else
    {
      VASSERT(FALSE && "Entity does not have a shader set!");
    }
  }
}

VModelPreviewVisCollector::VModelPreviewVisCollector(VisBaseEntity_cl *pEnt)
{
  m_pEntities = new VisEntityCollection_cl(1,1);
  if (pEnt != NULL)
    m_pEntities->AppendEntry(pEnt);
}

VModelPreviewVisCollector::~VModelPreviewVisCollector()
{
  V_SAFE_DELETE(m_pEntities);
}

//////////////////////////////////////////////////////////////////////////////////////
// VModelPreviewControl
//////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VModelPreviewControl, VPushButton, 0, &g_VisionEngineModule );
void VModelPreviewControl::Serialize( VArchive &ar )
{
  VPushButton::Serialize(ar);

  if(m_pComponent==NULL)
  {
    m_pComponent = new VModelPreviewComponent();
    AddComponent(m_pComponent);
  }

  m_pComponent->Serialize(ar);
}

bool VModelPreviewControl::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!VPushButton::Build(pNode,szPath,bWrite))
    return false;

  int iResolution[2] = {256,256};
  float fFOV[2] = {90.f,0.f};
  TiXmlElement *pRTNode = XMLHelper::SubNode(pNode,"rendertarget",bWrite);
  if (pRTNode)
  {
    XMLHelper::Exchange_Ints(pRTNode,"resolution",iResolution,2,bWrite);
    XMLHelper::Exchange_Floats(pRTNode,"fov",fFOV,2,bWrite);
  }

  if(m_pComponent!=NULL) 
  {
    RemoveComponent(m_pComponent);
    m_pComponent->DeleteThis();
  }
  m_pComponent = new VModelPreviewComponent();
  AddComponent(m_pComponent);

#if !defined(_VISION_MOBILE)
  m_pComponent->CreateEmptyLightGrid();
#endif
  

  TiXmlElement *pPreviewNode = XMLHelper::SubNode(pNode,"preview",bWrite);
  if (pPreviewNode)
  {
    const char *szModel = XMLHelper::Exchange_String(pPreviewNode,"model",NULL,bWrite);
    const char *szAnimation = XMLHelper::Exchange_String(pPreviewNode,"animation",NULL,bWrite);
    const char *szEntityClass = XMLHelper::Exchange_String(pPreviewNode,"entityclass",NULL,bWrite);
    hkvVec3 vPos(100.f,0,0);
    hkvVec3 vOri(0,0,0);
    bool bFit = true;
    XMLHelper::Exchange_Floats(pPreviewNode,"position",vPos.data,3,bWrite);
    XMLHelper::Exchange_Floats(pPreviewNode,"orientation",vOri.data,3,bWrite);
    XMLHelper::Exchange_Bool(pPreviewNode,"fit",bFit,bWrite);
    float fYawSpeed;
    XMLHelper::Exchange_Float(pPreviewNode,"yawspeed",fYawSpeed,bWrite);
    m_pComponent->SetYawSpeed(fYawSpeed);
    if (szModel)
    {
      VisBaseEntity_cl *pEnt = Vision::Game.CreateEntity( szEntityClass?szEntityClass:"VisBaseEntity_cl",vPos,szModel);
      VASSERT(pEnt);
      if (szAnimation)
      {
        VisAnimConfig_cl::StartSkeletalAnimation(pEnt, szAnimation, VANIMCTRL_LOOP|VSKELANIMCTRL_DEFAULTS);
        pEnt->SetAlwaysUpdateAnimations(true);
      }
      pEnt->SetOrientation(vOri);
      m_pComponent->SetPreviewEntity(pEnt);
    }
  }

  bool bRetVal = m_pComponent->InitComponent(iResolution[0], iResolution[1], (int)GetSize().x, (int)GetSize().y, fFOV[0], fFOV[1]);
  if(!bRetVal)
    return false;

  m_ButtonCfg.SetTexture(m_pComponent->GetRenderableTexture());

  return true;
}

void VModelPreviewControl::OnTick(float dtime)
{
  VPushButton::OnTick(dtime);
  VASSERT_MSG(m_pComponent!=NULL, "Could not find model preview component");
  m_pComponent->Update(dtime);
}

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
