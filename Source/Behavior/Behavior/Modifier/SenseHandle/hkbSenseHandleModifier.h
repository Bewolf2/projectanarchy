/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_SENSE_HANDLE_MODIFIER_H
#define HKB_SENSE_HANDLE_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Utils/hkbHandle.h>

extern const class hkClass hkbSenseHandleModifierRangeClass;

extern const class hkClass hkbSenseHandleModifierClass;

class hkbClosestLocalFrameCollector;
class hkbHandle;

/// A modifier for finding a handle to a character or rigid body in the world.
class hkbSenseHandleModifier : public hkbModifier
{
	public:
		// +version(2)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbSenseHandleModifier();

	protected:

		hkbSenseHandleModifier( const hkbSenseHandleModifier& mod );

	public:

		~hkbSenseHandleModifier();

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

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

	private:

			// compute the sensor position in world space
		void computeSensorPosWS( const hkbContext& context, hkbGeneratorOutput& inOut, hkVector4& sensorPosWS );

			// compute the max distance of all of the ranges
		void computeMinAndMaxDistance( hkReal& minDistance, hkReal& maxDistance ) const;

			// sense for rigid bodies and optionally local frames on them
		void senseInRigidBodies(	const hkVector4& sensorPosWS,
									hkReal minDistance,
									hkReal maxDistance,
									const hkbContext& context );

			// sense for local frames on the animation skeleton, or just for characters
		void senseInSkeletons(	const hkVector4& sensorPosWS,
								hkReal minDistance,
								hkReal maxDistance,
								const hkbContext& context );

			// sense for local frames in the given handle group
		void senseInGroup(	const hkVector4& sensorPosWS,
							hkReal minDistance,
							hkReal maxDistance,
							const hkbContext& context );

			// sense for local frames on a given rigid body 
		void senseInRigidBody(	hkbPhysicsInterface* physicsInterface,
								const hkVector4& sensorPosWS,
								hkReal maxDistance,
								hkbRigidBodyHandle rb,
								hkbClosestLocalFrameCollector& collector );

			// sense for local frames on a given character skeleton
		void senseInSkeleton(	const hkVector4& sensorPosWS,
								hkReal maxDistance,
								hkbCharacter& c,
								hkbClosestLocalFrameCollector& collector );

			// set the handle output properties when the handle has been sensed.
		void setOutputProperties();

	private:

			// The handle that was sensed.
		hkbHandle m_handle; //+nosave

	public:

		///////////////
		// properties
		///////////////

			/// How to choose a handle.
		enum SensingMode
		{
				/// Consider all nearby rigid bodies.
			SENSE_IN_NEARBY_RIGID_BODIES,

				/// Consider all nearby rigid bodies that do not belong to this character (may include non-character rigid bodies).
			SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER,

				/// Consider all nearby rigid bodies associated with a character other than this one.
			SENSE_IN_OTHER_CHARACTER_RIGID_BODIES,

				/// Consider only the rigid bodies of the current character.
			SENSE_IN_THIS_CHARACTER_RIGID_BODIES,

				/// Consider only the rigid bodies of the character in m_handleIn.
			SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES,

				/// Consider only the rigid body in m_handleIn.
			SENSE_IN_GIVEN_RIGID_BODY,

				/// Consider the skeletons of other characters.
			SENSE_IN_OTHER_CHARACTER_SKELETON, // must be first among the SKELETON modes!

				/// Consider the skeleton of this character.
			SENSE_IN_THIS_CHARACTER_SKELETON,

				/// Consider the skeleton of the character in m_handleIn.
			SENSE_IN_GIVEN_CHARACTER_SKELETON,

				/// Consider only the handles in the group of m_handleIn.
			SENSE_IN_GIVEN_LOCAL_FRAME_GROUP,
		};

			/// The offset of the sensor location in the space of the sensor handle, bone or character.
		hkVector4 m_sensorLocalOffset;	//+default(0,0,0)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NONE")
										//+hk.Description("The offset of the sensor location in the space of the sensor bone or character.")

			/// A range of distances in which to look for handles.
		struct Range
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, Range );
			HK_DECLARE_REFLECTION();

			Range()
			:	m_minDistance(0.0f),
				m_maxDistance(1.0f),
				m_ignoreHandle(false)
			{
			}

			Range(hkFinishLoadedObjectFlag f) : m_event(f)
			{
			}

				/// The event to send if the handle is found in this range.
			class hkbEventProperty m_event;	//+hk.Description("The event to send if the handle is found in this range.")

				/// The minimum distance between the sensor position and the sensed handle.
			hkReal m_minDistance;	//+default(0.0f)
									//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)
									//+hk.Description("The minimum distance between the sensor position and the sensed handle.")

				/// The maximum distance between the sensor position and the sensed handle.
			hkReal m_maxDistance;	//+default(1.0f)
									//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)
									//+hk.Description("The maximum distance between the sensor position and the sensed handle.")

				/// Whether or not to output the handle found.
			hkBool m_ignoreHandle;	//+hk.Description("Whether or not to output the handle found.")
		};

			/// The ranges of distances in which to look for handles.
		hkArray<struct Range> m_ranges;	//+hk.Description("The ranges of distances in which to look for handles.")

			/// The handle that has been sensed.  This will be null if no handle has been sensed.
		hkRefPtr<hkbHandle> m_handleOut;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT|FLAG_NOT_CHARACTER_PROPERTY")
											//+hk.Description("The handle that has been sensed. This will be null if no handle has been sensed.")

			/// A handle relative to which we want to find another handle (when the mode is one of the SENSE_IN_GIVEN modes).
		hkRefPtr<hkbHandle> m_handleIn;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_NOT_CHARACTER_PROPERTY")
										//+hk.Description("In sensing modes beginning with SENSE_IN_GIVEN, a handle will be sensed relative to this handle.")

			/// The name of the local frame to be sensed, if any.
		hkStringPtr m_localFrameName;	//+hk.Description("The name of the local frame to be sensed, if any.")

			/// The name of the local frame to sense from.
		hkStringPtr m_sensorLocalFrameName;	//+hk.Description("The name of the handle to sense from, if any.")

			/// The minimum distance between the sensor position and the sensed handle.
		hkReal m_minDistance;	//+default(0.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)
								//+hk.Description("The minimum distance between the sensor position and the sensed handle.")

			/// The maximum distance between the sensor position and the sensed handle.
		hkReal m_maxDistance;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0,absmax=1000.0,softmax=10.0)
								//+hk.Description("The maximum distance between the sensing bone and the handle.")

			/// The distance between the sensor and the sensed handle.  This is only computed once when the handle is first sensed.
			/// If m_keepFirstSensedHandle is true, the distance will not be recomputed each frame.
		hkReal m_distanceOut;	//+default(0.0f)
								//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
								//+hk.Description("The distance between the sensor and the sensed handle. This is only computed once when the handle is first sensed. If m_keepFirstSensedHandle is true, the distance will not be recomputed each frame.")

			/// The collision filter info used for sensing the rigid bodies.
		hkUint32 m_collisionFilterInfo;	//+default(0)
										//+hk.Description("The collision filter info used for sensing the rigid bodies.")

			/// The index of the ragdoll bone whose coordinates will be used to specify the sensor location.
		hkInt16 m_sensorRagdollBoneIndex;	//+default(-1)
											//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
											//+hk.Description("The ragdoll bone to be used for sensing.  You cannot specify both a ragdoll bone and an animation bone.")

			/// The index of the animation bone whose coordinates will be used to specify the sensor location.
		hkInt16 m_sensorAnimationBoneIndex;	//+default(-1)
											//+hkb.RoleAttribute("ROLE_BONE_INDEX")
											//+hk.Description("The animation bone to be used for sensing.  You cannot specify both a ragdoll bone and an animation bone.")

			/// Where to look for a handle.
		hkEnum<SensingMode, hkInt8> m_sensingMode;	//+default(hkbSenseHandleModifier::SENSE_IN_NEARBY_RIGID_BODIES)
													//+hk.Description("Where to look for a handle.")

			/// Whether to extrapolate the sensing bone position using the rigid body velocity. This only works when using a ragdoll sensing bone.
		hkBool m_extrapolateSensorPosition;	//+default(false)
											//+hk.Description("Whether to extrapolate the sensing bone position using the rigid body velocity. This only works when using a ragdoll sensing bone.")

			/// If this is true, once one handle is found it is kept and no more handles are sensed.  The handle 
			/// is re-evaluated each frame in case it is moving.
		hkBool m_keepFirstSensedHandle;	//+default(false)
										//+hk.Description("If this is true, once one handle is found it is kept and no more handles are sensed.  The handle is re-evaluated each frame in case it is moving.")

			/// Whether or not a handle was sensed.
		hkBool m_foundHandleOut;	//+default(false)
									//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_OUTPUT")
									//+hk.Description("Whether or not a handle was sensed.")

	private:

			// time elapsed since modify was called
		hkReal m_timeSinceLastModify; //+nosave

			// this event will be sent on the next update
		hkInt32 m_rangeIndexForEventToSendNextUpdate; //+nosave

	public:

		hkbSenseHandleModifier( hkFinishLoadedObjectFlag flag );
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
