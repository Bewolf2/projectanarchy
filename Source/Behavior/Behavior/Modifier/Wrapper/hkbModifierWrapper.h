/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MODIFIER_WRAPPER_H
#define HKB_MODIFIER_WRAPPER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbModifierWrapperClass;

/// A modifier that wraps another modifier
class hkbModifierWrapper : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

	protected:

		hkbModifierWrapper( hkbNodeType nodeType );

		hkbModifierWrapper( const hkbModifierWrapper& mod );

	public:

		~hkbModifierWrapper();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;		

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	public:

			/// Set the modifier that is to be wrapped.
		void setModifier( hkbModifier* modifier );

		///////////////
		// properties
		///////////////

			/// The nested modifier that is to be wrapped.
		hkRefPtr<hkbModifier> m_modifier;	//+hk.Description("The modifier that is to be wrapped.")

	public:

		hkbModifierWrapper( hkFinishLoadedObjectFlag flag ) : hkbModifier(flag), m_modifier(flag) {}

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
