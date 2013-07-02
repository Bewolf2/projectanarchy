/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBaseEntity.inl

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>


inline int VisBaseEntity_cl::GetStatusFlags() const
{
  return(statusFlags);
}


inline int VisBaseEntity_cl::GetCustomFlags() const
{
  return(customFlags);
}


inline void VisBaseEntity_cl::SetCustomFlags(int flags)
{
  customFlags = flags;
}



inline void VisBaseEntity_cl::ResetStatusFlags()
{  
  // Delete the status flags which are updated each frame
  statusFlags &= ~VIS_STATUS_SKELETALANIMATION_CHANGED;

  // remove only the O3D "change" flags
  ClearO3DChangeFlags();
}


inline void VisBaseEntity_cl::Handle()
{
  // send once-per-frame parent change notification if necessary
  int flags = 0;

  if ( GetO3DFlags() & VIS_OBJECT3D_POSCHANGED )
    flags |= VIS_MODSYSFLAG_FRAMEPOSITION;

  if ( GetO3DFlags() & VIS_OBJECT3D_ORICHANGED )
    flags |= VIS_MODSYSFLAG_FRAMEORIENTATION;
  
  if (flags)
    VisModuleSystemParent_cl::ModSysNotifyFunctionParentAltered(flags);

  // call handle function on all children in the module system
  VisModuleSystemParent_cl::ModSysNotifyFunctionHandle();
}



inline VisTraceAccuracy_e VisBaseEntity_cl::GetTraceAccuracy() const
{
  return traceAccuracy;
}


inline BOOL VisBaseEntity_cl::IsCoreUpdateRequired()
{
  BOOL bResult = ((GetO3DFlags()&VIS_OBJECT3D_ALLCHANGED)>0) || ((statusFlags & VIS_STATUS_ANIMATION_CHANGED)>0);
  return bResult;
}


inline void VisBaseEntity_cl::SetPhysicsObject(IVisPhysicsObject_cl *pPhysObj)
{
  if (pPhysObj != NULL)
    m_spPhysicsObject = pPhysObj;
  else
    DeletePhysicsObject();
}

inline IVisPhysicsObject_cl *VisBaseEntity_cl::GetPhysicsObject() const
{
  return m_spPhysicsObject;
}

inline BOOL VisBaseEntity_cl::IsRemoved() const
{
  return deadNowFlag;
}


inline void VisBaseEntity_cl::SetTracelineBitmask(unsigned int iBitMask)
{
  m_iTracelineBitMask = iBitMask;
}

inline unsigned int VisBaseEntity_cl::GetTracelineBitmask() const
{
  return m_iTracelineBitMask;
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
