/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VRenderInterface.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VString VisParticleConstraintList_cl::g_sLastError;

VisParticleConstraintList_cl VisParticleConstraint_cl::g_GlobalConstraintList;

VisParticleConstraintList_cl &VisParticleConstraint_cl::GlobalConstraintList()
{
  return g_GlobalConstraintList;
}



////////////////////////////////////////////////////////////////////
// Constraint base class
////////////////////////////////////////////////////////////////////

V_IMPLEMENT_SERIAL( VisParticleConstraint_cl, VisObject3D_cl, 0, &g_VisionEngineModule );

VisParticleConstraint_cl::VisParticleConstraint_cl(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode, float fPersistance)
{
  SetActive(true);
  m_bIsDead = false;
  m_iDebugColor.SetRGBA(100,180,255,64);
  m_eReflectBehavior = eReflectMode;
  m_fPersistance = fPersistance;
  m_iAffectBitMask = 0xffffffff;
  m_fReflectionNoise = 0.0f;
  g_GlobalConstraintList.AddConstraint(this);
}

VisParticleConstraint_cl::VisParticleConstraint_cl() // serialisation
{
  m_fReflectionNoise = 0.0f;
  g_GlobalConstraintList.AddConstraint(this);
}


VisParticleConstraint_cl::~VisParticleConstraint_cl()
{
}

void VisParticleConstraint_cl::DisposeObject()
{
  VisObject3D_cl::DisposeObject();
  g_GlobalConstraintList.RemoveConstraint(this);
}



bool VisParticleConstraint_cl::DataExchangeXML_Base(TiXmlElement *pNode, const char *szType, bool bWrite)
{
  if (!pNode)
    return false;
  if (bWrite)
    pNode->SetAttribute("type",szType);

  TiXmlElement *pReflect = XMLHelper::SubNode(pNode,"reflect",bWrite);
  if (pReflect)
  {
    VASSERT(sizeof(m_eReflectBehavior)==sizeof(int));
    XMLHelper::Exchange_Int(pReflect,"behavior",(int &)m_eReflectBehavior,bWrite);
    XMLHelper::Exchange_Float(pReflect,"persistance",m_fPersistance,bWrite);
    XMLHelper::Exchange_Float(pReflect,"noise",m_fReflectionNoise,bWrite);
  }
  XMLHelper::Exchange_Color(pNode,"debugcolor",m_iDebugColor,bWrite);
  XMLHelper::Exchange_Int(pNode,"constraintmask",(int &)m_iAffectBitMask,bWrite);

  return true;
}


#define CREATE_INSTANCE(classname) \
{\
  if (!_stricmp(szType,classname::GetShortName()))\
    return new classname(pNode);\
}

VisParticleConstraint_cl *VisParticleConstraint_cl::CreateConstraint(TiXmlElement *pNode)
{
  if (!pNode) return NULL;
  const char *szType = pNode->Attribute("type");
  if (!szType || !szType[0]) return NULL;

  CREATE_INSTANCE(VisParticleConstraintPoint_cl);
  CREATE_INSTANCE(VisParticleConstraintGroundPlane_cl);
  CREATE_INSTANCE(VisParticleConstraintPlane_cl);
  CREATE_INSTANCE(VisParticleConstraintSphere_cl);
  CREATE_INSTANCE(VisParticleConstraintInfCylinder_cl);
  CREATE_INSTANCE(VisParticleConstraintAABox_cl);
  CREATE_INSTANCE(VisParticleConstraintCamBox_cl);
  CREATE_INSTANCE(VisParticleAffectorFan_cl);
  CREATE_INSTANCE(VisParticleAffectorCyclone_cl);
  CREATE_INSTANCE(VisParticleAffectorGravityPoint_cl);
//  CREATE_INSTANCE(VisParticleConstraintTerrain_cl); // not supported
  
  // add more classes here

  VASSERT(FALSE); // invalid constraint type
  return NULL;
}

void VisParticleConstraint_cl::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  m_iLocalLoadingVersion = 3;

  if (ar.IsLoading())
  {
    int iTmp = 0;
    ar >> m_iLocalLoadingVersion; VVERIFY(m_iLocalLoadingVersion<=3 && "invalid constraint version number");
    ar >> m_iDebugColor;
    ar >> iTmp;m_eReflectBehavior=(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e)iTmp;
    ar >> m_bActive >> m_bIsDead >> m_fPersistance;
    ar >> m_iAffectBitMask;
    if (m_iLocalLoadingVersion>=2)
    {
      ar >> m_fReflectionNoise;
    } else
    {
      m_fReflectionNoise = 0.0f;
    }
  } else
  {
    ar << m_iLocalLoadingVersion;
    ar << m_iDebugColor;
    ar << (int)m_eReflectBehavior;
    ar << m_bActive << m_bIsDead << m_fPersistance;
    ar << m_iAffectBitMask;
    ar << m_fReflectionNoise; // version 2
  }
}

////////////////////////////////////////////////////////////////////
// Constraint list
////////////////////////////////////////////////////////////////////

VisParticleConstraintList_cl::VisParticleConstraintList_cl()
: m_Constraint(0,NULL),
  m_ForceBehavior(0,CONSTRAINT_REFLECT_DEFAULT)
{
  m_iConstraintCount = 0;
}

VisParticleConstraintList_cl::~VisParticleConstraintList_cl()
{
  ReleaseAllConstraints();
}


void VisParticleConstraintList_cl::ReleaseAllConstraints()
{
  int i;
  for (i=0;i<m_iConstraintCount;i++)
    V_SAFE_RELEASE(m_Constraint[i]);

  m_iConstraintCount = 0;
}

void VisParticleConstraintList_cl::CopyFrom(const VisParticleConstraintList_cl* pSrcList)
{
  ReleaseAllConstraints();
  if (!pSrcList) return;
  for (int i=0;i<pSrcList->m_iConstraintCount;i++)
    AddConstraint(pSrcList->GetConstraint(i));
  
}

void VisParticleConstraintList_cl::AddFrom(const VisParticleConstraintList_cl* pSrcList)
{
  if (!pSrcList)
    return;

  for (int i=0;i<pSrcList->m_iConstraintCount;i++)
  {
    VisParticleConstraint_cl *pConstraint = pSrcList->GetConstraint(i);

    if(pConstraint==NULL)
      continue;

    //just add the constraint if it doesn't already exist in the target list
    if(m_Constraint.GetElementPos(pConstraint, m_iConstraintCount)<0)
      AddConstraint(pConstraint);
  }
}


int VisParticleConstraintList_cl::AddConstraint(VisParticleConstraint_cl *pConstraint, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pConstraint && m_Constraint.GetElementPos(pConstraint, m_iConstraintCount)<0);
  int iIndex = m_Constraint.GetFreePos();
  pConstraint->AddRef();
  m_Constraint[iIndex] = pConstraint;
  m_ForceBehavior[iIndex] = eForceBehavior;
  if (iIndex>=m_iConstraintCount)
    m_iConstraintCount = iIndex+1;
  return iIndex;
}

bool VisParticleConstraintList_cl::RemoveConstraint(int iIndex)
{
  VisParticleConstraint_cl *pConstraint = GetConstraint(iIndex);
  if (pConstraint)
  {
    VisParticleConstraint_cl **pArray = m_Constraint.GetDataPtr();
    pConstraint->Release();
    pArray[iIndex] = NULL;
    while (m_iConstraintCount>0 && !pArray[m_iConstraintCount-1])
      m_iConstraintCount--;
    return true;
  }
  return false;
}

bool VisParticleConstraintList_cl::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  if (!pConstraint) return false;
  int iIndex = m_Constraint.GetElementPos(pConstraint, m_iConstraintCount);
  if (iIndex<0) return false;
  return RemoveConstraint(iIndex);
}


bool VisParticleConstraintList_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!pNode) return false;

  if (bWrite)
  {
    int i;
    for (i=0;i<GetConstraintCount();i++)
    {
      VisParticleConstraint_cl *pConstraint = GetConstraint(i);
      if (!pConstraint) continue;
      TiXmlElement constraint(XML_CONSTRAINT_NAME);
      TiXmlNode *pAdded = pNode->InsertEndChild(constraint);

      pConstraint->DataExchangeXML(pAdded->ToElement(),true);
    }
  }
  else
  {
    TiXmlElement* pElement;

    // load constraints
    TiXmlNode *pFirstNode = pNode->FirstChild(XML_CONSTRAINT_NAME);
    if (pFirstNode)
    {
      // iterate through all XML_CONSTRAINT_NAME nodes 
	    for (pElement=pFirstNode->ToElement(); pElement!=NULL; pElement=pElement->NextSiblingElement(XML_CONSTRAINT_NAME))
	    {
        VisParticleConstraint_cl *pConstraint = VisParticleConstraint_cl::CreateConstraint(pElement);
        if (!pConstraint) continue;
        AddConstraint(pConstraint);
	    }
    }
  }

  return true;
}


bool VisParticleConstraintList_cl::LoadFromFile(const char *szFilename)
{
  g_sLastError.Reset();
  TiXmlDocument doc;
  if (!doc.LoadFile(szFilename,Vision::File.GetManager()))
  {  
    g_sLastError = doc.ErrorDesc();
    return false; // error when loading
  }
  VASSERT(doc.RootElement());
  return DataExchangeXML(doc.RootElement(),false);
}

bool VisParticleConstraintList_cl::SaveToFile(const char *szFilename)
{
  g_sLastError.Reset();
  TiXmlDocument doc;
  TiXmlElement root("root");
  TiXmlNode *pRoot = doc.InsertEndChild(root);

  if (!DataExchangeXML(doc.RootElement(),true))
  {
    VASSERT(FALSE); // this should never happen!
    return false;
  }
  if (!doc.SaveFile(szFilename,Vision::File.GetManager()))
  {  
    g_sLastError = doc.ErrorDesc();
    return false; // error when saving
  }
  return true;
}


V_IMPLEMENT_SERIALX( VisParticleConstraintList_cl);
void VisParticleConstraintList_cl::SerializeX( VArchive &ar )
{
  int i;
  if (ar.IsLoading())
  {
    int iCount = 0;
    int iForceB = 0;
    ar >> iCount;
    for (i=0;i<iCount;i++)
    {
      VisParticleConstraint_cl *pConstr = (VisParticleConstraint_cl *)ar.ReadObject(NULL);
      ar >> iForceB;
      AddConstraint(pConstr,(VIS_CONSTRAINT_REFLECT_BEHAVIOR_e)iForceB);
    }
  } else
  {
    ar << m_iConstraintCount;
    for (i=0;i<m_iConstraintCount;i++)
    {
      ar.WriteObject(m_Constraint.GetDataPtr()[i]);
      ar << (int)m_ForceBehavior.GetDataPtr()[i];
    }
  }
}

////////////////////////////////////////////////////////////////////
// constraint helper classes
////////////////////////////////////////////////////////////////////

static inline bool HandleConstraintPlane_Min(Particle_t *p, char iPlaneComponent, float fPlaneVal, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eMode, float fFixedDistFactor, float fPersistance, float fFramePersistance)
{
  float fDist = p->pos[iPlaneComponent]-fPlaneVal;
  if (fDist>=p->size*0.5f) return false;

  // remove on touch plane
  if (eMode == CONSTRAINT_REFLECT_REMOVE)
    return true;

  // set to fixed distance
  p->pos[iPlaneComponent] = fPlaneVal + p->size*fFixedDistFactor;

  if (eMode == CONSTRAINT_REFLECT_BOUNCE)
  {
    if (p->velocity[iPlaneComponent]>0.f) return false;
    p->velocity[0] *= fPersistance;
    p->velocity[1] *= fPersistance;
    p->velocity[2] *= fPersistance;
    p->velocity[iPlaneComponent] = hkvMath::Abs (p->velocity[iPlaneComponent]);
  }
  else
  if (eMode == CONSTRAINT_REFLECT_GLIDE)
  {
    if (p->velocity[iPlaneComponent]>0.f) return false;
    hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
    float fOldLen = speed.getLength();
    // remove normal vector component
    speed[iPlaneComponent] = 0.f;
    // maintain same speed
    speed.setLength(fOldLen);
    p->velocity[0] = speed.x*fFramePersistance;
    p->velocity[1] = speed.y*fFramePersistance;
    p->velocity[2] = speed.z*fFramePersistance;
  }
  return false;
}


static inline bool HandleConstraintPlane_Max(Particle_t *p, char iPlaneComponent, float fPlaneVal, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eMode, float fFixedDistFactor, float fPersistance, float fFramePersistance)
{
  float fDist = fPlaneVal - p->pos[iPlaneComponent];
  if (fDist>=p->size*0.5f) return false;

  // remove on touch plane
  if (eMode == CONSTRAINT_REFLECT_REMOVE)
    return true;

  // set to fixed distance
  p->pos[iPlaneComponent] = fPlaneVal - p->size*fFixedDistFactor;

  if (eMode == CONSTRAINT_REFLECT_BOUNCE)
  {
    if (p->velocity[iPlaneComponent]<0.f) return false;
    p->velocity[0] *= fPersistance;
    p->velocity[1] *= fPersistance;
    p->velocity[2] *= fPersistance;
    p->velocity[iPlaneComponent] = -hkvMath::Abs (p->velocity[iPlaneComponent]);
  }
  else
  if (eMode == CONSTRAINT_REFLECT_GLIDE)
  {
    if (p->velocity[iPlaneComponent]<0.f) return false;
    hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
    float fOldLen = speed.getLength();
    // remove normal vector component
    speed[iPlaneComponent] = 0.f;
    // maintain same speed
    speed.setLength(fOldLen);
    p->velocity[0] = speed.x*fFramePersistance;
    p->velocity[1] = speed.y*fFramePersistance;
    p->velocity[2] = speed.z*fFramePersistance;
  }
  return false;
}


static inline bool HandleConstraintPlane(Particle_t *p, const hkvPlane& plane, const hkvVec3& vPlaneNrmlOrig, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eMode, float fFixedDistFactor, float fPersistance, float fFramePersistance, float fNoiseNormal, const VRandom& randGen)
{
  hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
  float fDist = plane.getDistanceTo(pos);
  if (fDist>=p->size*0.5f) return false;

  // remove on touch plane
  if (eMode == CONSTRAINT_REFLECT_REMOVE)
    return true;

  hkvVec3 vPlaneNrml(vPlaneNrmlOrig); 
  if (fNoiseNormal!=0.f)
  {
    vPlaneNrml.x += randGen.GetFloatNeg()*fNoiseNormal;
    vPlaneNrml.y += randGen.GetFloatNeg()*fNoiseNormal;
    vPlaneNrml.z += randGen.GetFloatNeg()*fNoiseNormal;
    vPlaneNrml.normalizeIfNotZero();
  }

  // set to fixed distance
  pos += vPlaneNrml*(p->size*fFixedDistFactor-fDist);
  p->pos[0] = pos.x;
  p->pos[1] = pos.y;
  p->pos[2] = pos.z;

  if (eMode == CONSTRAINT_REFLECT_BOUNCE)
  {
    hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
    if (speed.dot (vPlaneNrml) > 0.f) return false;
    speed = speed.getReflected(vPlaneNrml) * fPersistance;
    p->velocity[0] = speed.x;
    p->velocity[1] = speed.y;
    p->velocity[2] = speed.z;
  }
  else
  if (eMode == CONSTRAINT_REFLECT_GLIDE)
  {
    hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
    float fOldLen = speed.getLength();
    // remove normal vector component
    float fComp = speed.dot (vPlaneNrml);
    speed -= vPlaneNrml*fComp;
    // maintain same speed
    speed.setLength(fOldLen);
    p->velocity[0] = speed.x*fFramePersistance;
    p->velocity[1] = speed.y*fFramePersistance;
    p->velocity[2] = speed.z*fFramePersistance;
  }
  return false;
}


static inline bool ClampAABox(const hkvAlignedBBox &bbox, const hkvVec3& vBoxCenter, const hkvVec3& vInvBoxSize, hkvVec3& pos, hkvVec3& normal, float fRadius)
{
  VASSERT(bbox.isValid());
  normal = pos;
  normal.setMin(bbox.m_vMax);
  normal.setMax(bbox.m_vMin);
  normal = pos-normal;
  const float fNormalLen = normal.getLength();
  if (fNormalLen>fRadius) // outside bbox
    return false;

  if (hkvMath::isZero (fNormalLen)) // center inside box
  {
    hkvVec3 vDiff = pos-vBoxCenter;
    float relx = hkvMath::Abs (vDiff.x) * vInvBoxSize.x;
    float rely = hkvMath::Abs (vDiff.y) * vInvBoxSize.y;
    float relz = hkvMath::Abs (vDiff.z) * vInvBoxSize.z;

    // find the maximum component
    if (relx>rely && relx>relz)
    {
      if (vDiff.x>0.f)
      {
        normal.set(1.f,0.f,0.f);
        pos.x = bbox.m_vMax.x+fRadius;
      } else
      {
        normal.set(-1.f,0.f,0.f);
        pos.x = bbox.m_vMin.x-fRadius;
      }
    } 
    else if (rely>relz)
    {
      if (vDiff.y>0.f)
      {
        normal.set(0.f,1.f,0.f);
        pos.y = bbox.m_vMax.y+fRadius;
      } else
      {
        normal.set(0.f,-1.f,0.f);
        pos.y = bbox.m_vMin.y-fRadius;
      }
    }
    else  // z-maximum
    {
      if (vDiff.z>0.f)
      {
        normal.set(0.f,0.f,1.f);
        pos.z = bbox.m_vMax.z+fRadius;
      } else
      {
        normal.set(0.f,0.f,-1.f);
        pos.z = bbox.m_vMin.z-fRadius;
      }
    }
  } else // center outside, but sphere intersecting with box
  {
    normal *= (1.f/fNormalLen);
    pos += normal*(fRadius-fNormalLen);
  }

  return true;
}


static inline bool TestCapsuleCollision(
  Particle_t *p, 
  const hkvVec3& vPos, 
  const hkvVec3& vUnitDir, 
  float fLength, float fRadius, 
  //VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eMode, 
  //float fPersistance, float fFramePersistance,
  bool bComputeTouchPoint,
  hkvVec3& resPos, hkvVec3& resNormal)
{
  fRadius += p->size*0.5f;
  hkvVec3 vDiff(p->pos[0]-vPos.x, p->pos[1]-vPos.y, p->pos[2]-vPos.z);
  float fDot = vDiff.dot (vUnitDir);
  if (fDot<-fRadius || fDot>fLength+fRadius) // completely outside range
    return false;

  const float fRadiusSqr = fRadius*fRadius;
  const float fRadiusWithMargin = fRadius*1.05f;

  // inside the line
  if (fDot>=0.f && fDot<fLength)
  {
    hkvVec3 vRadius = vDiff - vUnitDir*fDot;
    if (vRadius.getLengthSquared () > fRadiusSqr)
      return false;

    if (bComputeTouchPoint)
    {
      resNormal = vRadius;
      resNormal.normalizeIfNotZero();
      resPos = vPos + vUnitDir*fDot + resNormal*fRadiusWithMargin;
    }

    return true;
  }

  // right half sphere
  if (fDot<=0.f)
  {
    if (vDiff.getLengthSquared () > fRadiusSqr)
      return false;
    if (bComputeTouchPoint)
    {
      resNormal = vDiff;
      resNormal.normalizeIfNotZero();
      resPos = vPos + resNormal*fRadiusWithMargin;
    }
    return true;
  }

  // left half sphere
  {
    hkvVec3 vEndPos = vPos+vUnitDir*fLength;
    hkvVec3 vDiffEnd(p->pos[0]-vEndPos.x, p->pos[1]-vEndPos.y, p->pos[2]-vEndPos.z);
    if (vDiffEnd.getLengthSquared () > fRadiusSqr)
      return false;
    if (bComputeTouchPoint)
    {
      resNormal = vDiffEnd;
      resNormal.normalizeIfNotZero();
      resPos = vEndPos + resNormal*fRadiusWithMargin;
    }
    return true;
  }
}


////////////////////////////////////////////////////////////////////
// point constraint
////////////////////////////////////////////////////////////////////

bool VisParticleConstraintPoint_cl::Influences(const hkvAlignedBBox &bbox)
{
  return bbox.contains(GetPosition())==TRUE;
}

void VisParticleConstraintPoint_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  pRenderer->RenderSphere((hkvVec3& )GetPosition(),m_fRadius,m_iDebugColor,state);

}

bool VisParticleConstraintPoint_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;

  hkvVec3 vPos = GetPosition();

  XMLHelper::Exchange_Floats(pNode,"position",vPos.data,3,bWrite);
  XMLHelper::Exchange_Float(pNode,"radius",m_fRadius,bWrite);

  if ( !bWrite )
    SetPosition( vPos );

  return true;
}



void VisParticleConstraintPoint_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  // Don't do anything to particles
}

V_IMPLEMENT_SERIAL( VisParticleConstraintPoint_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintPoint_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> m_fRadius;
  } else
  {
    ar << m_fRadius;
  }
}

////////////////////////////////////////////////////////////////////
// ground plane constraint
////////////////////////////////////////////////////////////////////


bool VisParticleConstraintGroundPlane_cl::Influences(const hkvAlignedBBox &bbox)
{
  return (bbox.m_vMin.z<=GetPosition().z && bbox.m_vMax.z>=GetPosition().z);
}

void VisParticleConstraintGroundPlane_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  hkvPlane plane; plane.setFromPointAndNormal (hkvVec3(0.f , 0.f, GetPosition().z), hkvVec3(0.f,0.f,1.f)); // pos and normal
  float fSize = 3000.f * Vision::World.GetGlobalUnitScaling();
  pRenderer->RenderPlane(plane,hkvVec3(vHelperPos.x,vHelperPos.y,GetPosition().z),fSize,m_iDebugColor,state);
}

bool VisParticleConstraintGroundPlane_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  
  float fHeight = GetPosition().z;
  XMLHelper::Exchange_Float(pNode,"height",fHeight,bWrite);

  if  ( !bWrite )
    SetPosition( GetPosition().x, GetPosition().y, fHeight );

  return true;
}

#define NEXT_PARTICLE p=(Particle_t *)(((char *)p)+iStride)

void VisParticleConstraintGroundPlane_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();

  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  int i;
  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;
  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    if (HandleConstraintPlane_Min(p,2,GetPosition().z,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance))
      pGroup->DestroyParticle(p,fTimeDelta);
  }
}

V_IMPLEMENT_SERIAL( VisParticleConstraintGroundPlane_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintGroundPlane_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);
}

void VisParticleConstraintGroundPlane_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleConstraint_cl::OnObject3DChanged(iO3DFlags);

  // remove only the "change" flags
  ClearO3DChangeFlags();
}


////////////////////////////////////////////////////////////////////
// generic plane constraint
////////////////////////////////////////////////////////////////////



bool VisParticleConstraintPlane_cl::Influences(const hkvAlignedBBox &bbox)
{
  hkvPlanePosition::Enum res = m_Plane.getObjectPosition (bbox);
  return (res == hkvPlanePosition::Coplanar) || (res == hkvPlanePosition::Spanning);
}


void VisParticleConstraintPlane_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  hkvPlane plane (m_Plane);

  // ensure the helper position is on the plane
  hkvVec3 vOnPlane = vHelperPos;
  m_Plane.projectOntoPlane(vOnPlane);

  hkvVec3 vDirX = GetObjDir();
  hkvVec3 vDirY = GetObjDir_Right();
  int iSubDiv = 5;
  if (m_bInfinite)
  {
    float fLen = 3000.f * Vision::World.GetGlobalUnitScaling();
    vDirX.setLength(fLen);
    vDirY.setLength(fLen);
  } else
  {
    iSubDiv = 1;
    vDirX.setLength(m_fSizeX);
    vDirY.setLength(m_fSizeY);
  }

  pRenderer->RenderPlane (vDirX, vDirY, vOnPlane,m_iDebugColor,state,RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID,iSubDiv);
}


bool VisParticleConstraintPlane_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  if (bWrite)
  {
    pNode->SetAttribute("plane",XMLHelper::FloatsToString(&m_Plane.m_vNormal.x,4));
  } else
  {
    XMLHelper::StringToFloats(pNode->Attribute("plane"),&m_Plane.m_vNormal.x,4);
  }

  return true;
}


void VisParticleConstraintPlane_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  const VRandom& randGen = pGroup->GetRandom();

  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  hkvVec3 vPlaneNrml = m_Plane.m_vNormal;

  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;

  int i;
  if (m_bInfinite)
  {
    for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      if (HandleConstraintPlane(p,m_Plane,vPlaneNrml,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance, m_fReflectionNoise, randGen))
        pGroup->DestroyParticle(p,fTimeDelta);
    }
  } else
  {
    hkvVec3 vDirX = GetObjDir();
    hkvVec3 vDirY = GetObjDir_Right();
    hkvVec3 vPos = GetPosition();
    ANALYSIS_IGNORE_WARNING_ONCE(6246)
    hkvVec3 vPlaneNrml = GetObjDir_Up();
    for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      hkvVec3 vDiff(p->pos[0]-vPos.x, p->pos[1]-vPos.y, p->pos[2]-vPos.z);
      float fDotX = vDiff.dot (vDirX);

      float fRadius = p->size*0.5f;
      // outside plane range?
      if (hkvMath::Abs (fDotX)>m_fSizeX+fRadius)
        continue;
      float fDotY = vDiff.dot (vDirY);
      if (hkvMath::Abs (fDotY)>m_fSizeY+fRadius)
        continue;

      // too far behind plane?
      float fPlaneDist = vDiff.dot (vPlaneNrml);
      if (fPlaneDist< -(fRadius+m_fThickness))
        continue;

      // otherwise default plane handling
      if (HandleConstraintPlane(p,m_Plane,vPlaneNrml,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance, m_fReflectionNoise, randGen))
        pGroup->DestroyParticle(p,fTimeDelta);
    }
  }
}

V_IMPLEMENT_SERIAL( VisParticleConstraintPlane_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintPlane_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);
 
  if (ar.IsLoading())
  {
    m_Plane.SerializeAs_VisPlane (ar);
    if (ar.GetLoadingVersion()>=VISION_ARCHIVE_VERSION_022) // Vision 7.0
    {
      ar >> m_bInfinite >> m_fSizeX >> m_fSizeY;
      if (m_iLocalLoadingVersion>=3)
        ar >> m_fThickness;
    }
  } else
  {
    m_Plane.SerializeAs_VisPlane (ar);
    ar << m_bInfinite << m_fSizeX << m_fSizeY; // Vision 7.0
    ar << m_fThickness; // local version 3
  }
}

void VisParticleConstraintPlane_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleConstraint_cl::OnObject3DChanged(iO3DFlags);

  // recalculate plane equation
  hkvVec3 vNormal(0,0,1.f);
  vNormal = GetRotationMatrix() * vNormal;
  vNormal.normalizeIfNotZero();
  m_Plane.setFromPointAndNormal(GetPosition(), vNormal);

  // remove only the "change" flags
  ClearO3DChangeFlags();
}

////////////////////////////////////////////////////////////////////
// sphere constraint
////////////////////////////////////////////////////////////////////

bool VisParticleConstraintSphere_cl::Influences(const hkvAlignedBBox &bbox)
{
  hkvBoundingSphere sphere;
  hkvVec3 vBoxCenter = bbox.getCenter();
  hkvVec3 vConstraintCenter = GetPosition();

  // for cylinders, move test center to box center
  sphere.m_vCenter[0] = (m_eAxis == AXIS_X) ? vBoxCenter.x : vConstraintCenter.x;
  sphere.m_vCenter[1] = (m_eAxis == AXIS_Y) ? vBoxCenter.y : vConstraintCenter.y;
  sphere.m_vCenter[2] = (m_eAxis == AXIS_Z) ? vBoxCenter.z : vConstraintCenter.z;
  sphere.m_fRadius = m_fRadius;
  return bbox.overlaps(sphere)==TRUE;
}

void VisParticleConstraintSphere_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  int iFace = m_bInside ? RENDERSTATEFLAG_BACKFACE : RENDERSTATEFLAG_FRONTFACE;
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, iFace);
  pRenderer->RenderSphere((hkvVec3& )GetPosition(),m_fRadius,m_iDebugColor,state);
}



bool VisParticleConstraintSphere_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;

  hkvVec3 vTempPos = GetPosition();

  XMLHelper::Exchange_Floats(pNode,"center",vTempPos.data,3,bWrite);
  XMLHelper::Exchange_Float(pNode,"radius",m_fRadius,bWrite);

  if ( !bWrite )
    SetPosition( vTempPos );

  return true;
}


void VisParticleConstraintSphere_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  hkvVec3 vPlaneNrml, vDiff;

  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;
  float fDist;

  int i;
  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
    if (m_bInside)
    {
      vDiff = GetPosition()-pos;
      if (m_eAxis != AXIS_NONE) // for cylinders
        vDiff[m_eAxis] = 0.f;
      fDist = m_fRadius - vDiff.getLength();
    } else
    {
      vDiff = (hkvVec3) pos-GetPosition();
      if (m_eAxis != AXIS_NONE) // for cylinders
        vDiff[m_eAxis] = 0.f;
      fDist = vDiff.getLength() - m_fRadius;
    }
    if (fDist>=p->size*0.5f) continue;

    // remove on touch plane
    if (eForceBehavior == CONSTRAINT_REFLECT_REMOVE)
    {
      pGroup->DestroyParticle(p,fTimeDelta);
      continue;
    }

    vPlaneNrml = vDiff;
    vPlaneNrml.normalizeIfNotZero();

    // set to fixed distance
    pos += vPlaneNrml*(p->size*fFixedDistFactor-fDist);
    p->pos[0] = pos.x;
    p->pos[1] = pos.y;
    p->pos[2] = pos.z;

    if (eForceBehavior == CONSTRAINT_REFLECT_BOUNCE)
    {
      hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
      if (speed.dot (vPlaneNrml) > 0.f) continue;
      speed = speed.getReflected(vPlaneNrml) * m_fPersistance;
      p->velocity[0] = speed.x;
      p->velocity[1] = speed.y;
      p->velocity[2] = speed.z;
    }
    else
    if (eForceBehavior == CONSTRAINT_REFLECT_GLIDE)
    {
      hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
      float fOldLen = speed.getLength();
      // remove normal vector component
      float fComp = speed.dot (vPlaneNrml);
      speed -= vPlaneNrml*fComp;
      // maintain same speed
      speed.setLength(fOldLen);
      p->velocity[0] = speed.x*fFramePersistance;
      p->velocity[1] = speed.y*fFramePersistance;
      p->velocity[2] = speed.z*fFramePersistance;
    }
  }
}

V_IMPLEMENT_SERIAL( VisParticleConstraintSphere_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintSphere_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> m_fRadius;
    ar >> m_bInside;
    ar >> (int &)m_eAxis;
  } else
  {
    ar << m_fRadius;
    ar << m_bInside;
    ar << (int)m_eAxis;
  }
}


////////////////////////////////////////////////////////////////////
// Infinite cylinder
////////////////////////////////////////////////////////////////////

bool VisParticleConstraintInfCylinder_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  VASSERT(sizeof(m_eAxis)==sizeof(int));
  XMLHelper::Exchange_Int(pNode,"axis",(int &)m_eAxis,bWrite);
  return true;
}


void VisParticleConstraintInfCylinder_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  int iFace = m_bInside ? RENDERSTATEFLAG_BACKFACE : RENDERSTATEFLAG_FRONTFACE;
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, iFace);
  hkvVec3 vDir = GetDirection() * 1000.f * Vision::World.GetGlobalUnitScaling();
  int iFlags = RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID;

  hkvVec3 vPosMinusDir = GetPosition()-vDir;
  hkvVec3 vDirTimesTwo = vDir * 2.0f;
  pRenderer->RenderCylinder((hkvVec3& )vPosMinusDir,(hkvVec3& )vDirTimesTwo, m_fRadius, m_iDebugColor,state, iFlags);
}

V_IMPLEMENT_SERIAL( VisParticleConstraintInfCylinder_cl, VisParticleConstraintSphere_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintInfCylinder_cl::Serialize( VArchive &ar )
{
  VisParticleConstraintSphere_cl::Serialize(ar);
}

////////////////////////////////////////////////////////////////////
// Axis aligned box constraint
////////////////////////////////////////////////////////////////////


bool VisParticleConstraintAABox_cl::Influences(const hkvAlignedBBox &bbox)
{
  return bbox.overlaps(m_Box)==TRUE;
}

void VisParticleConstraintAABox_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
//  Vision::Game.DrawBoundingBox(m_Box);
  int iFace = m_bInside ? RENDERSTATEFLAG_BACKFACE : RENDERSTATEFLAG_FRONTFACE;
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, iFace);

  hkvAlignedBBox bbox = m_Box;
  pRenderer->RenderAABox(bbox,m_iDebugColor,state);
  
}


bool VisParticleConstraintAABox_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  XMLHelper::Exchange_Floats(pNode,"boxmin",m_Box.m_vMin.data,3,bWrite);
  XMLHelper::Exchange_Floats(pNode,"boxmax",m_Box.m_vMax.data,3,bWrite);
  return true;
}


void VisParticleConstraintAABox_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  int i;
  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;
  if (m_bInside)
  {
    for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      if (HandleConstraintPlane_Min(p,0,m_Box.m_vMin.x, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      if (HandleConstraintPlane_Min(p,1,m_Box.m_vMin.y, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      if (HandleConstraintPlane_Min(p,2,m_Box.m_vMin.z, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      if (HandleConstraintPlane_Max(p,0,m_Box.m_vMax.x, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      if (HandleConstraintPlane_Max(p,1,m_Box.m_vMax.y, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      if (HandleConstraintPlane_Max(p,2,m_Box.m_vMax.z, eForceBehavior, fFixedDistFactor,m_fPersistance,fFramePersistance))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
    }
  } else
  {
    hkvVec3 vPlaneNrml;
    hkvVec3 vBoxCenter = m_Box.getCenter();
    hkvVec3 vInvBoxSize(1.f/m_Box.getSizeX(),1.f/m_Box.getSizeY(),1.f/m_Box.getSizeZ());
    for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
      if (!ClampAABox(m_Box,vBoxCenter,vInvBoxSize, pos,vPlaneNrml,p->size*0.5f))
        continue;

      // remove on touch plane
      if (eForceBehavior == CONSTRAINT_REFLECT_REMOVE)
      {
        pGroup->DestroyParticle(p,fTimeDelta);
        continue;
      }

      p->pos[0] = pos.x;
      p->pos[1] = pos.y;
      p->pos[2] = pos.z;

      if (eForceBehavior == CONSTRAINT_REFLECT_BOUNCE)
      {
        hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
        if (speed.dot (vPlaneNrml) > 0.f) continue;
        speed = speed.getReflected(vPlaneNrml) * m_fPersistance;
        p->velocity[0] = speed.x;
        p->velocity[1] = speed.y;
        p->velocity[2] = speed.z;
      }
      else
      if (eForceBehavior == CONSTRAINT_REFLECT_GLIDE)
      {
        hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
        float fOldLen = speed.getLength();
        // remove normal vector component
        float fComp = speed.dot (vPlaneNrml);
        speed -= vPlaneNrml*fComp;
        // maintain same speed
        speed.setLength(fOldLen);
        p->velocity[0] = speed.x*fFramePersistance;
        p->velocity[1] = speed.y*fFramePersistance;
        p->velocity[2] = speed.z*fFramePersistance;
      }

    }
  }
}

V_IMPLEMENT_SERIAL( VisParticleConstraintAABox_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintAABox_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    m_Box.SerializeAs_VisBoundingBox (ar);
    ar >> m_bInside;
  } 
  else
  {
    m_Box.SerializeAs_VisBoundingBox (ar);
    ar << m_bInside;
  }
}

void VisParticleConstraintAABox_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleConstraint_cl::OnObject3DChanged(iO3DFlags);

  // recalculate box
  hkvVec3 vRadius(m_Box.getSizeX()*0.5f,m_Box.getSizeY()*0.5f,m_Box.getSizeZ()*0.5f);
  const hkvVec3 vPosition = GetPosition();
  m_Box.m_vMin = vPosition - vRadius;
  m_Box.m_vMax = vPosition + vRadius;

  // remove only the "change" flags
  ClearO3DChangeFlags();
}



////////////////////////////////////////////////////////////////////
// Oriented box constraint
////////////////////////////////////////////////////////////////////

bool VisParticleConstraintOBox_cl::Influences(const hkvAlignedBBox &bbox)
{
  const hkvVec3& vCenter = GetPosition();
  const hkvMat3 &mRot = GetRotationMatrix();
  hkvAlignedBBox boxOverOBox;
  boxOverOBox.setInvalid ();
  hkvAlignedBBox temp = m_Box;
  temp.transformFromOrigin (hkvMat4 (mRot, hkvVec3::ZeroVector ()));
  boxOverOBox.expandToInclude (temp);
  boxOverOBox.translate (vCenter);
  
  return bbox.overlaps(boxOverOBox)==TRUE;
}

void VisParticleConstraintOBox_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  const hkvVec3& vCenter = GetPosition();
  const hkvMat3 &mRot = GetRotationMatrix();
  Vision::Game.DrawOrientedBoundingBox(m_Box, mRot, vCenter, m_iDebugColor);
}


bool VisParticleConstraintOBox_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  hkvVec3 vPos = this->GetPosition();
  hkvMat3 mRot = this->GetRotationMatrix();
  XMLHelper::Exchange_Floats(pNode,"boxmin",m_Box.m_vMin.data,3,bWrite);
  XMLHelper::Exchange_Floats(pNode,"boxmax",m_Box.m_vMax.data,3,bWrite);
  XMLHelper::Exchange_Floats(pNode,"position",vPos.data,3,bWrite);

  {
    float f[9];
    mRot.get (f, hkvMat3::RowMajor);

    XMLHelper::Exchange_Floats(pNode,"rotation", f,9,bWrite);

    mRot.set (f, hkvMat3::RowMajor);
  }

  if (!bWrite) {
    SetRotationMatrix(mRot);
    SetPosition(vPos);
  }
  return true;
}


void VisParticleConstraintOBox_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  const VRandom& randGen = pGroup->GetRandom();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;
  
  int i;
  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;
  const hkvVec3& vCenter = GetPosition();
  const hkvMat3 &mRot = GetRotationMatrix();
  hkvVec3 pvCorners[8];
  m_Box.getCorners(pvCorners);
  for (i=0; i<8; i++) {
    pvCorners[i] = mRot * pvCorners[i] + vCenter;
  }

  hkvPlane planeXMin; planeXMin.setFromPoints (pvCorners[0], pvCorners[1], pvCorners[2], hkvTriangleOrientation::ClockWise); const hkvVec3& normalXMin = planeXMin.m_vNormal;
  hkvPlane planeXMax; planeXMax.setFromPoints (pvCorners[4], pvCorners[6], pvCorners[5], hkvTriangleOrientation::ClockWise); const hkvVec3& normalXMax = planeXMax.m_vNormal;
  hkvPlane planeYMin; planeYMin.setFromPoints (pvCorners[0], pvCorners[4], pvCorners[1], hkvTriangleOrientation::ClockWise); const hkvVec3& normalYMin = planeYMin.m_vNormal;
  hkvPlane planeYMax; planeYMax.setFromPoints (pvCorners[2], pvCorners[7], pvCorners[6], hkvTriangleOrientation::ClockWise); const hkvVec3& normalYMax = planeYMax.m_vNormal;
  hkvPlane planeZMin; planeZMin.setFromPoints (pvCorners[2], pvCorners[6], pvCorners[4], hkvTriangleOrientation::ClockWise); const hkvVec3& normalZMin = planeZMin.m_vNormal;
  hkvPlane planeZMax; planeZMax.setFromPoints (pvCorners[1], pvCorners[7], pvCorners[3], hkvTriangleOrientation::ClockWise); const hkvVec3& normalZMax = planeZMax.m_vNormal;

  if (m_bInside)
  {
    for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      // x plane min
      if (HandleConstraintPlane(p,planeXMin,normalXMin,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      // x plane max
      if (HandleConstraintPlane(p,planeXMax,normalXMax,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      // y plane min
      if (HandleConstraintPlane(p,planeYMin,normalYMin,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      // y plane max
      if (HandleConstraintPlane(p,planeYMax,normalYMax,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      // z plane min
      if (HandleConstraintPlane(p,planeZMin,normalZMin,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
      // z plane max
      if (HandleConstraintPlane(p,planeZMax,normalZMax,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance,0.f,randGen))
        {pGroup->DestroyParticle(p,fTimeDelta);continue;}
    }
  } else
  {
    //TODO: handle outside obox constraint
  }
}

V_IMPLEMENT_SERIAL( VisParticleConstraintOBox_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintOBox_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    m_Box.SerializeAs_VisBoundingBox (ar);
    ar >> m_bInside;
  } else
  {
    m_Box.SerializeAs_VisBoundingBox (ar);
    ar << m_bInside;
  }
}

void VisParticleConstraintOBox_cl::OnObject3DChanged(int iO3DFlags)
{
  VisParticleConstraint_cl::OnObject3DChanged(iO3DFlags);

  // remove only the "change" flags
  ClearO3DChangeFlags();
}



////////////////////////////////////////////////////////////////////
// camera box constraint
////////////////////////////////////////////////////////////////////

void VisParticleConstraintCamBox_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  hkvAlignedBBox bbox;
  GetAbsoluteBBox(bbox);
  VASSERT(bbox.isValid());
  const float dx = bbox.getSizeX();
  const float dy = bbox.getSizeY();
  const float dz = bbox.getSizeZ();

  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  int i;
  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    while (p->pos[0]<bbox.m_vMin.x) p->pos[0]+=dx;
    while (p->pos[0]>bbox.m_vMax.x) p->pos[0]-=dx;
    while (p->pos[1]<bbox.m_vMin.y) p->pos[1]+=dy;
    while (p->pos[1]>bbox.m_vMax.y) p->pos[1]-=dy;
    while (p->pos[2]<bbox.m_vMin.z) p->pos[2]+=dz;
    while (p->pos[2]>bbox.m_vMax.z) p->pos[2]-=dz;
  }
}

bool VisParticleConstraintCamBox_cl::Influences(const hkvAlignedBBox &bbox)
{
  return true; // TODO (?)
}

void VisParticleConstraintCamBox_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  hkvAlignedBBox bbox;
  GetAbsoluteBBox(bbox);

  // draw a simple line box around the camera
  Vision::Game.DrawBoundingBox(bbox, m_iDebugColor);

  // ..and a filled shape box
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);

  hkvVec3 vPos = GetPosition();
  hkvAlignedBBox bbox3(hkvVec3 (vPos.x-m_vBoxDim.x,vPos.y-m_vBoxDim.y,vPos.z-m_vBoxDim.z), hkvVec3 (vPos.x+m_vBoxDim.x,vPos.y+m_vBoxDim.y,vPos.z+m_vBoxDim.z));
  pRenderer->RenderAABox(bbox3,m_iDebugColor,state);

}

bool VisParticleConstraintCamBox_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
  XMLHelper::Exchange_Floats(pNode,"boxdim",m_vBoxDim.data,3,bWrite);
  return true;
}


V_IMPLEMENT_SERIAL( VisParticleConstraintCamBox_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintCamBox_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  m_vBoxDim.SerializeAsVisVector (ar);
}

////////////////////////////////////////////////////////////////////
// fan affector
////////////////////////////////////////////////////////////////////

bool VisParticleAffectorFan_cl::Influences(const hkvAlignedBBox &bbox)
{
  return true; // TODO
}

void VisParticleAffectorFan_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  hkvVec3 vPos = GetPosition();
  hkvVec3 vDir = GetDirection();
      
  float fLen = m_fIntensity * Vision::World.GetGlobalUnitScaling();
  pRenderer->RenderCone((hkvVec3& )vPos, (hkvVec3& )vDir,m_fConeAngle,fLen,m_iDebugColor,state);
}

bool VisParticleAffectorFan_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;

  VASSERT(FALSE); // TODO
  return true;
}


void VisParticleAffectorFan_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;
  int i;
  if (m_fConeAngle<1.f || m_fIntensity<1.f) return;
  

  float fRandomFactor = 1.f;
  if (m_fRandomness>0.f) // apply some time based noise
  {
    float fTime = Vision::GetTimer()->GetTime();
    float fNoise = 0.6f*hkvMath::sinRad (fTime*0.32f) + 0.3f*hkvMath::cosRad (fTime*1.075364f) + 0.1f*hkvMath::sinRad (fTime*2.4229f);
    fRandomFactor = 1.f - m_fRandomness*fNoise;
  }

  const float fInt0 = m_fIntensity*100.f*fRandomFactor;
  const float fCosAngle = hkvMath::cosDeg (m_fConeAngle / 2.0f);
  const float fMult = fInt0/(1.f-fCosAngle);

  hkvVec3 vDir = GetDirection();
  hkvVec3 vPos = GetPosition();

  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
    hkvVec3 vDiff = pos-vPos;
    float fDist = vDiff.getLength();
    if (fDist<0.1f) continue;
    vDiff.normalizeIfNotZero();
    float fDotProd = vDiff.dot (vDir);
    if (fDotProd<fCosAngle) continue;
    float fIntensity = fMult/(fDist+10.f) * fTimeDelta * (fDotProd-fCosAngle);
    p->velocity[0] += fIntensity*vDiff.x;
    p->velocity[1] += fIntensity*vDiff.y;
    p->velocity[2] += fIntensity*vDiff.z;
  }
}

V_IMPLEMENT_SERIAL( VisParticleAffectorFan_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleAffectorFan_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> m_fRandomness;
    ar >> m_fConeAngle;
    ar >> m_fIntensity;
  } else
  {
    ar << m_fRandomness;
    ar << m_fConeAngle;
    ar << m_fIntensity;
  }
}


////////////////////////////////////////////////////////////////////
// cyclone affector
////////////////////////////////////////////////////////////////////

bool VisParticleAffectorCyclone_cl::Influences(const hkvAlignedBBox &bbox)
{
  hkvAlignedBBox thisbox;
  // create cyclone bounding box
  thisbox.expandToInclude(GetPosition());
  thisbox.expandToInclude(GetPosition()+GetDirection()*m_fAxisLen);
  thisbox.addBoundary(hkvVec3 (m_fRadius));

  return thisbox.overlaps(bbox)==TRUE;
}

void VisParticleAffectorCyclone_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  
  hkvVec3 vDir = GetDirection()*m_fAxisLen;
  pRenderer->RenderCylinder((hkvVec3& )GetPosition(),(hkvVec3& )vDir,m_fRadius,m_iDebugColor,state);
}

bool VisParticleAffectorCyclone_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;

  VASSERT(FALSE); // TODO
  return true;
}


static inline float BellCurve(float fIn)
{
  if (hkvMath::Abs (fIn)>=1.f) return 0.f;
  return 0.5f + 0.5f*hkvMath::cosRad (fIn * hkvMath::pi ());
}


void VisParticleAffectorCyclone_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  if (m_fIntensity<1.f || m_fRadius<=HKVMATH_LARGE_EPSILON) return;
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  float fInfRad = 1.f/m_fRadius;
  float fOuterRad = 6.f*m_fRadius; // radius for gravity falloff
  float fInfOutRad = 1.f/fOuterRad;

  hkvVec3 vPos = GetPosition();
  hkvVec3 vDir = GetDirection();

  int i;
//  bool bFirst = true;
  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    if (p->size<=HKVMATH_LARGE_EPSILON) continue;
    hkvVec3 pos(p->pos[0],p->pos[1],p->pos[2]);
    hkvVec3 vDiff = pos-vPos;

    // outside axis?
    float fDot = vDiff.dot (vDir);
    if (fDot<0.f || fDot>m_fAxisLen)
      continue;

    hkvVec3 vTangent = vDiff.cross(vDir);

    // distance to axis
    float fAxisDist = vTangent.getLength();
    float fRadDist = fAxisDist-m_fRadius;
    float fFalloff = BellCurve(fRadDist*fInfRad); // takes maximum at radius
    float fFalloff2 = BellCurve(fRadDist*fInfOutRad);
    float fSign = (fAxisDist>m_fRadius) ? fFalloff2 : -1.f;
    hkvVec3 vRadius = vTangent.cross(vDir);
    vRadius.setLength(m_fIntensity*0.2f);

    vTangent.setLength(fFalloff*m_fIntensity);
    if (fRadDist>=p->size) // inside a particle use less force
      vTangent += vRadius*fSign;
    else
      vTangent += vRadius*(fSign*fRadDist/p->size);


    // preserve old vertical speed
    hkvVec3 vSpeed(p->velocity[0],p->velocity[1],p->velocity[2]);
    float fVSpeed = vSpeed.dot (vDir);
    vTangent += vDir*fVSpeed;

    p->velocity[0] = vTangent.x;
    p->velocity[1] = vTangent.y;
    p->velocity[2] = vTangent.z;
  }
}

V_IMPLEMENT_SERIAL( VisParticleAffectorCyclone_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleAffectorCyclone_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> m_fAxisLen;
    ar >> m_fRadius;
    ar >> m_fIntensity;
  } else
  {
    ar << m_fAxisLen;
    ar << m_fRadius;
    ar << m_fIntensity;
  }
}

////////////////////////////////////////////////////////////////////
// gravity point affector
////////////////////////////////////////////////////////////////////

bool VisParticleAffectorGravityPoint_cl::Influences(const hkvAlignedBBox &bbox)
{
  return true; // infinite range
}

void VisParticleAffectorGravityPoint_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  pRenderer->RenderSphere((hkvVec3& )GetPosition(),m_fRadius,m_iDebugColor,state);
}

bool VisParticleAffectorGravityPoint_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;

  VASSERT(FALSE); // TODO
  return true;
}

void VisParticleAffectorGravityPoint_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  VASSERT(pGroup);
  if (m_fIntensity<=HKVMATH_LARGE_EPSILON) return;
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  const hkvVec3 vPos = GetPosition();
  const float fRadSqr = m_fRadius*m_fRadius;
  const float fMulOutside = fTimeDelta*m_fIntensity*fRadSqr;
  const float fMulInside = fTimeDelta*m_fIntensity/m_fRadius;

  int i;
  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    hkvVec3 vDiff = vPos - ((hkvVec3&) p->pos);
    const float fDistSqr = vDiff.getLengthSquared ();
    float fAtt;
    if (fDistSqr<fRadSqr) // linear attenuation inside sphere, 1/r^2 outside
      fAtt = hkvMath::sqrt (fDistSqr)*fMulInside;
    else
      fAtt = fMulOutside/fDistSqr;

    vDiff.setLength(fAtt);
    p->velocity[0] += vDiff.x;
    p->velocity[1] += vDiff.y;
    p->velocity[2] += vDiff.z;
  }
}

V_IMPLEMENT_SERIAL( VisParticleAffectorGravityPoint_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleAffectorGravityPoint_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> m_fRadius;
    ar >> m_fIntensity;
  } else
  {
    ar << m_fRadius;
    ar << m_fIntensity;
  }
}


////////////////////////////////////////////////////////////////////
// bone list
////////////////////////////////////////////////////////////////////


VisParticleConstraintBones_cl::VisParticleConstraintBones_cl(VisBaseEntity_cl *pEntity, int iBoneCount, const int *pBoneIndices, float fBoneRadiusScale)
  : VisParticleConstraint_cl(CONSTRAINT_REFLECT_NOTHING,0.f)
{
  Vision::Callbacks.OnUpdateSceneFinished += this;
  m_iBoneCount = 0;
  m_pBoneInfo = NULL;
  m_pEntity = pEntity;
  m_bValid = false;
  if (!m_pEntity || !m_pEntity->GetMesh() || !m_pEntity->GetMesh()->GetSkeleton())
    return;

  if (iBoneCount>0)
  {
    m_pBoneInfo = new BoneInfo_t[iBoneCount];
    for (int i=0;i<iBoneCount;i++)
    {
      if (pBoneIndices[i]<0)
        continue;

      BoneInfo_t &info = m_pBoneInfo[m_iBoneCount++];
      VisSkeletalBone_cl* pBone = m_pEntity->GetMesh()->GetSkeleton()->GetBone(pBoneIndices[i]);
      const hkvAlignedBBox &bbox = pBone->m_BoundingBox;
      float fRadius = info.fRadius = hkvMath::Max(bbox.getSizeY(),bbox.getSizeZ()) * 0.5f * fBoneRadiusScale;
      info.iIndex = pBoneIndices[i];
      info.fRangeStart = bbox.m_vMin.x + fRadius; // the radius should be included in the box extent
      info.fBoneLength = bbox.m_vMax.x - bbox.m_vMin.x - fRadius*2.f;
    }
  }
  m_bValid = m_iBoneCount>0;
}

VisParticleConstraintBones_cl::~VisParticleConstraintBones_cl()
{
  V_SAFE_DELETE_ARRAY(m_pBoneInfo);
  Vision::Callbacks.OnUpdateSceneFinished -= this;
}



V_IMPLEMENT_SERIAL( VisParticleConstraintBones_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintBones_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);
}


void VisParticleConstraintBones_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VASSERT(pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished);

  hkvVec3 translation(hkvNoInitialization);
  hkvQuat rotation(hkvNoInitialization);
  hkvVec3 vBoneStart;
  hkvVec3 vBoneDir(1.f,0,0);
  BoneInfo_t *pBoneInfo = m_pBoneInfo;

  // update the absolute bone positions here, as this operation is not thread-safe
  for (int iBone=0;iBone<m_iBoneCount;iBone++,pBoneInfo++)
  {
    m_pEntity->GetBoneCurrentWorldSpaceTransformation(pBoneInfo->iIndex,translation,rotation);
    vBoneStart.x = pBoneInfo->fRangeStart;
    pBoneInfo->m_vWorldStart = rotation.transform(vBoneStart);
    pBoneInfo->m_vWorldDir = rotation.transform(vBoneDir);
    pBoneInfo->m_vWorldStart += translation;
  }

}

void VisParticleConstraintBones_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  if (!m_bValid)
    return;

  VASSERT(pGroup);
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();

  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  // eForceBehavior = CONSTRAINT_REFLECT_BOUNCE; m_fPersistance = 0.5f; // REMOVE

  const BoneInfo_t *pBoneInfo = m_pBoneInfo;
  hkvVec3 resPos(hkvNoInitialization), resNormal(hkvNoInitialization);
  const bool bComputeTouchPoint = eForceBehavior!=CONSTRAINT_REFLECT_NOTHING;

  for (int iBone=0;iBone<m_iBoneCount;iBone++,pBoneInfo++)
  {
    Particle_t *p = pGroup->GetPhysicsParticleArray();
    for (int i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
    {
      if (!TestCapsuleCollision(p, pBoneInfo->m_vWorldStart, pBoneInfo->m_vWorldDir, pBoneInfo->fBoneLength, pBoneInfo->fRadius, bComputeTouchPoint, resPos, resNormal))
        continue;

      // remove on touch bone
      if (eForceBehavior == CONSTRAINT_REFLECT_REMOVE)
      {
        p->pos[0] = resPos.x; 
        p->pos[1] = resPos.y;
        p->pos[2] = resPos.z;
        pGroup->DestroyParticle(p,fTimeDelta);
        continue;
      }
      if (eForceBehavior == CONSTRAINT_REFLECT_BOUNCE)
      {
        p->pos[0] = resPos.x; 
        p->pos[1] = resPos.y;
        p->pos[2] = resPos.z;
        hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
        speed = speed.getReflected(resNormal) * m_fPersistance;
        p->velocity[0] = speed.x;
        p->velocity[1] = speed.y;
        p->velocity[2] = speed.z;
        continue;
      }

      if (eForceBehavior == CONSTRAINT_REFLECT_GLIDE)
      {
        p->pos[0] = resPos.x; 
        p->pos[1] = resPos.y;
        p->pos[2] = resPos.z;
        hkvVec3 speed(p->velocity[0],p->velocity[1],p->velocity[2]);
        float fOldLen = speed.getLength();
        // remove normal vector component
        float fComp = speed.dot (resNormal);
        speed -= resNormal*fComp;
        // maintain same speed
        speed.setLength(fOldLen);
        p->velocity[0] = speed.x*fFramePersistance;
        p->velocity[1] = speed.y*fFramePersistance;
        p->velocity[2] = speed.z*fFramePersistance;
      }

    }
  }
}

bool VisParticleConstraintBones_cl::Influences(const hkvAlignedBBox &bbox)
{
  return true; // TODO
}


void VisParticleConstraintBones_cl::BoneInfo_t::DebugRender(IVRenderInterface *pRenderer, VisBaseEntity_cl *pEntity) const
{
  VASSERT(pEntity);

  hkvVec3 translation(hkvNoInitialization);
  hkvQuat rotation(hkvNoInitialization);
  hkvVec3 vBoneStart;
  hkvVec3 vBoneDir(1.f,0,0);
  pEntity->GetBoneCurrentWorldSpaceTransformation(iIndex,translation,rotation);
  vBoneStart.x = fRangeStart;
  hkvVec3 vWrldStart = rotation.transform(vBoneStart);
  hkvVec3 vWrldDir = rotation.transform(vBoneDir);
  vWrldStart += translation;

  hkvVec3 vScaledDir = vWrldDir*fBoneLength;
  hkvVec3 vWrldEnd = vWrldStart + vScaledDir;
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA);
  VColorRef iColor(255,200,0,100);
  pRenderer->RenderCylinder((hkvVec3& )vWrldStart, (hkvVec3& )vScaledDir, fRadius, iColor, state, RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID|RENDERSHAPEFLAGS_CAP0|RENDERSHAPEFLAGS_CAP1, 14,1);
  pRenderer->RenderSphere((hkvVec3& )vWrldStart,fRadius, iColor, state, RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, 14,14);
  pRenderer->RenderSphere((hkvVec3& )vWrldEnd,fRadius, iColor, state, RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID, 14,14);
}


void VisParticleConstraintBones_cl::DebugRender(IVRenderInterface *pRenderer, const hkvVec3& vHelperPos) const
{
  const BoneInfo_t *pBoneInfo = m_pBoneInfo;
  for (int iBone=0;iBone<m_iBoneCount;iBone++,pBoneInfo++)
    pBoneInfo->DebugRender(pRenderer,m_pEntity);

}

bool VisParticleConstraintBones_cl::DataExchangeXML(TiXmlElement *pNode, bool bWrite)
{
  if (!DataExchangeXML_Base(pNode,GetShortName(),bWrite))
    return false;
// TODO
  return true;
}


#if defined (SUPPORTS_TERRAIN)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VisParticleConstraintTerrain_cl
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>

VisParticleConstraintTerrain_cl::VisParticleConstraintTerrain_cl(VTerrain *pTerrain, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eReflectMode, float fPersistance)
    : VisParticleConstraint_cl(eReflectMode,fPersistance)
{
  m_pTerrain = pTerrain;
  if (m_pTerrain==NULL && VTerrainManager::GlobalManager().GetResourceCount()>0)
    m_pTerrain = (VTerrain *)VTerrainManager::GlobalManager().GetResourceByIndex(0);
  m_iDebugColor.SetRGBA(0,100,0,64);
}


V_IMPLEMENT_SERIAL( VisParticleConstraintTerrain_cl, VisParticleConstraint_cl, 0, &g_VisionEngineModule );
void VisParticleConstraintTerrain_cl::Serialize( VArchive &ar )
{
  VisParticleConstraint_cl::Serialize(ar);
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version number");
    ar >> m_pTerrain;
  } else
  {
    ar << iLocalVersion;
    ar << m_pTerrain;
  }
}


void VisParticleConstraintTerrain_cl::HandleParticles(IVPhysicsParticleCollection_cl *pGroup, float fTimeDelta, VIS_CONSTRAINT_REFLECT_BEHAVIOR_e eForceBehavior)
{
  if (m_pTerrain==NULL)
    return;
  VASSERT(pGroup);
  Particle_t *p = pGroup->GetPhysicsParticleArray();
  const int iCount = pGroup->GetPhysicsParticleCount();
  const int iStride = pGroup->GetPhysicsParticleStride();

  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=m_eReflectBehavior;
  if (eForceBehavior==CONSTRAINT_REFLECT_DEFAULT) eForceBehavior=CONSTRAINT_REFLECT_NOTHING;

  int i;
  hkvPlane plane(hkvNoInitialization);
  hkvVec3 vPlaneNrml(hkvNoInitialization), vPos(hkvNoInitialization);
  const float fFramePersistance = hkvMath::pow (m_fPersistance,fTimeDelta);
  const float fFixedDistFactor = (eForceBehavior==CONSTRAINT_REFLECT_GLIDE) ? 0.499f : 0.5f;

  VTerrainSectorManager &sectors(m_pTerrain->m_SectorManager);
  const VTerrainConfig &config(m_pTerrain->m_Config);

  hkvVec3 vDirX(config.m_vSampleSpacing.x*0.5f,0.f,0.f);
  hkvVec3 vDirY(0.f,config.m_vSampleSpacing.y*0.5f,0.f);

  for (i=0;i<iCount;i++,NEXT_PARTICLE) if (p->valid)
  {
    vPos.x = p->pos[0];
    vPos.y = p->pos[1];
    VLargePosition vLargePos(config,(hkvVec3& )vPos);
    if (vLargePos.m_iSectorX<0 || vLargePos.m_iSectorY<0 || vLargePos.m_iSectorX>=config.m_iSectorCount[0] || vLargePos.m_iSectorY>=config.m_iSectorCount[1])
      continue;
    VTerrainSector *pSector = sectors.GetSector(vLargePos.m_iSectorX,vLargePos.m_iSectorY);
    if (!pSector->IsLoaded())
      continue; // may not call EnsureLoaded here as this is called in a thread!

    vPos.z = pSector->GetHeightAtRelPos(vLargePos.m_vSectorOfs);
  
    // compute normal manually (faster)
    vDirX.z = pSector->GetHeightAtRelPos(hkvVec3(vLargePos.m_vSectorOfs.x+vDirX.x,vLargePos.m_vSectorOfs.y,0.f)) - vPos.z;
    vDirY.z = pSector->GetHeightAtRelPos(hkvVec3(vLargePos.m_vSectorOfs.x,vLargePos.m_vSectorOfs.y+vDirY.x,0.f)) - vPos.z;
    vPlaneNrml = vDirX.cross(vDirY);
    vPlaneNrml.normalizeIfNotZero();
    VASSERT(vPlaneNrml.z>0);

    vPos.z += config.m_vTerrainPos.z;
    plane.setFromPointAndNormal(vPos,vPlaneNrml);

    if (HandleConstraintPlane(p,plane,vPlaneNrml,eForceBehavior,fFixedDistFactor,m_fPersistance,fFramePersistance, m_fReflectionNoise, pGroup->GetRandom()))
      pGroup->DestroyParticle(p,fTimeDelta);
  }
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
