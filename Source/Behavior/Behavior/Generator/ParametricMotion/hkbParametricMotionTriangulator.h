/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PARAMETRIC_MOTION_TRIANGULATOR_H
#define HKB_PARAMETRIC_MOTION_TRIANGULATOR_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>

struct hkGeometry;

#define EMPTY_MOTION_SPACE_VERTEX_ID		0xffffff
#define HKB_PARAMETRIC_MOTION_MINIMUM_NUMBER_OF_SAMPLE_POINTS	3


	/// Utilities used by hkbParametricMotionGenerators to build and sample a parameter space.
class hkbParametricMotionTriangulator
{
	public:
		struct Edge
		{
			HK_DECLARE_POD_TYPE();

			int m_startVtxIdx;
			int m_endVtxIdx;

			void initialize( int startVtxIdx, int endVtxIdx ) 
			{
				m_startVtxIdx = startVtxIdx;
				m_endVtxIdx = endVtxIdx;
			}
		};

	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbParametricMotionTriangulator );

		/// Triangulates a set of points.  The output geometry is stored in trianglesOut.
		static void HK_CALL triangulate( const hkVector4* points, int numPoints, hkGeometry& geometryOut, hkAabb& outParamsDomain, hkArray< int >& outHullIndices );

		/// Finds the triangle in geometry which contains the point defined by parameterX and parameterY.  If one is
		/// found, coordinatesOut will contain the barycentric coordinates of the point in the triangle.  The index of
		/// the triangle will also be returned.  If a triangle is not found -1 is returned and coordinatesOut left untouched.			
		static int HK_CALL findTriangleIndex( hkVector4Parameter params, const hkSimdReal* duration, const hkGeometry& geometry, hkVector4& coordinatesOut );

		/// Given a point in the motion space specified in the 'params' vector, the method looks for the closest triangle 
		/// that contains an animation in its every vertex.
		static int HK_CALL findNearestTriangleIndex( hkVector4Parameter params, const hkGeometry& geometry, hkVector4* arrCoordinatesOut );

		/// Collects the triangle edges that share a common property of having exactly one vertex
		/// that belongs to set of vertices indexed by the indices the 'hullIndices' array contains.
		/// Such edges are considered to 'shoot outwards' from the motion space.
		///
		/// The specified hull indices array contains the indices of the data clips that create the hull,
		/// of the triangulated motion space, same indices each vertex keeps an annotation of in its W component.
		static void HK_CALL collectOutgoingEdges( const hkGeometry& geometry, const hkArray< int >& hullIndices, hkArray< Edge >& outEdges );

	private:
		/// Calculates blend weights that will produce the desired output velocity
		static void HK_CALL calcBlendWeights( const hkVector4** velocity, const hkSimdReal* duration, hkVector4Parameter vP, hkVector4& outWeights ) ;

		/// A helper method that finds the distance from the beginning of the specified line to the projected
		/// position of the specified arbitrary point
		static hkReal HK_CALL projectVertexToLine( hkVector4Parameter lineStart, hkVector4Parameter lineEnd, hkVector4Parameter point );
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
