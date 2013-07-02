/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BLEND_CURVE_UTILS_H
#define HKB_BLEND_CURVE_UTILS_H


extern const class hkClass hkbBlendCurveUtilsClass;

class hkbBlendCurveUtils
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbBlendCurveUtils );
	HK_DECLARE_REFLECTION();

	/// The different types of blend curves.
	enum BlendCurve
	{
		/// A cubic curve that is smooth at the endpoints: f(t) = -6 t^3 + 3 t^2.
		BLEND_CURVE_SMOOTH,

		/// A linear curve: f(t) = t.
		BLEND_CURVE_LINEAR,

		/// A cubic curve that is abrupt at the beginning and smooth at the end: f(t) = -t^3 + t^2 + t
		BLEND_CURVE_LINEAR_TO_SMOOTH,

		/// A cubic curve that is smooth at the beginning and abrupt at the end: f(t) = -t^3 + 2 t^2
		BLEND_CURVE_SMOOTH_TO_LINEAR,

		/// If you customize this code, start your blend curves here in case more built-in curves are 
		/// added in a future release.
		
		// FIRST_USER_BLEND_CURVE = 32,
	};

	/// Compute the blend curve function (weight) for the given curve.  This is for blends specifying a 
	/// beginning and end point.
	static hkReal HK_CALL evaluateBlendCurve( int blendCurve, hkReal t );

	/// Compute what fraction of the remaining blend curve b(t) is traversed between t0 and t1.
	///
	/// Returns (b(t1) - b(t0)) / (1.0f - b(t0)).
	static hkReal HK_CALL computeFractionOfRemaining( int blendCurve, hkReal t0, hkReal t1 );
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
