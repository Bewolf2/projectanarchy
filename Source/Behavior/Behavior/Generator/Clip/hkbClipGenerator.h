/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CLIP_GENERATOR_H
#define HKB_CLIP_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Event/hkbEvent.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/Multithreaded/SampleAndCombine/hkaAnimationSampleAndCombineJobs.h>

extern const class hkClass hkbClipTriggerArrayClass;
extern const class hkClass hkbClipTriggerClass;
extern const class hkClass hkbClipGeneratorEchoClass;
extern const class hkClass hkbClipGeneratorClass;

class hkaMirroredSkeleton;
class hkbEventQueue;
class hkbBehaviorGraphData;
class hkaAnimationBinding;

/// An event that is attached to the local timeline of an hkbClipGenerator.  When the
/// clip passes the time of a trigger, the event associated with the trigger is sent.
class hkbClipTrigger
{
	public:
		// +version(1)

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbClipTrigger );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbClipTrigger();

			/// Construct a trigger given the local time and event.
			///
			/// To specify the local time of the trigger relative to the end of the clip,
			/// pass in true for relativeToEndOfClip.  Typically, localTime should be negative
			/// or zero in that case. To specify the trigger to be acyclic pass in true for
			/// acyclic.
		hkbClipTrigger( hkReal localTime, const hkbEventBase& event, bool relativeToEndOfClip, bool acyclic, bool isAnnotation );

			/// Copy constructor.
		hkbClipTrigger( const hkbClipTrigger& trigger );

			/// Finish constructor.
		hkbClipTrigger( hkFinishLoadedObjectFlag f );

		hkReal getTriggerTime( hkReal duration ) const;

			/// The local time at which the trigger is attached.
			///
			/// This time is relative to the remaining portion of the clip after cropping.
		hkReal m_localTime;

			/// The event to send when the time comes.
		class hkbEventProperty m_event;

			/// Whether m_localTime is relative to the end of the clip.
			///
			/// If false, m_localTime should be positive or zero.
			/// If true, m_localTime should be negative or zero.
		hkBool m_relativeToEndOfClip;

			/// Whether the trigger is a cyclic or acyclic.
			///
			/// m_acyclic and m_relativeToEndOfClip are mutually exclusive.
		hkBool m_acyclic; //+default(false)

			/// Whether or not this trigger was converted from an annotation attached to the animation.
		hkBool m_isAnnotation; //+default(false)
};

	/// An array of triggers wrapped for shared access.
class hkbClipTriggerArray : public hkReferencedObject
{
	//+vtable(1)
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbClipTriggerArray() {}
		hkbClipTriggerArray( const hkbClipTriggerArray& clipTriggerArray );
  	 
			/// Clears the trigger array by first removing references on payloads contained on hkbClipTriggers
			/// in the m_triggers array.  Should be called instead calling hkArray::clear.
		void clearTriggers();

			/// The triggers (events that occur at specific times).
		hkArray<class hkbClipTrigger> m_triggers;

	public:

		hkbClipTriggerArray( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_triggers(flag) {}
};

/// Generates an animation clip.
class hkbClipGenerator : public hkbGenerator
{
	public:
		// +version(4)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

			/// Construct with defaults.
		hkbClipGenerator();

			/// Construction given an animation control for playing the animation.
		hkbClipGenerator( hkaDefaultAnimationControl* animationControl );

	protected:

		hkbClipGenerator( const hkbClipGenerator& clipGen );

	public:

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
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void setLocalTime( const hkbContext& context, hkReal seconds ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
			// This is intended for internal use.  If you set the echoDuration to a positive
			// number, when the clip generator plays it will blend in a continuation of where it
			// left off last time it was played.  The duration of that blend is echoDuration.
		virtual void startEcho( hkReal echoDuration ) HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context ) HK_OVERRIDE;

	public:

		/// Internal storage for each echo.
		struct Echo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbClipGenerator::Echo );
			HK_DECLARE_REFLECTION();
			HK_DECLARE_POD_TYPE();

			Echo() {}
			Echo( hkFinishLoadedObjectFlag flag ) {}

			/// The offset between the echo and the current local time.
			HK_ALIGN16( hkReal m_offsetLocalTime );

			/// The current weight for blending in this echo.
			hkReal m_weight;

			/// How much to change the weight per unit time.
			hkReal m_dwdt;
		};

			/// Set the animation control for this clip and update reference count.
		void setAnimationControl( hkaDefaultAnimationControl* animationControl );

			/// Get the animation control for this clip.
		hkaDefaultAnimationControl* getAnimationControl() const;

			/// Returns the duration of the clip in local time.  DEPRECATED.
			///
			/// This is the intrinsic duration of the animation, factoring in cropping, but not
			/// the playback speed.
		hkReal getDurationLocalTime() const;

			/// Add a trigger to the clip.
		void addTrigger(	hkReal localTime,
							const hkbEventBase& event,
							bool relativeToEndOfClip=false,
							bool acyclic=false,
							bool isAnnotation=false );

			/// This returns true if the clip is running in pingpong mode backwards
		bool isPingPongBackwards() const;

			/// This returns the echo list
		const hkArray<struct Echo>& getEchos() const;

			/// Returns the motion extracted since last generate().
		const hkQsTransform& getExtractedMotion() const;

			/// For internal use.  Returns the local time offset by the given amount, taking into consideration whether
			/// the animation is looped or not.  The playback speed is not taken into consideration.  If the animation is
			/// single-play, the offset local time is capped at the end of the clip.
		hkReal offsetLocalTime( hkReal localTimeOffset ) const;

			/// For internal use. Is the clip using a mapper for retargeting. If this function is called when
			/// a clip is inactive then it would always return false.
		bool hasMapper() const;

			/// Returns the mapper data used for skeleton mapping, or HK_NULL if it does not exist.			
		hkaDefaultAnimationControlMapperData* getMapperData() const;

			/// Remove all of the triggers that have the m_isAnnotation flag set.
			/// This is used if you are changing the animation
			/// bindings of your clip generators at runtime and want to re-convert annotations to
			/// triggers.  In such a situation, we need to remove the old ones.
		void removeAnnotationTriggers();

			/// Only for internal use.
		void computeAnimationData( const hkbContext& context, int numPoseLocal );

			/// Only for internal use.  Used to set the local time of the animation control associated with the clip.  Used
			/// exclusively for sampling a specific point in a clip generator from the Havok Animation Tool.
		void setLocalLocalTime( hkReal localTime );
			/// Only for internal use. Used to get the local time of the animation control associated with the clip.  Used
			/// exclusively for sampling a specific point in a clip generator from the Havok Animation Tool.
		hkReal getLocalLocalTime() const;


			/// Whether the clip has been activated
		hkBool isActive() const;

			/// Sets the animation binding index
		void setAnimationBindingIndex( hkInt16 bindingIndex );

			/// Returns the animation binding index
		hkInt16 getAnimationBindingIndex() const;

		///////////////
		// properties
		///////////////

			/// Playback modes determine how time advances for the clip.
		enum PlaybackMode
		{
				/// Play the clip once from start to finish.
			MODE_SINGLE_PLAY = 0,

				/// Play the clip over and over in a loop.
			MODE_LOOPING,

				/// Don't advance the clip.  Let the user control the local time.
				///
				/// In this mode the direction of movement of the clip cursor is from old local time
				/// to the new local time. All the triggers within this interval are processed.
			MODE_USER_CONTROLLED,

				/// At the end of the animation, turn around backward, and then turn around again at the beginning, etc.
			MODE_PING_PONG,

				/// How many modes there are.
			MODE_COUNT
		};

		enum ClipFlags
		{
			//+defineflags(true)

			/// In SINGLE_PLAY mode, when the clip gets to the end, it will continue the motion if this is true.
			///
			/// If you keep playing a clip beyond the end, it will return the last pose.  This is sometimes useful
			/// as you go into a blend.  If this property is false, the motion is just zero after the end of the clip,
			/// which means that the animation comes to a halt.  If this property is true, the motion returned 
			/// after the clip reaches the end will be the motion present at the end of the clip, so the animation
			/// will keep going in the direction it was going.
			FLAG_CONTINUE_MOTION_AT_END = 0x1,

			/// In PING_PONG mode, if this is true, synchronization will be done on half a ping-pong cycle
			/// instead of the full cycle (back and forth).
			/// 
			/// Normally in ping-pong mode, the frequency of the clip generator is
			/// reported to be half the frequency of the underlying clip.
			/// So for the purpose of synchronization, one cycle of this clip will
			/// include playing the clip forward and then backward again to the start.
			/// If you instead want it to synchronize to half of the ping-pong cycle
			/// (the animation playing through once, not twice), set this to true.
			FLAG_SYNC_HALF_CYCLE_IN_PING_PONG_MODE = 0x2,

			/// If this flag is set the pose is mirrored about a plane.
			FLAG_MIRROR = 0x4,

			/// If this flag is set and if the output pose would be a dense pose
			FLAG_FORCE_DENSE_POSE = 0x8,

			/// If this flag is set then we do not convert annotation to triggers.
			FLAG_DONT_CONVERT_ANNOTATIONS_TO_TRIGGERS = 0x10,

			/// If this flag is set the motion in the animation will not be extracted.
			FLAG_IGNORE_MOTION = 0x20,
		};

			/// The name of the animation bundle this clip references (if any).
		hkStringPtr m_animationBundleName;

			/// The name of the animation to play.
		hkStringPtr m_animationName;

			/// Triggers (events that occur at specific times).
		hkRefPtr<hkbClipTriggerArray> m_triggers;

			/// (optional) The user specified partition mask (these need to be a subset of the partitions in the binding)
		hkUint32 m_userPartitionMask;

			/// The number of seconds (in clip time) to crop the beginning of the clip.
		hkReal m_cropStartAmountLocalTime;
		
			/// The number of seconds (in clip time) to crop the end of the clip.	
		hkReal m_cropEndAmountLocalTime;

			/// The time at which to start the animation in local time.
		hkReal m_startTime;	//+hkb.RoleAttribute("ROLE_TIME","FLAG_NONE")

			/// Playback speed (negative for backward).
		hkReal m_playbackSpeed;

			/// If m_enforcedDuration is greater than zero, the clip will be scaled to have the enforced duration.
			/// 
			/// Note that m_playbackSpeed can still be used.  The duration will only match the enforced duration
			/// when m_playbackSpeed is 1.  The effective duration will be m_enforcedDuration / m_playbackSpeed.
		hkReal m_enforcedDuration; //+default(0.0)

			/// In user controlled mode, this fraction (between 0 and 1) dictates the time of the animation.
			/// 
			/// This value interpolates along the clip within the part that remains after cropping.
		hkReal m_userControlledTimeFraction;

	private:
			/// An index into the character's hkbAnimationBindingSet.
		hkInt16 m_animationBindingIndex;	//+default(-1)
											//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

	public:
			/// The playback mode.
		hkEnum<PlaybackMode,hkInt8> m_mode;

			/// Flags for specialized behavior.
		hkInt8 m_flags; //+default(0)

			/// The maximum number of simultaneous echoes.
		static hkInt8 m_maxEchos;

	private:

		void processCyclicTriggers( hkReal beginLocalTime, hkReal endLocalTime, int loops, hkbEventQueue& eventQueue, const hkbContext& context );
		void processAcyclicTriggers( hkReal beginTime, hkReal endTime, hkbEventQueue& eventQueue, const hkbContext& context );

		void processCyclicTriggersNoLoop( hkReal beginLocalTime, hkReal endLocalTime, bool endInclusive, hkbEventQueue& eventQueue, const hkbContext& context );

			// Compute our begin/end times
			// Most of the time localTimes == animLocalTimes except in the case where we are stepping over our final frame of animation.
			// In this case, they are different because localTimes are used for triggers and animLocalTimes for computing motion.
		void computeBeginAndEndLocalTime(
			const hkReal timestep, 
			hkReal& beginLocalTime, 
			hkReal& beginAnimLocalTime,
			hkReal& endLocalAndAnimTime,
			int& loops, 
			bool& endOfClip ) const;

			// Compute the animLocalTimes for the very first or very last frame of animation.
		void computeFinalFrameTimes(
			const hkReal timestep,
			const hkBool32 lastFrame,
			hkReal& beginAnimLocalTime,
			hkReal& endAnimLocalTime ) const;

			// Compute the motion based on animation times.
		void computeMotion(
			const hkaMirroredSkeleton* mirroredSkeleton,
			const hkReal characterScale,
			const hkReal beginAnimLocalTime,
			const hkReal endAnimLocalTime,
			const int loops,
			hkQsTransform& motion ) const;
		
		hkReal getEndOfClipLocalTime() const ;

			// compute the speed, including the effects of m_enforcedDuration and m_playbackSpeed
		hkReal computeSpeed() const;

			// compute start time, including the effects of m_enforcedDuration
		hkReal computeStartTime() const;

			// sort the triggers from left to right
		void sortTriggers();

			// called by activate()
		void activateInternal();

			// called by deactivate()
		void deactivateInternal();

			// see if m_atEnd needs to be changed from true to false
		void checkAtEnd();

			// used when budgeting SPU memory
		void getNumTracksNumFloatTracksAndAnimation( const hkbContext& context, int numPoseLocal, int& numTracks, int& numFloatTracks, hkaAnimation*& animation );

			// adds triggers from the bindings set to m_triggers
		void addTriggersFromBindingSet( const hkbContext& context, class hkbAnimationBindingWithTriggers* bwt );

	private:

		hkArray<struct hkaAnimationSampleOnlyJob::AnimationData> m_animDatas; //+nosave

			// the animation control
		hkRefPtr<hkaDefaultAnimationControl> m_animationControl; //+nosave

			// When copying annotations to triggers we make a new triggers array 
			// but we throw it away on deactivate() and restore the original.
			// We do this only to support the case where shallow cloning is not being
			// used; we don't want the memory footprint to grow and grow.
		hkRefPtr<hkbClipTriggerArray> m_originalTriggers;	//+nosave

			// the retargeting skeleton mapper data
		hkaDefaultAnimationControlMapperData* m_mapperData; //+nosave

			// the animation binding. This is stored for easy access for SPUs
		hkaAnimationBinding* m_binding; //+nosave
								
			// The number of tracks in the animation.
		hkInt32 m_numAnimationTracks; //+nosave

		HKB_BEGIN_INTERNAL_STATE(0);

			// the motion extracted in the last update
		hkQsTransform m_extractedMotion; //+nosave

			// the list of active echos
		hkArray<struct hkbClipGenerator::Echo> m_echos; //+nosave

			// The local time of the clip (excactly as set in the animation control).
		hkReal m_localTime; //+nosave

			// the time lapsed since activate, taking into consideration the playback speed
		hkReal m_time; //+nosave

			// In user controlled mode, this fraction (between 0 and 1) is the time of the animation in the previous update.
		hkReal m_previousUserControlledTimeFraction; //+nosave

			// The buffer size to use when decompressing animations (use getMaxSizeOfCombinedDataChunks()).
		hkInt32 m_bufferSize; //+nosave

			// this tells us whether we have reached the end of the clip in MODE_SINGLE_PLAY
		hkBool m_atEnd; //+nosave

			// the start time to use next time time activate() is called
		hkBool m_ignoreStartTime; //+nosave

			// whether ping-pong mode is currently going backward
		hkBool m_pingPongBackward; //+nosave

		HKB_END_INTERNAL_STATE();

	public:

		hkbClipGenerator( hkFinishLoadedObjectFlag flag );

		friend class hkbCpuBehaviorJob;
		friend class hkbCpuUtils;
		friend class hkbSpuBehaviorUtils;
		friend class hkbClipGeneratorUtils;
};

#include <Behavior/Behavior/Generator/Clip/hkbClipGenerator.inl>

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
