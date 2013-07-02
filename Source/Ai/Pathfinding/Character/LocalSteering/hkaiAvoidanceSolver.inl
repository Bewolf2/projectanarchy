/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiMovementProperties::hkaiMovementProperties()
:	m_minVelocity(.02f),
	m_maxVelocity(5.64f),
	m_maxAcceleration(20.0f),
	m_maxDeceleration(30.0f),
	m_leftTurnRadius(-1.0f),
	m_rightTurnRadius(-1.0f),
	m_maxAngularVelocity(1.25f*HK_REAL_PI),
	m_maxAngularAcceleration(30.0f * m_maxAngularVelocity),
	m_maxTurnVelocity(0.5f * m_maxVelocity),
	m_kinematicConstraintType(CONSTRAINTS_LINEAR_ONLY)
{
}

inline hkaiAvoidanceProperties::hkaiAvoidanceProperties()
:	m_avoidanceSolverType(hkaiAvoidanceProperties::AVOIDANCE_SOLVER_SAMPLING),
	m_nearbyBoundariesSearchType(hkaiAvoidanceProperties::SEARCH_FLOOD_FILL),
	m_agentLimitTime(1.0f),
	m_obstacleLimitTime(1.0f),
	m_boundaryLimitTime(0.3f),
	m_limitObstacleDistance(1.0f),
	m_limitBoundaryDistance(-1.0f), 
	m_limitObstacleVelocity(0.5f),
	m_wallFollowingAngle(0.1f),
	m_dodgingPenalty(1.0f),
	m_velocityHysteresis(0.01f),
	m_sidednessChangingPenalty(0.0f),
	m_collisionPenalty(1.0f),
	m_penetrationPenalty(100.0f),
	m_maxNeighbors(16)
{
	hkSimdReal five; five.setFromFloat(5.f); // hkSimdReal_5 was used here, but sometime this struct is used as a global, and then order in global ctor list can have hksimdreal after this on some platforms.
	m_localSensorAabb.m_max.setAll( five ); 
	m_localSensorAabb.m_min.setNeg<4>( m_localSensorAabb.m_max );
}

inline hkReal hkaiMovementProperties::getMaxTurnVelocity() const
{
	return (m_kinematicConstraintType == CONSTRAINTS_LINEAR_AND_ANGULAR) ? m_maxTurnVelocity : m_maxVelocity;
}

inline void hkaiMovementProperties::setMaxTurnVelocity( hkReal newMaxTurnVel )
{
	HK_WARN_ONCE_ON_DEBUG_IF( m_kinematicConstraintType != CONSTRAINTS_LINEAR_AND_ANGULAR, 0x148de184, "Updating m_maxTurnVelocity, but angular constraints are disabled." );
	m_maxTurnVelocity = newMaxTurnVel;
}

inline hkReal hkaiMovementProperties::getTurnRadiusAtSpeed( hkReal speed ) const
{
	if (m_kinematicConstraintType == CONSTRAINTS_LINEAR_AND_ANGULAR)
	{
		return speed / m_maxAngularVelocity;
	}
	else
	{
		return 2.0f * (speed*speed) / (m_maxAcceleration+m_maxDeceleration);
	}
}

inline void hkaiMovementProperties::setMaxAngularVelocity( hkReal newMaxAngVel )
{
	m_maxAngularVelocity = newMaxAngVel;
}

inline hkReal hkaiMovementProperties::getMaxAngularVelocity() const
{
	return m_maxAngularVelocity;
}

inline void hkaiMovementProperties::setMaxAngularAcceleration( hkReal newMaxAngAccel )
{
	HK_WARN_ONCE_ON_DEBUG_IF( m_kinematicConstraintType != CONSTRAINTS_LINEAR_AND_ANGULAR, 0x148de184, "Updating m_maxAngularAcceleration, but angular constraints are disabled." );
	m_maxAngularAcceleration = newMaxAngAccel;
}

inline hkReal hkaiMovementProperties::getMaxAngularAcceleration() const
{
	HK_WARN_ONCE_ON_DEBUG_IF( m_kinematicConstraintType != CONSTRAINTS_LINEAR_AND_ANGULAR, 0x148de184, "m_maxAngularAcceleration requested, but angular constraints are disabled." );
	return m_maxAngularAcceleration;
}

inline void hkaiMovementProperties::setReasonableProperties( hkReal maxSpeed, hkReal maxAcceleration, hkReal turnRadiusIn )
{
	m_minVelocity = maxSpeed * 0.0035f; // Approximates old min/max ratio
	m_maxVelocity = maxSpeed;

	m_maxAcceleration = maxAcceleration;
	m_maxDeceleration = m_maxAcceleration; // Assume the two are the same

	hkReal turnRadius, turnSpeed;
	if(turnRadiusIn > 0) // enforce predefined turn radius
	{
		turnRadius = turnRadiusIn;
		turnSpeed = hkMath::sqrt(maxAcceleration * turnRadius); // max speed which respects max centripetal acceleration
		turnSpeed = hkMath::min2(turnSpeed, maxSpeed); // but don't speed up to turn
	}
	else // turn at max speed
	{
		turnRadius = maxSpeed*maxSpeed / maxAcceleration; // min radius which can be achieved without slowing down
		turnSpeed = maxSpeed;
	}

	m_leftTurnRadius = turnRadius;
	m_rightTurnRadius = turnRadius;

	m_maxTurnVelocity = turnSpeed;

	hkReal angularVelocityDuringTurn = turnSpeed / turnRadius;

	m_maxAngularVelocity = angularVelocityDuringTurn;

	m_maxAngularAcceleration = 1000.0f; // effectively unlimited
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
