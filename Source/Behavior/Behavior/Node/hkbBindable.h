/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_BINDABLE_H
#define HKB_BINDABLE_H

#include <Behavior/Behavior/Variables/hkbVariableBindingSet.h>

extern const class hkClass hkbBindableClass;

class hkbBindable;

	/// An abstract class for collecting hkbBindables.
class hkbBindableCollector
{
	public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbBindableCollector);
			/// Called on each hkbBindable when you pass this collector into hkbBindable::collectBindables().
		virtual void collectBindable( hkbBindable* bindable ) = 0;

		virtual ~hkbBindableCollector() {}
};

	/// An object that can be bound to behavior variables.
class hkbBindable : public hkReferencedObject
{
	public:
		//+vtable(1)

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbBindable();

	protected:

		hkbBindable( const hkbBindable& bindable );

	public:

		~hkbBindable();

			/// Collect this bindable and all of its child bindables.
			/// Override this if your object points to hkbBindables that it owns,
			/// in which case it should recur on all of the children.
		virtual void collectBindables( hkbBindableCollector& collector );

	public:
			/// Copy variables to properties, or vice-versa.
			/// Returns whether or not there are any output bindings.
		virtual hkBool32 copyVariablesToMembers( hkbSyncVariableSetup& syncVariableSetup, hkbBehaviorGraph& rootBehavior, bool reverse );

			/// Copies the variable that is bound to the m_enable member, if there is one.
		virtual void copyVariableToEnable( hkbSyncVariableSetup& syncVariableSetup, hkbBehaviorGraph& rootBehavior );

			/// Set the bindings from variables to this node.
		void setVariableBindings( hkbVariableBindingSet* bindings );

			/// Get the bindings from variables to this node.
		hkbVariableBindingSet* getVariableBindings() const;
		
			/// Whether there are any bindings to this node.
		bool hasBindings() const;

			/// Whether an m_enable member of this bindable has changed since the last call to clearHasEnableChanged().
		bool hasEnableChanged() const;

			/// Inform this bindable that m_enable has changed.
		void orHasEnableChanged( bool b );

			/// Set m_hasEnableChanged to false indicating that the bindable is up to date.
		void clearHasEnableChanged();

	protected:

			/// Util for copying variables to members
		static hkBool32 copyVariablesToMembersInternal(
			hkbBindable* bindable,
			hkbSyncVariableSetup& syncVariableSetup,
			hkbVariableBindingSet& variableBindingSet,
			hkbBehaviorGraph& rootBehavior,
			bool reverse );

			/// Util for copying variables to enable
		static void copyVariableToEnableInternal(
			hkbBindable* bindable,
			hkbSyncVariableSetup& syncVariableSetup,
			hkbVariableBindingSet& variableBindingSet,
			hkbBehaviorGraph& rootBehavior );

	private:
	
			// Builds the cache of nested bindables.
		void cacheBindables();

			// the variable bindings
		hkRefPtr<hkbVariableBindingSet> m_variableBindingSet;

			// Used by cacheBindables() to process all hkbBindables.
		class CacheBindablesCollector : public hkbBindableCollector
		{
			public:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BEHAVIOR,hkbBindable::CacheBindablesCollector);

				virtual void collectBindable( hkbBindable* bindable ) HK_OVERRIDE;
				
				hkbBindable* m_rootBindable;
		};

			// A cache of all of the nested bindables that have bindings.
			// Note that this bindable is excluded from its own cache because
			// the common case is no nested bindables, and so we won't waste the
			// array memory and extra cache miss in accessing it.
		hkArray<hkbBindable*> m_cachedBindables;	//+nosave

			// Whether the bindables have been cached yet.
		hkBool m_areBindablesCached; //+nosave

			// Whether a binding to an m_enable member has changed recently.
		hkBool m_hasEnableChanged; //+nosave

	public:

		hkbBindable( hkFinishLoadedObjectFlag flag ) : hkReferencedObject(flag), m_variableBindingSet(flag) {}

#if defined (HK_PLATFORM_HAS_SPU)
		friend class hkbSpuBehaviorUtils;
#endif
		friend class hkbTransitionEffect;
};

#include <Behavior/Behavior/Node/hkbBindable.inl>

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
