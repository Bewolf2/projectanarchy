/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_EVENT_DRIVEN_MODIFIER_H
#define HKB_EVENT_DRIVEN_MODIFIER_H

#include <Behavior/Behavior/Modifier/Wrapper/hkbModifierWrapper.h>

extern const class hkClass hkbEventDrivenModifierClass;

/// A modifier that activates/deactivates its child modifier on receiving
/// events
class hkbEventDrivenModifier : public hkbModifierWrapper
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbEventDrivenModifier();

	protected:

		hkbEventDrivenModifier( const hkbEventDrivenModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void handleEvent(const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getActiveEvents( hkPointerMap<int, bool>& activeEvents ) const HK_OVERRIDE;

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

			/// Event used to activate the wrapped modifier
		hkInt32 m_activateEventId;	//+default(hkbEvent::EVENT_ID_NULL)
									//+hkb.RoleAttribute("ROLE_EVENT_ID")
									//+hk.Description("Event used to activate the wrapped modifier.")

			/// Event used to deactivate the wrapped modifier
		hkInt32 m_deactivateEventId;	//+default(hkbEvent::EVENT_ID_NULL)
										//+hkb.RoleAttribute("ROLE_EVENT_ID")
										//+hk.Description("Event used to deactivate the wrapped modifier.")

		hkBool m_activeByDefault;	//+default(false)
									//+hk.Description("If the child needs to be activated when this modifier is activated set this to true.")

	private:
		
		HKB_BEGIN_INTERNAL_STATE(0);

			// Whether the child node is active or not.
		hkBool m_isActive; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbEventDrivenModifier( hkFinishLoadedObjectFlag flag );
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
