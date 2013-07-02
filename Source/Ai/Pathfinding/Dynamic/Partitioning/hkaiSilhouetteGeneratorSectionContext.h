/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_SILHOUETTE_GENERATOR_SECTION_CONTEXT
#define HK_AI_SILHOUETTE_GENERATOR_SECTION_CONTEXT

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/hkaiSilhouetteGenerator.h>

	/// State context that relates a silhouette generator to a particular nav mesh section
struct hkaiSilhouetteGeneratorSectionContext
{
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_NAVMESH,hkaiSilhouetteGeneratorSectionContext);

	inline hkaiSilhouetteGeneratorSectionContext();
	inline hkaiSilhouetteGeneratorSectionContext( hkFinishLoadedObjectFlag f);
	inline ~hkaiSilhouetteGeneratorSectionContext();

	inline const hkaiSilhouetteGenerator* getGenerator() const;
	inline int getGeneratorSize() const;
	
	inline void setGenerator( const hkaiSilhouetteGenerator* gen );
	inline void updateGeneratorSize();

		/// Relative transform of the generator (with respect to this section) the last time the overlap
		/// set was updated. This is used to do lazy updates of the overlapping face set, when the
		/// generator has moved by a significant amount.
	hkQTransform m_lastRelativeTransform;

protected:
		/// The silhouette generator for this data
	const hkaiSilhouetteGenerator* m_generator;

		/// The size of the silhouette generator. This is needed for DMA transfers.
	int m_generatorSize; //+nosave

public:
		/// The generator created silhouette(s) on this section last frame
	hkBool m_generatedLastFrame;

		/// The generator will create silhouette(s) on this section this frame
	hkBool m_generatingThisFrame;
};

#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiSilhouetteGeneratorSectionContext.inl>

#endif // HK_AI_SILHOUETTE_GENERATOR_SECTION_CONTEXT

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
