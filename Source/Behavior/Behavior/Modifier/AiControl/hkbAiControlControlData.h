/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_CONTROL_CONTROL_DATA_H
#define HKB_AI_CONTROL_CONTROL_DATA_H

/// Blendable elements of hkbAiControlControlData
struct hkbAiControlControlDataBlendable
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiControlControlDataBlendable );
	HK_DECLARE_REFLECTION();

	hkbAiControlControlDataBlendable()
		: m_desiredSpeed(5.0f)
		, m_maximumSpeed(5.0f)
	{}

	hkbAiControlControlDataBlendable( hkFinishLoadedObjectFlag flag ) {}

	/// The current desired speed for the character. Local steering will
	/// attempt to output requested velocities close to this speed (other than
	/// when slowing to the goal).
	hkReal m_desiredSpeed; //+default(5.0f)

	/// The current maximum speed for the character. Local steering will
	/// never output a velocity with a magnitude greater than this value.
	hkReal m_maximumSpeed; //+default(5.0f)
};

/// Non-blendable elements of hkbAiControlControlData
struct hkbAiControlControlDataNonBlendable
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiControlControlDataNonBlendable );
	HK_DECLARE_REFLECTION();

	hkbAiControlControlDataNonBlendable()
		: m_canControl(true)
	{}

	hkbAiControlControlDataNonBlendable( hkFinishLoadedObjectFlag flag ) {}

	/// Indicates that per-frame control of the character's velocity is
	/// possible. Must be set to true in order for the hkbAiControlModifier to
	/// output information about the requested velocity. Should be set to false
	/// while the character is not controllable (for instance, while it is
	/// jumping, or being knocked over).
	hkBool m_canControl; //+default(true)

	
	hkUint8 m_padding[sizeof(hkReal)-sizeof(hkBool)]; //+nosave
};

/// The collected control data which is provided by an hkbAiControlModifier
/// to the hkbAiControlDriver via devoted tracks in the hkbGeneratorOutput. 
/// That is, the "ControlData" for controlling "AiControl" itself.
struct hkbAiControlControlData
{
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbAiControlControlData );
	HK_DECLARE_REFLECTION();

	hkbAiControlControlData() {}

	hkbAiControlControlData( hkFinishLoadedObjectFlag flag )
	: m_blendable(flag)
	, m_nonBlendable(flag)
	{}

	hkbAiControlControlDataBlendable m_blendable;
	hkbAiControlControlDataNonBlendable m_nonBlendable;
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
