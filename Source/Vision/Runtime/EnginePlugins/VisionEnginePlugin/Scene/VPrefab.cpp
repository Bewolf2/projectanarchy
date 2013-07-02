/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>

VPrefabManager VPrefabManager::g_GlobalManager("Prefabs");


VPrefabManager& VPrefabManager::GlobalManager()
{
  return g_GlobalManager;
}

void VPrefabManager::OneTimeInit()
{
 Vision::ResourceSystem.RegisterResourceManager(this, VColorRef(200,100,0));
}

void VPrefabManager::OneTimeDeInit()
{
 Vision::ResourceSystem.UnregisterResourceManager(this);
}

// internal class to provide shapes with custom transformation through GetCustomShapeTransformation
class VPrefabShapesArchive : public VShapesArchive
{
public:
  VPrefabShapesArchive(IVFileInStream* pStream, VPrefabInstanceInfo &target)
    : VShapesArchive(pStream,NULL), m_TargetInfo(target)
  {
    m_RotationMat.setFromEulerAngles (target.m_vInstanceEuler.z, target.m_vInstanceEuler.y, target.m_vInstanceEuler.x);

    m_bIsIdentity = (target.m_vInstancePos.isZero ()) && (target.m_vInstanceEuler.isZero ());
  }

  virtual BOOL GetCustomShapeTransformation(hkvVec3& offset, hkvMat3 &rotation, hkvVec3& rotationAsEuler)
  {
    if (m_bIsIdentity)
      return FALSE;
    offset = m_TargetInfo.m_vInstancePos;
    rotation = m_RotationMat;
    rotationAsEuler = m_TargetInfo.m_vInstanceEuler;
    return TRUE;
  }

  bool m_bIsIdentity;
  hkvMat3 m_RotationMat;
  VPrefabInstanceInfo &m_TargetInfo;
};



VManagedResource *VPrefabManager::CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
{
  VPrefab *pPrefab = new VPrefab(this);
  pPrefab->SetFilename(szFilename);
  pPrefab->EnsureLoaded();
  return pPrefab;
}



VPrefab::VPrefab(VResourceManager *pParentManager) : VManagedResource(pParentManager)
{
  m_iSize = 0;
  SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
}


BOOL VPrefab::Unload()
{
  m_iSize = 0;
  m_BinaryBlock.FreeBuffer();
  return TRUE;
}

BOOL VPrefab::Reload()
{
  if (IsMissing())
    return FALSE;
  IVFileInStream *pIn;
  if (GetParentManager()!=NULL)
    pIn = GetParentManager()->CreateFileInStream(GetFilename(),this);
  else
    pIn = Vision::File.Open(GetFilename());
  if (pIn==NULL)
  {
    FlagAsMissing();
    return FALSE;
  }
  // must match saving code by vForge
  pIn->Read(&m_Header,sizeof(m_Header),"6i");
  m_iSize = pIn->GetSize() - sizeof(m_Header);

  // header validation:
  if (m_iSize<0 || m_Header.m_iArchiveVersion<0 || m_Header.m_iArchiveVersion>Vision::GetArchiveVersion() 
   || m_Header.m_iLocalVersion<0 || m_Header.m_iLocalVersion>VPREFAB_BINARY_VERSION_CURRENT)
  {
    pIn->Close();
    Vision::Error.Warning("Cannot load VPrefab '%s': Invalid version or broken file", GetFilename());
    FlagAsMissing();
    return FALSE;
  }

  m_BinaryBlock.EnsureCapacity(m_iSize);
  pIn->Read(m_BinaryBlock.GetBuffer(),m_iSize);

  pIn->Close();

  return TRUE;
}


BOOL VPrefab::Instantiate(VPrefabInstanceInfo &info)
{
  EnsureLoaded();
  if (!IsLoaded())
    return FALSE;

  // serialize from memory block (class VPrefabShapesArchive provides with transformation)
  VMemBlockWrapperStream inStream(m_BinaryBlock.GetBuffer(), m_iSize);
  VPrefabShapesArchive ar(&inStream, info);
  ar.SetLoadingVersion(m_Header.m_iArchiveVersion);

  if (info.m_bOutputInstances)
    info.m_Instances.EnsureSize(m_Header.m_iRootObjectCount);

  info.m_iInstanceCount = m_Header.m_iRootObjectCount;
  for (int i=0;i<m_Header.m_iRootObjectCount;i++)
  {
    VTypedObject *pObj = ar.ReadObject(NULL);
    if (info.m_bOutputInstances)
      info.m_Instances[i] = pObj;
    if (info.m_pParentObject!=NULL && pObj!=NULL && pObj->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
    {
      VisObject3D_cl *pObj3D = (VisObject3D_cl *)pObj;
      if (pObj3D->GetParent()==NULL) // this is how we identify the root elements
        pObj3D->AttachToParent(info.m_pParentObject);
    }
  }

  ar.Close();

  return TRUE;
}

/// Start of VPrefabInstance implementation
VPrefabInstance::VPrefabInstance() : VisBaseEntity_cl()
{
}

V_IMPLEMENT_SERIAL( VPrefabInstance, VisBaseEntity_cl, 0, &g_VisionEngineModule );

void VPrefabInstance::Serialize( VArchive& ar )
{
  VisBaseEntity_cl::Serialize(ar);

  char prefabVersion = 0;
  if(ar.IsLoading())
  {
    char loc_sFilename[FS_MAX_PATH];
    ar >> prefabVersion; VASSERT_MSG(prefabVersion==0, "Invalid version number, please re-export")
    ar.ReadStringBinary(loc_sFilename);
    VPrefab* loadedPrefab = VPrefabManager::GlobalManager().LoadPrefab(loc_sFilename);
    VPrefabInstanceInfo info;
    info.m_pParentObject = this;
    info.m_bOutputInstances = false;
    info.m_vInstancePos = GetPosition();
    info.m_vInstanceEuler = GetOrientation();
    loadedPrefab->Instantiate(info);
  }
  else
  {
    ar << prefabVersion;
    ar << m_sFilename;
  }
}

const char* VPrefabInstance::GetFileName() const
{
  return m_sFilename;
}
void VPrefabInstance::SetFileName(const char* file)
{
  m_sFilename = file;
}

START_VAR_TABLE(VPrefabInstance, VisBaseEntity_cl, "VPrefabInstance", VFORGE_HIDECLASS, "")  
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
