/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ROTATE_CHARACTER_MODIFIER_H
#define HKB_ROTATE_CHARACTER_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbRotateCharacterModifierClass;

struct hkbRotateCharacterModifierModifySetup;

	/// A modifier that rotates the character worldFromModel in response to events.
class hkbRotateCharacterModifier : public hkbModifier
{
	//+version(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbRotateCharacterModifier();
		hkbRotateCharacterModifier( const hkbRotateCharacterModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			// hkbModifier interface implementation
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The speed of rotation.
		hkReal m_degreesPerSecond; //+default(1.0f)+hk.RangeReal(absmin=-720,absmax=720,softmin=-360,softmax=360)+hk.Description("The speed of rotation.")

			/// The speed of rotation multiplier.
		hkReal m_speedMultiplier; //+default(1.0f)+hk.RangeReal(absmin=-1000,absmax=1000,softmin=-100,softmax=100)+hk.Description("The speed of rotation multiplier.  The speed of rotation is multiplied by this factor.")

			/// The axis of rotation.
		hkVector4 m_axisOfRotation; //+default(1,0,0)+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")+hk.Description("The axis of rotation.")

	private:
		
		HKB_BEGIN_INTERNAL_STATE(0);

		hkReal m_angle; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbRotateCharacterModifier( hkFinishLoadedObjectFlag flag );

		friend class hkbRotateCharacterModifierUtils;

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
