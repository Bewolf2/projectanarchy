/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_HAND_IK_DRIVER_INFO_H
#define HKB_HAND_IK_DRIVER_INFO_H

#include <Behavior/Behavior/Utils/hkbBlendCurveUtils.h>

class hkaSkeleton;

extern const class hkClass hkHandIkDriverInfoHandClass;
extern const class hkClass hkHandIkDriverInfoClass;

class hkbHandIkDriverInfo : public hkReferencedObject
{
	public:

		//+vtable(true)
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbHandIkDriverInfo()
		:	m_fadeInOutCurve(hkbBlendCurveUtils::BLEND_CURVE_SMOOTH)
		{}

		struct Hand
		{
			//+version(1)

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Hand );
			HK_DECLARE_REFLECTION();

				/// Construct with initial values.
			Hand();

				/// The axis of rotation for each elbow, in the coordinate frame of the elbow/forearm bone.
			hkVector4 m_elbowAxisLS;	//+default(0,1,0)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
										//+hk.Description("The axis of the elbow in the coordinate frame of the elbow/forearm.")

				/// The vector that is normal to the back of the hand, in the coordinate frame of the hand.
			hkVector4 m_backHandNormalLS;	//+default(0,1,0)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NORMALIZED")
											//+hk.Description("The vector that is normal to the back of the hand, in the coordinate frame of the hand.")

			hkVector4 m_handOffsetLS;	//+default(0,0,0)
										// +hk.Description("The offset of the hand in the local space of the hand bone")

			hkQuaternion m_handOrienationOffsetLS;	//+default(0,0,0,1)
													//+hk.Description("The rotation offset of the hand in the local space of the hand bone")

				/// Limit the elbow angle (to avoid elbow popping artifacts). Default is 180 degrees.
			hkReal m_maxElbowAngleDegrees;	//+default(180.0f)
											//+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The maximum elbow angle allowed.")

				/// Limit the elbow angle (to avoid elbow artifacts). Default is 0 degrees.
			hkReal m_minElbowAngleDegrees;	//+default(0.0f)+hk.RangeReal(absmin=0,absmax=180)
											//+hk.Description("The minimum elbow angle allowed.")

				/// Index of the shoulder bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_shoulderIndex;	//+default(-1)
										//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
										//+hk.Description("The shoulder bone.")

				/// Index of the sibling of the shoulder bone. If the bone is specified then the hand IK modifier applies the same rotation to the sibling bone as it does to the shoulder bone.
			hkInt16 m_shoulderSiblingIndex; //+default(-1)
											//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
											//+hk.Description("The sibling of the shoulder bone. If the bone is specified then the hand IK modifier applies the same rotation to the sibling bone as it does to the shoulder bone.")

				/// Index of the elbow bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_elbowIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The elbow bone.")

				/// Index of the sibling of the elbow bone. If the bone is specified then the hand IK modifier applies the same rotation to the sibling bone as it does to the elbow bone.
			hkInt16 m_elbowSiblingIndex;	//+default(-1)
											//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
											//+hk.Description("The sibling of the elbow bone. If the bone is specified then the hand IK modifier applies the same rotation to the sibling bone as it does to the elbow bone.")

				/// Index of the wrist bone.  If this is not -1, it overrides the character boneInfo.
			hkInt16 m_wristIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
									//+hk.Description("The wrist bone.")

				/// Set to true if the position of the end effector is to be solved for
			hkBool m_enforceEndPosition;	//+default(true)
											//+hk.Description("If true, the IK solver will place the hand at the target location")

				/// Set to true if the rotation of the end effector is to be solved for
			hkBool m_enforceEndRotation;	//+default(false)
											//+hk.Description("If true, the IK solver will orient the hand to the target rotation")

			hkStringPtr m_localFrameName;	//+hk.Description("Optional local frame which describes the contact location and rotation of the end effector.")

			Hand( hkFinishLoadedObjectFlag flag ) : m_localFrameName(flag) {}
		};

			/// A Hand structure for each hand.
		hkArray< struct Hand > m_hands; //hk.Description("The parameters describing all of the hands.")

			/// The current on-fraction is passed through this blend curve to produce the current gain that is used in the IK.
		hkEnum< hkbBlendCurveUtils::BlendCurve, hkInt8 > m_fadeInOutCurve;	//+default(hkbBlendCurveUtils::BLEND_CURVE_SMOOTH)
																			//+hk.Description("This blend curve is applied when fading in and out the IK.")

	public:

		bool isValid( const hkaSkeleton* skeleton, hkStringPtr& errorString ) const;

	public:

		hkbHandIkDriverInfo( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_hands( flag ) {}

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
