/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokContactListener.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokTerrain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>

void vHavokColliderInfo_t::SetInfo(void *pUserData)
{
  void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void *)pUserData, m_eType);

  switch (m_eType)
  {
  case V_USERDATA_OBJECT:  
    {
      IVObjectComponent* c = (IVObjectComponent*)pObject; 
      if (c->IsOfType( vHavokRigidBody::GetClassTypeId() ))
      {
        m_pRigidBody = (vHavokRigidBody *)pObject;
      }
      else if (c->IsOfType( vHavokCharacterController::GetClassTypeId() ))
      {
        m_pCharacter =  (vHavokCharacterController *)pObject;
      }
	  else if (c->IsOfType( vHavokRagdoll::GetClassTypeId() ))
      {
        m_pRagdoll =  (vHavokRagdoll *)pObject;
      }
    }
    break;
  case V_USERDATA_STATIC:  m_pStaticMesh = (vHavokStaticMesh *)pObject; 
    break;
#ifdef SUPPORTS_TERRAIN
  case V_USERDATA_TERRAIN: m_pTerrainSector = (vHavokTerrain *)pObject;
    break;
#endif
  default:
    break;
  }
}


void vHavokColliderInfo_t::FillScriptInfo(VScriptColliderInfo & collider)
{
  if(m_pRigidBody)
  {
    collider.m_pObjectComponent = m_pRigidBody;
  }
  else if (m_pCharacter)
  {
    collider.m_pObjectComponent = m_pCharacter;
  }
  else if (m_pRagdoll)
  {
    collider.m_pObjectComponent = m_pRagdoll;
  }

  if(m_pStaticMesh)
  {
    //TODO: if this asserts, find out which particular static meshes
    VASSERT(m_pStaticMesh->GetNumValidStaticMeshes()>=1 && m_pStaticMesh->GetWrappedStaticMesh(0)!=NULL);
    collider.m_pStaticMesh = m_pStaticMesh->GetWrappedStaticMesh(0);
  }
#ifdef SUPPORTS_TERRAIN
  if(m_pTerrainSector)
    collider.m_pTerrainSector = m_pTerrainSector->GetWrappedTerrainSector();
#else
  collider.m_pTerrainSector = NULL;
#endif

  //there is no support for this...
  collider.m_pDecorationGroup = NULL;
}



// --------------------------------------------------------------------------
// CONSTRUCTOR
// --------------------------------------------------------------------------

vHavokContactListener::vHavokContactListener(hkpWorld* physicsWorld)
{
  physicsWorld->markForWrite();
  physicsWorld->addContactListener( this );
  physicsWorld->addEntityListener( this );
  physicsWorld->unmarkForWrite();
}

vHavokContactListener::~vHavokContactListener()
{
}

// -------------------------------------------------------------------------- //
// COLLISION LISTENER                                                         //
// -------------------------------------------------------------------------- //

void vHavokContactListener::contactPointCallback (const hkpContactPointEvent &event)
{
  vHavokCollisionInfo_t stackInfo;
  vHavokCollisionInfo_t* pInfo = &stackInfo;
  bool bApplyProperties = false;

  for (int i=0;i<2;i++)
  {
    // Get collider user data and process it to the collider info structure
    pInfo->m_Collider[i].SetInfo((void *)(event.getBody(i)->getUserData()));

    // Get physics material properties
    switch (pInfo->m_Collider[i].m_eType)
    {
    case V_USERDATA_OBJECT:
    case V_USERDATA_TERRAIN:
      {
        // Perform a check to be on the safe side, though the hitObject should never be a character here.
        if (!pInfo->m_Collider[i].m_pCharacter)
        {
          hkpRigidBody *pRigidBody = event.getBody(i);
          pInfo->m_Materials[i].fDynamicFriction = float( pRigidBody->getFriction() );
          pInfo->m_Materials[i].fRestitution = float( pRigidBody->getRestitution() );
        }
      }
      break;

    case V_USERDATA_STATIC:
      {
        // Get the leaf shape key of the contact
        hkpShapeKey* keys = event.getShapeKeys(i);

        // If shape storage was allocated
        if ( keys != HK_NULL )
        {
          hkpShapeKey key = keys[0];

          // Get shape
          const hkpShape *pShape = pInfo->m_Collider[i].m_pStaticMesh->GetHkShape();
          VASSERT(pShape);

          if (pShape->getClassType() == &hkvBvCompressedMeshShapeClass)
          {
            const hkvBvCompressedMeshShape *pMeshShape = (hkvBvCompressedMeshShape*)(pShape);
            const hkvMeshMaterialCache* matCache = (const hkvMeshMaterialCache*) ( pMeshShape->m_userData );
            if (matCache)
            {
              hkUint32 matId = pMeshShape->getPrimitiveUserData( key );
              VASSERT(matId <= (hkUint32)matCache->getSize());
              const hkvMeshMaterial& pMaterial = (*matCache)[matId];
        
              pInfo->m_Materials[i].fDynamicFriction = pMaterial.m_fFriction;
              pInfo->m_Materials[i].fRestitution = pMaterial.m_fRestitution;
              pInfo->m_Materials[i].szUserData = pMaterial.m_userData.cString();

              // Apply per triangle physics properties manually  
              bApplyProperties = true;

              break;
            }
          }

          hkpRigidBody *pRigidBody = pInfo->m_Collider[i].m_pStaticMesh->GetHkRigidBody();
          pInfo->m_Materials[i].fDynamicFriction = float( pRigidBody->getFriction() );
          pInfo->m_Materials[i].fRestitution = float( pRigidBody->getRestitution() );
        }
      }
      break;
    default:
      break;
    }
  }

  // Apply physics properties
  if (bApplyProperties)
  {
    // Set friction
    float fFriction = hkvMath::Min(pInfo->m_Materials[0].fDynamicFriction, pInfo->m_Materials[1].fDynamicFriction);
    event.m_contactPointProperties->setFriction(fFriction);

    // Set restitution
    float fRestitution = hkvMath::Min(pInfo->m_Materials[0].fRestitution, pInfo->m_Materials[1].fRestitution);
    event.m_contactPointProperties->setRestitution(fRestitution);
  }
  
  // Collect all information in our own data structure
  if (event.m_contactPointProperties->getUserData() == 0)
  {
      vHavokConversionUtils::PhysVecToVisVecWorld(event.m_contactPoint->getPosition(), pInfo->m_vPoint);
        vHavokConversionUtils::PhysVecToVisVec_noscale(event.m_contactPoint->getNormal(), pInfo->m_vNormal);
    pInfo->m_fVelocity = HK2VIS_FLOAT_SCALED(event.getSeparatingVelocity());

    // Set user data for further processing of the contact point
    event.m_contactPointProperties->setUserData( reinterpret_cast<hkUlong>(pInfo) );
  }
  else
  {
    pInfo = reinterpret_cast<vHavokCollisionInfo_t*>( event.m_contactPointProperties->getUserData() );
  }

  // By setting the ProcessContactCallbackDelay to 0 we will receive callbacks for 
  // any collisions processed for this body every frame (simulation step).
  // event.m_callbackFiredFrom->setProcessContactCallbackDelay(0); 

  // Send collision message to both collider owners
  if (pInfo->m_Collider[0].m_pRigidBody && pInfo->m_Collider[0].m_pRigidBody->GetOwner())
    pInfo->m_pSenderTarget[0] = pInfo->m_Collider[0].m_pRigidBody->GetOwner();
  if (pInfo->m_Collider[1].m_pRigidBody && pInfo->m_Collider[1].m_pRigidBody->GetOwner())
    pInfo->m_pSenderTarget[1] = pInfo->m_Collider[1].m_pRigidBody->GetOwner();
  vHavokPhysicsModule::GetInstance()->OnObjectCollision(*pInfo);

  // Delete it
  //V_SAFE_DELETE(info);
  event.m_contactPointProperties->setUserData( 0 );
}

void vHavokContactListener::collisionAddedCallback (const hkpCollisionEvent &event)
{

}

void vHavokContactListener::collisionRemovedCallback (const hkpCollisionEvent &event)
{

}

/*
void vHavokCollisionListener::contactPointAddedCallback(hkpContactPointAddedEvent& event)
{
  // Collect all information in our own data structure
  if (event.m_contactPointProperties->getUserData() == NULL)
  {
    vHavokCollisionInfo_t* info = new vHavokCollisionInfo_t();
    info->m_vPoint = HK2VIS_VECTOR_SCALED(event.m_contactPoint->getPosition());
    info->m_vNormal = HK2VIS_VECTOR_NOSCALE(event.m_contactPoint->getNormal());
    info->m_fVelocity = event.m_projectedVelocity;

    // Set user data for further processing of the contact point
    event.m_contactPointProperties->setUserData( reinterpret_cast<hkUlong>(info) );
  }

  // By setting the ProcessContactCallbackDelay to 0 we will receive callbacks for 
  // any collisions processed for this body every frame (simulation step).
  //event.m_callbackFiredFrom->setProcessContactCallbackDelay(0);  
}

void vHavokCollisionListener::contactProcessCallback(hkpContactProcessEvent& event)
{
  // Nothing special to do here right now. This is the place to modify contact point 
  // data for custom response. Rarely there's more than one process callback before 
  // the first confirmed callback.
}

void vHavokCollisionListener::contactPointConfirmedCallback(hkpContactPointConfirmedEvent& event)
{
  // Get our custom collision info
  vHavokCollisionInfo_t* info = reinterpret_cast<vHavokCollisionInfo_t*>( event.m_contactPointProperties->getUserData() );
  if (!info)
    return;

  // Get collider user data and process it to the collider info structure
  info->m_ColliderA.SetInfo((void *)((hkpWorldObject *)event.m_collidableA->getOwner())->getUserData());
  info->m_ColliderB.SetInfo((void *)((hkpWorldObject *)event.m_collidableB->getOwner())->getUserData());

  // Send collision message to both collider owners
  if (info->m_ColliderA.m_pObject && info->m_ColliderA.m_pObject->GetOwner())
    Vision::Game.SendMsg(info->m_ColliderA.m_pObject->GetOwner(), VIS_MSG_HAVOK_ONCOLLISION, (INT_PTR)info, sizeof(vHavokCollisionInfo_t));
  if (info->m_ColliderB.m_pObject && info->m_ColliderB.m_pObject->GetOwner())
    Vision::Game.SendMsg(info->m_ColliderB.m_pObject->GetOwner(), VIS_MSG_HAVOK_ONCOLLISION, (INT_PTR)info, sizeof(vHavokCollisionInfo_t));
}

void vHavokCollisionListener::contactPointRemovedCallback(hkpContactPointRemovedEvent& event)
{
  // Get our custom collision info
  vHavokCollisionInfo_t* info = reinterpret_cast<vHavokCollisionInfo_t*>( event.m_contactPointProperties->getUserData() );
  if (!info)
    return;

  // Delete it
  V_SAFE_DELETE(info);
  event.m_contactPointProperties->setUserData( NULL );
}
*/

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
