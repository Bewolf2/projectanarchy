/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_PRIORITY_CONTROLLER
#define HK_AI_PRIORITY_CONTROLLER

class hkaiSilhouetteGenerator;
class hkAabb;
struct hkaiSilhouetteGeneratorSectionContext;

	/// A class that customization of silhouette behavior in hkaiOverlapManager.
	/// If no controller is specified, an hkaiDefaultSilhouettePriorityController will be used.
class hkaiSilhouettePriorityController : public hkReferencedObject
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

		virtual ~hkaiSilhouettePriorityController() {}

		//
		// Silhouette priority for face cutting.
		// The methods are used to determine which faces to cut each frame.
		//

		virtual hkReal getGeneratorAddedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up ) const = 0;
		virtual hkReal getGeneratorRemovedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up ) const = 0;
		virtual hkReal getGeneratorMovedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up,
				const hkQTransform& previousRelativeTransform, const hkQTransform& currentRelativeTransform) const = 0;

			/// Sort the generators on a given face from highest importance to lowest.
			/// This is needed when there are more than hkaiNavMeshCutFaceJob::MAX_MATERIALS distinct silhouette materials.
			/// In this case, the lowest-priority materials will be used as cutting instead of painting silhouettes.
			/// Also, the order of contexts may change even when no generators move, so some spatial information should
			/// be used as a sorting criterion to avoid flickering in the triangulation.
		virtual void sortSilhouetteGenerators( hkArrayBase<const hkaiSilhouetteGeneratorSectionContext*>& contexts ) const = 0;
};

class hkaiDefaultSilhouettePriorityController : public hkaiSilhouettePriorityController
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH);

		virtual hkReal getGeneratorAddedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up ) const HK_OVERRIDE;
		virtual hkReal getGeneratorRemovedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up ) const HK_OVERRIDE;
		virtual hkReal getGeneratorMovedPriority( const hkaiSilhouetteGenerator* gen, const hkAabb& aabb, hkVector4Parameter up,
			const hkQTransform& previousRelativeTransform, const hkQTransform& currentRelativeTransform) const HK_OVERRIDE;

			/// Sort by increasing material index, so lower materials are given higher priority.
			/// For equal materials, the x component of the position is used as a tie-breaker.
		virtual void sortSilhouetteGenerators( hkArrayBase<const hkaiSilhouetteGeneratorSectionContext*>& contexts ) const HK_OVERRIDE;

		static hkSimdReal HK_CALL getProjectedAabbArea( const hkAabb& aabb, hkVector4Parameter up );
};

#endif // HK_AI_OVERLAP_MANAGER

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
