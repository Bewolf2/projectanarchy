/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

/// The ConstraintVisualizer encapsulates the various rendering methods which are
/// used to visualize constraints in the engine view.
class ConstraintVisualizer
{
private:
  ConstraintVisualizer();
  ~ConstraintVisualizer();

public:
  /// Visualize a spring connection between two points
  static void RenderSpring (IVRenderInterface* pRenderer, const hkvVec3& startPos, const hkvVec3& endPos);

  /// Render a simple line between two points
  static void RenderSimpleLine (IVRenderInterface* pRenderer, const hkvVec3& etartPos, const hkvVec3& endPos);

  /// Visualize a prismatic connect (two intersecting boxes) between two points
  static void RenderPrismaticBoxes (IVRenderInterface* pRenderer, const hkvVec3& vStartPos, const hkvVec3& vEndPos);

  /// Render a cylinder at the passed position
  static void RenderCylinder (IVRenderInterface* pRenderer, const hkvVec3& vCenter, const hkvVec3& vDirection, float fHeight);

  /// Render a infinite plane at the passed center/ orientation
  static void RenderInfinitePlane (IVRenderInterface* pRenderer, const hkvVec3& vCenter, const hkvVec3& vOrientation);
};

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
