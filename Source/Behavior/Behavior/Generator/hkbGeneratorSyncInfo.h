/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_SYNC_INFO_H
#define HKB_GENERATOR_SYNC_INFO_H


extern const class hkClass hkbGeneratorSyncInfoSyncPointClass;

extern const class hkClass hkbGeneratorSyncInfoClass;

class hkbNode;
class hkbBehaviorGraph;
class hkbContext;

/// Information about the current synchronization state of a generator.
class hkbGeneratorSyncInfo
{
	//+version(1)	
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorSyncInfo );
		HK_DECLARE_REFLECTION();

			/// Initialize the array of contact points
		hkbGeneratorSyncInfo();

			/// Copy constructor copies everything but then clears the m_isMirrored flag.
		hkbGeneratorSyncInfo( const hkbGeneratorSyncInfo& syncInfo );

		hkbGeneratorSyncInfo( hkFinishLoadedObjectFlag flag );

			/// Assignment copies everything but then clears the m_isMirrored flag.
		void operator = ( const hkbGeneratorSyncInfo& syncInfo );

	public:

		/// Set this info to indicate that the generator should not be synced at all.
		void setUnsyncable();

		/// Whether this node is 
		bool canSync() const;

		/// Whether this generator is additive
		bool isAdditive() const;

		/// Returns the duration of the generator.
		hkReal getDuration() const;

		/// The description of a point on the timeline that is used for synchronization.
		struct SyncPoint
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorSyncInfo::SyncPoint );
			HK_DECLARE_REFLECTION();

			// This must be trivial, as it is called from the finish constructor
			SyncPoint() {}

			// Actually init the SyncPoint
			void init() { m_id = INVALID_ID; m_time = 0.0f; }

			enum { INVALID_ID = -1, START_OF_GENERATOR_ID = -2 };

			/// The id of the sync point.
			int m_id;

			/// The point on the timeline where this sync point is.
			hkReal m_time;
		};
		
		/// The interval the generator sync info is currently synchronized to. 
		struct ActiveInterval
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorSyncInfo::ActiveInterval );
			HK_DECLARE_REFLECTION();

			/// Initializes the interval as invalid
			ActiveInterval();
			ActiveInterval( hkFinishLoadedObjectFlag flag ) {}

			/// Returns true if the interval is valid, false otherwise
			bool isValid() const {  return m_fraction != -1.0f; }

			/// Resets the active interval to be invalid
			void reset() { m_fraction = -1.0f; }

			/// Sync points in the active interval
			SyncPoint m_syncPoints[2];
			/// Fraction of the sync infos local time along the interval
			hkReal m_fraction;
		};
		
		/// Mirrors the sync information for this sync info
		void mirrorSyncInfo( const hkbContext& context );

		/// Computes the active interval to start and end on the sync points provided
		bool computeActiveIntervalFromCommonPoints( int* syncPointIds, int numSyncPointIds );

		/// Computes the active interval to start and end with the same ids as another sync info.  If a
		/// valid active interval was found true is returned, otherwise false.
		bool computeActiveIntervalFromOther( const hkbGeneratorSyncInfo& other, bool recomputeLocalTime );

		/// Computes the local time of the sync info based on the active interval of another sync info
		void computeLocalTimeFromOther( const hkbGeneratorSyncInfo& other );

		/// Computes the amount of time to the next sync point using the active interval.  This should not
		/// be called until after the active interval has been computed for a given frame
		hkReal computeLocalTimeFromActiveInterval() const;

	protected:
		/// Computes the active interval from the sync info's own sync points
		hkReal computeActiveIntervalFraction(hkReal localTime, hkReal endTime, hkReal startTime) const;

		/// Does the same as findNextSyncPointId but also returns the time of the sync point.
		void findNextSyncPoint( hkReal startTime, const int* syncPointIds, int numSyncPointIds, SyncPoint& syncPointOut ) const;
		
		/// Does the same as findPreviousSyncPointId but also returns the time of the sync point
		void findPreviousSyncPoint( hkReal startTime, const int* syncPointIds, int numSyncPointIds, SyncPoint& syncPointOut ) const;

	public:

		/// This is the maximum number of sync points allowed on each clip.
		static const int MAX_SYNC_POINTS = 16;
			
		/// The sync points for this generator.
		struct SyncPoint m_syncPoints[hkbGeneratorSyncInfo::MAX_SYNC_POINTS];

		/// The duration of the generator
		hkReal m_duration;

		/// The current local time of the generator, expressed independently of m_playbackSpeed.  The local time
		/// is always in the range 0 to 1.0f / m_baseFrequency, inclusive.  If m_baseFrequency is zero then the local
		/// time will also be zero.
		hkReal m_localTime;

		/// The effective frequency of the generator is m_baseFrequency * m_playbackSpeed.  Note that this is not the actual speed
		/// of the node, but the speed of the node would like to be in if it is not being synchronized to a different
		/// speed.  This can be negative for reverse playback, or 0 if the generator wants to be stopped.
		hkReal m_playbackSpeed;

		/// The number of sync points in m_syncPoints.
		hkInt8 m_numSyncPoints;

		/// Whether or not this generator is cyclic.
		hkBool m_isCyclic;

		/// Whether this sync info has been mirrored.
		hkBool m_isMirrored;

		/// Whether or not this generator is additive.  This is not used for synchronization.  It is only
		/// used to ensure we do not subtract an additive.
		hkBool m_isAdditive;		

		/// The active interval on the generator.
		ActiveInterval m_activeInterval;
};

/// A reference counted wrapper for hkbGeneratorSyncInfo.
class hkbReferencedGeneratorSyncInfo : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbReferencedGeneratorSyncInfo( const hkbGeneratorSyncInfo& syncInfo );

		hkbReferencedGeneratorSyncInfo( hkFinishLoadedObjectFlag flag );

		hkbGeneratorSyncInfo m_syncInfo;
};

#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.inl>

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
