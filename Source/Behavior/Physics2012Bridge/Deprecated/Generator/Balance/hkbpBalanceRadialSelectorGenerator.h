/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKBP_BALANCE_RADIAL_SELECTOR_GENERATOR_H
#define HKBP_BALANCE_RADIAL_SELECTOR_GENERATOR_H

#include <Behavior/Utilities/Generator/RadialSelector/hkbRadialSelectorGenerator.h>
#include <Behavior/Physics2012Bridge/Deprecated/Modifier/Balance/hkbpCheckBalanceModifier.h>

extern const class hkClass hkbpBalanceRadialSelectorGeneratorClass;

/// This example custom generator uses information from an hkbCheckBalanceModifier to controls
/// its behavior as a subclass of hkbRadialSelectorGenerator.
class hkbpBalanceRadialSelectorGenerator : public hkbRadialSelectorGenerator
{
	//+version(1)

	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbpBalanceRadialSelectorGenerator();

	protected:

		hkbpBalanceRadialSelectorGenerator( const hkbpBalanceRadialSelectorGenerator& gen );

	public:

			/// Set the hkbpCheckBalanceModifier used to determine which child generators to blend.
		void setCheckBalanceModifier( hkbpCheckBalanceModifier* checkBalanceModifier );

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void activate( const hkbContext& context ) HK_OVERRIDE;

			// hkbNode interface implementation.
		virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

		///////////////
		// properties
		///////////////

		/// The X axis (in model space) to use to generate an angle.
		/// These should be the axes that make up the ground plane in model space.
		int m_xAxisMS; //+default(0)+hk.RangeInt32(absmin=0,absmax=2)

		/// The Y axis (in model space) to use to generate an angle
		/// these should be the axes that make up the ground plane in model space.
		int m_yAxisMS; //+default(1)+hk.RangeInt32(absmin=0,absmax=2)

	private:
		
		// the source of balance information
		hkRefPtr<hkbpCheckBalanceModifier> m_checkBalanceModifier;

	public:

		hkbpBalanceRadialSelectorGenerator( hkFinishLoadedObjectFlag flag ) : hkbRadialSelectorGenerator(flag), m_checkBalanceModifier(flag) {}

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
