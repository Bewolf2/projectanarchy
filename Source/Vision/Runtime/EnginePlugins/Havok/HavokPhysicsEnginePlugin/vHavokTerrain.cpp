/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCachedShape.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>
#include <Physics2012/Internal/Collide/StaticCompound/hkpStaticCompoundShape.h>
#include <Physics2012/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h> 
#include <Common/Base/Container/PointerMultiMap/hkPointerMultiMap.h>

// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

vHavokTerrain::vHavokTerrain(vHavokPhysicsModule &module) : m_bInitialized(FALSE), m_pRigidBody(NULL), m_terrainSector(NULL), m_module(module)
{
}

vHavokTerrain::~vHavokTerrain()
{
  CommonDeinit();
}

void vHavokTerrain::DisposeObject()
{
  CommonDeinit();
}

void vHavokTerrain::CommonDeinit()
{
  RemoveHkRigidBody();
}


// -------------------------------------------------------------------------- //
// Initialisation                                                             //
// -------------------------------------------------------------------------- //

void vHavokTerrain::Init(const VTerrainSector &terrainSector)
{
  // Init may only be called once
  VVERIFY_OR_RET(m_bInitialized == false);
  m_bInitialized = true;

  m_terrainSector = &terrainSector;

  // Build the Havok shape
  CreateHkRigidBody();
}


// -------------------------------------------------------------------------- //
// Havok Rigid Body Creation                                                  //
// -------------------------------------------------------------------------- //

void vHavokTerrain::CreateHkRigidBody()
{
  // Create the Havok shape
  hkpShape *pShape = vHavokShapeFactory::CreateShapeFromTerrain(*m_terrainSector);

  // Create the Havok rigid body
  hkpRigidBodyCinfo rci;
  rci.m_motionType = hkpMotion::MOTION_FIXED;
  rci.m_shape = pShape;

  //subtract the y sample spacing from the original position to make sure that the physical representation and the visible terrain match
  hkvVec3 vSpacing(0, m_terrainSector->m_Config.m_vSampleSpacing.y, 0);
  vHavokConversionUtils::VisVecToPhysVecWorld(hkvVec3(m_terrainSector->GetSectorOrigin() - vSpacing),rci.m_position);
  rci.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN);

  // We have two choices here. Heightfields in Havok are on the X,Z local plane, but in Vision they are 
  // X,Y, so some transform is required. We could rotate by -90 and invert the heights but that results 
  // in normals that are the opposite to what you would expect (which is fine though as double sided, but not ideal).
  // The second option is to rotate the opposite way and invert the lookup on Y (Havok local Z)
  // This second option of course shifts the direction of the extends, so we need to also include the translation for that:
  // y, but take into account the single sample edge overlap in the terrain
  hkSimdReal yvOffset; 
  yvOffset.setFromFloat( m_terrainSector->m_Config.m_vSectorSize.y + m_terrainSector->m_Config.m_vSampleSpacing.y );
  yvOffset.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
  // Counter act the shift the extent dir
  rci.m_position.addMul( hkVector4::getConstant<HK_QUADREAL_0100>(), yvOffset );  
  rci.m_rotation.setAxisAngle(hkVector4::getConstant<HK_QUADREAL_1000>(),hkSimdReal_PiOver2); // Basis change
  
  rci.m_friction = hkReal(0.2f);

  m_pRigidBody = new hkpRigidBody( rci );

  // Set user data to identify this terrain during collision detection (raycast etc)
  m_pRigidBody->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer(this, V_USERDATA_TERRAIN));

  // Add our instance to the module
  m_module.AddTerrain(this);

  // Remove reference
  pShape->removeReference();   
}

void vHavokTerrain::RemoveHkRigidBody()
{
  if (!m_pRigidBody)
    return;

  // Keep our object alive
  VSmartPtr<vHavokTerrain> keepAlive = this;

  // If we still have an owner, then remove the rigid body to the Havok world
  m_module.RemoveTerrain(this);

  // Free the rigid body 
  m_pRigidBody->removeReference();
  m_pRigidBody = NULL;
}


// -------------------------------------------------------------------------- //
// Accessing Havok Instances                                                  //
// -------------------------------------------------------------------------- //

const hkpShape *vHavokTerrain::GetHkShape() const
{
  if (m_pRigidBody == NULL) 
    return NULL;

  return m_pRigidBody->getCollidable()->getShape();;
}


// -------------------------------------------------------------------------- //
// Accessing Vision Instances                                                  //
// -------------------------------------------------------------------------- //

const VTerrainSector* vHavokTerrain::GetWrappedTerrainSector()
{
  return m_terrainSector;
}


// -------------------------------------------------------------------------- //
// Helper methods                                                             //
// -------------------------------------------------------------------------- //

void vHavokTerrain::SaveShapeToFile()
{
  VASSERT(m_pRigidBody != NULL);
  m_module.MarkForRead(); 
  const hkpShape *pShape = m_pRigidBody->getCollidable()->getShape();
  m_module.UnmarkForRead();
  vHavokCachedShape::SaveTerrainSectorShape(m_terrainSector, pShape);
}


// -------------------------------------------------------------------------- //
// Decoration group
// -------------------------------------------------------------------------- //

struct VHavokDecorationGroup_ModelAndScale 
{
  IVTerrainDecorationModel *model;
  float scale;
  int shapeStartIndex;
};

struct VHavokDecorationGroup_ShapeInfo
{
  hkpShape* pShape;
  hkVector4 vLocalOffset;
};

VHavokDecorationGroup::VHavokDecorationGroup(IVisDecorationGroup_cl &group)
{
  int iDecoCount = group.GetDecorationObjectCount();

#if defined(HK_DEBUG_SLOW)
  int iCollisionObjectsCount = 0;
  for ( int i=0; i<iDecoCount; i++ )
  {
    VTerrainDecorationInstance *pInst = group.GetDecorationObject(i);
    if ( pInst->GetModel() != NULL )
      iCollisionObjectsCount += pInst->GetModel()->GetCollisionSubObjectCount();
  }
  VASSERT_MSG(iCollisionObjectsCount>0, "These objects should only be created if the group has collision information.");
#endif

  hkQuaternion qSpeedTree2Engine;
  qSpeedTree2Engine.setAxisAngle(hkVector4::getConstant<HK_QUADREAL_0010>(), hkReal(-90)*HK_REAL_DEG_TO_RAD);

  hkpStaticCompoundShape *compoundShape = new hkpStaticCompoundShape();
  hkArray<VHavokDecorationGroup_ShapeInfo> shapeInfos;

  // Set up a cache of decoration collision capsules
  hkArray<VHavokDecorationGroup_ModelAndScale> capsulesCache;
  hkPointerMultiMap<IVTerrainDecorationModel*, int> modelToCache;
  const float scaleMatchTolerance = 0.05f; // raise this to lower memory by reducing set of shapes

  // Find all unique Model@Scale combinations (hkpStaticCompoundShape only supports scale
  // for hkpConvexVerticesShapes in 2011.2, so have to bake scale here)

  // For each decoration object
  for (int i = 0; i < iDecoCount; i++)
  {
    VTerrainDecorationInstance *pInstance = group.GetDecorationObject(i);
    IVTerrainDecorationModel *pModel = pInstance->GetModel();
    if ( pModel == NULL )
      continue;

    const int iCapsulesCount = pModel->GetCollisionSubObjectCount();
    if ( iCapsulesCount == 0 )
      continue;

    const float instanceScale = pInstance->GetScaling();

    // Is the Model/Scale pair in the cache?
    int iCacheIndex = -1;
    hkPointerMultiMap< IVTerrainDecorationModel*, int >::Iterator iter = modelToCache.findKey(pModel);
    for (; modelToCache.isValid(iter); iter = modelToCache.getNext(iter, pModel))
    {
      const int thisCacheIndex = modelToCache.getValue(iter);
      VASSERT( thisCacheIndex >= 0 && thisCacheIndex < capsulesCache.getSize());
      VHavokDecorationGroup_ModelAndScale& ms = capsulesCache[thisCacheIndex];
      VASSERT( ms.model == pModel );

      if ( hkMath::equal(ms.scale, instanceScale, scaleMatchTolerance) )
      {
        // Model/Scale found in cache!
        iCacheIndex = thisCacheIndex;
        break;
      }
    }

    // Compute base instance transformation
    const hkSimdReal hkInstanceScale = hkSimdReal::fromFloat(instanceScale) * 
      vHavokConversionUtils::GetVision2HavokScaleSIMD();

    hkvMat3 normalizedOrientation = pInstance->m_Orientation;
    normalizedOrientation.normalize();

    // Get the model instance transformation.
    hkQsTransform instanceTransform;
    vHavokConversionUtils::VisMatrixToHkQuat(normalizedOrientation, instanceTransform.m_rotation);
    vHavokConversionUtils::VisVecToPhysVecWorld(pInstance->m_vPosition, instanceTransform.m_translation);
    instanceTransform.setScale(hkVector4::getConstant<HK_QUADREAL_1>());

    // If not found, create a new shape for each collision sub-object
    // and update the capsules cache with the new information.
    int iShapesIndex;
    if ( iCacheIndex < 0 )
    {
      iShapesIndex = shapeInfos.getSize();

      for (int j=0; j<iCapsulesCount; j++)
      {
        const VDecorationCollisionPrimitive &decoCapsuleIn(*pModel->GetCollisionSubObject(j));

        // Compute capsule start and end point
        hkvVec3 vStart = decoCapsuleIn.GetStart();
        hkvVec3 vEnd = decoCapsuleIn.GetEnd();

		hkVector4 vs; vHavokConversionUtils::VisVecToPhysVec_noscale(vStart,vs);        
		hkVector4 ve; vHavokConversionUtils::VisVecToPhysVec_noscale(vEnd,ve);

		vs._setRotatedDir(qSpeedTree2Engine, vs);
		ve._setRotatedDir(qSpeedTree2Engine, ve);

        vs.mul(hkInstanceScale);
        ve.mul(hkInstanceScale); 

        // vs is relative to the origin of the shape's local space
        // -> translate the shape instance in the compound shape accordingly
        // (more efficient than using a hkpConvexTranslateShape for the sphere)
        hkVector4 vd;
        vd.setSub(ve, vs);
     
        hkReal fRadius = hkReal(VIS2HK_FLOAT_SCALED(decoCapsuleIn.GetRadius() * instanceScale));

        // Havok asserts for capsules with start~=end, so make it a sphere
        hkpShape *pShape;
		    if (vd.lengthSquared<3>() < hkSimdReal::fromFloat(1e-6f)) 
        {
          pShape = new hkpSphereShape(fRadius);
        }
        else
        {
          pShape = new hkpCapsuleShape(hkVector4::getConstant<HK_QUADREAL_0>(), vd, fRadius);
        }

        VHavokDecorationGroup_ShapeInfo shapeInfo;
        shapeInfo.pShape = pShape;
        shapeInfo.vLocalOffset = vs;
        shapeInfos.pushBack(shapeInfo);

        // translate to origin of capsule
        hkQsTransform shapeTransform = instanceTransform;
        vs._setRotatedDir(instanceTransform.m_rotation, vs); // we need vs in world space
        shapeTransform.m_translation.add(vs);

        // Add to compound shape
        compoundShape->addInstance(pShape, shapeTransform);
      }

      // Update cache
      modelToCache.insert(pModel, capsulesCache.getSize());
      VHavokDecorationGroup_ModelAndScale& ms = capsulesCache.expandOne();
      ms.model = pModel;
      ms.scale = instanceScale;
      ms.shapeStartIndex = iShapesIndex;      
    }
    // If found, retrieve the index into the array of shapes from the cache.
    else
    {
      iShapesIndex = capsulesCache[iCacheIndex].shapeStartIndex;
    }

	

    // Add all capsules and their transformation to the compound shape.
      // (translation needs to be computed though)
      const VHavokDecorationGroup_ShapeInfo* const shapeInfoPtr = &shapeInfos[iShapesIndex];
      for (int j = 0; j < iCapsulesCount; j++)
    {
        // translate to origin of capsule
        hkQsTransform shapeTransform = instanceTransform;
        hkVector4 vs;
        vs._setRotatedDir(instanceTransform.m_rotation, shapeInfoPtr[j].vLocalOffset);
        shapeTransform.m_translation.add(vs);

        // Add to compound shape
        compoundShape->addInstance(shapeInfoPtr[j].pShape, shapeTransform);
      }
    }

  // Bake the compound shape and create the Havok Rigid Body
  compoundShape->bake();
  hkpRigidBodyCinfo rigidBodyInfo;
  rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
  rigidBodyInfo.m_collisionFilterInfo = hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN);
  rigidBodyInfo.m_shape = compoundShape;
  m_pCompoundRigidBody= new hkpRigidBody(rigidBodyInfo);

  // Clear
  compoundShape->removeReference();
  for (int s = 0; s < shapeInfos.getSize(); s++)
  {
    shapeInfos[s].pShape->removeReference();
  }	
}

VHavokDecorationGroup::~VHavokDecorationGroup()
{
  CommonDeinit();
}

void VHavokDecorationGroup::CommonDeinit()
{
  m_pCompoundRigidBody->removeReference();
  m_pCompoundRigidBody = HK_NULL;
}

void VHavokDecorationGroup::DisposeObject()
{
  CommonDeinit();
}

hkpRigidBody* VHavokDecorationGroup::GetHkRigidBody()
{
  return (hkpRigidBody*)m_pCompoundRigidBody;
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
