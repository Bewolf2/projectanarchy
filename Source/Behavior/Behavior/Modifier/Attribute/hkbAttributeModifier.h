/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ATTRIBUTE_MODIFIER_H
#define HKB_ATTRIBUTE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbAttributeModifierAssignmentClass;

extern const class hkClass hkbAttributeModifierClass;

/// This modifier gives the user the ability to modify the values of the attributes. 
/// These are then copied into the track buffer. The data is passed 
/// through the behavior tree and can be blended by blend nodes and during transitions.
class hkbAttributeModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbAttributeModifier() : hkbModifier(HKB_NODE_TYPE_ATTRIBUTE_MODIFIER) {}

	protected:

		hkbAttributeModifier( const hkbAttributeModifier& mod );

	public:

		///////////////////////////
		// hkbNode interface
		///////////////////////////

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

		///////////////
		// properties
		///////////////

			/// A single assignment of a value to an attribute.
		struct Assignment
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAttributeModifier::Assignment );
			HK_DECLARE_REFLECTION();

				/// Construct with defaults.
			Assignment()
			:	m_attributeIndex(-1),
				m_attributeValue(0.0f) {}

				/// Construct with initial values.
			Assignment( int index, hkReal value )
			:	m_attributeIndex(index),
				m_attributeValue(value) {}

				/// The index of the attribute.
			int m_attributeIndex; //+default(-1)+hkb.RoleAttribute("ROLE_ATTRIBUTE_INDEX","FLAG_NONE")

				/// The value of the attribute.
			hkReal m_attributeValue; //+default(0.0f)
		};

			/// A list of attribute assignments.
		hkArray<struct Assignment> m_assignments;

	public:

		hkbAttributeModifier( hkFinishLoadedObjectFlag flag );
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
