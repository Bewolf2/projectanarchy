/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FACE_TARGET_MODIFIER_H
#define HKB_FACE_TARGET_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Target/hkbpTarget.h>

extern const class hkClass hkbpFaceTargetModifierClass;

/// An example custom modifier for facing the character toward a target.
class hkbpFaceTargetModifier : public hkbModifier
{
	//+version(2)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpFaceTargetModifier();

	private:

		hkbpFaceTargetModifier( const hkbpFaceTargetModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// The target towards which we want to face.
		hkRefPtr<hkbpTarget> m_targetIn;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description( "The target towards which we want to face." )
										
			/// This angle offsets the angle required to face the target.
		hkReal m_offsetAngle;

			/// If true, the modifier is only done once (for each begin-end pair)
		hkBool m_onlyOnce;

	private:

			// whether or not the modifier has faced the target yet
		hkBool m_done; //+nosave

	public:

		hkbpFaceTargetModifier( hkFinishLoadedObjectFlag flag );

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
