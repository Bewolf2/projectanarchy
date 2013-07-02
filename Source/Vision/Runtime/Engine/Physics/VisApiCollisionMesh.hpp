/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCollisionMesh.hpp

#ifndef VISAPICOLLISIONMESH_HPP_INCLUDED
#define VISAPICOLLISIONMESH_HPP_INCLUDED

#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>
#include <Vision/Runtime/Engine/Mesh/VisVertexBufferManagers.hpp>
#include <Vision/Runtime/Engine/Physics/VisApiCollisionMeshBase.hpp>

// flags use for rendering a collision mesh
#define COLMESH_RENDERFLAG_MESH              0x00000001
#define COLMESH_RENDERFLAG_NORMALS           0x00000002
#define COLMESH_RENDERFLAG_BOXES             0x00000004
#define COLMESH_RENDERFLAG_PRIMITIVENUMBER   0x00000008
#define COLMESH_RENDERFLAG_ENSURELOADED      0x00000100
#define COLMESH_RENDERFLAG_DEFAULT           (COLMESH_RENDERFLAG_MESH|COLMESH_RENDERFLAG_NORMALS)

#define COLMESH_VERSION_UNDEFINED            0x00000000
#define COLMESH_VERSION_1                    0x00000001
#define COLMESH_VERSION_2                    0x00000002
#define COLMESH_VERSION_3                    0x00000003 // introduced file time        
#define COLMESH_VERSION_CURRENT              COLMESH_VERSION_3


// other forward declarations:
struct VisColMeshTriArray_t;
struct VisColMeshIndexedTriSet_t;
struct VisTraceLineInfo_t;

class VisMeshBuffer_cl;
class VisBaseEntity_cl; 
class VDynamicMesh;

class VRigidCollisionMesh;
typedef VSmartPtr<VRigidCollisionMesh> VRigidCollisionMeshPtr;


/// \brief
///   Implements IVCollisionMesh for non-animated meshes (used for non-animated entities or static meshes).
///
/// Since for non-animated meshes the collision mesh is not instance specific, it is shared across instances.
/// See VBaseMesh::GetTraceMesh and VBaseMesh::GetCollisionMesh
class VRigidCollisionMesh : public IVCollisionMesh
{
public:
  VISION_APIFUNC VRigidCollisionMesh(VBaseMesh *pMesh, VisCollisionMeshSource_e eSource, const char *szFilename);
  VISION_APIFUNC VOVERRIDE ~VRigidCollisionMesh();

  VISION_APIFUNC VOVERRIDE BOOL Reload();

  VISION_APIFUNC bool LoadFromFile(const char *szFilename);

  VISION_APIFUNC VOVERRIDE VBaseSubmesh **GetTriangleSubmeshPtr(bool bForceCreate = false);
  VISION_APIFUNC VOVERRIDE hkvPlane* GetTrianglePlanesPtr(char **ppLeastSig, bool bForceCreate = false);
  VISION_APIFUNC VOVERRIDE hkvAlignedBBox* GetTriangleBoxesPtr(bool bForceCreate = false);
  VISION_APIFUNC VOVERRIDE unsigned char *GetTriangleFlagsPtr(bool bForceCreate = false);
  VISION_APIFUNC VOVERRIDE hkvVec2* GetVertexBaseUV(bool bForceCreate = false);

  VISION_APIFUNC void CreateFromRenderMesh();

protected:
  void CreateFromMeshBuffer(VisMeshBuffer_cl* pMeshBuffer);
};


/// \brief
///   Loader class for collision meshes.
class VCollisionMeshLoader : public VChunkFile
{
public:
  VCollisionMeshLoader(VRigidCollisionMesh *pCollMesh);

  VOVERRIDE void OnStartLoading();
  VOVERRIDE void OnFinishLoading();

  VOVERRIDE BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  VOVERRIDE void OnError(const char *szError, CHUNKIDTYPE chunkID, int iChunkOfs);
  VOVERRIDE void OnWarning(const char *szWarning, CHUNKIDTYPE chunkID, int iChunkOfs);

private:
  friend class IVCollisionMesh;
  friend class VRigidCollisionMesh;

  bool m_bAlreadyLoading;
  int m_iVersion;
  VRigidCollisionMesh *m_pCollMesh;

  char m_szPath[FS_MAX_PATH]; ///< path for prepending texture loading
};



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
