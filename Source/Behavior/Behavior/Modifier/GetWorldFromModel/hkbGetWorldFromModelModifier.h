/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GET_WORLD_FROM_MODEL_MODIFIER_H
#define HKB_GET_WORLD_FROM_MODEL_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbGetWorldFromModelModifierClass;

	/// A modifier that gets the worldFromModel.
class hkbGetWorldFromModelModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbGetWorldFromModelModifier();
		hkbGetWorldFromModelModifier( const hkbGetWorldFromModelModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// The current position of world from model.
		hkVector4 m_translationOut;	//+default(0,0,0,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
									//+hk.Description("The current position of world from model.")

			/// The current rotation of world from model.
		hkQuaternion m_rotationOut;	//+default(0,0,0,1)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
									//+hk.Description("The current rotation of world from model.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		// these are commented out on purpose to force the output properties to be part of the internal state
		//hkVector4 m_translationOut;
		//hkQuaternion m_rotationOut;

		HKB_END_INTERNAL_STATE();

	public:

		hkbGetWorldFromModelModifier( hkFinishLoadedObjectFlag flag );

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
