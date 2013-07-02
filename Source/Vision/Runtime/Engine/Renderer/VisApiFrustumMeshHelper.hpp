/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFrustumMeshHelper.hpp

#ifndef VISAPIFRUSTUMMESHHELPER_HPP_INCLUDED
#define VISAPIFRUSTUMMESHHELPER_HPP_INCLUDED

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

/// \brief
///  Helper class that creates a full-screen quad in screen space with the far frustum points as tex coords.
class VFrustumMeshHelper
{
public:
  enum FarCornerType_e {
    IN_VIEW_SPACE,
    IN_WORLD_SPACE
  };

  /// \brief
  ///  Creates a mesh buffer with 3 POSITION and 3 TEXCOORD0 elements.
  VISION_APIFUNC static VisMeshBuffer_cl* CreateMeshBuffer();

  /// \brief
  ///  Fills the given mesh buffer with the screen space corners (+-1, +-1, 0) as position values and the far frustum corners as tex coords.
  VISION_APIFUNC static void UpdateMeshBuffer(VisMeshBuffer_cl* pMeshBuffer, VisRenderContext_cl* pContext, FarCornerType_e type);
};

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
