/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_CONTROLLED_REACH_MODIFIER_H
#define HKBP_CONTROLLED_REACH_MODIFIER_H

#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbpControlledReachModifierClass;

class hkpRigidBody;

/// An example custom modifier to reach with the arms, under control of some user parameters.
class hkbpControlledReachModifier : public hkbModifier
{
	public:

		//+version(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpControlledReachModifier();
		
		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

		//////////////////////////
		// hkbModifier interface
		//////////////////////////

			// hkbModifier interface implementation
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) HK_OVERRIDE;

			/// Set the target being reached toward.
		void setTarget( hkpRigidBody* target );

	private:
	
		hkpRigidBody* m_target;			//+nosave+default(HK_NULL)

		hkReal m_time;					//+nosave+default(0.0f)

	public:

		///////////////
		// properties
		///////////////

			/// A very simple linear attack curve.
		hkReal m_fadeInStart;
		hkReal m_fadeInEnd;
		hkReal m_fadeOutStart;
		hkReal m_fadeOutEnd;

			/// The duration during which the hand Ik data fades out when there is no target.
		hkReal m_fadeOutDuration; //default(0.0f)

			/// Only reach if target is within this angle measured from forward direction.
		hkReal m_sensorAngle;

	private:

			// maximum number of hands the modifier can operate on
		enum { MAX_LIMBS = 2 };

	public:
			/// Index of the hand in the boneInfo. 
		hkInt16 m_handIndex[MAX_LIMBS];	

			/// Indicates which hand (or both) are enabled for reaching.
		hkBool m_isHandEnabled[MAX_LIMBS];

	public:

		hkbpControlledReachModifier( hkFinishLoadedObjectFlag flag );

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
