/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_SILLOUETTE_MERGER_UTILS_H
#define HK_SILLOUETTE_MERGER_UTILS_H

#include <Ai/Pathfinding/Dynamic/Silhouette/hkaiCompoundSilhouette.h>

extern const class hkClass hkaiSilhouetteMergerUtilsClass;

struct hkaiSilhouetteGenerationParameters;
struct hkaiSilhouetteGeneratorSectionContext;

/// Builds compound silhouettes and populates silhouette scenes.
class hkaiSilhouetteMergerUtils
{
 public:

		//
		// Silhouette creation
		//

		/// No merging: Makes one 'compound' for every simple silhouette
	static void HK_CALL createNonmergedCompoundSilhouettes( const hkaiSilhouetteGeneratorSectionContext* const* generatorContexts, int numGenerators, const hkaiSilhouetteGenerationParameters& params, hkArray<hkaiCompoundSilhouette>::Temp& compoundSilhouettesOut );
};

#endif // HK_SILLOUETTE_MERGER_H

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
