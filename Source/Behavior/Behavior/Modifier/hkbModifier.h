/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MODIFIER_H
#define HKB_MODIFIER_H

#include <Behavior/Behavior/Node/hkbNode.h>
#include <Behavior/Behavior/Generator/hkbGeneratorPartitionInfo.h>

class hkbContext;
class hkbGeneratorOutput;
class hkbGeneratorSyncInfo;

extern const hkClass hkbModifierClass;

	/// An interface to an object that modifies a pose.
class hkbModifier : public hkbNode
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbModifier( hkbNodeType nodeType ) : hkbNode(nodeType), m_enable(true) {}

	protected:

		hkbModifier( const hkbModifier& mod ) : hkbNode( mod ), m_enable(mod.m_enable) {}

	public:

			/// Modify an output pose and reference frame.  Must be called between calls to activate() and deactivate().
		virtual void modify( const hkbContext& context, hkbGeneratorOutput& inOut ) {}

			/// Modify an output pose and reference frame, including a time offset.  Must be called between calls to activate() and deactivate().
			/// Only implement this function if predictive modify actually makes sense.  Also prediction should be stateless, so ensure that predictive
			/// modify does not change the state of the modifier.
		virtual void predictiveModify( const hkbContext& context, hkbGeneratorOutput& inOut, hkReal timeOffset ) {}

			/// Modify the sync info as it passes through this modifier.
		virtual void modifySyncInfo( const hkbContext& context, hkbGeneratorSyncInfo& syncInfo ) {}

			/// Modify the sync info as it passes through this modifier.
		virtual void modifyPartitionInfo( const hkbContext& context, hkbGeneratorPartitionInfo& partitionInfo ) {}

		///////////////
		// properties
		///////////////

			/// If this is true the modifier will be active.
		hkBool m_enable;	//+default(true)
							//+hk.Description("If this is true the modifier will be active.")

	private:

			// make the layout test succeed
		hkBool m_padModifier[3];	//+nosave

	public:
		
		hkbModifier( hkFinishLoadedObjectFlag flag ) : hkbNode(flag) {}
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
