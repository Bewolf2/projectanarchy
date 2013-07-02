/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



///////////////////////////////////////////////////////////////////////////////////
// IVPhysicsParticleCollection_cl
///////////////////////////////////////////////////////////////////////////////////

inline IVPhysicsParticleCollection_cl::IVPhysicsParticleCollection_cl(unsigned int uiBaseSeed)
{
  m_uiBaseSeed = (uiBaseSeed!=0) ? uiBaseSeed : (Vision::Game.GetRand()+1); 
}

inline void IVPhysicsParticleCollection_cl::UpdateSeed()
{
  // Set seed based on frame, a prime number, and the base seed
 // m_generator.Reset(Vision::Game.GetUpdateSceneCount() * 137 + m_uiBaseSeed);

  // we shouldn't use GetUpdateSceneCount here because SetUpdateSceneCount might be called while this thread is running causing non-deterministic behavior
  // instead, increase the seed every frame manually. This has the advantage, that particles are deterministic even when spawned at distinct frame times.
  m_generator.Reset(m_uiBaseSeed);
  m_uiBaseSeed += 137;
}


///////////////////////////////////////////////////////////////////////////////////
// particle group
///////////////////////////////////////////////////////////////////////////////////

inline VisParticleGroupDescriptor_cl * ParticleGroupBase_cl::GetDescriptor() const
{
  return m_spDescriptor;
}

inline void ParticleGroupBase_cl::SetEmitter(VisParticleEmitter_cl *pEmitter)
{
  EnsureUpdaterTaskFinished();

  m_spEmitter = pEmitter;
  if (pEmitter)
    pEmitter->m_vLastEmitterPos = m_vPosition;
}

inline VisParticleEmitter_cl* ParticleGroupBase_cl::GetEmitter() const
{
  return m_spEmitter;
}

inline bool ParticleGroupBase_cl::RemoveConstraint(VisParticleConstraint_cl *pConstraint)
{
  EnsureUpdaterTaskFinished();

  bool bRes = m_Constraints.RemoveConstraint(pConstraint);

  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->RemoveConstraint(pConstraint);

  return bRes;
}

inline void ParticleGroupBase_cl::RemoveAllConstraints()
{
  EnsureUpdaterTaskFinished();

  m_Constraints.ReleaseAllConstraints();
  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->RemoveAllConstraints();
}

inline void ParticleGroupBase_cl::SetScaling(float fScale)
{
  if (m_fScaling != fScale)
  {
    m_fScaling = fScale;
    m_bBBoxValid = false;
    m_bVisibilityUpdate = true;
    SetInitialTransformation(); // local space update
  }
}

inline float ParticleGroupBase_cl::GetScaling() const
{
  return m_fScaling;
}


inline void ParticleGroupBase_cl::SetOrientation(float fYaw,float fPitch,float fRoll)
{
  hkvMat3 mTemp (hkvNoInitialization);
  mTemp.setFromEulerAngles (fRoll, fPitch, fYaw);
  SetOrientation(mTemp);
}

inline void ParticleGroupBase_cl::SetOrientation(const hkvMat3 &mRotMat)
{
  SetRotationMatrix(mRotMat);
}

inline void ParticleGroupBase_cl::SetOrientation(const hkvVec3& vDir, const hkvVec3& vUp, const hkvVec3& vRight)
{
  hkvMat3 mTemp (hkvNoInitialization);
  mTemp.setAxisXYZ (vDir,vRight,vUp);
  SetOrientation(mTemp);
}


inline void ParticleGroupBase_cl::SetPause(bool bStatus)
{
  m_bPaused = bStatus;
}

inline bool ParticleGroupBase_cl::IsPaused() const
{
  return m_bPaused;
}

inline void ParticleGroupBase_cl::SetHalted(bool bStatus)
{
  m_bHalted = bStatus;
}

inline bool ParticleGroupBase_cl::IsHalted() const
{
  return m_bHalted;
}


inline bool ParticleGroupBase_cl::AddParticleToCache(ParticleExt_t *pParticle)
{
  pParticle->valid = 0; // this will be checked again when returning free particle
  if (m_iCachedParticleCount>=m_iMaxCachedParticleCount)
    return false;

  // cache the index
  int iIndex = (int)(pParticle-GetParticlesExt()); // no division by particle size here :-)
  VASSERT(&GetParticlesExt()[iIndex]==pParticle);
  m_piCachedParticle[m_iCachedParticleCount++] = iIndex;
  return true;
}

inline void ParticleGroupBase_cl::InitSingleParticle(ParticleExt_t *pParticle)
{
  VASSERT(pParticle);
  m_spDescriptor->InitRandomParticle(pParticle, this);
}

inline bool ParticleGroupBase_cl::CreateParticleFromSource(const ParticleExt_t *pSrcParticle, float fTimeDelta, int iCopyFlags)
{
  ParticleExt_t *p = GetFreeParticle();
  if (!p) return false;
  m_spDescriptor->InitRandomParticle(p, this, pSrcParticle,iCopyFlags);
  HandleSingleParticle(p,fTimeDelta);
  return true;
}

 
inline void ParticleGroupBase_cl::SetPerFrameConstants(float dtime)
{
  if (m_spDescriptor->m_fFriction>0.f)
    m_fFrameFriction = hkvMath::pow (1.f-m_spDescriptor->m_fFriction, dtime);
  else 
    m_fFrameFriction = 1.f;

  m_vFrameWind = m_vWindSpeed*dtime;
  if (m_bWindInLocalSpace)
  {
    const hkvMat3 &mRot = GetRotationMatrix();
    m_vFrameWind = mRot.transformDirection (m_vFrameWind);
  }

  if (m_bInertiaAffectsGravity)
  {
    m_vFrameWind += m_spDescriptor->m_vGravity * dtime;
    m_vFrameWindNoInertia.setZero();
  }
  else
    m_vFrameWindNoInertia = m_spDescriptor->m_vGravity * dtime;

  // transform per-frame speeds back into local space
  if (GetUseLocalSpaceMatrix())
  {
    hkvMat3 transposedRot = GetRotationMatrix();
    transposedRot.transpose();
    m_vFrameWind = transposedRot.transformDirection (m_vFrameWind);
    m_vFrameWindNoInertia = transposedRot.transformDirection (m_vFrameWindNoInertia);
  }
}


inline void ParticleGroupBase_cl::SetParticleAnimFrame(ParticleExt_t *pParticle, float fFrame, bool bWrapAround)
{
  pParticle->animation_frame = (int)(fFrame);

  if(bWrapAround)
  {
    pParticle->animation_next_frame = (pParticle->animation_frame+1) % (m_iMaxAnimFrame + 1);
  }
  else
  {
    pParticle->animation_next_frame = hkvMath::Min (pParticle->animation_frame+1,m_iMaxAnimFrame);
  }
  
  pParticle->blend_weight = hkvMath::float2int ((fFrame - (float)pParticle->animation_frame)*255.99f);
}


inline bool ParticleGroupBase_cl::HandleSingleParticle(ParticleExt_t *pParticle, float fDeltaTime)
{
  // handle lifetime
  pParticle->m_fLifeTimeCounter += fDeltaTime*pParticle->m_fLifeTimeInc;
  if (VISION_UNLIKELY(pParticle->m_fLifeTimeCounter>=1.f)) // lifetime is over
  {
    if (m_bRepeatLifetime)
      pParticle->m_fLifeTimeCounter = hkvMath::mod (pParticle->m_fLifeTimeCounter,1.f);
    else
    {
      DestroyParticle(pParticle,fDeltaTime);
      return false;
    }
  }


  // assign new color
  if (VISION_LIKELY(m_pColorLookup!=NULL))
  {
    // do the color lookup at y=per particle random value, x=lifetime
    // use the non-interpolating version of LookupTexelColor because of performance considerations

    pParticle->color = m_InstanceColor * pParticle->m_ModColor * 
     m_pColorLookup[(int)(pParticle->m_fLifeTimeCounter*m_fColorBitmapSizeX)];
//    pParticle->color = m_pColorLookup[(int)(pParticle->m_fLifeTimeCounter*m_fColorBitmapSizeX)];

#ifdef PARTICLE_LIGHTGRID_LIGHTING
    VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
    if (pGrid)
    {
      hkvVec3 vLGColor(false);
      pGrid->GetMaximumColorAtPosition((hkvVec3& )pParticle->pos,vLGColor);
      vLGColor *= 2.0f;
      int r = (int)((float)pParticle->color.r * vLGColor.x);
      int g = (int)((float)pParticle->color.g * vLGColor.y);
      int b = (int)((float)pParticle->color.b * vLGColor.z);

      pParticle->color.r = hkvMath::Min(r,255);
      pParticle->color.g = hkvMath::Min(g,255);
      pParticle->color.b = hkvMath::Min(b,255);
    }
#endif
  } else
  {
    pParticle->color = m_InstanceColor * pParticle->m_ModColor;
  }

  // particle size : either from size curve of lookup or just grow size
  if (VISION_LIKELY(m_spSizeCurve!=NULL))
  {
    pParticle->size = m_spSizeCurve->GetValueFast(pParticle->m_fLifeTimeCounter) * pParticle->m_fSizeGrowth;
  } else
  {
    pParticle->size += pParticle->m_fSizeGrowth * fDeltaTime;
    if (pParticle->size<=0.f)
    {
      DestroyParticle(pParticle,fDeltaTime);
      return false;
    }
  }

  pParticle->pos[0] += pParticle->velocity[0]*fDeltaTime;
  pParticle->pos[1] += pParticle->velocity[1]*fDeltaTime;
  pParticle->pos[2] += pParticle->velocity[2]*fDeltaTime;

  if (m_bMovesWithEmitter)
  {
    float fWeight = m_fLocalFactorStart + m_fLocalFactorDiff * pParticle->m_fLifeTimeCounter;
    pParticle->pos[0] += m_vGroupMoveDelta.x * fWeight;
    pParticle->pos[1] += m_vGroupMoveDelta.y * fWeight;
    pParticle->pos[2] += m_vGroupMoveDelta.z * fWeight;
  }

  pParticle->velocity[0] = (pParticle->velocity[0] + m_vFrameWind.x*pParticle->m_fInertiaFactor + m_vFrameWindNoInertia.x)*m_fFrameFriction;
  pParticle->velocity[1] = (pParticle->velocity[1] + m_vFrameWind.y*pParticle->m_fInertiaFactor + m_vFrameWindNoInertia.y)*m_fFrameFriction;
  pParticle->velocity[2] = (pParticle->velocity[2] + m_vFrameWind.z*pParticle->m_fInertiaFactor + m_vFrameWindNoInertia.z)*m_fFrameFriction;

  float fAnimFrame;
  // animation
  switch (m_eParticleAnimMode)
  {
  case PARTICLE_ANIMATION_REPEAT:
  case PARTICLE_ANIMATION_LIFETIME:
    pParticle->m_fAnimPos = hkvMath::mod (pParticle->m_fAnimPos+pParticle->m_fAnimInc*fDeltaTime, 1.f);
    if (m_spAnimCurve)
    {
      fAnimFrame = m_spAnimCurve->GetValueFast(pParticle->m_fAnimPos);
      SetParticleAnimFrame(pParticle,fAnimFrame, m_eParticleAnimMode == PARTICLE_ANIMATION_REPEAT);
    } 
    else
      SetParticleAnimFrame(pParticle,pParticle->m_fAnimPos*m_fAnimFrameCount, m_eParticleAnimMode == PARTICLE_ANIMATION_REPEAT);
    break;
  }
  // verify animation frame is in valid range
  VASSERT(pParticle->animation_frame < m_spDescriptor->m_iUsedAnimFrames);

  // rotation
  if (m_spRotationCurve)
  {
    pParticle->m_fRotationParam0 = hkvMath::mod (pParticle->m_fRotationParam0+pParticle->m_fRotationParam1*fDeltaTime, 1.f);
    pParticle->angle = m_spRotationCurve->GetValueFast(pParticle->m_fRotationParam0);
  } else
  {
    // constant angle speed
    if (*((int*)&(pParticle->m_fRotationParam0)))
      pParticle->angle = hkvMath::mod (pParticle->angle+pParticle->m_fRotationParam0*fDeltaTime, (hkvMath::pi () * 2.0f));
  }

  if (!m_cUseDistortion)
    return true;

  // particle distortion
  if (m_cUseDistortion==DISTORTION_TYPE_SIZEMODE)
  {
    hkvVec3& distort = (hkvVec3&) pParticle->distortion;

    distort.x = m_vSizeMultiplier.x * pParticle->size;
    distort.y = m_vSizeMultiplier.y * pParticle->size;
    distort.z = m_vSizeMultiplier.z * pParticle->size;

    // still unrotated, so rotate vector:
    distort = m_cachedRotMatrix.transformDirection (distort);

  }
  else if (m_cUseDistortion==DISTORTION_TYPE_VELOCITY) // speed multiplier
  {
    pParticle->distortion[0] = pParticle->m_fDistortionMult * pParticle->velocity[0];
    pParticle->distortion[1] = pParticle->m_fDistortionMult * pParticle->velocity[1];
    pParticle->distortion[2] = pParticle->m_fDistortionMult * pParticle->velocity[2];
  } 
  else if (m_cUseDistortion==DISTORTION_TYPE_FIXLEN)
  {
    hkvVec3 dist(pParticle->velocity[0],pParticle->velocity[1],pParticle->velocity[2]);
    if (!dist.isZero ())
    {
      dist.setLength (pParticle->m_fDistortionMult);
      pParticle->distortion[0] = dist.x;
      pParticle->distortion[1] = dist.y;
      pParticle->distortion[2] = dist.z;
    } else
    {
      // do not change distortion
    }
  }
  else if (m_cUseDistortion==DISTORTION_TYPE_TRAIL)
  {
    // handled outside this loop

  }
  else if (m_cUseDistortion==DISTORTION_TYPE_CUSTOM)
  {
    // nothing to do

  }
  else 
  {
    VASSERT_MSG(FALSE, "Unsupported distortion mode")
  }

  if (m_bDistortionPlaneAligned) // plane align the particle to always point up
  {
    hkvVec3 vRight(hkvNoInitialization);
    if (hkvMath::Abs (pParticle->velocity[0])>hkvMath::Abs (pParticle->velocity[1]))
      vRight.set(0,1,0);
    else
      vRight.set(1,0,0);

    vRight = vRight.cross ((hkvVec3&) pParticle->distortion[0]);
    vRight.normalize();
    pParticle->normal[0] = vRight.x;
    pParticle->normal[1] = vRight.y;
    pParticle->normal[2] = vRight.z;
  }
  return true;
}

inline void ParticleGroupBase_cl::SetVisible(bool bStatus)
{
  if (bStatus) 
    SetVisibleBitmask(0xFFFFFFFF); 
  else 
    SetVisibleBitmask(0);

  // also activate/deactivate the visibiliy object.
  // Make sure this only happens when visible status changes, because SetActivate resets the test counters
  // currently the particle effect takes care for this
  VisVisibilityObject_cl *pVisObj = GetVisibilityObject();
  if (pVisObj)
    pVisObj->SetActivate(bStatus);

  if (m_spOnDestroyCreateGroup)
    m_spOnDestroyCreateGroup->SetVisible(bStatus);
}


inline void ParticleGroupBase_cl::SetDistortionType(unsigned char eDistortionType)
{
  m_cUseDistortion = eDistortionType;
}


inline unsigned char ParticleGroupBase_cl::GetDistortionType() const
{
  return m_cUseDistortion;
}


inline void ParticleGroupBase_cl::RenderBoundingBox(VColorRef iColor)
{
  const hkvAlignedBBox *pBBox = GetCurrentBoundingBox();
  if (pBBox)
    Vision::Game.DrawBoundingBox(*pBBox,iColor,1.f);
}


inline const hkvAlignedBBox *ParticleGroupBase_cl::GetCurrentBoundingBox()
{
  if (!m_bBBoxValid)
  {
    // create an oriented bounding box
    m_bBBoxValid = true;
    m_BoundingBox.setInvalid();
    if (m_spDescriptor->m_fDynamicInflateInterval>=0.f) // create bbox from all particles
    {
      InflateBoundingBox(true);
      return &m_BoundingBox;
    }
      
    // transform descriptor's local bounding box to global world space
    if (!m_spDescriptor->m_BoundingBox.isValid())
      return NULL;
    if (GetUseLocalSpaceMatrix())
      m_BoundingBox = m_spDescriptor->m_BoundingBox;
    else
    {
      if(m_spEmitter->GetType()==EMITTER_TYPE_MESH && m_pEmitterMeshEntity!=NULL)
      {
        m_BoundingBox.expandToInclude( m_pEmitterMeshEntity->GetBoundingBox());
        //add boundary of the initial bounding box
        hkvVec3 boundary(m_spDescriptor->m_BoundingBox.getSizeX()*0.5f,m_spDescriptor->m_BoundingBox.getSizeY()*0.5f,m_spDescriptor->m_BoundingBox.getSizeZ()*0.5f);
        m_BoundingBox.addBoundary(boundary);
      }
      else
      {
        hkvMat3 rotMat = GetRotationMatrix();
        hkvAlignedBBox orientedBBox = m_spDescriptor->m_BoundingBox;
        hkvMat4 mTransform (rotMat, GetPosition());
        orientedBBox.transformFromOrigin (mTransform);
        m_BoundingBox.expandToInclude (orientedBBox);
      }
    }
  }
  if (VISION_LIKELY(m_BoundingBox.isValid()))
    return &m_BoundingBox;
  return NULL;
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
