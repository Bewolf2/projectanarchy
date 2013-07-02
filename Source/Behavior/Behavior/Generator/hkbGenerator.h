/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_GENERATOR_H
#define HKB_GENERATOR_H

#include <Behavior/Behavior/Node/hkbNode.h>
#include <Behavior/Behavior/Generator/hkbGeneratorSyncInfo.h>
#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>

class hkbGeneratorOutput;
class hkaMirroredSkeleton;

extern const hkClass hkbGeneratorClass;

/// A base class for the principal node types that make up a behavior graph.
///
/// An hkbGenerator generates animation poses over time, along with other track data.
class hkbGenerator : public hkbNode
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbGenerator( hkbNodeType nodeType ) : hkbNode(nodeType), m_syncInfo(HK_NULL) {}
		~hkbGenerator();

	protected:

		hkbGenerator( const hkbGenerator& gen ) : hkbNode( gen ), m_syncInfo(HK_NULL) {}

	public:
			/// Generate an output pose and reference frame.
			/// 
			/// Must be called between calls to activate() and deactivate().
			/// Do not call this method directly.  Use hkbBehaviorGraph::generate().
		virtual void generate(	const hkbContext& context, 
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const = 0;

			/// Whether or not the generator can handle the situation where one of the 
			/// activeChildrenOutputs passed to generate() is the same address as the output argument.
			/// 
			/// Nodes that can recycle their output can be handled more efficiently.  See the implementation
			/// of hkbModifierGenerator for an example.
		virtual bool canRecycleOutput() const;

			/// Computes the synchronization info for the generator based on the previously
			/// computed synchronization info of the children.
		virtual void updateSync( const hkbContext& context );

			/// Set the time of the generator.
			/// 
			/// Do not call this method directly.  It is called by hkbBehaviorGraph::setActiveGeneratorLocalTime().
		virtual void setLocalTime( const hkbContext& context, hkReal seconds );

			/// Start an echo for self-transition.  This is for internal use.
		virtual void startEcho( hkReal echoDuration ) {}

			
			
			
			
			

			/// Called in root-first order on all of the active generators before update() is called on any node.
		virtual void preUpdate( const hkbContext& context, hkReal timestep ) {}

			/// Computes the partition info for this generator. Use the default implementation for generators that are leafs
			/// or with generators that have all the children that are generators.
		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context );
		
			/// Returns m_syncInfo and creates it if necessary.
		hkbGeneratorSyncInfo& accessSyncInfo();

			/// Returns m_syncInfo.
		const hkbGeneratorSyncInfo& getSyncInfo() const;

			/// Delete m_syncInfo.
		void discardSyncInfo();

			/// Returns beginning of the m_partitionIndices array.
		hkbGeneratorPartitionInfo& accessPartitionInfo();

			/// Returns beginning of the m_partitionIndices array.
		const hkbGeneratorPartitionInfo& getPartitionInfo() const;

	private:

		hkbGeneratorPartitionInfo m_partitionInfo; //+nosave

		hkbGeneratorSyncInfo* m_syncInfo; //+nosave

			// make the layout test succeed
		hkInt8 m_pad[4]; //+nosave

	public:

		hkbGenerator( hkFinishLoadedObjectFlag flag ) : hkbNode(flag) {}
};

#include <Behavior/Behavior/Generator/hkbGenerator.inl>

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
