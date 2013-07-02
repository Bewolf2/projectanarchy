/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkReal hkaiCharacter::getDesiredSpeed() const
{
	return m_desiredSpeed;
}

inline void hkaiCharacter::setDesiredSpeed( hkReal speed )
{
	HK_ASSERT2(0x30fa05de, hkMath::isFinite(speed), "Character desired speed is invalid");
	HK_ASSERT2(0x30fa05df, speed >= 0.f, "Character desired speed cannot be negative");
	m_desiredSpeed = speed;
}

inline const hkVector4& hkaiCharacter::getPosition() const
{
	return m_position;
}

inline void hkaiCharacter::setPosition( hkVector4Parameter position )
{
	HK_ASSERT2(0x30fa05e0, position.isOk<3>(), "Character position is invalid");
	m_position = position;
}

inline const hkVector4& hkaiCharacter::getVelocity() const
{
	return m_velocity;
}

inline void hkaiCharacter::setUp( hkVector4Parameter up )
{
	HK_ASSERT2(0x30fa05e0, up.isOk<3>() && (up.isNormalized<3>() || up.length<3>() < hkSimdReal_Eps), "Character position is invalid");
	m_up = up;
}

inline const hkVector4& hkaiCharacter::getUp() const
{
	return m_up;
}


inline void hkaiCharacter::setLinearAndAngularVelocity( hkVector4Parameter velocity )
{
	HK_ASSERT2(0x30fa05e1, velocity.isOk<4>(), "Character linear/angular velocity is invalid");
	m_velocity = velocity;
}
inline void hkaiCharacter::setLinearVelocity( hkVector4Parameter velocity )
{
	HK_ASSERT2(0x30fa05e2, velocity.isOk<3>(), "Character linear velocity is invalid");
	m_velocity.setXYZ( velocity );
}

inline const hkVector4& hkaiCharacter::getForward() const
{
	return m_forward;
}
inline void hkaiCharacter::setForward(hkVector4Parameter forward)
{
	HK_ASSERT2(0x30fa05e3, forward.isOk<3>(), "Character forward vector is invalid");
	HK_ASSERT2(0x30fa05e4, forward.isNormalized<3>(), "Character forward vector is not normalized");
	m_forward = forward;
}

inline void hkaiCharacter::setSensorAabb(const hkAabb& aabb)
{
	HK_ASSERT(0x197d098f, aabb.isValid() );

	HK_WARN_ONCE(0x20365912, "hkaiCharacter::setSensorAabb is deprecated and writes into non-owned memory. Set the sensor AABB in AvoidanceProperties instead.");
	getAvoidancePropertiesRW()->m_localSensorAabb = aabb;
}

inline void hkaiCharacter::getSensorAabb(hkAabb& aabb) const
{
	hkAabb localAabb = getAvoidanceProperties()->m_localSensorAabb;
	aabb.m_min.setAdd( localAabb.m_min, m_position );
	aabb.m_max.setAdd( localAabb.m_max, m_position );
}

inline void hkaiCharacter::getAdaptiveSensorAabb(hkAabb& aabb) const
{
	hkAabb localAabb = getAvoidanceProperties()->m_localSensorAabb;

	hkVector4 adaptiveRange; adaptiveRange.setAll(m_adaptiveRanger.getCurRange());
	adaptiveRange.setMax(adaptiveRange, getUp());

	aabb.m_min.setAddMul( m_position, localAabb.m_min, adaptiveRange);
	aabb.m_max.setAddMul( m_position, localAabb.m_max, adaptiveRange);
}

inline void hkaiCharacter::getLocalSensorAabb(hkAabb& aabb) const
{
	aabb = getAvoidanceProperties()->m_localSensorAabb;
}

inline void hkaiCharacter::getAdaptiveLocalSensorAabb(hkAabb& aabb) const
{
	hkAabb localAabb = getAvoidanceProperties()->m_localSensorAabb;

	hkVector4 adaptiveRange; adaptiveRange.setAll(m_adaptiveRanger.getCurRange());
	adaptiveRange.setMax(adaptiveRange, getUp());

	aabb.m_min.setMul( localAabb.m_min, adaptiveRange);
	aabb.m_max.setMul( localAabb.m_max, adaptiveRange);
}

inline void hkaiCharacter::adaptSensorAabb(hkUint32 numSensedCharacters)
{
	m_adaptiveRanger.updateRange(getAvoidanceProperties()->m_maxNeighbors, numSensedCharacters);
}


inline int hkaiCharacter::getNumBehaviorListeners() const
{
	return m_behaviorListeners.getSize();
}

inline hkaiCharacter::BehaviorListener* hkaiCharacter::getBehaviorListener( int listenerIdx ) const
{
	return m_behaviorListeners[listenerIdx];
}

inline hkaiCharacter::BehaviorListener* hkaiCharacter::getBehaviorListener() const
{
	HK_WARN_ONCE(0x715a6121, "The zero-argument getBehaviorListener() is deprecated and will be removed in a future release. Use the one-argument getBehaviorListener() instead.");
	HK_ASSERT2(0x1f1f8df5, getNumBehaviorListeners() < 2, "The zero-argument getBehaviorListener() must not be used if multiple behavior listeners are added to a character.");
	if(getNumBehaviorListeners() == 0)
	{
		return HK_NULL;
	}
	else
	{
		return getBehaviorListener(0);
	}
}

inline void hkaiCharacter::addBehaviorListener( BehaviorListener* listener )
{
	HK_ASSERT2(0x7b5ffc22, listener != HK_NULL, "NULL listeners must not be added to a character");
	HK_ASSERT2(0x243ced3a, m_behaviorListeners.indexOf(listener) == -1, "A listener must not be added to the same character twice");

	m_behaviorListeners.pushBack(listener);
}

inline void hkaiCharacter::removeBehaviorListener( BehaviorListener* listener )
{
	HK_ASSERT2(0x243ced3a, m_behaviorListeners.indexOf(listener) != -1, "A listener to be removed must be one of the character's listeners");

	m_behaviorListeners.removeAllAndCopy(listener);
}

inline void hkaiCharacter::setBehaviorListener( BehaviorListener* listener )
{
	HK_WARN_ONCE(0x715a6122, "setBehaviorListener() is deprecated and will be removed in a future release. Use addBehaviorListener instead.");
	HK_ASSERT2(0x1f1f8df5, getNumBehaviorListeners() < 2, "setBehaviorListener() must not be used if multiple behavior listeners are added to a character.");

	m_behaviorListeners.clear();

	if(listener != HK_NULL)
	{
		addBehaviorListener(listener);
	}
}

inline hkaiCharacter::State hkaiCharacter::getState( ) const
{
	return m_state;
}

inline void hkaiCharacter::setState( State state )
{
	m_state = state;
}

inline hkUint16 hkaiCharacter::getAvoidanceType( ) const
{
	return m_avoidanceType;
}

inline void hkaiCharacter::setAvoidanceType( hkUint16 avoidanceType )
{
	m_avoidanceType = avoidanceType;
}

inline hkaiCharacter::AvoidanceEnabledMask hkaiCharacter::getAvoidanceEnabledMask() const
{
	return m_avoidanceEnabledMask;
}

inline void hkaiCharacter::setAvoidanceEnabledMask( AvoidanceEnabledMask mask )
{
	m_avoidanceEnabledMask = mask;
}

inline hkBool32 hkaiCharacter::isAvoidanceEnabled() const
{
	return getAvoidanceEnabledMask().anyIsSet( AVOID_ALL );
}

inline hkBool32 hkaiCharacter::areConstraintsEnabled() const
{
	return getAvoidanceProperties()->m_movementProperties.m_kinematicConstraintType != hkaiAvoidanceSolver::MovementProperties::CONSTRAINTS_NONE;
}

inline  hkaiCharacter::AvoidanceState hkaiCharacter::getAvoidanceState() const
{
	return ( m_avoidanceState <= 0.5f) ? AVOIDANCE_SUCCESS : AVOIDANCE_FAILURE;
}

inline const hkaiAstarCostModifier* hkaiCharacter::getCostModifier() const
{
	return m_costModifier;
}

inline const hkaiAstarEdgeFilter* hkaiCharacter::getEdgeFilter() const
{
	return m_edgeFilter;
}

inline const hkaiSpatialQueryHitFilter* hkaiCharacter::getQueryHitFilter() const
{
	return m_hitFilter;
}

/// Gets the hit filter the character will use for spatial queries
inline const hkaiLocalSteeringFilter* hkaiCharacter::getSteeringFilter() const
{
	return m_steeringFilter;
}

inline hkUint32 hkaiCharacter::getAgentFilterInfo() const
{
	return m_agentFilterInfo;
}

inline void hkaiCharacter::setAgentFilterInfo( hkUint32 filterInfo )
{
	m_agentFilterInfo = filterInfo;
}

inline hkUint32 hkaiCharacter::getAgentPriority() const
{
	return m_agentPriority;
}

inline void hkaiCharacter::setAgentPriority( hkUint32 priority )
{
	m_agentPriority = priority;
}

inline const hkaiAvoidanceSolver::AvoidanceProperties* hkaiCharacter::getAvoidanceProperties() const
{
	return m_avoidanceProperties;
}

inline hkaiAvoidanceSolver::AvoidanceProperties* hkaiCharacter::getAvoidancePropertiesRW()
{
	return const_cast<hkaiAvoidanceSolver::AvoidanceProperties*> (m_avoidanceProperties.val());
}


inline hkReal hkaiCharacter::getRadius() const
{
	return m_radius;
}

inline void hkaiCharacter::setRadius( hkReal r )
{
	HK_ASSERT2(0x30fa05e5, hkMath::isFinite(r), "Character radius is invalid");
	HK_ASSERT2(0x30fa05e6, r >= 0.f, "Character radius cannot be negative");
	m_radius = r;
}

inline hkaiPackedKey hkaiCharacter::getCurrentNavMeshFace() const
{
	return m_currentNavMeshFace;
}

inline void hkaiCharacter::setCurrentNavMeshFace(hkaiPackedKey faceKey)
{
	m_currentNavMeshFace = faceKey;
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
