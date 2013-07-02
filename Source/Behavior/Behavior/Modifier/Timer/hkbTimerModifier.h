/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TIMER_MODIFIER_H
#define HKB_TIMER_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbTimerModifierClass;

	/// A modifier for sending an event after a time lapse.
class hkbTimerModifier : public hkbModifier
{
	public:
		// +version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbTimerModifier();
		hkbTimerModifier( const hkbTimerModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// When the timer alarm goes off.
		hkReal m_alarmTimeSeconds;	//+default(0)
									//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")
									//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
									//+hk.Description("An event is sent at this time.")

			/// The event id to send when the alarm goes off.
		class hkbEventProperty m_alarmEvent;	//+hk.Description("The event to send when the timer goes off.")		

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		hkReal m_secondsElapsed; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbTimerModifier( hkFinishLoadedObjectFlag flag );

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
