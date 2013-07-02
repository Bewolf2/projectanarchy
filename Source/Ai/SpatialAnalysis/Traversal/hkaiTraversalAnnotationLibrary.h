/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_TRAVERSAL_ANNOTATION_LIBRARY_H
#define HKAI_TRAVERSAL_ANNOTATION_LIBRARY_H

#include <Ai/SpatialAnalysis/Traversal/hkaiTraversalType.h>
#include <Ai/SpatialAnalysis/Utilities/hkaiIntervalPartitionLibrary.h>

#include <Ai/SpatialAnalysis/Traversal/Types/hkaiJumpTraversal.h>
#include <Ai/Pathfinding/NavMesh/UserEdge/hkaiUserEdgeUtils.h>

/// Encapsulates a set of traversal annotations, and their associated interval
/// partitions.
class hkaiTraversalAnnotationLibrary : public hkReferencedObject
{
public:	
	//+version(0)
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL);
	HK_DECLARE_REFLECTION();

	hkaiTraversalAnnotationLibrary() { }
	hkaiTraversalAnnotationLibrary(hkFinishLoadedObjectFlag f)
		: hkReferencedObject(f)
		, m_intervalPartitionLibrary(f)
		, m_userEdgePairs(f)
		, m_annotations(f)
		, m_vectorLibrary(f)
	{ }

	typedef hkUint32 Index;

	typedef hkUint32 VectorIndex;

	const static VectorIndex INVALID_VECTOR_INDEX = 0xFFFFFFFFu;

	/// \name Userdata/index mapping
	/// These members may be modified if desired, as long as the constraints
	/// described in their comments are maintained.
	///@{

	/// The maximum number of annotations which can be contained in a library.
	/// This gives the index/userdata mapping some room for flag bits.
	const static hkUint32 MAX_ANNOTATIONS = 2 << 20;

	/// A special userdata used for the ends of one-way user edge pairs.
	/// Must never be returned by getUserdataFromAnnotationIndex(), and must
	/// return false when passed to userdataIsAnnotationIndex().
	const static hkUint32 INVALID_USERDATA = 0;

	/// Returns whether the userdata should be interpreted as an annotation
	/// index. Must return false when called with INVALID_USERDATA.
	static hkBool32 userdataIsAnnotationIndex(hkUint32 userdata)
	{
		
		
		return (userdata & 0xFFF00000u) == 0x7F900000u;
	}

	/// Converts the provided annotation index to a userdata. Must not be
	/// called on values greater than MAX_ANNOTATIONS. Must produce values
	/// which return true when passed to userdataIsAnnotationIndex(), and must
	/// be the inverse of getAnnotationIndexFromUserdata().
	static hkUint32 getUserdataFromAnnotationIndex(Index index)
	{
		HK_ASSERT(0x75e697e2, index < MAX_ANNOTATIONS);
		return index | 0x7F900000u;
	}

	/// Converts the provided userdata to an annotation index. Must not be
	/// called on values which return false when passed to 
	/// userdataIsAnnotationIndex(), and must be the inverse of 
	/// getUserdataFromAnnotationIndex().
	static Index getAnnotationIndexFromUserdata(hkUint32 userdata)
	{
		HK_ASSERT(0x3b464c9f, userdataIsAnnotationIndex(userdata));
		return userdata & 0x000FFFFFu;
	}

	///@}

	/// Add an annotation to the library.
	/// 
	/// \param userEdgePair The user edge pair associated with the annotation.
	/// The m_userDataA, m_userDataB, and m_direction fields of \a userEdgePair
	/// are not used. The A1 and A2 points with which \a userEdgePair is 
	/// constructed must refer to the starting edge's left and right points, 
	/// respectively.
	/// \param traversalType The type of traversal associated with the 
	/// annotation.
	/// \param userdata The user-provided word inserted into the annotation,
	/// accessible through getUserdata(). This is not directly used as the
	/// userdata for the user edge.
	Index addAnnotation(
		hkaiUserEdgeUtils::UserEdgePair const& userEdgePair,
		hkaiTraversalType traversalType,
		hkUint32 userdata);

	/// Associate an interval partition to the most recently added annotation.
	/// For a given annotation, this function must be called with consecutively
	/// increasing \a offset from 0. The \a annotationIndex and \a offset are
	/// passed for debug checking purposes only.
	void addIntervalPartition(
		Index annotationIndex,
		int offset,
		hkaiIntervalPartition const& intervalPartition,
		bool storeYData, bool storeIntData);

	/// Associate a vec4 to the most recently added annotation.
	/// For a given annotation, this function must be called with consecutively
	/// increasing \a offset from 0. The \a annotationIndex and \a offset are
	/// passed for debug checking purposes only.
	void addVector(
		Index annotationIndex,
		int offset,
		hkVector4Parameter vector);

	int getNumAnnotations() const;
	hkaiUserEdgeUtils::UserEdgePair const& getUserEdgePair(Index annotationIndex) const;
	hkArray<hkaiUserEdgeUtils::UserEdgePair> const& getUserEdgePairArray() const;

	hkaiTraversalType getTraversalType(Index annotationIndex) const;
	hkUint32 getUserdata(Index annotationIndex) const;

	hkSimdReal evaluateT(Index annotationIndex, hkVector4Parameter p) const;
	hkSimdReal evaluateT(Index annotationIndex, hkVector4Parameter p, hkVector4Parameter left, hkVector4Parameter right) const;
	hkBool32 getClosestDefined(Index annotationIndex, int partitionOffset, hkReal & tInOut) const;
	hkBool32 isDefinedAt(Index annotationIndex, int partitionOffset, hkReal t) const;
	hkBool32 tryEvaluateYAt(Index annotationIndex, int partitionOffset, hkReal t, hkReal& y) const;
	hkBool32 tryGetIntDataAt(Index annotationIndex, int partitionOffset, hkReal t, hkUint32& intData) const;
	void getVectorData(Index annotationIndex, int vectorOffset, hkVector4 & vecOut) const;

	hkUint16 getNumPartitionIntervals(Index annotationIndex, int partitionOffset) const;
	void getPartitionInterval(
		Index annotationIndex, int partitionOffset, hkUint16 intervalIndex, 
		hkaiIntervalPartition::Interval & intervalOut) const;

	/// The up-vector used to generate the annotations.
	hkVector4 m_up;

public:
	struct Annotation
	{
		//+version(0)
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_SPATIAL, Annotation);
		HK_DECLARE_REFLECTION();
		HK_DECLARE_POD_TYPE();

		/// If this vector is dot4xyz1'd with a point on the start edge, it should
		/// produce the t value (between startLeft and startRight) which would 
		/// interpolate to that point.
		hkVector4 m_tEquation;

		/// User-defined data. The built-in analyzers fill this from the traversal
		/// analysis spec.
		hkUint32 m_userdata;

		/// The index of the first partition associated with this annotation, or
		/// hkaiIntervalPartitionLibrary::INVALID_PARTITION_INDEX if this
		/// annotation has no associated partitions.
		hkaiIntervalPartitionLibrary::Index m_firstPartitionIndex;

		/// The index of the first vector associated with this annotation, or
		/// INVALID_VECTOR_INDEX if this annotation has no associated vectors.
		VectorIndex m_firstVectorIndex;
		
		/// The number of partitions associated with this annotation. This is 
		/// used for debug checking only.
		hkUint8 m_numPartitions;

		/// The number of vectors associated with this annotation. This is used
		/// for debug checking only.
		hkUint8 m_numVectors;

		/// The type of traversal associated with the annotation.
		hkEnum<hkaiTraversalType, hkUint8> m_traversalType;
	};
private:

	hkaiIntervalPartitionLibrary m_intervalPartitionLibrary;
	hkArray<hkaiUserEdgeUtils::UserEdgePair> m_userEdgePairs;
	hkArray<Annotation> m_annotations;
	hkArray<hkVector4> m_vectorLibrary;
};

#endif

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
