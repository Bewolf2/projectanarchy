/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_NAV_MESH_PATH_SMOOTHING_UTIL_H
#define HKAI_NAV_MESH_PATH_SMOOTHING_UTIL_H

#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/NavMesh/Smoothing/hkaiStringPulling.h>
#include <Ai/Pathfinding/Utilities/PathWriter/hkaiPathWriter.h>

//Uncomment this and recompile to enable extra debug drawing.
//#define HKAI_DEBUG_SMOOTHING


	/// Utility for smoothing out A* search results.
class hkaiNavMeshPathSmoothingUtil
{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiNavMeshPathSmoothingUtil);
		typedef hkaiStringPulling::Vertex Vertex;
		typedef hkaiStringPulling::Side Side;
		typedef hkaiStringPulling::Funnel Funnel;


			/// Computes the smooth path that passes through the array of edges.
			/// If useSimpleSmoothing is true, findSimpleSmoothPath is used, otherwise a "full" smooth path is computed,
			/// avoiding obstacles by the character radius.
		static hkBool32 HK_CALL findSmoothedPathMain(const hkaiStreamingCollection::InstanceInfo* sectionInfo, 
			const hkArray<hkaiPackedKey>& visitedEdges, 
			hkaiPackedKey startFaceKey,
			hkVector4Parameter startPoint, 
			hkVector4Parameter endPoint, 
			hkBool32 project,
			hkBool32 computeNormals,
			hkVector4Parameter up, 
			hkReal radius, 
			bool useSimpleSmoothing,
			hkArray<hkaiPath::PathPoint>* pathArrayOut, 
			hkaiPath::PathPoint* pathPtrOut, 
			int maxPathLength, 
			hkSimdReal* stringPulledLength);

		static hkBool32 HK_CALL projectStraightPath(const hkaiStreamingCollection::InstanceInfo* sectionInfo, 
			const hkArray<hkaiPackedKey>& visitedEdges, 
			hkaiPackedKey startFaceKey,
			hkVector4Parameter startPoint, 
			hkVector4Parameter endPoint, 
			hkBool32 project,
			hkBool32 computeNormals,
			hkVector4Parameter up, 
			hkReal radius, 
			hkArray<hkaiPath::PathPoint>* pathArrayOut, 
			hkaiPath::PathPoint* pathPtrOut, 
			int maxPathLength);

			/// Computes a fast smoothed path. This assumes that the character has negligible radius.
			/// The path is ouput to the m_pathArrayOut or m_pathPtrOut, whichever is valid.
		hkBool32 findSimpleSmoothPath( hkBool32 computeNormals );

#ifdef HKAI_DEBUG_SMOOTHING

		//
		// Drawing methods. These have empty bodies unless HKAI_DEBUG_SMOOTHING is defined.
		//
		static hkBool32 m_debuggingOn;
		void drawInput(const hkaiStringPulling& puller, bool project);
		void drawFace(hkaiPackedKey faceKey);
		void showDebug(const hkaiStringPulling& puller, int i, Side side, hkReal heightOffset, 
			const Vertex& start, const Vertex& end, const char* description);
		void drawMeshCells(hkReal heightOffset);
		void drawCircles(const hkaiStringPulling& puller, bool project);
		void showDisc(const hkVector4& pos, hkReal radius, hkColor::Argb color, bool fill = false);
		void showDiscProjected(const hkVector4& pos, hkReal radius, hkColor::Argb color, bool fill = false);
		void showClosestPointsCheck(const Vertex& v0, const Vertex& v1);
		static void highlightNavMeshFaceByFace(const hkaiNavMesh& mesh, 
			hkaiNavMesh::FaceIndex faceIndex, hkColor::Argb highlightColor, const hkVector4& up, hkReal upOffset);
#endif

	private:

		hkaiNavMeshPathSmoothingUtil(class hkaiGeneralAccessor& accessor, 
			const hkArray<hkaiPackedKey>& visitedEdges, 
			hkaiPackedKey startFaceKey,
			hkVector4Parameter startPoint, 
			hkVector4Parameter endPoint, 
			hkVector4Parameter up, 
			hkReal radius, 
			hkaiPathWriter& writer);
		
	private:

		class hkaiGeneralAccessor& m_accessor;
		const hkArray<hkaiPackedKey>& m_visitedEdges;
		const hkVector4 m_startPoint;
		const hkVector4 m_endPoint;
		const hkVector4 m_up;
		const hkaiPackedKey m_startFaceKey;
		hkReal m_radius;

		hkaiPathWriter& m_writer;
};


#endif // HKAI_NAV_MESH_PATH_SMOOTHING_UTIL_H

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
