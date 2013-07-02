/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

inline hkaiLocalSteeringInput::hkaiLocalSteeringInput()
{
}

inline hkaiLocalSteeringInput::hkaiLocalSteeringInput(hkFinishLoadedObjectFlag f)
{
}


inline void hkaiLocalSteeringInput::validate( bool callCheckDeterminism ) const
{
	HK_ASSERT2(0x746c65a9, m_character, "Local steering input m_character is not set");
	if(!m_enableLocalSteering)
	{
		// no other fields are read if local steering is disabled
		return;
	}

	HK_ASSERT2(0x746c65a0, m_currentPosition.isOk<3>(), "Local steering input m_currentPosition is invalid");
	HK_ASSERT2(0x746c65a1, m_currentForward.isOk<3>(), "Local steering input m_currentForward is invalid");
	HK_ASSERT2(0x746c65a2, m_currentForward.isNormalized<3>(), "Local steering input m_currentForward is not normalized");
	HK_ASSERT2(0x746c65a3, m_currentVelocity.isOk<4>(), "Local steering input m_currentVelocity is invalid");
	HK_ASSERT2(0x746c65a4, m_desiredVelocity.isOk<3>(), "Local steering input m_desiredVelocity is invalid");
	HK_ASSERT2(0x746c65a5, m_desiredFutureDir.isOk<3>(), "Local steering input m_desiredFutureDir is invalid");
	if ( m_applyAvoidanceSteering )
	{
		HK_ASSERT2(0x746c65a6, m_desiredFutureDir.allExactlyEqualZero<3>() || m_desiredFutureDir.isNormalized<3>(), "Local steering input m_desiredFutureDir is not normalized");
	}
	HK_ASSERT2(0x746c65a7, hkMath::isFinite(m_distToLocalGoal) && m_distToLocalGoal >= 0.f, "Local steering input m_distToLocalGoal is invalid");
	HK_ASSERT2(0x746c65a8, hkMath::isFinite(m_timeToLocalGoal) && m_timeToLocalGoal >= 0.f, "Local steering input m_timeToLocalGoal is invalid");
	HK_ASSERT2(0x746c65a9, m_localGoalPlane.isOk<4>(), "Local steering input m_localGoalPlane is invalid");
	
#ifndef HK_PLATFORM_SPU // m_referenceFrame is CPU pointer
	if ( m_referenceFrame )
	{
		m_referenceFrame->validate();
	}
#endif

#ifdef HK_ENABLE_DETERMINISM_CHECKS
	if(callCheckDeterminism)
	{
		checkDeterminism();
	}
#endif
}

#define HKAI_CHECK_DETERMINISM_VECTOR3(ID, VEC) { hkVector4 v3; v3.setXYZ_0(VEC); hkCheckDeterminismUtil::checkMt(ID, v3); }

inline void hkaiLocalSteeringInput::checkDeterminism() const
{
	hkCheckDeterminismUtil::checkMt(0x5406f1c0, m_enableLocalSteering);
	if(!m_enableLocalSteering)
	{
		// no other fields are read if local steering is disabled
		return;
	}

	HKAI_CHECK_DETERMINISM_VECTOR3(0x5406f1d0, m_currentPosition);
	HKAI_CHECK_DETERMINISM_VECTOR3(0x5406f1d1, m_currentForward);
	HKAI_CHECK_DETERMINISM_VECTOR3(0x5406f1d2, m_currentVelocity);
	HKAI_CHECK_DETERMINISM_VECTOR3(0x5406f1d3, m_desiredVelocity);
	if ( m_referenceFrame )
	{
		m_referenceFrame->checkDeterminism();
	}
	HKAI_CHECK_DETERMINISM_VECTOR3(0x5406f1d4, m_desiredFutureDir);
	hkCheckDeterminismUtil::checkMt(0x5406f1d6, m_distToLocalGoal);
	hkCheckDeterminismUtil::checkMt(0x5406f1d7, m_timeToLocalGoal);
	hkCheckDeterminismUtil::checkMt(0x5406f1d8, m_isLocalGoalLast);
	hkCheckDeterminismUtil::checkMt(0x5406f1d9, m_applyAvoidanceSteering);
}
#undef HKAI_CHECK_DETERMINISM_VECTOR3

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
