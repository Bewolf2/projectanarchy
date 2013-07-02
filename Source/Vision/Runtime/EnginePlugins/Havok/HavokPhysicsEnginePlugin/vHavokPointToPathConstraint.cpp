/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPathConstraint.hpp>

#include <Physics/Constraint/Data/PointToPath/hkpPointToPathConstraintData.h>
#include <Physics/Constraint/Data/PointToPath/hkpLinearParametricCurve.h>


// ----------------------------------------------------------------------------
// vHavokPointToPathConstraintDesc
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPointToPathConstraintDesc, vHavokConstraintDesc, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPointToPathConstraintDesc::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokPointToPathConstraintDesc::vHavokPointToPathConstraintDesc()
:vHavokConstraintDesc(false)
{
  Reset();
}

// ----------------------------------------------------------------------------
void vHavokPointToPathConstraintDesc::Reset()
{
  vHavokConstraintDesc::Reset();

  m_vPathPivot = hkvVec3(0.0f, 0.0f, 0.0f);
  m_vPathDirection = hkvVec3(1.0f, 0.0f, 0.0f); 
  m_fPathLength = 0.0f;
  m_fFriction = 0.0f;           
}

// ----------------------------------------------------------------------------
void vHavokPointToPathConstraintDesc::Serialize( VArchive &ar )
{
  vHavokConstraintDesc::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));

    m_vPathPivot.SerializeAsVisVector (ar);
    m_vPathDirection.SerializeAsVisVector (ar);
    ar >> m_fFriction;
    ar >> m_fPathLength;
  }
  else
  {
    ar << s_iSerialVersion;
    m_vPathPivot.SerializeAsVisVector (ar);
    m_vPathDirection.SerializeAsVisVector (ar);
    ar << m_fFriction;
    ar << m_fPathLength;
  }
}


// ----------------------------------------------------------------------------
// vHavokPointToPathConstraint
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL( vHavokPointToPathConstraint, vHavokConstraint, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokPointToPathConstraint::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
hkpConstraintData* vHavokPointToPathConstraint::CreateConstraintData()
{
  return new hkpPointToPathConstraintData();
}

// ----------------------------------------------------------------------------
vHavokConstraintDesc *vHavokPointToPathConstraint::CreateConstraintDesc()
{
  return new vHavokPointToPathConstraintDesc();
}

// ----------------------------------------------------------------------------
void vHavokPointToPathConstraint::InitConstraintDataFromDesc(hkpConstraintData& data, vHavokConstraintDesc const& desc)
{
  vHavokConstraint::InitConstraintDataFromDesc(data, desc);
  hkpPointToPathConstraintData& lhData =
    static_cast<hkpPointToPathConstraintData&>(data);
  const vHavokPointToPathConstraintDesc* pPointToPathDesc = static_cast<const vHavokPointToPathConstraintDesc*>(&desc);

  hkVector4 pathPivot;
  hkvVec3d worldPivot; 

  if(!GetZonedPivot( pPointToPathDesc->m_vPathPivot, worldPivot ))
  {  
	  vHavokConversionUtils::VisVecToPhysVecLocal(pPointToPathDesc->m_vPathPivot, pathPivot);
  }
  else
  {
	  vHavokConversionUtils::VisVecToPhysVecLocal(worldPivot, pathPivot);
  }

  hkVector4 vLocalPivots[2];
  hkTransform mWorldToBody[2];

  for (int i = 0; i < 2; ++i)
  {
    VisBaseEntity_cl *pOwnerEntity = (pPointToPathDesc->m_spBodies[i])
      ? pPointToPathDesc->m_spBodies[i]->GetOwnerEntity()
      : NULL;

    if (pOwnerEntity)
    {
			vHavokConversionUtils::VisMatVecToPhysTransformWorld( pOwnerEntity->GetRotationMatrix(), pOwnerEntity->GetPosition(), mWorldToBody[i] );
			mWorldToBody[i].setInverse(mWorldToBody[i]);
		}
		else
		{
			mWorldToBody[i].setIdentity();
		}

		vLocalPivots[i].setTransformedPos( mWorldToBody[i], pathPivot );
  }

  hkpLinearParametricCurve *pLinearPath = new hkpLinearParametricCurve();
  hkVector4 vStart = pathPivot;
	hkVector4 vDirection; vHavokConversionUtils::VisVecToPhysVecLocal( pPointToPathDesc->m_vPathDirection*pPointToPathDesc->m_fPathLength, vDirection );
	hkVector4 vEnd; vEnd.setAdd(vStart, vDirection);

	vStart.setTransformedPos( mWorldToBody[1], pathPivot );
	vEnd.setTransformedPos( mWorldToBody[1], vEnd );

	{
	  pLinearPath->addPoint(vStart);
	  pLinearPath->addPoint(vEnd);
  }

  {
	  lhData.setInBodySpace(vLocalPivots[0],vLocalPivots[1],pLinearPath); 
  }

  // do not restrict orientation of the attached body
  lhData.setOrientationType(hkpPointToPathConstraintData::CONSTRAIN_ORIENTATION_NONE);

  lhData.setMaxFrictionForce(pPointToPathDesc->m_fFriction);

  pLinearPath->removeReference();

  m_savedPointToPathDesc = *pPointToPathDesc;
}

// ----------------------------------------------------------------------------
void vHavokPointToPathConstraint::SaveToDesc(vHavokConstraintDesc& desc)
{
  vHavokConstraint::SaveToDesc(desc);

  vHavokPointToPathConstraintDesc &pointToPathDesc = static_cast<vHavokPointToPathConstraintDesc &>(desc);

  pointToPathDesc.m_vPathPivot = m_savedPointToPathDesc.m_vPathPivot;
  pointToPathDesc.m_vPathDirection = m_savedPointToPathDesc.m_vPathDirection; 
  pointToPathDesc.m_fPathLength = m_savedPointToPathDesc.m_fPathLength;
  pointToPathDesc.m_fFriction = m_savedPointToPathDesc.m_fFriction;
}

// ----------------------------------------------------------------------------
void vHavokPointToPathConstraint::Serialize( VArchive &ar )
{
  vHavokConstraint::Serialize(ar);
  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT((iVersion > 0) && (iVersion <= s_iSerialVersion));
  }
  else
  {
    ar << s_iSerialVersion;
  }
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
