/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Samples/Engine/BasicAnimation/BasicAnimationPCH.h>
#include <Vision/Samples/Engine/BasicAnimation/SimpleVertexAnimatedObject.h>


SimpleVertexAnimatedObject_cl::SimpleVertexAnimatedObject_cl(void)
{
}

void SimpleVertexAnimatedObject_cl::InitFunction()
{
  SetCastShadows(TRUE);
  SetSingleAnimationMode();
}



//
//                              *** MODE: START SINGLE ANIMATION ***
//

void SimpleVertexAnimatedObject_cl::SetSingleAnimationMode()
{
  // start a simple vertex animation
  m_spAnimControl = VisAnimConfig_cl::StartVertexAnimation(this, "Noname", VSKELANIMCTRL_DEFAULTS);
}


bool SimpleVertexAnimatedObject_cl::IsAnimationRunning() const
{
  return true;
}


V_IMPLEMENT_SERIAL( SimpleVertexAnimatedObject_cl, VisBaseEntity_cl, 0, Vision::GetEngineModule() );
START_VAR_TABLE(SimpleVertexAnimatedObject_cl, VisBaseEntity_cl, "SimpleVertexAnimatedObject_cl", 0, "Models\\ripple.model")
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
