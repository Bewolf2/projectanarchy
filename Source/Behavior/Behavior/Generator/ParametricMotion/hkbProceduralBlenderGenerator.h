/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PROCEDURAL_BLENDER_GENERATOR_H
#define HKB_PROCEDURAL_BLENDER_GENERATOR_H

#include <Behavior/Behavior/Generator/hkbGenerator.h>

class hkbBlenderGenerator;

/// A generator which wraps a blender generator and computes blend weights procedurally.
class hkbProceduralBlenderGenerator : public hkbGenerator
{
public:
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();
	//+version(1)
	hkbProceduralBlenderGenerator(hkbNodeType nodeType);
	virtual ~hkbProceduralBlenderGenerator();
	hkbProceduralBlenderGenerator(hkFinishLoadedObjectFlag flag);

protected:
	hkbProceduralBlenderGenerator(hkbProceduralBlenderGenerator const& other);

public:

	//////////////////////
	// hkbNode interface 
	//////////////////////

	// hkbNode interface implementation.
	virtual void activate( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual int getMaxNumChildren( GetChildrenFlags flags ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual void getChildren( GetChildrenFlags flags, ChildrenInfo& childrenInfo ) HK_OVERRIDE;

	/// Subclasses which override isValid() should call and return the base class
	/// implementation at the bottom of their implementation, if not returning false.
	virtual bool isValid( const hkbCharacter* character, hkStringPtr& errorString ) const HK_OVERRIDE;

	/// Subclasses which override precompute() should call the base class implementation
	/// at the top of their implementation.
	virtual void precompute( const hkbContext& context ) HK_OVERRIDE;

	// hkbNode interface implementation.
	virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const = 0;

	///////////////////////////
	// hkbGenerator interface
	///////////////////////////

	// hkbGenerator interface implementation.
	virtual void generate( const hkbContext& context, const hkbGeneratorOutput** activeChildrenOutput, hkbGeneratorOutput& output, hkReal timeOffset = 0.0f ) const HK_OVERRIDE;

	// hkbGenerator interface implementation.
	virtual bool canRecycleOutput() const HK_OVERRIDE;

	// hkbGenerator interface implementation.
	virtual void updateSync( const hkbContext& context ) HK_OVERRIDE;

	// hkbGenerator interface implementation.
	virtual void preUpdate( const hkbContext& context, hkReal timestep ) HK_OVERRIDE;

private:
	////////////////////////////////////////////////////////////
	/// Virtual interface, to be implemented by subclasses
	////////////////////////////////////////////////////////////
	/// Should return the number of child generators
	virtual int getNumGenerators() const = 0;

	/// Should return the child generator at the indicated index
	virtual hkbGenerator* getGenerator(int generatorIdx) const = 0;

	/// Called each frame to compute the new blend weights.
	virtual void computeBlendWeights(hkReal* blendWeightsOut) const = 0;

protected:
	/// Called to deallocate structures built in precompute(). Sublcasses which override cleanup()
	/// should call the base class implementation at the top of their implementation.
	virtual void cleanup();

	/// Get the template's internal blender.
	hkbBlenderGenerator* getBlender() const;
	/// Set the blender directly. This is used by subclasses' copyNode() functions.
	void setBlender(hkbBlenderGenerator* blender);

private:
	void createBlender();
	void destroyBlender();
	void updateWeights(const hkbContext& context);

private:
	/// Blender used to blend together the clips in the generator.
	hkRefPtr<hkbBlenderGenerator> m_blender; //+nosave
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
