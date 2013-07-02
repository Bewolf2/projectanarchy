/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SET_WORLD_FROM_MODEL_MODIFIER_H
#define HKB_SET_WORLD_FROM_MODEL_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbSetWorldFromModelModifierClass;

	/// A modifier that sets the worldFromModel.
class hkbSetWorldFromModelModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbSetWorldFromModelModifier();
		hkbSetWorldFromModelModifier( const hkbSetWorldFromModelModifier& );

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

			// hkbModifier interface implementation
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// If m_setTranslation is true, the position of the character will be set to this.
		hkVector4 m_translation;	//+default(0,0,0,0)
									//+hk.Description("If m_setTranslation is true, the position of the character will be set to this.")

			/// If m_setRotation is true, the rotation of the character will be set to this.
		hkQuaternion m_rotation;	//+default(0,0,0,1)
									//+hk.Description("If m_setRotation is true, the rotation of the character will be set to this.")

			/// Set this to true to set the character's position to m_translation.
		hkBool m_setTranslation;	//+default(true)
									//+hk.Description("Set this to true to set the character's position to m_translation.")

			/// Set this to true to set the character's rotation to m_rotation.
		hkBool m_setRotation;		//+default(true)
									//+hk.Description("Set this to true to set the character's rotation to m_rotation.")

	public:

		hkbSetWorldFromModelModifier( hkFinishLoadedObjectFlag flag );
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
