/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TRANSFORM_VECTOR_MODIFIER_H
#define HKB_TRANSFORM_VECTOR_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbTransformVectorModifierClass;

	/// Deprecated.  Please use the hkbScriptGenerator instead.
	/// 
	/// Transforms a given vector by a given matrix.
class hkbTransformVectorModifier : public hkbModifier
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbTransformVectorModifier();
		hkbTransformVectorModifier( const hkbTransformVectorModifier& );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		//////////////
		// properties
		//////////////

			/// The rotational part of the transform to apply.
		hkQuaternion m_rotation;	//+default(0,0,0,1)
									//+hk.Description("The rotational part of the transform to apply.")

			/// The translational part of the transform to apply.
		hkVector4 m_translation;	//+hk.Description("The translational part of the transform to apply.")

			/// The vector to be transformed.
		hkVector4 m_vectorIn;	//+hk.Description("The vector to be transformed.")

			/// The transformed vector.
		hkVector4 m_vectorOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
								//+hk.Description("The transformed vector.")

			/// Just rotate the vector, don't transform it as a point.
		hkBool m_rotateOnly;	//+hk.Description("Just rotate the vector, don't transform it as a point.")

			/// Apply the inverse of the transformation.
		hkBool m_inverse;	//+hk.Description("Apply the inverse of the transformation.")

			/// Do the transform when the modifier is activated.
		hkBool m_computeOnActivate;	//+default(true)
									//+hk.Description("Do the transform when the modifier is activated.")

			/// Do the transform every time that modify is called.
		hkBool m_computeOnModify;	//+hk.Description("Do the transform every time that modify is called.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

		// this is commented out on purpose to force the output properties to be part of the internal state
		//hkVector4 m_vectorOut;

		HKB_END_INTERNAL_STATE();

	public:

		hkbTransformVectorModifier( hkFinishLoadedObjectFlag flag );
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
