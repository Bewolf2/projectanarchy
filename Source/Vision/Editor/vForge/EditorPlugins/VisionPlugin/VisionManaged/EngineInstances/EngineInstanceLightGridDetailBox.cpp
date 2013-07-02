/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceLightGridDetailBox.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;

#ifdef _MSC_VER
// disable warning C4996: deprecated function
#pragma warning( disable : 4996)
#endif

namespace VisionManaged
{

  EngineInstanceLightGridDetailBox::EngineInstanceLightGridDetailBox()
  {
    m_pParentZone = nullptr;
    m_pBox = new VLightGridDetailBox_cl();
    m_pBox->AddRef();
    m_fCenterX = m_fCenterY = m_fCenterZ = 0.f;
    m_fBoxDimX = m_fBoxDimY = m_fBoxDimZ = 10.f;
  }

  void EngineInstanceLightGridDetailBox::DisposeObject()
  {
    V_SAFE_RELEASE(m_pBox);
  }

  void EngineInstanceLightGridDetailBox::SetParentZone(Zone ^zone)
  {
    m_pParentZone = zone ? ((VisZoneResource_cl *)zone->NativeZonePtr.ToPointer()) : NULL;
  }



  void EngineInstanceLightGridDetailBox::SetPosition(float x,float y,float z)
  {
    m_fCenterX = x;
    m_fCenterY = y;
    m_fCenterZ = z;
    UpdateBox();
  }

  void EngineInstanceLightGridDetailBox::SetOrientation(float yaw,float pitch,float roll)
  {
  }

  void EngineInstanceLightGridDetailBox::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);
    VLightGrid_cl *pGrid = nullptr;
    
    if (mode==ShapeRenderMode::Selected && Vision::RenderLoopHelper.HasLightGrid())
    {
      pGrid = Vision::RenderLoopHelper.GetLightGrid();
      if (m_pParentZone && m_pParentZone->m_spZoneLightGrid!=nullptr)
        pGrid = m_pParentZone->m_spZoneLightGrid;
    }

    m_pBox->Render(pRenderer, VColorRef(255,255,10,50),state,g_bDisplayGrid ? pGrid : NULL);
  }

  void EngineInstanceLightGridDetailBox::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);
  }



  bool EngineInstanceLightGridDetailBox::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    (*bbox).Set(-m_fBoxDimX,-m_fBoxDimY,-m_fBoxDimZ,m_fBoxDimX,m_fBoxDimY,m_fBoxDimZ);
    return true;
  }


  ////////////////////////////////////////////////////////////////////////////////////////
  // EngineInstanceLightGridBoundingBox specific
  ////////////////////////////////////////////////////////////////////////////////////////

#define IX (bbox.m_vMin[0]+(float)x*fx)
#define IY (bbox.m_vMin[1]+(float)y*fy)
#define IZ (bbox.m_vMin[2]+(float)z*fz)

  void EngineInstanceLightGridBoundingBox::RenderShape(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (Vision::Editor.IsAnimatingOrPlaying())
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    VSimpleRenderState_t state(VIS_TRANSP_ALPHA, RENDERSTATEFLAG_FRONTFACE);

    // render box
    VColorRef iColor(200,128,0,100);
    hkvAlignedBBox bbox(
      hkvVec3 (m_fCenterX-m_fBoxDimX,m_fCenterY-m_fBoxDimY,m_fCenterZ-m_fBoxDimZ),
      hkvVec3 (m_fCenterX+m_fBoxDimX,m_fCenterY+m_fBoxDimY,m_fCenterZ+m_fBoxDimZ));

    int x,y,z;
    int sx = m_pBox->m_iSubDiv[0];
    int sy = m_pBox->m_iSubDiv[1];
    int sz = m_pBox->m_iSubDiv[2];
    float fx = bbox.getSizeX()/(float)sx;
    float fy = bbox.getSizeY()/(float)sy;
    float fz = bbox.getSizeZ()/(float)sz;

    if (m_bDisplayIndicators)
    {
      VLightGrid_cl *pGrid = Vision::RenderLoopHelper.GetLightGrid();
      if (m_pParentZone && m_pParentZone->m_spZoneLightGrid!=nullptr)
        pGrid = m_pParentZone->m_spZoneLightGrid;
      if (!pGrid)
        return;

      for (x=0;x<=sx;x++)
      {
        for (y=0;y<=sy;y++)
        {
          for (z=0;z<=sz;z++)
          {
            // render the grid axes
            hkvVec3 vPos(IX,IY,IZ);

            pGrid->RenderAtPosition (pRenderer, vPos, 0.25f * min (fx, min (fy, fz)), 5.0f);
          }
        }
      }
    }
    else
    {
      pRenderer->RenderAABox(bbox,iColor,state);

      for (x=0;x<=sx;x++)
        for (y=0;y<=sy;y++) if (m_bDisplayGrid||x==0||x==sx||y==0||y==sy)
          pRenderer->DrawLine(hkvVec3(IX,IY,bbox.m_vMin[2]),hkvVec3(IX,IY,bbox.m_vMax[2]),iColor,1.f);
      for (x=0;x<=sx;x++)
        for (z=0;z<=sz;z++) if (m_bDisplayGrid||x==0||x==sx||z==0||z==sz)
          pRenderer->DrawLine(hkvVec3(IX,bbox.m_vMin[1],IZ),hkvVec3(IX,bbox.m_vMax[1],IZ),iColor,1.f);
      for (y=0;y<=sy;y++)
        for (z=0;z<=sz;z++) if (m_bDisplayGrid||y==0||y==sy||z==0||z==sz)
          pRenderer->DrawLine(hkvVec3(bbox.m_vMin[0],IY,IZ),hkvVec3(bbox.m_vMax[0],IY,IZ),iColor,1.f);
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
