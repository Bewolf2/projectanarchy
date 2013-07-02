/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/vForge/CustomShape/CustomShapeManaged/CustomShapeManagedPCH.h>
#include <Vision/Samples/vForge/CustomShape/CustomShapeManaged/EngineInstanceNode.hpp>

using namespace ManagedFramework;
#using <mscorlib.dll>


namespace NodeManaged
{

  // create the native instance in the constructor
  EngineInstanceNode::EngineInstanceNode()
  {
    m_bIsVisible = true;
    m_pNode = new VNode_cl();
    m_pNode->AddRef();  // add the ref counter. Refcounted native objects have the advantage that they can't be destroyed
                        // in the engine while animating the view
  }

  // destroy the native instance of the node as well
  void EngineInstanceNode::DisposeObject()
  {
    if (m_pNode)
    {
      m_pNode->DisconnectAll();
      m_pNode->Dispose(); // remove it from the global list
      m_pNode->Release(); // decrease the refcounter
      m_pNode = NULL;
    }
  }

  void EngineInstanceNode::SetPosition(float x,float y,float z)
  {
    m_pNode->SetPosition(x,y,z);
  }

  void EngineInstanceNode::SetOrientation(float /*yaw*/,float /*pitch*/,float /*roll*/)
  {
    // no orientation
  }

  void EngineInstanceNode::RenderShape(VisionViewBase^ /*view*/, CSharpFramework::Shapes::ShapeRenderMode mode)
  {
    if (!m_pNode) return;
    IVRenderInterface* pRI = Vision::Contexts.GetCurrentContext()->GetRenderInterface();

    // render this shape as a small bounding box. Use a slightly larger bounding box if it is selected
    switch (mode)
    {
      case ShapeRenderMode::Normal:   if (m_bIsVisible) m_pNode->DebugRender(pRI,4.f,VColorRef(255,0,0,80), false); break;
      case ShapeRenderMode::Selected: m_pNode->DebugRender(pRI,5.f,VColorRef(255,160,0,140), false); break;
    }
  }


  bool EngineInstanceNode::GetLocalBoundingBox(BoundingBox^ %bbox)
  {
    // create a small bounding box for picking
    float fSize = 5.f;
    (*bbox).Set(-fSize,-fSize,-fSize,fSize,fSize,fSize);
    return true;
  }

  void EngineInstanceNode::TraceShape(Shape3D^ /* ownerShape*/, Vector3F /*rayStart*/,Vector3F /*rayEnd*/, ShapeTraceResult^% /*result*/)
  {
    // don't do anything because the shape code already does the picking
  }

  void EngineInstanceNode::LinkNode(EngineInstanceNode ^pOther)
  {
    // link to another instance
    if (!m_pNode || !pOther->m_pNode) return;
    if (!m_pNode->IsConnected(pOther->m_pNode)) // might already be linked
      m_pNode->ConnectPoint(pOther->m_pNode);
  }

  void EngineInstanceNode::UnLinkNode(EngineInstanceNode ^pOther)
  {
    // unlink another instance
    if (!m_pNode || !pOther->m_pNode) return;
    if (m_pNode->IsConnected(pOther->m_pNode)) // might already be unlinked
      m_pNode->DisconnectPoint(pOther->m_pNode);
  }

  bool EngineInstanceNode::OnExport(SceneExportInfo ^info)
  {
    // do the export: Get the export binary archive and serialize into it.
    // Requires the native node class to support serialization (derive from VisTypedEngineObject_cl and 
    // implement Serialize function)
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar.WriteObject(m_pNode);
    return true;
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
