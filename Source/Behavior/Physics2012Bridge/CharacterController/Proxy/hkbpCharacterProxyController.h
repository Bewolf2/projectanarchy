/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CHARACTER_PROXY_CONTROLLER_H
#define HKB_CHARACTER_PROXY_CONTROLLER_H

#include <Behavior/Physics2012Bridge/CharacterController/hkbpCharacterController.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>

	/// This is a wrapper for a Havok Physics2012 proxy-based character controller.
class hkbpCharacterProxyController : public hkbpCharacterController
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );

			// Ctor
		hkbpCharacterProxyController( hkpWorld& world, hkpCharacterProxyCinfo& characterProxy );

			// Dtor.
		virtual ~hkbpCharacterProxyController();

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

			/// The phantom is in the world
		bool isInWorld() const;

			/// The proxy associated with the character.
		hkRefPtr<hkpCharacterProxy> m_characterProxy;
};

#endif // HKB_CHARACTER_PROXY_CONTROLLER_H

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
