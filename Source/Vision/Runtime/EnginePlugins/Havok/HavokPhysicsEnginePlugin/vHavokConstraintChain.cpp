/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>

#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainData.h>
#include <Physics2012/Dynamics/Constraint/Chain/hkpConstraintChainInstance.h>

#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>

// ----------------------------------------------------------------------------
// vHavokConstraintChainDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokConstraintChainDesc, VTypedObject, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokConstraintChainDesc::s_iSerialVersion = 6;

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc::vHavokConstraintChainDesc()
{
  m_pszPathKey = NULL;
  Reset();
}

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc::vHavokConstraintChainDesc(bool bDummyNoInit)
{
  m_pszPathKey = NULL;
}

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc::~vHavokConstraintChainDesc()
{
  Reset();
}

// ----------------------------------------------------------------------------
const char *vHavokConstraintChainDesc::GetPathKey() const
{
  return m_pszPathKey;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainDesc::Reset()
{
  m_spAnchorBodies[0] = NULL;
  m_spAnchorBodies[1] = NULL;

  m_vAnchorPivots[0] = hkvVec3(0, 0, 0);
  m_vAnchorPivots[1] = hkvVec3(0, 0, 0);


  m_fLinkLength = 10.f;
  m_fLinkGap = 0.f;
  m_fDiameter = 1.0f;
  m_fMaterialDensity = 1.0f * 1e3f;
  m_fMaterialStretchingConstant = 1.0f * 1e9f;
  
  m_fChainDamping = 1.0f;
  m_fLinkLinearDamping = 0.1f;
  m_fLinkAngularDamping = 0.1f;

  m_bCalculateLinkInertia = false;
  m_vInertiaTensorAxisScaling.set (1.0f);
  m_fInertiaTensorUniformScaling = 1.f;

  m_iLinkCollisionGroup = 0;
  m_iLinkCollisionLayer = 0;
  m_iLinkSubSystemId = 0;
  m_iLinkSubSystemDontCollideWith = 0;

  m_bComputeCfm = false;
  m_fCfmFactor = 1.f;
  m_fFixedCfm = 0.0001f;

  m_bUseAngularFriction = false;
  m_fFrictionFactorMin = 0.3f;
  m_fFrictionFactorMax = 30.f;
  m_fBaseTension = 0.f;

  if (m_pszPathKey)
    V_SAFE_STRFREE(m_pszPathKey);
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainDesc::Serialize( VArchive &ar )
{
  if (ar.IsLoading())
  {
    Reset();

    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    vHavokRigidBody *pBody;
    if (iVersion < 6)
    {
      for (unsigned int i = 0; i < 2; ++i)
      {
        pBody = NULL;
        ar >> pBody;
        m_spAnchorBodies[i] = pBody;
      }
    }
    else
    {
      for (unsigned int i = 0; i < 2; ++i)
      {
        m_spAnchorBodies[i] = NULL;
        VisTypedEngineObject_cl *pParentObj = VisTypedEngineObject_cl::DeSerializeObjectReference(ar);
        if (!pParentObj || !pParentObj->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
          continue;
        VisBaseEntity_cl *pParentEntity = static_cast<VisBaseEntity_cl*>(pParentObj);
        m_spAnchorBodies[i] = static_cast<vHavokRigidBody*>(
          pParentEntity->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(vHavokRigidBody)));
      }
    }

    m_vAnchorPivots[0].SerializeAsVisVector (ar);
    m_vAnchorPivots[1].SerializeAsVisVector (ar);

    ar >> m_pszPathKey;

    ar >> m_fLinkLength;
    ar >> m_fLinkGap;
    ar >> m_fDiameter;
    if (iVersion <= 2)
    {
      // Older version save the mass, not the density. When loading, calculate
      // the density from the mass and the volume.
      float fMass;
      ar >> fMass;
      float fRadius = float(VIS2HK_FLOAT_SCALED(m_fDiameter * 0.5f));
      float fVolume = hkvMath::pi () * (fRadius*fRadius) * float(VIS2HK_FLOAT_SCALED(m_fLinkLength));
      if (fVolume > 0.f)
        m_fMaterialDensity = fMass / fVolume;
    }
    else
    {
      ar >> m_fMaterialDensity;
      ar >> m_fMaterialStretchingConstant;
    }

    ar >> m_fChainDamping;

    if (iVersion >= 2)
    {
      ar >> m_fLinkLinearDamping;
      ar >> m_fLinkAngularDamping;

      ar >> m_iLinkCollisionGroup;
      ar >> m_iLinkCollisionLayer;
      ar >> m_iLinkSubSystemId;
      ar >> m_iLinkSubSystemDontCollideWith;
    }

    if (iVersion >= 3)
    {
      ar >> m_fCfmFactor;
    }

    if (iVersion >= 4)
    {
      ar >> m_bCalculateLinkInertia;
      m_vInertiaTensorAxisScaling.SerializeAsVisVector (ar);
      ar >> m_fInertiaTensorUniformScaling;
    }

    if (iVersion >= 5)
    {
      ar >> m_bComputeCfm;
      ar >> m_fFixedCfm;
      ar >> m_bUseAngularFriction;
      ar >> m_fFrictionFactorMin;
      ar >> m_fFrictionFactorMax;
      ar >> m_fBaseTension;
    }
  }
  else
  {
    ar << s_iSerialVersion;

    for (unsigned int i = 0; i < 2; ++i)
    {
      if (m_spAnchorBodies[i])
        VisTypedEngineObject_cl::SerializeObjectReference(ar, m_spAnchorBodies[i]->GetOwner());
      else
        VisTypedEngineObject_cl::SerializeObjectReference(ar, NULL);
    }

    m_vAnchorPivots[0].SerializeAsVisVector (ar);
    m_vAnchorPivots[1].SerializeAsVisVector (ar);

    ar << m_pszPathKey;

    ar << m_fLinkLength;
    ar << m_fLinkGap;
    ar << m_fDiameter;
    
    // Version 3: The two following replace m_fLinkMass
    ar << m_fMaterialDensity;
    ar << m_fMaterialStretchingConstant;

    ar << m_fChainDamping;

    // Version 2
    ar << m_fLinkLinearDamping;
    ar << m_fLinkAngularDamping;

    ar << m_iLinkCollisionGroup;
    ar << m_iLinkCollisionLayer;
    ar << m_iLinkSubSystemId;
    ar << m_iLinkSubSystemDontCollideWith;

    // Version 3
    ar << m_fCfmFactor;

    // Version 4
    ar << m_bCalculateLinkInertia;
    m_vInertiaTensorAxisScaling.SerializeAsVisVector (ar);
    ar << m_fInertiaTensorUniformScaling;

    // Version 5
    ar << m_bComputeCfm;
    ar << m_fFixedCfm;
    ar << m_bUseAngularFriction;
    ar << m_fFrictionFactorMin;
    ar << m_fFrictionFactorMax;
    ar << m_fBaseTension;
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainDesc::SetPathKey(const char* pszPathKey)
{
  if (m_pszPathKey)
    V_SAFE_STRFREE(m_pszPathKey);
  if (pszPathKey)
    m_pszPathKey = vStrDup(pszPathKey);
}



// ----------------------------------------------------------------------------
// vHavokConstraintChain
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokConstraintChain, VisTypedEngineObject_cl, 0, &g_vHavokModule);

template<> DynArray_cl<vHavokConstraintChain *> vHavokElementManager<vHavokConstraintChain *>::elementTable(0, NULL);
template<> unsigned int vHavokElementManager<vHavokConstraintChain *>::g_iElementCount = 0;
template<> unsigned int vHavokElementManager<vHavokConstraintChain *>::g_iFreeElementCache[VHAVOK_ELEMENTMANAGER_FREE_INDEX_CACHE_SIZE] = { 0 };
template<> unsigned int vHavokElementManager<vHavokConstraintChain *>::g_iNumElementsInCache = 0;
template<> unsigned int vHavokElementManager<vHavokConstraintChain *>::g_iCurrentElementCacheIndex = 0;
template<> unsigned int vHavokElementManager<vHavokConstraintChain *>::g_iResizeGranularity = 64;

// ----------------------------------------------------------------------------
const unsigned int vHavokConstraintChain::s_iSerialVersion = 2;

// ----------------------------------------------------------------------------
void vHavokConstraintChainListenerProxy::constraintDeletedCallback(hkpConstraintInstance* constraint)
{
	m_owner->constraintDeletedCallback( constraint ); 
}

// ----------------------------------------------------------------------------
vHavokConstraintChain::vHavokConstraintChain()
{
  m_listener.setOwner(this);

  m_pConstraintChainData = NULL;
  m_pConstraintChain = NULL;
  m_bDisposed = false;
  m_pSerializationDesc = NULL;
  m_LinkBodies.SetDefaultValue(NULL);
  
  m_fApproximationError = 0.f;
  
  m_fUnstretchedLength = 0.f;
  m_fLastLinkExtra = 0.f;

  AddToElementManager();

  m_pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(m_pModule);
}

// ----------------------------------------------------------------------------
vHavokConstraintChain::~vHavokConstraintChain()
{
  DisposeObject();
  V_SAFE_DELETE(m_pSerializationDesc);
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::AddChainLink()
{
  VVERIFY_OR_RET_VAL(m_pConstraintChain, false);
  
  hkpWorld *pWorld = m_pModule->GetPhysicsWorld();
  VVERIFY_OR_RET_VAL(pWorld, false);

  if (GetNumLinks() == GetMaxNumLinks())
  {
    Vision::Error.Warning("Cannot add link - chain already has maximum length.");
    return false;
  }
  if (GetNumLinks() < 1)
  {
    Vision::Error.Warning("Cannot add link - at least one existing link is needed as reference.");
    return false;
  }

  // We need to position the newly created link. Calculate a transform that puts
  // it exactly one link length beyond the last link of the current chain.
  hkvVec3 vLastLinkPos;
  hkvMat3 mRot;
  VVERIFY(GetLinkTransform(GetNumLinks() - 1, mRot, vLastLinkPos));
  hkvVec3 vNewLinkPos = (mRot * hkvVec3(m_fLinkLength, 0.f, 0.f)) + vLastLinkPos;
  
  // Create the chain link rigid body; add it to the physics world and our
  // internal collection
  hkpRigidBody *pLinkBody = CreateChainLink(vNewLinkPos, mRot);
  m_LinkBodies[m_iNumLinks++] = pLinkBody;
  pWorld->addEntity(pLinkBody);

  // Add the new link body in place of the last anchor; shifting the anchor
  // one position up.
  int iLastBodyIndex = m_pConstraintChain->m_chainedEntities.getSize() - 1;
  m_pConstraintChain->m_chainedEntities.insertAt(iLastBodyIndex, pLinkBody);
  pLinkBody->addReference();

  // Get the currently last constraint info index
  int iLastInfoIndex = GetConstraintInfoCount() - 1;

  // Add an additional constraint info (also as a 'normal' link). This will
  // later be corrected with the adjusted link offset and the anchor offset.
  AppendConstraintInfo(CalcLinkPivot(0), CalcLinkPivot(1), false);
  AppendRagdollConstraint(iLastInfoIndex+1);

  // Update the former last constraint info to be a 'normal' link again.
  UpdateConstraintInfo(iLastInfoIndex, CalcLinkPivot(0), CalcLinkPivot(1));
  UpdateRagdollConstraint(iLastInfoIndex);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::AdjustLastChainLinkPivot()
{
  float fAllSegmentLength = m_fLinkLength * m_iNumLinks;
  m_fLastLinkExtra = m_fUnstretchedLength - fAllSegmentLength;

  hkvVec3 vLastLinkB((m_fLinkLength * 0.5f) + m_fLastLinkExtra, 0.f, 0.f);
  UpdateConstraintInfo(GetConstraintInfoCount() - 1, vLastLinkB, m_vAnchorPivots[1]);
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::AppendConstraintInfo(const hkvVec3& vPivotA, 
  const hkvVec3& vPivotB, bool bIsAnchor)
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::AppendRagdollConstraint(unsigned int iConstraintIndex)
{
  if (!m_bUseAngularFriction || !SupportsRagdollConstraints())
    return;

  VASSERT(m_ragdollConstraintInfos.getSize() == iConstraintIndex);
  
  vHavokRagdollConstraintInfo &info = m_ragdollConstraintInfos.expandOne();
  memset(&info, 0, sizeof(vHavokRagdollConstraintInfo));
  FillRagdollConstraintInfo(info, iConstraintIndex);

  if (info.m_instance)
  {
    m_pModule->GetPhysicsWorld()->markForWrite();
    m_pModule->GetPhysicsWorld()->addConstraint(info.m_instance);
    m_pModule->GetPhysicsWorld()->unmarkForWrite();
  }
  info.m_sourceChain = GetHkConstraintChainInstance();
  info.m_sourceInfo = iConstraintIndex;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::CalcLinkError(hkvVec3 const& vEndAnchor,
  DynObjArray_cl<hkvVec3> const& pivots, unsigned int iNumPivots)
{
  hkvVec3 vLastPivot = pivots.Get(iNumPivots - 1);
  m_fLastLinkExtra = (vLastPivot - vEndAnchor).getLength();
}

// ----------------------------------------------------------------------------
hkvVec3 vHavokConstraintChain::CalcLinkPivot(unsigned int iIndex)
{
  VASSERT_MSG(FALSE,  "This should never be called - override this method in subclasses!");
  return hkvVec3::ZeroVector ();
}

// ----------------------------------------------------------------------------
unsigned int vHavokConstraintChain::CalcNumLinksRequired(float fLength)
{
  VVERIFY_OR_RET_VAL(m_fLinkLength != 0.f, 0);

  float fToAdd = 1.f - GetLastLinkAllowedGap();
  if ((fToAdd > 0.f) && (fToAdd < 1.f))
    fLength += (fToAdd * m_fLinkLength);
  return (unsigned int)(fLength / m_fLinkLength);
}

// ----------------------------------------------------------------------------
unsigned int vHavokConstraintChain::CalcPivotPoints(DynObjArray_cl<hkvVec3>& pivots)
{
  m_fLastLinkExtra = 0.f;
  m_fApproximationError = 0.f;
  pivots.Reset();

  int iNumPivots;
  VisPath_cl* pPath = Vision::Game.SearchPath(m_sChainPathKey);
  if (pPath)
  {
    iNumPivots = CalcPivotPointsFromPath(pivots, pPath);
  }
  else
  {
    iNumPivots = CalcPivotPointsFromAnchors(pivots);
  }

  return iNumPivots;
}

// ----------------------------------------------------------------------------
unsigned int vHavokConstraintChain::CalcPivotPointsFromAnchors(DynObjArray_cl<hkvVec3>& pivots)
{
  // Determine the positions of the anchors in world space
  hkvVec3 vWorldSpaceAnchors[2];
  for (unsigned int i = 0; i < 2; ++i)
  {
    VisTypedEngineObject_cl *pOwnerObj = m_spAnchorBodies[i] != NULL
      ? m_spAnchorBodies[i]->GetOwner()
      : NULL;

    VisBaseEntity_cl *pOwnerEntity = (pOwnerObj && pOwnerObj->IsOfType(VisBaseEntity_cl::GetClassTypeId()))
      ? static_cast<VisBaseEntity_cl*>(pOwnerObj)
      : NULL;

    if (pOwnerEntity != NULL)
    {
      hkvMat3 mRot = pOwnerEntity->GetRotationMatrix();
      hkvVec3 vTrans = pOwnerEntity->GetPosition();
      vWorldSpaceAnchors[i] = vTrans + (mRot * m_vAnchorPivots[i]);
    }
    else
    {
      vWorldSpaceAnchors[i] = m_vAnchorPivots[i];
    }
  }

  // Determine how many links this chain is going to have
  hkvVec3 vWorldSpaceDirection = vWorldSpaceAnchors[1] - vWorldSpaceAnchors[0];
  float fLength = vWorldSpaceDirection.getLength();
  unsigned int iNumLinks = (unsigned int)(fLength / m_fLinkLength);
  vWorldSpaceDirection.normalizeIfNotZero();

  // Calculate and fill in the pivot positions
  unsigned int iNumPivots = 0;
  pivots[iNumPivots++] = vWorldSpaceAnchors[0];
  for (unsigned int i = 1; i <= iNumLinks; ++i)
  {
    hkvVec3 vPivot = vWorldSpaceAnchors[0] + (vWorldSpaceDirection * (m_fLinkLength * i));
    pivots[iNumPivots++] = vPivot;
  }

  // Calculate the last link mismatch
  CalcLinkError(vWorldSpaceAnchors[1], pivots, iNumPivots);

  return iNumPivots;
}

// ----------------------------------------------------------------------------
unsigned int vHavokConstraintChain::CalcPivotPointsFromPath(DynObjArray_cl<hkvVec3>& pivots, VisPath_cl* pPath)
{
  float fPathLength = pPath->GetLen();
  if (fPathLength <= 0)
    return 0;

  // Sampling interval is 10 samples per link (on a linear path)
  float fPathStepT = m_fLinkLength / fPathLength / 10.f; 
  // Epsilon is the difference in t at which we stop approximation
  const float fEpsilonT = fPathStepT / 100.f;

  unsigned int iNumPivots = 0;

  // We already know the first pivot point: the start of the path
  hkvVec3 vCurrentPivot;
  pPath->EvalPoint(0.f, vCurrentPivot);
  pivots[iNumPivots++] = vCurrentPivot;

  // We start at t=0
  float fCurrentT = 0.f;
  float fPrevT = 0.f;
  float fPrevPivotT = 0.f;

  while (true)
  {
    hkvVec3 vPrevPivot = vCurrentPivot;
    fPrevPivotT = fCurrentT;

    // Coarse approximation: search until the distance becomes larger 
    // than the desired link length
    bool bDone = false;
    float fDistance = 0.f;
    while (fDistance < m_fLinkLength)
    {
      // If we start an iteration and we are already at the end of the
      // path, break immediately: we're done.
      if (fCurrentT == 1.0)
      {
        bDone = true;
        break;
      }

      // Go further along the path
      fPrevT = fCurrentT;
      fCurrentT += fPathStepT;
      if (fCurrentT > 1.f)
        fCurrentT = 1.f;

      // Evaluate the point at which we are
      pPath->EvalPointSmooth(fCurrentT, vCurrentPivot);
      fDistance = (vCurrentPivot - vPrevPivot).getLength();
    }

    // If the coarse approximation says we can't get another link in there,
    // believe it and stop here.
    if (bDone)
    {
      vCurrentPivot = vPrevPivot;
      break;
    }

    // Fine approximation: get closer in binary steps, until delta t
    // becomes smaller than epsilon t. Start in the middle between the
    // previous and the current t value.
    while (true)
    {
      float fDeltaT = (fCurrentT - fPrevT)  / 2.f;
      VASSERT(fDeltaT >= 0);
      if (fDeltaT < fEpsilonT)
        break;

      // Evaluate where we are. If the distance at our test point is
      // smaller than the link length, continue at the half with the
      // bigger t values; otherweise, continue with smaller t values,
      // for which prev and current t are already set correctly.
      fCurrentT = fPrevT + fDeltaT;
      pPath->EvalPointSmooth(fCurrentT, vCurrentPivot);
      fDistance = (vCurrentPivot - vPrevPivot).getLength();
      if (fDistance < m_fLinkLength)
      {
        fPrevT = fCurrentT;
        fCurrentT += fDeltaT;
        VASSERT(fCurrentT <= 1.f);
      }
    }

    // We can assume we have found the best approximation for the next
    // pivot point. Add it to our list.
    pivots[iNumPivots++] = vCurrentPivot;

    // Calculate the approximation error of this link by comparing the length
    // of the chain link against the length of the actual path in between
    float fPivotDistance = (vCurrentPivot - vPrevPivot).getLength();
    float fPathDistance = 0;
    {
      const unsigned int iNumSteps = 20;
      float fEvalDeltaT = (fCurrentT - fPrevPivotT) / (float)iNumSteps;
      for (unsigned int i = 0; i < iNumSteps; ++i)
      {
        hkvVec3 v1, v2;
        pPath->EvalPointSmooth(fPrevPivotT + (fEvalDeltaT * (float)i), v1);
        pPath->EvalPointSmooth(fPrevPivotT + (fEvalDeltaT * (float)(i+1)), v2);
        fPathDistance += (v2 - v1).getLength();
      }
    }

    m_fApproximationError += hkvMath::Abs (fPivotDistance - fPathDistance) / m_fLinkLength;
  }

  // Calculate the average approximation error.
  m_fApproximationError /= (float)(iNumPivots - 1);

  // Calculate the last link mismatch
  {
    hkvVec3 vEndAnchor;
    pPath->EvalPoint(1.f, vEndAnchor);
    CalcLinkError(vEndAnchor, pivots, iNumPivots);
  }

  return iNumPivots;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::constraintDeletedCallback(hkpConstraintInstance* constraint)
{
  VVERIFY_OR_RET(m_pConstraintChain);
  
  m_pConstraintChain->removeConstraintListener(&m_listener);
  m_pConstraintChain->removeReference();
  m_pConstraintChain = NULL;
}

// ----------------------------------------------------------------------------
hkpRigidBody *vHavokConstraintChain::CreateChainLink(
  hkvVec3 vPivot1, hkvVec3 vPivot2)
{
  hkvVec3 vDirection = vPivot2 - vPivot1;
  hkvMat3 mRotation;
  mRotation.setLookInDirectionMatrix (vDirection);

  hkvVec3 vPosition = vPivot1 + (vDirection * 0.5f);

  return CreateChainLink(vPosition, mRotation);
}

// ----------------------------------------------------------------------------
hkpRigidBody *vHavokConstraintChain::CreateChainLink(
  hkvVec3 const& vPosition,   hkvMat3 const& mRotation)
{
  float fRadius = m_fDiameter / 2.f;

  float fLengthEffective = m_fLinkLength - m_fLinkGap - m_fDiameter;
  if (fLengthEffective <= 0)
    fLengthEffective = float(HK2VIS_FLOAT_SCALED(0.1f));

  hkvVec3 vCap2Effective = hkvVec3(fLengthEffective / 2.f, 0, 0);
  hkvVec3 vCap1Effective = -vCap2Effective;
  float fRadiusHk = float(VIS2HK_FLOAT_SCALED(fRadius));

  hkpRigidBodyCinfo bodyInfo;
  {
	  hkVector4 c0; vHavokConversionUtils::VisVecToPhysVecLocal(vCap1Effective, c0);
	  hkVector4 c1; vHavokConversionUtils::VisVecToPhysVecLocal(vCap2Effective, c1);
	  bodyInfo.m_shape = new hkpCapsuleShape(c0,c1, fRadiusHk);
  }

  bodyInfo.m_motionType = hkpMotion::MOTION_SPHERE_INERTIA;
  bodyInfo.m_qualityType = HK_COLLIDABLE_QUALITY_DEBRIS;

  float fVolumeHk = hkvMath::pi () * (fRadiusHk*fRadiusHk) * float(VIS2HK_FLOAT_SCALED(m_fLinkLength));
  float fMass = fVolumeHk * m_fMaterialDensity;

  if (m_bCalculateLinkInertia)
    hkpInertiaTensorComputer::setShapeVolumeMassProperties(bodyInfo.m_shape, fMass, bodyInfo);
  
  bodyInfo.m_inertiaTensor.mul(m_fInertiaTensorUniformScaling);
  for (unsigned int i = 0; i < 3; ++i)
    bodyInfo.m_inertiaTensor(i, i) *= m_vInertiaTensorAxisScaling[i];

  bodyInfo.m_collisionFilterInfo = m_iCollisionFilterInfo;

  bodyInfo.m_linearDamping = m_fLinkLinearDamping;
  bodyInfo.m_angularDamping = m_fLinkAngularDamping;

  bodyInfo.m_solverDeactivation = hkpRigidBodyCinfo::SOLVER_DEACTIVATION_OFF;

  hkpRigidBody *pBody = new hkpRigidBody(bodyInfo);
  bodyInfo.m_shape->removeReference();

  // Move/Rotate the body
  hkTransform hkTfOut;
  vHavokConversionUtils::VisMatVecToPhysTransformWorld(mRotation, vPosition, hkTfOut);
  pBody->setTransform(hkTfOut);

  return pBody;
}

// ----------------------------------------------------------------------------
hkpConstraintChainData *vHavokConstraintChain::CreateConstraintChainData()
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return NULL;
}

// ----------------------------------------------------------------------------
vHavokConstraintChainDesc *vHavokConstraintChain::CreateConstraintChainDesc()
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return NULL;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::DisposeObject()
{
  if (m_bDisposed)
    return;

  Reset();
  RemoveFromElementManager();
  m_bDisposed = true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::FillRagdollConstraintInfo(
  vHavokRagdollConstraintInfo &ragdollInfo, unsigned int iConstraintIndex)
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
}

// ----------------------------------------------------------------------------
vHavokRigidBody* vHavokConstraintChain::GetAnchorBody(unsigned int iIndex) const
{
  return (iIndex < 2)
    ? m_spAnchorBodies[iIndex]
  : NULL;
}

// ----------------------------------------------------------------------------
hkvVec3 vHavokConstraintChain::GetAnchorPivot(unsigned int iIndex) const
{
  return (iIndex < 2)
    ? m_vAnchorPivots[iIndex]
  : hkvVec3::ZeroVector ();
}

// ----------------------------------------------------------------------------
unsigned int vHavokConstraintChain::GetConstraintInfoCount()
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return 0;
}

// ----------------------------------------------------------------------------
hkVector4 vHavokConstraintChain::GetImpulseApplied() const
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return hkVector4(0.f, 0.f, 0.f, 0.f);
}

// ----------------------------------------------------------------------------
float vHavokConstraintChain::GetLastLinkAllowedGap() const
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
  return 0.5f;
}

// ----------------------------------------------------------------------------
hkpRigidBody* vHavokConstraintChain::GetLinkBody(unsigned int iIndex) const
{
  VVERIFY_OR_RET_VAL(iIndex < m_iNumLinks, NULL);
  return m_LinkBodies.Get(iIndex);
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::GetLinkTransform(unsigned int iIndex, 
  hkvMat3 &mRotation, hkvVec3& vTranslation) const
{
  if (!m_pConstraintChain || (iIndex >= m_iNumLinks) || !m_LinkBodies.Get(iIndex))
    return false;

  // Get the transformation from Havok
  const hkTransform &hkTrafo = m_LinkBodies.Get(iIndex)->getTransform();
  vHavokConversionUtils::PhysTransformToVisMatVec(hkTrafo, mRotation, vTranslation);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::GetLinkTransforms(hkvMat3 *pRotations, 
  hkvVec3* pTranslations, bool bRemoveRoll, bool bReverseOrder) const
{
  // Helper to detect link direction reversal when removing roll
  bool bDirection = false;

  hkvMat3 mReverseRot (hkvNoInitialization);
  if (bReverseOrder)
    mReverseRot.setFromEulerAngles (0, 0, 180);
  else
    mReverseRot.setIdentity ();

  for (unsigned int i = 0; i < m_iNumLinks; ++i)
  {
    hkvMat3 &mRot = pRotations[i];
    hkvVec3& vTrans = pTranslations[i];
    unsigned int iLink = bReverseOrder ? m_iNumLinks - i - 1 : i;
    GetLinkTransform(iLink, mRot, vTrans);

    if (bReverseOrder)
      mRot = mRot.multiply (mReverseRot);

    if ((i > 0) && bRemoveRoll)
      RemoveLinkRoll(pRotations[i-1], mRot, bDirection);
  }
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::GetParametersFromDesc(
  vHavokConstraintChainDesc const& desc)
{
  // The link dimensions must make sense
  if ((desc.m_fLinkLength - desc.m_fLinkGap) < desc.m_fDiameter)
  {
    Vision::Error.Warning("vHavokConstraintChain: Cannot create chain. Length of chain links is less than the chain diameter.");
    return false;
  }
  if (desc.m_fDiameter <= 0)
  {
    Vision::Error.Warning("vHavokConstraintChain: Cannot create chain. Chain diameter is zero or negative.");
    return false;
  }

  // Transfer the anchor information
  for (unsigned int i = 0; i < 2; ++i)
  {
    m_spAnchorBodies[i] = desc.m_spAnchorBodies[i];
    m_vAnchorPivots[i] = desc.m_vAnchorPivots[i];
  
    if (!m_spAnchorBodies[i] || !m_spAnchorBodies[i]->GetHkRigidBody())
    {
      Vision::Error.Warning("vHavokConstraintChain: Cannot create chain. Anchor body %i is missing.", i);
      return false;
    }
  }

  m_fLinkLength = desc.m_fLinkLength;
  m_fLinkGap = desc.m_fLinkGap;
  m_fDiameter = desc.m_fDiameter;

  m_fMaterialDensity = desc.m_fMaterialDensity;
  m_fMaterialStretchingConstant = desc.m_fMaterialStretchingConstant;

  m_fLinkLinearDamping = desc.m_fLinkLinearDamping;
  m_fLinkAngularDamping = desc.m_fLinkAngularDamping;

  m_bCalculateLinkInertia = desc.m_bCalculateLinkInertia;
  m_vInertiaTensorAxisScaling = desc.m_vInertiaTensorAxisScaling;
  m_fInertiaTensorUniformScaling = desc.m_fInertiaTensorUniformScaling;

  // Get the key of the path for the constraint chain. If no path is specified or the path
  // cannot be found later, the chain will be created between the two anchor points.
  m_sChainPathKey = desc.GetPathKey();

  // Calculate the collision infos for the chain links
  m_iCollisionFilterInfo = hkpGroupFilter::calcFilterInfo(
    desc.m_iLinkCollisionLayer, desc.m_iLinkCollisionGroup,
    desc.m_iLinkSubSystemId, desc.m_iLinkSubSystemDontCollideWith);

  m_bComputeCfm = desc.m_bComputeCfm;
  m_fCfmFactor = desc.m_fCfmFactor;
  m_fFixedCfm = desc.m_fFixedCfm;

  m_bUseAngularFriction = desc.m_bUseAngularFriction;
  m_fFrictionFactorMin = desc.m_fFrictionFactorMin;
  m_fFrictionFactorMax = desc.m_fFrictionFactorMax;
  m_fBaseTension = desc.m_fBaseTension;

  return true;
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::Init(vHavokConstraintChainDesc &desc)
{
  VASSERT_MSG(!m_bDisposed, "Cannot initialize an already disposed constraint chain!");
  if (m_bDisposed)
    return false;

  Reset();

  if (!GetParametersFromDesc(desc))
    return false;

  // Calculate the pivot points
  DynObjArray_cl<hkvVec3> pivots;
  unsigned int iNumPivots = CalcPivotPoints(pivots);

  // We must have at least one link in our chain (which means, at least two
  // pivot points)
  if (iNumPivots < 2)
  {
    Vision::Error.Warning("vHavokConstraintChain: Cannot create chain. There are less than two pivot points.");
    Reset();
    return false;
  }

  // Determine the numer of links and the actual chain length
  m_iNumLinks = iNumPivots - 1;
  m_fUnstretchedLength = (m_iNumLinks * m_fLinkLength) + m_fLastLinkExtra;

  // Also don't try to create the chain if there are going to be too many
  // chain links for havok to handle.
  if (m_iNumLinks > GetMaxNumLinks())
  {
    Vision::Error.Warning("vHavokConstraintChain: The current number of links (%i) exceeds the maximum (%i).", m_iNumLinks, GetMaxNumLinks());
    Reset();
    return false;
  }

  // Create the constraint chain data for the requested constraint chain 
  // type, and let the subclass fill in all data required for the specific 
  // constraint chain type.
  m_pConstraintChainData = CreateConstraintChainData();
  VASSERT(m_pConstraintChainData);
  if (!m_pConstraintChainData)
    return false;
  InitConstraintChainDataFromDesc(*m_pConstraintChainData, desc);

  // Check the effective length of the chain link. If that's the case,
  // print a warning.
  float fLengthEffective = m_fLinkLength - m_fLinkGap - m_fDiameter;
  if (fLengthEffective <= 0)
    Vision::Error.Warning("vHavokConstraintChain: The chain links would get an effective length of 0 or less. Increase the link length or decrease the link gap and/or diameter!");

  // Add constraint infos with the pivots for anchors and chain links
  {
    hkvVec3 vPivotA = m_vAnchorPivots[0];
    hkvVec3 vPivotB = hkvVec3(-m_fLinkLength * 0.5f, 0.f, 0.f);
    AppendConstraintInfo(vPivotA, vPivotB, true);

    vPivotA = CalcLinkPivot(0);
    vPivotB = CalcLinkPivot(1);
    for (unsigned int i = 0; i < m_iNumLinks - 1; ++i)
      AppendConstraintInfo(vPivotA, vPivotB);

    vPivotA = hkvVec3(m_fLinkLength * 0.5f, 0.f, 0.f);
    vPivotB = m_vAnchorPivots[1];
    AppendConstraintInfo(vPivotA, vPivotB, true);
  }

  // Create the constraint chain itself
  m_pConstraintChain = new hkpConstraintChainInstance(m_pConstraintChainData);
  m_pConstraintChain->setUserData((hkUlong)this);
  m_pConstraintChain->addConstraintListener(&m_listener);

  {
    vHavokMarkWorld mark(true);

    // Attach the rigid bodies to the constraint chain: the anchor bodies at each
    // end, and newly created hkRigidBody:s in between.
    m_pConstraintChain->addEntity(m_spAnchorBodies[0]->GetHkRigidBody());
    for (unsigned int i = 0; i < m_iNumLinks; ++i)
    {
      hkpRigidBody *pLinkBody = 
        CreateChainLink(pivots[i], pivots[i+1]);
      m_LinkBodies[i] = pLinkBody;
      m_pModule->GetPhysicsWorld()->addEntity(pLinkBody);
      m_pConstraintChain->addEntity(pLinkBody);
    }
    m_pConstraintChain->addEntity(m_spAnchorBodies[1]->GetHkRigidBody());

    for (unsigned int i = 0; i <= m_iNumLinks; ++i)
      AppendRagdollConstraint(i);
  }

  // Set the length once to add an additional last link (if necessary) and to
  // adjust the last link's pivot points.
  SetChainLength(m_fUnstretchedLength);

  m_pModule->AddConstraintChain(this);
  m_pModule->AddStepper(this);

  OnEstablish();

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::InitConstraintChainDataFromDesc(
  hkpConstraintChainData& data, vHavokConstraintChainDesc const& desc)
{
  // Nothing to do in the base class
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::OnDeserializationCallback(const VSerializationContext &context)
{
  // The chain description must have been set by the specialized deserialization method
  // If this assertion is triggered, the deserialization in the derived chain class
  // it not implemented correctly.
  VVERIFY_OR_RET(m_pSerializationDesc != NULL);

  // Both rigid bodies used as anchors need to be there.
  VVERIFY_OR_RET(m_pSerializationDesc->m_spAnchorBodies[0] != NULL);
  VVERIFY_OR_RET(m_pSerializationDesc->m_spAnchorBodies[1] != NULL);

  // Initialize with the deserialized data - this creates the Havok constraint chain.
  Init(*m_pSerializationDesc);

  // Finally delete the joint description that was created during deserialization,
  // and release the rigid bodies we used for creating the constraint.
  V_SAFE_DELETE(m_pSerializationDesc);
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::OnEstablish()
{
  VVERIFY_OR_RET(m_pConstraintChain);

  for (unsigned int i=0; i<2; i++)
  {
    if (m_spAnchorBodies[i])
    {
      VisTypedEngineObject_cl *pOwnerThis = (m_spAnchorBodies[i] != NULL) ? m_spAnchorBodies[i]->GetOwner() : NULL;
      VisTypedEngineObject_cl *pOwnerOther = (m_spAnchorBodies[1-i] != NULL) ? m_spAnchorBodies[1-i]->GetOwner() : NULL;

      if(pOwnerThis)
        Vision::Game.SendMsg(pOwnerThis, VIS_MSG_HAVOK_ONJOINTCREATE, (INT_PTR) pOwnerThis, (INT_PTR) pOwnerOther);
    }
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::RemoveBrokenConstraintChains()
{
  ///\todo Decide whether invalid constraints (i.e., broken ones, ones with
  ///      no m_pConstraint, and ones with no m_pConstraint->getOwner()) 
  ///      shall be removed. If yes, implement this here.
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::RemoveChainLink()
{
  VVERIFY_OR_RET_VAL(m_pConstraintChain, false);

  hkpWorld *pWorld = m_pModule->GetPhysicsWorld();
  VVERIFY_OR_RET_VAL(pWorld, false);

  if (GetNumLinks() < 2)
  {
    Vision::Error.Warning("Cannot remove link - at least one link must remain.");
    return false;
  }

  // Remove the chain link rigid body from both the constraint and the physics world
  int iLastLinkBodyIndex = m_pConstraintChain->m_chainedEntities.getSize() - 2;
  pWorld->removeEntity(m_pConstraintChain->m_chainedEntities[iLastLinkBodyIndex]);
  m_pConstraintChain->m_chainedEntities.removeAtAndCopy(iLastLinkBodyIndex);

  // Remove it from our internal list as well
  m_iNumLinks--;
  m_LinkBodies[m_iNumLinks]->removeReference(); // the m_pConstraintChain ref
  m_LinkBodies[m_iNumLinks]->removeReference(); // our ref
  m_LinkBodies[m_iNumLinks] = NULL;

  // Remove the last constraint info. The new last one will be updated later to
  // reflect the correct pivot information.
  int iLastConstraintIndex = GetConstraintInfoCount() - 1;
  RemoveConstraintInfo(iLastConstraintIndex);
  RemoveRagdollConstraint(iLastConstraintIndex);
  
  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::RemoveConstraintInfo(unsigned int iIndex)
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::RemoveLinkRoll(hkvMat3 const& mPrevRot, 
  hkvMat3& mRot, bool &bDirection) const
{
  hkvMat3 mPrevRotInv(mPrevRot);
  VVERIFY(mPrevRotInv.invert() == HKV_SUCCESS);

  // Calculate the rotation of the current link in the space of the
  // previous link
  hkvMat3 mThisInPrev = mPrevRotInv.multiply (mRot);

  // Check whether we are changing direction (that is, we have a sharp angle 
  // between two links). In this case, we need to change how we adjust the roll.
  hkvVec3 v2 (mThisInPrev * hkvVec3(1, 0, 0));
  bool bChangingDirection = bDirection ? v2[0] > 0 : v2[0] < 0;
  if (bChangingDirection)
    bDirection = !bDirection;

  // Adjust the roll component
  float rgfThisInPrev[3];
  mThisInPrev.getAsEulerAngles (rgfThisInPrev[2], rgfThisInPrev[1], rgfThisInPrev[0]);

  if (bDirection)
    rgfThisInPrev[2] = 180.f;
  else
    rgfThisInPrev[2] = 0.f;

  mRot.setFromEulerAngles (rgfThisInPrev[2], rgfThisInPrev[1], rgfThisInPrev[0]);

  // Transform back to global space
  mRot = mPrevRot.multiply (mRot);
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::RemoveRagdollConstraint(unsigned int iConstraintIndex)
{
  if (!m_bUseAngularFriction || !SupportsRagdollConstraints())
    return;

  VVERIFY_OR_RET(iConstraintIndex < (unsigned int)m_ragdollConstraintInfos.getSize());
  vHavokRagdollConstraintInfo &info = m_ragdollConstraintInfos[iConstraintIndex];
  if (info.m_instance)
  {
    if (info.m_instance->getOwner())
    {
      m_pModule->GetPhysicsWorld()->markForWrite();
      m_pModule->GetPhysicsWorld()->removeConstraint(info.m_instance);
      m_pModule->GetPhysicsWorld()->unmarkForWrite();
    }
    info.m_instance->removeReference();
    info.m_instance = NULL;
  }

  m_ragdollConstraintInfos.removeAtAndCopy(iConstraintIndex);
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::Reset()
{
  m_pModule->RemoveStepper(this);

  // Remove the ragdoll constraint extensions, if any
  for (int i = 0; i < m_ragdollConstraintInfos.getSize(); ++i)
  {
    vHavokRagdollConstraintInfo &info = m_ragdollConstraintInfos[i];
    if (info.m_instance)
    {
      if (info.m_instance->getOwner())
      {
        m_pModule->GetPhysicsWorld()->markForWrite();
        m_pModule->GetPhysicsWorld()->removeConstraint(info.m_instance);
        m_pModule->GetPhysicsWorld()->unmarkForWrite();
      }
      info.m_instance->removeReference();
      info.m_instance = NULL;
    }
  }
  m_ragdollConstraintInfos.clear();

  if (m_pConstraintChain)
  {
    m_pConstraintChain->removeConstraintListener(&m_listener);
    if (m_pConstraintChain->getOwner())
      m_pModule->RemoveConstraintChain(this);
    m_pConstraintChain->removeReference();
    m_pConstraintChain = NULL;
  }
  
  if (m_pConstraintChainData)
  {
    m_pConstraintChainData->removeReference();
    m_pConstraintChainData = NULL;
  }

  m_sChainPathKey.Reset();
  
  m_iNumLinks = 0;

  for (unsigned int i = 0; i < m_LinkBodies.GetSize(); ++i)
  {
    hkpWorld *pWorld = m_pModule->GetPhysicsWorld();
    if (pWorld)
      pWorld->markForWrite();

    if (m_LinkBodies[i])
    {
      if (m_LinkBodies[i]->getWorld())
        pWorld->removeEntity(m_LinkBodies[i]);
      m_LinkBodies[i]->removeReference();
    }

    if (pWorld)
      pWorld->unmarkForWrite();
  }
  m_LinkBodies.Reset();

  for (unsigned int i = 0; i < 2; ++i)
  {
    m_spAnchorBodies[i] = 0;
    m_vAnchorPivots[i] = hkvVec3::ZeroVector ();
  }

  m_vChainForce.setZero();
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::SaveToDesc(vHavokConstraintChainDesc& desc)
{
  desc.Reset();

  for (unsigned int i = 0; i < 2; ++i)
  {
    desc.m_spAnchorBodies[i] = m_spAnchorBodies[i];
    desc.m_vAnchorPivots[i] = m_vAnchorPivots[i];
  }

  desc.SetPathKey(m_sChainPathKey);

  desc.m_fLinkLength = m_fLinkLength;
  desc.m_fLinkGap = m_fLinkGap;
  desc.m_fDiameter = m_fDiameter;
  desc.m_fMaterialDensity = m_fMaterialDensity;
  desc.m_fMaterialStretchingConstant = m_fMaterialStretchingConstant;

  desc.m_fLinkLinearDamping = m_fLinkLinearDamping;
  desc.m_fLinkAngularDamping = m_fLinkAngularDamping;

  desc.m_bCalculateLinkInertia = m_bCalculateLinkInertia;
  desc.m_vInertiaTensorAxisScaling = m_vInertiaTensorAxisScaling;
  desc.m_fInertiaTensorUniformScaling = m_fInertiaTensorUniformScaling;

  desc.m_iLinkCollisionGroup = hkpGroupFilter::getSystemGroupFromFilterInfo(m_iCollisionFilterInfo);
  desc.m_iLinkCollisionLayer = hkpGroupFilter::getLayerFromFilterInfo(m_iCollisionFilterInfo);
  desc.m_iLinkSubSystemId = hkpGroupFilter::getSubSystemIdFromFilterInfo(m_iCollisionFilterInfo);
  desc.m_iLinkSubSystemDontCollideWith = hkpGroupFilter::getSubSystemDontCollideWithFromFilterInfo(m_iCollisionFilterInfo);

  desc.m_bComputeCfm = m_bComputeCfm;
  desc.m_fCfmFactor = m_fCfmFactor;
  desc.m_fFixedCfm = m_fFixedCfm;

  desc.m_bUseAngularFriction = m_bUseAngularFriction;
  desc.m_fFrictionFactorMin = m_fFrictionFactorMin;
  desc.m_fFrictionFactorMax = m_fFrictionFactorMax;
  desc.m_fBaseTension = m_fBaseTension;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::Serialize( VArchive &ar )
{
  VisTypedEngineObject_cl::Serialize(ar);

  V_SAFE_DELETE(m_pSerializationDesc);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    ar >> m_pSerializationDesc;
    VASSERT(m_pSerializationDesc);

    if (iVersion >= 2)
      SerializeKey(ar);
  }
  else
  {
    ar << s_iSerialVersion;

    // Save constraint data to the description, then serialize that.
    m_pSerializationDesc = CreateConstraintChainDesc();
    VASSERT(m_pSerializationDesc);
    SaveToDesc(*m_pSerializationDesc);
    ar << m_pSerializationDesc;

    SerializeKey(ar);
  }
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::SetChainLength(float fLength)
{
  if (!m_pConstraintChain)
    return false;
  
  unsigned int iNumLinksRequired = CalcNumLinksRequired(fLength);
  if (iNumLinksRequired < 1)
  {
    Vision::Error.Warning("Can't reduce chain to less than 1 link.");
    return false;
  }
  if (iNumLinksRequired > GetMaxNumLinks())
  {
    Vision::Error.Warning("Can't extend chain to more than the maximum number of links (%i).", GetMaxNumLinks());
    return false;
  }

  vHavokMarkWorld markWrite(true);
  TempRemoveFromWorld chainRemove(*m_pConstraintChain);

  while (m_iNumLinks > iNumLinksRequired)
  {
    if (!RemoveChainLink())
      return false;
  }
  while (m_iNumLinks < iNumLinksRequired)
  {
    if (!AddChainLink())
      return false;
  }

  m_fUnstretchedLength = fLength;
  AdjustLastChainLinkPivot();
  UpdateChainPhysicsProperties();

  // Activate the anchor rigid bodies to make the chain reflect the
  // changes just made.
  for (unsigned int i = 0; i < 2; ++i)
    m_spAnchorBodies[i]->GetHkRigidBody()->activate();

  return true;
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::SetLinkTransform(unsigned int iIndex, 
  hkvMat3 &mRotation, hkvVec3& vTranslation)
{
  if (!m_pConstraintChain || (iIndex >= m_iNumLinks) || !m_LinkBodies.Get(iIndex))
    return false;

  // Set the transformation int the Havok rigid body
  hkTransform hkTrafo;
  vHavokConversionUtils::VisMatVecToPhysTransformWorld(mRotation, vTranslation, hkTrafo);
  m_LinkBodies.Get(iIndex)->setTransform(hkTrafo);

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::Step(float dt)
{
  if (m_bUseAngularFriction)
    UpdateRagdollConstraintState();

  m_vChainForce.setZero();
  if (!m_pConstraintChain || !m_pConstraintChain->getOwner() || !m_iNumLinks)
    return;

  if (dt > 0.f)
  {
    hkVector4 hkChainForce = GetImpulseApplied();
	hkChainForce.mul(hkSimdReal::fromFloat(dt).reciprocal());
	vHavokConversionUtils::PhysVecToVisVec_noscale(hkChainForce, m_vChainForce);
  }
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChain::SupportsRagdollConstraints() const
{
  return false;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::UpdateChainPhysicsProperties()
{
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::UpdateConstraintInfo(unsigned int iIndex, 
  const hkvVec3& vPivotA, const hkvVec3& vPivotB)
{
  VASSERT_MSG(FALSE, "This should never be called - override this method in subclasses!");
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::UpdateRagdollConstraint(unsigned int iConstraintIndex)
{
  if (!m_bUseAngularFriction || !SupportsRagdollConstraints())
    return;

  VVERIFY_OR_RET(iConstraintIndex < (unsigned int)m_ragdollConstraintInfos.getSize());
  VVERIFY_OR_RET(iConstraintIndex < GetConstraintInfoCount());
  
  vHavokRagdollConstraintInfo &info = m_ragdollConstraintInfos[iConstraintIndex];
  if (info.m_instance)
  {
    if (info.m_instance->getOwner())
    {
      m_pModule->GetPhysicsWorld()->markForWrite();
      m_pModule->GetPhysicsWorld()->removeConstraint(info.m_instance);
      m_pModule->GetPhysicsWorld()->unmarkForWrite();
    }
    info.m_instance->removeReference();
    info.m_instance = NULL;
  }

  FillRagdollConstraintInfo(info, iConstraintIndex);

  if (info.m_instance)
  {
    m_pModule->GetPhysicsWorld()->markForWrite();
    m_pModule->GetPhysicsWorld()->addConstraint(info.m_instance);
    m_pModule->GetPhysicsWorld()->unmarkForWrite();
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraintChain::UpdateRagdollConstraintState()
{
  // intentionally left empty; must be overridden in derived classes
  // that support the ragdoll extension
}


// ----------------------------------------------------------------------------
// vHavokConstraintChain::TempRemoveFromWorld
// ----------------------------------------------------------------------------
vHavokConstraintChain::TempRemoveFromWorld::TempRemoveFromWorld(
  hkpConstraintChainInstance &chain)
: chain(chain), bWasInWorld(false)
{
  pWorld = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
  if (!pWorld)
    return;

  pWorld->addReference();

  if (chain.getOwner())
  {
    pWorld->removeConstraint(&chain);
    bWasInWorld = true;
  }
}

// ----------------------------------------------------------------------------
vHavokConstraintChain::TempRemoveFromWorld::~TempRemoveFromWorld()
{
  if (!pWorld)
    return;

  if (bWasInWorld)
    pWorld->addConstraint(&chain);

  pWorld->removeReference();
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
