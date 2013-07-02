/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_MOPP_NAV_MESH_MEDIATOR_H
#define HK_AI_MOPP_NAV_MESH_MEDIATOR_H

#include <Ai/Pathfinding/NavMesh/hkaiNavMesh.h>
#include <Ai/Pathfinding/Collide/NavMesh/hkaiNavMeshQueryMediator.h>

class hkpMoppCode;
class hkpMoppCompilerInput;

extern const class hkClass hkaiMoppNavMeshQueryMediatorClass;

	/// This is an implementation of hkaiNavMeshQueryMediator that uses Havok MOPP technology to bound the mesh
class hkaiMoppNavMeshQueryMediator : public hkaiNavMeshQueryMediator
{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

			/// This mediator is constructed from the nave mesh and MOPP code which bounds it. Use the utility method buildCode to generate appropriate code.
		hkaiMoppNavMeshQueryMediator( const hkpMoppCode* code, const hkaiNavMesh* navMesh );

		~hkaiMoppNavMeshQueryMediator();

		hkaiMoppNavMeshQueryMediator(hkFinishLoadedObjectFlag f);

			/// Call this utility method to generate MOPP code for the nav mesh.
		static hkpMoppCode* HK_CALL buildCode(const hkaiNavMesh* navMesh, const hkpMoppCompilerInput& req );

			/// Implementation of hkaiNavMeshQueryMediator interface.
		virtual hkaiPackedKey getClosestPoint( const GetClosestPointInput& input, hkVector4& closestPoint ) const HK_OVERRIDE;

			/// Implementation of hkaiNavMeshQueryMediator interface.
		virtual bool castRay( const RaycastInput& input, HitDetails& hitOut ) const HK_OVERRIDE;

			/// Implementation of hkaiNavMeshQueryMediator interface.
		virtual void queryAabb( const AabbQueryInput& input, hkArray<hkaiPackedKey>::Temp& hits ) const HK_OVERRIDE;

	protected:

		hkRefPtr<const hkpMoppCode>	m_code;
		hkVector4					m_codeInfoCopy;

		hkRefPtr<const hkaiNavMesh>	m_navMesh;
};

#endif // HK_AI_MOPP_NAV_MESH_MEDIATOR_H

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
