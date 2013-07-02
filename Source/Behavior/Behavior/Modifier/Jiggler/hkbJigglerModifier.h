/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_JIGGLER_MODIFIER_H
#define HKB_JIGGLER_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbJigglerModifierClass;

class hkbJigglerGroup;
class hkaPose;

/// This modifier jiggles the specified bone by simulating a mass and spring attached to the bone.
class hkbJigglerModifier : public hkbModifier
{
	public:
		// +version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbJigglerModifier();
		
	protected:

		hkbJigglerModifier( const hkbJigglerModifier& mod );

	public:

		~hkbJigglerModifier();

		//////////////////////////
		// hkbBindable interface
		//////////////////////////

			// hkbBindable interface implementation.
		virtual void collectBindables( hkbBindableCollector& collector ) HK_OVERRIDE;

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			// hkbModifier interface implementation.
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) HK_OVERRIDE;

			// Add a jiggler group.
		void addJigglerGroup( hkbJigglerGroup* jigglerGroup );

	public:

		//////////////
		// properties
		//////////////

			/// Whether to jiggle in world space or model space.
		enum JiggleCoordinates
		{
				/// Jiggle in world space
			JIGGLE_IN_WORLD_COORDINATES = 0,

				/// Jiggle in model space
			JIGGLE_IN_MODEL_COORDINATES = 1,
		};

			/// This is made public for HAT. You should call addJiggleGroup so that it gets
			/// reference counted. Each jiggler group represents a set of bones that share 
			/// jiggle parameters.
		hkArray<hkbJigglerGroup*> m_jigglerGroups;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_VARIABLE")
													//+hk.Description("The groups of bones for which the same jiggler settings will be applied.")

			/// Whether to jiggle in world space or model space. 
		hkEnum<JiggleCoordinates, hkInt8> m_jiggleCoordinates;	//+default(hkbJigglerModifier::JIGGLE_IN_WORLD_COORDINATES)
																//+hk.Description("Whether to jiggle in world space or model space.")

	private:

		HKB_BEGIN_INTERNAL_STATE(0);

			/// The current velocities in world coordinates (flattened for all groups)
		hkArray<hkVector4> m_currentVelocitiesWS; //+nosave

			/// The current positions in world coordinates or model coordinates based on m_jiggleCoordinates (flattened for all groups)
		hkArray<hkVector4> m_currentPositions; //+nosave

			/// Time slice information - used by the modify() method.
		hkReal m_timeStep;	//+nosave

			/// Indicates whether to compute world from model next modify.     
		hkBool m_initNextModify; //+nosave

		HKB_END_INTERNAL_STATE();

			/// For the very first frame we initialize the member variables.
		void initialize(	const hkaPose& pose,
							const hkbGeneratorOutput& inOut );

			/// Compute the new bone position using the mass spring system.
		void jiggleBone(	const hkVector4& desiredPositionWS, 
							const hkbJigglerGroup& jigglerGroup,
							hkVector4& currentVelocityWS,
							hkVector4& currentPositionWS );

			/// Give the bone positions preserve the bone length.
		void preserveBoneLength(	const hkReal nativeBoneLength,
									const hkVector4& positionOfParentWS,
									const hkbJigglerGroup& jigglerGroup,
									hkVector4& positionOfChildWS );

			/// Compute the new bone transform when we do not want to propagate
			/// the jiggle to the child bones but want to affect the siblings.
		void computeJiggleBoneTransform(	const hkQsTransform& worldFromModel,
											const hkbJigglerGroup& jigglerGroup,
											const hkQuaternion& rotationWS,
											const hkQsTransform* actualChildModelFromBone,
											const hkQsTransform* desiredChildModelFromBone,
											hkQsTransform& modelFromBone );

			/// Compute the new bone transform when we do not want to affect the siblings.
		void computeJiggleBoneTransformIgnoreSiblings(	const hkQsTransform& worldFromModel,
														const hkbJigglerGroup& jigglerGroup,
														const hkQuaternion& rotationWS,
														const bool hasChildBones,
														hkQsTransform& modelFromBone,
														bool& propagetOrNot );

			/// helper function used by modify() and computeJiggleBoneTransform().
		void computeJiggleBoneOrientation(	const hkQsTransform& worldFromModel,
											const hkQsTransform& worldFromBone,
											const hkQsTransform& actualChildModelFromBone,
											const hkQsTransform& desiredChildModelFromBone,
											hkQuaternion& newWorldFromBoneOrientation );

			/// Compute the rotation required to rotate a vector from a to b 
			/// to a vector from a to c
		void computeRotation(	const hkVector4& a,
								const hkVector4& b,
								const hkVector4& c,
								hkQuaternion& rotation );

			/// Given the rotation in world space, world from model and model from bone compute the
			/// rotated model from bone
		void rotateModelFromBone(	const hkQuaternion& rotationWS,
									const hkQsTransform& worldFromModel,
									hkQsTransform& modelFromBone );

	public:

		hkbJigglerModifier( hkFinishLoadedObjectFlag flag );

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
