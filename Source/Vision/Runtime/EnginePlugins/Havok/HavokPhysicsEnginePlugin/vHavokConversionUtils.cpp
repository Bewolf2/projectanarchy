/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>


// --------------------------------------------------------------------------
// Static Members
// --------------------------------------------------------------------------

hkReal vHavokConversionUtils::m_cachedVision2HavokScale = hkReal(0.01f);
hkReal vHavokConversionUtils::m_cachedHavok2VisionScale = hkReal(100);
hkVector4* vHavokConversionUtils::m_cachedWorldPivot = 0;

hkSimdReal vHavokConversionUtils::m_cachedVis2PhysScale = hkSimdReal::fromFloat(hkReal(0.01f));
hkSimdReal vHavokConversionUtils::m_cachedPhys2VisScale = hkSimdReal::fromFloat(hkReal(100));


// --------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------

void vHavokConversionUtils::InitializeVectorCache() 
{ 
	if( !m_cachedWorldPivot ) 
	{ 
		m_cachedWorldPivot = new hkVector4; 
		*m_cachedWorldPivot = hkVector4::getConstant<HK_QUADREAL_0>(); 
	} 
}

void vHavokConversionUtils::DeinitializeVectorCache() 
{ 
	if( m_cachedWorldPivot ) 
		delete m_cachedWorldPivot;  
	m_cachedWorldPivot = 0; 
}


// --------------------------------------------------------------------------
// Static Conversion Methods
// --------------------------------------------------------------------------

void vHavokConversionUtils::PhysTransformToVisMatVec(const hkTransform &hkTf, hkvMat3 &visRotMatrixOut, hkvVec3& visPositionOut)
{
	// Convert rotation matrix
	const hkRotation& hkRot = hkTf.getRotation();
	HkRotationToVisMatrix(hkRot, visRotMatrixOut);

	// Convert position
	const hkVector4& hkPos = hkTf.getTranslation();
	PhysVecToVisVecLocal(hkPos, visPositionOut);
}

void vHavokConversionUtils::PhysTransformToVisMatVecWorld(const hkTransform &hkTf, hkvMat3 &visRotMatrixOut, hkvVec3& visPositionOut)
{
	// Convert rotation matrix
	const hkRotation& hkRot = hkTf.getRotation();
	HkRotationToVisMatrix(hkRot, visRotMatrixOut);

	// Convert position
	const hkVector4& hkPos = hkTf.getTranslation();
	PhysVecToVisVecWorld(hkPos, visPositionOut);
}

void vHavokConversionUtils::VisMatVecToPhysTransform(const hkvMat3 &visRotMatrix, const hkvVec3& visPosition, hkTransform &hkTfOut)
{
	// Convert the rotation
	hkRotation& hkRot = hkTfOut.getRotation();
	VisMatrixToHkRotation(visRotMatrix, hkRot);

	// Convert the position
	hkVector4& hkPos = hkTfOut.getTranslation();
	VisVecToPhysVecLocal(visPosition, hkPos);

	HK_ASSERT(0xdee889, hkTfOut.isOk());
}

void vHavokConversionUtils::VisMatVecToPhysTransformWorld(const hkvMat3 &visRotMatrix, const hkvVec3& visPosition, hkTransform &hkTfOut)
{
	// Convert the rotation
	hkRotation& hkRot = hkTfOut.getRotation();
	VisMatrixToHkRotation(visRotMatrix, hkRot);

	// Convert the position
	hkVector4& hkPos = hkTfOut.getTranslation();
	VisVecToPhysVecWorld(visPosition, hkPos);

	HK_ASSERT(0xdee889, hkTfOut.isOk());
}


void vHavokConversionUtils::SetHavok2VisionScale(hkReal fHavok2VisionScale)
{
  m_cachedHavok2VisionScale = fHavok2VisionScale;
  m_cachedVision2HavokScale = hkReal(1) / fHavok2VisionScale;
  m_cachedPhys2VisScale.setFromFloat(m_cachedHavok2VisionScale);
  m_cachedVis2PhysScale.setFromFloat(m_cachedVision2HavokScale);
}

VHAVOK_IMPEXP  void vHavokConversionUtils::HkMatrixToVisMatrix( const hkMatrix4 &hkMatrix, hkvMat4 &visMatrixOut, bool bScalePosition, bool bScaleRotation, bool bIsWorldTransform )
{
	hkMatrix4 hkMat = hkMatrix;
	if (bScaleRotation) 
	{
		hkMat.getColumn(0).mul(m_cachedPhys2VisScale);
		hkMat.getColumn(1).mul(m_cachedPhys2VisScale);
		hkMat.getColumn(2).mul(m_cachedPhys2VisScale);
	}

	if( bIsWorldTransform )
	{
		hkMat.getColumn(3).sub(*m_cachedWorldPivot);
	}

	if (bScalePosition) 
	{
		hkMat.getColumn(3).mul(m_cachedPhys2VisScale);
	}

	// hkvMat4 is also column-major in memory
	float* visMat = (float*)visMatrixOut.getPointer();
	hkMat.getColumn<0>().store<4,HK_IO_NATIVE_ALIGNED>(visMat  );
	hkMat.getColumn<1>().store<4,HK_IO_NATIVE_ALIGNED>(visMat+4);
	hkMat.getColumn<2>().store<4,HK_IO_NATIVE_ALIGNED>(visMat+8);
	hkMat.getColumn<3>().store<4,HK_IO_NATIVE_ALIGNED>(visMat+12);
}

VHAVOK_IMPEXP  void vHavokConversionUtils::VisMatrixToHkMatrix( const hkvMat4 &visMatrix, hkMatrix4& hkMatOut, bool bScalePosition, bool bScaleRotation, bool bIsWorldTransform )
{
	// hkvMat4 is column-major in memory
	const float* visMat = (const float*)visMatrix.getPointer();
	hkMatOut.getColumn(0).load<4,HK_IO_NATIVE_ALIGNED>(visMat  );
	hkMatOut.getColumn(1).load<4,HK_IO_NATIVE_ALIGNED>(visMat+4);
	hkMatOut.getColumn(2).load<4,HK_IO_NATIVE_ALIGNED>(visMat+8);
	hkMatOut.getColumn(3).load<4,HK_IO_NATIVE_ALIGNED>(visMat+12);

	if (bScaleRotation) 
	{
		hkMatOut.getColumn(0).mul(m_cachedVis2PhysScale);
		hkMatOut.getColumn(1).mul(m_cachedVis2PhysScale);
		hkMatOut.getColumn(2).mul(m_cachedVis2PhysScale);
	}

	if (bScalePosition) 
	{
		hkMatOut.getColumn(3).mul(m_cachedVis2PhysScale);
	}

	if( bIsWorldTransform )
	{
			if (bScalePosition)
			{
				//world pivot is already in scaled position
				hkMatOut.getColumn(3).add(*m_cachedWorldPivot);
			}
			else 
			{
				//scale world pivot to vision scale
				hkMatOut.getColumn(3).addMul(*m_cachedWorldPivot, m_cachedPhys2VisScale);
			}
	}

	HK_ASSERT(0xdee889, hkMatOut.isOk());
}

void vHavokConversionUtils::SetVisionWorldPivot(const hkvVec3d& worldPivot )
{

  AlignedArray array(worldPivot);

  m_cachedWorldPivot->load<3,HK_IO_NATIVE_ALIGNED>(array.m_array);
  m_cachedWorldPivot->zeroComponent<3>();
  m_cachedWorldPivot->mul(m_cachedVis2PhysScale);
  HK_ASSERT(0xdee883, m_cachedWorldPivot->isOk<4>());
}

hkVector4Parameter vHavokConversionUtils::GetVisionWorldPivot() 
{ 
	return *m_cachedWorldPivot; 
}

hkpWeldingUtility::WeldingType vHavokConversionUtils::VisToHkWeldingType(VisWeldingType_e eWeldingType)
{
  switch(eWeldingType)
  {
  case VIS_WELDING_TYPE_NONE:
    return hkpWeldingUtility::WELDING_TYPE_NONE;

  case VIS_WELDING_TYPE_ANTICLOCKWISE:
    return hkpWeldingUtility::WELDING_TYPE_ANTICLOCKWISE;

  case VIS_WELDING_TYPE_CLOCKWISE:
    return hkpWeldingUtility::WELDING_TYPE_CLOCKWISE;

  case VIS_WELDING_TYPE_TWO_SIDED:
    return hkpWeldingUtility::WELDING_TYPE_TWO_SIDED;

  default:
    VASSERT(!"Unknown welding type");
  }

  return hkpWeldingUtility::WELDING_TYPE_NONE;
}

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
