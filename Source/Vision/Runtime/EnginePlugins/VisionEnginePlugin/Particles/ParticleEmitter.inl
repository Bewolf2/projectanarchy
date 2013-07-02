/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

///////////////////////////////////////////////////////////////////////////////////
// emitter inline functions
///////////////////////////////////////////////////////////////////////////////////

inline void VisParticleEmitter_cl::Helper_GetRandomSpherePosition(hkvVec3& vDst, float fRadius, const VRandom& randGen)
{
  float fRadiusSqr = fRadius*fRadius;
  // to distribute the particles evenly in the sphere we take a box and 
  // check if particle is inside the sphere. The chance is about 50% per iteration
  while (1)
  {
    vDst.x = randGen.GetFloatNeg()*fRadius;
    vDst.y = randGen.GetFloatNeg()*fRadius;
    vDst.z = randGen.GetFloatNeg()*fRadius;
    if (vDst.x*vDst.x+vDst.y*vDst.y+vDst.z*vDst.z<=fRadiusSqr)
      break;
  }
}

inline void VisParticleEmitter_cl::Helper_GetRandomOnSpherePosition(hkvVec3& vDst, float fRadius, const VRandom& randGen)
{
  Helper_GetRandomSpherePosition(vDst,fRadius, randGen);
  vDst.setLength(fRadius);
}

inline void VisParticleEmitter_cl::SetConeAngle(float fNewAngle, float fMinAngle)
{
  m_fConeAngle = fNewAngle;
  m_fMinConeAngle = fMinAngle;
  VASSERT(m_fConeAngle<0.f || m_fConeAngle>=m_fMinConeAngle);
}

inline float VisParticleEmitter_cl::GetConeAngle() const
{
  return m_fConeAngle;
}

inline void VisParticleEmitter_cl::SetType_Point()
{
  m_spPath=NULL;
  m_eType=EMITTER_TYPE_POINT;
}

inline void VisParticleEmitter_cl::SetType_Sphere(float fRadius) 
{
  m_spPath=NULL;
  m_eType=EMITTER_TYPE_SPHERE;
  m_vParam.set(fRadius,fRadius,fRadius, 0.0f);
}

inline void VisParticleEmitter_cl::SetType_Box(const hkvVec3& vBoxDim)
{
  m_spPath=NULL;
  m_eType=EMITTER_TYPE_BOX;
  m_vParam = (vBoxDim*0.5f).getAsVec4 (0.0f);
}

inline void VisParticleEmitter_cl::SetType_Plane(float fPlaneDimU,float fPlaneDimV)
{
  m_spPath=NULL;
  m_eType=EMITTER_TYPE_PLANE;
  m_vParam.set(fPlaneDimU*0.5f,fPlaneDimV*0.5f,0.f, 0.0f);
}

inline void VisParticleEmitter_cl::SetType_Path(VisPath_cl *pPath,float fRadius)
{
  m_eType=EMITTER_TYPE_PATH;
  m_vParam.x=fRadius;
  m_spPath=pPath;
  if (!pPath) m_eType=EMITTER_TYPE_UNKNOWN;
}

inline void VisParticleEmitter_cl::SetType_Path(const char *szPathKey,float fRadius)
{
  SetType_Path(Vision::Game.SearchPath(szPathKey),fRadius);
}

inline void VisParticleEmitter_cl::SetType_Ray(float fRayLen)
{
  m_eType=EMITTER_TYPE_RAY;
  m_vParam.x = fRayLen;
}

inline void VisParticleEmitter_cl::SetMeshEmitterEntity(VisBaseEntity_cl *pEntity)
{
  if (pEntity==NULL)
  {
    m_wpEntityMeshEmitter = NULL;
  } else
  {
    m_wpEntityMeshEmitter = pEntity->GetWeakReference();
  }
  UpdateMeshEmitterEntity();
}


inline void VisParticleEmitter_cl::UpdateMeshEmitterEntity()
{
  if (m_wpEntityMeshEmitter!=NULL && m_wpEntityMeshEmitter.GetPtr()!=NULL)
  {
    m_spEmitterMesh = m_wpEntityMeshEmitter->GetTraceMesh(true);
  }
  else
  {
    m_wpEntityMeshEmitter = NULL;
    m_spEmitterMesh = NULL;
  }
}

inline void VisParticleEmitter_cl::SetSpawnFrequency(float fParticlesPerSecond, float fVariation)
{
  m_ParticlesPerSec.Set(fParticlesPerSecond,fVariation);
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
