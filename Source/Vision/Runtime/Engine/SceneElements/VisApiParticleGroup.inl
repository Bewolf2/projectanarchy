/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiParticleGroup.inl

extern "C" bool g_bSupportsInstancing;


inline void VisParticleGroup_cl::SetParticleStride(short iStructStride, short iStructOfs)
{
  VASSERT(iStructStride>=sizeof(Particle_t));
  m_iStructStride = iStructStride;
  m_iStructOfs = iStructOfs;
}

inline void VisParticleGroup_cl::SetAnimationSubdivisions(int iSubDivX,int iSubDivY)
{
  m_iAnimSubDivX = iSubDivX;
  m_iAnimSubDivY = iSubDivY;
  UpdateDefaultShaderFlags();
}

inline void VisParticleGroup_cl::SetUseSmoothAnimationFading(BOOL bStatus)
{
  m_bUseSmoothAnimation = bStatus==TRUE;
  UpdateDefaultShaderFlags();
}

inline BOOL VisParticleGroup_cl::GetUseSmoothAnimationFading() const
{
  return m_bUseSmoothAnimation;
}


inline void VisParticleGroup_cl::SetParticleSizeAspectRatio(float fAspectRatio)
{
  m_fSizeAspect = fAspectRatio;
}

inline VisVisibilityObject_cl* VisParticleGroup_cl::GetVisibilityObject() const
{
  return m_spVisObject;
}

inline void VisParticleGroup_cl::SetVisibilityObject(VisVisibilityObject_cl* pVisObj)
{
  if (pVisObj == m_spVisObject)
    return;
  if (m_spVisObject != NULL)
    m_spVisObject->RemoveObject3D(this);
  m_spVisObject=pVisObj;
  if (pVisObj != NULL)
    m_spVisObject->AddObject3D(this);
}

inline const char *VisParticleGroup_cl::GetTexture() const
{
  if (!spTexture) return NULL;
  return spTexture->GetFilename();
}

inline void VisParticleGroup_cl::TriggerTextureAnim()
{
  if (texanim_id)
    texanim_id->SetTriggerStatus(TRUE);
}


inline void VisParticleGroup_cl::ResetTextureAnim()
{
  if (texanim_id)
    texanim_id->Reset();
}


inline float VisParticleGroup_cl::GetTextureAnimPhase()
{
  if (texanim_id)
    return texanim_id->GetCurrentPhase();
  return 0.f;
}


inline int VisParticleGroup_cl::GetTextureAnimNumOfPhases()
{
  if (texanim_id)
    texanim_id->GetNumberOfFrames();
  return 0;
}


inline void VisParticleGroup_cl::SetTextureAnimPhase(float newPhase)
{  
  if (texanim_id)
    texanim_id->SetAnimPhase(newPhase);
}


inline void VisParticleGroup_cl::IncTextureAnimPhase(float phaseDelta)
{  
  if (texanim_id)
    texanim_id->IncAnimPhase(phaseDelta);
}

inline VisTextureAnimInstance_cl* VisParticleGroup_cl::GetTextureAnimationInstance() const
{
  return texanim_id;
}


inline void VisParticleGroup_cl::SetAlwaysInForeGround(BOOL status)
{
  if (status)
  {
    m_iRenderState.SetFlag(RENDERSTATEFLAG_ALWAYSVISIBLE);
    if (m_spVisObject) 
      m_spVisObject->SetVisTestFlags(m_spVisObject->GetVisTestFlags() & (~VISTESTFLAGS_HARDWAREOCCLUSIONQUERY));
  }
  else
  {
    m_iRenderState.RemoveFlag(RENDERSTATEFLAG_ALWAYSVISIBLE);
    if (m_spVisObject) 
      m_spVisObject->SetVisTestFlags(m_spVisObject->GetVisTestFlags() | VISTESTFLAGS_HARDWAREOCCLUSIONQUERY);
  }
}


inline void VisParticleGroup_cl::SetTransparencyType(VIS_TransparencyType newTransparencyType)
{
  m_iRenderState.SetTransparency(newTransparencyType);
  int iOpaqueFlags = RENDERSTATEFLAG_WRITETOZBUFFER|RENDERSTATEFLAG_ALPHATEST|RENDERSTATEFLAG_USEFOG;
  if (newTransparencyType==VIS_TRANSP_COLORKEY||newTransparencyType==VIS_TRANSP_NONE)
    m_iRenderState.SetFlag(iOpaqueFlags);
  else
    m_iRenderState.RemoveFlag(iOpaqueFlags);
}


inline VIS_TransparencyType VisParticleGroup_cl::GetTransparencyType() const
{
  return (VIS_TransparencyType)m_iRenderState.GetTransparency();
}

inline int VisParticleGroup_cl::GetNumOfParticles() const 
{ 
  return numParticles; 
}


inline Particle_t *VisParticleGroup_cl::GetParticles() const
{ 
  return (Particle_t *)particles;
} 


inline BOOL VisParticleGroup_cl::GetCheckVisibility() const
{
  return (m_spVisObject!=NULL);
}


inline void VisParticleGroup_cl::SetTextureRepetitions(float numOfRepetitions)
{
  numtexrepetitions = numOfRepetitions;
}


inline void VisParticleGroup_cl::SetBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
  SetBoundingBox (hkvAlignedBBox(hkvVec3 (minX,minY,minZ), hkvVec3 (maxX,maxY,maxZ)));
}

inline void VisParticleGroup_cl::SetTechnique(VCompiledTechnique *pTechnique)
{
  m_spTechnique = pTechnique;
}

inline VCompiledTechnique *VisParticleGroup_cl::GetTechnique() const
{
  return m_spTechnique;
}

inline void VisParticleGroup_cl::SetUseNormals(bool bStatus)
{
  m_bUseNormals = bStatus;
}

inline bool VisParticleGroup_cl::GetUseNormals() const
{
  return m_bUseNormals;
}

inline void VisParticleGroup_cl::SetUseDistortion(bool bStatus)
{
  // This sets DISTORTION_TYPE_VELOCITY only
  m_cUseDistortion = (unsigned char)bStatus;
  if (bStatus) m_vParticleCenter.set(0.f,0.5f);
}

inline bool VisParticleGroup_cl::GetUseDistortion() const
{
  return m_cUseDistortion!=0;
}

void VisParticleGroup_cl::SetParticleCenter(float x, float y)
{
  m_vParticleCenter.set(x,y);
}

void VisParticleGroup_cl::GetParticleCenter(float &x, float &y) const
{
  x = m_vParticleCenter.x;
  y = m_vParticleCenter.y;
}


inline bool VisParticleGroup_cl::GetHardwareSpanning() const
{
  return m_bUseHardwareSpanning;
}

inline ParticleSort_t *VisParticleGroup_cl::GetParticleIndexList() const
{ 
  return m_pIndexList;
} 

inline void VisParticleGroup_cl::SetSortParticles(bool bStatus)
{
  m_bSortParticles = bStatus;
}

inline bool VisParticleGroup_cl::GetSortParticles() const
{
  return m_bSortParticles;
}


inline void VisParticleGroup_cl::SetUserSortKey(int iUserSortKey)
{
  m_iUserSortKey = iUserSortKey;
}

inline int VisParticleGroup_cl::GetUserSortKey()
{
  return m_iUserSortKey;
}


inline unsigned int VisParticleGroup_cl::GetRenderOrder() const
{
  return m_iRenderOrder;
}

inline void VisParticleGroup_cl::SetVisibleBitmask(unsigned int iBitmask)
{
  m_iVisibleBitmask=iBitmask;
}

inline unsigned int VisParticleGroup_cl::GetVisibleBitmask() const
{
  return m_iVisibleBitmask;
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
