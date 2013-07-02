/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Editing/MeshPaintingComponent.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL(VMeshPaintingComponent,IVObjectComponent,0,&g_VisionEngineModule);

bool VMeshPaintingComponent::g_bComponentsThroughUI = false;

VMeshPaintingComponent::VMeshPaintingComponent() :
    IVObjectComponent(NULL,VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE)
{
}

VMeshPaintingComponent::~VMeshPaintingComponent()
{
}

void VMeshPaintingComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  MarkOwnerBoundingBoxDirty(); // old mesh
  IVObjectComponent::SetOwner(pOwner);
  MarkOwnerBoundingBoxDirty(); // new mesh
  VEditableTerrain::ForceUpdateDecorationReceiverList();

  if (pOwner==NULL)
    VEditableTerrain::m_MeshPaintingComponents.SafeRemove(this); // might destroy this
  else
    VEditableTerrain::m_MeshPaintingComponents.AddUnique(this);
}


BOOL VMeshPaintingComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
#if defined( HK_ANARCHY )
  sErrorMsgOut = "Component type not supported in Anarchy.";
  return FALSE;
#else
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;
  if (pObject->IsOfType(V_RUNTIME_CLASS(VisStaticMeshInstance_cl)))
    return TRUE;
  return FALSE;
#endif
}

void VMeshPaintingComponent::MarkOwnerBoundingBoxDirty()
{
  if (g_bComponentsThroughUI)
    return;

  VisStaticMeshInstance_cl *pMesh = (VisStaticMeshInstance_cl *)GetOwner();
  if (pMesh==NULL)
    return;
  m_CurrentBBox = pMesh->GetBoundingBox();
  VEditableTerrain::AddDirtyVegetationRangeAllTerrains(m_CurrentBBox);
}



void VMeshPaintingComponent::OnOwnerTransformationChanged()
{
  VisStaticMeshInstance_cl *pMesh = (VisStaticMeshInstance_cl *)GetOwner();
  if (pMesh==NULL)
    return;

  // old and new location is dirty
  hkvAlignedBBox newBBox = pMesh->GetBoundingBox();
  if (newBBox!=m_CurrentBBox)
  {
    if (newBBox.isValid())
      VEditableTerrain::AddDirtyVegetationRangeAllTerrains(newBBox);
    if (m_CurrentBBox.isValid())
      VEditableTerrain::AddDirtyVegetationRangeAllTerrains(m_CurrentBBox);
    m_CurrentBBox = newBBox;
    VEditableTerrain::ForceUpdateDecorationReceiverList();
  }

}


START_VAR_TABLE(VMeshPaintingComponent,IVObjectComponent, "If attached to a static mesh, the mesh is a receiver for painting vegetation with the terrain editor.", VVARIABLELIST_FLAGS_NONE, "Mesh Decoration Painting" )
END_VAR_TABLE

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
