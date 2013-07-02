/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokContactListener.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************
#ifndef VHAVOKCONTACTLISTENER_HPP_INCLUDED
#define VHAVOKCONTACTLISTENER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokUserData.hpp>

class hkpContactListener;
class vHavokRigidBody;
class vHavokCharacterController;
class vHavokRagdoll;
class vHavokStaticMesh;
class vHavokTerrain;


/// 
/// \brief
///    Structure that holds information about one collider object.
///
/// Basically a collider object can be one of the following - dynamic object, static mesh or terrain.
/// When a collision is detected a message is sent to both colliders containing information about the
/// collision in the vHavokCollisionInfo_t structure.
///
struct vHavokColliderInfo_t
{
  vHavokColliderInfo_t()
  {
    m_pRigidBody = NULL;
    m_pCharacter = NULL;
    m_pStaticMesh = NULL;
#ifdef SUPPORTS_TERRAIN
    m_pTerrainSector = NULL; 
#endif
	m_pRagdoll = NULL;
    m_eType = V_USERDATA_UNDEFINED;
  }

  void SetInfo(void *pUserData);

  /// \brief
  ///   Internal helper to copy the content of the physics collider to physics engine independent representation
  void FillScriptInfo(VScriptColliderInfo & collider);

  vHavokUserDataType_e m_eType;            ///< Type of collider object
  vHavokRigidBody *m_pRigidBody;           ///< Physics object (can be NULL)
  vHavokCharacterController *m_pCharacter; ///< Character controller (can be NULL)
  vHavokRagdoll *m_pRagdoll;               ///< Ragdoll component (can be NULL)
  vHavokStaticMesh *m_pStaticMesh;         ///< Static mesh (can be NULL)
#ifdef SUPPORTS_TERRAIN
  vHavokTerrain *m_pTerrainSector;         ///< Terrain sector (can be NULL)
#endif

};


///
/// \brief
///   Structure that holds information about a collision.
///
/// When a collision is detected a message is sent to both colliders containing information about the
/// collision in this structure, for instance information about the impact point.
///
struct vHavokCollisionInfo_t
{
  vHavokCollisionInfo_t()
  {
    m_fVelocity = 0.0f;

    // init to Havok default values
    m_Materials[0].fDynamicFriction = m_Materials[1].fDynamicFriction = 0.5f;
    m_Materials[0].fRestitution = m_Materials[1].fRestitution = 0.4f;
    m_pSenderTarget[0] = m_pSenderTarget[1] = NULL;
  }

  // element access indices for m_Collider and m_pMaterials correspond to each other
  vHavokColliderInfo_t m_Collider[2];   ///< Pair of colliders this notification is about.
  VColMeshMaterial m_Materials[2];      ///< Pair of physics materials, which are involved into collision.
                                        ///< The materials are triangle based, if available, otherwise shape based.
                                        ///< If no materials were explicitly assigned to the corresponding colliders, a default material will be returned.

  hkvVec3 m_vPoint;                ///< Impact Point
  hkvVec3 m_vNormal;               ///< Impact Normal 
  hkReal m_fVelocity;                    ///< Relative velocity at the point of contact projected onto the collision normal
  VisTypedEngineObject_cl *m_pSenderTarget[2];
};


/// \brief 
///   Holds the hit's result for a sweep query
struct vHavokSweepResult
{
  vHavokSweepResult()
  {
    m_fDistance = 0.0f;
  }

  hkReal           m_fDistance;    ///< Distance to the hit point
  hkvVec3          m_vTouchPos;    ///< World-space impact point
  hkvVec3          m_vNormal;      ///< World-space impact normal
  vHavokColliderInfo_t  m_collInfo;     ///< Collider info
};


///
/// \brief
///    Class that implements the hkpContactListener Interface which is responsible for Havok collision callbacks.
///
/// The Havok contact listener is added to the Havok world to provide collision information. It responds to
/// collision callbacks, collects the collision information and sends messages to the collider objects. 
/// 
class vHavokContactListener : public VRefCounter, public hkpContactListener, public hkpEntityListener
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// 
  /// \brief
  /// Constructor of the Havok Contact Listener.
  /// 
  /// \param pPhysicsWorld
  ///   Pointer to the Havok physics world the Contact listener is added to.
  /// 
  vHavokContactListener(hkpWorld* pPhysicsWorld);  


  /// 
  /// \brief
  /// Destructor of the Havok Contact Listener.
  ///
  ~vHavokContactListener();


  ///
  /// @}
  ///


  ///
  /// @name hkpContactListener Virtual Overrides
  /// @{
  ///


  /// 
  /// \brief
  ///   Called for each new contact point between two bodies. 
  /// 
  /// This method is triggered when the contact point is generated by the Havok system. 
  /// This is the place where basic collision information is added.
  /// 
  /// \param event
  ///   Struct that contains detailed information about the collision.
  /// 
  /// \see
  ///   vHavokContactListener::collisionAddedCallback
  ///   vHavokContactListener::collisionRemovedCallback
  ///
  VOVERRIDE void contactPointCallback (const hkpContactPointEvent &event);


  /// 
  /// \brief
  ///   Called for a new collision between two bodies.
  /// 
  /// This method is triggered when the contact point is generated by the Havok system. 
  /// This is the place where basic collision information is added.
  /// 
  /// \param event
  ///   Struct that contains detailed information about the collision.
  /// 
  /// \see
  ///   vHavokContactListener::contactPointCallback
  ///   vHavokContactListener::collisionRemovedCallback
  ///
  VOVERRIDE void collisionAddedCallback (const hkpCollisionEvent &event);


  /// 
  /// \brief
  ///   Called when two bodies are no longer colliding.
  /// 
  /// This method is triggered when the contact point is generated by the Havok system. 
  /// This is the place where basic collision information is added.
  /// 
  /// \param event
  ///   Struct that contains detailed information about the collision.
  /// 
  /// \see
  ///   vHavokContactListener::contactPointCallback
  ///   vHavokContactListener::collisionAddedCallback
  ///
  VOVERRIDE void collisionRemovedCallback (const hkpCollisionEvent &event);


  ///
  /// @}
  ///


};

#endif // VHAVOKCONTACTLISTENER_HPP_INCLUDED

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
