/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Samples/Engine/Common/Entities/StaticCamera.hpp>


// ***********************************************************************************************
// StaticCamera_cl
// ***********************************************************************************************
// Static camera with a configurable look-at entity
// ***********************************************************************************************


StaticCamera_cl::StaticCamera_cl(void)
{
  m_pTarget = NULL;
}

StaticCamera_cl::~StaticCamera_cl(void)
{
}

void StaticCamera_cl::SetTarget( VisBaseEntity_cl* pTarget )
{
  m_pTarget = pTarget;
}

void StaticCamera_cl::SetTargetOffset( const hkvVec3& pos )
{
  m_pTargetPos = pos;
}


void StaticCamera_cl::ThinkFunction()
{
  // update the camera direction
  if ( m_pTarget != NULL )
  {
    hkvMat3 directionMatrix;
    hkvVec3 lookFrom = GetPosition();
    hkvVec3 lookAt = m_pTarget->GetPosition() + m_pTargetPos;
    directionMatrix.setLookInDirectionMatrix (lookAt - lookFrom);
    Vision::Camera.Set( directionMatrix, lookFrom );
  }
}


V_IMPLEMENT_SERIAL( StaticCamera_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(StaticCamera_cl, VisBaseEntity_cl, "StaticCamera_cl", 0, "")  
END_VAR_TABLE

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
