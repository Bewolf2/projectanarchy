/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BEHAVIOR_REFERENCE_GENERATOR_H
#define HKB_BEHAVIOR_REFERENCE_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/BehaviorGraph/hkbBehaviorGraph.h>

extern const class hkClass hkbBehaviorReferenceGeneratorClass;

class hkbBehaviorGraph;

	/// This generator is a wrapper for an hkbBehaviorGraph.  It does not save the
	/// behavior when serialized.  Only the name is saved.  The convention is that
	/// the user must provide a behavior at runtime, using the name to
	/// identify which behavior to use.
	///
	/// You need to make sure to go through the linking process so that the referencing
	/// behavior graph and the referenced behavior graph can communicate events,
	/// attributes, and variables.  See hkbBehaviorLinkingUtils::linkBehavior() and
	/// hkbBehaviorLinkingUtils::linkBehaviors().
class hkbBehaviorReferenceGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBehaviorReferenceGenerator();

		hkbBehaviorReferenceGenerator( const hkbBehaviorReferenceGenerator& that );
		~hkbBehaviorReferenceGenerator();

		//////////////////////
		// hkbNode interface
		//////////////////////

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation.
		virtual void generate(	const hkbContext& context,
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

	public:

			/// Get the behavior.
		hkbBehaviorGraph* getBehavior() const { return m_behavior; }

			/// Set the behavior.
		void setBehavior( hkbBehaviorGraph* behavior );

	public:

			// the name of the behavior
		hkStringPtr m_behaviorName;	//+hkb.RoleAttribute("ROLE_DEFAULT","FLAG_HIDDEN")

	private:

			// the behavior
		hkRefPtr<hkbBehaviorGraph> m_behavior; //+nosave

	public:

		hkbBehaviorReferenceGenerator( hkFinishLoadedObjectFlag flag );
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
