/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file progresswrapper.hpp

#pragma once

#using <mscorlib.dll>
#include <vcclr.h>

///\brief
/// Helper class that implements the VProgressStatus interface and forwards messages to the C# ProgressStatus object
class ProgressWrapper : public VProgressStatus
{
public:
  ///\brief
  /// Constructor that takes the  C# progress status object
  ProgressWrapper(ProgressStatus ^pProgress)
  {
    m_pProgress = pProgress;
  }

  virtual void OnProgressChanged() HKV_OVERRIDE
  {
    if (m_pProgress->IsAbortable)
      Application::DoEvents(); // update UI / Abort button click event
    m_pProgress->Percentage = this->GetCurrentProgress();
  }

  virtual bool WantsAbort() const HKV_OVERRIDE
  {
    if (VProgressStatus::WantsAbort())
      return true;

    return m_pProgress->WantsAbort;
  }

  gcroot<ProgressStatus ^> m_pProgress;

};

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
