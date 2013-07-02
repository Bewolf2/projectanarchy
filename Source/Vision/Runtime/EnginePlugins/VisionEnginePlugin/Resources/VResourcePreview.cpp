/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/Engine/Renderer/RenderLoop/VisionRenderLoop.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Resources/VResourcePreview.hpp>

#ifdef WIN32

class VAnimationResourcePreview : public VResourcePreview
{
public:
  VAnimationResourcePreview(VManagedResource *pOwner);
private:
  VAnimationResourcePreview(const VAnimationResourcePreview&);
  VAnimationResourcePreview& operator=(const VAnimationResourcePreview&);

public:
  virtual void OnRender(int iFlags) HKV_OVERRIDE;
};


class VFontResourcePreview : public VResourcePreview
{
public:
  VFontResourcePreview(VisFont_cl *pOwner);
private:
  VFontResourcePreview(const VFontResourcePreview&);
  VFontResourcePreview& operator=(const VFontResourcePreview&);

public:
  virtual void OnRender(int iFlags) HKV_OVERRIDE;
};


class VMeshBufferResourcePreview : public VResourcePreview
{
public:
  VMeshBufferResourcePreview(VManagedResource *pOwner);
private:
  VMeshBufferResourcePreview(const VMeshBufferResourcePreview&);
  VMeshBufferResourcePreview& operator=(const VMeshBufferResourcePreview&);

public:
  virtual void OnRender(int iFlags) HKV_OVERRIDE;

protected:
  void RenderStreamOutput(VisMeshBuffer_cl *pMesh, VBaseMesh *pSubMeshes=NULL);

protected:
  int m_iMaterialCount;
  int m_iSubmeshCount;
  int m_iTriCount;
  int m_iVertexCount;
};


class VBaseMeshResourcePreview : public VMeshBufferResourcePreview
{
public:
  VBaseMeshResourcePreview(VManagedResource *pOwner);
private:
  VBaseMeshResourcePreview(const VBaseMeshResourcePreview&);
  VBaseMeshResourcePreview& operator=(const VBaseMeshResourcePreview&);

public:
  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnDeActivate() HKV_OVERRIDE;
  virtual bool OnUpdate(float fTimeDiff) HKV_OVERRIDE;
  virtual void OnRender(int iFlags) HKV_OVERRIDE;
  virtual bool SupportsThumbnails() HKV_OVERRIDE;

private:
  void RenderMesh(VBaseMesh *pMesh, hkvVec3 light0, hkvVec3 light1, hkvVec3 light2);

private:
  float m_fYaw;

  VCompiledTechniquePtr m_spPreviewTechnique;
  int m_iLight0;
  int m_iLight1;
  int m_iLight2;

  VisTextureAnimInstancePtr *m_spTexAnims;
};


class VParticleFXResourcePreview : public VResourcePreview
{
public:
  VParticleFXResourcePreview(VManagedResource *pOwner);
private:
  VParticleFXResourcePreview(const VParticleFXResourcePreview&);
  VParticleFXResourcePreview& operator=(const VParticleFXResourcePreview&);

public:
  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnDeActivate() HKV_OVERRIDE;
  virtual bool OnUpdate(float fTimeDiff) HKV_OVERRIDE;
  virtual void OnRender(int iFlags) HKV_OVERRIDE;

  virtual bool SupportsThumbnails() HKV_OVERRIDE;
  virtual bool SupportsImageThumbnails() HKV_OVERRIDE;
  virtual VString GetThumbnailFilename() HKV_OVERRIDE;

private:
  hkvVec3 m_vLastPos;
  hkvVec3 m_vBoundingBoxCenter;
  VisParticleGroupCollection_cl m_Layers;
  VSmartPtr<VisParticleEffect_cl> m_spInst;
};


class VShaderFXLibResourcePreview : public VResourcePreview
{
public:
  VShaderFXLibResourcePreview(VManagedResource *pOwner);
private:
  VShaderFXLibResourcePreview(const VShaderFXLibResourcePreview&);
  VShaderFXLibResourcePreview& operator=(const VShaderFXLibResourcePreview&);

public:
  virtual void OnRender(int iFlags) HKV_OVERRIDE;
};


class VTexture2DResourcePreview : public VResourcePreview
{
public:
  VTexture2DResourcePreview(VManagedResource *pOwner, VisTextureAnimInstance_cl *pTexAnim);
private:
  VTexture2DResourcePreview(const VTexture2DResourcePreview&);
  VTexture2DResourcePreview& operator=(const VTexture2DResourcePreview&);

protected:
  virtual VTextureObject *GetTexture();

public:
  virtual bool OnUpdate(float fTimeDiff) HKV_OVERRIDE;
  virtual void OnRender(int iFlags) HKV_OVERRIDE;

private:
  VisTextureAnimInstancePtr m_spAnimInst;
};


class VBitmapResourcePreview : public VTexture2DResourcePreview
{
public:
  VBitmapResourcePreview(VManagedResource *pOwner);
private:
  VBitmapResourcePreview(const VBitmapResourcePreview&);
  VBitmapResourcePreview& operator=(const VBitmapResourcePreview&);

public:
  virtual VTextureObject *GetTexture() HKV_OVERRIDE;

private:
  int m_iLastX,m_iLastY;
  VTextureObjectPtr m_spUploaded;
};


class VTextureCubemapResourcePreview : public VResourcePreview
{
public:
  VTextureCubemapResourcePreview(VManagedResource *pOwner);
private:
  VTextureCubemapResourcePreview(const VTextureCubemapResourcePreview&);
  VTextureCubemapResourcePreview& operator=(const VTextureCubemapResourcePreview&);

public:
  virtual void OnActivate() HKV_OVERRIDE;
  virtual void OnRender(int iFlags) HKV_OVERRIDE;

private:
  VDynamicMeshPtr m_spSphere;
  VCompiledTechniquePtr m_spTech;
  VCompiledTechniquePtr m_spForegroundFillPassTechnique;
};

#endif //WIN32

////////////////////////////////////////////////////////////////////////////////////////////////
// VResourcePreviewRenderloop
////////////////////////////////////////////////////////////////////////////////////////////////

VResourcePreviewRenderloop::VResourcePreviewRenderloop(IVResourcePreview *pPreview, VColorRef clearColor)
: m_pPreview(pPreview), m_ClearColor(clearColor)
{
}

void VResourcePreviewRenderloop::OnDoRenderLoop(void *pUserData)
{
  Vision::RenderLoopHelper.ClearScreen(0, m_ClearColor);

  // trigger rendering (nothing else to render into this view)
  m_pPreview->OnRender(RESOURCEPREVIEWFLAG_FOR_THUMBNAIL);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

bool VResourcePreview::m_bPauseAnimation = false;
float VResourcePreview::m_fDistance = 0.f;
bool VResourcePreview::m_bOverwriteSceneCamera = false;

VColorRef VResourcePreview::m_thumbnailBackgroundColor(32, 32, 32, 0);
int VResourcePreview::m_iThumbnailResolutionX = 256;
int VResourcePreview::m_iThumbnailResolutionY = 256;
bool VResourcePreview::m_bSaveThumbnailsAsJPG = true;
bool VResourcePreview::m_bThumbnailRotateMeshes = true;


VResourcePreview::VResourcePreview(VManagedResource *pOwner) : IVResourcePreview(pOwner)
{
}


IVResourcePreview *VResourcePreview::CreateResourcePreview(VManagedResource *pResource)
{
#ifdef WIN32
  if (pResource == NULL)
    return NULL;

  IVResourcePreview *pPreview = pResource->CreateResourcePreview();
  if (pPreview != NULL)
    return pPreview;

  VResourceManager *pManager = pResource->GetParentManager();
  if (pManager == NULL)
    return NULL;

  // manager pools:
  if (pManager==&Vision::TextureManager.GetManager())
  {
    VTextureObject *pTexture = (VTextureObject *)pResource;

    if (pTexture->GetTextureType() == VTextureLoader::Texture2D)
      return new VTexture2DResourcePreview(pTexture, Vision::TextureManager.GetAnimationInstance(pTexture));
    if (pTexture->GetTextureType() == VTextureLoader::Cubemap)
      return new VTextureCubemapResourcePreview(pTexture);
  }
  if (pManager==&Vision::Shaders.GetShaderFXLibManager())
    return new VShaderFXLibResourcePreview(pResource);
  if (pManager==&Vision::Fonts)
    return new VFontResourcePreview((VisFont_cl *)pResource);
  if (pManager==&VisBitmap_cl::GetGlobalManager())
    return new VBitmapResourcePreview(pResource);
  if (pManager==VMeshManager::GetMeshManager())
  {
    VBaseMesh *pBaseMesh = (VBaseMesh *)pResource;
    return new VBaseMeshResourcePreview(pResource);
  }
  if (pManager==&VisMeshBuffer_cl::GetResourceManager())
    return new VMeshBufferResourcePreview(pResource);
  if (pManager==Vision::Animations.GetSequenceSetManager())
    return new VAnimationResourcePreview(pResource);
  if (pManager==&VisParticleGroupManager_cl::GlobalManager())
    return new VParticleFXResourcePreview(pResource);
#endif //WIN32

  return NULL;
}


bool VResourcePreview::SupportsImageThumbnails()
{
  // If thumbnails are at all supported, and no custom thumbnail override exists, we save as image thumbnails.
  return SupportsThumbnails() && !SupportsCustomThumbnails();
}


bool VResourcePreview::SaveImageThumbnail(const char* szFileName, int iResolutionX, int iResolutionY, VColorRef* backgroundColor)
{
  // Query the resource preview about preferred values, but override these if any preferences
  // were specified in the function call parameters.
  VString sFileName;
  int iResX = GetDefaultThumbnailResolutionX();
  int iResY = GetDefaultThumbnailResolutionY();
  QueryImageThumbnailParameters(sFileName, iResX, iResY);
  if (!VStringUtil::IsEmpty(szFileName))
  {
    sFileName = szFileName;
  }
  if (iResolutionX > 0)
  {
    iResX = iResolutionX;
  }
  if (iResolutionY > 0)
  {
    iResY = iResolutionY;
  }

  VColorRef defaultBackgroundColor = GetThumbnailBackgroundColor();
  if (backgroundColor == NULL)
    backgroundColor = &defaultBackgroundColor;

  // If any of the settings is still not valid, fail.
  if (sFileName.IsEmpty() || iResX < 1 || iResY < 1)
  {
    return false;
  }

  // Add file extension if not already available (currently only jpg and bmp is supported)
  if (!VFileHelper::HasExtension(sFileName, "jpg") && !VFileHelper::HasExtension(sFileName, "bmp"))
    sFileName += GetSaveThumbnailsAsJPG() ? ".jpg" : ".bmp";

  // create render targets
  VisRenderableTextureConfig_t config;
  config.m_iHeight = iResX;
  config.m_iWidth  = iResY;
  config.m_iMultiSampling = 0; // optionally enable MS here (e.g. set to 2, 4,...)
  config.m_eFormat = VTextureLoader::B8G8R8A8;
  VisRenderableTexturePtr spRT =  Vision::TextureManager.CreateRenderableTexture("<Thumbnail>",config);
  if (spRT==NULL)
    return false;

  config.m_iMultiSampling = 0;
  config.m_bResolve = false;
  config.m_eFormat = VTextureLoader::D24S8;
  config.m_bRenderTargetOnly = true;
  config.m_bIsDepthStencilTarget = true;
  VisRenderableTexturePtr spDS = Vision::TextureManager.CreateRenderableTexture("<Thumbnail_ds>",config);
  if (spDS==NULL)
    return false;

  VisRenderContextPtr spContext(new VisRenderContext_cl());
  VResourcePreviewRenderloop *pRL = new VResourcePreviewRenderloop(this, *backgroundColor);
  spContext->SetRenderLoop(pRL);
  spContext->SetCamera(Vision::Contexts.GetMainRenderContext()->GetCamera()); // share camera for same view angles
  spContext->SetViewProperties(Vision::Contexts.GetMainRenderContext()->GetViewProperties());

  spDS->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  spRT->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  spContext->SetRenderTarget(0,spRT);
  spContext->SetDepthStencilTarget(spDS);

  // render:
  bool bSaved = false;

  spContext->Activate();
  VisRenderContext_cl::GlobalTick();
  Vision::Renderer.BeginRendering();
  Vision::RenderSceneHelper();
  Vision::Renderer.EndRendering();

  bSaved = SaveRenderTarget(spRT, sFileName);

  // release
  Vision::Contexts.GetMainRenderContext()->Activate();
  spContext = NULL;

  return bSaved;
}


void VResourcePreview::QueryImageThumbnailParameters(VString &sAbsDestFilenameOut, int &iPreferredResXOut, int &iPreferredResYOut)
{
  sAbsDestFilenameOut = GetThumbnailFilename();
}


VString VResourcePreview::GetThumbnailFilename()
{
  if (!Vision::Editor.IsInEditor())
  {
    return VString();
  }

  VString sAbsFileName;
  if (!Vision::Editor.MakePathAbsoluteForProject(GetOwner()->GetFilename(), sAbsFileName))
  {
    return VString();
  }

  VString sFinalFileName;
  sFinalFileName.Format("%s_data\\Thumbnail.%s", sAbsFileName.AsChar(), GetSaveThumbnailsAsJPG() ? "jpg" : "bmp");

  return sFinalFileName;
}


bool VResourcePreview::ThumbnailAlreadySaved()
{
  VString fileName = GetThumbnailFilename();
  if (fileName.IsEmpty() || !VFileHelper::Exists(fileName))
  {
    return false;
  }

  return true;
}


bool VResourcePreview::DeleteThumbnail()
{
  VString fileName = GetThumbnailFilename();
  if (fileName.IsEmpty() || !VFileHelper::Exists(fileName))
  {
    return false;
  }

  return VFileHelper::Delete(fileName);
}


void VResourcePreview::PauseAnimation(bool bPause)
{
  m_bPauseAnimation = bPause;
}

bool VResourcePreview::IsAnimationPaused()
{
  return m_bPauseAnimation;
}


void VResourcePreview::IncrementDistance(float fDelta)
{
  m_fDistance -= fDelta*hkvMath::sqrt(m_fDistance);
  if (m_fDistance <= 1.0f)
    m_fDistance = 1.0f;
}


void VResourcePreview::SetDistance(float fValue)
{
  m_fDistance = fValue;
  if (m_fDistance <= 1.0f)
    m_fDistance = 1.0f;
}


float VResourcePreview::GetDistance(void)
{
  return m_fDistance;
}


void VResourcePreview::SetOverwriteSceneCamera(bool bValue)
{
  m_bOverwriteSceneCamera = bValue;
}


bool VResourcePreview::GetOverwriteSceneCamera()
{
  return m_bOverwriteSceneCamera;
}


VColorRef VResourcePreview::GetThumbnailBackgroundColor()
{
  return m_thumbnailBackgroundColor;
}

void VResourcePreview::SetThumbnailBackgroundColor(VColorRef color)
{
  m_thumbnailBackgroundColor = color;
}


int VResourcePreview::GetDefaultThumbnailResolutionX()
{
  return m_iThumbnailResolutionX;
}

void VResourcePreview::SetDefaultThumbnailResolutionX(int iResX)
{
  m_iThumbnailResolutionX = iResX;
}


int VResourcePreview::GetDefaultThumbnailResolutionY()
{
  return m_iThumbnailResolutionY;
}

void VResourcePreview::SetDefaultThumbnailResolutionY(int iResY)
{
  m_iThumbnailResolutionY = iResY;
}


bool VResourcePreview::GetSaveThumbnailsAsJPG()
{
  return m_bSaveThumbnailsAsJPG;
}

void VResourcePreview::SetSaveThumbnailsAsJPG(bool bAsJPG)
{
  m_bSaveThumbnailsAsJPG = bAsJPG;
}


bool VResourcePreview::GetThumbnailRotateMeshes()
{
  return m_bThumbnailRotateMeshes;
}

void VResourcePreview::SetThumbnailRotateMeshes(bool bRotate)
{
  m_bThumbnailRotateMeshes = bRotate;
}


void VResourcePreview::LookAtBBox(hkvMat4 &transform, float fYaw, const hkvAlignedBBox &bbox, const hkvVec3& offset)
{
  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();

  // center to bounding box center
  hkvMat4 bboxOffset;
  bboxOffset.setTranslation(-bbox.getCenter());

  // rotation around bounding box center
  hkvMat3 rot (hkvNoInitialization);
  rot.setFromEulerAngles (0, 0, fYaw);
  hkvMat4 rotation;
  rotation.setIdentity ();
  rotation.setRotationalPart(rot);

  // position in front of the camera
  hkvMat4 position;
  //////////////////////////////////////////////////////////////////////////
  // TODO: correct this when hkvVec3 is replaced by hkvVec3 inside VisContextCamera_cl
  hkvVec3 pos(pContext->GetCamera()->GetPosition());
  // const hkvVec3& pos = pContext->GetCamera()->GetPosition();
  hkvVec3 dir(pContext->GetCamera()->GetDirection());
  // const hkvVec3& dir = pContext->GetCamera()->GetDirection();
  position.setTranslation(pos + dir * m_fDistance + offset);
  //////////////////////////////////////////////////////////////////////////

  // apply
  transform.setIdentity();
  transform = transform.multiply(position);
  transform = transform.multiply(rotation);
  transform = transform.multiply(bboxOffset);
}

void VResourcePreview::UpdateDistanceFromBBox(const hkvAlignedBBox& bbox)
{
  VisRenderContext_cl *pMain = Vision::Contexts.GetMainRenderContext();

  float fFar,fNear;  
  pMain->GetClipPlanes(fNear, fFar);

  float fFovX, fFovY;
  pMain->GetFOV(fFovX, fFovY);

  SetDistance(bbox.getBoundingSphere().m_fRadius / hkvMath::tanDeg(fFovX * 0.5f) + fNear);
}


void VResourcePreview::TrackDirectionalLight()
{
  static hkvVec3 lgColors[6];
  VLightGrid_cl::GetDirectionalLightColors(lgColors,hkvVec3(0.5f,0.5f,0.71f),hkvVec3(0.7f,0.7f,0.7f),hkvVec3(0.4f,0.4f,0.4f));
  Vision::RenderLoopHelper.TrackLightGridInfo(lgColors);
}


bool VResourcePreview::SaveRenderTarget(VisRenderableTexture_cl *pTex, const char *szAbsFilename)
{
  VFileAccessManager::RCSEditFile(szAbsFilename);

  if (pTex==NULL)
    return false;

  int sx = pTex->GetTextureWidth();
  int sy = pTex->GetTextureHeight();

  Image_cl image;
  static VMemoryTempBuffer<512*512*3> buffer(sx*sy*3);
  UBYTE *pDest = (UBYTE *)buffer.GetBuffer();
  Vision::Game.WriteScreenToBuffer(0,0,sx,sy,pDest,pTex);
  UBYTE szLine[4];
  // flip vertically:
  int iStride = sx*3;
  for (int y=0;y<sy/2;y++)
  {
    UBYTE *l1 = &pDest[y*iStride];
    UBYTE *l2 = &pDest[(sy-1-y)*iStride];
    for (int x=0;x<sx;x++,l1+=3,l2+=3)
    {
      szLine[0] = l1[0]; szLine[1] = l1[1]; szLine[2] = l1[2]; 
      l1[0] = l2[2]; l1[1] = l2[1]; l1[2] = l2[0]; // flip components
      l2[0] = szLine[2]; l2[1] = szLine[1]; l2[2] = szLine[0]; // flip components        
    }
  }
  image.AddColorMap(sx,sy,COLORDEPTH_24BPP,pDest);

  int iSavingResult = -1;

  if (VFileHelper::HasExtension(szAbsFilename,"BMP"))
    iSavingResult = image.SaveBMP(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"JPG"))
    iSavingResult = image.SaveJPEG(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"JPEG"))
    iSavingResult = image.SaveJPEG(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"TGA"))
    iSavingResult = image.SaveTGA(szAbsFilename);
  else if (VFileHelper::HasExtension(szAbsFilename,"DDS"))
    iSavingResult = image.SaveUncompressedDDS(szAbsFilename);

  //VFileAccessManager::RCSAddFile(szAbsFilename);

  return iSavingResult == 0;
}

#if defined(WIN32)

////////////////////////////////////////////////////////////////////////////////////////////////
// Preview Globals
////////////////////////////////////////////////////////////////////////////////////////////////

const VColorRef iOverlayBGColor(20,20,20,220);
const VSimpleRenderState_t iAlphaState(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_NOWIREFRAME|RENDERSTATEFLAG_ALWAYSVISIBLE);
const float fStartY = 48.f;

#define PRINT_TEXT(_pos,_szbuffer,_color) \
  if (iDryRun==0) \
  {\
    VRectanglef textRect;\
    pFont->GetTextDimension(_szbuffer,textRect);\
    textRect+=_pos;\
    bgRect.Add(textRect);\
  }\
  else pFont->PrintText(pRI, _pos,_szbuffer, _color);

#define DRAW_BACKGROUND() \
{\
  if (iDryRun==1 && bgRect.IsValid())\
  {\
    bgRect.Add(4.f);\
    pRI->DrawSolidQuad(bgRect.m_vMin,bgRect.m_vMax, iOverlayBGColor, iAlphaState);\
  }\
}

#define DRAW_WITH_BACKGROUND \
  VRectanglef bgRect;\
  for (int iDryRun=0;iDryRun<2;iDryRun++)\
  {\
    DRAW_BACKGROUND();


#define PRINT_STREAM(_checkMember,_name,_member) \
  if (descr._checkMember) \
  {\
    sprintf(szText,"%s : %s",_name,\
      VisMBVertexDescriptor_t::GetComponentFormatString(VisMBVertexDescriptor_t::GetComponentFormat(descr._member)));\
    PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;\
  }

inline static const char *GetMemorySizeStr(int iSize, char *szBuffer)
{
  if (iSize<1024)
    sprintf(szBuffer,"%iBytes",iSize);
  else if (iSize<1024*1024)
    sprintf(szBuffer,"%.2fkB",(float)iSize/1024.f);
  else
    sprintf(szBuffer,"%.2fMB",(float)iSize/(1024.f*1024.f));
  return szBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VAnimationResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VAnimationResourcePreview::VAnimationResourcePreview(VManagedResource *pOwner) : VResourcePreview(pOwner)
{
}

void VAnimationResourcePreview::OnRender(int iFlags)
{
  VisAnimSequenceSet_cl *pSet = (VisAnimSequenceSet_cl *)GetOwner();

  VisFont_cl *pFont = &Vision::Fonts.DebugFont();
  float fHeight = pFont->GetFontHeight();
  char szText[4096];
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  DRAW_WITH_BACKGROUND
    hkvVec2 vText(20.f,fStartY);
    for (int i=0;i<pSet->GetSequenceCount();i++,vText.y += fHeight)
    {
      VisAnimSequence_cl* pSeq = pSet->GetSequence(i);
      sprintf(szText,"%s animation: '%s'  Duration: %.2fs",
        pSeq->GetType()==VIS_MODELANIM_VERTEX ? "Vertex" : "Skeletal", 
        pSeq->GetName(),pSeq->GetLength());
      PRINT_TEXT(vText, szText, V_RGBA_WHITE);
    }
  }
  Vision::RenderLoopHelper.EndOverlayRendering();
}



////////////////////////////////////////////////////////////////////////////////////////////////
// VBaseMeshResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VBaseMeshResourcePreview::VBaseMeshResourcePreview(VManagedResource *pOwner) : VMeshBufferResourcePreview(pOwner)
{
  VBaseMesh *pMesh = (VBaseMesh *)(pOwner);
  VASSERT(pMesh);

  m_iLight0 = m_iLight1 = m_iLight2 = -1; 
      
  if(!(pMesh->GetMeshBufferStreamMask() & VERTEX_STREAM_NORMAL) || !(pMesh->GetMeshBufferStreamMask() & VERTEX_STREAM_TANGENT))
  {
      Vision::Shaders.LoadShaderLibrary("\\Shaders\\BaseShaders.ShaderLib", SHADERLIBFLAG_HIDDEN);
      m_spPreviewTechnique = Vision::Shaders.CreateTechnique("BS_Fullbright", NULL);
      VASSERT(m_spPreviewTechnique);
  }
  else
  {
    Vision::Shaders.LoadShaderLibrary("\\Shaders\\ResourcePreview.ShaderLib", SHADERLIBFLAG_HIDDEN);

    VTechniqueConfig cfg;
    for (int i=0; i<pMesh->GetSubmeshCount(); i++)
    {
      VisSurface_cl *pSurface = pMesh->GetBaseSubmesh(i)->GetSurface();
      VASSERT(pSurface);
      if (pSurface->GetTransparencyType () == VIS_TRANSP_ALPHATEST || pSurface->GetTransparencyType () == VIS_TRANSP_ALPHA)
      {
        cfg.AddInclusionTag("ALPHATEST");
        // if found we render all meshes with alpha testing
        break;
      }
    }

    m_spPreviewTechnique = Vision::Shaders.CreateTechnique("ResourcePreview", NULL, &cfg);
    VASSERT(m_spPreviewTechnique);

    VASSERT(m_spPreviewTechnique->GetShaderCount() > 0);
    m_iLight0 = m_spPreviewTechnique->GetShader(0)->GetConstantBuffer(VSS_PixelShader)->GetRegisterByName("Light0");
    VASSERT(m_iLight0 >= 0);
    m_iLight1 = m_spPreviewTechnique->GetShader(0)->GetConstantBuffer(VSS_PixelShader)->GetRegisterByName("Light1");
    VASSERT(m_iLight1 >= 0);
    m_iLight2 = m_spPreviewTechnique->GetShader(0)->GetConstantBuffer(VSS_PixelShader)->GetRegisterByName("Light2");
    VASSERT(m_iLight2 >= 0);
  }

  m_fYaw = 0.f;
  m_spTexAnims = NULL;
}

void VBaseMeshResourcePreview::OnActivate()
{
  VASSERT(m_spTexAnims==NULL);
  VBaseMesh *pMesh = (VBaseMesh *)GetOwner();
  m_spTexAnims = new VisTextureAnimInstancePtr[pMesh->GetSubmeshCount()];
  for (int i=0;i<pMesh->GetSubmeshCount();i++)
  {
    VBaseSubmesh *pSubmesh = pMesh->GetBaseSubmesh(i);
    VisSurface_cl *pSurface = pSubmesh->GetSurface();
    m_spTexAnims[i] = Vision::TextureManager.GetAnimationInstance(pSurface->m_spDiffuseTexture);
  }

  UpdateDistanceFromBBox(pMesh->GetBoundingBox());
}

void VBaseMeshResourcePreview::OnDeActivate()
{
  V_SAFE_DELETE_ARRAY(m_spTexAnims);
}

bool VBaseMeshResourcePreview::OnUpdate(float fTimeDiff)
{
  // manually animate these anim instances
  if (!Vision::Editor.IsAnimatingOrPlaying())
  {
    VBaseMesh *pMesh = (VBaseMesh *)GetOwner();
    for (int i=0;i<pMesh->GetSubmeshCount();i++)
      if (m_spTexAnims[i])
        m_spTexAnims[i]->HandleAnim(Vision::GetTimer()->GetTimeDifference());
  }

  if (IsAnimationPaused())
    return FALSE;

  if (!GetThumbnailRotateMeshes())
    return FALSE;

  m_fYaw = hkvMath::mod (m_fYaw + fTimeDiff*60.f,360.f);
  return true;
}

void VBaseMeshResourcePreview::OnRender(int iFlags)
{
  if (m_spPreviewTechnique==NULL)
    return;

  Vision::RenderLoopHelper.ClearScreen(VisRenderLoopHelper_cl::VCTF_DepthStencil, VColorRef (128, 128, 128, 0), 1.0f, 255);

  bool bThumbnail = (iFlags&RESOURCEPREVIEWFLAG_FOR_THUMBNAIL)>0;

  VBaseMesh *pMesh = (VBaseMesh *)GetOwner();

  VisContextCamera_cl *cam = Vision::Contexts.GetCurrentContext()->GetCamera();
  hkvMat3 oldRotation = cam->GetRotationMatrix();
  hkvMat3 newRotation = oldRotation;

  hkvVec3 modelOffset = hkvVec3::ZeroVector ();

  //////////////////////////////////////////////////////////////////////////
  // TODO: correct this when hkvAlignedBBox is replaced by hkvAlignedBBox inside VBaseMesh
  hkvAlignedBBox bbox(pMesh->GetBoundingBox().m_vMin, pMesh->GetBoundingBox().m_vMax);
  // const hkvAlignedBBox &bbox = pMesh->GetBoundingBox();
  //////////////////////////////////////////////////////////////////////////

  // overwrite rotation (must be done before Helper_CreateTransformation)
  if (bThumbnail && GetOverwriteSceneCamera())
  {
    // calculate bounding box areas
    float fFront = bbox.getSizeX() * bbox.getSizeZ();
    float fTop   = bbox.getSizeX() * bbox.getSizeY();
    float fSide  = bbox.getSizeY() * bbox.getSizeZ();

    // set camera rotation to view onto the largest side of the bounding box
    float fY = (fFront >= fSide) ? 85.0f : 175.0f;
    float fP = ((fTop >= fFront) && (fTop >= fSide)) ? 40.0f : 15.0f;

    newRotation.setFromEulerAngles (0, fP, fY);

    // apply new rotation
    cam->SetRotationMatrix(newRotation);
    cam->Update();

    float fAspectRatio = ((bbox.getSizeX() + bbox.getSizeY()) * 0.5f) / bbox.getSizeZ();

    // offset model by 10% of the bounding box height
    if (fAspectRatio < 0.45f) // good threshold for relatively high models
      modelOffset.z = -bbox.getSizeZ()*0.1f;
  }

  // calculate transformation (must be done after the rotation overwrite)
  hkvMat4 transform;
  LookAtBBox(transform, m_fYaw, bbox, modelOffset);

  m_iMaterialCount = pMesh->GetSurfaceCount();
  m_iSubmeshCount = pMesh->GetSubmeshCount();

  VFogParameters fog = Vision::World.GetFogParameters();
  VFogParameters oldFogParameters = fog;
  fog.depthMode = fog.heightFogMode = VFogParameters::Off;
  Vision::World.SetFogParameters(fog);
      
  TrackDirectionalLight();

  // render
  Vision::RenderLoopHelper.BeginMeshRendering();
  {
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
        
    // transform bounding box center to world space
    hkvVec3 bboxCenter = bbox.getCenter();
    bboxCenter = transform * bboxCenter;

    // lights are three times the local radius away from the bounding box center
    float localRadius = bbox.getBoundingSphere().m_fRadius * 3.0f;

    // camera vectors
    hkvVec3 cameraOut = -newRotation.getAxis(0);
    hkvVec3 cameraRight = -newRotation.getAxis(1);
    hkvVec3 cameraUp = newRotation.getAxis(2);

    // normalized light directions
    hkvVec3 light0(0.577350f, -0.577350f, 0.577350f);
    hkvVec3 light1(-0.624695f, 0.624695f, 0.468521f);
    hkvVec3 light2(-0.486664f, -0.811107f, 0.324442f);

    // rotate light directions depending on the current camera orientation
    light0 = cameraOut*light0.x - cameraUp*light0.y + cameraRight*light0.z;
    light0.normalizeIfNotZero();
    light1 = cameraOut*light1.x - cameraUp*light1.y + cameraRight*light1.z;
    light1.normalizeIfNotZero();
    light2 = cameraOut*light2.x - cameraUp*light2.y + cameraRight*light2.z;
    light2.normalizeIfNotZero();

    // calculate world-space light positions
    light0 = bboxCenter + light0*localRadius;
    light1 = bboxCenter + light1*localRadius;
    light2 = bboxCenter + light2*localRadius;

    RenderMesh(pMesh, light0, light1, light2);
  }
  Vision::RenderLoopHelper.EndMeshRendering();

  if (bThumbnail && GetOverwriteSceneCamera())
  {
    cam->SetRotationMatrix(oldRotation);
    cam->Update();
  }

  Vision::World.SetFogParameters(oldFogParameters);

  if (!bThumbnail)
    RenderStreamOutput(pMesh->GetMeshBuffer(), pMesh);
}


bool VBaseMeshResourcePreview::SupportsThumbnails()
{
  return TRUE;
}


void VBaseMeshResourcePreview::RenderMesh(VBaseMesh *pMesh, hkvVec3 light0, hkvVec3 light1, hkvVec3 light2)
{
  VASSERT(m_spPreviewTechnique);

  VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = pMesh->GetMeshBuffer()->GetPrimitiveType();
  m_iVertexCount = pMesh->GetMeshBuffer()->GetVertexCount();
  m_iTriCount = 0;

  if (m_iVertexCount==0)
    return;

  int iMeshStreams = pMesh->GetMeshBufferStreamMask();

  for (int i=0;i<pMesh->GetSubmeshCount();i++)
  {
    VBaseSubmesh *pSubmesh = pMesh->GetBaseSubmesh(i);

    VisSurface_cl *pSurface = pSubmesh->GetSurface();
    if (pSurface==NULL)
      continue;
        
    VCompiledTechnique *pTech = m_spPreviewTechnique;
    if (pTech==NULL)
      pTech = pSurface->GetTechnique();

    // backup and set new
    VTextureObjectPtr spOldDiffuse = pSurface->m_spDiffuseTexture;
    if (m_spTexAnims[i] != NULL)
      pSurface->m_spDiffuseTexture = m_spTexAnims[i]->GetCurrentFrame();

    int iNumTris, iFirstTri;
    pSubmesh->GetRenderRange(iFirstTri, iNumTris);
    iFirstTri = VisMeshBuffer_cl::GetCalcPrimitiveCount(ePrimType, iFirstTri, iFirstTri); // TODO: the ranges should be available in this format already
    iNumTris = VisMeshBuffer_cl::GetCalcPrimitiveCount(ePrimType, iNumTris, iNumTris);
        
    for (int iPass=0; iPass<m_spPreviewTechnique->GetShaderCount(); iPass++)
    {
      VCompiledShaderPass *pShader = m_spPreviewTechnique->GetShader(iPass);
      VASSERT(pShader);

      if (m_iLight0 > -1)
      {
        VShaderConstantBuffer *pConstantBuffer = pShader->GetConstantBuffer(VSS_PixelShader);
        VASSERT(pConstantBuffer);
        pConstantBuffer->SetSingleRegisterF(m_iLight0, light0.x, light0.y, light0.z, 1.0f);
        pConstantBuffer->SetSingleRegisterF(m_iLight1, light1.x, light1.y, light1.z, 1.0f);
        pConstantBuffer->SetSingleRegisterF(m_iLight2, light2.x, light2.y, light2.z, 1.0f);
      }
         
      // apply blending states
      const VStateGroupBlend *pBlendGroup = VisRenderStates_cl::GetBlendStateGroup(pSurface->GetTransparencyType());
      VisRenderStates_cl::SetBlendState(*pBlendGroup);

      // apply rasterizer states
      VisRenderStates_cl::SetRasterizerState(pSurface->IsDoubleSided() ? *VisRenderStates_cl::GetRasterizerStateGroup(VIS_RASTERIZER_CULL_NONE) : *VisRenderStates_cl::GetRasterizerStateGroup(VIS_RASTERIZER_CULL_BACK));
          
      Vision::RenderLoopHelper.ResetMeshStreams();
      int iRequestedMeshStream = (pShader->GetStreamMask () | VERTEX_STREAM_INDEXBUFFER);
      VASSERT_MSG( (iMeshStreams & iRequestedMeshStream) != 0,"The provided and request mesh stream do not match in any value");
      Vision::RenderLoopHelper.AddMeshStreams(pMesh->GetMeshBuffer(), iMeshStreams & iRequestedMeshStream );
      Vision::RenderLoopHelper.BindSurfaceTextures(pSurface, pShader);
      Vision::RenderLoopHelper.RenderMeshes(pShader, ePrimType, iFirstTri, iNumTris, m_iVertexCount);
    }

    // restore
    pSurface->m_spDiffuseTexture = spOldDiffuse;
    m_iTriCount += iNumTris;
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////
// VBitmapResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VBitmapResourcePreview::VBitmapResourcePreview(VManagedResource *pOwner) : VTexture2DResourcePreview(pOwner, NULL)
{
  m_iLastX = m_iLastY = 0;
}

VTextureObject *VBitmapResourcePreview::GetTexture()
{
  VisBitmap_cl *pBMP = (VisBitmap_cl *)GetOwner();
  if (m_spUploaded==NULL || m_iLastX!=pBMP->GetWidth() || m_iLastY!=pBMP->GetHeight())
  {
    m_iLastX=pBMP->GetWidth();
    m_iLastY=pBMP->GetHeight();
    m_spUploaded = Vision::TextureManager.Create2DTextureObject("<BitmapPreview>", m_iLastX, m_iLastY, 1, VTextureLoader::B8G8R8A8);
    m_spUploaded->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  //m_spUploaded->UploadDataMipmap(0,(const char *)pBMP->GetDataPtr());
  m_spUploaded->UpdateRect(0,0,0,0,0,-1,(const char *)pBMP->GetDataPtr(), V_TEXTURE_LOCKFLAG_DISCARDABLE);
  return m_spUploaded;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VFontResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VFontResourcePreview::VFontResourcePreview(VisFont_cl *pOwner) : VResourcePreview(pOwner)
{
}

void VFontResourcePreview::OnRender(int iFlags)
{
  VisFont_cl *pFont = (VisFont_cl *)GetOwner();
  int sx,sy;
  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
  pContext->GetSize(sx,sy);
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
    DRAW_WITH_BACKGROUND
      hkvVec2 vPos(20.f,fStartY);
      PRINT_TEXT(vPos, "abcdefghijklmnopqrstuvwxyz0123456789.,+-", V_RGBA_WHITE);
      vPos.y += pFont->GetFontHeight();
      PRINT_TEXT(vPos, "ABCDEFGHIJKLMNOPQRSTUVWXYZ:;_#*!$&/()=?", V_RGBA_WHITE);
    }
  Vision::RenderLoopHelper.EndOverlayRendering();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VMeshBufferResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VMeshBufferResourcePreview::VMeshBufferResourcePreview(VManagedResource *pOwner) : VResourcePreview(pOwner)
{
  m_iTriCount = m_iVertexCount = m_iMaterialCount = m_iSubmeshCount = -1;
}

void VMeshBufferResourcePreview::OnRender(int iFlags)
{
  bool bThumbnail = (iFlags&RESOURCEPREVIEWFLAG_FOR_THUMBNAIL)>0;
  if (!bThumbnail)
    RenderStreamOutput((VisMeshBuffer_cl *)GetOwner());
}

void VMeshBufferResourcePreview::RenderStreamOutput(VisMeshBuffer_cl *pMesh, VBaseMesh *pSubMeshes)
{
  VisMBVertexDescriptor_t descr;
  pMesh->GetVertexDescriptor(descr);
  m_iTriCount = pMesh->GetCurrentPrimitiveCount();
  m_iVertexCount = pMesh->GetVertexCount();

  VisFont_cl *pFont = &Vision::Fonts.DebugFont();
  float fHeight = pFont->GetFontHeight();
  char szText[1024];
  char szTemp[1024];
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();

  DRAW_WITH_BACKGROUND
    hkvVec2 vText(20.f,fStartY);
    if (m_iMaterialCount>=0)
    {
      sprintf(szText,"Materials : %i",m_iMaterialCount);
      PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    }
    if (m_iSubmeshCount>=0)
    {
      sprintf(szText,"Submeshes : %i",m_iSubmeshCount);
      PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    }
    if (m_iVertexCount>=0)
    {
      sprintf(szText,"Vertices : %i",m_iVertexCount);
      PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    }
    if (m_iTriCount>=0)
    {
      sprintf(szText,"Triangles : %i",m_iTriCount);
      PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    }

    sprintf(szText,"Streams :");
    PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    vText.x += 20.f;
    PRINT_STREAM(HasPosition(), "POSITION", m_iPosOfs);
    PRINT_STREAM(HasColor(),    "COLOR",    m_iColorOfs);
    PRINT_STREAM(HasSecondaryColor(),"COLOR2",m_iSecondaryColorOfs);
    PRINT_STREAM(HasNormal(),   "NORMAL",   m_iNormalOfs);
    for (int j=0;j<MAX_MESHBUFFER_TEXCOORDS;j++)
    {
      sprintf(szTemp,"TEXCOORD%i",j);
      PRINT_STREAM(HasTexCoord(j),szTemp,m_iTexCoordOfs[j]);
    }
    vText.x -= 20.f;
    sprintf(szText,"Vertex Size : %iB (%s)", descr.m_iStride,GetMemorySizeStr(descr.m_iStride*m_iVertexCount,szTemp));
    PRINT_TEXT(vText, szText, VColorRef(200,200,200,255)); vText.y += fHeight;
    if (pMesh->GetIndexType()!=0)
    {
      sprintf(szText,"Indices : %i (%ibit)", pMesh->GetIndexCount(),pMesh->GetIndexType());
      PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
    }
    if (pSubMeshes)
    {
      vText.y += 4.f;
      PRINT_TEXT(vText, "Submeshes:", V_RGBA_WHITE); vText.y += fHeight;
      vText.x += 20.f;
      for (int j=0;j<pSubMeshes->GetSubmeshCount();j++)
      {
        VBaseSubmesh *pSM = pSubMeshes->GetBaseSubmesh(j);
        const VBaseGeometryInfo& geom(pSM->GetGeometryInfo());
        char szName[1024];
        if (!geom.m_name.IsEmpty())
          sprintf(szName,"Name='%s' ",geom.m_name.AsChar());
        else szName[0]=0;
        sprintf(szText,"%i. Mat='%s' %sLOD=%i Clip=(%g,%g)", j+1, pSM->GetSurface()->GetName(),szName,(int)geom.m_iLODIndex,geom.m_fNearClipDistance, geom.m_fFarClipDistance);
        PRINT_TEXT(vText, szText, V_RGBA_WHITE); vText.y += fHeight;
      }
      vText.x -= 20.f;
    }
  }

  Vision::RenderLoopHelper.EndOverlayRendering();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VParticleFXResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VParticleFXResourcePreview::VParticleFXResourcePreview(VManagedResource *pOwner)
: VResourcePreview(pOwner), m_Layers(16,16)
{
}

void VParticleFXResourcePreview::OnActivate()
{
  VisParticleEffectFile_cl *pFX = (VisParticleEffectFile_cl *)GetOwner();

  hkvAlignedBBox bbox;
  pFX->GetBoundingBox(bbox);

  m_spInst = pFX->CreateParticleEffectInstance(hkvVec3::ZeroVector (), hkvVec3::ZeroVector ());
  m_spInst->SetHandleWhenVisible(false);
  m_spInst->SetVisibleBitmask(0);

  m_spInst->UpdateVisibilityBoundingBox(); // basically ensure the simulation has finished
  m_spInst->GetCurrentBoundingBox(bbox);

  UpdateDistanceFromBBox(bbox);
  m_vBoundingBoxCenter = bbox.getCenter();
}

void VParticleFXResourcePreview::OnDeActivate()
{
  m_spInst->DisposeObject();
  m_spInst = NULL;
}

bool VParticleFXResourcePreview::OnUpdate(float fTimeDiff)
{
  if (IsAnimationPaused())
    return false;

  if (!Vision::Editor.IsAnimatingOrPlaying())
    m_spInst->Tick(fTimeDiff);

  return true;
}

void VParticleFXResourcePreview::OnRender(int iFlags)
{
  bool bThumbnail = (iFlags&RESOURCEPREVIEWFLAG_FOR_THUMBNAIL)>0;

  VisContextCamera_cl *cam = Vision::Contexts.GetCurrentContext()->GetCamera();
  hkvMat3 currentRotation = cam->GetRotationMatrix();

  VisParticleEffectFile_cl *pFX = (VisParticleEffectFile_cl *)GetOwner();
  hkvVec3 vPos;
  hkvAlignedBBox bbox;
  pFX->GetBoundingBox(bbox);

  // overwrite rotation (must be done before Helper_CreateTransformation)
  if (bThumbnail && GetOverwriteSceneCamera())
  {
    hkvAlignedBBox currentBox;
    m_spInst->GetCurrentBoundingBox(currentBox);

    // calculate bounding box areas
    float fFront = currentBox.getSizeX() * currentBox.getSizeZ();
    float fTop   = currentBox.getSizeX() * currentBox.getSizeY();
    float fSide  = currentBox.getSizeY() * currentBox.getSizeZ();

    // set camera rotation to view onto the largest side of the bounding box
    float fY = (fFront >= fSide) ? 85.0f : -5.0f;
    float fP = ((fTop >= fFront) && (fTop >= fSide)) ? 40.0f : 20.0f;

    hkvMat3 newRotation (hkvNoInitialization);
    newRotation.setFromEulerAngles (0, fP, fY);

    // apply new rotation
    cam->SetRotationMatrix(newRotation);
    cam->Update();
  }

  // calculate transformation (must be done after the rotation overwrite)
  vPos = cam->GetPosition() + cam->GetDirection()*GetDistance() - m_vBoundingBoxCenter;

  m_spInst->IncPosition(vPos-m_vLastPos,true);
  m_vLastPos = vPos;
  m_spInst->UpdateVisibilityBoundingBox(); // basically ensure the simulation has finished

  m_Layers.Clear();
  int iCount = m_spInst->GetParticleGroupCount();
  for (int i=0;i<iCount;i++)
  {
    VisParticleGroup_cl *pLayer = m_spInst->GetParticleGroup(i);
    if (pLayer==NULL)
      continue;
    m_Layers.AppendEntry(pLayer);
    pLayer->SetVisibleBitmask(1);
    pLayer->SetRenderOrder(1);
    pLayer->SetAlwaysInForeGround(TRUE);
  }
      
  // render
  Vision::RenderLoopHelper.RenderParticleSystems(&m_Layers,1,1);
  for (int i=0;i<(int)m_Layers.GetNumEntries();i++)
  {
    m_Layers.GetEntry(i)->SetVisibleBitmask(0);
    m_Layers.GetEntry(i)->SetRenderOrder(0);
  }

  if (bThumbnail && GetOverwriteSceneCamera())
  {
    cam->SetRotationMatrix(currentRotation);
    cam->Update();
  }
}

bool VParticleFXResourcePreview::SupportsThumbnails()
{
  return true;
}

bool VParticleFXResourcePreview::SupportsImageThumbnails()
{
  return true;
}

VString VParticleFXResourcePreview::GetThumbnailFilename()
{
  if (!Vision::Editor.IsInEditor())
  {
    return VString();
  }

  VString sAbsFileName;
  if (!Vision::Editor.MakePathAbsoluteForProject(GetOwner()->GetFilename(), sAbsFileName))
  {
    return VString();
  }

  VString sFinalFileName;
  sFinalFileName.Format("%s.Thumbnail.%s", sAbsFileName, GetSaveThumbnailsAsJPG() ? "jpg" : "bmp");

  return sFinalFileName;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VShaderFXLibResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VShaderFXLibResourcePreview::VShaderFXLibResourcePreview(VManagedResource *pOwner) : VResourcePreview(pOwner)
{
}

void VShaderFXLibResourcePreview::OnRender(int iFlags)
{
  VisFont_cl *pFont = &Vision::Fonts.DebugFont();
  VShaderEffectLib *pLib = (VShaderEffectLib *)GetOwner();

  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();

  char szBuffer[1024];
  float fHeight = pFont->GetFontHeight();
  DRAW_WITH_BACKGROUND
    float y = fStartY;
    for (int iFX = 0;iFX<pLib->m_Effects.Count();iFX++)
    {
      VShaderEffectResource *pFX = pLib->m_Effects.GetAt(iFX);
      sprintf(szBuffer,"- Effect: '%s'",pFX->m_sName.GetChar());
      PRINT_TEXT(hkvVec2(20.f,y),szBuffer, VColorRef(200,200,255,255));
      y+=fHeight;
      for (int iTech=0;iTech<pFX->m_Techniques.Count();iTech++)
      {
        VShaderEffectTechnique *pTech = pFX->m_Techniques.GetAt(iTech);
        sprintf(szBuffer,"- Technique: '%s'",pTech->m_sName.GetChar());
        PRINT_TEXT(hkvVec2(40.f,y), szBuffer, VColorRef(200,255,200,255));
        y+=fHeight;
      }
      y+=2.f;
      for (int iParam=0;iParam<pFX->m_Parameter.Count();iParam++)
      {
        VShaderParam *pParam = pFX->m_Parameter.GetAt(iParam);
        sprintf(szBuffer,"- Parameter: '%s'",pParam->GetName());
        PRINT_TEXT( hkvVec2(40.f,y), szBuffer, VColorRef(255,200,200,255));
        y+=fHeight;
      }
      y+=2.f;
    }
  }
  Vision::RenderLoopHelper.EndOverlayRendering();
}



////////////////////////////////////////////////////////////////////////////////////////////////
// VTexture2DResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VTexture2DResourcePreview::VTexture2DResourcePreview(VManagedResource *pOwner, VisTextureAnimInstance_cl *pTexAnim) : VResourcePreview(pOwner)
{
  m_spAnimInst = pTexAnim;
}


VTextureObject *VTexture2DResourcePreview::GetTexture()
{
  if (m_spAnimInst)
    return m_spAnimInst->GetCurrentFrame();
  return (VTextureObject *)GetOwner();
}


bool VTexture2DResourcePreview::OnUpdate(float fTimeDiff)
{
  if (IsAnimationPaused())
    return FALSE;

  if (m_spAnimInst!=NULL && !Vision::Editor.IsAnimatingOrPlaying())
    m_spAnimInst->HandleAnim(fTimeDiff,true);
  return m_spAnimInst!=NULL; // update if animated
}

void VTexture2DResourcePreview::OnRender(int iFlags)
{
  VTextureObject *pTex = GetTexture();
  VASSERT(pTex);

  int sx,sy,tx,ty,iDepth;
  VisRenderContext_cl *pContext = Vision::Contexts.GetCurrentContext();
  pContext->GetSize(sx,sy);
  pTex->GetRealTextureDimensions(tx,ty,iDepth);

  int mx = sx;
  int my = sy;
  if (m_bRenderMetaData)
  {
    mx -= 40;
    my -= 80;
  }

  if (mx<10|| my<10)
    return;

  int dx = tx;
  int dy = ty;

  {
    float fx = (float)tx/(float)mx;
    float fy = (float)ty/(float)my;
    float fScale = (fx>fy) ? 1.f/fx : 1.f/fy;
    dx = (int)((float)dx*fScale);
    dy = (int)((float)dy*fScale);
  }

  hkvVec2 vPos1((float)((sx-dx)/2), (float)((sy-dy)/2));
  hkvVec2 vPos2(vPos1.x + (float)dx, vPos1.y + (float)dy);
  hkvVec2 vPosTxt(vPos1.x, vPos2.y + 12.f);
  VRectanglef textRect;
  char szText[1024],szBuffer[128];

  unsigned int iSampleCount = 1;
  if (pTex->IsRenderable())
  {
    const VisRenderableTextureConfig_t* config = static_cast<VisRenderableTexture_cl*>(pTex)->GetConfig();
    iSampleCount = hkvMath::Max(iSampleCount, config->m_iMultiSampling);
  }

  sprintf(szText,"Size: %i x %i; Samples: %i; Mipmaps: %i; Format: %s", tx, ty, iSampleCount, pTex->GetMipMapLevelCount(),
    VTextureLoader::GetTextureFormatString(pTex->GetTextureFormat(),  szBuffer));

  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  {
    //pRI->DrawSolidQuad(vPos1, vPos2, V_RGBA_CYAN, iAlphaState);
    pRI->DrawTexturedQuad(vPos1, vPos2, pTex, hkvVec2(0,0), hkvVec2(1.f,1.f), V_RGBA_WHITE, iAlphaState);

    if (m_bRenderMetaData)
    {
      Vision::Fonts.DebugFont().GetTextDimension(szText,textRect);
      textRect.Add(4.f);
      pRI->DrawSolidQuad(vPosTxt + textRect.m_vMin,vPosTxt + textRect.m_vMax, iOverlayBGColor, iAlphaState);
      Vision::Fonts.DebugFont().PrintText(pRI, vPosTxt, szText, V_RGBA_WHITE);
    }
  }
  Vision::RenderLoopHelper.EndOverlayRendering();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// VTextureCubemapResourcePreview
////////////////////////////////////////////////////////////////////////////////////////////////

VTextureCubemapResourcePreview::VTextureCubemapResourcePreview(VManagedResource *pOwner) : VResourcePreview(pOwner)
{
  m_spSphere = Vision::Game.LoadDynamicMesh("\\Models\\MagicBall.model",true);
  VShaderEffectLib *pLib =  Vision::Shaders.LoadShaderLibrary("\\Shaders\\Cubemap.ShaderLib",SHADERLIBFLAG_HIDDEN);
  m_spTech = Vision::Shaders.CreateTechnique("Cubemap",NULL,NULL,EFFECTCREATEFLAG_FORCEUNIQUE,pLib);
  m_spForegroundFillPassTechnique = Vision::Shaders.CreateTechnique("BS_ForeGroundFill",NULL);
}

void VTextureCubemapResourcePreview::OnActivate()
{
  VASSERT(m_spSphere);

  UpdateDistanceFromBBox(m_spSphere->GetBoundingBox());
}

void VTextureCubemapResourcePreview::OnRender(int iFlags)
{
  if (m_spTech==NULL || m_spTech->GetShaderCount()!=1 || m_spForegroundFillPassTechnique==NULL)
    return;
      
  bool bThumbnail = (iFlags&RESOURCEPREVIEWFLAG_FOR_THUMBNAIL)>0;

  hkvMat4 transform;
  LookAtBBox(transform, 0.0f, m_spSphere->GetBoundingBox(), hkvVec3::ZeroVector());

  VCompiledShaderPass *pShader = m_spTech->GetShader(0);
  int iSampler = pShader->GetSamplerIndexByName(VSS_PixelShader, "Cubemap");
  if (iSampler<0)
    return;

  VTextureCubeObject* pTex = (VTextureCubeObject *)GetOwner();
  VStateGroupTexture *pStateGroupTexture = pShader->GetStateGroupTexture(VSS_PixelShader, iSampler);
  if (pStateGroupTexture != NULL)
    pStateGroupTexture->m_spCustomTex = pTex; 

  int iMeshStreams = m_spSphere->GetMeshBufferStreamMask();
  VisMeshBuffer_cl::MB_PrimitiveType_e ePrimType = m_spSphere->GetMeshBuffer()->GetPrimitiveType();
  int iVertexCount = m_spSphere->GetMeshBuffer()->GetVertexCount();
  int iIndexCount = m_spSphere->GetMeshBuffer()->GetIndexCount();
  int iNumTris = VisMeshBuffer_cl::GetCalcPrimitiveCount(ePrimType,iVertexCount,iIndexCount);

  Vision::RenderLoopHelper.BeginMeshRendering();
    Vision::RenderLoopHelper.ResetMeshStreams();
    Vision::RenderLoopHelper.SetMeshTransformationMatrix(transform);
    Vision::RenderLoopHelper.AddMeshStreams(m_spSphere->GetMeshBuffer(), iMeshStreams & (pShader->GetStreamMask () | VERTEX_STREAM_INDEXBUFFER));
    if (!bThumbnail)
      Vision::RenderLoopHelper.RenderMeshes(m_spForegroundFillPassTechnique->GetShader(0), ePrimType, 0,iNumTris, iVertexCount);
    Vision::RenderLoopHelper.BindMeshTexture(pTex,iSampler, pShader->GetStateGroupSampler(VSS_PixelShader, iSampler));
    Vision::RenderLoopHelper.RenderMeshes(pShader, ePrimType, 0,iNumTris, iVertexCount);
  Vision::RenderLoopHelper.EndMeshRendering();

  char szText[1024],szBuffer[128];
  sprintf(szText,"Size: %i x %i; Mipmaps: %i; Format: %s",pTex->GetTextureWidth(),pTex->GetTextureHeight(),
          pTex->GetMipMapLevelCount(),VTextureLoader::GetTextureFormatString(pTex->GetTextureFormat(),szBuffer));

  VisFont_cl *pFont = &Vision::Fonts.DebugFont();
  IVRender2DInterface *pRI = Vision::RenderLoopHelper.BeginOverlayRendering();
  DRAW_WITH_BACKGROUND
    PRINT_TEXT(hkvVec2(20.f,fStartY),szText, V_RGBA_WHITE);
  }
  Vision::RenderLoopHelper.EndOverlayRendering();
}

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
