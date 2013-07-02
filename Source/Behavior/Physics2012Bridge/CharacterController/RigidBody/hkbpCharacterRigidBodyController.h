/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_RIGID_BODY_CONTROLLER_H
#define HKB_CHARACTER_RIGID_BODY_CONTROLLER_H

#include <Behavior/Physics2012Bridge/CharacterController/hkbpCharacterController.h>
#include <Physics2012/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>

	/// This is a wrapper for a Havok Physics2012 rigid-body-based character controller.
class hkbpCharacterRigidBodyController : public hkbpCharacterController
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Ctor
		hkbpCharacterRigidBodyController( hkpWorld& world, hkpCharacterRigidBodyCinfo& characterRigidBodyCinfo );

			/// Destructor, removes the rigid body from the physics world.
		virtual ~hkbpCharacterRigidBodyController();

	//////////////////////////////////////////////////////////////////////////
	// hkbCharacterController interface
	//////////////////////////////////////////////////////////////////////////

			// hkbCharacterController interface
		virtual void integrate( hkVector4Parameter newVelocity, hkReal timestep ) HK_OVERRIDE;

			// hkbCharacterController interface
		virtual bool isIntegratedWithPhysics() const HK_OVERRIDE;

			// hkbCharacterController interface
		virtual void getPosition( hkVector4& positionOut ) const HK_OVERRIDE;

			// hkbCharacterController interface
		virtual void getLinearVelocity( hkVector4& positionOut ) const HK_OVERRIDE;

			// hkbCharacterController interface
		virtual bool checkSupport( hkVector4Parameter down, hkReal timestep, hkVector4& supportingNormalOut ) HK_OVERRIDE;

			// hkbCharacterContoller interface
		virtual void setCollisionFilterInfo( hkUint32 filterInfo ) HK_OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// hkbpCharacterController interface
	//////////////////////////////////////////////////////////////////////////

			// hkbpCharacterController interface
		virtual const hkpCollidable* getCollidable() const HK_OVERRIDE;

	protected:

			// The rigid body is in the world
		bool isInWorld() const;

			// The character rigid body being simulated
		hkRefPtr<hkpCharacterRigidBody> m_characterRigidBody;
};

#endif // HKB_CHARACTER_RIGID_BODY_CONTROLLER_H

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
