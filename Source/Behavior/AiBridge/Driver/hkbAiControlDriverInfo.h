/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_CONTROL_DRIVER_INFO_H
#define HKB_AI_CONTROL_DRIVER_INFO_H

#include <Behavior/Behavior/Event/hkbEvent.h>

extern const hkClass hkbAiControlDriverInfoClass;

/// Information on character shape and capabilities which is used by the
/// hkbAiControlDriver to construct and control the HKAI-side representation 
/// of the character.
class hkbAiControlDriverInfo : public hkReferencedObject
{
public:
	//+vtable(true)
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbAiControlDriverInfo();

	hkbAiControlDriverInfo( hkFinishLoadedObjectFlag flag );

	virtual const hkClass* getClassType() const { return &hkbAiControlDriverInfoClass; }

	enum StateSource
	{
		CHARACTER_ROOT,
		CUSTOM_BONE,
		CHARACTER_CONTROLLER
	};

	hkReal m_radius;	//+default(0.4f)
						//+hk.Description("The radius of the character's AI collision volume")
						//+hk.RangeReal(absmin=0.001,softmin=0.1,softmax=10)

	hkEnum<StateSource, hkInt8> m_stateSource;		//+default(hkbAiControlDriverInfo::CHARACTER_ROOT)
													//+hk.Description("Source of positional and velocity data sent to AI.")

	hkInt16 m_customSourceBone;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_NONE")
								//+hk.Description("The bone to use for positional and velocity tracking if the source is \"Bone\".")

	hkBool m_loopBackVelocity;	//+default(false)
								//+hk.Description("If set to true, velocity data will not be reported to AI.")

	hkReal m_sensorHorizontalExtent;	//+default(5.0f)
										//+hk.Description("The extent of the character's local avoidance sensor in each horizontal direction.")
										//+hk.RangeReal(absmin=0.01,softmin=3,softmax=20)

	hkReal m_sensorHeight;	//+default(2.0f)
							//+hk.Description("The extent of the character's local avoidance sensor above the ground.")
							//+hk.RangeReal(absmin=0.001,softmin=1,softmax=3)

	hkReal m_wallFollowingAngle;	//+default(0.1f)
									//hk.Description("Maximum angle between wall following direction and sideways direction.")
									//+hk.RangeReal(absmin=0,absmax=1.57)

	hkReal m_velocityHysteresis;	//+default(0.01f)
									//hk.Description("Preference for continuing along the same velocity.")
									//+hk.RangeReal(absmin=0,absmax=1.57)

	hkReal m_sidednessChangingPenalty;	//+default(0.0f)
										//hk.Description("Unwillingness to change the pasing side.")
										//+hk.RangeReal(absmin=0,softmax=1)

	hkReal m_collisionPenalty;	//+default(1.0f)
								//hk.Description("A multiplier applied to the penalty for projected collisions.")
								//+hk.RangeReal(absmin=0.001,softmin=0.1,softmax=10)

	hkReal m_penetrationPenalty;	//+default(100.0f)
									//hk.Description("A multiplier applied to the penalty for ongoing penetrations.")
									//+hk.RangeReal(absmin=0.001,softmin=0.1,softmax=100)

	hkInt32 m_maxNeighbors;	//+default(16)
							//hk.Description("The maximum number of neighboring characters the character should avoid.")
							//+hk.RangeReal(absmin=1,softmax=16)

	hkReal m_leftTurnRadius; //+default(-1.0f)
							 //hk.Description("The character's desired turning radius when turning left.")
							 //+hk.RangeReal(absmin=-1,softmax=20)

	hkReal m_rightTurnRadius; //+default(-1.0f)
							  //hk.Description("The character's desired turning radius when turning right.")
							  //+hk.RangeReal(absmin=-1,softmax=20)

	hkReal m_goalDistTolerance; //+default(0.25f)
								//hk.Description("A character that is within this distance of the goal is considered to have reached the goal")
								//+hk.RangeReal(absmin=-1,softmax=20)

	hkReal m_userEdgeTolerance; //+default(0.25f)
								//hk.Description("A character that is within this distance of a user edge is considered to have entered the user edge")
								//+hk.RangeReal(absmin=0.001,softmax=2)
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
