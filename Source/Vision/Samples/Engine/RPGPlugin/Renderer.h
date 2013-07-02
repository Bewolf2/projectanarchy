/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_RENDERER_H__
#define RPG_PLUGIN_RENDERER_H__

struct RPG_ViewParams
{
  // Projection
  VisProjectionType_e m_projectionType;
  float m_fovX;
  float m_fovY;
  float m_orthoSizeX;
  float m_orthoSizeY;
  float m_nearClip;
  float m_farClip;
  hkvMat4 m_projectionMatrix;

  // Viewport
  int m_vpX;
  int m_vpY;
  int m_vpW;
  int m_vpH;
  float m_vpNear;
  float m_vpFar;
};

namespace RPG_RendererUtil
{
  void StoreViewParams(RPG_ViewParams& viewParams);

  void LoadViewParams(RPG_ViewParams const& viewParams);
}

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
