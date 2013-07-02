/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_NAVMESH_CUTTER_H
#define HK_NAVMESH_CUTTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiCompoundSilhouette.h>
#include <Ai/Internal/NavMesh/hkaiNavMeshGenerationSettings.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshUtils.h>
#include <Ai/Pathfinding/Dynamic/NavMeshCutter/hkaiNavMeshSilhouetteSelector.h>
#include <Common/Base/Container/Set/hkSet.h>


extern const class hkClass hkaiNavMeshCutterMeshInfoClass;

extern const class hkClass hkaiNavMeshCutterSavedConnectivityClass;

extern const class hkClass hkaiNavMeshCutterOriginalEdgeInfoClass;

extern const class hkClass hkaiNavMeshCutterClass;

class hkaiStreamingCollection;
class hkaiGeneralAccessor;

// Uncomment this to enable more detailed timers in the cutter
//#define HKAI_CUTTER_DETAILED_TIMERS
#ifdef HKAI_CUTTER_DETAILED_TIMERS
#	define HKAI_CUTTER_TIME_CODE_BLOCK(X,Y) HK_TIME_CODE_BLOCK(X, Y)
#	define HKAI_CUTTER_TIMER_BEGIN_LIST(X,Y) HK_TIMER_BEGIN_LIST(X, Y)
#	define HKAI_CUTTER_TIMER_SPLIT_LIST(X) HK_TIMER_SPLIT_LIST(X)
#	define HKAI_CUTTER_TIMER_END_LIST() HK_TIMER_END_LIST()
#else
#	define HKAI_CUTTER_TIME_CODE_BLOCK(X,Y)
#	define HKAI_CUTTER_TIMER_BEGIN_LIST(X,Y)
#	define HKAI_CUTTER_TIMER_SPLIT_LIST(X)
#	define HKAI_CUTTER_TIMER_END_LIST()
#endif


// Uncomment this to add extra validation checks to the nav mesh cutter
//#define HKAI_CUTTER_EXTRA_CHECKS

const int EDGE_CONNECTIVITY_INTERNAL = 0xffffff;
const int EDGE_CONNECTIVITY_SILHOUETTE = 0xfffffe;


/// hkaiMap operations
struct hkaiCutterOperations
{
	inline static unsigned hash( hkaiPackedKey key, unsigned mod ) { return (unsigned(key) * 2654435761U) & mod; }
	inline static void invalidate( hkaiPackedKey& key ) { key = HKAI_INVALID_PACKED_KEY; }
	inline static hkBool32 isValid( hkaiPackedKey key ) { return key != HKAI_INVALID_PACKED_KEY; }
	inline static hkBool32 equal( hkaiPackedKey key0, hkaiPackedKey key1 ) { return key0 == key1; }
};

struct hkaiFaceCutResults;

/// A face key which can be resolved to the same face (cut or not) after other faces are re-cut, as long
/// as the face itself is not re-cut. Fill the structure with hkaiNavMeshCutter::makePersistentFaceKey,
/// and resolve it to a face key with hkaiNavMeshCutter::resolvePersistentFaceKey.
struct hkaiPersistentFaceKey
{
	//+version(0)
	HK_DECLARE_POD_TYPE();
	HK_DECLARE_REFLECTION();

	hkaiPackedKey m_key;
	hkInt16 m_offset;

	static const hkInt16 OFFSET_UNCUT_FACE = -1;

	hkaiRuntimeIndex getRuntimeId() const { return hkaiGetRuntimeIdFromPacked(m_key); }

	hkBool32 isValid() const { return m_key != HKAI_INVALID_PACKED_KEY; }

	hkBool32 operator==(hkaiPersistentFaceKey const& other) const
	{
		return m_key == other.m_key && m_offset == other.m_offset;
	}
	hkBool32 operator!=(hkaiPersistentFaceKey const& other) const
	{
		return m_key != other.m_key || m_offset != other.m_offset;
	}

	void setInvalid() { m_key = HKAI_INVALID_PACKED_KEY; m_offset = -1; }
};

struct hkaiPersistentEdgeKey
{
	HK_DECLARE_POD_TYPE();
	HK_DECLARE_REFLECTION();

	hkaiPersistentFaceKey m_faceKey;
	hkInt16 m_edgeOffset;

	/// A value for m_edgeOffset which specifies that this "edge key" is to be
	/// used only for its contained face key.
	static const hkInt16 EDGE_OFFSET_FACE_ONLY = -1;

	hkaiRuntimeIndex getRuntimeId() const { return m_faceKey.getRuntimeId(); }

	hkBool32 hasValidFace() const { return m_faceKey.isValid(); }
	hkBool32 isValidEdge() const { return m_faceKey.isValid() && m_edgeOffset != EDGE_OFFSET_FACE_ONLY; }

	hkBool32 operator==(hkaiPersistentEdgeKey const& other) const
	{
		return m_faceKey == other.m_faceKey && m_edgeOffset == other.m_edgeOffset;
	}
	hkBool32 operator!=(hkaiPersistentEdgeKey const& other) const
	{
		return m_faceKey != other.m_faceKey || m_edgeOffset != other.m_edgeOffset;
	}

	void setInvalid() { m_faceKey.setInvalid(); m_edgeOffset = EDGE_OFFSET_FACE_ONLY; }

	void setFaceOnly() { m_edgeOffset = EDGE_OFFSET_FACE_ONLY; }
};


/// Dynamically and incrementally cuts 2d shapes from a nav mesh.
/// Starting with a base nav mesh, this class can cut a subset of
/// faces to allow for additional obstacles. Cutting is always done from the
/// base nav mesh which is modified in place. Each original face is
/// replaced by one or more smaller "cut" faces.
/// Cut faces, edges and vertices appear after the original faces,
/// edges and vertices. Note that their indices will change after each cut
/// as empty space is compacted.
/// Original faces, edges and vertices remain at their original indices.
/// Faces which have been cut and therefore not logically part of the nav mesh
/// have hkaiNavMesh::FLAG_HIDDEN set.
class hkaiNavMeshCutter : public hkReferencedObject
{
public:

	//+vtable(true)
	//+version(11)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

	enum ClearanceResetMethod
	{
			/// Always reset all clearance values. This is fast, but will increase path search costs.
		CLEARANCE_RESET_ALL,
			
			/// Selectively reset clearance values by querying the mediator for nearby faces.
		CLEARANCE_RESET_MEDIATOR,

			/// Selectively reset clearance values by flood filling from cut faces.
		CLEARANCE_RESET_FLOODFILL,
	};

		/// List of OriginalEdgeInfos
	struct SavedConnectivity
	{
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, SavedConnectivity);
		SavedConnectivity();
		SavedConnectivity( hkFinishLoadedObjectFlag f) : m_storage(f) {}
		~SavedConnectivity();

		void addConnectivity( hkaiPackedKey edgeKey );
		void popConnectivityForFace( hkaiStreamingCollection* collection, hkaiPackedKey faceKey );
		hkBool32 popConnectivityForEdgeAndFace( hkaiStreamingCollection* collection, hkaiPackedKey edgeKey );
		void popConnectivityForOppositeFace( hkaiStreamingCollection* collection, hkaiPackedKey faceKey );

		void restore( hkaiStreamingCollection* collection, hkaiPackedKey edgeKey );
		void append( const SavedConnectivity& other);

		typedef hkSetUint32 StorageMap;
		StorageMap m_storage;
	};

		/// Special values in the m_magic array
	enum
	{
			/// Face has not been cut
		FACE_NOT_CUT = -1,
			/// Face is entirely removed and has no corresponding cut faces
		FACE_COMPLETELY_CUT = -2
	};

	typedef hkaiNavMeshSilhouetteSelector SilhouetteSelector;

	/// Map an original edge to an array of new edges
	struct FaceEdgePair
	{
		HK_DECLARE_POD_TYPE();
		hkaiPackedKey m_edgeKey;
		hkaiPackedKey m_faceKey;

		inline void set(hkaiPackedKey faceKey, hkaiPackedKey edgeKey)
		{
			m_faceKey = faceKey;
			m_edgeKey = edgeKey;
		}

		inline void setWithSection( hkaiRuntimeIndex sectionId, hkaiNavMesh::FaceIndex faceIndex, hkaiNavMesh::EdgeIndex edgeIndex )
		{
			m_faceKey = hkaiGetPackedKey(sectionId, faceIndex);
			m_edgeKey = hkaiGetPackedKey(sectionId, edgeIndex);
		}

	};

	typedef hkArray<hkaiPackedKey> FaceList;
	typedef hkPointerMap< hkaiPackedKey, FaceList*, hkContainerTempAllocator > EdgeMap;
	typedef hkSet< hkaiPackedKey, hkContainerTempAllocator> EdgeSet;

	//
	// Methods
	//

		/// Default
	hkaiNavMeshCutter();

		/// Use the supplied mesh manager as the base mesh.
	void init( hkVector4Parameter up, class hkaiStreamingCollection* collection );

		/// Only set up one section
	void initSection( hkaiRuntimeIndex sectionIdx, class hkaiNavMeshInstance* meshInstance, bool doChecks = true );

	void setClearanceResetMethod( ClearanceResetMethod crt ) { m_clearanceResetMethod = crt; }
	ClearanceResetMethod getClearanceResetMethod( ) const { return m_clearanceResetMethod; }

		/// Cut faces returned by the selector.
		/// Note that cut (but not original) face, edge and vertex indices will be invalid after this call.
		/// If the job queue and job thread pool are specified, the cutting will be performed multithreaded.
	hkBool32 cutSilhouettesIncremental( SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams, 
		hkArray<hkaiPackedKey>::Temp& cutFacesKeys, hkArray<hkaiPackedKey>::Temp& uncutFacesKeys, const hkBitField* sectionsToStep, class hkJobQueue* jobQueue = HK_NULL, 
		class hkJobThreadPool* threadPool = HK_NULL );

		/// Get the lowest-numbered cut face corresponding to the given original face, or HKAI_INVALID_PACKED_KEY
		/// if the face is not cut or is copmpletely cut.
	hkaiPackedKey getFirstCutKeyFromOriginal(hkaiPackedKey orig) const;

		/// Get the keys of the faces which were cut from an original face.
		/// Returns false if the face was not cut.
	hkBool32 getCutKeysFromOriginal( hkaiPackedKey orig, hkArray<hkaiPackedKey>& cutOut ) const;

		/// Get the start and end indices of the faces which were cut from an original face.
		/// Returns false if the face was not cut.
		/// The range is inclusive, so iterating over the faces would look like:
		/// hkaiNavMesh::FaceIndex faceStart = hkaiNavMesh::INVALID_FACE_INDEX, faceEnd = = hkaiNavMesh::INVALID_FACE_INDEX;
		/// for (hkaiNavMesh::FaceIndex f = faceStart; f <= faceEnd; f++) { ... }
	hkBool32 getCutKeyRangeFromOriginal( hkaiPackedKey orig, hkaiNavMesh::FaceIndex& faceStartOut, hkaiNavMesh::FaceIndex& faceEndOut) const;

		/// Returns the number of faces which were cut from an original face.
		/// Returns 0 if the face was not cut.
	int getNumCutKeysFromOriginal( hkaiPackedKey orig ) const;

		/// Get the original key of the face from a cut face.
		/// If the face isn't cut, returns HKAI_INVALID_FACE_KEY
	int getOriginalFromCutKey( hkaiPackedKey cutFaceKey) const;

		/// Get the original number of faces for a mesh section
	int getOriginalNumberOfFaces( hkaiRuntimeIndex sectionIdx ) const;
	
		/// Gets the closest point on the specified face. If the face is hidden, its corresponding hit faces are checked.
	hkaiPackedKey getClosestPointOnFaceLocal( hkVector4Parameter position, hkaiPackedKey faceKey, hkVector4& closestPointOut ) const;

		/// Undoes all cuts for the specified section
	void uncutSection( hkaiRuntimeIndex sectionIndex, bool doChecks = true );

		/// Access the mesh being cut.
	const hkaiStreamingCollection& getManager() const { return *m_streamingCollection; }

	hkaiNavMeshCutter(hkFinishLoadedObjectFlag f);

	void matchNewEdgesFromOriginalEdge( hkaiStreamingCollection* collection, hkArrayBase<FaceEdgePair>& pairsA, hkArrayBase<FaceEdgePair>& pairsB );

	const hkArray<int>& getMappingForSection( hkaiRuntimeIndex secIdx ) const { return m_meshInfos[secIdx].m_magic; }

	enum GatherCutEdgesMode
	{
		GATHER_ALL_EDGES,
		GATHER_BOUNDARY_EDGES
	};

	void gatherCutEdges( hkaiPackedKey originalEdgeKey, hkaiPackedKey originalFaceKey, hkArray<hkaiNavMeshCutter::FaceEdgePair>::Temp& cutPairs, GatherCutEdgesMode gatherMode = GATHER_ALL_EDGES) const;

		/// Merge contiguous boundary edges on the cut face that came from the same original edge
	void mergeCutBoundaryEdges( hkaiPackedKey originalFaceKey, hkaiPackedKey originalEdgeKey );

	void gatherCutEdgesFromCutFaces( hkaiPackedKey originalEdgeKey, const hkArrayBase<hkaiPackedKey>& cutfaceKeys,  hkArray<hkaiNavMeshCutter::FaceEdgePair>::Temp& cutPairs ) const;

	void resetClearanceForCutFaces( const hkArrayBase<hkaiRuntimeIndex>& updatedSections, const hkArrayBase<hkaiPackedKey>& cutFaceKeys  );

		/// Make a persistent record to the given (possibly cut) face, which can be resolved to the equivalent face
		/// even after face re-cuts move it to a different place, as long as the original face is not itself re-cut.
	void makePersistentFaceKey(hkaiPackedKey key, hkaiPersistentFaceKey & persistentFaceKeyOut) const;
	
	/// Make a persistent record to the given edge (which may belong to a cut face), which can be resolved to the 
	/// equivalent edge even after face re-cuts move it to a different place, as long as neither face of the edge
	/// is itself re-cut.
	void makePersistentEdgeKey(hkaiPackedKey edgeKey, hkaiPackedKey faceKey, hkaiPersistentEdgeKey & persistentEdgeKeyOut) const;

		/// Resolve a previously created persistent face key to its current face key. This face key is valid until
		/// the next cutting operation.
	hkaiPackedKey resolvePersistentFaceKey(hkaiPersistentFaceKey const& persistentFaceKey) const;

	/// Resolve a previously created persistent edge key to its current edge key. This edge key is valid until
	/// the next cutting operation.
	hkaiPackedKey resolvePersistentEdgeKey(hkaiPersistentEdgeKey const& persistentEdgeKey) const;

		/// Static version of makePersistentFaceKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key is assumed to be un-cut.
	static void makePersistentFaceKey(hkaiNavMeshCutter const* cutter, hkaiPackedKey key, hkaiPersistentFaceKey & persistentFaceKeyOut);

	/// Static version of makePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
	/// key is assumed to be un-cut.
	static void makePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPackedKey edgeKey, hkaiPackedKey faceKey, hkaiPersistentEdgeKey & persistentEdgeKeyOut);

		/// Static version of resolvePersistentFaceKey which allows the cutter to be NULL. If the cutter is NULL, the
		/// key must be un-cut.
	static hkaiPackedKey resolvePersistentFaceKey(hkaiNavMeshCutter const* cutter, hkaiPersistentFaceKey const& persistentFaceKey);

	/// Static version of resolvePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
	/// key must be un-cut.
	static hkaiPackedKey resolvePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPersistentEdgeKey const& persistentEdgeKey);

	/// Static version of resolvePersistentEdgeKey which allows the cutter to be NULL. If the cutter is NULL, the
	/// key must be un-cut. Additionally, returns the face key.
	static hkaiPackedKey resolvePersistentEdgeKey(hkaiNavMeshCutter const* cutter, hkaiGeneralAccessor const& accessor, hkaiPersistentEdgeKey const& persistentEdgeKey, hkaiPackedKey & faceKeyOut);

protected:

	hkBool32 isValid() const;
	void removeCutFacesFromOriginals( const hkArrayBase<hkaiRuntimeIndex>& sections, const hkArrayBase<hkaiPackedKey>& updatedFaceKeys, hkaiNavMeshUtils::RemoveOwnedFacesMode rofMode );
	void removeUnusedCutEdges( const hkArrayBase<hkaiRuntimeIndex>& cutSections );
	void applyCutResults( const hkArrayBase<hkaiRuntimeIndex>& cutSections, hkArray<hkaiFaceCutResults>::Temp& results, hkArray<hkaiPackedKey>::Temp& cutFaces );
	void matchStreamingFaces( const hkArrayBase<hkaiRuntimeIndex>& cutSections, const hkSet<hkaiPackedKey>::Temp& cutFaces );
	void intersectMeshWithSilhouettes( const hkArrayBase<hkaiPackedKey>& faceKeys, hkaiNavMeshCutter::SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams, hkArrayBase<hkaiFaceCutResults>& results);
	void intersectMeshWithSilhouettesMT( const hkArrayBase<hkaiPackedKey>& faceKeys, hkaiNavMeshCutter::SilhouetteSelector* selector, const struct hkaiSilhouetteGenerationParameters& genParams, class hkJobQueue* jobQueue, class hkJobThreadPool* threadPool, hkArrayBase<hkaiFaceCutResults>& results );
	hkBool32 doEdgesMatch( hkaiPackedKey keyA, hkaiPackedKey keyB ) const;

	void calcNewConnectivity( const hkArrayBase<hkaiPackedKey>& uncutFaceKeys, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, SavedConnectivity& tempConnectivity, hkArray<hkaiPackedKey>::Temp& forceRecutFaceKeys );
	
		/// Removes and restores OriginalEdgeInfo's for faces in the updatedFaceKeys array
	static void HK_CALL restoreConnectivity( hkaiStreamingCollection* collection, const hkArrayBase<hkaiPackedKey>& updatedFaceKeys, SavedConnectivity& connectivity);

		/// Figure out faces which to reset, using the dynamic mediator
	void getFacesForResetByMediator ( hkSet<hkaiPackedKey>& facesToReset, const hkArrayBase<hkAabb>& faceAabbs ) const;
	void getFacesForResetByFloodFill( hkSet<hkaiPackedKey>& facesToReset, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, const hkArrayBase<hkAabb>& faceAabbs ) const;

		// Called during loading to set the face flags and force-cut the face.
	void setFaceAsStreaming( hkaiPackedKey faceKey );


public:
		/// Cutting information for a nav mesh. One MeshInfo is created for each nav mesh that's loaded in the hkaiNavMeshManager.
	struct MeshInfo
	{
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, MeshInfo);

		MeshInfo() {}

		int m_originalNumFaces;
		int m_originalNumEdges;
		int m_originalNumVertices; 

		/// A bidirectional mapping between original faces and cut faces.
		/// 
		/// For indices between 0 and m_originalNumFaces, each element i
		/// either -1, indicating the face has not been cut, or the index of
		/// the first cut face corresponding to this original face.
		/// 
		/// For indices greater than or equal to m_originalNumFaces, each
		/// element holds the index of the original face corresponding to the
		/// cut face.
		hkArray<int> m_magic;

		MeshInfo(hkFinishLoadedObjectFlag f)
			: m_magic(f)
		{
		}
		void init(const hkaiNavMeshInstance* meshInstance);

			/// Get the number of faces before any cuts.
		int getOriginalNumFaces() const { return m_originalNumFaces; }

			/// Get the number of edges before any cuts.
		int getOriginalNumEdges() const { return m_originalNumEdges; }

			/// Get the number of vertices before any cuts.
		int getOriginalNumVertices() const { return m_originalNumVertices; }

		hkBool32 isValid(const hkaiNavMeshInstance*, const SavedConnectivity& connectivityInfo) const;
	};

protected:


	hkArray<struct MeshInfo> m_meshInfos;
	
 		/// saved version of modified edges
	struct SavedConnectivity m_connectivityInfo; 
	hkRefPtr<hkaiStreamingCollection> m_streamingCollection;

		/// List of faces which have to be recut next frame
	hkArray<hkaiPackedKey> m_forceRecutFaceKeys;

public:
		/// Used during edge matching
	hkVector4 m_up; 

		/// Edge matching parameters. They should be set as least as large as the ones used in nav mesh generation.
	struct hkaiNavMeshGenerationSettings::EdgeMatchingParameters m_edgeMatchParams;

		/// Tolerance used to determine when a pair of edges can be easily matched with each other.
	hkReal m_cutEdgeTolerance; //+default(1e-4f)

		/// Cut edges below this length will not be considered for matching (and thus left blocked).
		/// You can safely set this to the minimum character width.
	hkReal m_minEdgeMatchingLength; //+default(.01f)

		/// Whether or not to do nav mesh validation during cutting.
		/// These are always disabled during release builds, but potentially slow in debug builds
	hkBool m_performValidationChecks; //+default(true)

		/// How to reset the clearance cache after cutting.
	hkEnum<ClearanceResetMethod, hkUint8> m_clearanceResetMethod; //+default(hkaiNavMeshCutter::CLEARANCE_RESET_ALL)
};

	/// Utility methods for hkaiNavMeshCutter.
class hkaiNavMeshCutterHelper
{
public:
		/// Face patching
	static void HK_CALL connectCutFaceToUncutFace(hkaiStreamingCollection* collection, hkaiNavMeshCutter::SavedConnectivity& connectivityInfo,
		hkaiPackedKey uncutEdgeKey, hkaiPackedKey originalCutEdgeKey,
		const hkArrayBase<hkaiNavMeshCutter::FaceEdgePair>& cutPairs);

		/// Makes a copy of the original face, and makes the new face "cut" from the original
	static void HK_CALL replaceFaceWithCutFace(hkaiStreamingCollection* collection, hkaiPackedKey faceKey, hkaiPackedKey skipEdgeKey, hkaiNavMeshCutter::SavedConnectivity& connectivityInfo,
		hkArray<int>& magic, hkaiNavMeshCutter::EdgeMap& edgeMap);

		/// Returns the sum of the projected distances between the edge endpoints
	static hkSimdReal HK_CALL edgeEndpointDistance(const hkaiNavMeshInstance& meshA, const hkaiNavMeshInstance& meshB, hkVector4Parameter up, hkaiNavMesh::EdgeIndex edgeA, hkaiNavMesh::EdgeIndex edgeB);

		/// Merge contiguous boundary edges on the cut face that came from the same original edge
	static void HK_CALL mergeContiguousBoundaryEdges( hkaiStreamingCollection* collection, const hkaiNavMesh::Face& originalFace, hkaiPackedKey originalEdgeKey, hkaiNavMesh::FaceIndex cutFaceIndex );

};

#endif // HK_NAVMESH_CUTTER_H

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
