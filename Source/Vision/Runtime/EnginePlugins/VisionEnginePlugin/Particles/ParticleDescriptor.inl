/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


///////////////////////////////////////////////////////////////////////////////////
// descriptor inline functions
///////////////////////////////////////////////////////////////////////////////////

// static
inline float VisParticleGroupDescriptor_cl::GetRandomValue(const VRandom& randGen, float fAverage, float fVariation)
{
  float fMul = 1.f + fVariation*randGen.GetFloatNeg();
  return fMul * fAverage;
}


inline float VisParticleGroupDescriptor_cl::GetRandomLifeTime(const VRandom& randGen) const 
{
  return m_ParticleLifeTime.GetRandomValue(randGen);
}

inline float VisParticleGroupDescriptor_cl::GetRandomGroupLifeTime(const VRandom& randGen) const 
{
  return m_GroupLifeTime.GetRandomValue(randGen);
}

inline float VisParticleGroupDescriptor_cl::GetRandomSpeed(const VRandom& randGen) const
{
  return m_ParticleSpeed.GetRandomValue(randGen);
}



inline int VisParticleGroupDescriptor_cl::GetEstimatedMaxParticleCount(float fLifeTime, float fFreq) const
{
  if (fLifeTime<0.f)
  {
    fLifeTime = m_ParticleLifeTime.m_fAverage * (m_ParticleLifeTime.m_fVariation*0.5f+1.f); // take average variation into account

    // Take group lifetime into account also and choose the lower of the two.
    if(m_GroupLifeTime.m_fAverage > 0.0f)
    {
      float groupLifetime = m_GroupLifeTime.m_fAverage * (m_GroupLifeTime.m_fVariation*0.5f+1.f);
      float fillPercentage = m_DefaultEmitter.m_StartupFillPercentage.m_fAverage * (m_DefaultEmitter.m_StartupFillPercentage.m_fVariation*0.5f+1.f);
      fLifeTime = hkvMath::interpolate(groupLifetime, fLifeTime, hkvMath::clamp(fillPercentage * 0.01f, 0.0f, 1.0f));
    }
  }
  if (fFreq<0.f)
    fFreq = m_DefaultEmitter.m_ParticlesPerSec.m_fAverage;

  int iCount = (int)(fLifeTime*1.01f*fFreq); // just very rough estimate
  iCount = hkvMath::Min(iCount,0xfff0); // clamp it because we use short indices in some places
  return iCount;
}


inline void VisParticleGroupDescriptor_cl::SetTextureFilename(const char *szFilename)
{
//  char szPathname[FS_MAX_PATH];
//  CombineWithRelPath(szPathname,szFilename);
  m_sTextureFilename = szFilename;
}

inline void VisParticleGroupDescriptor_cl::SetTransparency(VIS_TransparencyType eTransp)
{
  m_eTransp = eTransp;
}

inline void VisParticleGroupDescriptor_cl::SetAnimationSubdivisions(int iSubDivX,int iSubDivY, int iUsedCount)
{
  m_iAnimSubDiv[0] = iSubDivX;
  m_iAnimSubDiv[1] = iSubDivY;
  if (iUsedCount<0)
    m_iUsedAnimFrames = GetAnimationFrameCount(); 
  else
    m_iUsedAnimFrames = iUsedCount;
}


inline int VisParticleGroupDescriptor_cl::GetAnimationFrameCount() const
{
  return m_iAnimSubDiv[0]*m_iAnimSubDiv[1];
}

/*
inline bool VisParticleGroupDescriptor_cl::SetColorLookupTexture(const char *szFilename)
{
  m_spColorLookup = VisBitmap_cl::LoadBitmapFromFile(szFilename);
  return (m_spColorLookup!=NULL);
}

inline void VisParticleGroupDescriptor_cl::SetColorLookupTexture(VisBitmap_cl *pBitmap)
{
  m_spColorLookup = pBitmap;
}

inline bool VisParticleGroupDescriptor_cl::SetSizeLookupTexture(const char *szFilename, float fMaxSize)
{
  m_spSizeLookup = VisBitmap_cl::LoadBitmapFromFile(szFilename);
  if (fMaxSize>0.f) 
    m_ParticleStartSize.Set(fMaxSize,0.f);
  return (m_spSizeLookup!=NULL);
}

*/


inline void VisParticleGroupDescriptor_cl::SetParticleLifeTime(float fAverage, float fVariation)
{
  m_ParticleLifeTime.Set(fAverage,fVariation);
}

inline void VisParticleGroupDescriptor_cl::SetGroupLifeTime(float fAverage, float fVariation)
{
  m_GroupLifeTime.Set(fAverage,fVariation);
}


inline void VisParticleGroupDescriptor_cl::SetSizeGrowth(float fStartSize, float fSizeGrowth)
{
  m_ParticleStartSize.Set(fStartSize,0.f);
  m_ParticleSizeGrowth.Set(fSizeGrowth,0.f);
}

inline void VisParticleGroupDescriptor_cl::SetSpeed(float fAverage, float fVariation)
{
  m_ParticleSpeed.Set(fAverage,fVariation);
}



inline void VisParticleGroupDescriptor_cl::SetFriction(float fValue)
{
  m_fFriction = fValue;
}

inline void VisParticleGroupDescriptor_cl::SetGravity(float fValue)
{
  m_vGravity.z = fValue;
}

inline int VisParticleGroupDescriptor_cl::binaryWeightSearch(float fAccumulatedWeight)
{
  int	startIndex=0;
  int stopIndex = m_iUsedAnimFrames-1;

  while(stopIndex > startIndex)
  {
    const int midIndex = (startIndex+stopIndex) >> 1;

    if(m_spProbabilityFrameWeights[midIndex] < fAccumulatedWeight)
      startIndex = midIndex + 1;
    else
      stopIndex = midIndex;
  }

  return startIndex;
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
