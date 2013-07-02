/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VWallmarkManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/Projector.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

#define PROJECTOR_VERSION_1                1 //VisibleBitmask
#define PROJECTOR_VERSION_2                2 //Lightmapped
#define PROJECTOR_VERSION_3                3 //UniqueID64
#define PROJECTOR_VERSION_4                4 //GeometryTypeFilter
#define PROJECTOR_VERSION_5                5 //FarClipDistance
#define PROJECTOR_VERSION_6                6 //InfluenceBitmask
#define PROJECTOR_VERSION_7                7 //Custom shader effect
#define PROJECTOR_VERSION_8                8 //Unique ID through base class
#define PROJECTOR_CURRENT_VERSION          PROJECTOR_VERSION_8


//////////////////////////////////////////////////////////////////////////////////////////////////////
// wallmark shader
//////////////////////////////////////////////////////////////////////////////////////////////////////


V_IMPLEMENT_SERIAL( VProjectorShaderPass, VCompiledShaderPass, 0, &g_VisionEngineModule );


void VProjectorShaderPass::PostCompileFunction(VShaderEffectResource *pSourceFX,VShaderPassResource *pSourceShader)
{
  VCompiledShaderPass::PostCompileFunction(pSourceFX,pSourceShader); // base

  VShaderConstantBuffer *pVS = GetConstantBuffer(VSS_VertexShader);
  VShaderConstantBuffer *pPS = GetConstantBuffer(VSS_PixelShader);

  m_iVSRegRefPlaneS   = pVS->GetRegisterByName("RefPlaneS");
  m_iVSRegRefPlaneT   = pVS->GetRegisterByName("RefPlaneT");
  m_iVSRegRefPlaneQ   = pVS->GetRegisterByName("RefPlaneQ");
  m_iVSRegRefPlaneAtt = pVS->GetRegisterByName("RefPlaneAtt");
  m_iVSRegColor       = pVS->GetRegisterByName("PrimaryColor");
  m_iPSRegColor       = pPS->GetRegisterByName("PrimaryColor");
  m_iRegProjTexSampler = GetSamplerIndexByName(VSS_PixelShader, "WmarkTex");
}


void VProjectorShaderPass::SetProperties(const VProjectedWallmark *pProjector)
{
  // setup texture
  if (m_iRegProjTexSampler>=0)
  {
    VStateGroupTexture *pStateGroupTexture = GetStateGroupTexture(VSS_PixelShader, m_iRegProjTexSampler);
    if (pStateGroupTexture != NULL)
      pStateGroupTexture->m_spCustomTex = pProjector->GetCurrentTexture();

    VStateGroupSampler *pStateGroupSampler = GetStateGroupSampler(VSS_PixelShader, m_iRegProjTexSampler);
    if (pStateGroupSampler != NULL)
    {
      // different blend types require different border colors
      switch (pProjector->GetTransparencyType())
      {
      case VIS_TRANSP_MULTIPLICATIVE:
        pStateGroupSampler->m_fBorderColor.set(1,1,1,0); // will cause visual artifacts on consoles (limitation on border color values)
        break;
      case VIS_TRANSP_MODULATE2X:
        pStateGroupSampler->m_fBorderColor.set(0.5f,0.5f,0.5f,0); // will cause visual artifacts on consoles (limitation on border color values)
        break;
      default:
        pStateGroupSampler->m_fBorderColor.set(0,0,0,0);
      }

      pStateGroupSampler->ComputeHash();
    }
  }

  VShaderConstantBuffer *pPS = GetConstantBuffer(VSS_PixelShader);

  hkvPlane s(hkvNoInitialization),t(hkvNoInitialization),q(hkvNoInitialization),att(hkvNoInitialization);
  pProjector->GetProjectionPlanes(s, t, q, att);

  VShaderConstantBuffer *pVS = GetConstantBuffer(VSS_VertexShader);

  // setup planes
  if (m_iVSRegRefPlaneS>=0) pVS->SetSingleRegisterF(m_iVSRegRefPlaneS, &s.m_vNormal.x);
  if (m_iVSRegRefPlaneT>=0) pVS->SetSingleRegisterF(m_iVSRegRefPlaneT, &t.m_vNormal.x);
  if (m_iVSRegRefPlaneQ>=0) pVS->SetSingleRegisterF(m_iVSRegRefPlaneQ, &q.m_vNormal.x);
  if (m_iVSRegRefPlaneAtt>=0) pVS->SetSingleRegisterF(m_iVSRegRefPlaneAtt, &att.m_vNormal.x);

  // setup color
  if (m_iVSRegColor>=0 || m_iPSRegColor>=0)
  {
    hkvVec4 vColor(false);
    if (pProjector->GetTransparencyType() == VIS_TRANSP_MULTIPLICATIVE || pProjector->GetTransparencyType() == VIS_TRANSP_MODULATE2X)
    {
      vColor.x = vColor.y = vColor.z = vColor.w = 1.f;
    }
    else
    {
      VColorRef colRef = pProjector->GetColor();
      vColor.x = ((float)colRef.r) * V_DIV255;
      vColor.y = ((float)colRef.g) * V_DIV255;
      vColor.z = ((float)colRef.b) * V_DIV255;
      vColor.w = ((float)colRef.a) * V_DIV255;
    }
    if (m_iPSRegColor>=0) pPS->SetSingleRegisterF(m_iPSRegColor, vColor.data);
    if (m_iVSRegColor>=0) pVS->SetSingleRegisterF(m_iVSRegColor, vColor.data);

  }

  // other states:
  if (pProjector->m_spCustomEffect==NULL)
  {
    m_cStateGroupMask = STATEGROUP_DEPTHSTENCIL|STATEGROUP_RASTERIZER|STATEGROUP_TESSELATION|STATEGROUP_SAMPLERS|STATEGROUP_TEXTURES;
    VisRenderStates_cl::SetBlendState(*VisRenderStates_cl::GetOverlayBlendStateGroup(pProjector->GetTransparencyType()));
  } else
  {
    m_cStateGroupMask = STATEGROUP_ALL;
  }

  m_bModified = true;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////
// class VProjectedWallmark
/////////////////////////////////////////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VProjectedWallmark, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );


VProjectedWallmark::VProjectedWallmark() :
  m_vDirection(1,0,0), m_vUpDir(0,0,1), m_vRight(0,1,0)
{
  VWallmarkManager &manager(VWallmarkManager::GlobalManager());
  manager.m_AllProjectedWallmarks.Add(this); // wallmarks from file are not in the fading collection  
  manager.EnsureRenderCallbackRegistered();
  m_fLifeTime = m_fFadeOutTime = 0.f;
  m_iColor = V_RGBA_WHITE;
  m_fInitialAlpha = (float)m_iColor.a;
  m_eTransp = VIS_TRANSP_ADDITIVE;
  m_fConeFactorX = m_fConeFactorY = 1.f;
  m_bPrimDirty = true;
  m_bFromFile = false;
  m_fDepth = 50.f;
  m_bLightmapped = false;
  SetVisibleBitmask(0xffffffff);
  SetInfluenceBitmask(0xffffffff);
  m_iGeometryTypeFilter = PROJECTOR_AFFECTS_DEFAULT;
  m_fFarClipDistance = -1.f;
  m_ePassType = VPT_Undefined;
  SetCustomShaderEffect(NULL);

  V_COMPILE_ASSERT(PROJECTOR_AFFECTS_ENTITIES!=PROJECTOR_AFFECTS_TERRAIN);
  V_COMPILE_ASSERT(PROJECTOR_AFFECTS_ENTITIES!=PROJECTOR_AFFECTS_STATICMESHES);
}


VProjectedWallmark::VProjectedWallmark(
    const hkvVec3& origin, 
    const hkvVec3& destpos, 
    const hkvVec3& up, 
    const hkvVec3& right, 
    float fConeFactorX, float fConeFactorY, float fDepth, 
    VTextureObject* pTexture, const char *szKey, 
    VIS_TransparencyType eTransp, 
    VColorRef iColor, float fLifeTime, float fFadeOutTime)
{
  VWallmarkManager &manager(VWallmarkManager::GlobalManager());
  manager.m_AllProjectedWallmarks.Add(this);
  if (fLifeTime>0.f)
  {
    manager.m_FadingProjectedWallmarks.Add(this);
    manager.EnsureSimulationCallbackRegistered();
  }

  manager.EnsureRenderCallbackRegistered();
  m_fLifeTime = fLifeTime + fFadeOutTime;
  m_fFadeOutTime = fFadeOutTime;

  m_bPrimDirty = true;
  m_bFromFile = false;
  m_vOrigin = origin;
  m_vDestPos = destpos;
  m_vDirection = destpos-origin;
  m_vDirection.normalizeIfNotZero();
  m_vUpDir = up;
  m_vRight = right;

  m_fDepth = fDepth;
  SetTexture(pTexture);
  m_bLightmapped = false;

  m_iColor = iColor;
  m_fInitialAlpha = (float)m_iColor.a;
  m_eTransp   = eTransp;

  m_fConeFactorX = fConeFactorX;
  m_fConeFactorY = fConeFactorY;
  SetVisibleBitmask(0xffffffff);
  SetInfluenceBitmask(0xffffffff);
  m_iGeometryTypeFilter = PROJECTOR_AFFECTS_DEFAULT;
  m_fFarClipDistance = -1.f;
  m_ePassType = VPT_Undefined;

  SetCustomShaderEffect(NULL);
}


VProjectedWallmark::~VProjectedWallmark()
{
}


void VProjectedWallmark::DisposeObject()
{
  VisTypedEngineObject_cl::DisposeObject();
  VWallmarkManager &manager(VWallmarkManager::GlobalManager());
  manager.m_AllProjectedWallmarks.SafeRemove(this);
  manager.m_FadingProjectedWallmarks.SafeRemove(this);
  if (manager.m_AllProjectedWallmarks.Count()==0)
    manager.m_iGeomRefHashMask = 0;
}


BOOL VProjectedWallmark::TickFunction(float dtime)
{
  if (m_fLifeTime>0.f) // otherwise infinite lifetime...
  {
    m_fLifeTime -= dtime;
    if (m_fLifeTime<=0.f)
      return TRUE;

    if (m_fLifeTime<m_fFadeOutTime)
    {
      float fBlend = m_fLifeTime/m_fFadeOutTime;
      m_iColor.a = (UBYTE)(m_fInitialAlpha*fBlend);
    }
  }
  return FALSE;
}

void VProjectedWallmark::PrepareForRendering()
{
  // update the primitive list
  if (m_bPrimDirty)
  {
    m_bPrimDirty = false;
    float fDist = GetLength();
//    float rx = fDist / m_fConeFactorX;
//    float ry = fDist / m_fConeFactorY;
//    float fRadius = hkvMath::Max(rx,ry);

    // Update boundingbox: build 8 corners of frustum
    m_BoundingBox.setInvalid();
    float fRadX = 0.5f/m_fConeFactorX;
    float fRadY = 0.5f/m_fConeFactorY;
    const float _rx[2] = {-fRadX,fRadX};
    const float _ry[2] = {-fRadY,fRadY};
    const float _rz[2] = {fDist-m_fDepth,fDist+m_fDepth};
    for (int z=0;z<2;z++) for (int y=0;y<2;y++) for (int x=0;x<2;x++)
    {
      hkvVec3 corner = m_vOrigin + m_vRight*_rx[x]*_rz[z] + m_vUpDir*_ry[y]*_rz[z] + m_vDirection*_rz[z];
      m_BoundingBox.expandToInclude(corner);
    }

    // get affected primitives
    VWallmarkManager &manager(VWallmarkManager::GlobalManager());
    m_GeoInstanceList.Clear();
    Vision::GetSceneManager()->GatherStaticGeometryInBoundingBox(m_BoundingBox,m_GeoInstanceList);

    // post filter according to some criteria
    int iCount = m_GeoInstanceList.GetNumEntries();
    for (int i=0;i<iCount;i++)
    {
      VisStaticGeometryInstance_cl *pGeom = m_GeoInstanceList.GetEntry(i);
      int iTypeBit = 1<<pGeom->GetGeometryType();
      if ((iTypeBit&m_iGeometryTypeFilter)==0 || !(pGeom->GetVisibleBitmask()&GetInfluenceBitmask()))
      {
        m_GeoInstanceList.FlagForRemoval(i);
        continue;
      }
      manager.m_iGeomRefHashMask |= VWallmarkManager::GetGeomHashMask(pGeom);
    }
    m_GeoInstanceList.RemoveFlagged();
  }
}




VTextureObject* VProjectedWallmark::SetTexture(const char *szFilename)
{
  VTextureObject *pTex = Vision::TextureManager.Load2DTexture(szFilename);
  SetTexture(pTex);
  return pTex;
}

void VProjectedWallmark::SetTexture(VTextureObject* pTexture)
{
  m_spTexture=pTexture;
  m_spTexAnim = Vision::TextureManager.GetAnimationInstance(pTexture);
}

VTextureObject* VProjectedWallmark::GetCurrentTexture() const
{
  if (m_spTexAnim)
    return m_spTexAnim->GetCurrentFrame();
  return m_spTexture;
}


bool VProjectedWallmark::SetCustomShaderEffect(VCompiledEffect *pFX)
{
  m_spCustomEffect = pFX;
  bool bResult = true;
  for (int i=0;i<STATIC_GEOMETRY_TYPE_FIRSTCUSTOM;i++)
  {
    if (pFX==NULL)
    {
      m_spCustomTechnique[i] = NULL;
      continue;
    }
    const char *szInclusion = NULL;
    if (i==STATIC_GEOMETRY_TYPE_TERRAIN)
      szInclusion = "VTerrain"; // same as for default shaders
    VTechniqueConfig cfg(szInclusion,NULL);
    m_spCustomTechnique[i] = pFX->FindCompatibleTechnique(Vision::Shaders.GetGlobalTechniqueConfig(), &cfg);
    if (m_spCustomTechnique[i]==NULL) // this is valid
      continue;
    bool bTechniqueValid = m_spCustomTechnique[i]->GetShaderCount()==1;
    for (int j=0;j<m_spCustomTechnique[i]->GetShaderCount();j++)
      if (!m_spCustomTechnique[i]->GetShader(j)->IsOfType(VProjectorShaderPass::GetClassTypeId()))
        bTechniqueValid = false;

    if (!bTechniqueValid)
    {
      Vision::Error.Warning("Tried to assign non-valid shader effect to projector");
      m_spCustomTechnique[i] = NULL;
    }
  }

  m_ePassType = VPT_Undefined;
  for (int i=0;i<STATIC_GEOMETRY_TYPE_FIRSTCUSTOM;i++)
  {
    if (m_spCustomTechnique[i]==NULL)
      m_ePassType |= VPT_TransparentPass;// default shader is considered transparent pass
    else
      m_ePassType |= m_spCustomTechnique[i]->m_ePassType;
  }
  VWallmarkManager::GlobalManager().m_eProjectedWMPassTypes |= m_ePassType;

  return bResult;
}


void VProjectedWallmark::SetPosition(const hkvVec3& vPos)
{
  float fLen = GetLength();
  m_vOrigin = vPos;
  SetLength(fLen);
  m_bPrimDirty = true;
}

void VProjectedWallmark::SetOrientation(const hkvMat3 &vMat)
{
  float fLen = GetLength();
  hkvVec3 t1, t2, t3;
  vMat.getAxisXYZ(&t1, &t2, &t3); 
  m_vDirection = t1; 
  m_vRight = t2; 
  m_vUpDir = t3;;
  SetLength(fLen);
  m_bPrimDirty = true;
}

void VProjectedWallmark::SetConeAngles(float fAngleX, float fAngleY)
{
  VASSERT(fAngleX>0.f && fAngleX<180.f);
  m_fConeFactorX = 0.5f / hkvMath::tanDeg (fAngleX / 2.0f);
  VASSERT(fAngleY>0.f && fAngleY<180.f);
  m_fConeFactorY = 0.5f / hkvMath::tanDeg (fAngleY / 2.0f);
  m_bPrimDirty = true;
}

void VProjectedWallmark::SetRadius(float fRadius)
{
  hkvVec3 dir = m_vDestPos - m_vOrigin;
  float fDistance = dir.getLength();
  float fConeFactor = fDistance / (2.f * fRadius);

  m_fConeFactorX = fConeFactor;
  m_fConeFactorY = fConeFactor;
  m_bPrimDirty = true;
}

void VProjectedWallmark::SetLength(float fLen)
{
  m_vDestPos = m_vOrigin + m_vDirection * fLen;
  m_bPrimDirty = true;
}


float VProjectedWallmark::GetLength() const
{
  return (m_vDestPos - m_vOrigin).getLength();
}


void VProjectedWallmark::SetFadeOutRange(float fDist)
{
  m_fDepth=fDist;
  m_bPrimDirty=true;
}

void VProjectedWallmark::Serialize( VArchive &ar )
{
  VisEffectConfig_cl fxConfig;
  if (ar.IsLoading())
  {
    int iVersion;
    ar >> iVersion; VASSERT(iVersion>=0 && iVersion<=PROJECTOR_CURRENT_VERSION);
    m_vOrigin.SerializeAsVisVector (ar);
    m_vDestPos.SerializeAsVisVector (ar);
    m_vDirection.SerializeAsVisVector (ar);
    m_vUpDir.SerializeAsVisVector (ar);
    m_vRight.SerializeAsVisVector (ar);
    ar >> m_fDepth;
    char szTexName[FS_MAX_PATH+1];
    ar.ReadStringBinary(szTexName,FS_MAX_PATH);
    ar >> m_iColor;
    ar >> (int &)m_eTransp;
    ar >> m_fConeFactorX >> m_fConeFactorY;
    if (iVersion>=PROJECTOR_VERSION_1)
      ar >> m_iVisibleBitmask;
    if (iVersion>=PROJECTOR_VERSION_2)
      ar >> m_bLightmapped;
    if (iVersion>=PROJECTOR_VERSION_3 && iVersion<PROJECTOR_VERSION_8)
      ar >> m_iUniqueID64;
    if (iVersion>=PROJECTOR_VERSION_4)
      ar >> m_iGeometryTypeFilter; // version 4
    if (iVersion>=PROJECTOR_VERSION_5)
      ar >> m_fFarClipDistance; // version 5
    if (iVersion>=PROJECTOR_VERSION_6)
      ar >> m_InfluenceBitmask; // version 6
    if (iVersion>=PROJECTOR_VERSION_7)
      ar >> fxConfig; // version 7

    m_bPrimDirty=true; // recalc primitive list

    if (m_iGeometryTypeFilter==-1)
      m_iGeometryTypeFilter = PROJECTOR_AFFECTS_DEFAULT; // entities are not default
    // load the texture
    SetTexture(szTexName);
    SetCustomShaderEffect(fxConfig.GetEffect(0)); // may be NULL
    m_bFromFile = true;
  } else
  {
    ar << (int)PROJECTOR_CURRENT_VERSION; // version
    m_vOrigin.SerializeAsVisVector (ar);
    m_vDestPos.SerializeAsVisVector (ar);
    m_vDirection.SerializeAsVisVector (ar);
    m_vUpDir.SerializeAsVisVector (ar);
    m_vRight.SerializeAsVisVector (ar);
    ar <<  m_fDepth;
    const char *texname = m_spTexture ? m_spTexture->GetFilename() : NULL;
    ar << texname;
    ar << m_iColor;
    ar << (int)m_eTransp;
    ar << m_fConeFactorX << m_fConeFactorY;
    ar << m_iVisibleBitmask;
    ar << m_bLightmapped;
    //ar << m_iUniqueID64; // prior to version 7
    ar << m_iGeometryTypeFilter; // version 4
    ar << m_fFarClipDistance; // version 5
    ar << m_InfluenceBitmask; // version 6
    fxConfig.SetEffect(m_spCustomEffect);
    ar << fxConfig; // version 8

  }
}


#ifdef SUPPORTS_SNAPSHOT_CREATION

void VProjectedWallmark::GetDependencies(VResourceSnapshot &snapshot)
{
  VisTypedEngineObject_cl::GetDependencies(snapshot);
  if (m_spTexture) 
    m_spTexture->GetDependencies(snapshot);
}

#endif

void VProjectedWallmark::GetProjectionPlanes(hkvPlane& s, hkvPlane& t, hkvPlane& q, hkvPlane& att) const
{
  s.m_vNormal.x = m_fConeFactorX * m_vRight.x;
  s.m_vNormal.y = m_fConeFactorX * m_vRight.y;
  s.m_vNormal.z = m_fConeFactorX * m_vRight.z;
  s.m_fNegDist = -(s.m_vNormal.x*m_vOrigin[0] + s.m_vNormal.y*m_vOrigin[1] + s.m_vNormal.z*m_vOrigin[2]);

  t.m_vNormal.x = m_fConeFactorY * m_vUpDir.x;
  t.m_vNormal.y = m_fConeFactorY * m_vUpDir.y;
  t.m_vNormal.z = m_fConeFactorY * m_vUpDir.z;
  t.m_fNegDist = -(t.m_vNormal.x*m_vOrigin[0] + t.m_vNormal.y*m_vOrigin[1] + t.m_vNormal.z*m_vOrigin[2]);

  q.m_vNormal.x = m_vDirection.x;
  q.m_vNormal.y = m_vDirection.y;
  q.m_vNormal.z = m_vDirection.z;
  q.m_fNegDist = -(q.m_vNormal.x*m_vOrigin[0] + q.m_vNormal.y*m_vOrigin[1] + q.m_vNormal.z*m_vOrigin[2]);

  float invDepth = 1.f/m_fDepth;
  att.m_vNormal.x = invDepth * m_vDirection.x;
  att.m_vNormal.y = invDepth * m_vDirection.y;
  att.m_vNormal.z = invDepth * m_vDirection.z;
  att.m_fNegDist = -(att.m_vNormal.x * m_vDestPos[0] + att.m_vNormal.y * m_vDestPos[1] + att.m_vNormal.z * m_vDestPos[2]);
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
