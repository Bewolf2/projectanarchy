/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// Computes a metric to allow us to compare points for similarity.
//	Metric is based on:
//    - Angle between normals
//    - Contact Point Distances
//	  - Velocity
hkReal hkpCharacterProxy::surfaceDistance(const hkpRootCdPoint& p1
												 , const hkpRootCdPoint& p2
												 HK_SPU_ARG( hkpSpuCharacterProxyCollisionCache* collisionCache )
												 ) const
{
	// Small angle approx (changed from cross product HVK-2184)
	hkSimdReal angleSquared = hkSimdReal_1 - p1.m_contact.getNormal().dot<3>(p2.m_contact.getNormal());
	angleSquared.mul(hkSimdReal::fromFloat(m_contactAngleSensitivity * m_contactAngleSensitivity));

	hkSimdReal planeDistanceSqrd = p1.m_contact.getDistanceSimdReal() - p2.m_contact.getDistanceSimdReal();
	planeDistanceSqrd.mul(planeDistanceSqrd);

	hkVector4 p1Vel, p2Vel;

	extractSurfaceVelocity(p1, p1Vel HK_SPU_ARG( collisionCache ) );
	extractSurfaceVelocity(p2, p2Vel HK_SPU_ARG( collisionCache ) );

	hkVector4 velDiff; velDiff.setSub( p1Vel, p2Vel );
	hkSimdReal velocityDiffSqrd = velDiff.lengthSquared<3>();

	return (angleSquared * hkSimdReal::fromFloat(10.0f) + velocityDiffSqrd * hkSimdReal::fromFloat(0.1f) + planeDistanceSqrd).getReal();
}

// Extract velocity information from the contact point.
void hkpCharacterProxy::extractSurfaceVelocity(const hkpRootCdPoint& hit
											   , hkVector4& velocityOut
											   HK_SPU_ARG( hkpSpuCharacterProxyCollisionCache* collisionCache )
											   ) const
{
	velocityOut.setZero();

	// Grab body information // Body A is always the shapePhantom

#if defined(HK_PLATFORM_SPU)
	// 	hkpSpuCharacterProxyUtil::CollidableData collisionDataOnSpu( hit.m_rootCollidableB );
	// 	hkpRigidBody* body =  collisionDataOnSpu.m_bodyOnSpu;
	hkpRigidBody* body = collisionCache->getRigidBodyOnSpu( hit.m_rootCollidableB );
#else
	hkpRigidBody* body = hkpGetRigidBody(hit.m_rootCollidableB);
#endif

	if (body)
	{
		// Extract velocity at point on surface
		body->getPointVelocity(hit.m_contact.getPosition(), velocityOut);
	}
}

void hkpCharacterProxy::convertFractionToDistance( const hkpRootCdPoint* cHit, int numHits, const hkVector4& displacement ) const
{
	hkpRootCdPoint* hit = const_cast<hkpRootCdPoint*>(cHit);
	for (int i = numHits-1; i>=0; i--)
	{
		const hkSimdReal fraction = hit->m_contact.getDistanceSimdReal();
		hit->m_contact.getPosition().setW(fraction);
		const hkSimdReal projLength = displacement.dot<3>(hit->m_contact.getNormal());
		hit->m_contact.setDistanceSimdReal( fraction * -projLength );
		hit++;
	}
}

void hkpCharacterProxy::convertFractionToDistance( const HK_COLLECTOR_TYPE& collector, const hkVector4& displacement ) const
{
#if defined(HK_PLATFORM_SPU)
	const hkpRootCdPoint* cHit = collector.getHits();
	int numHits = collector.getNumHits();
#else
	const hkpRootCdPoint* cHit = collector.getHits().begin();
	int numHits = collector.getHits().getSize();
#endif
	convertFractionToDistance( cHit, numHits, displacement );
}

#if !defined(HK_PLATFORM_SPU)
void hkpCharacterProxy::addEntityOrPhantom( const hkpCollidable* collidable
										   , hkArray<hkpRigidBody*>& bodies
										   , hkArray<hkpPhantom*>& phantoms )
{
	hkpRigidBody* body =  hkpGetRigidBody( collidable );
	hkpPhantom* phantom = hkpGetPhantom( collidable );

	if ( body && ( bodies.indexOf(body) == -1 ) )
	{
		bodies.pushBack( body );
		// Note: adding/removing listeners from bodies and phantoms is not thread safe.
		body->addEntityListener(this);	
	}

	if (phantom && (phantoms.indexOf(phantom) == -1) )
	{
		phantoms.pushBack( phantom );
		// Note: adding/removing listeners from bodies and phantoms is not thread safe.
		phantom->addPhantomListener( this );
	}
}
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
