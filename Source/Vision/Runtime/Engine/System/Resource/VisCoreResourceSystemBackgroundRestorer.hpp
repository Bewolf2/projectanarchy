/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisCoreResourceSystemBackgroundRestorer.hpp

#ifndef VISCORERESOURCEBACKGROUNDRESTORER_HPP_INCLUDED
#define VISCORERESOURCEBACKGROUNDRESTORER_HPP_INCLUDED

//-----------------------------------------------------------------------------------

#include <Vision/Runtime/Engine/System/Resource/IVisApiBackgroundResourceRestorer.hpp>

//-----------------------------------------------------------------------------------

/// \brief
///   Handles restoring resources when the application was in the background state.
class VisResourceSystemBackgroundRestorer_cl : public IVisBackgroundResourceRestorer_cl
{
public:
  // Constructor
  VisResourceSystemBackgroundRestorer_cl(const VisResourceConfig_t &config,
    int iTotalResourceSize, int iOldParticleRingBufferSize);

  // Interface
  void BeginRestore(DynArray_cl<VResourceManager*> resourceManagers,
    int iNumResourceManagers);

  virtual bool IsFinished() const HKV_OVERRIDE;

  virtual void Tick() HKV_OVERRIDE;

private:
  // Member variables
  int m_iResMgrIndex, m_iResIndex;
  int m_iTotalResourceSize, m_iRestoredSize;
  int m_iOldParticleRingBufferSize;
  VisResourceConfig_t m_config;

  // use pointer to not to try to unload dependent resources if they
  // were already unloaded by a parent resource (static mesh -> buffer)
  DynArray_cl<VResourceManager*> m_resourceManagers;
  int m_iNumResourceManagers;

  VisBackgroundRestoreObject_cl m_callbackData;
  bool m_bFinished;
  float m_fProgressUpdateGranularity, m_fLastProgress;
};

//-----------------------------------------------------------------------------------

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
