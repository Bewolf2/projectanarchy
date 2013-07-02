/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_ABSTRACT_FACE_CUTTER_H
#define HK_ABSTRACT_FACE_CUTTER_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>

struct hkaiFaceCutResults;

	/// Wrapper for the internal triangulator.
	/// This applies the pImpl idiom to get around header restrictions.
class hkaiAbstractFaceCutter
{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiAbstractFaceCutter);

#ifdef HK_PLATFORM_SPU
		typedef hkaiSpuNavMeshAccessor MeshType;
#else
		typedef hkaiNavMeshInstance MeshType;
#endif

		void reset(hkaiPackedKey faceKey);

		void setFace(const MeshType& origMesh, hkVector4Parameter worldUp, bool isWallClimbing, hkUint32 faceKey,
			hkSimdRealParameter regionExpansion, hkArrayBase<hkVector4>& faceVertsOut, hkArrayBase<int>& indexRemap);

		hkBool32 addSilhouette( const hkVector4* points, int loopSize, int materialId );

		hkBool32 computeConvexDecomposition(hkaiFaceCutResults* results, 
			const hkArrayBase<hkVector4>& faceVertices, hkArrayBase<int>& indexRemap);

		const hkVector4& getUp() const;

		hkaiAbstractFaceCutter();
		~hkaiAbstractFaceCutter();

	protected:
		hkLocalArray<char> m_buffer;
};

#endif // HK_ABSTRACT_FACE_CUTTER_H

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
