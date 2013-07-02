/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_FOOT_IK_CONTROLLER_INFO_H
#define HKB_FOOT_IK_CONTROLLER_INFO_H

class hkaSkeleton;

extern const class hkClass hkbFootIkDriverInfoLegClass;
extern const class hkClass hkbFootIkDriverInfoClass;

class hkbFootIkDriverInfo : public hkReferencedObject
{
	public:
		
		//+vtable(true)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbFootIkDriverInfo()
			:	m_raycastDistanceUp(0.5f),
				m_raycastDistanceDown(0.8f),
				m_originalGroundHeightMS(0.0f),
				m_verticalOffset(0.0f),
				m_collisionFilterInfo(3),
				m_forwardAlignFraction(1.0f),
				m_sidewaysAlignFraction(0.0f),
				m_sidewaysSampleWidth(0.2f),
				m_lockFeetWhenPlanted(false),
				m_useCharacterUpVector(false),
				m_isQuadrupedNarrow(false)
		{}

			/// The structure holds the information required by the footPlacementIkSolver
		struct Leg
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Leg );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
				/// 
				/// We use the same initial values as in hkaFootPlacementIkSolver::Setup.
			Leg();

				/// Axis of rotation of the knee, in local space of the knee.  Default is (0,0,1).
			hkVector4 m_kneeAxisLS; //+default(0,0,1)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
									//+hk.Description("The axis of rotation of the knee, in local space of the knee.")

				/// The end of the foot, in the local space of the ankle/foot. If this is (0,0,0), the default, the
				/// length of the foot won't be considered (only one raycast will be used).
			hkVector4 m_footEndLS;	//+default(0,0,0)
									//+hk.Description("The end of the foot, in the local space of the ankle/foot. If this is zero the length of the foot won't be considered, so only one raycast will be used.")

				/// The height of the ankle below which the foot is considered planted. Used to calculate gains.  Default is 0.
			hkReal m_footPlantedAnkleHeightMS;	//+default(0)
												//+hk.RangeReal(absmin=-100,absmax=100,softmin=-1,softmax=1)
												//+hk.Description("The height of the ankle below which the foot is considered planted. Used to calculate gains.")

				/// The height of the ankle above which the foot is considered fully raised. Used to calculate gains.  Default is 0.
			hkReal m_footRaisedAnkleHeightMS;	//+default(0.5f)
												//+hk.RangeReal(absmin=-100,absmax=100,softmin=-1,softmax=1)
												//+hk.Description("The height of the ankle above which the foot is considered fully raised. Used to calculate gains.")

			/*
			** Foot Placement limits (used to clamp the IK results)
			*/

				/// Maximum height the ankle can reach (in model space).  Default is 0.7.
			hkReal m_maxAnkleHeightMS;	//+default(0.7f)
										//+hk.RangeReal(absmin=-100,absmax=100,softmin=-1,softmax=1)
										//+hk.Description("The maximum height the ankle can reach, in model space, as a result of foot IK.")

				/// Minimum height the ankle can reach (in model space).  Default is -0.1.
			hkReal m_minAnkleHeightMS;	//+default(-0.1f)
										//+hk.RangeReal(absmin=-100,absmax=100,softmin=-1,softmax=1)
										//+hk.Description("The minimum height the ankle can reach, in model space, as a result of foot IK.")

				/// Limit the knee angle (to avoid knee popping artifacts). Default is 180 degrees. 
			hkReal m_maxKneeAngleDegrees;	//+default(180.0f)
											//+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The maximum knee angle that is allowed as a result of foot IK.")

				/// Limit the hinge angle (to avoid knee artifacts). Default is 0 degrees. 
			hkReal m_minKneeAngleDegrees;	//+default(0)
											//+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The minimum knee angle that is allowed as a result of foot IK.")

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
			
			Leg( hkFinishLoadedObjectFlag flag) {}
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

			/// The extra vertical distance the character is offset during IK.  Useful for crouching the character.
		hkReal m_verticalOffset;	//+default(0)
									//+hk.RangeReal(absmin=-10,absmax=10,softmin=-1,softmax=1)
									//+hk.Description("The extra vertical distance the character is offset during IK.  Useful for crouching the character.")

			/// The collision filter info to use when raycasting the ground.
		hkUint32 m_collisionFilterInfo; //+default(3)
										//+hk.Description("The collision filter info to use when raycasting the ground.  Normally, you should set this to a layer that collides with your ground terrain.")

			/// How much to align the character to the ground normal in the forward direction.
		hkReal m_forwardAlignFraction;	//+default(1.0f)
										//+hk.Description("How much to align the character to the ground normal in the forward direction.")

			/// How much to align the character to the ground normal in the sideways direction.
		hkReal m_sidewaysAlignFraction;	//+default(1.0f)
										//+hk.Description("How much to align the character to the ground normal in the sideways direction.")

			/// How far from the character position in the sideways direction to cast the rays to compute the ground slope in the sideways direction. This applies only when the m_isQuadrupedNarrow is set to true.
		hkReal m_sidewaysSampleWidth;	//+default(0.0f)
										//+hk.Description("How far from the world-from-model in the sideways direction to cast the rays to compute the ground slope in the sideways direction. This applies only when the m_isQuadrupedNarrow is set to true.")

			/// Whether to use foot locking.
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

			/// This works for any character with more the two legs. If the character is such that in the sideways direction the feet are really close then set this to true. 
		hkBool m_isQuadrupedNarrow;	//+default(false)
									//+hk.Description("This works for any character with more the two legs. If the character is such that in the sideways direction the feet are really close then set this to true.")

	public:

		bool isValid( const hkaSkeleton* skeleton, hkStringPtr& errorString ) const;

	public:

		hkbFootIkDriverInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_legs(flag) {}

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
