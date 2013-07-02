/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAVMESH_INSTANCE_H
#define HKAI_NAVMESH_INSTANCE_H

extern const hkClass hkaiNavMeshInstanceClass;
#include <Ai/Pathfinding/Common/hkaiReferenceFrame.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>

class hkaiNavMeshUtils;

	/// Provides "instanced" access to hkaiNavMesh data, allowing changes to made to the instance without modifying the original data.
	/// This allows the same nav mesh to be shared between multiple hkaiWorlds.
	/// For faces and edges, a map is stored; if the value of this is -1, the face or edge is unchanged and the original can be read.
	/// If the map value is non-negative, it is an index into the corresponding m_instancedEdges or m_instancedFaces array.
	/// New faces, edges, and vertices (e.g. from cutting or user edges) are added to the m_ownedFaces, m_ownedEdges, and m_ownedVertices arrays.
class hkaiNavMeshInstance : public hkReferencedObject
{
	//+vtable(true)
	//+version(4)

	friend class hkaiNavMeshUtils;

#ifdef HK_PLATFORM_SPU
	friend class hkaiSpuNavMeshAccessor;
#endif

	public:
		
        HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);
		HK_DECLARE_REFLECTION();

		typedef hkUint16 CutInfo;

		enum DebugValues
		{
			DEAD_FACE = 0xDEADFACE,
			DEAD_EDGE = 0xDEADED6E,
		};

		enum CutInfoValues
		{
			NOT_CUT_EDGE = 0xFFFF,
		};

		hkaiNavMeshInstance();
		hkaiNavMeshInstance( hkFinishLoadedObjectFlag f );

		~hkaiNavMeshInstance();
		
			/// Initialize the instance for a given nav mesh.
		void init( const hkaiNavMesh& mesh, hkaiSectionUid uid, bool setupClearances = true );


			/// Reinitialize the instance for a given nav mesh (to be used if the instance was already in use)
		void reinit( const hkaiNavMesh& mesh, bool setupClearances = true );

			/// Initialize without setting up instancing data.
			/// This can be used e.g. to perform an A* search (which operates on an hkaiNavMeshInstance)
			/// without having to maintain an instance outside of the query.
			/// Note that this disables the benefits of instancing, thus if the instance is modified, so is the original mesh.
		void tempInit( const hkaiNavMesh& mesh, bool setupClearances );


		//
		// Read-only accessors. These are used during A*. The structure (face or edge) returned may
		// be either an original version, an instanced version, or a new version introduced by cutting.
		//

			/// Get the face at the given index.
		inline const hkaiNavMesh::Face& getFace( hkaiNavMesh::FaceIndex fIdx ) const;

			/// Get the edge at the given index.
		inline const hkaiNavMesh::Edge& getEdge( hkaiNavMesh::EdgeIndex eIdx ) const;

			/// Get the world-space vertex position. This transforms the local space vertex by m_transforms
		inline void getVertexPosition( hkaiNavMesh::VertexIndex vIdx, hkVector4& vOut ) const;

			/// Get the local-space vertex position. This is faster and more accurate if e.g., we're subtracting positions
		inline void getLocalVertexPosition( hkaiNavMesh::VertexIndex vIdx, hkVector4& vOut ) const;


		//
		// Edge vertex methods. These can be slightly more efficient than calling getVertexPosition or getLocalVertexPosition 
		// twice, because the DMAs can be interleaved on SPU.
		//
		
			/// Get the vertices for the given edge index
		inline void getEdgePoints( hkaiNavMesh::EdgeIndex eIdx, hkVector4& edgeA, hkVector4& edgeB ) const;

			/// Get the vertices for the given edge
		inline void getEdgePoints( const hkaiNavMesh::Edge& edge, hkVector4& edgeA, hkVector4& edgeB ) const;

			/// Get the local-space vertices for the given edge index
		inline void getEdgeLocalPoints( hkaiNavMesh::EdgeIndex eIdx, hkVector4& edgeA, hkVector4& edgeB ) const;

			/// Get the local-space vertices for the given edge
		inline void getEdgeLocalPoints( const hkaiNavMesh::Edge& edge, hkVector4& edgeA, hkVector4& edgeB ) const;


		//
		// Face flag access. These are modified frequently by nav mesh cutting - storing them in a separate array
		// means that we don't need to instance a face just to change its flags.
		//

			/// Return the flags for a given face.
		inline const hkaiNavMesh::FaceFlags getFlagsForFace( hkaiNavMesh::FaceIndex f ) const;

			/// Checks whether the face is hidden. More concise version of getFlagsForFace(f).anyIsSet(hkaiNavMesh::FACE_HIDDEN)
		HK_FORCE_INLINE hkBool32 isFaceHidden( hkaiNavMesh::FaceIndex f ) const;

			/// Checks whether the face is cut. More concise version of getFlagsForFace(f).anyIsSet(hkaiNavMesh::FACE_CUT)
		HK_FORCE_INLINE hkBool32 isFaceCut( hkaiNavMesh::FaceIndex f ) const;

			/// Checks whether the face was added for streaming. More concise version of getFlagsForFace(f).anyIsSet(hkaiNavMesh::FACE_STREAMING)
		HK_FORCE_INLINE hkBool32 isFaceStreaming( hkaiNavMesh::FaceIndex f ) const;

			/// Sets the flags for a given face.
		inline void setFlagsForFace( hkaiNavMesh::FaceIndex f, hkaiNavMesh::FaceFlags flags );

		//
		// Edge and face data accessors
		//

		HK_FORCE_INLINE hkaiNavMesh::EdgeData getEdgeData( hkaiNavMesh::EdgeIndex eIdx) const;
		HK_FORCE_INLINE hkaiNavMesh::FaceData getFaceData( hkaiNavMesh::FaceIndex fIdx) const;
		HK_FORCE_INLINE const hkaiNavMesh::EdgeData* getEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx) const;
		HK_FORCE_INLINE const hkaiNavMesh::FaceData* getFaceDataPtr( hkaiNavMesh::FaceIndex fIdx) const;
		HK_FORCE_INLINE hkaiNavMesh::EdgeData* getWritableEdgeDataPtr( hkaiNavMesh::EdgeIndex eIdx, hkBool32 instanceIfNotAlready = false);
		HK_FORCE_INLINE hkaiNavMesh::FaceData* getWritableFaceDataPtr( hkaiNavMesh::FaceIndex fIdx, hkBool32 instanceIfNotAlready = false);
		HK_FORCE_INLINE int getEdgeDataStriding( ) const { return m_edgeDataStriding; }
		HK_FORCE_INLINE int getFaceDataStriding( ) const { return m_faceDataStriding; }

		void setEdgeData( hkaiNavMesh::EdgeIndex eIdx, hkaiNavMesh::EdgeData d);
		void setFaceData( hkaiNavMesh::FaceIndex fIdx, hkaiNavMesh::FaceData d);


		//
		// Opposite edge/face access
		//

		HK_FORCE_INLINE hkaiPackedKey getOppositeFaceKeyForEdge( const hkaiNavMesh::Edge& edge ) const;
		HK_FORCE_INLINE hkaiPackedKey getOppositeEdgeKeyForEdge( const hkaiNavMesh::Edge& edge ) const;
		HK_FORCE_INLINE hkaiRuntimeIndex getOppositeSectionRuntimeIndexForEdge( const hkaiNavMesh::Edge& edge ) const;
		
		HK_FORCE_INLINE hkaiPackedKey getOppositeFaceKeyForEdge( hkaiNavMesh::EdgeIndex e) const { return getOppositeFaceKeyForEdge( getEdge(e) ); }
		HK_FORCE_INLINE hkaiPackedKey getOppositeEdgeKeyForEdge( hkaiNavMesh::EdgeIndex e) const { return getOppositeEdgeKeyForEdge( getEdge(e) ); }
		HK_FORCE_INLINE hkaiRuntimeIndex getOppositeSectionRuntimeIndexForEdge( hkaiNavMesh::EdgeIndex e) const { return getOppositeSectionRuntimeIndexForEdge( getEdge(e) ); }

		//
		// Edge cut info - these are only non-trivial for owned edges that came from cutting
		//

			/// Return cutInfo for an edge. If the edge index refers to an original or instanced edge, NOT_CUT_EDGE is always returned.
		inline hkaiNavMeshInstance::CutInfo getCutInfoForEdge( hkaiNavMesh::EdgeIndex e ) const;

		//
		// Get a user-modifiable version of the structure at the given index.
		// These methods never return a reference to a structure in the original nav mesh, only to instanced or owned structures.
		// When an original (non-instanced) stucture is requested, there are two possible behaviors
		//   - if instanceIfNotAlready is false, an assert is triggered and NULL reference is returned.
		//   - if instanceIfNotAlready is true, a new instance is created, and the reference to that is returned.
		//

			// Get a user-modifiable version of the face at the given index.
		inline hkaiNavMesh::Face* getWritableFace( hkaiNavMesh::FaceIndex fIdx, hkBool32 instanceIfNotAlready = false);

			// Get a user-modifiable version of the edge at the given index.
		inline hkaiNavMesh::Edge* getWritableEdge( hkaiNavMesh::EdgeIndex eIdx, hkBool32 instanceIfNotAlready = false );


		//
		// Read-only access for original faces/edges
		// The structure returned always belongs to the original face, it is never an instanced version.
		//

			/// Get the original face at the given index
		inline const hkaiNavMesh::Face& getOriginalFace( hkaiNavMesh::FaceIndex fIdx ) const;

			/// Get the original edge at the given index.
		inline const hkaiNavMesh::Edge& getOriginalEdge( hkaiNavMesh::EdgeIndex eIdx ) const;


			/// Get the owned face at the given index
		inline const hkaiNavMesh::Face& getOwnedFace( int fIdx ) const;

			/// Get the owned face at the given index
		inline hkaiNavMesh::Face& getOwnedFace( int fIdx );

			/// Get the owned edge at the given index.
		inline const hkaiNavMesh::Edge& getOwnedEdge( int eIdx ) const;

		/// Get the owned edge at the given index.
		inline hkaiNavMesh::Edge& getOwnedEdge( int eIdx );

		//
		// Local-to-world transform access
		//

			/// Get the reference frame for this instance (read-only)
		inline const hkaiReferenceFrame& getReferenceFrame() const;

			/// Get the reference frame for this instance
		inline       hkaiReferenceFrame& getReferenceFrame();

			/// Get the local-to-world transform (read-only)
		inline const hkTransform& getTransform() const;

			/// Set the local-to-world transform
			/// \warning If this mesh instance is part of a streaming collection, do not call this directly, but use hkaiStreamingCollection::setSectionTransform
		inline void setTransform( const hkTransform& transform );

		//
		// Adding and removing instances.
		//
		
			/// Create an instanced version of the face, or return the instance if it has already been instanced.
		hkaiNavMesh::Face& instanceFace( hkaiNavMesh::FaceIndex fIdx );

			/// Create an instanced version of the face, or return the instance if it has already been instanced.
		hkaiNavMesh::Edge& instanceEdge( hkaiNavMesh::EdgeIndex eIdx );

			/// Remove instanced version of the face at the given index
		void removeInstancedFace( hkaiNavMesh::FaceIndex fIdx );
		
			/// Remove instanced version of the edge at the given index
		void removeInstancedEdge( hkaiNavMesh::EdgeIndex eIdx );


		//
		// Simple self-explanatory accessors
		//
		inline int getNumFaces() const {return m_numOriginalFaces + m_ownedFaces.getSize(); }
		inline int getNumEdges() const {return m_numOriginalEdges + m_ownedEdges.getSize(); }
		inline int getNumVertices() const {return m_numOriginalVertices + m_ownedVertices.getSize(); }

		inline int getNumOriginalFaces() const {return m_numOriginalFaces; }
		inline int getNumOriginalEdges() const {return m_numOriginalEdges; }
		inline int getNumOriginalVertices() const {return m_numOriginalVertices; }

		inline int getNumOwnedFaces() const {return m_ownedFaces.getSize(); }
		inline int getNumOwnedEdges() const {return m_ownedEdges.getSize(); }
		inline int getNumOwnedVertices() const {return m_ownedVertices.getSize(); }

		inline const hkaiNavMesh* getOriginalMesh() const { return m_originalMesh; }
		inline hkaiSectionUid getSectionUid() const  { return m_sectionUid; }
		inline hkaiRuntimeIndex getRuntimeId() const  { return m_runtimeId; }
		inline bool isWallClimbing() const { return m_originalMesh->isWallClimbing(); }

		inline void setSectionUid(hkaiSectionUid uid)  { m_sectionUid = uid; }
		inline void setRuntimeId(hkaiRuntimeIndex idx) { m_runtimeId = idx; }

		
			/// Computes the transformed AABB of the nav mesh instance.
			/// Note that this is the transformed version of the orin gal mesh's AABB, so it might
			/// be larger than the smallest enclosing AABB.
		void getAabb( hkAabb& aabb ) const;

		
		//
		// Adding new faces/edges/vertices to the nav mesh
		// These are called by nav mesh cutting and never need to be called by the user.
		//

		struct AddFaceContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, AddFaceContext);
			inline AddFaceContext();
			hkaiNavMesh::Face m_face;
			hkaiNavMesh::FaceFlags m_flags;
			hkaiNavMesh::FaceData m_data[hkaiNavMesh::MAX_DATA_PER_FACE];
		};

		struct AddEdgeContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, AddEdgeContext);
			inline AddEdgeContext();
			hkaiNavMesh::Edge m_edge;
			hkaiNavMesh::EdgeData m_data[hkaiNavMesh::MAX_DATA_PER_EDGE];
			CutInfo m_cutInfo; //+default(NOT_CUT_EDGE)
		};

		void getFaceContext( hkaiNavMesh::FaceIndex fIdx, AddFaceContext& ctx ) const;
		void getFaceDataForContext( hkaiNavMesh::FaceIndex fIdx, AddFaceContext& ctx ) const;
		void getEdgeContext( hkaiNavMesh::EdgeIndex eIdx, AddEdgeContext& ctx ) const;
		void getEdgeDataForContext( hkaiNavMesh::EdgeIndex eIdx, AddEdgeContext& ctx ) const;


			/// Add new vertices to the end of the owned vertex array.
		void appendVertices( const hkVector4* v, int numVerts );

			/// Add new faces to the end of the owned face array.
		void appendFaces( const AddFaceContext* ctxs, int numFaces );
		
			/// Add new faces to the end of the owned face array. The start of the new edges is returned.
		hkaiNavMesh::Edge* appendEdges( const AddEdgeContext* ctxs, int numEdges );

			/// Expand the owned edge array, and return a pointer to the start of the new edges
		hkaiNavMesh::Edge* expandEdgesBy(int n);


		//
		// Vertex clearance information
		//
		static inline int HK_CALL getNumClearanceValuesForFace( int numEdges ) { return 3*(numEdges-2); }
		inline bool hasClearanceInfo() const;
		inline hkReal getMaxGlobalClearance() const { return m_maxGlobalClearance; }
		void setMaxGlobalClearance( hkReal globalClear );
		inline hkHalf getClearance(int index) const;
		inline hkHalf getGlobalClearance( hkaiNavMesh::VertexIndex v) const;
		inline int getClearanceIndexForFace( hkaiNavMesh::FaceIndex f ) const { return m_faceClearanceIndices[f]; }
		
		inline hkArray<hkHalf>& getClearanceCache() { return m_clearanceCache; }
		inline hkArray<hkHalf>& getGlobalClearanceCache() { return m_globalClearanceCache; }
		inline hkArray<int>& getFaceClearanceIndices() { return m_faceClearanceIndices; }
		
		void initClearanceCache();
		void initGlobalClearanceCache();
		void resetVertexClearances();
		

			/// Advanced usage - used on the SPU as an optimization for A*
		inline void getFaceAndEdgePointers( hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex edgeIndex, 
			const hkaiNavMesh::Face*& facePtr, const hkaiNavMesh::Edge*& edgePtr, const hkaiNavMesh::FaceData*& faceDataOut ) const;

		inline const hkaiNavMesh::Face& getFaceAndDataPtr(hkaiNavMesh::FaceIndex fIdx, const hkaiNavMesh::FaceData*& faceDataOut) const;
		inline const hkaiNavMesh::Edge& getEdgeAndDataPtr(hkaiNavMesh::EdgeIndex eIdx, const hkaiNavMesh::EdgeData*& edgeDataOut) const;

			/// Gets an ID of an original face suitable for use with an hkDebugDisplayHandler. Used by hkaiNavMeshGeometryViewer.
		inline hkUlong getDisplayIdForOriginalFace( hkaiNavMesh::FaceIndex faceIndex )const;

			/// Gets an ID of an owned face suitable for use with an hkDebugDisplayHandler. Used by hkaiNavMeshGeometryViewer.
		inline hkUlong getDisplayIdForOwnedFace( hkaiNavMesh::FaceIndex faceIndex ) const;


			/// Utility method for validating determinism
		void checkDeterminism() const;

		inline hkReal getErosionRadius() const;

#ifndef HK_PLATFORM_SPU
			/// Utility method for accessnig data. On CPU, this is just a simple pointer dereference.
		template<typename DATA>
		static inline DATA HK_CALL getUserDataFromMainMemory(const DATA* dataPtr);

			/// Copy the owned edge and associated data. The indices are indices into m_ownedEdges, not hkaiNavMesh::EdgeIndex's.
		inline void copyOwnedEdge(int dstIndex, int srcIndex);

	protected:


			/// Copy the owned face and associated data. The indices are indices into m_ownedFaces, not hkaiNavMesh::FaceIndex's.
		inline void copyOwnedFace(int dstIndex, int srcIndex);

			/// Copy the instancnd edge and associated data. The indices are indices into m_instancedEdges, not hkaiNavMesh::EdgeIndex's.
		inline void copyInstancedEdge(int dstIndex, int srcIndex);


		inline void setNumOwnedEdges( int numOwnedEdges );
		inline void setNumOwnedFaces( int numOwnedFaces );
		inline void setNumInstancedEdges( int numInstancedFaces );
#endif
	protected:

		//
		// Pointers to the original nav mesh data, and their respective sizes.
		//

		const hkaiNavMesh::Face*		m_originalFaces;		//+nosave
		int								m_numOriginalFaces;		//+nosave

		const hkaiNavMesh::Edge*		m_originalEdges;		//+nosave
		int								m_numOriginalEdges;		//+nosave

		const hkVector4*				m_originalVertices;		//+nosave
		int								m_numOriginalVertices;	//+nosave

		const hkaiNavMesh::FaceData*	m_originalFaceData;	//+nosave
		int								m_faceDataStriding; //+nosave
		
		const hkaiNavMesh::EdgeData*	m_originalEdgeData;	//+nosave
		int								m_edgeDataStriding; //+nosave


			/// The nav mesh that this instances
		hkRefPtr<const hkaiNavMesh>	m_originalMesh;

			/// Referennce frame for the instance
		hkaiReferenceFrame			m_referenceFrame;

		//
		// Mapping between the original structures and the instanced versions.
		// Each map is the size of the original array, and initialized to -1
		//

			/// Mapping information from original to instanced edges.
		hkArray<int>				m_edgeMap;

			/// Mapping information from original to instanced faces.
		hkArray<int>				m_faceMap;

			/// Faces which are modifiable copies of the original versions.
		hkArray<hkaiNavMesh::Face>	m_instancedFaces;

			/// Edges which are modifiable copies of the original versions.
		hkArray<hkaiNavMesh::Edge>	m_instancedEdges;

			/// Faces that belong specifically to this instance, usually created during cutting
		hkArray<hkaiNavMesh::Face>	m_ownedFaces;

			/// Edges that belong specifically to this instance, usually created during cutting
		hkArray<hkaiNavMesh::Edge>	m_ownedEdges;

			/// Vertices that belong specifically to this instance, usually created during cutting
		hkArray<hkVector4>			m_ownedVertices;
		
			/// Flags for all the faces. These are stored separately to avoid extra instancing of the faces during cutting.
		hkArray<hkaiNavMesh::FaceFlags> m_faceFlags; //+overridetype(hkArray<hkUint8>)

			/// Cutting information - for cut edges, this is the relative edge index (for the given face) from which the cut edge came.
			/// For internal or silhouette edges, this is set to NOT_CUT_EDGE
		hkArray<hkaiNavMeshInstance::CutInfo>	m_cuttingInfo;

			/// User data for instanced faces.
		hkArray<hkaiNavMesh::FaceData> m_instancedFaceData;

			/// User data for instanced edges.
		hkArray<hkaiNavMesh::EdgeData> m_instancedEdgeData;

			/// User data for owned faces.
		hkArray<hkaiNavMesh::FaceData> m_ownedFaceData;

			/// User data for owned edges.
		hkArray<hkaiNavMesh::EdgeData> m_ownedEdgeData;

		//
		// Vertex clearance information
		//
			/// Clearance in the arc of an edge
		mutable hkArray<hkHalf>		m_clearanceCache;

			/// Clearance in all directions at a vertex
		mutable hkArray<hkHalf>		m_globalClearanceCache;

			/// Mapping from a face index to the start of its information in the clearance cache
		hkArray<int>				m_faceClearanceIndices;

			/// The upper limit used during clearance calculations. Should be larger than the biggest character that will do A* queries.
		hkReal						m_maxGlobalClearance;

			/// Unique section ID of this instance.
		hkaiSectionUid				m_sectionUid; //+default(0)

			/// Runtime ID of this instance.
			/// Only assigned when the mesh is loaded.
		hkaiRuntimeIndex			m_runtimeId; //+default(0)

};



#ifndef HK_PLATFORM_SPU
typedef hkaiNavMeshInstance hkaiNavMeshAccessor;
#else
typedef class hkaiSpuNavMeshAccessor hkaiNavMeshAccessor;
#endif

#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.inl>

#endif //HKAI_NAVMESH_INSTANCE_H

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
