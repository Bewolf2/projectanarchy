/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokChainAnimation.hpp>

#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>


// ----------------------------------------------------------------------------
// vHavokChainAnimation
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokChainAnimation, IVObjectComponent, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokChainAnimation::s_iSerialVersion = 2;

// ----------------------------------------------------------------------------
const unsigned int vHavokChainAnimation::InvalidIndex = (unsigned int)(-1);


// ----------------------------------------------------------------------------
vHavokChainAnimation::vHavokChainAnimation(
  int iComponentFlags/* =VIS_OBJECTCOMPONENTFLAG_NONE */)
  : IVObjectComponent((const char*)NULL, iComponentFlags)
{
  m_iConstraintChainIndex = InvalidIndex;
  m_pOwnerEntity = NULL;
}

// ----------------------------------------------------------------------------
vHavokChainAnimation::~vHavokChainAnimation()
{
  CommonDeinit();
}

// ----------------------------------------------------------------------------
BOOL vHavokChainAnimation::CanAttachToObject(
  VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
  {
    sErrorMsgOut = "Component can only be added to entities.";
    return FALSE;
  }

  return TRUE;
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::CommonDeinit()
{
  Vision::Callbacks.OnUpdateSceneFinished -= this;

  m_iConstraintChainIndex = InvalidIndex;
  m_pOwnerEntity = NULL;
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::CommonInit()
{
  CommonDeinit();

  if (!m_pOwner || !m_pOwner->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl)))
    return;

  Vision::Callbacks.OnUpdateSceneFinished += this;

  m_pOwnerEntity = static_cast<VisBaseEntity_cl*>(m_pOwner);
  VisAnimConfig_cl *pAnimConfig = m_pOwnerEntity->GetAnimConfig();

  // If there is no AnimConfig, but we can get a skeleton, create the AnimConfig.
  if (!pAnimConfig)
  {
    VDynamicMesh *pMesh = m_pOwnerEntity->GetMesh();
    VisSkeleton_cl *pSkeleton = pMesh ? pMesh->GetSkeleton() : NULL;
    if (pSkeleton)
    {
      VisAnimFinalSkeletalResult_cl* pFinalSkeletalResult;
      pAnimConfig = VisAnimConfig_cl::CreateSkeletalConfig(pMesh, &pFinalSkeletalResult);
      m_pOwnerEntity->SetAnimConfig(pAnimConfig);
    }
  }
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::DisposeObject()
{
  CommonDeinit();
  IVObjectComponent::DisposeObject();
}

// ----------------------------------------------------------------------------
vHavokConstraintChain *vHavokChainAnimation::GetConstraintChain()
{
  if (ConstraintChainKey.IsEmpty())
    return NULL;

  // Check whether the last known index is valid, and if yes, whether it still 
  // points to "our" chain
  if ((m_iConstraintChainIndex != InvalidIndex) && 
      (m_iConstraintChainIndex < vHavokConstraintChain::ElementManagerGetSize()))
  {
    vHavokConstraintChain *pChain = 
      vHavokConstraintChain::ElementManagerGetAt(m_iConstraintChainIndex);
    if (pChain && pChain->HasObjectKey(ConstraintChainKey))
      return pChain;
  }

  // If we weren't lucky above, our last known index is definitely invalid
  m_iConstraintChainIndex = InvalidIndex;

  // Try to find the constraint chain by name
  for (unsigned int i = 0; i < vHavokConstraintChain::ElementManagerGetSize(); i++)
  {
    vHavokConstraintChain *pChain = vHavokConstraintChain::ElementManagerGet(i);
    if (pChain->HasObjectKey(ConstraintChainKey))
    {
      // Any further checks (number of bones, etc.)
      m_iConstraintChainIndex = i;
      // Any further initialization
      return pChain;
    }
  }

  // No success
  return NULL;
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::OnDeserializationCallback(const VSerializationContext &context)
{
  // The owner constraint chain should now be established; we can init this 
  // component now.
  CommonInit();
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    UpdateBones();
  }
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  // Re-Init
  CommonInit();
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::Serialize(VArchive &ar)
{
  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT_MSG((iVersion > 0) && (iVersion <= s_iSerialVersion), "Invalid version of serialized data!");

    ar >> ConstraintChainKey;
    
    if (iVersion >= 2)
    {
      ar >> BoneRoll;
      ar >> ReverseBoneOrder;
    }
  }
  else
  {
    ar << s_iSerialVersion;
    ar << ConstraintChainKey;
    
    // Version 2
    ar << BoneRoll;
    ar << ReverseBoneOrder;
  }
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);
  CommonInit();
}

// ----------------------------------------------------------------------------
void vHavokChainAnimation::UpdateBones()
{
  // Check that everything we need is in place
  VisAnimConfig_cl *pAnimConfig = m_pOwnerEntity ? m_pOwnerEntity->GetAnimConfig() : NULL;
  VisAnimFinalSkeletalResult_cl *pSkelResult = pAnimConfig ? pAnimConfig->GetFinalResult() : NULL;
  VisSkeleton_cl *pSkeleton = pSkelResult ? pSkelResult->GetSkeleton() : NULL;
  if (!pSkeleton)
    return;
  const unsigned int iNumBones = pSkeleton->GetBoneCount();
  if (iNumBones < 1)
    return;

  vHavokConstraintChain *pConstraintChain = GetConstraintChain();
  if (!pConstraintChain)
    return;
  const unsigned int iNumLinks = pConstraintChain->GetNumLinks();
  if (iNumLinks < 2)
    return;

  // Prepare transforms needed to process the constraint chain data
  hkvMat3 mWorldToEntityRot = m_pOwnerEntity->GetRotationMatrix();
  mWorldToEntityRot.invert();
  hkvVec3 vWorldToEntityTrans = m_pOwnerEntity->GetPosition();
  vWorldToEntityTrans *= -1.f;
  hkvVec3 vWorldToEntityScaling = m_pOwnerEntity->GetScaling();
  for (int i = 0; i < 3; ++i)
  {
    if (vWorldToEntityScaling.data[i] != 0.f)
        vWorldToEntityScaling.data[i] = 1.f / vWorldToEntityScaling.data[i];
  }

  // Retrieve position/rotation of all links; correct the roll component
  hkvMat3 *pLinkRotations = new hkvMat3[iNumLinks];
  hkvVec3* pLinkPositions = new hkvVec3[iNumLinks];
  pConstraintChain->GetLinkTransforms(
    pLinkRotations, pLinkPositions, true, ReverseBoneOrder != FALSE);

  // Calculate the conversion factor to get from bone indices to link indices.
  // For this, we have to take several points into account:
  // - The visible chain may have to be shorter or longer than the physics 
  //   chain, due to length adjustments.
  // - To match the outermost bones to the ends of the chain, we need to use
  //   the ends of the chain links as reference positions (instead of their 
  //   centers). We achieve this by assuming the chain to have one more link,
  //   and then shifting the on-chain position of a bone by half a link length.
  float fLastLinkExtraFractions = 
    pConstraintChain->GetLastLinkExtra() / pConstraintChain->GetLinkLength();
  float fBoneToLinkFactor = 
    ((float)iNumLinks + 1.f + fLastLinkExtraFractions) / (float)iNumBones;

  // Computes the roll angle interpolation along the chaing for each bone
  // depending on the two anchor entities attached
  float boneRollStep = 0.0f; // delta roll for each bone
  float acumBoneRoll = 0.0f; // acum roll for each bone
  float rollStart,rollEnd; // will store the anchors' roll-angles
  {
    vHavokRigidBody* pRBody;
    VisObject3D_cl* pObj3D;
    hkvMat3 mRot(hkvNoInitialization);
    float angles[3];

    // Retrieve roll angles from the two anchors anchors

    hkvMat3 mReverseRot;
    if (ReverseBoneOrder)
      mReverseRot.setFromEulerAngles (0, 0, 180);
    else
      mReverseRot.setIdentity ();

    int i = 0;
    for ( ; i < 2; ++i )
    {
      // store roll in start or end roll
      float* pRoll = ( i==0 ) ? &rollStart : &rollEnd;

      // retrieve anchor's owner 3d
      pRBody = pConstraintChain->GetAnchorBody(i);
      if ( ! pRBody ) break;
      pObj3D = pRBody->GetOwner3D();
      if ( ! pObj3D ) break;

      // get roll angle from rotation matrix
      pObj3D->GetRotationMatrix( mRot );
      mRot.getAsEulerAngles (angles[2], angles[1], angles[0]);

      *pRoll = angles[0] + ReverseBoneOrder*180.0f;
    }

    // only if the two roll angles have been correctly retrieved
    if ( i == 2 )
    {
      // angle between two anchors
      float rollDiff = rollEnd - rollStart;
      
      // adjust the [-pi,pi] issue in roll. Gets the minimum angle with correct sign.
      if      ( rollDiff >= 180.0f )  rollDiff -= 360.0f;
      else if ( rollDiff <= -180.0f ) rollDiff += 360.0f;

      // delta depends on number of bones
      boneRollStep = rollDiff / iNumBones;

      acumBoneRoll = ReverseBoneOrder ? rollDiff : 0;
    }
  }
  // Iterate over all bones, find the link to use (or the links to interpolate
  // between), transform to entity space and apply to the bones.
  for (unsigned int iBone = 0; iBone < iNumBones; iBone++)
  {
    unsigned int iFirstLink, iSecondLink;
    float fInterLinkT;

    // Determine indices and balance for the links to use
    float fChainT = (iBone * fBoneToLinkFactor) - 0.5f;
    if (ReverseBoneOrder != FALSE)
      fChainT -= fLastLinkExtraFractions;

    if (fChainT <= 0)
    {
      iFirstLink = 0;
      iSecondLink = 1;
      fInterLinkT = hkvMath::Max(fChainT, -1.5f);
    }
    else if (fChainT >= (iNumLinks - 1))
    {
      iFirstLink = (iNumLinks - 2);
      iSecondLink = (iNumLinks - 1);
      fInterLinkT = hkvMath::Min(fChainT - iFirstLink, 1.5f);
    }
    else
    {
      iFirstLink = (unsigned int)floorf(fChainT);
      iSecondLink = iFirstLink + 1;
      fInterLinkT = fChainT - iFirstLink;
    }

    // Get the link transforms and transform to entity space (the scaling needs
    // to be removed as well, as the entity's world transform applies it again)
    hkvVec3 vPos1(pLinkPositions[iFirstLink]);
    hkvVec3 vPos2(pLinkPositions[iSecondLink]);
    hkvMat3 mRot1(pLinkRotations[iFirstLink]);
    hkvMat3 mRot2(pLinkRotations[iSecondLink]);

    mRot1 = mWorldToEntityRot.multiply (mRot1);
    vPos1 = mWorldToEntityRot * (vPos1 + vWorldToEntityTrans).compMul (vWorldToEntityScaling);

    mRot2 = mWorldToEntityRot.multiply (mRot2);
    vPos2 = mWorldToEntityRot * (vPos2 + vWorldToEntityTrans).compMul (vWorldToEntityScaling);

    // Interpolate the bone position
    hkvVec3 vPosFinal = vPos1 + ((vPos2 - vPos1) * fInterLinkT);

    // Interpolate the bone rotation
    hkvQuat qRot1, qRot2, qRotFinal;
    qRot1.setFromMat3 (mRot1);
    qRot2.setFromMat3 (mRot2);

    // If interpolation would have to take the "long way" (happens when the dot product
    // is negative), negate one of the quaternions to prevent interpolation artifacts.
    if (qRot1.dot (qRot2) < 0.f)
      qRot1.flipSigns ();

    qRotFinal.setSlerp (qRot1, qRot2, hkvMath::Min(1.f, hkvMath::Max(0.f, fInterLinkT)));
    qRotFinal.normalize();

    // Applied final roll will be the BoneRoll property value to be rotated all bones
    // and the accumulated roll for each bone from the interpolation between anchors
    float finalRoll = -acumBoneRoll+ BoneRoll + ReverseBoneOrder*180.0f;
    if ( fabs(finalRoll) > 0.0001f )
    {
      hkvQuat qRoll (hkvNoInitialization);
      qRoll.setFromEulerAngles(finalRoll, 0.f, 0.f );
      qRotFinal = qRoll.multiplyReverse (qRotFinal);
    }
    acumBoneRoll += (ReverseBoneOrder) ? -boneRollStep : boneRollStep;

    // Apply to the bone
    pSkelResult->SetCustomBoneTranslation(iBone, vPosFinal);
    pSkelResult->SetCustomBoneRotation(iBone, qRotFinal);

    // For debugging: visualize bone orientation
#if 0
    {
      hkvMat4 mWorld;
      m_pOwnerEntity->GetWorldMatrix(mWorld);
      
      hkvMat3 mRotFinal;
      qRotFinal.ToMatrix(mRotFinal);

      const float fLength = 5.f;
      const float fWidth = 1.f;

      {
        hkvVec3 vLineStart(0.f, 0.f, 0.f);
        hkvVec3 vLineEnd(fLength, 0.f, 0.f);
        hkvVec3 vHead1(fLength-fWidth, fWidth, 0.f);
        hkvVec3 vHead2(fLength-fWidth, -fWidth, 0.f);

        vLineStart = ((vLineStart * mRotFinal) + vPosFinal) * mWorld;
        vLineEnd = ((vLineEnd * mRotFinal) + vPosFinal) * mWorld;
        vHead1 = ((vHead1 * mRotFinal) + vPosFinal) * mWorld;
        vHead2 = ((vHead2 * mRotFinal) + vPosFinal) * mWorld;

        Vision::Game.DrawSingleLine(vLineStart, vLineEnd, V_RGBA_BLUE);
        Vision::Game.DrawSingleLine(vLineEnd, vHead1, V_RGBA_BLUE);
        Vision::Game.DrawSingleLine(vLineEnd, vHead2, V_RGBA_BLUE);
        Vision::Game.DrawSingleLine(vHead1, vHead2, V_RGBA_BLUE);
      }

      {
        hkvVec3 vLineStart(0.f, 0.f, 0.f);
        hkvVec3 vLineEnd(0.f, fLength, 0.f);
        hkvVec3 vHead1(0.f, fLength-fWidth, fWidth);
        hkvVec3 vHead2(0.f, fLength-fWidth, -fWidth);

        vLineStart = ((vLineStart * mRotFinal) + vPosFinal) * mWorld;
        vLineEnd = ((vLineEnd * mRotFinal) + vPosFinal) * mWorld;
        vHead1 = ((vHead1 * mRotFinal) + vPosFinal) * mWorld;
        vHead2 = ((vHead2 * mRotFinal) + vPosFinal) * mWorld;

        Vision::Game.DrawSingleLine(vLineStart, vLineEnd, V_RGBA_GREEN);
        Vision::Game.DrawSingleLine(vLineEnd, vHead1, V_RGBA_GREEN);
        Vision::Game.DrawSingleLine(vLineEnd, vHead2, V_RGBA_GREEN);
        Vision::Game.DrawSingleLine(vHead1, vHead2, V_RGBA_GREEN);
      }

      {
        hkvVec3 vLineStart(0.f, 0.f, 0.f);
        hkvVec3 vLineEnd(0.f, 0.f, fLength);
        hkvVec3 vHead1(fWidth, 0.f, fLength-fWidth);
        hkvVec3 vHead2(-fWidth, 0.f, fLength-fWidth);

        vLineStart = ((vLineStart * mRotFinal) + vPosFinal) * mWorld;
        vLineEnd = ((vLineEnd * mRotFinal) + vPosFinal) * mWorld;
        vHead1 = ((vHead1 * mRotFinal) + vPosFinal) * mWorld;
        vHead2 = ((vHead2 * mRotFinal) + vPosFinal) * mWorld;

        VColorRef orange(255, 165, 0);
        Vision::Game.DrawSingleLine(vLineStart, vLineEnd, orange);
        Vision::Game.DrawSingleLine(vLineEnd, vHead1, orange);
        Vision::Game.DrawSingleLine(vLineEnd, vHead2, orange);
        Vision::Game.DrawSingleLine(vHead1, vHead2, orange);
      }
    }
#endif
  }

  // Calculate a new visibility bounding box for the entity:
  {
    const VisSkeletalAnimResult_cl *pAnimResult = pAnimConfig->GetCurrentObjectSpaceResult();
    VASSERT(pAnimResult);
    if (!pAnimResult)
      return;

    hkvAlignedBBox newEntityVisBox;
    hkvMat4 mEntityWorldTrans (hkvNoInitialization);
    m_pOwnerEntity->GetWorldMatrix(mEntityWorldTrans);


    for (int iBone = 0; iBone < pSkeleton->GetBoneCount(); ++iBone)
    {
      VisSkeletalBone_cl *pBone = pSkeleton->GetBone(iBone);

      hkvMat4 mBoneObjectTrans(hkvNoInitialization);
      pAnimResult->GetBoneTransformationMatrix(iBone, mBoneObjectTrans);
      hkvMat4 mBoneWorldTrans = mEntityWorldTrans;

      mBoneWorldTrans = mBoneWorldTrans.multiply (mBoneObjectTrans);

      // Inflate the new bounding box by the 8 transformed corner points 
      // of the bone bounding box.
      for (int i = 0; i < 8; ++i)
      {
        bool bx = (i & 1) > 0;
        bool by = (i & 2) > 0;
        bool bz = (i & 4) > 0;
        hkvVec3 vBBoxPt(
          bx ? pBone->m_BoundingBox.m_vMin[0] : pBone->m_BoundingBox.m_vMax[0],
          by ? pBone->m_BoundingBox.m_vMin[1] : pBone->m_BoundingBox.m_vMax[1],
          bz ? pBone->m_BoundingBox.m_vMin[2] : pBone->m_BoundingBox.m_vMax[2]);
        vBBoxPt = mBoneWorldTrans.transformPosition(vBBoxPt);
        newEntityVisBox.expandToInclude(vBBoxPt);
      }
    }
    m_pOwnerEntity->SetCurrentVisBoundingBox(newEntityVisBox);
    m_pOwnerEntity->SetPosition( newEntityVisBox.getCenter() );
  }

  V_SAFE_DELETE_ARRAY(pLinkPositions);
  V_SAFE_DELETE_ARRAY(pLinkRotations);

  // For debugging: Draw bounding boxes
#if 0
  m_pOwnerEntity->DrawBoneBoundingBoxes();
  Vision::Game.DrawBoundingBox(*m_pOwnerEntity->GetCurrentVisBoundingBoxPtr(), V_RGBA_BLUE);
#endif
}

// ----------------------------------------------------------------------------
START_VAR_TABLE(vHavokChainAnimation, IVObjectComponent, "Animates the bones of a model according to a Havok constraint chain", VVARIABLELIST_FLAGS_NONE, "Havok Constraint Chain Animation" )
  DEFINE_VAR_VSTRING(vHavokChainAnimation, ConstraintChainKey, "The object key of the constraint chain to drive the animation", "", 0,0, NULL);
  DEFINE_VAR_FLOAT(vHavokChainAnimation, BoneRoll, "The roll to apply to each bone in addition to the link transformation", "0", 0, NULL);
  DEFINE_VAR_BOOL(vHavokChainAnimation, ReverseBoneOrder, "Whether to reverse the bone order before applying the transformations", "FALSE", 0, NULL);
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
