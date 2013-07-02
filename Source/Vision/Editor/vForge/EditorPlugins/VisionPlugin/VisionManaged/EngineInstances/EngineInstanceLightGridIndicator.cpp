/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceLightGridIndicator.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceLightGridIndicator::EngineInstanceLightGridIndicator()
  {
    m_bVisible = true;
    posx=posy=posz=0.f;
    m_fRadius = 20.f;
    m_bRendergrid = false;
    m_pParentZone = nullptr;
  }

  void EngineInstanceLightGridIndicator::DisposeObject()
  {
  }

  
  void EngineInstanceLightGridIndicator::SetParentZone(Zone ^zone)
  {
    m_pParentZone = zone ? ((VisZoneResource_cl *)zone->NativeZonePtr.ToPointer()) : NULL;
  }

  void EngineInstanceLightGridIndicator::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vDir(rayEnd.X-rayStart.X,rayEnd.Y-rayStart.Y,rayEnd.Z-rayStart.Z);
/*  
    hkvAlignedBBox bbox;
    if (!GetLocalBoundingBox(bbox))
      return;
    
    float fT;
    hkvVec3 vTouch;

    if (!bbox.Intersect(vStart,vDir, -1.f, &fT, &vTouch))
      return;

    fT *= vDir.GetLength();
    pResult->SetShapeHit_IfCloser(ownerShape,fT);
  */
  }

  bool EngineInstanceLightGridIndicator::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    const float fSize = m_fRadius;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

 
  void EngineInstanceLightGridIndicator::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;
    VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
    if (m_pParentZone && m_pParentZone->m_spZoneLightGrid!=nullptr)
      pGrid = m_pParentZone->m_spZoneLightGrid;
    if (!pGrid)
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();

    // render the grid axes
    hkvVec3 vPos(posx,posy,posz);
    pGrid->RenderAtPosition(pRenderer,vPos,m_fRadius,5.0f);

    // render the grid cells
    if (m_bRendergrid)
    {
      hkvVec3 vRadius(m_fRadius,m_fRadius,m_fRadius);
      hkvAlignedBBox bbox(vPos-vRadius,vPos+vRadius);
      pGrid->RenderGrid(pRenderer, bbox);
    }
  }


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
