/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_VIEWER_H
#define HKAI_NAV_MESH_VIEWER_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>
#include <Ai/Visualize/VisualDebugger/Viewer/hkaiViewerBase.h>
#include <Ai/Visualize/Debug/hkaiNavMeshDebugUtils.h>

class hkProcessContext;
class hkaiViewerContext;
class hkaiNavMesh;

/// Base class for nav mesh viewers.
class hkaiNavMeshBaseViewer : 	public hkaiViewerBase,
								public hkaiWorld::Listener
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_VDB);

		hkaiNavMeshBaseViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshBaseViewer();


		//
		// hkProcess interface
		//
		void init();

			/// Returns the tag associated with this viewer type.
		virtual int getProcessTag() = 0;

		//
		// hkaiWorld::Listener interface
		//

			/// Update function that draws the mesh.
		virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) HK_OVERRIDE;

		//
		// hkaiViewerContextWorldListener interface
		//
		virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE;
		virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;

	public:

			/// Settings that describe how nav meshes should be drawn.
		hkaiNavMeshDebugUtils::DebugInfo m_settings;

};

/// Viewer to display the edges and faces of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshGeometryViewer : public hkaiNavMeshBaseViewer
{
	public:
	
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

		enum
		{
				/// The number of faces to consider a block.
				/// If one face in the block is cut, the geometry for all the rest is recomputed.
			BLOCK_SIZE = 1024,

				/// Total number of display geometries per block.
				/// One solid geometry, plus four wire frames: shared, boundary, external and retriangulated edges.
			NUM_GEOMS_PER_BLOCK = 5,
		};

			/// Registers hkaiNavMeshGeometryViewer with the hkViewerFactory.
		static void HK_CALL registerViewer();

			/// Returns the viewer name. This text appears in the VDB client viewer menu.
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Geometry"; }

			/// Creates a hkaiNavMeshGeometryViewer.
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

			/// Returns the tag associated with this viewer type.
			/// This is passed to the drawing calls so that the VDB knows from which viewer those calls came.
		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

		virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;

		//
		// hkaiWorld::Listener interface
		//
			/// Update function that manages the display geometries associated with nav meshes.
		virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) HK_OVERRIDE;

			/// Callback raised whenever a nav mesh is changed. Display geometries need to updated when this happens.
		virtual void dynamicNavMeshModifiedCallback( hkaiWorld::NavMeshModifiedCallbackContext& context ) HK_OVERRIDE;

			// Callback fired from the world when a mesh is added.
		virtual void navMeshInstanceAdded( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, const hkaiNavMeshQueryMediator* mediator, hkaiDirectedGraphInstance* hierarchyGraph ) HK_OVERRIDE;

			// Callback fired from the world when a mesh is added.
		virtual void navMeshInstanceRemoved( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, hkaiDirectedGraphInstance* hierarchyGraph  ) HK_OVERRIDE;


	protected:

		hkaiNavMeshGeometryViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshGeometryViewer();

			/// Builds and adds a display geometry for the provided nav mesh.
		void addNavMesh(const hkaiNavMeshInstance* navMesh, const hkaiWorld* world);

			/// Removes the display geometry associated with a particular nav mesh.
		void removeNavMesh(const hkaiNavMeshInstance* navMesh);

	protected:

			/// Add solid and wireframe geometry for a set of faces.
		void addFaceGeometry( const hkaiNavMeshInstance* navMeshInstance, const hkaiWorld* world, hkaiNavMesh::FaceIndex startFaceIndex, int numFaces, hkUlong idBase);

			/// Remove all geometries for a block.
		void removeGeometryForBlock( hkUlong baseId );

			/// Update the transform on all geometries in a block.
		void updateGeometryForBlock( hkUlong baseId, const hkTransform& transform );

			/// Get a hash of the face flags in a block. Used to determine whether or not a block needs to get udpated.
		static hkUint32 HK_CALL getFlagBlockCrc( const hkaiNavMeshInstance* instance, hkaiNavMesh::FaceIndex startFaceIndex, int numFaces );

			/// Find the index in m_navMeshInfos for the given hkaiNavMeshInstance, or -1 if it's not found.
		int indexOfInstance( const hkaiNavMeshInstance* instance ) const;
		
		struct OriginalFaceBlock
		{
				/// The start face index of the block
			hkaiNavMesh::FaceIndex m_blockStart;

				/// The number of faces in the block.
			int m_blockSize;

				/// The base ID used for the m_displayHandler. The IDs used are [m_idBase, m_idBase+NUM_GEOMS_PER_BLOCK).
				/// For original faces, the base ID is the address of it's entry in hkaiNavMesh::m_faceMap, 
				/// since this is static and not shared between nav meshes.
				/// For owned faces, this is the address of the first owned face.
			hkUlong m_idBase;

				/// A hash of the face flags, computed from getFlagBlockCrc.
			hkUint32 m_flagHash;
		};


			/// Internal bookkeeping information
		struct NavMeshInfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshGeometryViewer::NavMeshInfo);
			const hkaiNavMeshInstance*	m_navMeshInstance;
			const hkaiWorld*			m_world;

			hkArray<OriginalFaceBlock> m_originalFaceBlocks;
			
				/// See OriginalFaceBlock::m_idBase
			hkUlong m_ownedFaceIdBase;
		};

			/// Information pertaining to nav meshes for which display geometries have been added.
		hkArray<NavMeshInfo> m_navMeshInfos;

		static int m_tag;
};

/// Viewer to display the vertex normals of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshVertexNormalsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Vertex Normals"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:

		hkaiNavMeshVertexNormalsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshVertexNormalsViewer() {}

	protected:

		static int m_tag;
};

/// Viewer to display the edge normals of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshEdgeNormalsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Edge Normals"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:

		hkaiNavMeshEdgeNormalsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshEdgeNormalsViewer() {}

	protected:

		static int m_tag;
};

/// Viewer to display the face normals of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshFaceNormalsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Face Normals"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:
	
		hkaiNavMeshFaceNormalsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshFaceNormalsViewer() {}

	protected:

		static int m_tag;
};

/// Viewer to display the vertex labels of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshVertexLabelsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Vertex Labels"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:

		hkaiNavMeshVertexLabelsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshVertexLabelsViewer() {}

	protected:

		static int m_tag;
};

/// Viewer to display the edge labels of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshEdgeLabelsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Edge Labels"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:

		hkaiNavMeshEdgeLabelsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshEdgeLabelsViewer() {}

	protected:

		static int m_tag;
};

/// Viewer to display the face labels of hkaiNavMesh objects associated with AI worlds.
class hkaiNavMeshFaceLabelsViewer : public hkaiNavMeshBaseViewer
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);
		static void HK_CALL registerViewer();
		static inline const char* HK_CALL getName() { return "AI - Nav Mesh Face Labels"; }
		static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

		virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	protected:

		hkaiNavMeshFaceLabelsViewer(const hkArray<hkProcessContext*>& contexts);
		virtual ~hkaiNavMeshFaceLabelsViewer() {}

	protected:

		static int m_tag;
};

#endif  // HKAI_NAV_MESH_VIEWER_H

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
