/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_JOB_H
#define HKB_BEHAVIOR_JOB_H

#include <Common/Base/Thread/JobQueue/hkJobQueue.h>
#include <Animation/Animation/Playback/Multithreaded/hkaAnimationJobs.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>
#include <Behavior/Behavior/Generator/hkbGeneratorOutput.h>
#include <Behavior/Behavior/Variables/hkbVariableBindingSetUtils.h>
#include <Behavior/Behavior/Driver/FootIk/hkbFootIkDriver.h>

class hkbModifier;
class hkbVariableValueSet;
class hkbVariableValue;
class hkaMirroredSkeleton;

	/// The base class for all behavior jobs used when multithreading.
class hkbBehaviorJob : public hkJob
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBehaviorJob );

			/// each job type is enumerated.
		enum JobSubType
		{
			BEHAVIOR_JOB_GENERATE_BEHAVIOR_GRAPH,
			BEHAVIOR_JOB_GENERATE_NODES,
			BEHAVIOR_JOB_UPDATE_CHARACTER,

			// just a marker marking the end of the behavior job id range
			BEHAVIOR_JOB_END		
		};

	protected:

		hkbBehaviorJob( JobSubType type, hkUint16 size, hkJobSpuType jobSpuType );

	public:

			/// The data needed during generate.  This data is not used much on the SPU,
			/// but the pointer is passed along from job to job so that the PPU jobs
			/// can use it.  This points to main memory.
		hkbBehaviorGraph::GenerateWorkingData* m_behaviorGenerateData;
};

	/// A job for generating a behavior graph (analogous to hkbBehaviorGraph::generate()).
class hkbGenerateBehaviorGraphJob : public hkbBehaviorJob
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGenerateBehaviorGraphJob );

		hkbGenerateBehaviorGraphJob();
};

struct hkbModifierInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbModifierInfo );

		/// The modifier itself.
	HK_ALIGN16( hkbModifier* m_modifier );

		/// The type of this modifier.
	hkEnum<hkbNodeType, int> m_modifierType;	
};

	/// The information about each node that needs to be sent when doing an hkbGenerateNodesJob.
struct hkbGenerateNodesJobNodeSetup
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGenerateNodesJobNodeSetup );


		/// Read-write flags.
	enum
	{
		READ_FROM_CPU = 1,
		WRITE_TO_CPU = 2,
	};

		/// The node.
	HK_ALIGN16( hkbNode* m_node );

		/// The data that pertains to this particular node (node dependent).
	void* m_data;

		/// The track buffer on the CPU.
	hkbGeneratorOutput::Tracks* m_tracks;

		/// The size of the track buffer .
	int m_trackSizeBytes;

		/// The size of m_data.
	int m_dataSizeBytes;

		/// The index of the first child in the flattened list in the hkbGenerateNodesJob.
	int m_firstChildIndex;

		/// The number of children of this node.
	int m_numChildren;

		/// The type of the node.
	hkEnum<hkbNodeType, int> m_nodeType;

		/// The index of the output to use on the SPU.
	int m_spuOutputIndex;

		/// The index of the output slot for this node to use on the CPU.  
		/// A value of -1 means to use the final output buffer.
	int m_cpuOutputIndex;

		/// Whether to read or write the track data from/to the CPU.
	int m_trackReadWriteFlags;

		/// Index that specifies which variable and attribute setup this node is associated with
	hkInt8 m_variableAndAttributeSetupIndex;
};

	/// A job to generate a set of nodes in a behavior graph.
class hkbGenerateNodesJob : public hkbBehaviorJob
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGenerateNodesJob );

		hkbGenerateNodesJob();

	public:

		struct CharacterInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGenerateNodesJob::CharacterInfo );

				/// The world-from-model previous to this generate().
			hkQsTransform m_initialWorldFromModel;

				/// The model up vector in model space (usually (0,1,0) or (0,0,1)).
			hkVector4 m_modelUpMS;

				/// The model forward vector in model space.
			hkVector4 m_modelForwardMS;

				/// The model right vector in model space.
			hkVector4 m_modelRightMS;

				/// World up of the character.
			hkVector4 m_worldUpWS;

			hkaSkeleton* m_animationSkeleton;

				/// Mirror skeleton of the character
			hkaMirroredSkeleton* m_mirroredSkeleton;

				/// FootIk internal leg data
			hkbFootIkDriver::InternalLegData* m_internalLegData;

				/// The pose of the character in the previous frame
			hkQsTransform* m_lastCharacterPose;

				/// The number of internal leg data
			int m_numInternalLegData;

				/// The character is scaled by this amount.
			hkReal m_scale;
		};

		struct VariableAndAttributeSetup
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbGenerateNodesJob::VariableAndAttributeSetup );

			/// Information about syncing variables on the SPU
			hkbSyncVariableSetup m_syncVariableSetup;

			/// A map between the attribute IDs in the local behavior graph to those in the root behavior graph.
			const hkInt32* m_attributeIdMap;

			/// The number of elements in m_attributeIdMap.
			hkInt32 m_numAttributeIds;
		};

		enum
		{
			MAX_VARIABLE_AND_ATTRIBUTE_SETUPS = 8
		};

			/// Character information 
		CharacterInfo* m_characterInfo;

			/// Information about each node.
		hkbGenerateNodesJobNodeSetup* m_nodeSetups;

			/// A template describing the tracks layout.
		hkbGeneratorOutput::Tracks* m_tracksTemplate;

		/// Information about variables and attributes on the SPU
		// Pointer to array of objects
		VariableAndAttributeSetup* m_variableAndAttributeSetups;

			/// Intermediate output track buffer.
		hkVector4* m_intermediateOutputTracksBufferCpu;

			/// Each intermediate output can operate on a different bone set. 
		hkInt16* m_intermediateOutputSizesSpu;

			/// Each intermediate output can have a different buffer size. 
		hkUint32* m_intermediateOutputTracksBufferOffsetCpu;

			/// The child indices (flattened) of all of the nodes.
		int* m_childrenIndices;

			/// The attributes from the behavior.
		hkReal* m_attributes;
			
			/// The total size of the intermediate output track buffer on Spu.
		hkUint32 m_intermediateOutputTotalTracksBufferSizeSpu;

			/// The number of intermediate outputs that have been allocated on the CPU.
		hkUint32 m_numIntermediateOutputsCpu;

			/// The size of the tracks template.
		hkUint16 m_tracksTemplateSizeBytes;

			/// The size of the list of child indices.
		hkInt16 m_childrenIndicesSizeBytes;

			//The number of variable and attribute setups used
		hkInt16 m_numVariableAndAttributeSetups;

			/// The number of behavior attributes.
		hkUint16 m_numAttributes;

			/// The number of nodes in this batch.
		hkUint8 m_numNodes;

			/// The number of intermediate outputs to allocate on the SPU.
		hkUint8 m_numIntermediateOutputsSpu;

			/// The node setup index of the root node. If the root node output is different
			/// from the data.m_output the pose must be copied properly.
		hkInt8 m_nodeSetupIndexOfRootNode;
};

	/// A job to update the behavior graph of a character (first of two stages).
class hkbUpdateCharacterJob : public hkbBehaviorJob
{
	public:

		hkbUpdateCharacterJob( hkbCharacter& character, hkbContext& context, hkReal deltaTime );

			/// The character that is being updated.
		hkbCharacter& m_character;

			/// The context for updating the character.
		hkbContext& m_context;

			/// The timestep.
		hkReal m_deltaTime;
};

#include <Behavior/Behavior/Multithreaded/hkbBehaviorJob.inl>

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
