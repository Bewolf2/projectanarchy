/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_POLYGON_SETUP_2D_H
#define HK_AI_POLYGON_SETUP_2D_H


extern const class hkClass PolygonSetup2dClass;

class hkClassEnum;
class hkPseudoRandomGenerator;
extern const hkClassEnum* PolygonSetup2dTestCaseEnum;

	/// Unit test information
class PolygonSetup2d
{
	public:
		HK_DECLARE_REFLECTION();

		enum TestCase
		{
			TEST_BOX,
			TEST_2BOX,
			TEST_2TRI,
			TEST_FLAT_HEX,
			TEST_2FLAT_HEX,
			TEST_2FLAT_HEXa,
			TEST_2SPHINX,
			TEST_SPIRAL,
			TEST_H_BORDER,
			TEST_HOOKED_CROSS_BORDER,
			TEST_FRACTAL_BORDER,
			TEST_OVERHANG,
			TEST_JAWS,
			TEST_STAR,
			TEST_2STAR,
			TEST_STAR_CUTOUT,
			TEST_MULTIHOLE	//last! see TEST_MAX
		};
		enum { TEST_MAX = TEST_MULTIHOLE+1 };

		static void HK_CALL setupTestSimple(
			TestCase test,
			hkArray< hkArray<hkVector2> >& out,
			hkVector2& fromOut,
			hkVector2& toOut );
		static void HK_CALL setupTest(
			TestCase test,
			hkArray< hkArray<hkVector2> >& out,
			hkArray<hkVector2>& fromOut,
			hkArray<hkVector2>& toOut );

		PolygonSetup2d( hkArray<hkVector2>& out ) : m_pts(out) {}

			// modify existing vertices
		PolygonSetup2d& translate( hkReal tx, hkReal ty );
		PolygonSetup2d& rotate( hkReal rad );
		PolygonSetup2d& rotateIndices( int i );
		PolygonSetup2d& reverseWinding();
		PolygonSetup2d& scale( hkReal x, hkReal y );
		PolygonSetup2d& fractalize( hkReal disp, int levels, hkPseudoRandomGenerator* rand = HK_NULL );

			// create vertices
		PolygonSetup2d& createBox( hkReal xside, hkReal yside );
		PolygonSetup2d& createTriangle();
		PolygonSetup2d& createFlatHex( hkReal halfheight, hkReal bodylen );
		PolygonSetup2d& createSphinx(hkReal elongate=0);
		PolygonSetup2d& createStar( int numArms, hkReal insideRadius = 1.0f, hkReal outsideRadius = 3.0f );
		PolygonSetup2d& createMalteseCross( hkReal armLength = 3.0f, hkReal insideArmWidth = 1.0f, hkReal outsideArmWidth = 2.0f );
		PolygonSetup2d& createSpiral( int numSegments, hkReal width = 0.5f);
		PolygonSetup2d& createFig8();
		PolygonSetup2d& createCrown();
		PolygonSetup2d& createH();
		PolygonSetup2d& createHookedCross();
		PolygonSetup2d& createOverhang();
		PolygonSetup2d& createComb(int numTeeth);
		PolygonSetup2d& createMultihole(int partIndex);
		PolygonSetup2d& createWrapAround();

		hkArray<hkVector2>& m_pts; //+reflected(false)
		int m_pad;
};

#endif // HK_AI_POLYGON_SETUP_2D_H

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
