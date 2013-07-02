/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeManager.hpp>

extern VModule g_VisionEngineModule;

VCustomVolumeObject::VCustomVolumeObject()
{
  m_bCreatedFromEditor = false;
  m_bCustomStaticMesh = true;
  m_vScale.set(1.0f, 1.0f, 1.0f);
  Vision::Callbacks.OnAfterSceneLoaded += this;
  m_iManagerIndex = VCustomVolumeManager::GlobalManager().AddInstance(this);
}

VCustomVolumeObject::~VCustomVolumeObject()
{ 
  if(!m_bCreatedFromEditor)
  {
    Deregister();
  }
}

void VCustomVolumeObject::DisposeObject()
{
  VisObject3D_cl::DisposeObject();
  m_spStaticMesh = NULL;
}

void VCustomVolumeObject::Init()
{
  LoadStaticMesh();
}

void VCustomVolumeObject::LoadStaticMesh()
{
  if(m_bCustomStaticMesh || !m_bCreatedFromEditor )
  {
    if(!m_sStaticMeshPath.IsEmpty())
    {
      Vision::GetApplication()->GetLoadingProgress().PushRange(0.f,100.f); // provide range for loading the mesh
      m_spStaticMesh = VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(m_sStaticMeshPath.AsChar());
      VASSERT_MSG(m_spStaticMesh != NULL, "Loading static mesh failed");
      Vision::GetApplication()->GetLoadingProgress().PopRange();
    }
    else {
      m_spStaticMesh = NULL;
    }
  }
  else {
    m_spStaticMesh = new VisStaticMesh_cl();
    m_spStaticMesh->SetSupportMaterialEditing(false);
    m_spStaticMesh->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
    VASSERT(Vision::Editor.IsInEditor());
  }
}

V_IMPLEMENT_SERIAL( VCustomVolumeObject, VisObject3D_cl, 0, &g_VisionEngineModule );
void VCustomVolumeObject::Serialize( VArchive &ar )
{
  VisObject3D_cl::Serialize(ar);
  if(ar.IsLoading())
  {
    unsigned int uiVersion = 0;
    ar >> uiVersion;
    VASSERT(uiVersion == VV_Version1);
    ar >> m_sStaticMeshPath;
    ar >> m_bCustomStaticMesh;
    m_vScale.SerializeAsVec3(ar);
  }
  else {
    ar << static_cast<unsigned int>(VV_CurrentVersion);
    ar << m_sStaticMeshPath;
    ar << m_bCustomStaticMesh;
    m_vScale.SerializeAsVec3(ar);
  }
}

void VCustomVolumeObject::OnSerialized(VArchive& ar)
{
  VisObject3D_cl::OnSerialized(ar);
  if(ar.IsLoading())
    Init();
}

void VCustomVolumeObject::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  VASSERT(pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded);

  if(m_spStaticMesh == NULL)
  {
    LoadStaticMesh();
  }
}

void VCustomVolumeObject::SetCustomStaticMesh(bool bValue)
{ 
  if(m_bCustomStaticMesh != bValue)
  {
    m_bCustomStaticMesh = bValue; 
    LoadStaticMesh();
  }
}

void VCustomVolumeObject::SetStaticMeshPath(const char* szPath)
{
  if(m_bCustomStaticMesh && m_sStaticMeshPath != szPath)
  {
    m_sStaticMeshPath = szPath;
    if(!m_sStaticMeshPath.IsEmpty())
      LoadStaticMesh();
    else
      m_spStaticMesh = NULL;
  }
  else
    m_sStaticMeshPath = szPath;
}

void VCustomVolumeObject::SetCreatedFromEditor()
{
  if(!m_bCreatedFromEditor)
  {
    Deregister();
  }
  m_bCreatedFromEditor = true;
}

void VCustomVolumeObject::Deregister()
{
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  VCustomVolumeManager::GlobalManager().RemoveInstance(m_iManagerIndex);
}

START_VAR_TABLE(VCustomVolumeObject, VisObject3D_cl, "Custom Volume Entity", VFORGE_HIDECLASS, "")
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
