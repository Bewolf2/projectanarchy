/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_MODIFIER_H
#define HKB_FOOT_IK_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/FootIk/hkbFootIkGains.h>
#include <Behavior/Behavior/Attributes/hkbAttributes.h>

extern const class hkClass hkbFootIkModifierInternalLegDataClass;

extern const class hkClass hkbFootIkModifierLegClass;

extern const class hkClass hkbFootIkModifierClass;

class hkaSkeleton;
class hkaFootPlacementIkSolver;
class hkaPose;

	/// Deprecated.  Please use the hkbFootIkControlsModifier and the hkbFootIkDriver.
	/// Performs foot IK on the incoming pose.
class hkbFootIkModifier : public hkbModifier
{
	public:
		// +version(3)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbFootIkModifier();

	protected:

		hkbFootIkModifier( const hkbFootIkModifier& mod );

	public:

		~hkbFootIkModifier();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbModifier interface
		///////////////////////////

			// hkbModifier interface implementation.
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	public:

		///////////////
		// properties
		///////////////

			/// foot Ik gains.
		struct hkbFootIkGains m_gains; //+hk.Description("The gains that can be used to smooth out the foot IK, avoiding abrupt changes from one frame to the next.")

		/// The structure holds the information required by the footPlacementIkSolver
		struct Leg
		{
			// +version(2)
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Leg );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
				/// 
				/// We use the same initial values as in hkaFootPlacementIkSolver::Setup.
			Leg()
				:	m_originalAnkleTransformMS( hkQsTransform::getIdentity() ),
					m_kneeAxisLS(1,0,0),
					m_footEndLS(hkVector4::getZero()),
					m_footPlantedAnkleHeightMS(0.0f),
					m_footRaisedAnkleHeightMS(0.5f),
					m_maxAnkleHeightMS(0.7f),
					m_minAnkleHeightMS(-0.1f),
					m_maxKneeAngleDegrees(180.0f),
					m_minKneeAngleDegrees(0),
					m_verticalError(0.0f),
					m_hipIndex(-1),
					m_kneeIndex(-1),
					m_ankleIndex(-1),
					m_hitSomething(false),
					m_isPlantedMS(false),
					m_isOriginalAnkleTransformMSSet(false)
			{}

				/// The original position and orientation of the ankle (the one we base the foot placement on).  Default is identity.
			hkQsTransform m_originalAnkleTransformMS;	//+default(0,0,0,0,0,0,0,1,1,1,1,1)
														//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

				/// Axis of rotation of the knee, in local space of the knee.  Default is (1,0,0).
			hkVector4 m_kneeAxisLS; //+default(1,0,0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("The axis of rotation of the knee, in local space of the knee.")

				/// The end of the foot, in the local space of the ankle/foot. If this is (0,0,0), the default, the
				/// length of the foot won't be considered (only one raycast will be used).
			hkVector4 m_footEndLS;	//+default(0,0,0)
									//+hk.Description("The end of the foot, in the local space of the ankle/foot. If this is zero the length of the foot won't be considered, so only one raycast will be used.")

				/// This event is sent if the foot raycast does not find a hit when the foot is supposed to be planted.
			class hkbEventProperty m_ungroundedEvent;	//+hk.Description("This event is sent if any of the foot raycasts do not find a hit.")

				/// The height of the ankle below which the foot is considered planted. Used to calculate gains.  Default is 0.
			hkReal m_footPlantedAnkleHeightMS;	//+default(0)
												//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
												//+hk.Description("The height of the ankle below which the foot is considered planted. Used to calculate gains.")

				/// The height of the ankle above which the foot is considered fully raised. Used to calculate gains.  Default is 0.
			hkReal m_footRaisedAnkleHeightMS;	//+default(0.5f)
												//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
												//+hk.Description("The height of the ankle above which the foot is considered fully raised. Used to calculate gains.")

			/*
			** Foot Placement limits (used to clamp the IK results)
			*/

				/// Maximum height the ankle can reach (in model space).  Default is 0.7.
			hkReal m_maxAnkleHeightMS;	//+default(0.7f)
										//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
										//+hk.Description("The maximum height the ankle can reach, in model space, as a result of foot IK.")

				/// Minimum height the ankle can reach (in model space).  Default is -0.1.
			hkReal m_minAnkleHeightMS;	//+default(-0.1f)
										//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
										//+hk.Description("The minimum height the ankle can reach, in model space, as a result of foot IK.")

				/// Limit the knee angle (to avoid knee popping artifacts). Default is 180 degrees. 
			hkReal m_maxKneeAngleDegrees;	//+default(180.0f)
											//+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The maximum knee angle that is allowed as a result of foot IK.")

				/// Limit the hinge angle (to avoid knee artifacts). Default is 0 degrees. 
			hkReal m_minKneeAngleDegrees;	//+default(0)
											//+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The minimum knee angle that is allowed as a result of foot IK.")

				/// The distance between the input foot height and the ground.
			hkReal m_verticalError; //+default(0)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("The distance between the input foot height and the ground.")

				/// Index of the hip bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_hipIndex; //+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")+hk.Description("The hip bone.")

				/// Index of the knee bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_kneeIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The knee bone.")

				/// Index of the ankle bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_ankleIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The ankle bone.")
			
				/// Whether or not the ground was hit by the raycast.
			hkBool m_hitSomething;	//+default(false)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not the ground was hit by the raycast.")

				/// Whether or not the foot is planted in model space in the incoming animation. If the height 
				/// of the ankle goes below m_footPlantedAnkleHeightMS the foot is considered planted. 
			hkBool m_isPlantedMS;	//+default(false)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not the foot is planted in model space in the incoming animation. If the height of the ankle goes below m_footPlantedAnkleHeightMS the foot is considered planted.")

				/// Indicates whether there is useful data in m_originalAnkleTransformMS.  Default is false.
			hkBool m_isOriginalAnkleTransformMSSet; //+default(false)
													//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

			Leg( hkFinishLoadedObjectFlag flag) : m_ungroundedEvent(flag) {}
		};

			/// The properties of each leg of the character.
		hkArray< struct Leg > m_legs; //+hk.Description("The properties of each leg of the character.")

			/// When ray casting from the foot towards the ground, the (positive) distance, 
			/// from the foot and in the up direction, where the ray starts. Default is 0.5(m), you may
			/// want to change this according to the scale of your character
		hkReal m_raycastDistanceUp; //+default(0.5f)
									//+hk.RangeReal(absmin=0,absmax=100,softmin=0,softmax=1)
									//+hk.Description("When ray casting from the foot towards the ground, the distance above the foot where the ray starts.")

			/// When ray casting from the foot towards the ground, the (positive) distance, 
			/// from the foot and in the down direction, where the ray ends. Default is 0.8(m), you may
			/// want to change this according to the scale of your character
		hkReal m_raycastDistanceDown;	//+default(0.8f)
										//+hk.RangeReal(absmin=0,absmax=100,softmin=0,softmax=1)
										//+hk.Description("When ray casting from the foot towards the ground, the distance below the foot where the ray ends.")

			/// The height of the ground where the model was created/animated, in model space.  Default is 0.
		hkReal m_originalGroundHeightMS;	//+default(0)
											//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
											//+hk.Description("The height of the ground where the model was created/animated, in model space.")

			/// This is the weighted average of the min and the max distance between the actual feet position 
			/// relative to the ground and the desired feet position relative to the ground based on 
			/// m_gains.m_errorUpDownBias. The modifier computes this information and stores it in this property.
			/// Based on m_gains.m_worldFromModelFeedbackGain property it then decides whether to apply it or not.
		hkReal m_errorOut;	//+default(0)
							//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
							//+hk.Description("This is the weighted average of the min and the max distance between the actual feet position relative to the ground and the desired feet position relative to the ground based on m_gains.m_errorUpDownBias. The modifier computes this information and stores it in this property. Based on m_gains.m_worldFromModelFeedbackGain property it then decides whether to apply it or not.")


			/// The extra vertical distance the character is offset during IK.  Useful for crouching the character.
		hkReal m_verticalOffset;	//+default(0)
									//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
									//+hk.Description("The extra vertical distance the character is offset during IK.  Useful for crouching the character.")

		/// The collision filter info to use when raycasting the ground.
		hkUint32 m_collisionFilterInfo; //+default(3)
										//+hk.Description("The collision filter info to use when raycasting the ground.  Normally, you should set this to a layer that collides with your ground terrain.")

			/// How much to align the character to the ground normal in the forward direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.
		hkReal m_forwardAlignFraction;	//+default(1.0f)
										//+hk.Description("How much to align the character to the ground normal in the forward direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.")

			/// How much to align the character to the ground normal in the sideways direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.
		hkReal m_sidewaysAlignFraction;	//+default(0.0f)
										//+hk.Description("How much to align the character to the ground normal in the sideways direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.")
	
			/// How far from the character position in the sideways direction to cast the rays to compute the ground slope in the sideways direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.
		hkReal m_sidewaysSampleWidth;	//+default(0.2f)
										//+hk.Description("How far from the world-from-model in the sideways direction to cast the rays to compute the ground slope in the sideways direction. This works only when the m_alignMode is set to ALIGN_MODE_FORWARD.")

			/// If the user sets this property to false then it means the user wants to use the track system for the footIk
			/// modifier. If the property is set to true then the user does not want to use the track system at all.
		hkBool m_useTrackData;	//+default(false)
								//+hk.Description("Set this to true if you are using the Foot IK Controls Modifier to turn on foot IK.  Set this to false if you just want foot IK to be on all the time.")

			/// Whether to use foot locking
		hkBool m_lockFeetWhenPlanted;	//+default(false)
										//+hk.Description("Whether to lock the feet on the ground.")

			/// Whether to use character up-vector instead of world up-vector. 
			/// You can think of this as being the direction of gravity used by the foot IK.
			/// This is useful if you want the character to be in any orientation, such as if it is walking around on the walls 
			/// and ceiling, or on the surface of a convex object.  When this is set to true you will
			/// usually want m_hipOrientationGain to be zero and m_alignWorldFromModelGain
			/// to be greater than zero.  That way the character will be aligned with the surface normal so
			/// the IK will cast rays in the direction of the surface normal.
		hkBool m_useCharacterUpVector;  //+default(false)
										//+hk.Description("Whether to use the character up-vector as the up-vector instead of world up vector. If this is set to true the rays are cast in the direction opposite to model up vector instead of in the direction of gravity. This is useful if you want the character to be in any orientation. When this is set to true you would probably want the m_hipOrientationGain to be zero and  m_alignWorldFromModelGain to be greater than zero.")

		enum AlignMode
		{
				/// Align in the both forward and right direction.
			ALIGN_MODE_FORWARD_RIGHT = 0,

				/// Align in the forward direction only. 
			ALIGN_MODE_FORWARD,				
		};

			/// How to align the world-from-model when hkbFootIkGains::m_alignWorldFromModelGain is non-zero.
		hkEnum<AlignMode, hkInt8> m_alignMode;	//+default(hkbFootIkModifier::ALIGN_MODE_FORWARD_RIGHT)
												//+hk.Description("How to align the world-from-model when hkbFootIkGains::m_alignWorldFromModelGain is non-zero.")

			/// Internal and output data for each leg.
			///
			/// This is public because it is reflected, which it is so that 
			/// you can bind variables to it.
		struct InternalLegData
		{
			// +version(1)
			HK_DECLARE_REFLECTION();

				/// The position of the ground below the foot, as computed by the foot IK raycasts.
				/// These are stored because we need to delay one frame before adjusting the world-from-model.
			hkVector4 m_groundPosition; //+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")

				/// The foot IK solver used for this leg.
			hkaFootPlacementIkSolver* m_footIkSolver; //+nosave+overridetype(void*)

				/// Construct with defaults.
			InternalLegData()
			:	m_groundPosition( hkVector4::getZero() ),
				m_footIkSolver(HK_NULL) {}

			protected:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbFootIkModifier::InternalLegData );
		};

	private:

		void setup( const hkbContext& context, const hkaSkeleton& skeleton );
		void adjustHipOrientation(	const hkbContext& context, 
									hkbGeneratorOutput& inOut, 
									hkaPose& pose, 
									const hkVector4& upVector,
									hkReal hipOrientationGain );
		void alignWorldFromModelUpWithGroundNormal( const hkbContext& context, 
													const hkVector4& raycastUp, 
													hkbGeneratorOutput& inOut,
													hkReal alignWorlFromModelGain );
		void computeGroundNormal( const hkbContext& context, const hkQsTransform& worldFromModel, const hkVector4& raycastUp, hkVector4& groundNormal );

		// Compute the plane normal from a set of points.
		bool computePlaneNormal(	const hkVector4* const points, 
									const int numPoints, 
									const hkVector4& up, 
									hkVector4& normal );

		// Compute the plane forward from a set of points which lie on the plane perpendicular to the plane forward.
		bool computePlaneForward(	const hkVector4* const points, 
									const int numPoints,
									const hkVector4& forward, 
									const hkVector4& up,
									hkVector4& planeForward );

			// The size of this array should be same as the m_legs array size.
			// We reflect this so that it can be accessed by variables.
		hkArray< struct InternalLegData > m_internalLegData; //+serialized(false)

			// Data to store the previous footIk track data. 
		hkReal m_prevIsFootIkEnabled; //+nosave

			// whether or not foot IK has been initialized
		hkBool m_isSetUp; //+nosave

			// whether or not the ground position for each leg has been computed
		hkBool m_isGroundPositionValid; //+nosave

	public:

		hkbFootIkModifier( hkFinishLoadedObjectFlag flag );
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
