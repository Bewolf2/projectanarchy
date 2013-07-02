/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_MODIFIER_GENERATOR_H
#define HKB_MODIFIER_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>
#include <Behavior/Behavior/Modifier/hkbModifier.h>

extern const class hkClass hkbModifierGeneratorClass;

/// A generator that wraps another generator and applies a modifier to the results.
///
/// When a method of hkbModifierGenerator is called, it first calls the same method
/// on the child generator.  It then calls the analogous method on the stored modifier.
class hkbModifierGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbModifierGenerator();

			/// Construction given a modifier and a generator.
		hkbModifierGenerator( hkbModifier* modifier, hkbGenerator* generator );

	private:

		hkbModifierGenerator( const hkbModifierGenerator& modGen );

	public:

		~hkbModifierGenerator();

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
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual void computePartitionInfo( const hkArray<hkbNodeChildInfo>& activeChildren, const hkbContext& context ) HK_OVERRIDE;

			// hkbGenerator interface implementation.
		virtual bool canRecycleOutput() const HK_OVERRIDE;

	public:

			/// Get the modifier that is doing the modifying.
		hkbModifier* getModifier() { return m_modifier; }

			/// Set the modifier that is doing the modifying.
		void setModifier( hkbModifier* mod );

			/// Get the generator whose output is being modified by the modifier.
		hkbGenerator* getGenerator() { return m_generator; }

			/// Set the generator whose output is being modified by the modifier.
		void setGenerator( hkbGenerator* gen );

	public:

			// The modifier being applied to a generator.
		hkRefPtr<hkbModifier> m_modifier;

			// The generator to which a modifier is being applied.
		hkRefPtr<hkbGenerator> m_generator;

	public:

			// Constructor for initialization of vtable fixup
		hkbModifierGenerator( hkFinishLoadedObjectFlag flag );
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
