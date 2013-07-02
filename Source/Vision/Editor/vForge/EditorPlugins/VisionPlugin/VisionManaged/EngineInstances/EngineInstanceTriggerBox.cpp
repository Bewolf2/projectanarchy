/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceTriggerBox.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceTriggerBox::EngineInstanceTriggerBox()
  {
    m_eShapeType = TriggerShape_e::Box;
    _bx=_by=_bz=0.f;
    _x=_y=_z=0.f;
    m_pEntity = nullptr;
    m_pEntityWP = nullptr;

    // set the model file later
    TriggerBoxEntity_cl *pEntity = (TriggerBoxEntity_cl *)Vision::Game.CreateEntity("TriggerBoxEntity_cl", hkvVec3(0,0,0));
    Debug::Assert(pEntity!=nullptr,"Could not create trigger box entity!");
    if (pEntity)
    {
      m_pEntity = pEntity;
      m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(pEntity->GetWeakReference());
    }
  }

  void EngineInstanceTriggerBox::DisposeObject()
  {
    TriggerBoxEntity_cl *pEntity = GetTriggerEntity();
    V_SAFE_DISPOSEOBJECT(pEntity);
    m_pEntity = nullptr;
    V_SAFE_DELETE(m_pEntityWP);
  }

  void EngineInstanceTriggerBox::SetPosition(float x,float y,float z)
  {
    IEngineInstanceObject3D::SetPosition(x,y,z);
    _x=x;_y=y;_z=z;
    UpdateAbsBox();
  }

  void EngineInstanceTriggerBox::SetOrientation(float yaw,float pitch,float roll)
  {
    IEngineInstanceObject3D::SetOrientation(yaw,pitch,roll);
    UpdateAbsBox();
  }

  void EngineInstanceTriggerBox::UpdateAbsBox()
  {
    if (GetTriggerEntity()==nullptr)
      return;

    // set local bounding box
    hkvAlignedBBox localbox (hkvVec3 (-_bx,-_by,-_bz), hkvVec3 (+_bx,+_by,+_bz));
    hkvMat3 rotmat = GetTriggerEntity()->GetRotationMatrix();
    if (m_eShapeType==TriggerShape_e::Sphere)
    {
      GetTriggerEntity()->m_bIsSphere = true;
      GetTriggerEntity()->m_bIsOriented = false;
      GetTriggerEntity()->SetSphere(hkvVec4(_x,_y,_z, _bx));
      rotmat.setIdentity();
    } else
    {
      GetTriggerEntity()->m_bIsSphere = false;
      GetTriggerEntity()->m_bIsOriented = !rotmat.isIdentity ();
      GetTriggerEntity()->m_LocalBoundingBox = localbox;
    }

    // set absolute bounding box (using local box+orientation)
    hkvAlignedBBox &bbox = GetTriggerEntity()->m_AbsBoundingBox;
    bbox.setInvalid();

    hkvAlignedBBox temp = localbox;
    hkvMat4 mTransform (rotmat, GetTriggerEntity()->GetPosition());
    temp.transformFromOrigin (mTransform);
    bbox.expandToInclude (temp);
  }

  bool EngineInstanceTriggerBox::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    /*
    float r = _bx;
    if (_bIsSphere)
      (*bbox).Set(-r,-r,-r, r,r,r);
    else
      (*bbox).Set(-_bx,-_by,-_bz, _bx,_by,_bz);
    return true;
    */
    return false; // Now in the shape
  }

  void EngineInstanceTriggerBox::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if ( GetTriggerEntity() == nullptr )
      return;
/*
    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);

    pResult->SetShapeHit_IfCloser(ownerShape,traceInfo.distance);
*/
  }

  void EngineInstanceTriggerBox::RenderShape(VisionViewBase ^view, ShapeRenderMode mode, unsigned int iColor)
  {
    if (!GetTriggerEntity())
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView ^>(view))->GetRenderInterface();
    
    VColorRef iColorRef(false);
    iColorRef.SetRGBA(iColor);
    iColorRef.a = 20;
    if (mode==ShapeRenderMode::Selected)
      iColorRef.a = 120;

    GetTriggerEntity()->DebugRender(pRenderer,iColorRef);
  }

  bool EngineInstanceTriggerBox::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( GetTriggerEntity() != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << GetTriggerEntity();
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
