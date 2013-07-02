/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SILHOUETTE_GENERATOR_H
#define HK_SILHOUETTE_GENERATOR_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiConvexSilhouette.h>
#include <Ai/Pathfinding/NavMesh/hkaiNavMeshInstance.h>

struct hkaiSilhouetteGenerationParameters;

/// Abstract base class for something which generates silhouettes. Also hold pointer to the (possibly shared) compound silhouette it is part of.
class hkaiSilhouetteGenerator : public hkReferencedObject
{
	public:

		// +version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		enum GeneratorType
		{
				/// Previously used by hkaiPhysics2012BodySilhouetteGenerator, but now this inherits from hkaiPointCloudSilhouetteGenerator.
			GENERATOR_PHYSICS_2012_BODY_REMOVED = 0, 

				/// Used by hkaiPointCloudSilhouetteGenerator
			GENERATOR_POINT_CLOUD, 

				/// Previously used by hkaiStoragePointCloudSilhouetteGenerator, but this class was removed.
			GENERATOR_STORAGE_POINT_CLOUD_REMOVED,

				/// Reserved for user silhouette generator types.
			GENERATOR_USER,

				/// Invalid for silhouette generator types.
			GENERATOR_MAX,
		};

		enum
		{
			
			
			//> Review
			MAX_SIZE_FOR_SPU = 256
		};

		enum
		{
				/// Material index to indicate that the silhouette is cutting (as opposed to painting).
			CUTTING_MATERIAL = -1,
		};


		enum ForceGenerateOntoPpuReasons
		{
			/// Requested by user
			FORCE_PPU_USER_REQUEST = 8,	

			/// Requested by complexity of silhouette(s)
			FORCE_PPU_SILHOUETTE_COMPLEXITY_REQUEST = 16,	
		};
		
			/// Constructor
		hkaiSilhouetteGenerator( GeneratorType type /*= GENERATOR_USER */);
		hkaiSilhouetteGenerator( hkFinishLoadedObjectFlag f);

			/// Destructor
		virtual ~hkaiSilhouetteGenerator();

#ifndef HK_PLATFORM_SPU
			/// Abstract interface for generating child silhouettes.
			/// This is called during single-threaded silhouette generation and gives the generator an opportunity to cache its results.
			/// If this function is called again with the same value for localTransform, the cached results will be returned
			/// \param localTransform Transform from the generator's local space into the desired space
		virtual const hkaiConvexSilhouetteSet* generateSilhouettesCached( const hkQTransform& localTransform, const hkaiSilhouetteGenerationParameters& params ) = 0;

			/// Abstract interface for invalidating cached silhouettes.
			/// This is only called by the world if silhouette generation parameters are changed.
		virtual void invalidateCachedSilhouettes() = 0;
#endif
			/// Abstract interface for generating child silhouettes.
			/// This is called during multi-threaded silhouette generation and MUST be threadsafe.
			/// \param localTransform Transform from the generator's local space into the desired space
		virtual void generateSilhouettesImmediate( const hkQTransform& localTransform, const hkaiSilhouetteGenerationParameters& params, hkArrayBase<hkVector4>& vertices, hkArrayBase<int>& sizes ) const = 0;

			/// Called internally each frame before isDirty() and willGenerateSilhouettes() are checked.
			/// Method is called automatically during silhouette updating, and therefore does not need to be called by the user each frame.
		virtual void update( hkVector4Parameter up ) = 0;
		
			/// Abstract interface for the (3d) AABB of the generator which is used during the merging process to accelerate intersection queries.
			/// \param localTransform Transform from the generator's local space into the desired space
		virtual void getAabb(const hkQTransform& localTransform, hkAabb& aabbOut) const = 0;

			/// Return a 3D bounding box of the local-space silhouette shape.
			/// This data is used to evaluate motion thresholds on moving silhouettes to determine when the overlapping faces need to be updated.
		virtual void getLocalAabb( hkAabb& aabbOut ) const = 0;

			/// Whether or not the generated silhouette has changed and relevant faces need to be cut
		virtual bool isDirty() const = 0;

			/// Whether or not the silhouette from this generator should be considered this frame on the given nav mesh section.
			/// This can return different results from isDirty().
			/// For example, if the generator doesn't generate silhouettes when it moves over a certain relative velocity,
			/// and the generator accelerates rapidly, willGenerateSilhouettes will return false,
			/// but isDirty() should return true (to make sure the nav mesh gets un-cut).
		virtual bool willGenerateSilhouettes( const hkaiNavMeshAccessor& meshInstance, hkVector4Parameter up ) const;

			/// Move the silhouette generator by the specified translation. This is used for recentering the world.
		virtual void shiftWorldSpace( hkVector4Parameter shift );

			/// Returns the size of the generator. Necessary for DMA transfers to SPU.
		virtual int getSize() const = 0;

			/// Whether or not m_cachedSilhouettes are valid for the given transform
		hkBool32 canReuseCachedSilhouettes( const hkQTransform& localTransform, hkVector4Parameter referenceUp ) const;

		//
		// Utility methods for determining motion relative to a nav mesh instance
		//
			/// Computes the sum of the rotational and angular displacements.
		hkSimdReal getDisplacementRelativeToSection( const hkQTransform& prevTransform, const hkQTransform& currTransform ) const;

			/// Checks whether the displacement is greater than m_lazyRecomputeDisplacementThreshold.
			/// Equivalent to getDisplacementRelativeToSection() > getDisplacementRelativeToSection,
			/// but avoids the expensive rotational computation if possible.
		hkBool32 hasMovedAgainstSection( const hkQTransform& prevTransform, const hkQTransform& currTransform ) const ;

			/// PlayStation(R)3 only: Interface for determining whether the generator is allowed to be run on SPU.
		hkBool32 canRunOnSpu() const;
			/// PlayStation(R)3 only: Set whether the generator is allowed to be run on SPU.
		hkUint8 getForceGenerateOnPpuReasons() const;
			/// PlayStation(R)3 only: Set whether the generator is allowed to be run on SPU.
		void setForceGenerateOnPpuReasons(hkUint8 c);

			/// Get the type of the generator
		inline GeneratorType getType() const;

			/// Get the user data
		inline hkUlong getUserData() const;

			/// Set the user data
		inline void setUserData( hkUlong data );

			/// Get the epsilon value for lazy updates
		inline hkReal getLazyRecomputeDisplacementThreshold() const;

			/// Set the epsilon value for lazy updates
		inline void setLazyRecomputeDisplacementThreshold( hkReal lazyRecomputeDisplacementThreshold );

			/// Returns a transform that puts the local-space silhouette shape into the frame of reference of the given nav mesh section.
			/// This data is used to evaluate motion thresholds on moving silhouettes to determine when the overlapping faces need to be updated.
		inline void _getRelativeTransform( const hkQTransform& meshInstanceTransform, hkQTransform& relativeTransform ) const;

			/// Returns the material ID used for faces during cutting.
		inline int getMaterialId() const { return m_materialId; }

			/// Set the material ID used for faces during cutting. 
			/// If materialId == CUTTING_MATERIAL (the default value), the silhouette will cut instead of applying the material
		inline void setMaterialId( int material ) { m_materialId = material; }

		enum ExpansionFlags
		{
				/// No expansion
			AABB_EXPAND_NONE = 0, 

				/// Expand the AABB downward by the extrusion amount
			AABB_EXTRUDE_DOWN = 1,

				/// Expand the AABB in the plane perpendicular to the up direction
			AABB_EXPAND_PLANAR = 2,

				/// Expand the AABB in all directions. This is generally only done for wall climbing meshes.
			AABB_EXPAND_UNIFORM = 4,
		};
			/// Utility method for handling the expansion of the silhouette AABB
		static void HK_CALL expandAabb( hkAabb& aabbInOut, hkVector4Parameter up, hkSimdRealParameter extrusion, hkSimdRealParameter planarExpansion, hkUint32 expansionFlags );

	protected:
			/// User data
		hkUlong m_userData; //+default(0)

			/// Threshold used for TIM calculation. Must be the same as the expansion radius used during silhouette generation.
		hkReal m_lazyRecomputeDisplacementThreshold;	//+default(0.1f)

			/// Type
		hkEnum<GeneratorType, hkUint8> m_type;

			/// PlayStation(R)3 only: If this flag is non zero, cutting for all nav mesh faces under this generator will run on the PPU only
		hkUint8   m_forceGenerateOntoPpu;

			/// Face data for new faces. If this is CUTTING_MATERIAL, no faces will be generated under the silhouette.
		int		m_materialId; //+default(hkaiSilhouetteGenerator::CUTTING_MATERIAL)

			/// Cached silhouette (in space defined by m_cachedSilhouettesTransform)
		hkRefPtr<hkaiConvexSilhouetteSet> m_cachedSilhouettes;

			/// Current world-space transform of the silhouette generator.
			/// Updating this transform is dependent on the implementation:
			/// * hkaiPhysicsBodySilhouetteGeneratorBase modifies the transform during update() based on the rigid body.
			/// * hkaiPointCloudSilhouetteGenerator requires the user to update the transform.
		hkQTransform m_transform;
};

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/hkaiSilhouetteGenerator.inl>

//
// Static utility functions to enable same code on CPU/SPU
// On CPU, these forward directly to the virtual function
// On SPU, they switch based on the type
typedef void (HK_CALL *hkaiGenerateSilhouettesFunc)(const hkaiSilhouetteGenerator* gen, const hkQTransform& localTransform, const hkaiSilhouetteGenerationParameters& params, hkArrayBase<hkVector4>& vertices, hkArrayBase<int>& sizes);
typedef void (HK_CALL *hkaiGetAabbFunc)(const hkaiSilhouetteGenerator* gen, const hkQTransform& localTransform, hkAabb& aabbOut );

#endif // HK_SILHOUETTE_GENERATOR_H

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
