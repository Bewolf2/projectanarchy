/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_CONTROLLER_DRIVER_H
#define HKB_CHARACTER_CONTROLLER_DRIVER_H

#include <Behavior/Utilities/Physics/hkbPhysicsBaseTypes.h>

class hkbContext;
class hkbGeneratorOutput;
class hkbCharacter;

	/// Manages the character controller associated with a behavior character.
class hkbCharacterControllerDriver : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			/// Ctor.
		hkbCharacterControllerDriver();

			/// Dtor.
		~hkbCharacterControllerDriver();

			/// Returns if the controller is enabled.
		bool isEnabled() const;

			/// Sets if the controller is enabled.
			/// This can cause an hkbCharacterController to be deleted.
		void setEnabled( bool enabled );

			/// Drives the hkbCharacterController forward based on Behavior data.
		void drive( hkReal timestep, hkbContext& context, const hkbGeneratorOutput& generatorOutput );

			/// Use hkbCharacterController results to feedback into Behavior.
		void postPhysics( hkReal timestep, hkbContext& context, hkbGeneratorOutput& inOut );

			/// Returns whether the underlying hkbCharacterController is integrated with Physics.
		bool isIntegratedWithPhysics() const;

			/// The current position of the character controller.
		const hkVector4& getPosition() const;

			/// The current linear velocity of the character controller.
		const hkVector4& getLinearVelocity() const;

			/// Returns whether the underlying hkbCharacterController is supported.
		bool isSupported() const;

			/// Returns the supporting normal calculated by the hkbCharacterController.
		const hkVector4& getSupportingNormal() const;

			/// Reset the hkbCharacterController (this will delete the controller if it exists, it will be recreated as needed).
		void reset();

			/// Get the underlying controller, this may be null
		const hkbCharacterController* getController() const;

			/// Sets the collision filter on the controller dynamically (after creation)
		void setCollisionFilterInfo( hkUint32 filterInfo );

	protected:

			// Helper function to create the controller.
		void createController( hkbPhysicsInterface* physicsInterface, const hkbContext& context, const hkQsTransform& initialTransform );

			// The current controller (maybe null).
		hkRefPtr<hkbCharacterController> m_controller;

			// The expected or anticipated position of the controller after it is integrated.
		hkVector4 m_expectedPosition;

			// The horizontal displacement used to calculate the final position. 
		hkVector4 m_horizontalDisplacement;

			// The vertical displacement used to calculate the final position.
		hkReal m_verticalDisplacement;

			// Indicating the controller reported it was supported.
		hkBool m_isSupported;
		
			// Stored supporting normal from the controller.
		hkVector4 m_supportingNormal;

			// Stored from current character controller
		hkVector4 m_storedPosition;

			// Stored from current character controller
		hkVector4 m_storedLinearVelocity;

			// Current enabled status.
		hkBool m_isEnabled;
};

#endif // HKB_CHARACTER_CONTROLLER_DRIVER_H

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
