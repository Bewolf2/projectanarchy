/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VShadowMapGenSpotDir.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/ShadowMapping/VBaseShadowMapComponentSpotDirectional.hpp>

VShadowMapGenSpotDir::VShadowMapGenSpotDir(IVRendererNode *pRendererNode, VisLightSource_cl *pRefLight, 
  IVShadowMapFormat* pShadowMapFormat, IVShadowMapComponent* pComponent, int iNumParts, int iMaxNumParts) :
  VShadowMapGenerator(pRendererNode, pRefLight, pShadowMapFormat, pComponent, iNumParts, iMaxNumParts),
  m_fFovX(0.0f), m_fFovY(0.0f), m_fNear(0.0f)
{
  // Force update by invalidating the last stored values
  m_vLastCameraPos.set(FLT_MAX,FLT_MAX,FLT_MAX);
  m_vLastCameraDir.set(FLT_MAX,FLT_MAX,FLT_MAX);
  m_vLastLightDir.set(FLT_MAX,FLT_MAX,FLT_MAX);
}

VShadowMapGenSpotDir::~VShadowMapGenSpotDir()
{
  DeInitialize();
}

bool VShadowMapGenSpotDir::Initialize()
{
  return VShadowMapGenerator::Initialize();
}

void VShadowMapGenSpotDir::Update(bool force)
{
  VisContextCamera_cl *pMainCam = m_pRendererNode->GetReferenceContext()->GetCamera();
  VASSERT(pMainCam);
  float fCamAngle = hkvMath::Abs (hkvMath::acosDeg (pMainCam->GetDirection().dot (m_vLastCameraDir)));
  hkvVec3 vCamMove = pMainCam->GetPosition() - m_vLastCameraPos;
  m_pRendererNode->GetReferenceContext()->GetViewFrustum(m_MainViewFrustum);

  float fFovX, fFovY;
  m_pRendererNode->GetReferenceContext()->GetFinalFOV(fFovX, fFovY);

  float fNear, fFar;
  m_pRendererNode->GetReferenceContext()->GetClipPlanes(fNear, fFar);

  VBaseShadowMapComponentSpotDirectional* pComponent = static_cast<VBaseShadowMapComponentSpotDirectional*>(m_pShadowComponent);

  if (m_eProjectionType == SHADOW_PROJECTION_ORTHOGRAPHIC)
  {
    bool bOverestimateCascades = pComponent->GetOverestimateCascades() ? true : false;
    if (bOverestimateCascades)
    {
      if (m_fFovX != fFovX || m_fFovY != fFovY || m_fNear != fNear)
      {
        // In case user switches the FOV at runtime we need to update it here
        float fStartDistance = fNear;
        for (int iCascade = 0; iCascade < m_iNumParts; iCascade++)
        {
          float fEndDistance = pComponent->GetCascadeRange(iCascade) + fNear;
          m_pParts[iCascade].ComputeOffset(fStartDistance, fEndDistance);
          fStartDistance = fEndDistance;
        }

        m_fFovX = fFovX;
        m_fFovY = fFovY;
        m_fNear = fNear;
        force = true;
      }

      if (force || m_vLastLightPos != m_pLightSource->GetPosition() || 
        m_vLastLightDir != m_pLightSource->GetDirection() || 
        vCamMove.getLength() > pComponent->GetShadowMapCameraUpdateInterval() || 
        fCamAngle > pComponent->GetShadowMapCameraUpdateAngle())
      {
        for (int iCascade = 0; iCascade < m_iNumParts; iCascade++)
        {
          VShadowMapPart& part = m_pParts[iCascade];
          
          VisContextCamera_cl* pCam = part.GetRenderContext()->GetCamera();
          VisContextCamera_cl *pReferenceCam = m_pRendererNode->GetReferenceContext()->GetCamera();

          hkvVec3 vCascadeCenter = pReferenceCam->GetPosition() + pReferenceCam->GetDirection() * part.m_fCenterOffset;
          pCam->SetDirection(GetDirection());

          float fRadius = part.m_fRadius + pComponent->GetShadowMapCameraUpdateInterval();

          float fCameraDistanceFromCenter = fFar;
          hkvVec3 vCamPos = vCascadeCenter - GetDirection() * fCameraDistanceFromCenter;
          pCam->SetPosition(vCamPos);

          //snapping
          hkvVec3 offset = pCam->GetWorldToCameraTransformation().getColumn(3).getAsVec3();

          float fPixelSize = (fRadius * 2) / (float)m_pShadowComponent->GetShadowMapSize();
          offset.x -= floorf(offset.x / fPixelSize) * fPixelSize;
          offset.y -= floorf(offset.y / fPixelSize) * fPixelSize;
          
          vCamPos -= pCam->GetObjDir_Right() * offset.x - pCam->GetObjDir_Up() * offset.y;
          pCam->SetPosition(vCamPos);
          
          
          // Set far clip plane so that we have z = 1 at the far side of the sphere - the shader expects this when using the BoundingBox cascade selection method.
          part.GetRenderContext()->SetClipPlanes(0.0f, fCameraDistanceFromCenter + fRadius);
          part.GetRenderContext()->SetOrthographicSize(fRadius * 2, fRadius * 2);
          part.Update();
        }

        m_vLastLightPos = m_pLightSource->GetPosition();
        m_vLastLightDir = m_pLightSource->GetDirection();
        m_vLastCameraPos = pMainCam->GetPosition();
        m_vLastCameraDir = pMainCam->GetDirection();
      }
    }
    else
    {
      float fStartDistance = 0.0f;
      for (int iCascade = 0; iCascade < m_iNumParts; iCascade++)
      {
        VShadowMapPart& part = m_pParts[iCascade];
        VisContextCamera_cl* pCam = part.GetRenderContext()->GetCamera();

        // TODO: Performance

        // Compute the extremes of the view frustum fragment for this cascade
        hkvVec3 vCorners[8];
        float fEndDistance = pComponent->GetCascadeRange(iCascade);
        part.m_fCullDistance = fEndDistance;
        part.GetViewFrustumCornersAtDistance(m_pRendererNode->GetReferenceContext(), fStartDistance, &vCorners[0]);
        part.GetViewFrustumCornersAtDistance(m_pRendererNode->GetReferenceContext(), fEndDistance,   &vCorners[4]);
        fStartDistance = fEndDistance;

        pCam->SetDirection(GetDirection());
        float fMinRight = FLT_MAX;
        float fMinUp = FLT_MAX;
        float fMaxRight = -FLT_MAX;
        float fMaxUp = -FLT_MAX;
        float fMinDist = FLT_MAX;
        float fMaxDist = -FLT_MAX;
        for (int iCorner = 0; iCorner < 8; iCorner ++ )
        {
          float fRight = pCam->GetObjDir_Right().dot (vCorners[iCorner]);
          if (fRight < fMinRight)
            fMinRight = fRight;
          if (fRight > fMaxRight)
            fMaxRight = fRight;

          float fUp = pCam->GetObjDir_Up().dot (vCorners[iCorner]);
          if (fUp < fMinUp)
            fMinUp = fUp;
          if (fUp > fMaxUp)
            fMaxUp = fUp;

          float fDist = pCam->GetDirection().dot (vCorners[iCorner]);
          if (fDist < fMinDist)
            fMinDist = fDist;
          if (fDist > fMaxDist)
            fMaxDist = fDist;
        }

        // Since we need to take geometry into account which lies in front of the view frustum, we need to
        // offset the near clip plane (in this case, the camera position) away from the view frustum
        fMinDist -= (fFar - fNear);

        hkvVec3 vCamPos = pCam->GetObjDir_Right() * ((fMinRight + fMaxRight)*0.5f) 
          + pCam->GetObjDir_Up() * ((fMinUp + fMaxUp)*0.5f)
          + pCam->GetDirection() * fMinDist;

        float fPadding = pComponent->GetShadowMapCameraUpdateInterval();
        pCam->SetPosition(vCamPos);
        part.GetRenderContext()->SetClipPlanes(0.0f, fMaxDist - fMinDist);
        part.GetRenderContext()->SetOrthographicSize(fMaxRight - fMinRight + fPadding, fMaxUp - fMinUp + fPadding);

        part.Update();
      }
    }
  }
  else if (m_eProjectionType == SHADOW_PROJECTION_PERSPECTIVE)
  {
    if (m_pLightSource->GetRadius() <= 0) //this is the case if triggered is set to false for the light
    {
      return; //do render the light at all
    }

    if (force || m_vLastLightPos != m_pLightSource->GetPosition() || m_vLastLightDir != m_pLightSource->GetDirection() ||
      m_fLastRadius != m_pLightSource->GetRadius() || m_fLastAngle != m_pLightSource->GetProjectionAngle())
    {
      VShadowMapPart& part = m_pParts[0];
      VisContextCamera_cl* pCam = part.GetRenderContext()->GetCamera();

      pCam->SetDirection(GetDirection());
      pCam->SetPosition(GetPosition());
      part.GetRenderContext()->SetClipPlanes(m_pShadowComponent->GetNearClip(), m_pLightSource->GetRadius());
      float fAngle = m_pLightSource->GetProjectionAngle();
      part.GetRenderContext()->SetFOV(fAngle, fAngle);

      part.Update();

      m_vLastLightPos = m_pLightSource->GetPosition();
      m_vLastLightDir = m_pLightSource->GetDirection();
      m_fLastRadius = m_pLightSource->GetRadius();
      m_fLastAngle = fAngle;
    }
  }

  m_iPartsRendered = 0;
}

void VShadowMapGenSpotDir::UpdateLightShader(VShadowMapApplyShaderPass *pPass, hkvMat4 (&mLightProjection)[4], hkvVec4& vCascadeScale)
{
  VShadowMapGenerator::UpdateLightShader(pPass, mLightProjection, vCascadeScale);  
  float fFadeOutStart, fFadeOutEnd;
  ComputeFadeOutParams(fFadeOutStart, fFadeOutEnd);
  pPass->SetFadeoutParameters(fFadeOutStart, fFadeOutEnd);
}

void VShadowMapGenSpotDir::ComputeFadeOutParams(float &fFadeOutStart, float &fFadeOutEnd)
{
  VBaseShadowMapComponentSpotDirectional* pComponent = static_cast<VBaseShadowMapComponentSpotDirectional*>(m_pShadowComponent);
  float fTmpFadeOutStart = pComponent->GetShadowFadeOutStart();
  float fTmpFadeOutEnd = pComponent->GetShadowFadeOutEnd();
  const float fLastCascadeRange = m_pParts[pComponent->GetCascadeCount() - 1].m_fCullDistance;
  fFadeOutStart = (fTmpFadeOutStart > 0.0f) ? fTmpFadeOutStart : fLastCascadeRange * 0.9f;
  fFadeOutEnd = (fTmpFadeOutEnd > 0.0f) ? fTmpFadeOutEnd : fLastCascadeRange;
}

float VShadowMapGenSpotDir::GetCascadeCullDistance(int iCascade) const
{
  VBaseShadowMapComponentSpotDirectional* pComponent = static_cast<VBaseShadowMapComponentSpotDirectional*>(m_pShadowComponent);
  float fCullDistance = m_pParts[iCascade].m_fCullDistance;
  if (pComponent->GetCascadeSelection() == VBaseShadowMapComponentSpotDirectional::CSM_SELECT_BY_BOUNDINGBOX)
  {
    fCullDistance *= 2;
  }
  if (iCascade == pComponent->GetCascadeCount() - 1)
  {
    fCullDistance = hkvMath::Max(fCullDistance, pComponent->GetShadowFadeOutEnd());
  }
  return fCullDistance;
}


void VShadowMapGenSpotDir::GetBoundingBoxVS(hkvAlignedBBox &boundingBox) const
{
  VASSERT(m_eProjectionType==SHADOW_PROJECTION_ORTHOGRAPHIC && m_iNumParts>0);

  if (!boundingBox.isValid())
    return;

  // transform shadow-caster bounding box with view-matrix of first cascade, since actual cascade is indifferent here 
  boundingBox.transformFromOrigin(m_pParts[0].GetCamera()->GetWorldToCameraTransformation());
}

void VShadowMapGenSpotDir::UpdateOrthoShadowVolumeVS(const hkvAlignedBBox &boundingBoxWS)
{
  VASSERT(m_eProjectionType == SHADOW_PROJECTION_ORTHOGRAPHIC);

  m_ShadowVolumeVS = boundingBoxWS;
  GetBoundingBoxVS(m_ShadowVolumeVS);
}

bool VShadowMapGenSpotDir::IsEntityInsideOrthoShadowVolume(const VisBaseEntity_cl *pEntity) const
{
  VASSERT(pEntity!=NULL && m_eProjectionType==SHADOW_PROJECTION_ORTHOGRAPHIC);

  if (!m_ShadowVolumeVS.isValid())
    return false;

  // get axis-aligned bounding box of entity in view-space of current light
  hkvAlignedBBox entityVolume;
  pEntity->GetCurrentVisBoundingBox(entityVolume);
  GetBoundingBoxVS(entityVolume);

  // do not test against far plane since shadow volume is projected to infinity
  if (entityVolume.m_vMin.x > m_ShadowVolumeVS.m_vMax.x) return false;
  if (entityVolume.m_vMin.y > m_ShadowVolumeVS.m_vMax.y) return false;
  if (entityVolume.m_vMax.x < m_ShadowVolumeVS.m_vMin.x) return false;
  if (entityVolume.m_vMax.y < m_ShadowVolumeVS.m_vMin.y) return false;
  if (entityVolume.m_vMax.z < m_ShadowVolumeVS.m_vMin.z) return false;

  return true;
}

bool VShadowMapGenSpotDir::IsMeshInsideOrthoShadowVolume(const VisStaticGeometryInstance_cl *pStaticMeshInstance) const
{
  VASSERT(pStaticMeshInstance!=NULL && m_eProjectionType == SHADOW_PROJECTION_ORTHOGRAPHIC);

  if (!m_ShadowVolumeVS.isValid())
    return false;

  // get axis-aligned bounding box of mesh in view-space of current light
  hkvAlignedBBox meshVolume = pStaticMeshInstance->GetBoundingBox();
  GetBoundingBoxVS(meshVolume);

  // do not test against far plane since shadow volume is projected to infinity
  if (meshVolume.m_vMin.x > m_ShadowVolumeVS.m_vMax.x) return false;
  if (meshVolume.m_vMin.y > m_ShadowVolumeVS.m_vMax.y) return false;
  if (meshVolume.m_vMax.x < m_ShadowVolumeVS.m_vMin.x) return false;
  if (meshVolume.m_vMax.y < m_ShadowVolumeVS.m_vMin.y) return false;
  if (meshVolume.m_vMax.z < m_ShadowVolumeVS.m_vMin.z) return false;

  return true;
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
