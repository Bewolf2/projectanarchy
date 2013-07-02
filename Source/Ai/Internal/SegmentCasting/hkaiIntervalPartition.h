/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_INTERVAL_PARTITION_H
#define HK_AI_INTERVAL_PARTITION_H

#include <Common/Base/Math/Vector/hkVector2.h>


/// A piecewise-linear cadlag function (with discontinuities) defined over a real interval.
class hkaiIntervalPartition
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH, hkaiIntervalPartition);

	enum Op
	{
		OP_MIN,
		OP_MAX
	};

	enum Combiner
	{
		COMB_UNION,
		COMB_INTERSECTION
	};

	struct Interval
	{
		HK_DECLARE_POD_TYPE();

		static const hkUint32 INVALID_DATA = 0xFFFFFFFF;

		Interval() { }

		Interval(hkReal leftX, hkReal rightX, hkReal slope, hkReal offset, hkUint32 data = INVALID_DATA) : 
			m_leftX(leftX), m_rightX(rightX), m_slope(slope), m_offset(offset), m_data(data) { }

		void setFromEndpoints(hkVector2 const& left, hkVector2 const& right, hkUint32 data = INVALID_DATA);
		void setFromEndpoints(hkReal leftX, hkReal leftY, hkReal rightX, hkReal rightY, hkUint32 data = INVALID_DATA);

		void setFromPacked(hkVector4Parameter packed, hkUint32 data = hkUint32(-1) )
		{
			packed.store<4>(&m_leftX);
		}

		void getPacked(hkVector4 & packedOut) const
		{
			packedOut.load<4, HK_IO_NATIVE_ALIGNED>(&m_leftX);
		}

		void interpolateParallelogram(
			hkVector4Parameter origin,
			hkVector4Parameter xVector,
			hkVector4Parameter yVector,
			hkVector4 & leftOut,
			hkVector4 & rightOut) const;

		hkVector2 getLeftEndpoint() const;
		hkVector2 getRightEndpoint() const;

		hkReal evaluateAt(hkReal x) const;

		hkBool32 operator==(Interval const& other) const;

		bool isOk() const;

		hkVector2 intersect(Interval const& other) const;

		void shiftX(hkReal shift);
		void shiftY(hkReal shift);

		hkBool32 clampY(hkReal front, hkReal back);
		template<Op op>
		hkBool32 clampFunc(hkReal slope, hkReal offset);

		template<Op op>
		hkReal getExtremeY() const;

		hkReal m_leftX;
		hkReal m_rightX;
		hkReal m_slope;
		hkReal m_offset;

		hkUint32 m_data;
	};

	hkaiIntervalPartition();

	void reserve(int numIntervals);

	/// In-place union the function's domain with the provided single interval.
	/// For overlaps, the provided op is used to choose the dominant output.
	template<Op op>
	void unionOne(Interval const& interval);

	/// Shift the domain, such that for any x, the result of evaluating the
	/// function at x+shift after this call is the same as evaluating the 
	/// function at x before this call.
	void shiftX(hkReal shift);

	/// Shift the range, such that for any x in the function's domain, the 
	/// result of evaluating the function at x after this call is \a shift 
	/// greater than it was before this call.
	void shiftY(hkReal shift);

	/// Remove all intervals.
	void clear();

	/// Clip the domain of the function, such that no points outside the
	/// interval from \a left to \b right are defined, and all points within
	/// the range are unchanged.
	void clipX(hkReal left, hkReal right);

	/// Clip the range of the function, such that any points which previously
	/// evaluated to a y-value outside the interval from \a front to \a back
	/// are now undefined.
	void clipY(hkReal front, hkReal back);

	/// Clip the function against a straight line, such that any points which
	/// previously were dominated by the straight line (with respect to the
	/// provided op) are undefined, and all other points are unchanged.
	/// 
	/// For instance, if op is OP_MIN, areas below the line will be discarded.
	template<Op op>
	void clipFunc(hkReal slope, hkReal offset);

	/// Horizontally extrude the function with respect to the provided \a op.
	///
	/// If \a comb is COMB_UNION, an x-value will be within the resultant
	/// function's domain if any x-value within \a halfWidth of x was in the
	/// original function's domain. If \a comb is COMB_INTERSECTION, an x-value
	/// will be within the resultant function's domain if all x-value within
	/// \a halfWidth of x were in the original function's domain. 
	/// 
	/// For any x in the resultant function's domain, the result of evaluating
	/// the resultant function at x will be the extremum of the original 
	/// function within \a halfWidth of x, with respect to the provided \a op.
	template<Op op, Combiner comb>
	void extrudeX(hkReal halfWidth);

	/// Combine two functions.
	///
	/// If \a comb is COMB_UNION, the domain of the resultant function is the
	/// union of the domains of the two original functions. If \a comb is
	/// COMB_INTERSECTION, the domain of the resultant function is the 
	/// intersection of the domains of the two original functions.
	/// 
	/// For any x in the resultant function's domain, the result of evaluating
	/// the resultant function at x will be the extremum of the two original
	/// functions at x, with respect to the provided \a op. If \a comb is
	/// COMB_UNION and the x is only in the domain of one of the original 
	/// functions, the result is the same as evaluating that original function
	/// at that x.
	template<Op op, Combiner comb>
	void combine(hkaiIntervalPartition const& other);

	/// Return the number of defined intervals in the function.
	hkInt32 getNumIntervals() const { return m_intervals.getSize(); }

	/// Return whether the function contains no defined intervals.
	hkBool32 isEmpty() const { return m_intervals.isEmpty(); }

	/// Get the const interval at the given index.
	Interval const& getInterval(hkInt32 index) const;

	/// Access the interval at the given index. Code which calls this function
	/// and modifies the resultant interval must ensure that the function 
	/// remains cadlag and does not become multiply-defined at any point.
	Interval & accessInterval(hkInt32 index);

	/// Performs basic sanity checks on the function.
	bool isOk() const;

	/// Appends an interval to the function, whose domain must be disjoint 
	/// from and greater than all elements in the current domain.
	void appendInterval(Interval const& interval);

	/// Appends another function to the function, whose domain must be disjoint 
	/// from and greater than all elements in the current domain.
	void appendPartition(hkaiIntervalPartition const& other);

	/// Appends an interval to the function, whose domain must be disjoint 
	/// from and greater than all elements in the current domain.
	void appendIntervalFromEndpoints(hkReal leftX, hkReal leftY, hkReal rightX, hkReal rightY, hkUint32 data = Interval::INVALID_DATA);

	/// Return the index of the interval at which \a x is defined. If \a x is
	/// not within the function's domain, returns -1.
	hkInt32 findInterval(hkReal x) const;

	/// Return the index of the interval at which \a x is defined. If \a x is
	/// not within the function's domain, returns the index of the rightmost
	/// interval which is to the left of \a x. If the function contains no
	/// intervals, or if the leftmost interval is to the right of \a x, returns
	/// -1.
	hkInt32 findLeftInterval(hkReal x) const;

	/// Return the index of the interval at which \a x is defined. If \a x is
	/// not within the function's domain, returns the index of the leftmost
	/// interval which is to the right of \a x. If the function contains no
	/// intervals, or if the rightmost interval is to the left of \a x, returns
	/// getNumIntervals().
	hkInt32 findRightInterval(hkReal x) const;

	/// Returns whether the \a other function is structurally identical to this
	/// function -- that is, whether the domains are the same, the evaluation
	/// result is the same for any x, and the functions contain the same number
	/// of intervals, with the intervals at each index having identical left 
	/// and right extents.
	hkBool32 equals(hkaiIntervalPartition const& other) const;

	// remove intervals where "other" evaluates to a lower value
	void clipMin(hkaiIntervalPartition const& other);
	
	// remove intervals where "other" is defined
	void clipDefined(hkaiIntervalPartition const& other);

	// remove intervals where "other" is not defined
	void clipNotDefined(hkaiIntervalPartition const& other);

	void swap(hkaiIntervalPartition & other);

	/// Returns whether all points between \a left and \a right are
	/// in the function's domain.
	hkBool32 coversInterval(hkReal left, hkReal right) const;

	/// Attempts to evaluate the function at \a x. Return whether \a x was
	/// actually within the function's domain. If it was not, \a yOut is
	/// undefined.
	hkBool32 tryEvaluateAt(hkReal x, hkReal & yOut) const;

	/// Attempts to evaluate the function at \a x. Returns the corresponding
	/// y if \a x is within the function's domain; otherwise, returns 
	/// \a defaultY.
	hkReal evaluateWithDefault(hkReal x, hkReal defaultY) const;

	/// Get the extremal value in the function's domain with respect to the op
	/// specified.
	///
	/// \note The input function is treated as doubly-defined at
	/// discontinuities; thus, this actually returns the least upper bound or
	/// greatest lower bound.
	template<Op op>
	hkReal getExtremeX() const;

	/// Get the extremal value in the function's range with respect to the op
	/// specified.
	///
	/// \note The input function is treated as doubly-defined at
	/// discontinuities; thus, this actually returns the least upper bound or
	/// greatest lower bound.
	template<Op op>
	hkReal getExtremeY() const;

	/// Complement the domain of the function, such that all x-values that were
	/// previously defined are no longer defined, and all x-values that were
	/// not previously defined are now defined.
	///
	/// \note The input function must not have any defined intervals which are
	/// entirely outside [minX, maxX]. If the input function may have such
	/// intervals, clipX() should be called before this function is called.
	/// 
	/// \param minX The minimum value of the new domain.
	/// \param maxX The maximum value of the new domain.
	void complement(hkReal minX, hkReal maxX);

	/// Reflect the function around the line X=mid. 
	/// 
	/// \post For any a, evaluating the function at mid+a will produce the same
	/// result as evaluating the original function at mid-a.
	void reflectX(hkReal mid);

	/// Remap the domain of the function while preserving the output. The
	/// remapping is linear, and described by two original domain values and
	/// their remapped equivalents. Neither of these values need actually be
	/// defined by an interval; they are used only to compute the mapping
	/// function.
	///
	/// \post Evaluating the function at newX1 will produce the same result
	/// result as evaluating the function at curX1 would have before calling
	/// this function. Likewise with newX2 and curX2.
	///
	/// \param curX1 An x-coordinate in the original domain.
	/// \param curX2 Another x-coordinate in the original domain.
	/// \param newX1 The remapped curX1.
	/// \param newX2 The remapped curX1.
	void remapX(hkReal curX1, hkReal curX2, hkReal newX1, hkReal newX2);

	/// Remap the output of the function without remapping the domain. The
	/// remapping is linear, and described by two original output values and
	/// their remapped equivalents. Neither of these values need actually be
	/// in the function's range; they are used only to compute the mapping
	/// function.
	///
	/// \post Wherever evaluating the function would have produced curY1, will
	/// now produce newY1. Likewise with curY2 and newY2.
	///
	/// \param curX1 An x-coordinate in the original domain.
	/// \param curX2 Another x-coordinate in the original domain.
	/// \param newX1 The remapped curX1.
	/// \param newX2 The remapped curX1.
	void remapY(hkReal curY1, hkReal curY2, hkReal newY1, hkReal newY2);

	/// Spread out all intervals, up to a limit of maxSpread on each side,
	/// to cover undefined areas. Gaps are filled by linear extrapolation.
	void spread(hkReal maxSpread);

	/// Contract all intervals by maxContract on each side. This will create
	/// gaps between all adjacent intervals even if there is no gap in the
	/// domain, so it should generally be used after a discardRAnge(). 
	/// Intervals with width less than 2*maxContract will be discarded.
	void contract(hkReal maxContract);

	void copyFrom(hkaiIntervalPartition const& other);

	void copyRegionFrom(hkaiIntervalPartition const& other, hkReal minX, hkReal maxX);

	/// Merge adjacent intervals even if they do not have the same offset and
	/// slope. Afterwards, Y values (and integer data) will be undefined, but
	/// the function's domain will be unchanged, and the number of intervals
	/// will be minimal.
	void discardRange();

	void setAllData(hkUint32 data);

	/// Pass each interval to the provided functor (which must take an
	/// Interval &), allowing it to modify the interval. Store the modified
	/// interval if the functor returns true; otherwise, discard the interval.
	template<typename FilterType>
	void filterIntervals(FilterType const& filter);

protected:
	typedef hkContainerHeapAllocator Allocator;

	typedef hkArray<Interval, Allocator> IntervalArray;

	IntervalArray m_intervals;

	static void HK_CALL addRemovingDupes(
		IntervalArray & intervals, 
		Interval const& interval);

};

#include <Ai/Internal/SegmentCasting/hkaiIntervalPartition.inl>

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
