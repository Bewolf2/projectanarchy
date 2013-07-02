/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP
#define __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiIncludes.hpp>

class vHavokAiNavMeshDebugDisplayHandler : public hkDebugDisplayHandler
{
public:
	VOVERRIDE ~vHavokAiNavMeshDebugDisplayHandler() {}

	// Unhandled functions
	VOVERRIDE hkResult addGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint, hkGeometry::GeometryType geomType) HKV_OVERRIDE;
	VOVERRIDE hkResult addGeometry(hkDisplayGeometry* geometry, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	VOVERRIDE hkResult addGeometryInstance(hkUlong origianalGeomId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	VOVERRIDE hkResult setGeometryColor(hkColor::Argb color, hkUlong id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult setGeometryTransparency(float alpha, hkUlong id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult setGeometryPickable( hkBool isPickable, hkUlong id, int tag ) HKV_OVERRIDE;
	VOVERRIDE hkResult setGeometryVisibility(int geometryIndex, bool isEnabled, hkUlong id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult updateGeometry(const hkTransform& transform, hkUlong id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult updateGeometry(const hkMatrix4& transform, hkUlong id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint) HKV_OVERRIDE;
	VOVERRIDE hkResult updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name) HKV_OVERRIDE;
	VOVERRIDE hkResult displayPoint(const hkVector4& position, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult displayText(const char* text, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult sendMemStatsDump(const char* data, int length) HKV_OVERRIDE;
	VOVERRIDE hkResult holdImmediate() HKV_OVERRIDE;

	// only these functions are implemented
	VOVERRIDE hkResult displayLine(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;
	VOVERRIDE hkResult display3dText(const char* text, const hkVector4& pos, hkColor::Argb color, int id, int tag) HKV_OVERRIDE;

	// compatibility with VisionIntegration branch
#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
	VOVERRIDE hkResult displayPoint2d(const hkVector4& position, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	VOVERRIDE hkResult displayLine2d(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	VOVERRIDE hkResult displayTriangle2d(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
	VOVERRIDE hkResult displayText2d(const char* text, const hkVector4& pos, hkReal sizeScale, hkColor::Argb color, int id, int tag)  HKV_OVERRIDE { return HK_SUCCESS; }
#endif
};


#endif	// __VHAVOK_AI_NAVMESH_DEBUG_DISPLAY_HPP

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
