/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GRAVITY_MODIFIER_H
#define HKB_GRAVITY_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbGravityModifierClass;

/// An example custom modifier the computes the motion of the character
/// by applying gravity given an initial velocity.
class hkbGravityModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbGravityModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

		// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

		// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// The initial velocity of the character. X - character forward in model space , Y - character right in model space and Z - character up in model space.
		hkVector4 m_initialVelocityInMS; //+default(0,0,0)+hk.Description("The initial velocity of the world from model. X - character forward in model space , Y - character right in model space and Z - character up in model space.")

			/// Gravity constant.
		hkReal m_gravityConstant; //+default(9.8f)+hk.RangeReal(absmin=-1000,absmax=1000,softmin=-100,softmax=100)+hk.Description("Gravity constant.")

	private:

			/// The current velocity of the character in model space.
		hkVector4 m_currentVelocityInMS; //+nosave

			/// time elapsed since modify was called
		hkReal m_secondsElapsed; //+nosave

	public:

		hkbGravityModifier( hkFinishLoadedObjectFlag flag );

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
