/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_STREAMING_COLLECTION_H
#define HKAI_STREAMING_COLLECTION_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>

class hkaiNavMeshInstance;
class hkaiNavVolumeInstance;
class hkaiDirectedGraphInstance;
class hkaiNavMeshQueryMediator;
class hkaiNavVolumeMediator;
class hkcdDynamicAabbTree;

	/// A group of hkaiNavMeshInstances and/or hkaiNavVolumes that have been loaded into the hkaiWorld and connected via stitching.
	/// This is the main input for path searches (e.g. hkaiPathfindingUtil::findPathInput).
	/// This is maintained by the hkaiWorld and doesn't need to be handled by the user.
class hkaiStreamingCollection : public hkReferencedObject
{
public:
	//+vtable(true)
	//+version(2)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_NAVMESH);

	typedef hkaiTreeHandle TreeHandle;

	hkaiStreamingCollection();
	
	~hkaiStreamingCollection();

	hkaiStreamingCollection(hkFinishLoadedObjectFlag f);

		/// The relevant data structure(s) for a section.
		/// Only one of m_instancePtr and m_volumePtr can be non-NULL, as with m_mediator and m_volumeMediator.
	struct InstanceInfo
	{
		//+version(4)
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiStreamingCollection::InstanceInfo);
		HK_DECLARE_REFLECTION();

		HK_FORCE_INLINE InstanceInfo() 
		:	m_instancePtr(HK_NULL),
			m_volumeInstancePtr(HK_NULL),
			m_clusterGraphInstance(HK_NULL),
			m_mediator(HK_NULL),
			m_volumeMediator(HK_NULL),
			m_treeNode( TreeHandle(-1) )
		{
		}

		inline bool isEmpty() const
		{
			// Can't have mediators without the corresponding structure, so no need to check them.
			return !(m_instancePtr || m_volumeInstancePtr || m_clusterGraphInstance);
		}

		hkaiNavMeshInstance* m_instancePtr;
		hkaiNavVolumeInstance*	m_volumeInstancePtr;
		hkaiDirectedGraphInstance* m_clusterGraphInstance;
		const hkaiNavMeshQueryMediator* m_mediator;
		const hkaiNavVolumeMediator* m_volumeMediator;
		hkaiTreeHandle m_treeNode;
	};


#ifndef HK_PLATFORM_SPU
		/// Initialize the collection with the given instance, graph, and mediator. These are reference counted.
		/// Note that any other meshes or nav volumes that have been added will be removed.
	void init( hkaiNavMeshInstance* meshInstance, hkaiDirectedGraphInstance* clusterGraph = HK_NULL, const hkaiNavMeshQueryMediator* mediator = HK_NULL);

		/// Initialize the collection with the given nav volume and mediator. These are reference counted.
		/// Note that any other meshes or nav volumes that have been added will be removed.
	void init( hkaiNavVolumeInstance* vol, const hkaiNavVolumeMediator* mediator = HK_NULL );

		/// Initialize the collection with the given instance, graph, and mediator WITHOUT adding references to them.
		/// This should only be used for temporary collections (i.e on the stack).
	HK_FORCE_INLINE void initNoRefCount( hkaiNavMeshInstance* meshInstance, hkaiDirectedGraphInstance* clusterGraph = HK_NULL, const hkaiNavMeshQueryMediator* mediator = HK_NULL);

		/// Initialize the collection with the given graph, WITHOUT adding a reference to it.
		/// This should only be used for temporary collections (i.e on the stack).
	HK_FORCE_INLINE void initNoRefCount( hkaiDirectedGraphInstance* clusterGraph );

		/// Initialize the collection with the given nav volume and mediator WITHOUT adding references to them.
		/// This should only be used for temporary collections (i.e on the stack).
	HK_FORCE_INLINE void initNoRefCount( hkaiNavVolumeInstance* vol, const hkaiNavVolumeMediator* mediator = HK_NULL);
#endif
	
	void addInstance( hkaiNavMeshInstance* instance, const hkaiNavMeshQueryMediator* mediator, hkaiDirectedGraphInstance* clusterGraph = HK_NULL );
	void removeInstance( hkaiNavMeshInstance* instance, hkaiDirectedGraphInstance* clusterGraph = HK_NULL );
	void removeInstanceAt( hkaiRuntimeIndex i );

	void addMeshGraph( hkaiDirectedGraphInstance* clusterGraph );
	void removeMeshGraph( hkaiDirectedGraphInstance* clusterGraph );
	void removeMeshGraphAt( hkaiRuntimeIndex i );

	void addVolume( hkaiNavVolumeInstance* instance, const hkaiNavVolumeMediator* mediator );
	void removeVolume( hkaiNavVolumeInstance* instance );
	void removeVolumeAt( hkaiRuntimeIndex i );

		/// Removes all loaded nav mesh instances and nav volumes
	void clear();
	

	HK_FORCE_INLINE const InstanceInfo* getInstanceInfoPtr() const { return m_instances.begin(); }
	HK_FORCE_INLINE int getNumInstanceInfos() const { return m_instances.getSize(); }

	int getNumInstances() const;
	HK_FORCE_INLINE const hkaiNavMeshInstance* getInstanceAt( hkaiRuntimeIndex i ) const { return m_instances[i].m_instancePtr; }
	HK_FORCE_INLINE       hkaiNavMeshInstance* getInstanceAt( hkaiRuntimeIndex i )       { return m_instances[i].m_instancePtr; }
	HK_FORCE_INLINE const hkaiNavMeshQueryMediator* getMediatorAt( hkaiRuntimeIndex i ) const { return m_instances[i].m_mediator; }
	HK_FORCE_INLINE const hkaiDirectedGraphInstance* getGraphInstanceAt( hkaiRuntimeIndex id ) const
	{
		HK_ASSERT(0x61ce1fa5, id < m_instances.getSize() );
		return ( id < 0 ) ? HK_NULL : m_instances[id].m_clusterGraphInstance;
	}
	HK_FORCE_INLINE hkaiDirectedGraphInstance* getGraphInstanceAt( hkaiRuntimeIndex id )
	{
		HK_ASSERT(0x61ce1fa5, id < m_instances.getSize() );
		return ( id < 0 ) ? HK_NULL : m_instances[id].m_clusterGraphInstance;
	}

	int getNumVolumes() const;
	HK_FORCE_INLINE const hkaiNavVolumeInstance* getVolumeInstanceAt( hkaiRuntimeIndex i ) const { return m_instances[i].m_volumeInstancePtr; }
	HK_FORCE_INLINE       hkaiNavVolumeInstance* getVolumeInstanceAt( hkaiRuntimeIndex i )       { return m_instances[i].m_volumeInstancePtr; }
	HK_FORCE_INLINE const hkaiNavVolumeMediator* getVolumeMediatorAt( hkaiRuntimeIndex i ) const { return m_instances[i].m_volumeMediator; }

	// Static accessors for specfic sections
	static HK_FORCE_INLINE const hkaiNavMeshInstance* getInstanceAt( const InstanceInfo* infoMainMem, hkaiRuntimeIndex i ) { return infoMainMem[i].m_instancePtr; }
	static HK_FORCE_INLINE const hkaiDirectedGraphInstance* getGraphInstanceAt( const InstanceInfo* infoMainMem, hkaiRuntimeIndex i ) { return infoMainMem[i].m_clusterGraphInstance; }
	static HK_FORCE_INLINE const hkaiNavVolumeInstance* getVolumeInstanceAt( const InstanceInfo* infoMainMem, hkaiRuntimeIndex i ) { return infoMainMem[i].m_volumeInstancePtr; }

	//
	// Nav Mesh struct access
	//
	HK_FORCE_INLINE const hkaiNavMesh::Face& getFaceFromPacked( hkaiPackedKey faceKey) const;
	HK_FORCE_INLINE       hkaiNavMesh::Face& getWritableFaceFromPacked( hkaiPackedKey faceKey, hkBool32 instanceIfNeeded = false);
	
	HK_FORCE_INLINE const hkaiNavMesh::Edge& getEdgeFromPacked(     hkaiPackedKey edgeKey) const;
	HK_FORCE_INLINE       hkaiNavMesh::Edge& getWritableEdgeFromPacked( hkaiPackedKey edgeKey, hkBool32 instanceIfNeeded  = false );

	HK_FORCE_INLINE const hkaiNavMesh::FaceData* getFaceDataPtrFromPacked(hkaiPackedKey faceKey) const;
	HK_FORCE_INLINE       hkaiNavMesh::FaceData* getWritableFaceDataPtrFromPacked(hkaiPackedKey faceKey, hkBool32 instanceIfNeeded = false );

	HK_FORCE_INLINE const hkaiNavMesh::EdgeData* getEdgeDataPtrFromPacked(hkaiPackedKey edgeKey) const;
	HK_FORCE_INLINE       hkaiNavMesh::EdgeData* getWritableEdgeDataPtrFromPacked(hkaiPackedKey edgeKey, hkBool32 instanceIfNeeded = false );

	static const hkaiNavMesh::Face& HK_CALL getFaceFromPacked( const InstanceInfo* info, hkaiPackedKey faceKey);
	static void HK_CALL getEdgePoints( const InstanceInfo* info, hkaiPackedKey edgeKey, hkVector4& ea, hkVector4& eb );

	static HK_FORCE_INLINE const hkaiNavMeshInstance* getNavMeshInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId );
	static HK_FORCE_INLINE const hkaiDirectedGraphInstance* getGraphInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId );
	static HK_FORCE_INLINE const hkaiNavVolumeInstance* getVolumeInstancePtrFromMainMem( const InstanceInfo* infoMainMem, hkaiRuntimeIndex sectionId );

	hkaiRuntimeIndex findSectionIdByUid( hkaiSectionUid uid ) const;
	hkaiRuntimeIndex findGraphIdByUid( hkaiSectionUid uid ) const;
	hkaiRuntimeIndex findVolumeIdByUid( hkaiSectionUid uid ) const;

		/// Set the local-to-world transform for an instance, and update its entry in the AABB tree
		/// \note This calls hkaiNavMeshInstance::setTransform internally
	void setSectionTransform( hkaiNavMeshInstance& meshInstance, const hkTransform& transform );

		/// Set the local-to-world transform for section (hkaiNavMeshInstance and/or hkaiDirectedGraphInstance), and update its entry in the AABB tree
		/// \note This calls hkaiNavMeshInstance::setTransform internally
	void setSectionTransform( hkaiRuntimeIndex runtimeIndex, const hkTransform& transform );

		/// Set the local-to-world translation for a volume instance, and update its entry in the AABB tree
		/// \note This calls hkaiNavVolumeInstance::setTranslation internally
	void setVolumeTranslation( hkaiNavVolumeInstance& volumeInstance, hkVector4Parameter translation );


		/// Initializes the internal AABB tree. This is done automatically in addTreeNode(), but can be called before so that getAabbTree() returns a valid pointer.
	void setupTree();


	HK_FORCE_INLINE const hkcdDynamicAabbTree* getAabbTree() const { return m_tree; }

	//
	// Validation methods
	//
		/// Check whether the edge key belongs to the face key
	bool areKeysConsistent( hkaiPackedKey faceKey, hkaiPackedKey edgeKey ) const;
	static void HK_CALL validate( const hkaiStreamingCollection& collection, bool checkAabb = true, bool checkClearanceCaches = true);
	static void HK_CALL validateExternalEdges( const hkaiStreamingCollection& collection, const hkaiNavMeshInstance& meshInstance );
	static void HK_CALL validateGraphInstance( const hkaiStreamingCollection& manager, const hkaiDirectedGraphInstance& graph );

protected:
	
		/// Updates the internal AABB tree when a new mesh or volume are loaded
	void addTreeNode( hkaiRuntimeIndex sectionIdx, const hkAabb& aabb );


public:


		/// Whether or not to reference count the nav mesh instance, etc.
		/// This is false by default but true if using ::initNoRefCount
	hkBool m_isTemporary; //+default(false)

	/// AABB tree that is updated when a section is added/removed.
	/// This is used by raycast and AABB queries to reduce the number of meshes queried.
	/// The section ID (rather than the nav mesh pointer) is used at the leaf data
	hkRefPtr<hkcdDynamicAabbTree> m_tree;

	hkArray<InstanceInfo> m_instances;
};

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.inl>
#endif // HKAI_STREAMING_COLLECTION_H

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
