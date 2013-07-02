/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiCollisionMeshBase.hpp

#ifndef VISAPICOLLISIONMESHBASE_HPP_INCLUDED
#define VISAPICOLLISIONMESHBASE_HPP_INCLUDED

#include <Vision/Runtime/Base/Physics/VCollisionMeshBase.hpp>

/// \brief
///   Enum that defines collision mesh types. See IVCollisionMesh::GetSceneElementType
enum VColMeshSceneElement_e
{
  V_COLMESH_SCENEELEMENT_UNKNOWN = 0,
  V_COLMESH_SCENEELEMENT_ENTITY = 1,
  V_COLMESH_SCENEELEMENT_STATICMESH = 2,
  V_COLMESH_SCENEELEMENT_TERRAIN = 3
};

// flags use for rendering a collision mesh
#define COLMESH_RENDERFLAG_MESH              0x00000001
#define COLMESH_RENDERFLAG_NORMALS           0x00000002
#define COLMESH_RENDERFLAG_BOXES             0x00000004
#define COLMESH_RENDERFLAG_PRIMITIVENUMBER   0x00000008
#define COLMESH_RENDERFLAG_ENSURELOADED      0x00000100
#define COLMESH_RENDERFLAG_DEFAULT           (COLMESH_RENDERFLAG_MESH|COLMESH_RENDERFLAG_NORMALS)


class IVCollisionMesh;
typedef VSmartPtr<IVCollisionMesh> IVCollisionMeshPtr;

class VBaseMesh;
class VBaseSubmesh;
class VTraceHit;
struct VisColMeshTriArray_t;
struct VisColMeshIndexedTriSet_t;
struct VisTraceLineInfo_t;
class VisStaticSubmeshInstance_cl;
class VColMeshMaterial;
class VPhysicsSubmesh;
struct VisTraceLineInfo_t;

/// \brief
///   Enumeration specifying the source of a collision mesh (render mesh, file, shadow mesh, or custom geometry)
enum VisCollisionMeshSource_e
{
  VIS_COLMESH_SOURCE_RENDERMESH = 1,          ///< Collision mesh was generated from the render mesh
  VIS_COLMESH_SOURCE_CUSTOM = 2,              ///< Collision mesh was generated from custom geometry
  VIS_COLMESH_SOURCE_FILE = 3,                ///< Collision mesh was loaded from a .vcolmesh file
  VIS_COLMESH_SOURCE_SHADOWMESH = 4           ///< Collision mesh was generated from the shadow mesh
};

/// \brief
///  Base class for trace results
class VTraceHit
{
public:
  /// \brief constructor
  VTraceHit() {}
  /// \brief destructor
  virtual ~VTraceHit() {}

  /// \brief fills a VisTraceLineInfo_t with the necessary information
  VISION_APIFUNC virtual bool FillTraceHit(VisTraceLineInfo_t *pTraceInfo, IVCollisionMesh *pColMesh, int iTriangleIndex, bool bIgnoreTransparentSurfaces);
};


/// \brief
///   Shared (abstract) base class for collision meshes. 
/// 
/// This base class represents a generic collision mesh. It is used by the raycasting, tracing, and physics
/// features of the Vision engine in order to perform CPU-side operations on the mesh data. 
///
/// There are two specializations of this class: The VRigidCollisionMesh, representing a rigid collision mesh
/// (e.g. static mesh, non-animated entity), and the VAnimatedCollisionMesh, representing a collision mesh
/// which supports mesh deformations like skinning and vertex animations/morph targets.
class IVCollisionMesh : public VManagedResource
{
public:

  /// \brief
  ///   Constructor of the IVCollisionMesh class.
  IVCollisionMesh(VBaseMesh *pMesh, VisCollisionMeshSource_e eSource, const char *szFilename);

  virtual ~IVCollisionMesh();
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;

  /// \brief
  ///   Pure virtual function to retrieve an array that contains the submesh pointer for each triangle of this collision mesh (one submesh per triangle).
  VISION_APIFUNC virtual VBaseSubmesh **GetTriangleSubmeshPtr(bool bForceCreate = false) = 0;

  /// \brief
  ///   Pure virtual function to retrieve the triangle plane array of this collision mesh (one plane per triangle).
  VISION_APIFUNC virtual hkvPlane* GetTrianglePlanesPtr(char **ppLeastSig, bool bForceCreate = false) = 0;

  /// \brief
  ///   Pure virtual function to retrieve the triangle bounding box array of this collision mesh (one box per triangle).
  VISION_APIFUNC virtual hkvAlignedBBox* GetTriangleBoxesPtr(bool bForceCreate = false) = 0;

  /// \brief
  ///   Pure virtual function to retrieve the triangle flags array of this collision mesh (one 8-bit flag per triangle).
  VISION_APIFUNC virtual unsigned char *GetTriangleFlagsPtr(bool bForceCreate = false) = 0;

  /// \brief
  ///   Pure virtual function to retrieve the UV coordinate array of this collision mesh (three UV vectors per triangle).
  VISION_APIFUNC virtual hkvVec2* GetVertexBaseUV(bool bForceCreate = false) = 0;

  /// \brief
  ///   Return the number of vertices in this mesh.
  inline int GetVertexCount() const { return m_pBaseMesh->GetVertexCount(); }

  /// \brief
  ///   Return the number of triangles in this mesh.
  inline int GetTriangleCount() const { return m_pBaseMesh->GetIndexCount() / 3; }

  /// \brief
  ///   Return the number of materials in this mesh.
  inline int GetMaterialCount() const { return m_iNumMaterials; }

  /// \brief
  ///   Return the number of submeshes in this mesh.
  inline int GetSubmeshCount() const { return m_iNumSubmeshes; }

  /// \brief
  ///   Return the number of triangle surface indices in this mesh.
  inline int GetTriSrfIndexCount() const 
  { 
    return m_pTriSrfIndices ? GetTriangleCount() : 0;
  }

  /// \brief
  ///   Returns the internal mesh data.
  inline VSimpleCollisionMeshBase *GetMeshData() const { return m_pBaseMesh; }

  /// \brief
  ///   Returns a pointer to the submeshes.
  inline VPhysicsSubmesh *GetSubmeshes() const { return m_pSubmeshes; }

  /// \brief
  ///   Returns a pointer to the materials.
  inline VColMeshMaterial *GetMaterials() const { return m_pMaterials; }

  /// \brief
  ///   Return a pointer to the triangle surface indices.
  inline short *GetTriSrfIndices() const { return m_pTriSrfIndices; }

  /// \brief
  ///   Returns an enum that defines the origin of this mesh.
  inline VisCollisionMeshSource_e GetSource() const { return m_eSource; }

  /// \brief
  ///   Returns an enum that defines the topology of this mesh.
  inline VisColMeshType_e GetColmeshType() const { return m_eMeshType; }

  /// \brief
  ///   Internal function
  VISION_APIFUNC void SetMeshData(VSimpleCollisionMeshBase *pMesh);

  /// \brief
  ///   Returns a pointer to the vertex list and the number of vertices.
  VISION_APIFUNC int GetVertexList(hkvVec3* &pVertexList);

  /// \brief
  ///   Returns a pointer to the index list, index type and the number of vertices.
  VISION_APIFUNC int GetIndexList(void* &pIndexList, int &iIndexType);

  // Legacy:
  VISION_APIFUNC BOOL GetCollisionMeshInfo(VisColMeshTriArray_t &info);
  VISION_APIFUNC BOOL GetCollisionMeshInfo(VisColMeshIndexedTriSet_t &info);

  /// \brief
  ///   Returns the UV coordinates of the center of a hit triangle.
  VISION_APIFUNC hkvVec2 GetBaseUVFromTriangle(int hitTriIndex);

  /// \brief
  ///  Returns the UV coordinates of a touch point.
  VISION_APIFUNC hkvVec2 GetBaseUVFromTouchPoint(int hitTriIndex, hkvVec3 touchPointinLocalSpace);

  inline const VString *GetUserDataStrings() const { return m_pUserDataStrings; }
  inline int GetUserDataStringCount() const { return m_iNumUserDataStrings; }
  inline const short* GetUserDataReferences() const { return m_pUserDataRefs; }

  VISION_APIFUNC BOOL DebugRender(VColorRef iMeshColor=V_RGBA_WHITE, VColorRef iNormalColor=V_RGBA_GREEN, float fNormalLength=5.f, int iRenderFlags=COLMESH_RENDERFLAG_DEFAULT, const hkvMat4* pTransform = NULL);
  VISION_APIFUNC BOOL DebugRender(int iTriangleFlags, VColorRef iMeshColor=V_RGBA_WHITE, VColorRef iNormalColor=V_RGBA_GREEN, float fNormalLength=5.f, int iRenderFlags=COLMESH_RENDERFLAG_DEFAULT, const hkvMat4* pTransform = NULL);
  VISION_APIFUNC BOOL DebugRender(const hkvMat4& transform, int iTriangleFlags = 0, VColorRef iMeshColor=V_RGBA_WHITE, VColorRef iNormalColor=V_RGBA_GREEN, float fNormalLength=5.f, int iRenderFlags=COLMESH_RENDERFLAG_DEFAULT);


  VISION_APIFUNC float GetDistance(const hkvMat4* pTransform, const hkvAlignedBBox &RefBox, int iFirstTri, int iNumTris);

  VISION_APIFUNC int PerformIndexedCollisionMeshTraceTest(VisBaseEntity_cl *pEntity,const hkvMat4* pTransform, const hkvVec3& vStartPos, const hkvVec3& vTargetPos, float fDistanceFactor, const hkvAlignedBBox& box, BOOL bDoublesided, int iFirstTri, int iNumTris, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, bool bIgnoreTransparent);
  VISION_APIFUNC int PerformIndexedCollisionMeshTraceTest(VisStaticGeometryInstance_cl *pGeoInstance,const hkvMat4* pTransform, const hkvVec3& vStartPos, const hkvVec3& vTargetPos, float fDistanceFactor, const hkvAlignedBBox& box, BOOL bDoublesided, int iFirstTri, int iNumTris, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, bool bIgnoreTransparent);
  VISION_APIFUNC int PerformIndexedCollisionMeshTraceTest(VTraceHit *pTraceHit,const hkvMat4* pTransform, const hkvVec3& vStartPos, const hkvVec3& vTargetPos, float fDistanceFactor, const hkvAlignedBBox& box, BOOL bDoublesided, int iFirstTri, int iNumTris, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, bool bIgnoreTransparent);

  inline void SetSceneElementType(VColMeshSceneElement_e eSceneElementType) { m_eSceneElement = eSceneElementType; }
  inline VColMeshSceneElement_e GetSceneElementType() const { return m_eSceneElement; }

  inline VisCollisionMeshGeoType_e GetType() const { return m_eGeoType; }

  /// \brief
  ///   Gets the file time that corresponds to the system time at which collision mesh had been exported.
  inline __int64 GetFileTime() const { return m_iFileTime; }

protected:
  friend class VCollisionMeshLoader;

  VOVERRIDE int CalculateInternalSize();
  void Allocate(int iVertexCount, int iIndexCount, int iPrimCount, int iIndexType);
  VColMeshMaterial* AllocateMaterials(int iMaterialCount);
  VPhysicsSubmesh* AllocateSubmeshes(int iSubmeshCount);
  BOOL IsValidBaseUVInTraceLineInfo(const VisTraceLineInfo_t &info);
  VBaseMesh *m_pRenderMesh;
  VSimpleCollisionMeshBase *m_pBaseMesh;          ///< just the vertices/indices, used for old models and anything else
  
  VColMeshMaterial *m_pMaterials;
  int m_iNumMaterials;
  VPhysicsSubmesh *m_pSubmeshes;
  int m_iNumSubmeshes;
  short *m_pTriSrfIndices;

  hkvVec2* m_pBaseUV;              ///< array of base texture UV coordinates, computed on demand
  VBaseSubmesh **m_pTriangleSubmeshRef;      
  unsigned char *m_pTriangleFlags;        ///< array of triangle flags, computed on demand
  hkvPlane* m_pPlane;                  ///< array collision planes, computed on demand
  char *m_pLeastSig;                      ///< array of index of least significant plane component. In parallel to planes
  hkvAlignedBBox* m_pBBox;                        ///< array collision boxes, computed on demand

  VString *m_pUserDataStrings;
  short *m_pUserDataRefs;
  int m_iNumUserDataStrings;

  VisCollisionMeshSource_e m_eSource;
  VisColMeshType_e m_eMeshType;
  VColMeshSceneElement_e m_eSceneElement;
  VisCollisionMeshGeoType_e m_eGeoType; 
  __int64 m_iFileTime; // system time at which collision mesh had been exported
};



/// \brief
///   Resource manager class that holds instances of type IVCollisionMesh.
///
/// One global instance can be accessed via Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_COLLISIONMESHES).
class VCollisionMeshManager : public VisResourceManager_cl
{
public:
  VCollisionMeshManager();
  virtual ~VCollisionMeshManager();

  int CheckSubmeshCollision(VisStaticSubmeshInstance_cl *pSubmeshInst, const hkvMat4* pTransform, const hkvVec3& vStart, const hkvVec3& vTarget, float fDistanceFactor, const hkvAlignedBBox& box, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, bool bIgnoreTransparent);
  int CheckEntityCollision(VisBaseEntity_cl *pEntity, const hkvMat4* pTransform, const hkvVec3& vStart, const hkvVec3& vTarget, float fDistanceFactor, const hkvAlignedBBox& box, int iStoreResultCount, VisTraceLineInfo_t *pFirstTraceInfo, bool bIgnoreTransparent);

  // debug:
  int RenderAllCollisionMeshes(VColorRef iMeshColor, VColorRef iNormalColor, float fNormalLength,int iRenderFlags);
};


#endif //VISAPICOLLISIONMESHBASE_HPP_INCLUDED

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
