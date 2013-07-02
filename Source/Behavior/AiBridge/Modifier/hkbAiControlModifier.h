/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_AI_CONTROL_MODIFIER_H
#define HKB_AI_CONTROL_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>
#include <Behavior/Behavior/Modifier/AiControl/hkbAiControlControlData.h>

extern const class hkClass hkbAiControlModifierClass;

/// A modifier for reporting a character's current movement abilities to the AI
/// system, and thereby influence the requested velocity which the AI system
/// generates.
///
/// If multiple AI control modifiers are active, the most rootward modifier
/// takes precedence. If no AI control modifiers are active, the default 
/// control data defined in the AI steering modifier is used.
/// 
/// This modifier is used as an accessory to the AI steering modifier, and has
/// no effect if the AI steering modifier is not active.
class hkbAiControlModifier : public hkbModifier
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbAiControlModifier();

protected:

	hkbAiControlModifier( const hkbAiControlModifier& mod );

public:

	~hkbAiControlModifier();

	//////////////////////
	// hkbNode interface 
	//////////////////////

	// hkbNode interface implementation.
	virtual void activate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void deactivate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual hkbAiControlModifier* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	virtual bool isValid(const hkbCharacter* character, hkStringPtr& errorString) const;

	//////////////////////////
	// hkbModifier interface
	//////////////////////////

	// hkbModifier interface implementation
	virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

public:
	/// The control data to be sent to AI when this modifier is active.
	hkbAiControlControlData m_controlData;

public:
	hkbAiControlModifier(hkFinishLoadedObjectFlag flag);


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
