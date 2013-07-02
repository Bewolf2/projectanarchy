/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_CONTROLLER_H
#define HKB_CHARACTER_CONTROLLER_H

	/// Interface between Behavior and Physics to control a character (used by hkbCharacterControllerDriver).
class hkbCharacterController : public hkReferencedObject
{
public:

	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	
		/// The character driver will call this function to integrate the character controller.
		/// The character controller need not update it's position immediately if isIntegratedWithPhysics() == true.
		/// But the position should be updated after hkbPhysicsInterface::step is called.
	virtual void integrate( hkVector4Parameter newVelocity, hkReal timestep ) = 0;

		/// Indicates if the controller is integrated as part of hkbPhysicsInterface::step.
	virtual bool isIntegratedWithPhysics() const = 0;

		/// The current position of the character controller.
	virtual void getPosition( hkVector4& positionOut ) const = 0;

		/// The current linear velocity of the character controller.
	virtual void getLinearVelocity( hkVector4& linearVelocityOut ) const = 0;

		/// Checks if the character controller is currently supported in the down direction, this is called once per frame
		/// after the character is integrated and hkbPhysicsInterface::step is (possibly) called. If the character is
		/// supported it should return true and fill out supportingNormalOut.
	virtual bool checkSupport( hkVector4Parameter down, hkReal timestep, hkVector4& supportingNormalOut ) = 0;

		/// Sets the collision filter info dynamically (after creation)
	virtual void setCollisionFilterInfo( hkUint32 filterInfo ) = 0;
};

#endif // HKB_CHARACTER_CONTROLLER_H

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
