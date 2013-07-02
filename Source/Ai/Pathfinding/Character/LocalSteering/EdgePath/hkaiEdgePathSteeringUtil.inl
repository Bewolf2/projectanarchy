/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


inline void hkaiEdgePathSteeringUtil::calcTangentDirs(
	hkVector4 const& p1, 
	hkVector4 const& p2, 
	hkVector4 const& c1, 
	hkVector4 const& c2, 
	hkVector4 const& up, 
	hkVector4 & tangentDir1, 
	hkVector4 & tangentDir2,
	hkVector4 & perpDir1,
	hkVector4 & perpDir2)
{
	// Okay, so this function looks a little scary. I understand. Don't worry. We'll get through this together.
	//
	// First of all, take a look at tangentGeometry.svg. Note that the triangles TCP, ATP, and ACT are all similar.
	//
	// The basic approach is to take vectors PA and AT, normalize them, and mix them to produce vector PT.

	HK_ASSERT(0x4e320661, up.isNormalized<3>());

	// Find PC for each. Note that these are in the same direction as PA.
	hkVector4 pc1; pc1.setSub(c1, p1);
	hkVector4 pc2; pc2.setSub(c2, p2);

	pc1.subMul(up, up.dot<3>(pc1));
	pc2.subMul(up, up.dot<3>(pc2));

	hkSimdReal pc1LenSq = pc1.lengthSquared<3>();
	hkSimdReal pc2LenSq = pc2.lengthSquared<3>();

	// From now on, we're going to do C1 stuff in X and Z, and C2 stuff in Y and W.
	hkVector4 pc1LenSq_pc2LenSq_pc1LenSq_pc2LenSq;
	pc1LenSq_pc2LenSq_pc1LenSq_pc2LenSq.set(pc1LenSq, pc2LenSq, pc1LenSq, pc2LenSq);

	// Save the reciprocal square root -- we'll use it again later.
	hkVector4 pc1LenRecip_pc2LenRecip_pc1LenRecip_pc2LenRecip;
	pc1LenRecip_pc2LenRecip_pc1LenRecip_pc2LenRecip.setSqrtInverse(
		pc1LenSq_pc2LenSq_pc1LenSq_pc2LenSq);

	// This completes the normalization. Again, pcDir == paDir.
	hkVector4 pc1Dir; pc1Dir.setMul(pc1, pc1LenRecip_pc2LenRecip_pc1LenRecip_pc2LenRecip.getComponent<0>());
	hkVector4 pc2Dir; pc2Dir.setMul(pc2, pc1LenRecip_pc2LenRecip_pc1LenRecip_pc2LenRecip.getComponent<1>());
	
	HK_ON_DEBUG( const hkSimdReal oneEMinusThree = hkSimdReal::fromFloat(1e-3f); )
	HK_ASSERT(0x4a0a5590, pc1Dir.isNormalized<3>(1e-3f) || pc1.allEqualZero<3>(oneEMinusThree));
	HK_ASSERT(0x4a0a5591, pc2Dir.isNormalized<3>(1e-3f) || pc2.allEqualZero<3>(oneEMinusThree));

	// Find AT for each. Signed radius is used to orient AT; afterward we'll just use squared radius, so this is
	// the only time the sign matters. 
	hkVector4 at1Dir; at1Dir.setCross(up, pc1Dir);
	at1Dir.setFlipSign(at1Dir, c1.getW());
	hkVector4 at2Dir; at2Dir.setCross(up, pc2Dir);
	at2Dir.setFlipSign(at2Dir, c2.getW());

	HK_ASSERT(0x4a0a5592, at1Dir.isNormalized<3>(1e-3f) || pc1.allEqualZero<3>(oneEMinusThree));
	HK_ASSERT(0x4a0a5593, at2Dir.isNormalized<3>(1e-3f) || pc1.allEqualZero<3>(oneEMinusThree));

	// Get squared radii; again, C1 squared radius is in X and Z, and C2 squared radius is in Y and W.
	hkVector4 r1Sq_r2Sq_r1Sq_r2Sq;
	r1Sq_r2Sq_r1Sq_r2Sq.set(c1.getW(), c2.getW(), c1.getW(), c2.getW());
	r1Sq_r2Sq_r1Sq_r2Sq.mul(r1Sq_r2Sq_r1Sq_r2Sq);

	// PT.lenSq = PC.lenSq - CT.lenSq = PC.lenSq - r^2
	hkVector4 pt1LenSq_pt2LenSq_pt1LenSq_pt2LenSq;
	pt1LenSq_pt2LenSq_pt1LenSq_pt2LenSq.setSub(pc1LenSq_pc2LenSq_pc1LenSq_pc2LenSq, r1Sq_r2Sq_r1Sq_r2Sq);

	// Now we put squared radius into X and Y. 
	hkVector4 r1Sq_r2Sq_pt1LenSq_pt2LenSq;
	r1Sq_r2Sq_pt1LenSq_pt2LenSq.setSelect<hkVector4ComparisonMask::MASK_XY>(r1Sq_r2Sq_r1Sq_r2Sq, pt1LenSq_pt2LenSq_pt1LenSq_pt2LenSq);

	// Now X and Y store CT.lenSq, and Z and W store PT.lenSq.
	// This is interesting, because AT.lenSq = PT.lenSq*CT.lenSq/PC.lenSq,
	// and AP.lenSq = PT.lenSq*PT.lenSq / PC.lenSq. 

	hkVector4 at1LenSq_at2LenSq_ap1LenSq_ap2LenSq;
	at1LenSq_at2LenSq_ap1LenSq_ap2LenSq.setMul(r1Sq_r2Sq_pt1LenSq_pt2LenSq, pt1LenSq_pt2LenSq_pt1LenSq_pt2LenSq);

	// Now, think back to pt1LenSq. There's actually a chance that it's negative, if p is inside the circle.
	// In this case, there is no tangent, but we should still do something other than ignore the situation. 
	// Our strategy will be to output atDir as the tangent. This is equivalent to shrinking the circle until 
	// the point is on its border. We're doing it here because the math is easier than doing it earlier AND
	// if we do it later we might throw up a floating point exception when we do that sqrt down there.

	hkVector4Comparison insideCircle = pt1LenSq_pt2LenSq_pt1LenSq_pt2LenSq.lessEqualZero();
	at1LenSq_at2LenSq_ap1LenSq_ap2LenSq.setSelect(
		insideCircle,
		hkVector4::getConstant<HK_QUADREAL_1100>(),
		at1LenSq_at2LenSq_ap1LenSq_ap2LenSq);

	// Do our sqrt, and then multiply by the PC.len^-1 we kept from earlier. Now these are [0,pcLen].
	hkVector4 at1Len_at2Len_ap1Len_ap2Len; 
	at1Len_at2Len_ap1Len_ap2Len.setSqrt(at1LenSq_at2LenSq_ap1LenSq_ap2LenSq);

	at1Len_at2Len_ap1Len_ap2Len.mul(pc1LenRecip_pc2LenRecip_pc1LenRecip_pc2LenRecip);


	// Finally, mix to produce the tangents.
	tangentDir1.setMul(at1Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<0>());
	tangentDir1.addMul(pc1Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<2>());
	tangentDir2.setMul(at2Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<1>());
	tangentDir2.addMul(pc2Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<3>());

	//HK_ASSERT(0x4a0a5594, tangentDir1.isNormalized<3>());
	//HK_ASSERT(0x4a0a5595, tangentDir2.isNormalized<3>());


	// And the perpendiculars come pretty much for free, when they'd need a cross product later.
	perpDir1.setMul(at1Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<2>());
	perpDir1.subMul(pc1Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<0>());
	perpDir2.setMul(at2Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<3>());
	perpDir2.subMul(pc2Dir, at1Len_at2Len_ap1Len_ap2Len.getComponent<1>());

	// Re-flip the directions so they're always left perpendiculars.
	perpDir1.setFlipSign(perpDir1, c1.getW());
	perpDir2.setFlipSign(perpDir2, c2.getW());
}


void hkaiEdgePathSteeringUtil::calcMutualTangentPoints(
	hkVector4 const& aIn, 
	hkVector4 const& bIn, 
	hkVector4 const& up, 
	hkVector4 & aTangentPoint, 
	hkVector4 & bTangentPoint)
{
	hkVector4 a; a.setSubMul(aIn, up, aIn.dot<3>(up));
	hkVector4 b; b.setSubMul(bIn, up, bIn.dot<3>(up));

	HK_ASSERT(0x12866beb, !a.allExactlyEqual<3>(b));

	hkSimdReal ar = aIn.getW();
	hkSimdReal br = bIn.getW();

	if(ar.isEqual(-br))
	{
		hkVector4 aToB; aToB.setSub(b,a);
		hkVector4 perp; perp.setCross(aToB, up); perp.normalize<3>();
		aTangentPoint.setAddMul(a, perp, ar);
		bTangentPoint.setAddMul(b, perp, ar);
	}
	else if(ar.isEqualZero())
	{
		hkVector4 tanDir1, tanDir2, perpDir1, perpDir2;
		calcTangentDirs(a, a, b, b, up, tanDir1, tanDir2, perpDir1, perpDir2);
		aTangentPoint = a;
		bTangentPoint.setAdd(a, tanDir1);
	}
	else if(br.isEqualZero())
	{
		hkVector4 tanDir1, tanDir2, perpDir1, perpDir2;
		calcTangentDirs(b, b, a, a, up, tanDir1, tanDir2, perpDir1, perpDir2);
		bTangentPoint = b;
		aTangentPoint.setAdd(b, tanDir1);
	}
	else
	{
		hkSimdReal t = ar / (ar+br);
		hkVector4 c; c.setInterpolate(a, b, t);
		{
			hkSimdReal aW; aW.setFlipSign(a.getW(), t.lessZero());
			a.setW(aW);
			hkSimdReal bW; bW.setFlipSign(b.getW(), t.greater(hkSimdReal_1));
			b.setW(bW);
		}
		hkVector4 tanDir1, tanDir2, perpDir1, perpDir2;
		calcTangentDirs(c, c, a, b, up, tanDir1, tanDir2, perpDir1, perpDir2);
		aTangentPoint.setAdd(c, tanDir1);
		bTangentPoint.setAdd(c, tanDir2);
	}
}

hkBool32 hkaiEdgePathSteeringUtil::inCapsule( hkVector4 const& p, hkVector4 const& c1, hkVector4 const& c2, hkVector4 const& up )
{
	HK_ASSERT2(0x439f5b62, 
		c1.getW().isEqualZero() 
		|| c2.getW().isEqualZero() 
		|| (c1.getW().isLessZero() && c2.getW().isGreaterZero())
		|| (c1.getW().isGreaterZero() && c2.getW().isLessZero()),
		"Radii must have opposite signs");

	HK_ASSERT2(0x439f5b63, !c1.allExactlyEqual<3>(c2), "Capsule points must be distinct");

	hkSimdReal r1 = c1.getW();
	hkSimdReal r2 = -c2.getW();


	hkVector4 c1c2;
	c1c2.setSub(c2, c1);
	c1c2.subMul(up.dot<3>(c1c2), up);

	hkSimdReal c1c2LenInvSq = c1c2.lengthSquared<3>().reciprocal();

	hkVector4 c1p;
	c1p.setSub(p, c1);
	c1p.subMul(up.dot<3>(c1p), up);

	hkSimdReal t = c1c2.dot<3>(c1p);
	t.mul(c1c2LenInvSq);
	t.setClamped(t, hkSimdReal_0, hkSimdReal_1);

	hkSimdReal r;
	r.setInterpolate(r1, r2, t);
	hkSimdReal rSq;
	rSq.setMul(r, r);

	hkVector4 pToProj; 
	pToProj.setSubMul(c1p, c1c2, t);
	pToProj.subMul(up.dot<3>(pToProj), up);

	hkSimdReal distToProjSq = pToProj.lengthSquared<3>();
	
	return distToProjSq.isLess(rSq);
}

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
