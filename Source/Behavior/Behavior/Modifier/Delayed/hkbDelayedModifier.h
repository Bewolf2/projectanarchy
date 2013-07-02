/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_DELAYED_MODIFIER_H
#define HKB_DELAYED_MODIFIER_H

#include <Behavior/Behavior/Modifier/Wrapper/hkbModifierWrapper.h>

extern const class hkClass hkbDelayedModifierClass;

/// A modifier that applies another modifier after a delay, and optionally for
/// a finite duration.
class hkbDelayedModifier : public hkbModifierWrapper
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbDelayedModifier();

	protected:

		hkbDelayedModifier( const hkbDelayedModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			// hkbModifier interface implementation
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

			// hkbModifier interface implementation
		virtual void modifySyncInfo( const hkbContext& context, hkbGeneratorSyncInfo& syncInfo ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The number of seconds to delay before applying the modifier.
		hkReal m_delaySeconds;	//+default(0.0f)
								//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
								//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10)
								//+hk.Description("The number of seconds to delay before applying the modifier.")

			/// The number of seconds to apply the modifier after the delay.  Set this to zero to apply the modifier indefinitely.
		hkReal m_durationSeconds;	//+default(0.0f)
									//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
									//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10)
									//+hk.Description("The number of seconds to apply the modifier after the delay.  Set this to zero to apply the modifier indefinitely.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		hkReal m_secondsElapsed; //+nosave
		hkBool m_isActive; //+nosave

		HKB_END_INTERNAL_STATE();

			// whether or not the child modifier is active
		bool isChildActive() const;

	public:

		hkbDelayedModifier( hkFinishLoadedObjectFlag flag );

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
