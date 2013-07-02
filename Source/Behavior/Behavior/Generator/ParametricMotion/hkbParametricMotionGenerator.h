/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKB_PARAMETRIC_MOTION_GENERATOR_H
#define HKB_PARAMETRIC_MOTION_GENERATOR_H

#include <Behavior/Behavior/Generator/ParametricMotion/hkbProceduralBlenderGenerator.h>
#include <Common/Base/Math/Vector/hkVector2.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>


struct hkGeometry;
class hkbClipGenerator;
class hkbBlenderGenerator;
class hkbMotionSpace;

extern const class hkClass hkbParametricMotionGeneratorClass;


class hkbParametricMotionGenerator : public hkbProceduralBlenderGenerator
{
public:
	//+version(2)
	HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BEHAVIOR );
	HK_DECLARE_REFLECTION();

	hkbParametricMotionGenerator();
	virtual ~hkbParametricMotionGenerator();
	hkbParametricMotionGenerator( hkFinishLoadedObjectFlag flag );

protected:
	/// Copy constructor
	hkbParametricMotionGenerator( const hkbParametricMotionGenerator& gen );

public:
	virtual bool isValid(const hkbCharacter* character, hkStringPtr& errorString) const HK_OVERRIDE;
	virtual void precompute(const hkbContext& context) HK_OVERRIDE;
	virtual hkbNode* cloneNode( hkbBehaviorGraph& rootBehavior ) const HK_OVERRIDE;

	/// Copy the node, possibly cleaning up
	void copyNode( const hkbParametricMotionGenerator& gen, bool shouldCleanup );

private:
	virtual int getNumGenerators() const HK_OVERRIDE;
	virtual hkbGenerator* getGenerator(int generatorIdx) const HK_OVERRIDE;
	virtual void computeBlendWeights(hkReal* blendWeightsOut) const HK_OVERRIDE;
	virtual void cleanup() HK_OVERRIDE;

public:
	enum MotionSpaceType
	{
		MST_UNKNOWN,
		MST_ANGULAR,
		MST_DIRECTIONAL
	};

	/// Motion space the generator operates in
	hkEnum<hkbParametricMotionGenerator::MotionSpaceType, hkInt8> m_motionSpace;

	/// Clip generates in the blend.  Each clip needs to have an hkaLocomotionReferenceFrame associated with it.
	hkArray< hkRefPtr<hkbGenerator> > m_generators; //+hk.Description("Child generators that make up the blend")

	/// DOC
	hkReal m_xAxisParameterValue;	//+default(0.0f)
	/// DOC
	hkReal m_yAxisParameterValue;	//+default(0.0f)

protected:
	/// Extents of the motion space domain
	hkAabb	m_paramsDomain; //+nosave

	/// Space that defines the motion parameters specified by the generators children.  This space
	/// is created on the fly in precompute.
	hkGeometry* m_animationSpace;   //+nosave
	hkArray< hkSimdReal > m_clipDurations; //+nosave

	friend class hkbParametricMotionGeneratorViewer;

protected:

	/// Transforms a set of parameters to a specific point in the corresponding animation space.
	void paramToAnimationSpace( hkVector4Parameter inMotionSpaceParam, hkVector4& outAnimSpacePt ) const;

	/// Clamps a desired animation space parameter value to a valid range
	void clampAnimationSpaceParam( hkVector4Parameter inAnimSpaceParam, hkVector4& outAnimSpaceParam ) const;

	/// Returns true if a vertex in the parameter space is valid, false otherwise
	bool isValid( int generatorIndex ) const;

	/// Sample the blend weights at the specified motions space position
	void sampleWeights( hkVector4Parameter inParam, hkLocalArray<int>& activeGeneratorIndices, hkVector4& activeGeneratorWeights ) const;


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
