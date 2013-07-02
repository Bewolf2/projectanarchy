/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/VisionManagedPCH.h>
#include <Vision/Editor/vForge/EditorPlugins/VisionPlugin/VisionManaged/EngineInstances/EngineInstanceClothObject.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace VisionManaged
{

  EngineInstanceClothObject::EngineInstanceClothObject()
  {
    // set the model file later
    ClothEntity_cl *pEntity = (ClothEntity_cl *)Vision::Game.CreateEntity("ClothEntity_cl", hkvVec3(0,0,0));
    Debug::Assert(pEntity!=nullptr,"Could not create Cloth entity!");
    m_bVisible = true;
    m_iVisibleBitmask = VIS_ENTITY_VISIBLE;
    m_fScaleX = m_fScaleY = m_fScaleZ = 1.f;
    m_fYaw = m_fPitch = m_fRoll = 0.f;
    
    // reference the entity with a weak pointer. This will make sure that we correctly get a null pointer
    // if the entity gets deleted in the engine
    m_pEntityWP = new VWeakPtr<VisBaseEntity_cl>(pEntity->GetWeakReference());
  }

  void EngineInstanceClothObject::DisposeObject()
  {
    ClothEntity_cl *pEntity = GetClothEntity();
    V_SAFE_DISPOSEOBJECT(pEntity);
    V_SAFE_DELETE( m_pEntityWP );
  }

  bool EngineInstanceClothObject::SetModelFile(String ^modelfile, Vector3F scaling, String ^%sError)
  {
    ClothEntity_cl *pEnt = GetClothEntity();
    if (!pEnt)
    {
      sError = gcnew String("No entity set");
      return false;
    }

    m_fScaleX = scaling.X;
    m_fScaleY = scaling.Y;
    m_fScaleZ = scaling.Z;

    hkvVec3 vScaling(scaling.X,scaling.Y,scaling.Z);
    if ( modelfile != nullptr)
    {
      VString sModelFile;
      ConversionUtils::StringToVString( modelfile, sModelFile );
/*
      // cannot opt. this anymore since scaling might change
      if (pEnt->GetModel())
      {
        VString sOldModel = pEnt->GetModel()->GetFilename();
        if (sOldModel==sModelFile) // no mesh creating required
          return true;
      }
      */
      VString sErrorStr;
      if (!pEnt->SetMeshModel(sModelFile, vScaling, &sErrorStr))
      {
        sError = ConversionUtils::VStringToString(sErrorStr);
        return false;
      }
    }
    else
    {
      VString sErrorStr;
      pEnt->SetMeshModel(NULL, vScaling, &sErrorStr);
    }

    return true;
  }


  void EngineInstanceClothObject::SetPhysicsProperties(int iTicks, bool bWhenVisible, float fGravity,int iInitialCount)
  {
    ClothEntity_cl *pEnt = GetClothEntity();
    if (!pEnt)
      return;
    pEnt->SetPhysicsTicks(iTicks);
    pEnt->SetGravity(fGravity);
    pEnt->SetSimulateWhenVisible(bWhenVisible);
    pEnt->SetInitialTickCount(iInitialCount);
  }

    void EngineInstanceClothObject::RunInitialPhysicsTicks()
    {
      ClothEntity_cl *pEnt = GetClothEntity();
      if (pEnt)
        pEnt->RunInitialTicks();
    }

/*
  void EngineInstanceClothObject::SetEntityKey(String ^key)
  {
    VString sKey;
    ConversionUtils::StringToVString(key, sKey);
    if (GetClothEntity())
      GetClothEntity()->SetEntityKey(sKey);
  }
*/

  void EngineInstanceClothObject::SetPosition(float x,float y,float z)
  {
    if (GetClothEntity())
      GetClothEntity()->SetClothPosition(hkvVec3(x,y,z));
  }

  void EngineInstanceClothObject::SetOrientation(float yaw,float pitch,float roll)
  {
    m_fYaw = yaw;
    m_fPitch = pitch;
    m_fRoll = roll;

    if (GetClothEntity())
      GetClothEntity()->SetClothOrientation(hkvVec3(yaw,pitch,roll));
  }


  void EngineInstanceClothObject::SetScaling(float x,float y, float z) 
  {
    if (hkvMath::isFloatEqual (x,m_fScaleX) && hkvMath::isFloatEqual (y,m_fScaleY) && hkvMath::isFloatEqual (z,m_fScaleZ)) // model update required?
      return;

    m_fScaleX = x;
    m_fScaleY = y;
    m_fScaleZ = z;

    ClothEntity_cl *pEnt = GetClothEntity();
    if (!pEnt || !pEnt->GetMesh())
      return;

    // re-construct model with new scaling
    hkvVec3 vScaling(x,y,z);
    pEnt->SetMeshModel(pEnt->GetMesh()->GetFilename(), vScaling, NULL);
    pEnt->SetClothOrientation(hkvVec3(m_fYaw,m_fPitch,m_fRoll));
  }

  bool EngineInstanceClothObject::GetLocalBoundingBox(BoundingBox ^%bbox)
  {
    if ( GetClothEntity() == nullptr )
      return false;

    //*bbox = nullptr;
    VDynamicMesh *pMesh = GetClothEntity()->GetMesh();
    if (!pMesh || !pMesh->IsLoaded())
      return false;

    hkvAlignedBBox temp;
    if (!pMesh->GetVisibilityBoundingBox(temp))
      return false;

    (*bbox).Set(temp.m_vMin.x,temp.m_vMin.y,temp.m_vMin.z, temp.m_vMax.x,temp.m_vMax.y,temp.m_vMax.z );
    return true;
  }

  void EngineInstanceClothObject::TraceShape(Shape3D ^ownerShape, Vector3F rayStart,Vector3F rayEnd, ShapeTraceResult ^%result)
  {
    if ( GetClothEntity() == nullptr )
      return;

    ShapeTraceResult ^pResult = result;
    hkvVec3 vStart(rayStart.X,rayStart.Y,rayStart.Z);
    hkvVec3 vEnd(rayEnd.X,rayEnd.Y,rayEnd.Z);

    VisBaseEntity_cl *list = GetClothEntity();
    VisEntityCollection_cl traceCollection(1, &list, 1, 1);
    static VisTraceLineInfo_t traceInfo;//@@@@
    int iFlags = VIS_TRACESETTINGS_FORCEOBJ_POLYGONACC|VIS_TRACESETTINGS_STORESURFACEINFO;
    int iTraceCount = Vision::CollisionToolkit.TraceLineEntities(vStart,vEnd,traceCollection, &traceInfo, iFlags);
    if (iTraceCount<1) return;

    pResult->SetShapeHit_IfCloser(ownerShape,traceInfo.distance);
  }


  bool EngineInstanceClothObject::OnExport(SceneExportInfo ^info) 
  {
    Debug::Assert( GetClothEntity() != nullptr );
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << GetClothEntity();
    return true;
  }


  void EngineInstanceClothObject::AddConstraint(EngineInstanceConstraint ^pConstraint)
  {
    VClothMesh* pMesh = GetClothMesh();
    if (!pMesh)
      return;

    pMesh->RemoveConstraint(pConstraint->GetConstraint());
    pMesh->AddConstraint(pConstraint->GetConstraint(),false);
  }

  void EngineInstanceClothObject::RemoveConstraint(EngineInstanceConstraint ^pConstraint)
  {
    VClothMesh* pMesh = GetClothMesh();
    if (pMesh)
      pMesh->RemoveConstraint(pConstraint->GetConstraint());
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
