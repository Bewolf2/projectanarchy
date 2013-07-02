/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEmitter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


///////////////////////////////////////////////////////////////////////////////////
// Particle group emitter
///////////////////////////////////////////////////////////////////////////////////
 
VisParticleEmitter_cl::VisParticleEmitter_cl()
{
  // don't forget to copy members in Clone function!
  SetType_Point();
  m_ParticlesPerSec.Set(20.f,0.f);
  m_FixParticleCount.Set(-1.f,0.f);
  m_StartupFillPercentage.Set(-1.f,0.f);
  m_fConeAngle  = -1.f; // all directions
  m_fMinConeAngle = 0.f;
  m_eConeMode = CONE_ANGLE_MODE_ALL_DIMENSIONS;
  m_fCountLookupTime = 10.f;
  m_fSpawnCountLookupPos = 0.f;
  m_fSpawnTimeCtr = 0.f;// Vision::Game.GetFloatRand();
  m_bRandomOffset = false;
  m_bEmitFromSurface = false;
  m_fIntensity = 1.f;
  m_bInterpolateEmitterPos = true;
  //m_spCountCurve = new VCurve2D();
}


VisParticleEmitter_cl::~VisParticleEmitter_cl()
{
}

#define COPY_MEMBER(member) {pNew->member=member;}

VisParticleEmitter_cl *VisParticleEmitter_cl::Clone(const VRandom& randGen) const
{
  VisParticleEmitter_cl *pNew = new VisParticleEmitter_cl;

  COPY_MEMBER(m_eType);
  COPY_MEMBER(m_vParam);
  COPY_MEMBER(m_spPath);

  COPY_MEMBER(m_fConeAngle);
  COPY_MEMBER(m_fMinConeAngle);
  COPY_MEMBER(m_eConeMode);
  COPY_MEMBER(m_ParticlesPerSec);
  COPY_MEMBER(m_spCountCurve);
  COPY_MEMBER(m_fSpawnCountLookupPos);
  COPY_MEMBER(m_bRandomOffset);
  COPY_MEMBER(m_bEmitFromSurface);
  COPY_MEMBER(m_FixParticleCount);
  COPY_MEMBER(m_StartupFillPercentage);
  COPY_MEMBER(m_fCountLookupTime);

  COPY_MEMBER(m_fSpawnCountLookupPos);
  COPY_MEMBER(m_fSpawnTimeCtr);
  COPY_MEMBER(m_fIntensity);
  COPY_MEMBER(m_spEmitterMask);

  // the cloned emitter should have this variation:
  if (pNew->m_bRandomOffset)
  {
    pNew->m_fSpawnCountLookupPos = randGen.GetFloat();
    pNew->m_fSpawnTimeCtr = randGen.GetFloat();
  }

  return pNew;
}

bool VisParticleEmitter_cl::GetBoundingBox(hkvAlignedBBox &dstBox) const
{
  switch (m_eType)
  {
    case EMITTER_TYPE_UNKNOWN:return false;
    case EMITTER_TYPE_POINT:
      dstBox.m_vMin.set(-1.f,-1.f,-1.f);
      dstBox.m_vMax.set( 1.f, 1.f, 1.f);
      return true;
    case EMITTER_TYPE_SPHERE:
      dstBox.m_vMax.set(m_vParam.x,m_vParam.x,m_vParam.x);
      dstBox.m_vMin = -dstBox.m_vMax;
      return true;

    case EMITTER_TYPE_BOX:
      dstBox.m_vMax = m_vParam.getAsVec3 ();
      dstBox.m_vMin = -dstBox.m_vMax;
      return true;
    case EMITTER_TYPE_PLANE:
      dstBox.m_vMax = m_vParam.getAsVec3 ();
      dstBox.m_vMax.z = 1.f;
      dstBox.m_vMin = -dstBox.m_vMax;
      return true;

    case EMITTER_TYPE_PATH:
      VASSERT(m_spPath);
//      m_spPath->GetBoundingBox(); // does not exist yet
      VASSERT(FALSE);
      return false;

    case EMITTER_TYPE_RAY:
      dstBox.m_vMin.set(-1.f,-1.f,-1.f);
      dstBox.m_vMax.set(m_vParam.x,1.f,1.f);
      return true;
  }
  return false;
}


const char *VisParticleEmitter_cl::GetTypeName() const
{
  switch (m_eType)
  {
    case EMITTER_TYPE_UNKNOWN:return "unknown";
    case EMITTER_TYPE_POINT:return "point";
    case EMITTER_TYPE_SPHERE:return "sphere";
    case EMITTER_TYPE_BOX:return "box";
    case EMITTER_TYPE_PLANE:return "plane";
    case EMITTER_TYPE_PATH:return "path";
    case EMITTER_TYPE_RAY:return "ray";
    case EMITTER_TYPE_MESH:return "mesh";
  }
  VASSERT(FALSE);
  return "unknown";
}


#define CHECK_EMITTER_NAME(name,type) \
  if (!_stricmp(szName,name)) {m_eType=type;return true;}

bool VisParticleEmitter_cl::GetTypeFromName(const char *szName)
{
  m_eType=EMITTER_TYPE_UNKNOWN;
  if (!szName) return false;
  CHECK_EMITTER_NAME("unknown",EMITTER_TYPE_UNKNOWN);
  CHECK_EMITTER_NAME("point",EMITTER_TYPE_POINT);
  CHECK_EMITTER_NAME("sphere",EMITTER_TYPE_SPHERE);
  CHECK_EMITTER_NAME("box",EMITTER_TYPE_BOX);
  CHECK_EMITTER_NAME("plane",EMITTER_TYPE_PLANE);
  CHECK_EMITTER_NAME("path",EMITTER_TYPE_PATH);
  CHECK_EMITTER_NAME("ray",EMITTER_TYPE_RAY);
  CHECK_EMITTER_NAME("mesh",EMITTER_TYPE_MESH);
  VASSERT(FALSE);
  return false;
}


void VisParticleEmitter_cl::ModulateEmitterMask(float u, float v, VColorRef &iDestColor) const
{
  VASSERT(m_spEmitterMask!=NULL);
  int x,y;
  m_spEmitterMask->MapUVToTexel(u,v,x,y);
  iDestColor *= m_spEmitterMask->LookupTexelColor(x,y);
}


void VisParticleEmitter_cl::SpawnSingleParticle(ParticleExt_t *pParticle, ParticleGroupBase_cl *pGroup, VColorRef &iColor, hkvMat3 &emitCS, bool &bEmitCSValid)
{
  VASSERT(pGroup);
  hkvVec3 vGroupPos(hkvNoInitialization);
  hkvVec3 vTempVec(hkvNoInitialization);
  bEmitCSValid = false;
  const VRandom& randGen = pGroup->GetRandom();
  hkvVec4 vScaledParam = m_vParam;
  vScaledParam *= pGroup->GetScaling();

  // interpolate properly between this position and last frame's position
  const BOOL bLocalSpace = pGroup->GetUseLocalSpaceMatrix();
  if (bLocalSpace)
    vGroupPos.set(0.f,0.f,0.f);
  else
  {
    float fWeight = m_fParticleAge + pGroup->m_fLocalFactorStart - ( m_fParticleAge * pGroup->m_fLocalFactorStart );
    vGroupPos = m_vLastEmitterPos + (pGroup->m_vPosition - m_vLastEmitterPos) * fWeight;
  }

  switch (m_eType)
  {
    case EMITTER_TYPE_POINT:
      pParticle->pos[0] = vGroupPos.x;
      pParticle->pos[1] = vGroupPos.y;
      pParticle->pos[2] = vGroupPos.z;
      break;
    case EMITTER_TYPE_SPHERE:
    {
      Helper_GetRandomSpherePosition(vTempVec,vScaledParam.x,randGen);
      if (m_bEmitFromSurface)
      {
        bEmitCSValid = true;
        vTempVec.normalizedEnsureUnitLength();
        emitCS.setLookInDirectionMatrix(vTempVec);
        vTempVec *= vScaledParam.x;
      }
      pParticle->pos[0] = vGroupPos.x + vTempVec.x;
      pParticle->pos[1] = vGroupPos.y + vTempVec.y;
      pParticle->pos[2] = vGroupPos.z + vTempVec.z;

      break;
    }
    case EMITTER_TYPE_BOX:
    {
      const float fx = randGen.GetFloatNeg()*vScaledParam.x;
      const float fy = randGen.GetFloatNeg()*vScaledParam.y;
      const float fz = randGen.GetFloatNeg()*vScaledParam.z;

      if (m_bEmitFromSurface)
      {
        int iFace = randGen.GetInt()%6;
        hkvVec3 pos(hkvNoInitialization);
        switch (iFace)
        {
          case VTM_CUBEMAPFACE_POS_X:pos.set( vScaledParam.x,fy,fz);break;
          case VTM_CUBEMAPFACE_NEG_X:pos.set(-vScaledParam.x,fy,fz);break;
          case VTM_CUBEMAPFACE_POS_Y:pos.set(fx, vScaledParam.y,fz);break;
          case VTM_CUBEMAPFACE_NEG_Y:pos.set(fx,-vScaledParam.y,fz);break;
          case VTM_CUBEMAPFACE_POS_Z:pos.set(fx,fy, vScaledParam.z);break;
          case VTM_CUBEMAPFACE_NEG_Z:pos.set(fx,fy,-vScaledParam.z);break;
        }
        hkvVec3 vFaceNrml = VLightGrid_cl::GetFaceNormal(iFace);
        if (!bLocalSpace)
        {
          vFaceNrml = pGroup->GetRotationMatrix() * vFaceNrml;
          pos = pGroup->GetRotationMatrix() * pos + vGroupPos;
        }
        pParticle->pos[0] = pos.x;
        pParticle->pos[1] = pos.y;
        pParticle->pos[2] = pos.z;
        bEmitCSValid = true;
        emitCS.setLookInDirectionMatrix(vFaceNrml);
      } 
      else // m_bEmitFromSurface
      {
        if (bLocalSpace)
        {
          pParticle->pos[0] = fx;
          pParticle->pos[1] = fy;
          pParticle->pos[2] = fz;
        } else
        {
          hkvVec3 pos = vGroupPos + pGroup->m_vDirection*fx + pGroup->m_vRightDir*fy + pGroup->m_vUpDir*fz;
          pParticle->pos[0] = pos.x;
          pParticle->pos[1] = pos.y;
          pParticle->pos[2] = pos.z;
        }
      }
      break;
    }
    case EMITTER_TYPE_PLANE:
    {
      const float fx = randGen.GetFloatNeg();
      const float fy = randGen.GetFloatNeg();
      if (bLocalSpace)
      {
        pParticle->pos[0] = 0.f;
        pParticle->pos[1] = fx*vScaledParam.x;
        pParticle->pos[2] = fy*vScaledParam.y;
      } else
      {
        hkvVec3 pos = vGroupPos + pGroup->m_vRightDir*fx*vScaledParam.x + pGroup->m_vUpDir*fy*vScaledParam.y;
        pParticle->pos[0] = pos.x;
        pParticle->pos[1] = pos.y;
        pParticle->pos[2] = pos.z;
      }
      // note: m_bEmitFromSurface is not necessary here as it matches the group's coordinate system anyway
      if (m_spEmitterMask!=NULL)
      {
        ModulateEmitterMask(fx*0.5f+0.5f,fy*0.5f+0.5f, iColor);
        if (iColor.a==0)
          pGroup->AddParticleToCache(pParticle); // discard this particle
      }
      break;
    }
    case EMITTER_TYPE_RAY:
    {
      float fT = randGen.GetFloat() * vScaledParam.x;
      if (bLocalSpace)
      {
        pParticle->pos[0] = fT;
        pParticle->pos[1] = 0.f;
        pParticle->pos[2] = 0.f;
      }
      else
      {
        pParticle->pos[0] = vGroupPos.x + pGroup->m_vDirection.x*fT;
        pParticle->pos[1] = vGroupPos.y + pGroup->m_vDirection.y*fT;
        pParticle->pos[2] = vGroupPos.z + pGroup->m_vDirection.z*fT;
      }
      if (m_bEmitFromSurface) // == span particles orthogonal to the ray axis
      {
        float fAngle = randGen.GetFloat() * hkvMath::pi()*2.f;
        float fSin = hkvMath::sinRad(fAngle);
        float fCos = hkvMath::cosRad(fAngle);
        bEmitCSValid = true;
        if (bLocalSpace)
        {
          // swizzle the axes (dir becomes up)
          const hkvVec3 vRight(0,1,0), vUp(0,0,1);
          emitCS.setAxis(0,vRight *  fCos + vUp * fSin);
          emitCS.setAxis(1,vRight * -fSin + vUp * fCos);
          emitCS.setAxis(2,hkvVec3(1,0,0));
        }else
        {
          // swizzle the axes (dir becomes up)
          emitCS.setAxis(0,pGroup->m_vRightDir *  fCos + pGroup->m_vUpDir * fSin);
          emitCS.setAxis(1,pGroup->m_vRightDir * -fSin + pGroup->m_vUpDir * fCos);
          emitCS.setAxis(2,pGroup->m_vDirection);
        }
      }
      break;
    }
    case EMITTER_TYPE_PATH:
    {
      float fT = randGen.GetFloat();
      VASSERT(m_spPath);
      m_spPath->EvalPointSmooth(fT,vTempVec);
      pParticle->pos[0] = vTempVec.x;
      pParticle->pos[1] = vTempVec.y;
      pParticle->pos[2] = vTempVec.z;
      break;
    }
    case EMITTER_TYPE_MESH:
    {
      if (m_spEmitterMesh)
      {
        VisBaseEntity_cl *pEnt = m_wpEntityMeshEmitter.GetPtr();
        hkvMat4 worldMatrix(hkvNoInitialization);
        pEnt->GetWorldMatrix(worldMatrix);
        hkvVec3 vPos(hkvNoInitialization);
        int iRandomVertex, iRandomTri;
        if (m_bEmitFromSurface)
        {
          VTriangle tri;
          int iTriCount = m_spEmitterMesh->GetTriangleCount();
          iRandomTri = randGen.GetInt()%iTriCount;
          m_spEmitterMesh->GetMeshData()->GetTriangle(tri, iRandomTri);
          vPos = (tri.GetVertex0() + tri.GetVertex1() + tri.GetVertex2()) * (1.f/3.f); // center of triangle
          bEmitCSValid = true;
          hkvVec3 vNrml = worldMatrix.transformDirection(-tri.GetNormal());
          
          emitCS.setLookInDirectionMatrix(vNrml);
          iRandomVertex = m_spEmitterMesh->GetMeshData()->GetVertexIndex(iRandomTri,0); // must be initialized as well for the emitter mask
        } else
        {
          int iVertexCount = m_spEmitterMesh->GetVertexCount();
          iRandomVertex = randGen.GetInt()%iVertexCount;
          vPos = *m_spEmitterMesh->GetMeshData()->GetVertex(iRandomVertex);
        }

        vPos = worldMatrix.transformPosition(vPos);

        pParticle->pos[0] = vPos.x;
        pParticle->pos[1] = vPos.y;
        pParticle->pos[2] = vPos.z;

        if (m_spEmitterMask!=NULL)
        {
          const hkvVec2* pUV = m_spEmitterMesh->GetVertexBaseUV(true);
          VASSERT(pUV!=NULL);
          ModulateEmitterMask(pUV[iRandomVertex].x,pUV[iRandomVertex].y, iColor);
          if (iColor.a==0)
            pGroup->AddParticleToCache(pParticle); // discard this particle
        }
      } 
      else
      {
        pParticle->pos[0] = vGroupPos.x;
        pParticle->pos[1] = vGroupPos.y;
        pParticle->pos[2] = vGroupPos.z;
      }
      break;
    }
  }
}


void VisParticleEmitter_cl::HandleEmitter(ParticleGroupBase_cl *pGroup, float fTimeDelta, int &iRemainingCount)
{
  int i;
  VASSERT(pGroup);
  if (iRemainingCount==0) return; // -1 means no limit

  float fCurrentGrowSpeed;
  if (m_spCountCurve)
  {
    VASSERT(m_fCountLookupTime>0.f)
    m_fSpawnCountLookupPos = hkvMath::mod (m_fSpawnCountLookupPos+fTimeDelta/m_fCountLookupTime,1.f);
    fCurrentGrowSpeed = m_spCountCurve->GetValueFast(m_fSpawnCountLookupPos);
  } else
  {
    fCurrentGrowSpeed = m_ParticlesPerSec.m_fAverage;
  }
  if (m_ParticlesPerSec.m_fVariation>0.f)
    fCurrentGrowSpeed = VisParticleGroupDescriptor_cl::GetRandomValue(pGroup->GetRandom(), fCurrentGrowSpeed, m_ParticlesPerSec.m_fVariation);

  // new particle count in this frame
  float fTimeReminder = hkvMath::mod (m_fSpawnTimeCtr,1.f);
  m_fSpawnTimeCtr = fTimeReminder + fCurrentGrowSpeed*fTimeDelta*m_fIntensity;
  int iNewCount = (int)m_fSpawnTimeCtr;
  if (iRemainingCount>0)
  {
    if (iNewCount>iRemainingCount)
      iNewCount = iRemainingCount;
    iRemainingCount -= iNewCount;
  }

  if (iNewCount) // spawn new particles
  {
    float fTimeStep = 0.999f/(float)iNewCount; // avoid rounding issues
    float fFractionOfs = 1.f; // start with oldest (imnportant for trails)
    pGroup->m_iValidCount += iNewCount; // keep system from destroying itself
    pGroup->m_vGroupMoveDelta.setZero();
    if (!m_bInterpolateEmitterPos)
      m_vLastEmitterPos = pGroup->m_vPosition;

    for (i=0;i<iNewCount;i++,fFractionOfs-=fTimeStep)
    {
      ParticleExt_t *p = pGroup->GetFreeParticle();
      if (!p) break; // no free particle anymore?
      m_fParticleAge = 1.0f - fFractionOfs; // interpolation value. "old" particles (weight=0) should be simulated with highest time delta
      pGroup->InitSingleParticle(p);

      float fScaledDelta = fFractionOfs*fTimeDelta;

      // setup per frame variables that are dtime dependent
      pGroup->SetPerFrameConstants(fScaledDelta);

      // do not spawn all at same position for >1 particle per frame
      pGroup->HandleSingleParticle(p, fScaledDelta);
    }
  }

  m_vLastEmitterPos = pGroup->m_vPosition;
}



bool VisParticleEmitter_cl::DataExchangeXML(TiXmlElement *pEmitter, bool bWrite)
{
  if (bWrite)
    pEmitter->SetAttribute("type",GetTypeName());
  else
    GetTypeFromName(pEmitter->Attribute("type"));

  hkvVec3 vTemp;

  switch (m_eType)
  {
    case EMITTER_TYPE_POINT:
      break;
    case EMITTER_TYPE_SPHERE:
      XMLHelper::Exchange_Float(pEmitter,"radius",m_vParam.x,bWrite);
      break;
    case EMITTER_TYPE_BOX:
      XMLHelper::Exchange_Floats(pEmitter,"boxdim",m_vParam.data,3,bWrite);
      break;
    case EMITTER_TYPE_PLANE:
      XMLHelper::Exchange_Floats(pEmitter,"planedim",m_vParam.data,2,bWrite);
      break;
    case EMITTER_TYPE_RAY:
      XMLHelper::Exchange_Float(pEmitter,"length",m_vParam.x,bWrite);
      break;
    case EMITTER_TYPE_PATH:
      if (bWrite)
      {
        const char *szPathKey = m_spPath->GetKey();
        if (szPathKey)
          pEmitter->SetAttribute("pathkey",szPathKey);
      } else
      {
        const char *szPathKey = pEmitter->Attribute("pathkey");
        SetType_Path(szPathKey);
      }
      break;
  }

  XMLHelper::Exchange_Bool(pEmitter,"emitfromsurface",m_bEmitFromSurface,bWrite);
  m_StartupFillPercentage.DataExchangeXML("startupfillpercentage",pEmitter,bWrite);
  m_FixParticleCount.DataExchangeXML("fixparticlecount",pEmitter,bWrite);
  m_ParticlesPerSec.DataExchangeXML("frequency",pEmitter,bWrite);
//  m_ParticlesPerSec.Set(50.0f, 0.5f); // testing simple sparks
  XMLHelper::Exchange_VString(pEmitter,"mask", m_sEmitterMaskFilename, bWrite);

  TiXmlElement *pLookup = XMLHelper::SubNode(pEmitter,"countlookup",bWrite);
  if (pLookup)
  {
    m_spCountCurve = VCurve2D::Exchange_Curve(pLookup,"curve",m_spCountCurve,bWrite);
    if (m_spCountCurve && !bWrite)
      m_spCountCurve->CreateLookup(256);

    XMLHelper::Exchange_Float(pLookup,"time",m_fCountLookupTime,bWrite);
    XMLHelper::Exchange_Bool(pLookup,"randomofs",m_bRandomOffset,bWrite);
    if (m_bRandomOffset)
    {
      // Just make a quick rand object here (table is static so this is just on the stack).
      VRandom quickRandGen(Vision::Game.GetUpdateSceneCount() * 137);
      m_fSpawnCountLookupPos = quickRandGen.GetFloat();
      m_fSpawnTimeCtr = quickRandGen.GetFloat();
    }
  }
  TiXmlElement *pDirection = XMLHelper::SubNode(pEmitter,"direction",bWrite);
  if (pDirection)
  {
    float fConeAngles[2] = {m_fMinConeAngle,m_fConeAngle};
    int iCount = XMLHelper::Exchange_Floats(pDirection,"coneangle",fConeAngles,2,bWrite);
    if (iCount==1) // old format
      m_fConeAngle = fConeAngles[0];
    else if (iCount==2) // new version: min/max
    {
      m_fMinConeAngle = fConeAngles[0];
      m_fConeAngle = fConeAngles[1];
    }

    const char *coneMode[] = {"alldims","yaw","pitch"};
    const int coneModeVal[3] = {CONE_ANGLE_MODE_ALL_DIMENSIONS, CONE_ANGLE_MODE_YAW_ONLY, CONE_ANGLE_MODE_PITCH_ONLY};
    XMLHelper::Exchange_Enum(pDirection,"conemode",(int &)m_eConeMode,3,coneMode,coneModeVal,bWrite);
  }

  return true;
}



void VisParticleEmitter_cl::OnXMLExchangeFinished(VisParticleGroupDescriptor_cl *pDesc)
{
  if (!m_sEmitterMaskFilename.IsEmpty())
  {
    char szFilename[FS_MAX_PATH];
    pDesc->MakeFilenameAbsolute(szFilename, m_sEmitterMaskFilename);
    m_spEmitterMask = VisBitmap_cl::LoadBitmapFromFile(szFilename);
    if (m_spEmitterMask !=NULL && !m_spEmitterMask->IsLoaded())
      m_spEmitterMask = NULL;
  }
}


V_IMPLEMENT_SERIALX( VisParticleEmitter_cl);
void VisParticleEmitter_cl::SerializeX( VArchive &ar )
{
  char iVersion = 2;
  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT(iVersion<=2 && "Invalid version number");
    ar >> (int &)m_eType;
    ar >> m_vParam;
    ar >> m_fConeAngle >> m_fMinConeAngle;
    ar >> (int &)m_eConeMode;
    ar >> m_ParticlesPerSec;
    ar >> m_FixParticleCount;
    ar >> m_StartupFillPercentage;
    ar >> m_fCountLookupTime;
    ar >> m_bRandomOffset;
    ar >> m_bInterpolateEmitterPos;
    ar >> m_fIntensity;
    ar >> m_fSpawnCountLookupPos;
    ar >> m_fSpawnTimeCtr;
    m_spCountCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spCountCurve);
    if (iVersion>=1)
    {
      m_spEmitterMask = VisBitmap_cl::DoArchiveExchange(ar, NULL);
      if (m_spEmitterMask !=NULL && !m_spEmitterMask->IsLoaded())
        m_spEmitterMask = NULL;
    }
    if (iVersion>=2)
      ar >> m_bEmitFromSurface;
  } 
  else
  {
    ar << iVersion;
    ar << (int)m_eType;
    ar << m_vParam;
    ar << m_fConeAngle << m_fMinConeAngle;
    ar << (int)m_eConeMode;
    ar << m_ParticlesPerSec;
    ar << m_FixParticleCount;
    ar << m_StartupFillPercentage;
    ar << m_fCountLookupTime;
    ar << m_bRandomOffset;
    ar << m_bInterpolateEmitterPos;
    ar << m_fIntensity;
    ar << m_fSpawnCountLookupPos;
    ar << m_fSpawnTimeCtr;
    m_spCountCurve = VCurve2D::DoArchiveLookupExchange(ar,m_spCountCurve);
    VisBitmap_cl::DoArchiveExchange(ar, m_spEmitterMask); // version 1
    ar << m_bEmitFromSurface; // version 2
  }
}


V_IMPLEMENT_SERIALX( VisParticleEmitterList_cl);
void VisParticleEmitterList_cl::SerializeX( VArchive &ar )
{
  if (ar.IsLoading())
  {
    int iCount;
    ar >> iCount;
    EnsureCapacity(iCount);
    VisParticleEmitter_cl *pEmitter = NULL;
    for (int i=0;i<iCount;i++)
    {
      ar >> pEmitter;
      Add(pEmitter); // must be refcounted
    }
  }
  else
  {
    int iCount = Count();
    ar << iCount;
    for (int i=0;i<iCount;i++)
      ar << GetAt(i);
      
  }
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
