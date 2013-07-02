/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

HK_FORCE_INLINE hkbClipTrigger::hkbClipTrigger()
:	m_localTime(0.0f),
	m_event(hkbEvent::EVENT_ID_NULL),
	m_relativeToEndOfClip(false),
	m_acyclic(false),
	m_isAnnotation(false)
{
}

HK_FORCE_INLINE hkbClipTrigger::hkbClipTrigger(	hkReal localTime, 
												const hkbEventBase& event, 
												bool relativeToEndOfClip, 
												bool acyclic,
												bool isAnnotation )
:	m_localTime(localTime),
	m_event(event),
	m_relativeToEndOfClip(relativeToEndOfClip),
	m_acyclic(acyclic),
	m_isAnnotation(isAnnotation)
{
}

HK_FORCE_INLINE hkbClipTrigger::hkbClipTrigger( const hkbClipTrigger& trigger )
:	m_localTime(trigger.m_localTime),
	m_event(trigger.m_event),
	m_relativeToEndOfClip(trigger.m_relativeToEndOfClip),
	m_acyclic(trigger.m_acyclic),
	m_isAnnotation(trigger.m_isAnnotation)
{
}

HK_FORCE_INLINE hkReal hkbClipTrigger::getTriggerTime( hkReal duration ) const
{
	if( m_acyclic )
	{
		return m_localTime;
	}
	else
	{
		hkReal time = ( m_relativeToEndOfClip ? ( duration + m_localTime ) : m_localTime );

		// always move the cyclic trigger onto the clip interval if it is outside
		if ( time < 0.0f ) 
		{
			time = 0.0f;
		}

		if ( time > duration )
		{
			time = duration;
		}

		return time;
	}
}

HK_FORCE_INLINE hkReal hkbClipGenerator::getEndOfClipLocalTime() const 
{
	// The local time does not take into consideration the cropping, but getDuration does.
	hkReal duration = getDurationLocalTime();

	return duration + m_animationControl->getCropStartAmountLocalTime();
}

HK_FORCE_INLINE hkaDefaultAnimationControl* hkbClipGenerator::getAnimationControl() const
{
	return m_animationControl;
}

HK_FORCE_INLINE bool hkbClipGenerator::isPingPongBackwards() const
{
	return m_pingPongBackward;
}

HK_FORCE_INLINE const hkArray<struct hkbClipGenerator::Echo>& hkbClipGenerator::getEchos() const
{
	return m_echos;
}

HK_FORCE_INLINE const hkQsTransform& hkbClipGenerator::getExtractedMotion() const
{
	return m_extractedMotion;
}

HK_FORCE_INLINE hkReal hkbClipGenerator::offsetLocalTime( hkReal localTimeOffset ) const
{
	hkReal localTime;
	int loops;

	// The offseted local time is computed as a pure local time on the animation clip, independent of playback speed.  We
	// use getFutureTime() to handle looping, but we don't want to factor in playback speed since we need to maintain
	// a constant offset regardless of playback speed.  An added difficulty is that when the playback speed is zero,
	// getFutureTime() will not advance the time at all.  Therefore, we temporarily set the playback speed to 1
	// when computing the local time for this pose.
	{
		hkReal oldPlaybackSpeed = m_animationControl->getPlaybackSpeed();
		m_animationControl->setPlaybackSpeed(1.0f);
		m_animationControl->getFutureTime( localTimeOffset, localTime, loops );
		m_animationControl->setPlaybackSpeed( oldPlaybackSpeed );
	}

	// in single play mode, if we echo beyond the end of the clip then we just play the last pose
	if ( ( m_mode == MODE_SINGLE_PLAY ) && loops )
	{
		localTime = m_animationControl->getAnimationBinding()->m_animation->m_duration + m_animationControl->getCropStartAmountLocalTime();
	}

	return localTime;
}

HK_FORCE_INLINE bool hkbClipGenerator::hasMapper() const
{
	return ( m_mapperData != HK_NULL ); 
}

HK_FORCE_INLINE hkaDefaultAnimationControlMapperData* hkbClipGenerator::getMapperData() const
{
	return m_mapperData;
}

HK_FORCE_INLINE hkBool hkbClipGenerator::isActive() const
{
	return m_binding != HK_NULL;
}

HK_FORCE_INLINE void hkbClipGenerator::setAnimationBindingIndex( hkInt16 bindingIndex )
{
	m_animationBindingIndex = bindingIndex;
}

HK_FORCE_INLINE hkInt16 hkbClipGenerator::getAnimationBindingIndex() const
{
	return m_animationBindingIndex;
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
