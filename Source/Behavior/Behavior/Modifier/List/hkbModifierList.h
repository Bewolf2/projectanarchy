/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MODIFIER_LIST_H
#define HKB_MODIFIER_LIST_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbModifierListClass;

/// Applies a sequence of modifiers as a single modifier.
///
/// This class is especially useful if a behavior graph shares a sequence of 
/// modifiers among several branches.  You can create the sequence once and then
/// just add it to each branch instead of having to add all of the modifiers
/// individually.
class hkbModifierList : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();
		
			/// Contruct an empty sequence.
		hkbModifierList() : hkbModifier(HKB_NODE_TYPE_MODIFIER_LIST) {}

	private:

		hkbModifierList( const hkbModifierList& modList );

	public:

		~hkbModifierList();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

			// hkbNode interface implementation
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		/////////////////////////
		// hkbModifier interface
		/////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			// hkbModifier interface implementation
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

			// hkbModifier interface implementation
		virtual void modifySyncInfo( const hkbContext& context, hkbGeneratorSyncInfo& syncInfo ) HK_OVERRIDE;

		//////////////////////////////////
		// hkbModifierList functions
		//////////////////////////////////

			/// Adds a modifier to the sequence.
		void addModifier( hkbModifier* modifier );

			/// Returns the modifiers.
		const hkArray< hkbModifier* >& getModifiers() { return m_modifiers; }

	public:

			/// The list of modifiers to apply. This is made public for HAT.
		hkArray<hkbModifier*> m_modifiers; //+hk.Description("The list of modifiers to apply.")

		hkbModifierList( hkFinishLoadedObjectFlag flag );
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
