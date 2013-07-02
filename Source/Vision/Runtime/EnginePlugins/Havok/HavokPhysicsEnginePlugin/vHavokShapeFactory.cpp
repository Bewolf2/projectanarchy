/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCachedShape.hpp>

#ifdef SUPPORTS_TERRAIN
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSectorManager.hpp>
#endif

#include <Physics2012/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics2012/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldCollection.h>
#include <Physics2012/Collide/Shape/HeightField/TriSampledHeightField/hkpTriSampledHeightFieldBvTreeShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShapeCinfo.h> 

#include <Common/Base/Config/hkConfigVersion.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>
#include <Common/Base/Reflection/Registry/hkVtableClassRegistry.h>
#include <Common/Base/Reflection/hkClass.h>

// Initialize static members
vHavokShapeCache* vHavokShapeFactory::m_pShapeCacheTable = NULL;

// -------------------------------------------------------------------------- //
// Init/ Deinit                                                               //
// -------------------------------------------------------------------------- //

void vHavokShapeFactory::Init()
{
  // hkStoragStringMap allocates at construction already memory. Since at static construction time the memory system of Havok
  // is still not initialized, the creation must be done later.
  m_pShapeCacheTable = new vHavokShapeCache;
}

void vHavokShapeFactory::Deinit()
{
  V_SAFE_DELETE(m_pShapeCacheTable);
}


// -------------------------------------------------------------------------- //
// Havok Shape - Convex                                                       //
// -------------------------------------------------------------------------- //

hkRefNew<hkpShape> vHavokShapeFactory::CreateConvexHullShapeFromMesh(VBaseMesh *pMesh, const hkvVec3& vScaleIn, const char **szShapeCacheId, int iCreationFlags)
{
  VVERIFY_OR_RET_VAL(pMesh != NULL, NULL);

  const bool bAllowStaticMeshCaching = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableShapeCaching==TRUE;
  const bool bCacheShape = iCreationFlags & VShapeCreationFlags_CACHE_SHAPE;
  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);
  const bool bForceHktShapeCaching = pModule->IsHktShapeCachingEnforced();
  const bool bShrinkShape = iCreationFlags & VShapeCreationFlags_SHRINK;

  char szShapeId[512];
  if (bCacheShape)
  {
    VASSERT(szShapeCacheId != NULL);

    // Check whether shape has been already cached for this model with the respective scaling.
    vHavokShapeFactory::GetIDStringForConvexShape(szShapeId, pMesh->GetFilename(), vScaleIn, bShrinkShape);
    hkpShape *pCachedShape = FindShape(szShapeId, szShapeCacheId);
    if (pCachedShape)
    {
      pCachedShape->addReference();
      return pCachedShape;
    }
 
    // Check if shape is already cached on disk 
    if (bAllowStaticMeshCaching)
    {
      pCachedShape = vHavokCachedShape::LoadConvexShape(pMesh, vScaleIn, bShrinkShape);
      if (pCachedShape)
      {
        *szShapeCacheId = AddShape(szShapeId, pCachedShape);

  #ifdef HK_DEBUG_SLOW
        const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pCachedShape);
        HK_ASSERT2(0x695cc897, loadedClassType && (hkString::strCmp( loadedClassType->getName(), "hkvConvexVerticesShape" ) == 0), "Serialized in a unexpected cached Havok shape type!");
  #endif

        return pCachedShape;
      }
      else if(!Vision::Editor.IsInEditor() && !bForceHktShapeCaching)
      {
        Vision::Error.Warning("Cached HKT file for %s is missing. Please generate HKT file (see documentation for details).", pMesh->GetFilename());
      }
    }
  }

  // Get the collision mesh for the specified mesh
  const IVCollisionMesh *pColMesh = (iCreationFlags & VShapeCreationFlags_USE_VCOLMESH) ? pMesh->GetCollisionMesh(true, true) : pMesh->GetTraceMesh(true, true);
  VASSERT(pColMesh != NULL);

  hkvMat4 tranform;
  tranform.setScalingMatrix(vScaleIn);

  hkGeometry geom;

  const int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
  for (int i=0;i<iNumColMeshes;i++)
    BuildGeomFromCollisionMesh(pColMesh, i, tranform, true, geom);
  VVERIFY_OR_RET_VAL(geom.m_vertices.getSize() > 0, NULL);

  // Set the build configuration to set planes equations and connectivity automatically
  hkpConvexVerticesShape::BuildConfig config;
  config.m_createConnectivity = true;
  config.m_shrinkByConvexRadius = bShrinkShape;

  hkStridedVertices stridedVerts;
  stridedVerts.m_numVertices = geom.m_vertices.getSize();
  stridedVerts.m_striding    = sizeof(hkVector4);
  stridedVerts.m_vertices    = (const hkReal*)geom.m_vertices.begin();

  // Create convex shape
  hkvConvexVerticesShape *pConvexShape = new hkvConvexVerticesShape(pColMesh->GetFileTime(), stridedVerts, config);

  // Add shape to cache
  if (bCacheShape)
    *szShapeCacheId = AddShape(szShapeId, pConvexShape);

  // Only cache shape to HKT file when inside vForge or when enforced.
  if ((Vision::Editor.IsInEditor() && bAllowStaticMeshCaching && bCacheShape) || bForceHktShapeCaching)
    vHavokCachedShape::SaveConvexShape(pMesh, vScaleIn, bShrinkShape, pConvexShape);

  return pConvexShape;
}


// -------------------------------------------------------------------------- //
// Havok Shape - Mesh                                                         //
// -------------------------------------------------------------------------- //
hkRefNew<hkpShape> vHavokShapeFactory::CreateShapeFromMesh(VBaseMesh* pMesh, const hkvVec3& vScale, const char **szShapeCacheId,
                                                           int iCreationFlags, VisWeldingType_e eWeldingType)
{
  VVERIFY_OR_RET_VAL(pMesh != NULL, NULL);

  const bool bAllowStaticMeshCaching = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableShapeCaching==TRUE;
  const bool bCacheShape = iCreationFlags & VShapeCreationFlags_CACHE_SHAPE;  
  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);
  const bool bForceHktShapeCaching = pModule->IsHktShapeCachingEnforced();
  
  char szShapeId[512];
  if (bCacheShape)
  {
    VASSERT(szShapeCacheId != NULL);

    // Check whether shape has been already cached for this model with the respective scaling
    vHavokShapeFactory::GetIDStringForMeshShape(szShapeId, pMesh->GetFilename(), vScale, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, eWeldingType);
    hkpShape *pCachedShape = FindShape(szShapeId, szShapeCacheId);
    if (pCachedShape)
    {
      pCachedShape->addReference();
      return pCachedShape;
    }

    // Check if shape is already cached on disk
    if (bAllowStaticMeshCaching )
    {
      pCachedShape = vHavokCachedShape::LoadMeshShape(pMesh, vScale, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, eWeldingType);
      if (pCachedShape)
      {
        *szShapeCacheId = AddShape(szShapeId, pCachedShape);

#ifdef HK_DEBUG_SLOW
        const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pCachedShape);
        HK_ASSERT2(0x695cc897, loadedClassType && (hkString::strCmp( loadedClassType->getName(), "hkvBvCompressedMeshShape" ) == 0), "Serialized in a unexpected cached Havok shape type!");
#endif

        hkvBvCompressedMeshShape* pCompressedMeshShape = reinterpret_cast<hkvBvCompressedMeshShape*>(pCachedShape);
        pCompressedMeshShape->SetupMaterials(); // just to be sure we don't try to access it as material ptr ever
        return pCompressedMeshShape;
      }
      else if(!Vision::Editor.IsInEditor() && !bForceHktShapeCaching)
      {
        Vision::Error.Warning("Cached HKT file for %s is missing. Please generate HKT file (see documentation for details).", pMesh->GetFilename());
      }
    }
  }
 
  hkGeometry geom;

  // Get the collision mesh for the specified mesh
  IVCollisionMesh *pColMesh = (iCreationFlags & VShapeCreationFlags_USE_VCOLMESH) ? pMesh->GetCollisionMesh(true, true) : pMesh->GetTraceMesh(true, true);
  VASSERT(pColMesh != NULL);

  hkvMat4 tranform;
  tranform.setScalingMatrix(vScale);

  int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
  for (int i=0;i<iNumColMeshes;i++)
    BuildGeomFromCollisionMesh(pColMesh, i, tranform, false, geom);
  VVERIFY_OR_RET_VAL(geom.m_vertices.getSize() > 0, NULL);

  ///XX A DynamicMesh can be animated. We are treating it as static here.

  hkpDefaultBvCompressedMeshShapeCinfo ci( &geom );
  ci.m_collisionFilterInfoMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE; // Collision info
  ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE; // Materials
  ci.m_weldingType = vHavokConversionUtils::VisToHkWeldingType(eWeldingType);
  hkvBvCompressedMeshShape* pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, pColMesh->GetFileTime());
  VASSERT_MSG(pCompressedMeshShape->getNumChildShapes() > 0, "hkvBvCompressedMeshShape could not be created for dynamic model!");
  
  if (bCacheShape)
    *szShapeCacheId = AddShape(szShapeId, pCompressedMeshShape);

  // Only cache shape to HKT file when inside vForge or when enforced.
  if ((Vision::Editor.IsInEditor() && bAllowStaticMeshCaching && bCacheShape) || bForceHktShapeCaching)   
    vHavokCachedShape::SaveMeshShape(pMesh, vScale, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, eWeldingType, pCompressedMeshShape);

  return pCompressedMeshShape;
}


// -------------------------------------------------------------------------- //
// Havok Shape - Static Mesh                                                  //
// -------------------------------------------------------------------------- //

void vHavokShapeFactory::ExtractScaling(const hkvMat4 &mat, hkvVec3& destScaling)
{
  hkvVec3 vx(hkvNoInitialization),vy(hkvNoInitialization),vz(hkvNoInitialization);
  mat.getAxisXYZ (&vx, &vy, &vz);
  destScaling.set(vx.getLength(),vy.getLength(),vz.getLength());
}

class vHavokCompressedInfoCinfo : public hkpDefaultBvCompressedMeshShapeCinfo
{
public:
  
    vHavokCompressedInfoCinfo( const hkGeometry* geometry, hkUint8* collisionInfoPerTri ) : 
      hkpDefaultBvCompressedMeshShapeCinfo( geometry ), m_collisionInfoPerTri(collisionInfoPerTri)
    {
    }

    virtual ~vHavokCompressedInfoCinfo() {}

		virtual hkUint32 getTriangleCollisionFilterInfo(int triangleIndex) const 
    {
      if (m_collisionInfoPerTri)
      {
        hkUint8 cdinfo = m_collisionInfoPerTri[ triangleIndex ];
        return cdinfo;
      }
      else return 0;
    }

		virtual hkUint32 getTriangleUserData(int triangleIndex) const
    {
      int matId = m_geometry->m_triangles[triangleIndex].m_material;
      if (matId >= 0) 
      {
        VASSERT(matId < 256);
        return (hkUint32)matId;
      }
      return 0;
    }

    hkUint8* m_collisionInfoPerTri;
};

hkRefNew<hkpShape> vHavokShapeFactory::CreateShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, int iCreationFlags, const char **szShapeCacheId)
{
  int iCount = meshInstances.GetLength();
  VVERIFY_OR_RET_VAL(iCount>0 && szShapeCacheId!=NULL, NULL);
  
  // Actual mesh scale, which is only used for caching.
  hkvVec3 vScale(hkvNoInitialization);
  ExtractScaling(meshInstances[0]->GetTransform(), vScale);

  char szShapeId[512];
  const bool bAllowStaticMeshCaching = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableShapeCaching==TRUE;
  const vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);
  const bool bForceHktShapeCaching = pModule->IsHktShapeCachingEnforced();

  // For single mesh instances the per instance welding type is used. For merged mesh instances the global merged welding type is used. 
  VisWeldingType_e eWeldingType = (iCount==1) ? meshInstances[0]->GetWeldingType() : (VisWeldingType_e)vHavokPhysicsModule_GetWorldSetupSettings().m_iMergedStaticWeldingType;

  const bool bAllowPerTriCollisionInfo = iCreationFlags & VShapeCreationFlags_ALLOW_PERTRICOLINFO;
  const bool bShrinkByCvxRadius = iCreationFlags & VShapeCreationFlags_SHRINK;

  // Check whether shape has been already cached for this mesh with the respective scaling.
  // We are just caching single static mesh instances and no static mesh collections.
  hkpShape *pCachedShape = HK_NULL; 
  if (iCount == 1)
  {
    // first, find the convex version
    GetIDStringForConvexShape(szShapeId, meshInstances[0]->GetMesh()->GetFilename(), vScale, bShrinkByCvxRadius);
    pCachedShape = FindShape(szShapeId, szShapeCacheId);
    if (!pCachedShape)
    {
      // then find the mesh version
      GetIDStringForMeshShape(szShapeId, meshInstances[0]->GetMesh()->GetFilename(), vScale, meshInstances[0]->GetCollisionBehavior(), eWeldingType);
      pCachedShape = FindShape(szShapeId, szShapeCacheId);
    }
    if (pCachedShape)
    {
      pCachedShape->addReference();
      return pCachedShape; 
    }

    if (bAllowStaticMeshCaching)
    {
      // first, find the convex version
      pCachedShape = vHavokCachedShape::LoadConvexShape(meshInstances[0]->GetMesh(), vScale, bShrinkByCvxRadius);
      if (pCachedShape)
      {
        *szShapeCacheId = AddShape(szShapeId, pCachedShape);
#ifdef HK_DEBUG_SLOW
        const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pCachedShape);
        HK_ASSERT2(0x5432c902, loadedClassType && (hkString::strCmp( loadedClassType->getName(), "hkvConvexVerticesShape" ) == 0), "Serialized in a unexpected cached Havok shape type!");
#endif
        return pCachedShape; 
      }
      else
      {
        // then find the mesh version
        pCachedShape = vHavokCachedShape::LoadMeshShape(meshInstances[0]->GetMesh(), vScale, meshInstances[0]->GetCollisionBehavior(), eWeldingType);
      }
      if (pCachedShape )
      {
        *szShapeCacheId = AddShape(szShapeId, pCachedShape);
#ifdef HK_DEBUG_SLOW
        const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pCachedShape);
        HK_ASSERT2(0x5432c902, loadedClassType && (hkString::strCmp( loadedClassType->getName(), "hkvBvCompressedMeshShape" ) == 0), "Serialized in a unexpected cached Havok shape type!");
#endif
        hkvBvCompressedMeshShape *pCompressedMeshShape = reinterpret_cast<hkvBvCompressedMeshShape*>(pCachedShape);
        pCompressedMeshShape->SetupMaterials();
        return pCachedShape; 
      }
      else if(!Vision::Editor.IsInEditor() && !bForceHktShapeCaching)
      {
        Vision::Error.Warning("Cached HKT file for %s is missing. Please generate HKT file (see documentation for details).", meshInstances[0]->GetMesh()->GetFilename());
      }
    }
  }

  // Get the reference transformation. We use the first static mesh as reference
  // transformation, and thus as the position of the corresponding rigid body.
  hkvMat4 referenceTransform = meshInstances[0]->GetTransform();

  // Remove any scaling from the reference matrix. This one has to be applied to
  // the Havok shapes, and thus needs to be part of the mesh transforms.
  referenceTransform.setScalingFactors(hkvVec3 (1));

  // We need the inverse referenceTransform to transform each instance into reference space
  referenceTransform.invert();
  referenceTransform.setRow (3, hkvVec4 (0, 0, 0, 1));

  // Determine collision type from first static mesh instance.
  const VisStaticMeshInstance_cl *pMeshInstance = meshInstances[0];
  VisStaticMesh_cl *pMesh = pMeshInstance->GetMesh();
  IVCollisionMesh *pColMesh = pMesh->GetCollisionMesh(true, true);
  const bool bIsConvex = pColMesh->GetType()==VIS_COLMESH_GEOTYPE_CONVEXHULL;

  // Only create a convex shape if a single mesh instance is used, since otherwise merging multiple mesh instances in one single convex hull
  // will provide in most cases not the desired behavior. Moreover we can only create either a convex hull or a concave mesh shape, therefore
  // mesh instances with different collision type can't be merged into one shape.
  hkpShape *pShape = (bIsConvex && iCount==1) ?
    CreateConvexShapeFromStaticMeshInstances(meshInstances, referenceTransform, bShrinkByCvxRadius) : 
    CreateMeshShapeFromStaticMeshInstances(meshInstances, referenceTransform, bAllowPerTriCollisionInfo, eWeldingType);

  // We are just caching single static mesh instances and no static mesh collections.
  if (iCount == 1)
  {
     *szShapeCacheId = AddShape(szShapeId, pShape);

    // Only cache shape to HKT file when inside vForge or when enforced.
    if ((Vision::Editor.IsInEditor() && bAllowStaticMeshCaching) || bForceHktShapeCaching)
    {
      if (bIsConvex)
        vHavokCachedShape::SaveConvexShape(meshInstances[0]->GetMesh(), vScale, bShrinkByCvxRadius, (hkvConvexVerticesShape*)pShape);
      else
        vHavokCachedShape::SaveMeshShape(meshInstances[0]->GetMesh(), vScale, meshInstances[0]->GetCollisionBehavior(), eWeldingType, (hkvBvCompressedMeshShape*)pShape);
    }
  }
  return pShape;
}

hkpShape* vHavokShapeFactory::CreateConvexShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, hkvMat4 &refTransform, bool shrinkByCvxRadius)
{
  VVERIFY_OR_RET_VAL(meshInstances.GetLength()==1, NULL);

  // Get the collision mesh for the static mesh instance
  VisStaticMeshInstance_cl *pMeshInstance = meshInstances[0];
  VisStaticMesh_cl *pMesh = pMeshInstance->GetMesh();
  IVCollisionMesh *pColMesh = pMesh->GetCollisionMesh(true, true);
  VVERIFY_OR_RET_VAL(pColMesh!=NULL, NULL);

  // We transform each static mesh into the reference space.
  hkvMat4 mTransform = refTransform;
  mTransform = mTransform.multiply (pMeshInstance->GetTransform());

  hkGeometry geom;
  int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
  for (int i=0;i<iNumColMeshes;i++)
    BuildGeomFromCollisionMesh(pColMesh, i, mTransform, false, geom);
 
  // Set the build configuration to set planes equations and connectivity automatically
  hkpConvexVerticesShape::BuildConfig config;
  config.m_createConnectivity = true;
  config.m_shrinkByConvexRadius = shrinkByCvxRadius;

  hkStridedVertices stridedVerts;
  stridedVerts.m_numVertices = geom.m_vertices.getSize();
  stridedVerts.m_striding    = sizeof(hkVector4);
  stridedVerts.m_vertices    = (const hkReal*)geom.m_vertices.begin();

  // Create convex shape
  hkvConvexVerticesShape *pConvexShape = new hkvConvexVerticesShape(pColMesh->GetFileTime(), stridedVerts, config);
  
  return pConvexShape;
}

hkpShape* vHavokShapeFactory::CreateMeshShapeFromStaticMeshInstances(const VisStaticMeshInstCollection &meshInstances, hkvMat4 &refTransform, 
                                                                     bool bAllowPerTriCollisionInfo, VisWeldingType_e eWeldingType)
{
  int iCount = meshInstances.GetLength();
  VVERIFY_OR_RET_VAL(iCount>0, NULL);

  hkGeometry geom;
  hkInt64 iFileTime = 0;

  // Iterate all the passed mesh instances
  hkvMeshMaterialCache materials;
  hkArray<hkUint8> collisionMask;
  int subPartIndex = 0;
  for (int i = 0; i < iCount; i++)
  { 
    // Get the collision mesh for the static mesh instance
    const VisStaticMeshInstance_cl *pMeshInstance = meshInstances[i];
    VisStaticMesh_cl *pMesh = pMeshInstance->GetMesh();
    IVCollisionMesh *pColMesh = pMesh->GetCollisionMesh(true, true);
    if (pColMesh == NULL)
    {
      VASSERT(false);
      continue;
    }

    // Simply retrieve file time from last static mesh instance
    iFileTime = pColMesh->GetFileTime();

    // Create the Havok triangle sub part
    {
      int startingNumTris = geom.m_triangles.getSize();

      // We transform each static mesh into the reference space.
      hkvMat4 mTransform = refTransform;
      mTransform = mTransform.multiply (pMeshInstance->GetTransform());

      int iNumColMeshes = hkvMath::Max(pColMesh->GetSubmeshCount(), 1);
      for (int i=0;i<iNumColMeshes;i++)
      {
        int startingNumVerts = geom.m_vertices.getSize();
        BuildGeomFromCollisionMesh(pColMesh, i, mTransform, false, geom);
        int endNumVerts = geom.m_vertices.getSize();
        int endNumTris = geom.m_triangles.getSize();
        VASSERT( (endNumVerts - startingNumVerts) > 0 );

        hkUint32 cdMask = 0;
        if (bAllowPerTriCollisionInfo && (pColMesh->GetSubmeshCount() > 0))
        {
          const VPhysicsSubmesh& submesh = pColMesh->GetSubmeshes()[i];
          cdMask = submesh.iGroupFilter & 0xff; // only lower 8 bits stored in compressed mesh.

          int numCD = endNumTris - collisionMask.getSize();
          hkUint8* cdi = collisionMask.expandBy( numCD );
          hkString::memSet(cdi, cdMask, numCD);
        }
      }
   
      // ColMesh can have Materials. Also need material to store the orig static mesh index (so if count > 1)
      if ( pColMesh->GetTriSrfIndices() )
      {
        VColMeshMaterial *pColMaterials = pColMesh->GetMaterials();
        VASSERT(pColMaterials);
        int iNumMats = pColMesh->GetMaterialCount();
        int matIndexOffset = materials.getSize();

        for(int ii=0;ii<iNumMats;ii++)
        {
          hkvMeshMaterial& mat = materials.expandOne();
          // in Havok there is no StaticFriction, DynamicFriction, anisotropic StaticFriction/ DynamicFriction
          // instead there is only Friction available
          // so DynamicFriction is used for Havok Friction -> is there a better workaround?
          mat.m_fFriction = pColMaterials[ii].fDynamicFriction; 

          mat.m_fRestitution = pColMaterials[ii].fRestitution;
          if (! pColMaterials[ii].szUserData.IsEmpty()) 
            mat.m_userData = pColMaterials[ii].szUserData.GetChar();   

          mat.m_iOrignalSubMeshIndex = subPartIndex;
        }

        // reindex geom mat id
        if (matIndexOffset> 0)
        {
          for (int ti=startingNumTris; ti < geom.m_triangles.getSize(); ++ti)
          {
            VASSERT(geom.m_triangles[ti].m_material < iNumMats);
            VASSERT(geom.m_triangles[ti].m_material >= 0);
            geom.m_triangles[ti].m_material += matIndexOffset;
          }
        }
      }

      // per instance part
      ++subPartIndex;     
    }
  }

  const bool bHaveTriCDData = collisionMask.getSize() > 0;
  vHavokCompressedInfoCinfo ci( &geom, bHaveTriCDData ? collisionMask.begin() : HK_NULL );
  ci.m_weldingType = vHavokConversionUtils::VisToHkWeldingType(eWeldingType);
  ci.m_collisionFilterInfoMode = bHaveTriCDData ? hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT : hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE;
  
  hkvBvCompressedMeshShape *pCompressedMeshShape = HK_NULL;
  if ( materials.getSize() > 0)
  {
    ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_8_BIT; 
    pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, materials, iFileTime);
  }
  else
  {
    ci.m_userDataMode = hkpBvCompressedMeshShape::PER_PRIMITIVE_DATA_NONE; 
    pCompressedMeshShape = new hkvBvCompressedMeshShape(ci, iFileTime);
  }
  VASSERT_MSG(pCompressedMeshShape->getNumChildShapes() > 0, "hkvBvCompressedMeshShape could not be created for static model!");

  pCompressedMeshShape->SetupMaterials();

  return pCompressedMeshShape;
}


// -------------------------------------------------------------------------- //
// Havok Shape - Terrain                                                      //
// -------------------------------------------------------------------------- //

#ifdef SUPPORTS_TERRAIN

hkpShape* vHavokShapeFactory::CreateShapeFromTerrain(const VTerrainSector& terrain)
{
  // Check if shape is already cached on disk. Only try to load cached shape file outside of vForge, since otherwise the physics
  // representation will not be updated on terrain editing.
  const bool bAllowStaticMeshCaching = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings().m_bEnableShapeCaching==TRUE;
  if (!Vision::Editor.IsInEditor() && bAllowStaticMeshCaching)
  {
    hkpShape *pCachedShape = vHavokCachedShape::LoadTerrainSectorShape(&terrain);
    if (pCachedShape)
      return pCachedShape;
    else
      Vision::Error.Warning("Cached HKT file for %s is missing. Please generate HKT file (see documentation for details).", terrain.GetFilename());
  }

  hkpSampledHeightFieldBaseCinfo ci; 
  ci.m_xRes = terrain.m_Config.m_iHeightSamplesPerSector[0]+1; 
  ci.m_zRes = terrain.m_Config.m_iHeightSamplesPerSector[1]+1;

  // Overlapping samples into next sectors on all edges (so res-1 is scale)
  ci.m_scale.set( VIS2HK_FLOAT_SCALED(terrain.m_Config.m_vSectorSize.x/float(ci.m_xRes-1)), 1, VIS2HK_FLOAT_SCALED(terrain.m_Config.m_vSectorSize.y/float(ci.m_zRes-1)) );
  ci.m_minHeight = VIS2HK_FLOAT_SCALED(terrain.m_fMinHeightValue);
  ci.m_maxHeight = VIS2HK_FLOAT_SCALED(terrain.m_fMaxHeightValue);

  hkvSampledHeightFieldShape* heightFieldShape = new hkvSampledHeightFieldShape(ci, &terrain);  
  const bool bExact = terrain.GetPhysicsType() >= VTerrainSector::VPHYSICSTYPE_PRECISE;
  const bool bHasHoles = terrain.HasHoles();

  if (!bExact && bHasHoles)
    Vision::Error.Warning("Terrain sector has holes but uses approximated physics representation. Therefore precise physics representation enforced.");

  // Two choices here.. could use just the heightfield as is, or
  // wrap it in a tri sampler. The tri sampler will give exact collisions, the heightfield alone will be based on collision spheres.
  // We *have* to use the accurate version in case the sector has holes
  if (bExact || bHasHoles)
  {
    hkpTriSampledHeightFieldCollection* collection;
    hkpTriSampledHeightFieldBvTreeShape* bvTree;

    if (bHasHoles) // use a slightly modified version of hkpTriSampledHeightFieldCollection in case it has holes
    {
      collection = new hkvTriSampledHeightFieldCollection(&terrain, heightFieldShape);
      bvTree = new hkvTriSampledHeightFieldBvTreeShape(collection);    
    } 
    else
    {
      collection = new hkpTriSampledHeightFieldCollection(heightFieldShape);
      bvTree = new hkpTriSampledHeightFieldBvTreeShape(collection);
    }

    // Winding must be set to WELDING_TYPE_ANTICLOCKWISE (see hkpTriSampledHeightFieldCollection::initWeldingInfo()).
    if (terrain.GetPhysicsType() == VTerrainSector::VPHYSICSTYPE_PRECISE_OFFLINE_WELDING)
      hkpMeshWeldingUtility::computeWeldingInfo(collection, bvTree, hkpWeldingUtility::WELDING_TYPE_ANTICLOCKWISE);

    collection->removeReference();
    heightFieldShape->removeReference();
    return bvTree;	
  }
  else
  {
    return heightFieldShape;
  }
}

#endif


// -------------------------------------------------------------------------- //
// Havok Shape Caching                                                        //
// -------------------------------------------------------------------------- //

const char* vHavokShapeFactory::AddShape(const char *szShapeId, hkpShape *pShape)
{
  VASSERT(m_pShapeCacheTable!=NULL && szShapeId!=NULL);

  VASSERT_MSG(!m_pShapeCacheTable->hasKey(szShapeId), "Shape shouldn't be cached twice");

  const char *szShapeCacheId = m_pShapeCacheTable->insert(szShapeId, pShape); 

  // We will keep the shape in our table, thus need to have a reference on it
  pShape->addReference();

  return szShapeCacheId;
}

void vHavokShapeFactory::RemoveShape(const char *szShapeID)
{
  VASSERT(m_pShapeCacheTable != NULL);
  if (szShapeID == NULL)
    return;

  hkpShape* pShape = FindShape(szShapeID);
  if (pShape == NULL)
    return;

  if (pShape->m_referenceCount == 1)
  {
    m_pShapeCacheTable->remove(szShapeID);
    pShape->removeReference();
  }
}

bool vHavokShapeFactory::ClearCache()
{
  VASSERT(m_pShapeCacheTable != NULL);

  // Iterate all shapes in cache and check whether all shapes have a reference count 
  // of 1 before we call removeReferecne(). If so return true, false otherwise.
  bool bRefCountSafe = true;
  
  // Remove reference from all shapes
  vHavokShapeCache::Iterator iter = m_pShapeCacheTable->getIterator();
  while (m_pShapeCacheTable->isValid(iter))
  {
    hkpShape *pShape = m_pShapeCacheTable->getValue(iter);
    if (pShape->m_referenceCount != 1)
      bRefCountSafe = false;
    pShape->removeReference();
    iter = m_pShapeCacheTable->getNext(iter);
  }

  // Purge all
  m_pShapeCacheTable->clear();
 
  return bRefCountSafe;
}

hkpShape* vHavokShapeFactory::FindShape(const char *szShapeId, const char **szShapeCacheId)
{
  VASSERT(m_pShapeCacheTable!=NULL && szShapeId!=NULL);

  vHavokShapeCache::Iterator iter = m_pShapeCacheTable->findKey(szShapeId);
  if (m_pShapeCacheTable->isValid(iter))
  {
    if (szShapeCacheId)
      *szShapeCacheId = m_pShapeCacheTable->getKey(iter);
    return m_pShapeCacheTable->getValue(iter);
  }

  return HK_NULL;
}


// -------------------------------------------------------------------------- //
// Havok Shape - Helper                                                       //
// -------------------------------------------------------------------------- //

hkvVec3 vHavokShapeFactory::GetPivotOffset(const VDynamicMesh *pMesh, const hkvVec3& vScaleEntity)
{
  // Get center of bounding box
  const hkvAlignedBBox& bbox = pMesh->GetCollisionBoundingBox();
  hkvVec3 vCenter = bbox.getCenter();

  // Take scaling into account
  vCenter.x *= vScaleEntity.x;
  vCenter.y *= vScaleEntity.y;
  vCenter.z *= vScaleEntity.z;

  return vCenter;
}

hkvVec3 vHavokShapeFactory::GetPivotOffset(const VDynamicMesh *pMesh, float fUniformScaleEntity)
{
  // Get center of bounding box
  const hkvAlignedBBox& bbox = pMesh->GetCollisionBoundingBox();
  hkvVec3 vCenter = bbox.getCenter();

  // Take scaling into account
  vCenter.x *= fUniformScaleEntity;
  vCenter.y *= fUniformScaleEntity;
  vCenter.z *= fUniformScaleEntity;

  return vCenter;
}

void vHavokShapeFactory::GetIDStringForConvexShape(char *szIDString, const char *szMeshName, const hkvVec3& vScale, bool bShrinkByCvxRadius)
{
	if (bShrinkByCvxRadius)
		sprintf(szIDString, "%s|Cvx_%.2f_%.2f_%.2f_t", szMeshName, vScale.x, vScale.y, vScale.z);
	else
		sprintf(szIDString, "%s|Cvx_%.2f_%.2f_%.2f", szMeshName, vScale.x, vScale.y, vScale.z);
}

void vHavokShapeFactory::GetIDStringForMeshShape(char *szIDString, const char *szMeshName, const hkvVec3& vScale, VisStaticMeshInstance_cl::VisCollisionBehavior_e eCollisionBehavior,
                                                 VisWeldingType_e eWeldingType) 
{
  sprintf(szIDString, "%s|Msh_%.2f_%.2f_%.2f_%i_%i", szMeshName, vScale.x, vScale.y, vScale.z, (int)eCollisionBehavior, (int)eWeldingType);
}

void vHavokShapeFactory::BuildGeomFromCollisionMesh(const IVCollisionMesh *pColMesh, int iSubmeshIndex,const hkvMat4 &transform, bool bConvex, hkGeometry& geom)
{
  const VSimpleCollisionMeshBase *pMeshBase = pColMesh->GetMeshData();

  int iNumVerts = pMeshBase->GetVertexCount(); 
  int iFirstVertexIndex = 0;
  int iNumTris = pMeshBase->GetIndexCount() / 3; 
  int iStartIndex = 0;

  const bool bSubmeshsSupported = pColMesh->GetSubmeshCount() > 0;
  if (bSubmeshsSupported)
  {
    VASSERT(iSubmeshIndex < pColMesh->GetSubmeshCount());
    VPhysicsSubmesh submesh = pColMesh->GetSubmeshes()[iSubmeshIndex];
    iNumVerts = (submesh.iLastVertexIndex-submesh.iFirstVertexIndex)+1;
    iFirstVertexIndex = submesh.iFirstVertexIndex;
    iNumTris = submesh.iNumIndices / 3;
    iStartIndex = submesh.iStartIndex;
    VASSERT(pColMesh->GetTriSrfIndices());
  }

  hkTransform hkT; // in Vision scaling
  {
    hkMatrix4 m; vHavokConversionUtils::VisMatrixToHkMatrix(transform, m, false, false, true);
    m.get(hkT);
  }

  VASSERT(iNumVerts > 0);
  const unsigned int uiVtxComponentCount = iNumVerts * 3;

  int geomVertexStartIndex = geom.m_vertices.getSize();
  int vertexIndexOffset = geomVertexStartIndex - iFirstVertexIndex;

  hkVector4 *pVertices = (hkVector4*)geom.m_vertices.expandBy( iNumVerts );
  hkvVec3* pVertex = pMeshBase->GetVertexPtr();

  // start retrieving vertices from the start of the vertex-list (iFirstVertexIndex=0) when submeshes are not supported,
  // otherwise from the iFirstVertexIndex supplied by the submesh
  pVertex = &pVertex[iFirstVertexIndex]; 
  for (int i=0; i < iNumVerts; ++i, pVertex++) 
  {
    hkVector4 d; vHavokConversionUtils::VisVecToPhysVec_noscale(*pVertex,d);
    pVertices[i]._setTransformedPos(hkT, d);
    pVertices[i].mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
  }

  // For convex hulls we do not need to retrieve triangle information.
  if (bConvex)
    return;

  VASSERT(iNumTris > 0);

  int indexStriding = 0;   
  void *pIndices = NULL;

  int geomTriStartIndex = geom.m_triangles.getSize();
  geom.m_triangles.expandBy(iNumTris);

  short* triMatIndices = pColMesh->GetTriSrfIndices()? pColMesh->GetTriSrfIndices() + (iStartIndex/3) : HK_NULL;
  if (pMeshBase->m_pIndex16)
  {
    unsigned short* visionInd = &pMeshBase->m_pIndex16[iStartIndex];
    for (int ti=0; ti < iNumTris; ++ti)
    {
      hkGeometry::Triangle& tri = geom.m_triangles[geomTriStartIndex + ti];
      // Note we flip the winding for the static meshes so that 'front facing' is CW 
      tri.m_a = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_c = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_b = (int)*visionInd + vertexIndexOffset; ++visionInd;
      if (triMatIndices)
      {
        tri.m_material = triMatIndices[ti];
      }
      else
      {
        tri.m_material = 0;
      }
    }
  }
  else if (pMeshBase->m_pIndex32)
  {
    unsigned int* visionInd = &pMeshBase->m_pIndex32[iStartIndex];
    for (int ti=0; ti < iNumTris; ++ti)
    {
      hkGeometry::Triangle& tri = geom.m_triangles[geomTriStartIndex + ti];
      // Note we flip the winding for the static meshes so that 'front facing' is CW 
      tri.m_a = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_c = (int)*visionInd + vertexIndexOffset; ++visionInd;
      tri.m_b = (int)*visionInd + vertexIndexOffset; ++visionInd;
      if (triMatIndices)
      {
        tri.m_material = triMatIndices[ti];
      }
      else
      {
        tri.m_material = 0;
      }
    }
  }
  else
  {
    VASSERT(FALSE);
  }
}

#ifdef SUPPORTS_SNAPSHOT_CREATION

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VisStaticMeshInstance_cl *pMeshInstance)
{
  VASSERT(pMeshInstance != NULL);

  // Get scale
  hkvVec3 vScale(hkvNoInitialization);
  ExtractScaling(pMeshInstance->GetTransform(), vScale);

  // There is no real way to figure out if a convex or mesh shape is required and the filename is based on that.
  // So try them both.

  // Convex version
  {
    VStaticString<FS_MAX_PATH> szCachedShapeName(pMeshInstance->GetMesh()->GetFilename());
    bool shrinkToFit = false; //how do if true / can it be true for static mesh?
    vHavokCachedShape::GetConvexShapePath(szCachedShapeName, vScale, shrinkToFit);
    IVFileInStream *pIn = Vision::File.Open(szCachedShapeName);
    if (pIn)
    {
      snapshot.AddFileDependency(pMeshInstance->GetMesh(), szCachedShapeName, pIn->GetSize() );
      pIn->Close();
      return;
    }
  }

  // Mesh version
  {
    VStaticString<FS_MAX_PATH> szCachedShapeName(pMeshInstance->GetMesh()->GetFilename());
    vHavokCachedShape::GetMeshShapePath(szCachedShapeName, vScale, pMeshInstance->GetCollisionBehavior(), pMeshInstance->GetWeldingType());
    IVFileInStream *pIn = Vision::File.Open(szCachedShapeName);
    if (pIn)
    {
      snapshot.AddFileDependency(pMeshInstance->GetMesh(), szCachedShapeName, pIn->GetSize() );
      pIn->Close();
      return;
    }
  }
}

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VisBaseEntity_cl *pEntity)
{
  VASSERT(pEntity != NULL);

  // Get wrapped rigid body
  vHavokRigidBody *pWrappedRigidBody = pEntity->Components().GetComponentOfType<vHavokRigidBody>();
  if (pWrappedRigidBody == NULL)
    return;

  // Get shape
  vHavokPhysicsModule *pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);
  pModule->MarkForRead();
  const hkpRigidBody *pRigidBody = pWrappedRigidBody->GetHkRigidBody();
  VASSERT(pRigidBody != NULL);
  const hkpShape *pShape = pRigidBody->getCollidable()->getShape();
  pModule->UnmarkForRead();
  
  // Only convex/ mesh rigid bodies have a cached HKT file
  const hkClass* loadedClassType = hkVtableClassRegistry::getInstance().getClassFromVirtualInstance(pShape);
  if (loadedClassType!=&hkvConvexVerticesShapeClass && loadedClassType!=&hkvBvCompressedMeshShapeClass)
    return;

  // Get mesh
  VDynamicMesh *pMesh = pEntity->GetMesh();
  if (pMesh == NULL)
    return;

  // Get scale
  hkvVec3 vScale = pEntity->GetScaling();
  bool shrinkToFit = pWrappedRigidBody->Havok_TightFit;

  // Get HKT file dependency for convex/ mesh rigid body
  if (loadedClassType == &hkvConvexVerticesShapeClass) 
  {
    VStaticString<FS_MAX_PATH> szCachedShapeName(pMesh->GetFilename());
    vHavokCachedShape::GetConvexShapePath(szCachedShapeName, vScale, shrinkToFit);
    IVFileInStream *pIn = Vision::File.Open(szCachedShapeName);
    if (pIn)
    {
      snapshot.AddFileDependency(pMesh, szCachedShapeName, pIn->GetSize() );
      pIn->Close();
    }
  }
  else if(loadedClassType == &hkvBvCompressedMeshShapeClass) 
  {
    VStaticString<FS_MAX_PATH> szCachedShapeName(pMesh->GetFilename());
    vHavokCachedShape::GetMeshShapePath(szCachedShapeName, vScale, VisStaticMeshInstance_cl::VIS_COLLISION_BEHAVIOR_CUSTOM, (VisWeldingType_e)pWrappedRigidBody->Havok_WeldingType);
    IVFileInStream *pIn = Vision::File.Open(szCachedShapeName);
    if (pIn)
    {
      snapshot.AddFileDependency(pMesh, szCachedShapeName, pIn->GetSize() );
      pIn->Close();
    }
  }
}

void vHavokShapeFactory::GetHktDependencies(VResourceSnapshot &snapshot, VTerrainSector *pSector)
{
  VASSERT(pSector != NULL);

  // Return when no physics representation
  vHavokTerrain *pHavokTerrain = (vHavokTerrain*) pSector->GetPhysicsUserData();
  if (pHavokTerrain == NULL)
    return;

  const VTerrainSector::VPhysicsType_e ePhysicsType = pSector->GetPhysicsType();
  const bool bHasHoles = pSector->HasHoles();

  // First get filename by specifying extension (hmap) to be able to retrieve the absolute path. 
  // Afterwards remove extension.
  char szFilename[FS_MAX_PATH];
  pSector->m_Config.GetSectorFilename(szFilename, pSector->m_iIndexX, pSector->m_iIndexY, "hmap", true);
  char szPath[FS_MAX_PATH];
  bool bSuccess = VFileHelper::GetAbsolutePath(szFilename, szPath, Vision::File.GetManager());
  VASSERT_MSG(bSuccess, "vHavokShapeFactory::GetHktDependencies: Failed to make the path to the sector hmap absolute, the file may not exist!");
  VFileHelper::GetFilenameNoExt(szFilename, szPath);

  // Build the cached shape filename
  VStaticString<FS_MAX_PATH> szCachedShapeName(szFilename);
  vHavokCachedShape::GetTerrainSectorShapePath(szCachedShapeName, ePhysicsType, bHasHoles);
  IVFileInStream *pIn = Vision::File.Open(szCachedShapeName);
  if (pIn)
  {
    snapshot.AddFileDependency(pSector, szCachedShapeName, pIn->GetSize() );
    pIn->Close();
  }

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
