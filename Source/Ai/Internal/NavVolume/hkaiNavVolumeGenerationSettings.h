/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_NAV_VOLUME_GENERATION_SETTINGS_H
#define HKAI_NAV_VOLUME_GENERATION_SETTINGS_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/Common/hkaiVolume.h>
#include <Ai/Pathfinding/Common/hkaiMaterialPainter.h>

class hkaiMaterialPainter;
class hkaiVolume;

class hkaiNavVolumeChunkDomainOverrideCallback;


/// This input structure controls all of the major settings for automatic nav volume generation
struct hkaiNavVolumeGenerationSettings
{
	// +version(8)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiNavVolumeGenerationSettings);

	/// Flags to determine whether the triangles for a material are walkable, extruded, etc.
	enum MaterialFlagsBits
	{
			/// Indicates that triangles of the specified material will be ignored for the purposes of nav volume generation.
		MATERIAL_NONE = 0,

			/// If this bit is specified for a material, the triangles with the material will be extruded and subtracted from the navigable space.
		MATERIAL_BLOCKING = 1,

			/// Default type - unless otherwise specified, all materials are BLOCKING.
		MATERIAL_DEFAULT = MATERIAL_BLOCKING,
	};

	typedef hkFlags<MaterialFlagsBits, hkUint32> ConstructionFlags;

		/// Storage for material properties. All triangles with the specified index will be treated with the specified flags.
	struct MaterialConstructionInfo
	{
		//+version(2)
		MaterialConstructionInfo();
		MaterialConstructionInfo(hkFinishLoadedObjectFlag f);

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, MaterialConstructionInfo);
		HK_DECLARE_REFLECTION();

			/// Material ID
		int m_materialIndex; //+default(-1)

			/// Flags to be applied to the material.
		ConstructionFlags m_flags; //+default(hkaiNavVolumeGenerationSettings::MATERIAL_DEFAULT)

			/// Desired resolution, as a multiple of the cellWidth.
			/// This allows certain triangles to be specified as lower detail, which can greatly improve build times.
		int m_resolution; //+default(1)

	};


		/// Settings to control how the volume domain is divided into chunks, for memory-efficient voxelization and merging
	struct ChunkSettings
	{
		// +version(0)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, ChunkSettings);

		enum
		{
			DEFAULT_MAX_CHUNK_SIZE = 256,	///< Default value used to limit chunk sizes in the X, Y and Z dimensions
		};

		ChunkSettings();
		ChunkSettings(hkFinishLoadedObjectFlag f) {}

			/// Maximum size of a chunk (in cells) along the X axis. If zero, the domain will not be divided into chunks.
		hkUint16 m_maxChunkSizeX;	//+default(hkaiNavVolumeGenerationSettings::ChunkSettings::DEFAULT_MAX_CHUNK_SIZE) +hk.RangeInt32(absmin=0,absmax=0xffff)
			/// Maximum size of a chunk (in cells) along the Y axis. If zero, the domain will not be divided into chunks.
		hkUint16 m_maxChunkSizeY;	//+default(hkaiNavVolumeGenerationSettings::ChunkSettings::DEFAULT_MAX_CHUNK_SIZE) +hk.RangeInt32(absmin=0,absmax=0xffff)
			/// Maximum size of a chunk (in cells) along the Z axis. If zero, the domain will not be divided into chunks.
		hkUint16 m_maxChunkSizeZ;	//+default(hkaiNavVolumeGenerationSettings::ChunkSettings::DEFAULT_MAX_CHUNK_SIZE) +hk.RangeInt32(absmin=0,absmax=0xffff)
	};

		/// Advanced settings to control how initial cells are merged and split.
	struct MergingSettings
	{
		// +version(3)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, MergingSettings);

		MergingSettings();
		MergingSettings(hkFinishLoadedObjectFlag f) {}

			/// Penalty applied to the number of cells. Increasing this will results may result in a fewer cells in the final structure,
			/// possibly at the expense of more edge connections or more elongated cells.
		hkReal m_nodeWeight; //+default(10.0f)
		
			/// Penalty applied to the number of edges. Increasing this will results may result in a fewer edges in the final structure,
			/// possibly at the expense of more elongated cells or a higher number of cells.
		hkReal m_edgeWeight; //+default(1.0f)

			/// Whether or not to estimate the number of new edges, rather than explicitly calculating them.
			/// Using the estimate can speed up generation times by approximately 30%.
		hkBool m_estimateNewEdges; //+default(true)

			/// Merging terminates if we observe this many consecutive iterations the slope of graph of cell count below m_slopeThreshold below.
			/// Setting this to zero forces zero merging iterations.
		int m_iterationsStabilizationThreshold; //+default(10) 

			/// If slope of graph of cellcount goes below this for more than m_iterationsStabilizationThreshold, we terminate
		hkReal m_slopeThreshold; //+default(1.0f)

			/// User can force termination after only this number of iterations
		int m_maxMergingIterations; //+default(1000000)

		//
		// Advanced settings.
		//

			/// Random seed used internally during merging
		int m_randomSeed;		//+default(123)

			/// Determines the likelihood of a merge being accepted. Advanced use only.
		hkReal m_multiplier;	//+default(1.0f)
								//+hk.RangeReal(absmin=0)

			/// Do a first pass of all simple merges
		hkBool m_useSimpleFirstMergePass;	//+default(true)
	};

	hkaiNavVolumeGenerationSettings();
	hkaiNavVolumeGenerationSettings(hkFinishLoadedObjectFlag f);
	~hkaiNavVolumeGenerationSettings();

	void setConstructionPropertiesForMaterial( int materialIndex, hkUint32 constructionFlags, int resolution = 1 );

		/// The AABB to be used by the final volume.
		/// This defaults to an invalid AABB; if it is unchanged, the geometry's AABB will be used.
	hkAabb	m_volumeAabb;

	
	/// Input horizontal range to validate against (to check for corrupt data)
	hkReal m_maxHorizontalRange;	//+default(10000.0f)
									//+hk.RangeReal(absmin=0)

	/// Input 'up' range to validate against (to check for corrupt data)
	hkReal m_maxVerticalRange;		//+default(1000.0f)
									//+hk.RangeReal(absmin=0)

	/// Checks the input geometry for simple corruption, such as NAN/INF values, or triangles which overlap many other triangles.
	hkBool m_validateInputGeometry; //+default(false)

	/// The up vector for the world
	hkVector4 m_up;					//+default( 0 0 1 );

	/// The height of the character used to extrude the geometry when generating the nav volume.
	hkReal m_characterHeight;		//+default(1.75)
									//+hk.RangeReal(absmin=0,softmax=10)
									//+hk.Description("The character height used to extrude the input geometry.")

	/// The depth of the character used to extrude the geometry when generating the nav volume.
	hkReal m_characterDepth;		//+default(0.0f)
									//+hk.RangeReal(softmin=-10,absmax=0)
									//+hk.Description("The character depth used to extrude the input geometry.")

	/// The width of the character used to extrude the geometry when generating the nav volume.
	hkReal m_characterWidth;		//+default(1.75)
									//+hk.RangeReal(absmin=0,softmax=10)
									//+hk.Description("The character width used to extrude the input geometry.")

	/// The minimum navvolume cell width in units.
	hkReal m_cellWidth;				//+default(1.0f)
									//+hk.RangeReal(absmin=0)

		/// Controls how the domain resolution is computed from the AABB
	enum CellWidthToResolutionRounding
	{
			/// Resolution is rounded down. In pseudocode,
			/// res = (hkUint16) (extents / cellWidth)
		ROUND_TO_ZERO,

			/// Resolution is rounded to the nearest integer. In pseudocode,
			/// res = (hkUint16) (extents / cellWidth)
		ROUND_TO_NEAREST
	};

		/// Controls how the domain resolution is computed from the AABB
	hkEnum<CellWidthToResolutionRounding, hkUint8> m_resolutionRoundingMode; //+default(hkaiNavVolumeGenerationSettings::ROUND_TO_NEAREST)

	/// Settings for dividing the volume domain into chunks
	ChunkSettings m_chunkSettings;

	const hkaiNavVolumeChunkDomainOverrideCallback* m_chunkDomainCallback;	//+nosave
																			//+default(HK_NULL)

	/// The border around the geometry to ensure the voxelization fully encompasses the navigable volume
	hkReal m_border;				//+default(0.01f)
									//+hk.RangeReal(absmin=0)

	/// If true (default), then border cells will be included, otherwise excluded
	hkBool m_useBorderCells;		//+default(true)

	// Settings for merging
	MergingSettings m_mergingSettings;

	/// Any regions with volume less than this threshold will be automatically removed
	hkReal m_minRegionVolume;		//+default(5.0f)
									//+hk.RangeReal(absmin=0)

	/// Minimum acceptable distance to the region seed points (below).
	/// If no face in a region is within the minimum distance to at least one seed point, the region will be removed.
	hkReal m_minDistanceToSeedPoints;	//+default(1.0f)

	/// If points are specified here, any regions not connected to the points will be removed
	hkArray<hkVector4> m_regionSeedPoints;

	/// Default ConstructionFlag
	MaterialConstructionInfo m_defaultConstructionInfo;

	/// Material-to-ConstructionFlags map - determines which properties should be used for which materials.
	/// If the material index is not found in the map, then m_defaultConstructionProperties is used
	hkArray<hkaiNavVolumeGenerationSettings::MaterialConstructionInfo> m_materialMap;

	/// Carver volumes used to remove sections of the input volume - see hkaiVolume for more details
	hkArray< hkRefPtr<const hkaiVolume> > m_carvers;

	/// Painters used to paint sections of the input volume with materials - see hkaiVolume for more details
	hkArray< hkRefPtr<const hkaiMaterialPainter> > m_painters;

	//
	// Debugging Information
	//
	
		/// If this flag is set, the input data (hkGeometry and Settings) will be serialized out.
	hkBool m_saveInputSnapshot; //+default(false)

		/// Base filename of the saved input snapshot.
	hkStringPtr m_snapshotFilename;
};

#endif	// HKAI_NAV_VOLUME_GENERATION_SETTINGS_H

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
