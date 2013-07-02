/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_POSE_MATCHING_GENERATOR_H
#define HKB_POSE_MATCHING_GENERATOR_H

#include <Behavior/Behavior/Generator/Blender/hkbBlenderGenerator.h>

extern const class hkClass hkbPoseMatchingGeneratorClass;

	/// Matches the ragdoll to the first pose of each of the child generators.
	/// 
	/// This generator has two modes.  In MODE_MATCH, the current ragdoll pose is matched with
	/// the first pose of each of the child generators.  The best matching pose is
	/// produced as output (gradually).  When a new child is chosen to have the best
	/// matching pose, that child is blended in over time and the other children
	/// are blended out.  In MODE_PLAY, the best matching child is played.
class hkbPoseMatchingGenerator : public hkbBlenderGenerator
{
	//+version(2)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbPoseMatchingGenerator();

	protected:

		hkbPoseMatchingGenerator( const hkbPoseMatchingGenerator& gen );

	public:

		~hkbPoseMatchingGenerator();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void handleEvent( const hkbContext& context, const hkbEvent e ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;
		
			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getActiveEvents( hkPointerMap<int, bool>& activeEvents ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

	public:

			/// Returns the current matching error.
		hkReal getError() { return m_error; }

	public:

		///////////////
		// properties
		///////////////

			/// The rotation of the frame of reference used for pose matching.
		hkQuaternion m_worldFromModelRotation;	//+default(0,0,0,1)
												//+hk.Description("The rotation of the frame of reference used for pose matching.")

			/// The modifier is either performing matching or playing out the matched animation.
		enum Mode
		{
				/// Perform pose matching.  Generate the currently matched pose.
			MODE_MATCH,

				/// Play back the current animation beginning at the matched pose.
			MODE_PLAY
		};

			/// Speed at which to blend in and out the blend weights (in weight units per second).
		hkReal m_blendSpeed;	//+default(1.0f)
								//+hk.RangeReal(absmin=0.0001,absmax=1000.0,softmax=10.0)
								//+hk.Description("The speed at which to blend in and out the blend weights, in weight units per second, when the best match changes.")

			/// Don't switch if the pelvis speed is below this.
		hkReal m_minSpeedToSwitch;	//+default(0.2f)
									//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
									//+hk.Description("The pelvis speed must be above this to switch poses.")

			/// The minimum time between switching matches when the matching error is 0.
			/// 
			/// In order to avoid too much oscillating back and forth between matches, we 
			/// introduce some hysteresis.  We only allow a switch to a new match
			/// after a minimum amount of time has passed without a better match being found.  
			/// The time bound is a linear function of the error.
		hkReal m_minSwitchTimeNoError;	//+default(0.2f)
										//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)+hk.Description("The minimum time between switching matches when the matching error is 0. In order to avoid too much oscillating back and forth between matches, we introduce some hysteresis.  We only allow a switch to a new match after a minimum amount of time has passed without a better match being found. The time bound is a linear function of the error.")

			/// The minimum time between switching matches when the matching error is 1.
		hkReal m_minSwitchTimeFullError;	//+default(0)
											//+hk.RangeReal(absmin=0.0,absmax=100.0,softmax=10.0)
											//+hk.Description("The minimum time between switching matches when the matching error is 1.")

			/// An event to set m_mode to MODE_PLAY.
		hkInt32 m_startPlayingEventId;	//+default(hkbEvent::EVENT_ID_NULL)
										//+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")
										//+hk.Description("When this event is received, the mode is changed from matching to playing.")

			/// An event to set m_mode to MODE_MATCH.
		hkInt32 m_startMatchingEventId;	//+default(hkbEvent::EVENT_ID_NULL)
										//+hkb.RoleAttribute("ROLE_EVENT_ID","FLAG_NONE")
										//+hk.Description("When this event is received, the mode is changed from playing to matching.")

			/// The root (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_rootBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("The root bone used for pose matching.  Pose matching uses three bone from the ragdoll skeleton which are assumed to be representative of the pose.  The L-shaped skeleton between these three bones is matched to determine similar poses.")

			/// A second (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_otherBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A second bone used for pose matching.")

			/// A third (ragdoll) bone used for pose matching.  If this is -1, the index is taken from the character's hkbBoneInfo.
		hkInt16 m_anotherBoneIndex;	//+default(-1)
									//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
									//+hk.Description("A third bone used for pose matching.")

			/// The (ragdoll) pelvis bone used to measure the speed of the ragdoll.
		hkInt16 m_pelvisIndex;	//+default(-1)
								//+hkb.RoleAttribute("ROLE_BONE_INDEX","FLAG_RAGDOLL")
								//+hk.Description("The pelvis bone that is used to measure the speed of the character.")

			/// Whether matching poses or playing out the remaining animation.
		hkEnum< Mode, hkInt8 > m_mode;	//+default(hkbPoseMatchingGenerator::MODE_MATCH)
										//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

	private:

			// update the best matching pose
		void updateInternal( const hkbContext& context, hkReal timestep );

		HKB_BEGIN_INTERNAL_STATE(0);

			// The index of the child that is the current match to the ragdoll pose.
		int m_currentMatch; //+nosave

			// The child that matches best to the ragdoll.
			// This is different than m_currentMatch due to hysteresis.
		int m_bestMatch; //+nosave

			// The time since m_bestMatch changed.
		hkReal m_timeSinceBetterMatch; //+nosave

			// The current error (between 0 and 1).
		hkReal m_error; //+nosave

			// When the mode changes from playing to matching we need to reset 
			// the local time of the playing clip to zero.
		hkBool m_resetCurrentMatchLocalTime; //+nosave

		HKB_END_INTERNAL_STATE();

			// A pose matching utility which does the work.
		class hkaPoseMatchingUtility* m_poseMatchingUtility; //+nosave

	public:

		hkbPoseMatchingGenerator( hkFinishLoadedObjectFlag flag ) : hkbBlenderGenerator(flag) {}
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
