/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_ANIMATED_SKELETON_GENERATOR_H
#define HKB_ANIMATED_SKELETON_GENERATOR_H

#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Behavior/Behavior/Generator/hkbGenerator.h>

extern const class hkClass hkbAnimatedSkeletonGeneratorClass;

class hkaAnimatedSkeleton;

/// An example custom generator that plays an hkaAnimatedSkeleton.  This generator is
/// primarily intended for testing.  Note that if you clone a hkbBehaviorGraph with 
/// one of these in it, the stored hkaAnimatedSkeleton will not be cloned or copied.  
/// You will have to set it again.  Also, synchronization is not supported. 
class hkbAnimatedSkeletonGenerator : public hkbGenerator
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
		HK_DECLARE_REFLECTION();

		hkbAnimatedSkeletonGenerator();

	protected:

		hkbAnimatedSkeletonGenerator( const hkbAnimatedSkeletonGenerator& gen );

	public:

		//////////////////////
		// hkbNode interface 
		//////////////////////

			// hkbNode interface implementation.
		virtual void update( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

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


public:

			/// Set the animated skeleton to be played.
		void setAnimatedSkeleton( hkaAnimatedSkeleton* animatedSkeleton );

	private:

			/// The animated skeleton to be played.
		hkRefPtr<hkaAnimatedSkeleton> m_animatedSkeleton; //+nosave

	public:

		hkbAnimatedSkeletonGenerator( hkFinishLoadedObjectFlag flag );

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
