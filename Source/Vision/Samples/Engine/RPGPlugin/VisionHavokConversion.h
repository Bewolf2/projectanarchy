/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_VISION_HAVOK_CONVERSION_H__
#define RPG_PLUGIN_VISION_HAVOK_CONVERSION_H__

namespace RPG_VisionHavokConversion
{
  // Vision -> Havok
  void VisionToHavokDirection(hkvVec3 const& dirIn, hkVector4& dirOut);

  void VisionToHavokPoint(hkvVec3 const& pointIn, hkVector4& pointOut);

  void VisionToHavokRotationQuaternion(hkvMat3 const& rotIn, hkQuaternion& rotOut);

  hkReal VisionToHavokScalar(float s);

  // Havok -> Vision
  void HavokToVisionDirection(hkVector4Parameter dirIn, hkvVec3& dirOut);

  void HavokToVisionPoint(hkVector4Parameter pointIn, hkvVec3& pointOut);

  void HavokToVisionRotation(const hkRotation& rotIn, hkvMat3& matOut);
}

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

#include <Vision/Samples/Engine/RPGPlugin/VisionHavokConversion.inl>

#endif

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
