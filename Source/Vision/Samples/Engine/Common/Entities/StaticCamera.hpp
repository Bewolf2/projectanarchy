/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file StaticCamera.hpp

#ifndef STATIC_CAMERA_HPP_INCLUDED
#define STATIC_CAMERA_HPP_INCLUDED

#ifndef _VISION_DOC

// ***********************************************************************************************
// StaticCamera_cl
// ***********************************************************************************************
// Static camera with a configurable look-at entity
// ***********************************************************************************************
class StaticCamera_cl : public VisBaseEntity_cl
{
public:
  StaticCamera_cl();
  ~StaticCamera_cl();

  virtual void ThinkFunction();

  // camera setup
  void SetTarget( VisBaseEntity_cl* pTarget );

  void SetTargetOffset( const hkvVec3& pos );

private:
  hkvVec3 m_pTargetPos;
  VisBaseEntity_cl *m_pTarget;

public:
  V_DECLARE_SERIAL( StaticCamera_cl, )
  IMPLEMENT_OBJ_CLASS(StaticCamera_cl);
};

#endif // _VISION_DOC

#endif //STATIC_CAMERA_HPP_INCLUDED

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
