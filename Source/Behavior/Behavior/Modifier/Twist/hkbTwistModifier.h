/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_TWIST_MODIFIER_H
#define HKB_TWIST_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Animation/Animation/Animation/hkaAnimation.h>

extern const class hkClass hkbTwistModifierClass;

class hkaMirroredSkeleton;

/// A modifier to twist a chain of bones (such as the spine of the character) around a given axis.
class hkbTwistModifier : public hkbModifier
{
	public:

		//+version(1)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Constructor.
		hkbTwistModifier();

	protected:

		hkbTwistModifier( const hkbTwistModifier& mod );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
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

	public:

		enum SetAngleMethod
		{ 
			LINEAR,
			RAMPED, 
		};

			/// Whether m_axisOfRotation is in model space or local space.
		enum RotationAxisCoordinates
		{
				/// Indicates that m_axisOfRotation is in model space.
			ROTATION_AXIS_IN_MODEL_COORDINATES = 0,

				/// Indicates that m_axisOfRotation is in local parent space.
			ROTATION_AXIS_IN_PARENT_COORDINATES,

				/// Indicates that m_axisOfRotation is in local space.
			ROTATION_AXIS_IN_LOCAL_COORDINATES,
		};

			/// The axis of rotation.
		hkVector4 m_axisOfRotation; //+default(1,0,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("Defines the axis of rotation along the chain")

			/// The total twist angle to apply to chain of bones
		hkReal m_twistAngle;	//+default(0.0f)
								//+hk.RangeReal(absmin=-360,absmax=360)
								//+hk.Description("The total twist angle in degrees distributed along the chain")

			/// Index of the first bone in the chain.  This bone must be closer to the root than endBoneIndex.
		hkInt16 m_startBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The first bone in the chain.  This bone must be closer to the root than endBoneIndex.")

			/// Index of the last bone in the chain.  This bone must be farther from the root than startBoneIndex.
		hkInt16 m_endBoneIndex; //+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
								//+hk.Description("The last bone in the chain.  This bone must be farther from the root than startBoneIndex.")

			/// Twist angle per bone increased via LINEAR or RAMPED method.
		hkEnum< SetAngleMethod, hkInt8 > m_setAngleMethod;	//+default(0)
															//+hk.Description("How the rotations are distributed along the bone chain. Ramped distributes the angles exponentially whereas linear distributes the angles equally")

			/// Whether the m_axisOfRotation is in model space or local space.
		hkEnum<RotationAxisCoordinates, hkInt8> m_rotationAxisCoordinates;	//+default(hkbTwistModifier::ROTATION_AXIS_IN_MODEL_COORDINATES)
																			//+hk.Description("Whether the m_axisOfRotation is in model space, parent bone, or local space.")

			/// Twist angle per bone is ADDITIVE or NOT
		hkBool m_isAdditive;	//+default(1)
								//+hk.Description("If true the twist angle per bone adds to the existing rotation, otherwise it replaces it")

	protected:

			// Get the rootward and leafward bones
		void getBonesForChain( hkInt16& rootwardBoneIdx, hkInt16& leafwardBoneIdx ) const;

			// The bone chain which we want to twist
		hkArray<hkInt16> m_boneChainIndices; //+nosave

			// The parent indices of m_startBoneIndex
		hkArray<hkInt16> m_parentBoneIndices; //+nosave

	public:

		hkbTwistModifier( hkFinishLoadedObjectFlag flag );

		friend class hkbTwistModifierUtils;

#if defined (HK_PLATFORM_HAS_SPU)
		friend class hkbSpuBehaviorUtils;
#endif
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
