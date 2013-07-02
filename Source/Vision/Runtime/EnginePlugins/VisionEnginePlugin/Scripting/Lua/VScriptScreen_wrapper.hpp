/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VSCRIPSCREEN_WRAPPER_HPP
#define __VSCRIPSCREEN_WRAPPER_HPP

/// 
/// \brief
///   Small wrapper to unify utility functions inside the scripting binding
/// 
class VScriptScreen_wrapper
{
public:

  VScriptScreen_wrapper() {}
  ~VScriptScreen_wrapper() {}


  hkvVec3*  PickPoint(float x, float y, float fMaxDist=10000.f, bool bIgnoreEntities=false, bool bIgnoreStaticGeomerty=false)
  {
    hkvVec3 traceStart = Vision::Camera.GetCurrentCameraPosition();
    hkvVec3 traceDir;
    Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(x, y, traceDir, fMaxDist);
    hkvVec3 traceEnd = traceStart + traceDir;
    VisTraceLineInfo_t traceInfo;
    if (!Vision::CollisionToolkit.TraceLine(traceStart, traceEnd, (ULONG)(bIgnoreStaticGeomerty?0:VIS_TRACE_ALL) , (ULONG)(bIgnoreEntities?0:VIS_TRACE_ALL) ,NULL, NULL, &traceInfo))
    {
      return new hkvVec3(traceInfo.touchPoint); //resource management by lua
    }

    return NULL;
  }

  hkvVec3* Project3D(float x, float y, float fDist)
  {
    hkvVec3 traceStart = Vision::Camera.GetCurrentCameraPosition();
    hkvVec3 traceDir;
    Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(x, y, traceDir, fDist);
    hkvVec3 traceEnd = traceStart + traceDir;

    return new hkvVec3(traceEnd); //resource management by lua
  }

  VisBaseEntity_cl * PickEntity(float x, float y, float fMaxDist=10000.f, bool bIgnoreStaticGeomerty=false)
  {
    hkvVec3 traceStart = Vision::Camera.GetCurrentCameraPosition();
    hkvVec3 traceDir;
    Vision::Contexts.GetCurrentContext()->GetTraceDirFromScreenPos(x, y, traceDir, fMaxDist);
    hkvVec3 traceEnd = traceStart + traceDir;
    VisTraceLineInfo_t traceInfo;
    if (!Vision::CollisionToolkit.TraceLine(traceStart, traceEnd, (ULONG)(bIgnoreStaticGeomerty?0:VIS_TRACE_ALL), (ULONG) VIS_TRACE_ALL ,NULL, NULL, &traceInfo))
    {
      return traceInfo.colliderEntity;
    }

    return NULL;
  }

  bool SaveToFile(const char * szFileName = NULL, bool bJpgInsteadBmp = true)
  {
    return Vision::Game.SaveScreenShot(szFileName, bJpgInsteadBmp);
  }

  float GetDeviceDpi()
  {
    return Vision::Video.GetDeviceDpi();
  }
};

#endif // __VSCRIPSCREEN_WRAPPER_HPP

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
