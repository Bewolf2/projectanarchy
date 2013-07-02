/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadow.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/BlobShadowManager.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>



VBlobShadowShader::VBlobShadowShader()
{
  m_iShadowTexSampler = m_iGradientTexSampler = -1;
  m_iRegPlaneU = m_iRegPlaneV = m_iRegColor = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// overridden function to extract the shader registers from the compiled version
void VBlobShadowShader::PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX, pSourceShader);

  m_iRegPlaneU = GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("TexRefPlaneU");
  m_iRegPlaneV = GetConstantBuffer(VSS_VertexShader)->GetRegisterByName("TexRefPlaneV");
  VASSERT(m_iRegPlaneU>=0 && m_iRegPlaneV>=0);

  m_iRegColor = GetConstantBuffer(VSS_PixelShader)->GetRegisterByName("ShadowColor");

  VASSERT(m_iRegColor>=0);

  m_iShadowTexSampler   = GetSamplerIndexByName(VSS_PixelShader, "ShadowTexture");
  m_iGradientTexSampler = GetSamplerIndexByName(VSS_PixelShader, "NormalGradient");
}

void VBlobShadowShader::UpdateShadow(const VBlobShadow *pShadow)
{
  if (m_iShadowTexSampler>=0)
  {
    VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iShadowTexSampler);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pShadow->GetShadowTexture();
  }

  // update the uv projection planes
  const float fSizeX = pShadow->m_ShadowBox.getSizeX();
  const float fSizeY = pShadow->m_ShadowBox.getSizeY();

  hkvPlane planeU; 
  planeU.m_vNormal = hkvVec3(1.f/fSizeX, 0, 0);  
  planeU.m_fNegDist = -planeU.m_vNormal.dot (pShadow->m_ShadowBox.m_vMin);

  hkvPlane planeV; 
  planeV.m_vNormal = hkvVec3(0, 1.f/fSizeY, 0);  
  planeV.m_fNegDist = -planeV.m_vNormal.dot (pShadow->m_ShadowBox.m_vMin);

  GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(m_iRegPlaneU, &planeU.m_vNormal.x);
  GetConstantBuffer(VSS_VertexShader)->SetSingleRegisterF(m_iRegPlaneV, &planeV.m_vNormal.x);

  // shadow color
  GetConstantBuffer(VSS_PixelShader)->SetSingleRegisterF(m_iRegColor, pShadow->m_vBlendColor.data);

  m_bModified = true;
}


// register the shader class in the engine module
V_IMPLEMENT_DYNCREATE(VBlobShadowShader, VCompiledShaderPass, &g_VisionEngineModule);





/////////////////////////////////////////////////////////////////////////////
// Simple shadow functions
/////////////////////////////////////////////////////////////////////////////
VBlobShadow::VBlobShadow(int iComponentFlags) : IVObjectComponent(0, iComponentFlags)
{
  Enabled = true;
  m_bNormalFalloff = true;
  SetColor(V_RGBA_GREY);
  m_spShadowTex = VBlobShadowManager::GlobalManager().GetDefaultShadowTexture();
}



BOOL VBlobShadow::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisObject3D_cl or derived classes.";
    return FALSE;
  }
  
  return TRUE;
}


VBlobShadow::~VBlobShadow()
{
}

void VBlobShadow::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // add or remove from manager according to whether we have an owner or not
  if (pOwner)
  {
    VBlobShadowManager::GlobalManager().Instances().AddUnique(this);
  }
  else
  {
    VBlobShadowManager::GlobalManager().Instances().SafeRemove(this);
  }
}


void VBlobShadow::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  SetColor(ShadowColor); // sets the float dependency

  if (!strcmp(pVar->name,"TextureFilename"))
  {
    if (value && value[0])
      m_spShadowTex = Vision::TextureManager.Load2DTexture(value);
    else
      m_spShadowTex = VBlobShadowManager::GlobalManager().GetDefaultShadowTexture();
  }
}


V_IMPLEMENT_SERIAL(VBlobShadow,IVObjectComponent,0,&g_VisionEngineModule);
void VBlobShadow::Serialize( VArchive &ar )
{
  char iLocalVersion = 2;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion==2, "Invalid local version. Please re-export");
    ar >> Enabled >> m_bNormalFalloff;
    m_ShadowBox.SerializeAs_VisBoundingBox (ar);
    ar >> Radius >> Height;
    ar >> ShadowColor >> m_vBlendColor;
    m_spShadowTex = (VTextureObject *)ar.ReadProxyObject();
  } else
  {
    ar << iLocalVersion;
    ar << Enabled << m_bNormalFalloff;
    m_ShadowBox.SerializeAs_VisBoundingBox (ar);
    ar << Radius << Height;
    ar << ShadowColor << m_vBlendColor;
    ar.WriteProxyObject(m_spShadowTex);
  }
}



void VBlobShadow::SetBoundingBoxFromOwnerProperties()
{
  VASSERT(GetOwner());
  VisObject3D_cl *pOwner = (VisObject3D_cl *)GetOwner();

  // build absolute bounding box
  pOwner->GetPosition((hkvVec3&) m_ShadowBox.m_vMin);
  m_ShadowBox.m_vMax = m_ShadowBox.m_vMin;
  m_ShadowBox.addBoundary(hkvVec3 (Radius));
  m_ShadowBox.m_vMin.z -= Height;
}


START_VAR_TABLE(VBlobShadow,IVObjectComponent,"Blob shadow component",VVARIABLELIST_FLAGS_NONE, "Blob Shadow" )
  DEFINE_VAR_COLORREF(VBlobShadow, ShadowColor, "Color of the shadow", "127,127,127,255", 0, NULL);
  DEFINE_VAR_FLOAT(VBlobShadow, Radius, "Radius of the blob shadow", "80.0", 0, NULL);
  DEFINE_VAR_FLOAT(VBlobShadow, Height, "Max. height of the pivot over ground", "200.0", 0, NULL);
  DEFINE_VAR_BOOL(VBlobShadow, Enabled, "Max. height of the pivot over ground", "TRUE", 0, NULL);
  DEFINE_VAR_STRING_CALLBACK(VBlobShadow, TextureFilename, "Filename of the texture", "\\Textures\\blobShadow.dds", DISPLAY_HINT_TEXTUREFILE, NULL);
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
