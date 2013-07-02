/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_EDGE_GEOMETRY_VALIDATOR_H
#define HKAI_EDGE_GEOMETRY_VALIDATOR_H

#include <Ai/Internal/Boolean/hkaiFaceEdges.h>

	/// Optional class to store the memory used in hkaiEdgeGeometry::checkConsistency()
class hkaiEdgeGeometryValidator
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiEdgeGeometryValidator);
	void clear();

	hkaiFaceEdges m_faceEdges;
	hkArray<int> m_vertexIndexMap;
	hkArray<int> m_faceIndices;   
	hkArray<int> m_indicesCount;
};

#endif //HKAI_EDGE_GEOMETRY_VALIDATOR_H

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
