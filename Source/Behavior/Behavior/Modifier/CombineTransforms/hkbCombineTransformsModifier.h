/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_COMBINE_TRANSFORMS_MODIFIER_H
#define HKB_COMBINE_TRANSFORMS_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbCombineTransformsModifierClass;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// This modifier multiplies two input transforms and produces an output transform.  The inputs and outputs
	/// are in the form of a translation vector and a quaternion.  You can optionally request inversion of the
	/// inputs and output.
class hkbCombineTransformsModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbCombineTransformsModifier();

	protected:

		hkbCombineTransformsModifier( const hkbCombineTransformsModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

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

			/// The translation of the combined transform.
		hkVector4 m_translationOut;	//+default(0,0,0,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The position of the combined transform.")
			
			/// The orientation of the combined transform.
		hkQuaternion m_rotationOut;	//+default(0,0,0,1)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The orientation of the combined transform.")

			// The translation of the left transform to be combined.
		hkVector4 m_leftTranslation;	//+default(0,0,0,0)
										//+hk.Description("The translation of the left transform to be combined")

			// The rotation of the left transform to be combined.
		hkQuaternion m_leftRotation;	//+default(0,0,0,0)
										//+hk.Description("The rotation of the left transform to be combined")

			// The translation of the right transform to be combined.
		hkVector4 m_rightTranslation;	//+default(0,0,0,0)
										//+hk.Description("The translation of the right transform to be combined")

			// The rotation of the right transform to be combined.
		hkQuaternion m_rightRotation;	//+default(0,0,0,0)
										//+hk.Description("The rotation of the right transform to be combined")

			// The left transform is inverted before the transforms are combined.
		hkBool m_invertLeftTransform;	//+default(false)
										//+hk.Description("The left transform is inverted before the transforms are combined")

			// The right transform is inverted before the transforms are combined.
		hkBool m_invertRightTransform;	//+default(false)
										//+hk.Description("The right transform is inverted before the transforms are combined")

			// The resulting transform is inverted after the transforms are combined.
		hkBool m_invertResult;	//+default(false)
								//+hk.Description("The resulting transform is inverted after the transforms are combined")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		// these are commented out on purpose to force the output properties to be part of the internal state
		//hkVector4 m_translationOut;
		//hkQuaternion m_rotationOut;

		HKB_END_INTERNAL_STATE();

	public:

		hkbCombineTransformsModifier( hkFinishLoadedObjectFlag flag );

};

#endif // HKB_COMBINE_TRANSFORMS_MODIFIER_H

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
