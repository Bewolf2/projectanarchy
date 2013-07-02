/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_CONSTRAIN_RIGID_BODY_MODIFIER_H
#define HKBP_CONSTRAIN_RIGID_BODY_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Target/hkbpTarget.h>
#include <Physics/Constraint/Data/hkpConstraintData.h>

extern const class hkClass hkbpConstrainRigidBodyModifierClass;

/// An example custom modifier for constraining a rigid body.
class hkbpConstrainRigidBodyModifier : public hkbModifier
{
	//+version(2)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpConstrainRigidBodyModifier();

	private:

		hkbpConstrainRigidBodyModifier( const hkbpConstrainRigidBodyModifier& mod );

	public:

		~hkbpConstrainRigidBodyModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

		// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

		// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

		/// The target to constrain to.
		hkRefPtr<hkbpTarget> m_targetIn;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("The target to constrain to.")
										
			/// threshold for breaking
		hkReal m_breakThreshold; //+default(1.0f)+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The break threshold if the constrain is breakable.")

		enum PivotPlacement
		{
			/// place the pivot midway between the centroid the the rigid body to constraint and the target rigid body
			PIVOT_MIDWAY_BETWEEN_CENTROIDS,

			/// place the pivot at the contact point on the target rigid body
			PIVOT_AT_TARGET_CONTACT_POINT,

			///place the pivot midway between the centeroid of the rigid body to constrain and the target rigid body shape centeroid
			PIVOT_MIDWAY_BETWEEN_TARGET_SHAPE_CENTROID_AND_BODY_TO_CONSTRAIN_CENTROID,
		};

		enum BoneToConstrainPlacement
		{
			/// Constrain bone to constrain at its current position
			BTCP_AT_CURRENT_POSITION,

			/// Constrain bone to constrain such that the pivot point and its center of mass coincide
			BTCP_ALIGN_COM_AND_PIVOT,
		};

			/// the rigid body to constrain
		hkInt16 m_ragdollBoneToConstrain; //+default(-1)+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")+hk.Description("The index of the ragdoll bone to constrain.")

			/// whether or not the constraint is breakable
		hkBool m_breakable; //+default(false)+hk.Description("Whether the constrain is breakable on not.")

			/// indicates where to place the pivot of the constraint
		hkEnum< PivotPlacement, hkInt8 > m_pivotPlacement; //+hk.Description("Where to place the pivot of the constrain.")

			/// indicates where to place the bone to constrain at the time of constraining.
		hkEnum<hkbpConstrainRigidBodyModifier::BoneToConstrainPlacement,hkInt8> m_boneToConstrainPlacement; //+hk.Description("Where to place the bone to constrain at the time of constraining.")

			/// Type of constraint (only BallAndSocket and limited hinge has been implemented)
		hkEnum<hkpConstraintData::ConstraintType,hkInt8> m_constraintType; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")+hk.Description("The type of the constrain.")

			/// When this is set to true after the variable data has been consumed it is cleared every frame.
		hkBool m_clearTargetData; //+default(false)+hk.Description("When this is set to true after the variable data has been consumed it is cleared every frame.")

			/// If this is true the constraint is hinge otherwise it is ball and socket.
		hkBool m_isConstraintHinge; //+default(true)+hk.Description("If this is true the constraint is hinge otherwise it is ball and socket.")

	private:

		// find and add the constraint
		void addConstraint( const hkbContext& context );

		// remove the constraint
		void removeConstraint( const hkbContext& context );

		class hkpConstraintInstance* m_constraint; //+nosave

	public:

		hkbpConstrainRigidBodyModifier( hkFinishLoadedObjectFlag flag );
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
