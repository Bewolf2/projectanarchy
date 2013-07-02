/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_OUTPUT_H
#define HKB_GENERATOR_OUTPUT_H


#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>

/// Stores the output produced by an hkbGenerator.
class hkbGeneratorOutput
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput );

			/// The standard tracks used by behavior graphs.
		enum StandardTracks
		{
			TRACK_WORLD_FROM_MODEL,
			TRACK_EXTRACTED_MOTION,
			TRACK_POSE,
			TRACK_FLOAT_SLOTS,
			TRACK_RIGID_BODY_RAGDOLL_CONTROLS,
			TRACK_RIGID_BODY_RAGDOLL_BLEND_TIME,
			TRACK_POWERED_RAGDOLL_CONTROLS,
			TRACK_POWERED_RAGDOLL_WORLD_FROM_MODEL_MODE,
			TRACK_KEYFRAMED_RAGDOLL_BONES,
			TRACK_KEYFRAME_TARGETS,
			TRACK_ANIMATION_BLEND_FRACTION,
			TRACK_ATTRIBUTES,
			TRACK_FOOT_IK_CONTROLS,
			TRACK_CHARACTER_CONTROLLER_CONTROLS,
			TRACK_HAND_IK_CONTROLS_0,
			TRACK_HAND_IK_CONTROLS_1,
			TRACK_HAND_IK_CONTROLS_2,
			TRACK_HAND_IK_CONTROLS_3,
			TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_0,
			TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_1,
			TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_2,
			TRACK_HAND_IK_CONTROLS_NON_BLENDABLE_3,
			TRACK_DOCKING_CONTROLS,
			TRACK_AI_CONTROL_CONTROLS_BLENDABLE,
			TRACK_AI_CONTROL_CONTROLS_NON_BLENDABLE,
			NUM_STANDARD_TRACKS,
		};

		enum
		{
			// This needs to be set to the number of tracks in StandardTracks above
			// that have the prefix TRACK_HAND_IK_CONTROLS.
			NUM_HAND_IK_CONTROLS_TRACKS = 4,
		};

			/// The different types of data in the tracks.
		enum TrackTypes
		{
				/// The track should be blended as hkReals.
			TRACK_TYPE_REAL,

				/// The track should be blended as hkQsTransforms.
			TRACK_TYPE_QSTRANSFORM,

				/// The track should not be blended.
			TRACK_TYPE_BINARY,
		};

		enum TrackFlags
		{
			TRACK_FLAG_ADDITIVE_POSE = 1,
			TRACK_FLAG_PALETTE = 1<<1,
			TRACK_FLAG_SPARSE = 1<<2,
		};

			/// The header of a track describes the track.
		struct TrackHeader
		{
			//+version(1)

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::TrackHeader );

				/// The capacity of this track (in number of elements, not bytes).
			hkInt16 m_capacity;

				/// The number of data elements (not bytes) currently in the track.
			hkInt16 m_numData;

				/// The offset of the data for this track from the start of the track buffer.
			hkInt16 m_dataOffset;

				/// The size of each element in the track.
			hkInt16 m_elementSizeBytes;

				/// How much track data has been blended in.  If this is 0, it means the data is invalid.
			hkReal m_onFraction;

				/// The flags for this track.
			hkFlags<TrackFlags,hkInt8> m_flags;

				/// The type of data in the track.
			hkEnum<TrackTypes,hkInt8> m_type;
		};

			/// The master header describes the track buffer as a whole.
		struct TrackMasterHeader
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::TrackMasterHeader );

				/// The total number of bytes allocated to the track.
			hkInt32 m_numBytes;

				/// The number of tracks.
			hkInt32 m_numTracks;

				/// Unused memory to make this struct 16 bytes.
			hkInt8 m_unused[8];
		};

			/// This type is used to access the master header and individual track headers.  We never allocate
			/// one of these directly.  Instead we allocate a block of memory and then cast it to a Tracks.
			/// The array of a single TrackHeader represents a larger array depending on the number of tracks.
			/// You can use m_trackheaders to access all of them, not just one.  Beyond the last TrackHeader
			/// the buffer contains the actual track data.
		struct Tracks
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::Tracks );

				/// The master header contains information about the tracks.
			struct TrackMasterHeader m_masterHeader;

				/// An array of headers for each track (usually there is actually more than 1 track).
			struct TrackHeader m_trackHeaders[1];
		};

			/// An accessor class used for const access to a track (both its header and data).
		class ConstTrack
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::ConstTrack );

					/// Construction given the header and data.
				ConstTrack( TrackHeader* header, hkReal* data );

					/// Get the track header.
				const TrackHeader* getTrackHeader() const;

					/// Get the track data as hkReals.
				const hkReal* getDataReal() const;

					/// Get the track data as hkQsTransforms.
				const hkQsTransform* getDataQsTransform() const;

					/// Return the palette/sparseness indices associated with this track (they are after the data).
				const hkInt8* getIndices() const;

					/// Is the track data valid (m_onFraction > 0)?
				bool isValid() const;

					/// Returns the number of elements of data in the track.
				int getNumData() const;

					/// Returns the capacity of the track.
				int getCapacity() const;

					/// Returns the number of bytes in an element.
				int getElementSizeBytes() const;

					/// Returns the type of data in the track.
				TrackTypes getType() const;

					/// Returns the m_onFraction for the track.
				hkReal getOnFraction() const;

					/// Whether or not this track is a palette (TRACK_FLAG_PALETTE).
				bool isPalette() const;

					/// Whether or not this track is a sparse (TRACK_FLAG_SPARSE).
				bool isSparse() const;

			protected:

				TrackHeader* m_header;
				hkReal* m_data;
		};

			/// An accessor class used for non-const access to a track (both its header and data).
		class Track : public hkbGeneratorOutput::ConstTrack
		{
			public:

				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGeneratorOutput::Track );

				Track( TrackHeader* header, hkReal* data );

					/// Writable access to the track data as hkReals.
				hkReal* accessDataReal();

					/// Writable access to the track as hkQsTransforms.
				hkQsTransform* accessDataQsTransform();

					/// Return the palette/sparseness indices associated with this track (they are after the data).
				hkInt8* accessIndices() const;

					/// Set the track data as hkReals.
				void setDataReal( const hkReal* data, int count );

					/// Set the track data as hkQsTransforms.
				void setDataQsTransform( const hkQsTransform* data, int count );

					/// Set one element of the track data as an hkReal.
				void setDataReal( int index, const hkReal& data );

					/// Set one element of the track data as an hkQsTransform.
				void setDataQsTransform( int index, const hkQsTransform& data );

					/// Set the track to be valid (m_onFraction = 1.0).
				void setValid();

					/// Set the m_onFraction for the track.
				void setOnFraction( hkReal onFraction );

					/// Sets the number of elements in the track.
				void setNumData( hkInt16 numData );
		};

	public:

			/// Construct with a single track for the world-from-model transform.
		hkbGeneratorOutput();

			/// Construct with internal memory allocation and standard track layout.
		hkbGeneratorOutput(	int numBones,
							int numRagdollBones,
							int numAttributes,
							int numHands,
							int numTracks = NUM_STANDARD_TRACKS,
							int numFloatSlots = 0 );

			/// Construct given a pose buffer, which will not be deleted on destruction.
			/// Use this if you want to allocate your own track memory instead of having
			/// it allocated on the heap.
		hkbGeneratorOutput( Tracks* tracks );

			/// Construct given the track buffer size.  Track memory will be allocated internally.
			/// The track buffer will be totally uninitialized, so be careful.  This constructor
			/// is mainly intended to be used internally.
		hkbGeneratorOutput( int trackBufferSizeBytes );

		~hkbGeneratorOutput();

			/// Set all of the tracks to be invalid (no data).
		void clearTracks();

			/// Set a track to contain valid data.
		void setValid( int trackId );

			/// Whether or not a track contains valid data.
		bool isValid( int trackId ) const;

			/// Whether the track exists
		bool trackExists( int trackId ) const;

			/// Const access to a track.
		ConstTrack getTrack( int trackId ) const;

			/// Non-const access to a track.
		Track accessTrack( int trackId );

			/// Non-const access to a track header.
		TrackHeader& accessTrackHeader( int trackId );

			/// Non-const access to all of the track data.
		Tracks* accessTracks();

			/// Const access to all of the track data.
		const Tracks* getTracks() const;

			/// Const access to track data as hkReals.
		const hkReal* getTrackDataReal( int trackId ) const;

			/// Const access to track data as hkQsTransforms.
		const hkQsTransform* getTrackDataQsTransform( int trackId ) const;

			/// Const access to the world-from-model (track).
		const hkQsTransform& getWorldFromModel() const;

			/// Const access to the extracted motion (track).
		const hkQsTransform& getExtractedMotion() const;

			/// Const access to the pose track.
		const hkQsTransform* getPoseLocal() const;

			/// Const access to the bone weights.
		const hkReal* getBoneWeights() const;

			/// Const access to partitionInfo. This is a pointer to the generator's partitionInfo
		const hkbGeneratorPartitionInfo& getPartitionInfo() const;

			/// Const access to the float track
		const hkReal* getFloats() const;

			/// Const access to the attributes track
		const hkReal* getAttributes() const;

			/// Returns the number of tracks.
		int getNumTracks() const;

			/// Returns the size of the pose track.
		int getNumPoseLocal() const;

			/// Returns the size of the bone weights.
		int getNumBoneWeights() const;

			/// Returns the size of the float track.
		int getNumFloats() const;

			/// Returns the size of the attribute track.
		int getNumAttributes() const;

			/// Non-const access to track data as hkReals.
		hkReal* accessTrackDataReal( int trackId );

			/// Non-const access to track data as hkQsTransforms.
		hkQsTransform* accessTrackDataQsTransform( int trackId );

			/// Non-const access to the world-from-model (track).
		hkQsTransform& accessWorldFromModel();

			/// Non-const access to the extracted motion (track).
		hkQsTransform& accessExtractedMotion();

			/// non-const access to the pose track.
		hkQsTransform* accessPoseLocal();

			/// non-const access to the bone weights track.
		hkReal* accessBoneWeights();

			//Sets the partition info
		hkbGeneratorPartitionInfo& accessPartitionInfo();

			/// Set the pose to be additive.
		void setPoseAdditive(); 

			/// Returns whether or not the pose is additive.
		bool isPoseAdditive() const;

			/// Set the pose to be dense.
		void setPoseLocalDense();

			/// Clear pose data
		void clearPoseData();

	private:

			// the track data (including headers)
		struct Tracks* m_tracks; //+nosave

			// whether or not to delete the track data
		bool m_deleteTracks;
};

#include <Behavior/Behavior/Generator/hkbGeneratorOutput.inl>

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
