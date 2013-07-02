/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/HavokAiEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokAiEnginePlugin/vHavokAiNavMeshDebugDisplayHandler.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

// Unhandled functions
hkResult vHavokAiNavMeshDebugDisplayHandler::addGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint, hkGeometry::GeometryType geomType) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::addGeometry(hkDisplayGeometry* geometry, hkUlong id, int tag, hkUlong shapeIdHint) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::addGeometryInstance(hkUlong origianalGeomId, const hkTransform& transform, hkUlong id, int tag, hkUlong shapeIdHint) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::setGeometryColor(hkColor::Argb color, hkUlong id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::setGeometryTransparency(float alpha, hkUlong id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::setGeometryVisibility(int geometryIndex, bool isEnabled, hkUlong id, int tag) {return HK_FAILURE;}
hkResult vHavokAiNavMeshDebugDisplayHandler::setGeometryPickable( hkBool isPickable, hkUlong id, int tag ) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::updateGeometry(const hkTransform& transform, hkUlong id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::updateGeometry(const hkMatrix4& transform, hkUlong id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::skinGeometry(hkUlong* ids, int numIds, const hkMatrix4* poseModel, int numPoseModel, const hkMatrix4& worldFromModel, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::removeGeometry(hkUlong id, int tag, hkUlong shapeIdHint) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::updateCamera(const hkVector4& from, const hkVector4& to, const hkVector4& up, hkReal nearPlane, hkReal farPlane, hkReal fov, const char* name) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::displayPoint(const hkVector4& position, hkColor::Argb color, int id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::displayText(const char* text, hkColor::Argb color, int id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, const hkTransform& transform, hkColor::Argb color, int id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::displayGeometry(const hkArrayBase<hkDisplayGeometry*>& geometries, hkColor::Argb color, int id, int tag) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::sendMemStatsDump(const char* data, int length) { return HK_FAILURE; }
hkResult vHavokAiNavMeshDebugDisplayHandler::holdImmediate() { return HK_FAILURE; }

hkResult vHavokAiNavMeshDebugDisplayHandler::displayLine(const hkVector4& start, const hkVector4& end, hkColor::Argb color, int id, int tag)
{
	VColorRef vcol; vcol.SetRGBA( hkColor::getRedAsChar(color), hkColor::getGreenAsChar(color), hkColor::getBlueAsChar(color), hkColor::getAlphaAsChar(color) );
	hkvVec3 s; vHavokConversionUtils::PhysVecToVisVecWorld(start,s);
	hkvVec3 e; vHavokConversionUtils::PhysVecToVisVecWorld(end,e);
	Vision::Game.DrawSingleLine( s, e, vcol );
	return HK_SUCCESS;
}

hkResult vHavokAiNavMeshDebugDisplayHandler::displayTriangle(const hkVector4& a, const hkVector4& b, const hkVector4& c, hkColor::Argb color, int id, int tag)
{
	hkvVec3 va; vHavokConversionUtils::PhysVecToVisVecWorld(a,va); 
	hkvVec3 vb; vHavokConversionUtils::PhysVecToVisVecWorld(b,vb); 
	hkvVec3 vc; vHavokConversionUtils::PhysVecToVisVecWorld(c,vc); 
	VColorRef vcol; vcol.SetRGBA( hkColor::getRedAsChar(color), hkColor::getGreenAsChar(color), hkColor::getBlueAsChar(color), hkColor::getAlphaAsChar(color) );
	Vision::Game.DrawSingleTriangle( va, vb, vc, vcol );
	return HK_SUCCESS;
}

hkResult vHavokAiNavMeshDebugDisplayHandler::display3dText(const char* text, const hkVector4& pos, hkColor::Argb color, int id, int tag)
{
	VColorRef vcol; vcol.SetRGBA( hkColor::getRedAsChar(color), hkColor::getGreenAsChar(color), hkColor::getBlueAsChar(color), hkColor::getAlphaAsChar(color) );
	Vision::Message.SetTextColor(vcol);
	hkvVec3 vpos; vHavokConversionUtils::PhysVecToVisVecWorld(pos,vpos);
	Vision::Message.DrawMessage3D(text, vpos);
	Vision::Message.SetTextColor(V_RGBA_WHITE);
	return HK_SUCCESS;
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
