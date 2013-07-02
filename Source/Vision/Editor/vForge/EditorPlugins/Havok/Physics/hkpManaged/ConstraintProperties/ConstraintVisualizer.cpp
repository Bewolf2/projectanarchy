/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/ConstraintVisualizer.hpp>

#using <mscorlib.dll>

void ConstraintVisualizer::RenderSpring (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos)
{
  // Calculate the look at matrix
  hkvVec3 vSource(startPos.x, startPos.y, startPos.z);
  hkvVec3 vTarget(endPos.x, endPos.y, endPos.z);
  hkvMat3 lookAtMat;
  lookAtMat.setLookInDirectionMatrix (vTarget - vSource);

  // Calculate the length and thickness of the spring
  float fVecLen = (vTarget - vSource).getLength();
  float fSpringThickness = fVecLen / 40.0f;

  // Render the spring
  hkvVec3 vOldPos;
  for (int i = -5; i < 105; i++)
  {
    // Calculate the position on the spring line
    float fPosOnLine = fVecLen * ((float) i / 100.0f);
    fPosOnLine = hkvMath::clamp (fPosOnLine, 0.0f, fVecLen);
    hkvVec3 vCurrentPos = vSource + lookAtMat.getAxis(0) * fPosOnLine;

    // Calculate the local offset (to visualize circles)
    float fAngle = ((float)(i % 10)) * 2 * hkvMath::pi () / 10.0f;
    float diffX = sin(fAngle);
    float diffY = cos(fAngle);
    vCurrentPos += lookAtMat.getAxis(1) * fSpringThickness * diffX;
    vCurrentPos += lookAtMat.getAxis(2) * fSpringThickness * diffY;

    // Render the next line fragment
    if (!vOldPos.isZero (0.0f))
    {
      VColorRef color(255, 255, 0);
      pRenderer->DrawLine(vOldPos, hkvVec3(vCurrentPos.x, vCurrentPos.y, vCurrentPos.z), color, 2);
    }

    vOldPos = hkvVec3(vCurrentPos.x, vCurrentPos.y, vCurrentPos.z);
  }
}

void ConstraintVisualizer::RenderSimpleLine (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos)
{
  VColorRef color(255, 255, 0);
  pRenderer->DrawLine(startPos, endPos, color, 2);
}

void ConstraintVisualizer::RenderPrismaticBoxes (IVRenderInterface* pRenderer, const hkvVec3& vStartPos, const hkvVec3& vEndPos)
{
  const VColorRef colorIB(0, 0, 200);
  const VColorRef colorOB(180, 180, 180);

  // Calculate the look at matrix
  hkvMat3 lookAtMat (hkvNoInitialization);
  lookAtMat.setLookInDirectionMatrix (vEndPos - vStartPos);

  // Compute box widths, so that they work for all world scales
  float fVecLen = (vStartPos - vEndPos).getLength();
  const float fWidthIB = fVecLen / 100.0f;
  const float fWidthOB = fWidthIB * 2.0f;

  // Setup the two boxes
  hkvAlignedBBox bBoxInner (hkvVec3 (0, fWidthIB, fWidthIB), hkvVec3 (fVecLen/2, -fWidthIB, -fWidthIB));
  hkvAlignedBBox bBoxOuter (hkvVec3 (fVecLen/2, fWidthOB, fWidthOB), hkvVec3 (fVecLen, -fWidthOB, -fWidthOB));

  // Render the two bounding boxes
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  hkvVec3 corners[8];

  const hkvMat4 ThisIsALookAtMatrixYARLY (lookAtMat, vStartPos);

  bBoxInner.getCorners(corners);
  ThisIsALookAtMatrixYARLY.transformPositions(corners, 8);

  pRenderer->RenderBox(corners,sizeof(hkvVec3),colorIB,state);

  bBoxOuter.getCorners(corners);
  ThisIsALookAtMatrixYARLY.transformPositions(corners, 8);

  pRenderer->RenderBox(corners,sizeof(hkvVec3),colorOB,state);
}

void ConstraintVisualizer::RenderCylinder (IVRenderInterface* pRenderer, const hkvVec3& vCenter, const hkvVec3& vDirection, float fHeight)
{
  const VColorRef color(255, 255, 0);
  float fRadius = fHeight / 10.0f;

  hkvVec3 vStartPos = vCenter;
  vStartPos -= (vDirection * fHeight/2);  
  hkvVec3 vScaledDir = vDirection * fHeight;

  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
  pRenderer->RenderCylinder(vStartPos, vScaledDir, fRadius, color, state);
}

void ConstraintVisualizer::RenderInfinitePlane (IVRenderInterface* pRenderer, const hkvVec3& vCenter, const hkvVec3& vOrientation)
{
  VColorRef color(255, 255, 0, 128);
  VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);

  hkvMat3 mRotation (hkvNoInitialization);
  mRotation.setFromEulerAngles (vOrientation.z, vOrientation.y, vOrientation.x);
 
  hkvVec3 vNormal = mRotation.getAxis(2);
  hkvPlane plane (hkvNoInitialization); plane.setFromPointAndNormal (vCenter, vNormal);

  float fSize = 250.0f*EditorManager::Settings->GlobalUnitScaling;
  pRenderer->RenderPlane(plane, vCenter, fSize, fSize, color, state);
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
