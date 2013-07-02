/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_POINTCLOUD_SILHOUETTE_GENERATOR_H
#define HK_POINTCLOUD_SILHOUETTE_GENERATOR_H

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/hkaiSilhouetteGenerator.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabbUtil.h>


/// This generator is the base class for one which produces the silhouette of a point cloud (returned by getPoints())
/// This implementation of hkaiPointCloudSilhouetteGenerator stores the 'local' points explicitly and transforms them
/// into world space on demand - The user should update the transform of the cloud as required using setTransform().
class hkaiPointCloudSilhouetteGenerator : public hkaiSilhouetteGenerator
{
	public:

			//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

			/// Level of detail used to generate the silhouette from the specified points.
		enum DetailLevel
		{
				/// Used to indicate that a silhouette shouldn't be created.
			DETAIL_INVALID,

				/// Use the full point set for silhouette generation.
				/// If more than one size is specified, the silhouette will be a union of convex hulls.
			DETAIL_FULL,

				/// Use the local space AABB of the point cloud, transformed by the silhouette generator's transform.
			DETAIL_OBB,

				/// Use the convex hull of all the points. If 0 or 1 sizes are specified, this is the same as DETAIL_FULL.
			DETAIL_CONVEX_HULL,
		};

			/// Constructor
		hkaiPointCloudSilhouetteGenerator();
		hkaiPointCloudSilhouetteGenerator( hkFinishLoadedObjectFlag f);

			/// Destructor
		virtual ~hkaiPointCloudSilhouetteGenerator();

		//
		// hkaiSilhouetteGenerator interface
		//
		
#ifndef HK_PLATFORM_SPU
			/// Generate silhouettes
		virtual const hkaiConvexSilhouetteSet* generateSilhouettesCached( const hkQTransform& localTransform, const hkaiSilhouetteGenerationParameters& params ) HK_OVERRIDE;

		virtual void invalidateCachedSilhouettes() HK_OVERRIDE;
#endif

		virtual void generateSilhouettesImmediate( const hkQTransform& localTransform, const hkaiSilhouetteGenerationParameters& params, hkArrayBase<hkVector4>& vertices, hkArrayBase<int>& sizes ) const HK_OVERRIDE;


			/// Update internal state - Method called automatically during silhouette updating, and therefore does not need to be called by the user each frame.
		virtual void update( hkVector4Parameter up ) HK_OVERRIDE;

			/// Abstract interface for the (3d) AABB of the generator which is used during the merging process to accelerate intersection queries.
		virtual void getAabb(const hkQTransform& localTransform, hkAabb& aabbOut) const HK_OVERRIDE;

			/// Return a 3D bounding box of the local-space silhouette shape.
		virtual void getLocalAabb( hkAabb& aabbOut ) const HK_OVERRIDE;

			/// Whether or not the silhouette from this generator has changed
		virtual bool isDirty() const HK_OVERRIDE;

			/// Whether or not the silhouette from this generator should be considered this frame on the given nav mesh section.
		virtual bool willGenerateSilhouettes( const hkaiNavMeshAccessor& meshInstance, hkVector4Parameter up ) const HK_OVERRIDE;

		virtual int getSize() const HK_OVERRIDE { return sizeof(hkaiPointCloudSilhouetteGenerator); }


		//
		// hkaiPointCloudSilhouetteGenerator-specific methods
		//

		static void HK_CALL registerMtFunctions();

			/// Set enabled
		inline void setEnabled(bool enabled);

			/// Get enabled
		inline bool getEnabled() const;

			/// Copy points to the pointsOutArray, applying the localTransform
		virtual void getPoints(const hkQTransform& localTransform, hkArray<hkVector4>::Temp& pointsOut) const;

			/// Set m_localPoints. The input points are copied to m_localPoints and m_localAabb is updated.
		inline void setLocalPoints(const hkArrayBase<hkVector4>& points);

			/// Set m_localPoints. The input points are copied to m_localPoints and sizes are copied to m_silhouetteSizes, and m_localAabb is updated.
		void setLocalPoints(const hkArrayBase<hkVector4>& points, const hkArray<int>& sizes );

			/// Set m_localPoints to be the corners of the specified AABB.
		void setFromAabb(const hkAabb& aabb);

			/// Set m_localPoints to "borrow" the specified vector data. The data must persist for the lifetime
			/// of the generator, and it is up to the user to deallocate it later.
			/// Internally, this calls hkArrayBase::setDataUserFree. See the notes there for more information.
		inline void setLocalPointDataUserFree( const hkVector4* pointData, int numPoints );

			/// Get local points
		inline const hkArray<hkVector4>& getLocalPoints() const;

			/// Set transform
		inline void setTransform(const hkQTransform& t);

			/// Set transform
		inline const hkQTransform& getTransform() const;

			/// Set weld tolerance
		inline void setWeldTolerance(hkReal weldTolerance);
	
			/// Get weld tolerance
		inline hkReal getWeldTolerance() const;

			/// Returns the detail level of the generator.
		inline DetailLevel getDetailLevel() const;

			/// Sets the desired detail level on a silhouette.
			/// By default, this does NOT force the silhouette to be recut at the new detail level.
			/// If that is desired, call with setDirty set to true.
		inline void setDetailLevel( DetailLevel d, bool setDirty = false );


		inline int getNumChildSilhouettes( ) const;
		inline int getSizeOfChildSilhouette(int i) const;

		enum PointCloudFlagBits
		{
			/// User has called setLocalPoints()
			LOCAL_POINTS_CHANGED = 1,
		};

	protected:
		
			/// AABB of the local-space points
		hkAabb m_localAabb;

			/// Local-space silhouette points
		hkArray<hkVector4>	m_localPoints;

			/// Sizes of the silhouettes. If this is empty, then it is assumed that there's one silhouette using all of the points.
		hkArray<int>	m_silhouetteSizes;

			/// When making the 2D convex hull of the current point cloud, we will weld all vertices within tolerance of each other - this
			/// avoids silhouettes causing insertion of very short edges into the cut dynamic navmesh.
		hkReal m_weldTolerance; //+default(1e-2f)

			/// Whether or not to generate the full shape silhouette
		hkEnum<DetailLevel, hkUint8> m_silhouetteDetailLevel; //+default(hkaiPointCloudSilhouetteGenerator::DETAIL_FULL)

			/// See FlagBits above
		hkFlags<PointCloudFlagBits, hkUint8> m_flags; //+default(hkaiPointCloudSilhouetteGenerator::LOCAL_POINTS_CHANGED)

			/// setLocalPoints called since last update()
		hkBool m_localPointsChanged; //+default(true)

			/// Can enable/disable a generator
		hkBool m_isEnabled; //+default(true)
};

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/PointCloud/hkaiPointCloudSilhouetteGenerator.inl>

#endif // HK_POINTCLOUD_SILHOUETTE_GENERATOR_H

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
