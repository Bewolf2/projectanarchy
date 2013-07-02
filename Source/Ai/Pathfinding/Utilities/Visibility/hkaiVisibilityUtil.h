/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_VISIBILITY_UTIL_H
#define HKAI_VISIBILITY_UTIL_H

#include <Ai/Pathfinding/hkaiBaseTypes.h>
#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>

//#define HKAI_DEBUG_VISIBLE_SET

	/// Utility for computing the set of visible points to/from a start point
class hkaiVisibilityUtil
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, hkaiVisibilityUtil );

		/// The behavior of the visibility query when the start point is closer than the character radius to a boundary edge.
		/// Naively, this would remove all the visible space, but special handling can be done to treat this.
	enum InitialIntersectionBehavior
	{
			/// If the start point is closer than the character radius to a boundary edge,
			/// the half-space defined by the edge and passing through the start point will be removed.
			/// This allows a character standing near a wall to "see" things away from the wall.
		REMOVE_HALF_SPACE,

			/// No special handling is done, and all the visible space will be removed.
		NOTHING_VISIBLE
	};

		/// Input settings for hkaiVisibilityUtil::findVisibleSet().
	struct FindVisibleSetInput : public hkaiPathfindingUtil::NearestFeatureInput
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, FindVisibleSetInput );

		FindVisibleSetInput();

			/// Controls the behavior when the start point is closer than the character radius to a boundary edge.
		hkEnum<InitialIntersectionBehavior, hkUint8> m_initialIntersectionBehavior; //+default(hkaiVisibilityUtil::REMOVE_HALF_SPACE)
	};
	
		/// A list of arcs that have visibility to the start point
	class VisibleSet
	{
	public:

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, VisibleSet );

			/// An arc with angles from [-pi, pi]
		struct Arc
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, Arc );

				/// Min value of the arc
			hkReal m_min;

				/// Min value of the arc
			hkReal m_max;

				/// Whether or not this arc overlaps the other
			inline hkBool32 overlaps( const Arc& other ) const;

				/// Whether or not this arc fully contains the other
			inline hkBool32 contains( const Arc& other ) const;

				/// Whether or not the arc is below the min angle threshold
			inline hkBool32 isBigEnough( hkReal minAngle ) const;
		};

			/// Initialize the set to a single arc with [-pi, pi]
		inline void init();

			/// Remove subArc from the set
		void subtract( const Arc& subArc );

			/// Gets the complement of this set.
		void getComplement( VisibleSet& complementOut ) const;

			/// Computes an arc using the specified points.
		void getArcFromPoints( hkVector4Parameter p, hkVector4Parameter e1, hkVector4Parameter e2, int upAxis, Arc& arcOut );

			/// Whether or not the set is empty.
		inline bool isEmpty() const;

			/// The list of arcs in the set
		hkArray<Arc> m_arcs;

			/// Threshold angle - arcs shorter than this are removed from the set.
		hkReal m_minAngle;
	};

		/// Main function to compute the visible set
	static void HK_CALL findVisibleSet( const class hkaiStreamingCollection* collection, const FindVisibleSetInput& input, VisibleSet& visibleSetOut );

protected:

		/// Internal callback used to process edges on the faces encountered during the callback.
	class VisibleSetCallback : public hkaiPathfindingUtil::NearestFeatureCallback
	{
	public:
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI, VisibleSetCallback );

		VisibleSetCallback( const hkaiStreamingCollection* collection, hkVector4Parameter startPoint, hkVector4Parameter up, VisibleSet& set);

			/// Unused method from the base class
		virtual bool edgeTraversed(hkaiPackedKey edgeKey, const hkaiAgentTraversalInfo& agentInfo) HK_OVERRIDE { HK_ASSERT(0x5b68b914, false); return false; }

			/// Callback where all the work happens
		virtual bool faceTraversed(hkaiPackedKey faceKey, const hkaiAgentTraversalInfo& agentInfo) HK_OVERRIDE;

#ifdef HKAI_DEBUG_VISIBLE_SET 
			/// Disabled drawing function
		void drawArc( const hkaiVisibilityUtil::VisibleSet::Arc& arc, hkVector4Parameter up, hkReal radius, hkColor::Argb color );

			/// Disabled drawing functions
		void debugDraw();
#endif
	
	protected:

			/// Process the points from the edge: clip them to the cirle, compute the tangents, and subtract any arcs
		void handleEdge( hkVector4Parameter edgePointA, hkVector4Parameter edgePointB, hkSimdRealParameter searchRadius, hkSimdRealParameter characterRadius );
		void handleEdgeSegment( hkVector4Parameter edgePointA, hkVector4Parameter edgePointB, hkSimdRealParameter searchRadius, hkSimdRealParameter characterRadius );
		void handlePoint( hkVector4Parameter point, hkSimdRealParameter searchRadius, hkSimdRealParameter characterRadius );

			/// Subtracts the arc defined by the two points
		void substractSegment( hkVector4Parameter pointA, hkVector4Parameter pointB );
	
	public:

			/// Search start point
		hkVector4 m_startPoint;

			/// Up vector of the search
		hkVector4 m_up;
			
			/// Major axis of the up vector
		int m_upAxis;
		
			/// Search radius
		hkSimdReal m_searchRadius;

			/// Collection to be searched
		const hkaiStreamingCollection* m_streamingCollection;

			/// Optional edge filter
		const class hkaiAstarEdgeFilter* m_edgeFilter;

			/// See description in FindVisibleSetInput.
		hkEnum<InitialIntersectionBehavior, hkUint8> m_initialIntersectionBehavior;


			/// Output set
		VisibleSet& m_visibleSet;
	};

};

#include <Ai/Pathfinding/Utilities/Visibility/hkaiVisibilityUtil.inl>

#endif // HKAI_VISIBILITY_UTIL_H

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
