/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef __VISION_SIMPLEVERTEXANIMATEDOBJECT_HPP
#define __VISION_SIMPLEVERTEXANIMATEDOBJECT_HPP

// ***********************************************************************************************
// Class: SimpleVertexAnimatedObject_cl
// Copyright (C) Havok.com Inc. All rights reserved.
// ***********************************************************************************************
// 
// The SimpleVertexAnimatedObject_cl class is about vertex animations and shows you how to start
// a vertex animation.
//
// ***********************************************************************************************
class SimpleVertexAnimatedObject_cl : public VisBaseEntity_cl
{
public:
  SimpleVertexAnimatedObject_cl(void);

  // VisBaseEntity_cl overrides
  VOVERRIDE void InitFunction();

  // toggle visibility
  void SetActivate(bool bStatus) 
  {
    SetVisibleBitmask(bStatus ? 0xffffffff:0);
    SetCastShadows(bStatus);
  }

  // sample mode functions
  void SetSingleAnimationMode();
  bool IsAnimationRunning() const;

  VisVertexAnimControl_cl *GetVertexAnimControl() const {return m_spAnimControl;}
public:
  V_DECLARE_SERIAL( SimpleVertexAnimatedObject_cl, )
  IMPLEMENT_OBJ_CLASS(SimpleVertexAnimatedObject_cl);
  VSmartPtr<VisVertexAnimControl_cl> m_spAnimControl;
};

#endif //__VISION_SAMPLEVERTEXANIMATEDOBJECT_HPP

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
