/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiStaticMesh.hpp

#ifndef DEFINE_VISAPISTATICMESH
#define DEFINE_VISAPISTATICMESH

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiBaseMesh.hpp>

class VisStaticSubmesh_cl;
class VisStaticMeshInstance_cl;
class VisStaticMesh_cl;
class VMeshManager;
class IVCollisionMesh;


// define smartpointer
typedef VSmartPtr<VisStaticMesh_cl> VisStaticMeshPtr;
typedef VSmartPtr<IVCollisionMesh> IVCollisionMeshPtr;


/// \brief
///   Derived class for loading a VisStaticMesh_cl resource 
class VStaticMeshSerializationProxy : public IVSerializationProxy
{
public:
  inline VStaticMeshSerializationProxy(VisStaticMesh_cl *pStaticMesh) 
  {
    VASSERT(pStaticMesh); 
    m_pStaticMesh=pStaticMesh;
  }

  VISION_APIFUNC VOVERRIDE IVSerializationProxyCreator *GetInstance();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

protected:
  // serialization
  inline VStaticMeshSerializationProxy() 
  {
    m_pStaticMesh = NULL;
  }

  V_DECLARE_SERIAL_DLLEXP( VStaticMeshSerializationProxy, VISION_APIDATA )

  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );

private:
  VisStaticMesh_cl *m_pStaticMesh;
};


/// \brief
///   Class representing a static mesh (typically corresponding to a vmesh file, but VisStaticMesh_cl
/// objects can also be created with code).
/// 
/// VisStaticMeshInstance_cl objects can be created by calling VisStaticMesh_cl::CreateInstance. 
/// 
/// For better rendering and scene management performance, static meshes are split into submeshes,
/// each of which is a VisStaticSubmesh_cl, and which are in turn automatically instanced when a
/// VisStaticMesh_cl is instanced. 
class VisStaticMesh_cl : public VBaseMesh
{
public:

  ///
  /// @name Construction and Destruction; Allocation/deallocation
  /// @{
  ///

  /// \brief
  ///   Default constructor of the static mesh class. Creates an empty static mesh object.
  VISION_APIFUNC VisStaticMesh_cl();

  /// \brief
  ///   Constructor of the static mesh class. 
  /// 
  /// This constructor will not load the passed vmesh file. The vmesh is not valid for rendering or
  /// tracing until EnsureLoaded() has been called on it.
  /// 
  /// For simply loading a vmesh from file, you should use
  /// VMeshManager::LoadStaticMeshFile instead.
  /// 
  /// \param szFileName
  ///   File name (including .vmesh extension) of the mesh this VisStaticMesh_cl should be loaded
  ///   from.
  VISION_APIFUNC VisStaticMesh_cl(const char *szFileName);

  /// \brief
  ///   Destructor of the static mesh class.
  VISION_APIFUNC virtual ~VisStaticMesh_cl();

  /// \brief
  ///   Allocates the specified number of submeshes submeshes for a static mesh.
  /// 
  /// Note that you will only have to call this function if you create VisStaticMesh_cl objects in
  /// code; it is not required to call this method for meshes loaded from a vmesh file.
  /// 
  /// \param iNumMeshes
  ///   Number of submeshes to allocate.
  VISION_APIFUNC VOVERRIDE void AllocateSubmeshes(int iNumMeshes);

  /// \brief
  ///   Frees the submeshes for a static mesh.
  VISION_APIFUNC VOVERRIDE void FreeSubmeshes();

  ///
  /// @}
  ///

  ///
  /// @name Submesh Access
  /// @{
  ///

  /// \brief
  ///   Returns the submesh with the specified index.
  VISION_APIFUNC VOVERRIDE VBaseSubmesh *GetBaseSubmesh(int iMeshIndex) const;

  /// \brief
  ///   Returns the submesh with the specified index.
  inline VisStaticSubmesh_cl *GetSubmesh(int iMeshIndex) const;

  ///
  /// @}
  ///


  ///
  /// @name Instance Creation
  /// @{
  ///

  /// \brief
  ///   Creates an instance of this static mesh.
  /// 
  /// VisStaticSubmeshInstance_cl instances will automatically be created from the
  /// VisStaticSubmesh_cl objects contained in this static mesh. 
  /// 
  /// Note that after creating a VisStaticMeshInstance_cl, Vision's default VisibilityCollector
  /// implementation still requires the instance's submesh instances to be assigned to visibility
  /// zones.
  /// 
  /// \param mTransform
  ///   Transformation matrix for the new mesh instance.
  /// 
  /// \return
  ///   VisStaticMeshInstance_cl *: the newly created VisStaticMeshInstance_cl.
  inline VisStaticMeshInstance_cl *CreateInstance(const hkvMat4& mTransform)
  {
    return CreateInstance(mTransform, NULL);
  }

  /// \brief
  ///   This override allows for providing an instance to fill out. If the passed pInstance is
  ///   !=NULL then this is the return value
  VISION_APIFUNC VisStaticMeshInstance_cl *CreateInstance(const hkvMat4& mTransform, 
    VisStaticMeshInstance_cl *pInstance);

  /// \brief
  ///   This override allows to specify whether the OnStaticMeshInstanceCreated method is triggered on the 
  ///   currently installed physics module once the instance has been created. 
  VISION_APIFUNC VisStaticMeshInstance_cl *CreateInstance(const hkvMat4& mTransform, 
    VisStaticMeshInstance_cl *pInstance, bool bNotifyPhysics);

  ///
  /// @}
  ///

  ///
  /// @name Resource Handling
  /// @{
  ///
#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Overridden function. See base class for details.
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Called internally to update the memory consumption for this mesh
  VISION_APIFUNC VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  /// \brief
  ///   Unloads this static mesh.
  VISION_APIFUNC BOOL Unload();

  /// \brief
  ///   Reloads this static mesh.
  /// 
  /// Overridden function; If the static mesh was originally generated
  /// from a .vmesh file, the file will be reloaded.
  /// 
  /// \return
  ///   BOOL: TRUE if the Reload was successful.
  VISION_APIFUNC BOOL Reload();

  /// \brief
  ///   Loads a static mesh (.vmesh) file.
  VISION_APIFUNC static VisStaticMesh_cl *LoadStaticMesh(const char *szFilename);

  /// \brief
  ///   Looks for a static mesh loaded from a .vmesh file and returns the mesh if it was found.
  ///   Otherwise, NULL is returned.
  VISION_APIFUNC static VisStaticMesh_cl *FindStaticMesh(const char *szFilename);

  /// \brief
  ///   Returns the global resource manager responsible for handling static meshes.
  VISION_APIFUNC static VMeshManager &GetResourceManager();

  /// \brief
  ///   Returns whether the mesh was loaded from file or not.
  VISION_APIFUNC BOOL IsLoadFromFile() const { return m_bLoadFromFile; }


  ///
  /// @}
  ///

  ///


  ///
  /// @name Miscellaneous
  /// @{
  ///
  
  /// \brief
  ///   Returns the lightmap page index of this static mesh.
  inline int GetLightmapPageIndex(int iSubmeshIndex) const {return m_pForceLightmapPageIndices ? m_pForceLightmapPageIndices[iSubmeshIndex] : -1;}

#ifdef SUPPORTS_SNAPSHOT_CREATION

  /// \brief
  ///   Saves a resources.xml file into the model's subfolder. This file can be loaded by the
  ///   VResourceSnapshot class
  VISION_APIFUNC VOVERRIDE bool SaveResourceDependencyXML(VString *psErrorRes = NULL);

#endif

  /// \brief
  ///   Overridden function. Uses the global VisRenderer_cl::GetUseSingleBufferedStaticMeshes() status
  VISION_APIFUNC VOVERRIDE bool GetUseDoubleBuffering() const;

  VOVERRIDE IVSerializationProxy *CreateProxy() { return new VStaticMeshSerializationProxy(this); }

  ///
  /// @}
  ///

protected:
  friend class VisStaticMeshLoader_cl;
  friend class VMeshManager;

  // Protected functions
  VISION_APIFUNC void Init();

  VISION_APIFUNC virtual bool LoadFromFile(const char *szFilename) HKV_OVERRIDE; 

  VisStaticSubmesh_cl *m_pSubmeshes;

  int *m_pForceLightmapPageIndices; ///< optional pointer to m_iNumSubmeshes integer values
};






/// \brief
///   Class representing a renderable subset of a static mesh (VisStaticMesh_cl).
/// 
/// Submesh are a part of a static mesh. They only have a single material ("surface") and a single
/// lightmap. Each submesh has to be renderable with a single graphics API rendering call and may
/// only reference a range of up to 32767 vertices in the static mesh instance.
/// 
/// VisStaticSubmesh_cl are the basis for the VisStaticSubmeshInstance_cl objects, which are
/// implicitly created as part of the VisStaticMeshInstance_cl object in
/// VisStaticMesh_cl::CreateInstance calls.
/// 
/// VisStaticSubmesh_cl objects can not be created directly.
/// 
/// Submeshes don't have their own vertex and index information, but reference the render mesh /
/// shadow mesh data in the VisStaticMesh_cl they are part of.
/// 
/// Writing data to submeshes is typically only required when creating VisStaticMesh_cl objects
/// manually.
class VisStaticSubmesh_cl : public VBaseSubmesh
{
public:

  /// \brief
  ///   Virtual destructor
  VOVERRIDE ~VisStaticSubmesh_cl();

  /// \brief
  ///   Returns the static mesh the submesh is part of.
  inline VisStaticMesh_cl *GetMesh() const { return (VisStaticMesh_cl *)m_pMesh; }

protected:
  friend class VisStaticMesh_cl;
  friend class VisStaticMeshInstance_cl;

  VisStaticSubmesh_cl();
};


/// \brief
///   Internal loader class for loading .vmesh files.
class VisStaticMeshLoader_cl : public VBaseMeshLoader
{
public:
  VisStaticMeshLoader_cl(VisStaticMesh_cl *pGroup);

  VOVERRIDE void OnStartLoading();
  VOVERRIDE void OnFinishLoading();

  VOVERRIDE BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  VOVERRIDE void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);
  VOVERRIDE void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs);

  class VModelInfoXMLDocument *m_pInfoDoc;
};





inline VisStaticSubmesh_cl *VisStaticMesh_cl::GetSubmesh(int iMeshIndex) const
{ 
  VASSERT (iMeshIndex>=0 && iMeshIndex < m_iNumSubmeshes); 
  return &(m_pSubmeshes[iMeshIndex]); 
}

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
