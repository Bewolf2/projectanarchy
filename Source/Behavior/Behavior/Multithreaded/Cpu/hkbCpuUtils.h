/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_CPU_UTILS_H
#define HKB_CPU_UTILS_H

#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>
#include <Behavior/Behavior/Multithreaded/hkbBehaviorJob.h>

class hkJobQueue;
class hkbNode;
class hkbModifier;

	/// The information about a node pertaining to the partitioning of the active behavior graph to be
	/// divided into jobs to be run on multiple cores.
struct hkbNodePartitionInfo
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbNodePartitionInfo );

		/// The index of the output slot for this node to use on the CPU.  
		/// A value of -1 means to use the final output buffer.
	int m_cpuOutputIndex;

		/// The number of nodes in this partition (only valid for first node in partition).
	int m_numNodesInPartition;

		/// The number of nodes in this partition (only valid for first node in partition).
	int m_numGeneratorsInPartition;

		/// Whether or not this node is to be run on the SPU.
	hkBool m_runOnSpu;

	/// Whether this node requires an output (otherwise it is recycling).
	hkBool m_requiresOwnOutput;
};

	/// The information about a node pertaining to the partitioning of the active behavior graph to be
	/// divided into jobs to be run on multiple cores. Unlike hkbNodePartitionInfo this information
	/// is temporary.
struct hkbNodePartitionInfoInternal
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbNodePartitionInfoInternal );

		/// The index of the partition to which this node belongs.
	int m_partitionIndex;

		/// The index of this node within the partition.
	int m_nodeIndexInPartition;

		/// The index of the output slot for this node to use on the SPU.
	int m_spuOutputIndex;

		/// The number of setups required for this partition (only valid for the first node in partition).
	int m_numNodeSetups;

		/// What type of generator this is.
	hkEnum<hkbNodeType, int> m_generatorType;

		/// Whether or not the output of this node needs to be available on the CPU.
	hkBool m_outputOnCpu;
};

class hkbCpuUtils
{
	public:

			/// Returns the number of intermediate outputs needed on the CPU.
		static int HK_CALL computeSpuNodePartitionInfo(	hkbBehaviorGraph& rootBehavior, 
														hkbNodePartitionInfo* partitionInfoArray,
														hkbNodePartitionInfoInternal* partitionInfoInternalArray,
														int maxPartitionBytes,
														hkbBehaviorGraph::GenerateWorkingData& data,
														hkInt16* intermediateGeneratorOutputSizes );

			/// Whether or not the generator can run on the SPU.
		static bool HK_CALL canGenerateOnSpu( hkbGenerator* gen, hkbNodeType gt, hkbBehaviorGraph& rootBehavior );

			/// Whether or not the modifier can run on the SPU.
		static bool HK_CALL canModifyOnSpu( hkbModifier* mod, hkbBehaviorGraph& rootBehavior );

			/// Compute the number of bytes needed on the SPU for a generator.
		static int HK_CALL computeSpuBytes( hkbGenerator* gen, hkbNodeType gt, hkbBehaviorGraph::GenerateWorkingData& data );

			/// Compute the number of bytes needed on the SPU for a modifier.
		static int HK_CALL computeSpuBytes( hkbModifier* mod, hkbBehaviorGraph::GenerateWorkingData& data );

			/// Compute the number of bytes needed on the SPU for the information in the hkbBehaviorGraphData.
		static int HK_CALL computeSpuBytesForBehavior( const hkbBehaviorGraph& behavior );

			/// Compute the number of bytes to work with the mirrored skeleton
		static int HK_CALL computeSpuBytesForMirroredSkeleton( const hkaSkeleton& animationSkeleton );

			/// Flatten a hierarchy of modifiers into a list.  The input root modifier should be a node
			/// clone.  The returned modifiers are clones.
		static void HK_CALL flattenModifiers(	hkbModifier* rootModifierClone, 
												hkbBehaviorGraph& rootBehavior,
												hkArray<hkbModifier*>& modifiersLeafFirst );
};

#define HKB_ROUND_UP_FOR_SPU(x) HK_NEXT_MULTIPLE_OF( 128, x )

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
