/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_RADIAL_SELECTOR_GENERATOR_H
#define HKB_RADIAL_SELECTOR_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>

extern const class hkClass hkbRadialSelectorGeneratorGeneratorPairClass;

extern const class hkClass hkbRadialSelectorGeneratorGeneratorInfoClass;

extern const class hkClass hkbRadialSelectorGeneratorClass;

/// An example custom generator for blending child generators based on a planar angle.
/// The functionality of this node is subsumed by the parametric blending mode of the
/// hkbBlenderGenerator, so you are better of using the hkbBlenderGenerator, which is
/// part of the hkbBehavior library.
class hkbRadialSelectorGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbRadialSelectorGenerator();

	protected:

		hkbRadialSelectorGenerator( const hkbRadialSelectorGenerator& gen );

	public:

		~hkbRadialSelectorGenerator();

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////////////////
		// hkbGenerator interface
		///////////////////////////

			// hkbGenerator interface implementation
		virtual void generate(	const hkbContext& context, 
								const hkbGeneratorOutput** activeChildrenOutput,
								hkbGeneratorOutput& output,
								hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual bool canRecycleOutput() const HK_OVERRIDE;

			// hkbGenerator interface implementation
		virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

			/// Description of a generator that is at a particular angle.
		class GeneratorInfo
		{
			public:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRadialSelectorGenerator::GeneratorInfo );
				HK_DECLARE_REFLECTION();

				class hkbGenerator* m_generator; //+default(HK_NULL)
				
					/// between 0 and 2*pi
				hkReal m_angle; //+default(0)+hk.RangeReal(absmin=0.0,absmax=6.283185)

					/// rate of motion along radial spoke (meters per second)
				hkReal m_radialSpeed; //+default(0)+hk.RangeReal(absmin=0,absmax=100,softmax=10)
		};

			/// Description of a pair of generators that should be blended to cover an interval of angles.
		class GeneratorPair
		{
			public:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR, hkbRadialSelectorGenerator::GeneratorPair );
				HK_DECLARE_REFLECTION();

				/// The generator info for each endpoint of this pair.  If you want a single generator
				/// at a single angle, you can set m_generators[1].m_generator to HK_NULL.
				class GeneratorInfo m_generators[2];

				/// The minimum angle for which this pair is valid.  Note that this is different
				/// from the angles in the GeneratorInfo to allow for cropping.  
				hkReal m_minAngle; //+default(0)+hk.RangeReal(absmin=0.0,absmax=6.283185)

				/// The maximum angle for which this pair is valid.  Note that this is different
				/// from the angles in the GeneratorInfo to allow for cropping.
				hkReal m_maxAngle; //+default(0)+hk.RangeReal(absmin=0.0,absmax=6.283185)
		};

		///////////////
		// properties
		///////////////

			/// The pairs of generators that cover the circle of possible angles.
		hkArray<class GeneratorPair> m_generatorPairs;

			/// The current angle selected.
		hkReal m_angle; //+default(0)+hk.RangeReal(absmin=0.0,absmax=6.283185)

			/// The current radius selected.  This indicates the position of the COM of the 
			/// character.  The animation will be advanced so that it begins at the appropriate
			/// COM.
		hkReal m_radius; //+default(0)+hk.RangeReal(absmin=0,absmax=10)

	private:

		// the index of the current interval being used
		int m_currentGeneratorPairIndex; //+nosave+default(-1)

		// if the current angle is outside any interval, then this is the nearest interval endpoint (0 or 1)
		int m_currentEndpointIndex; //+nosave+default(-1)

		// the current fraction along the current interval (between 0 and 1)
		hkReal m_currentFraction; //+nosave

		// whether or not a pair has been chosen already
		hkBool m_hasSetLocalTime; //+nosave

		// make the layout test succeed
		hkInt8 m_pad[4]; //+nosave

	private:

		// finds the interval and/or endpoint that overlaps or is closest to m_angle
		void chooseBestPair();

		// Computes how far along from minAngle to maxAngle that angle lies.  
		// If less than 0 or more than 1, then angle is outside the interval.
		static hkReal computeFractionOfInterval( hkReal angle, hkReal minAngle, hkReal maxAngle );

	public:

		hkbRadialSelectorGenerator( hkFinishLoadedObjectFlag flag );

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
