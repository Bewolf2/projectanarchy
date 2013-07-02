/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VISMOBILEEXITDIALOG_HPP_INCLUDED
#define VISMOBILEEXITDIALOG_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>

/// \brief
///   This class provides a simple exit dialog functionality.
class VisMobileExitDialog : public VDialog
{
public:
  VisMobileExitDialog() : VDialog(), m_bExitTriggered(false) {}

  virtual void OnInitDialog() HKV_OVERRIDE;
  virtual void OnItemClicked(VMenuEventDataObject* pEvent) HKV_OVERRIDE;

  inline bool IsExitTriggered() const
  {
    return m_bExitTriggered;
  }

  virtual void OnSetFocus(bool bStatus) HKV_OVERRIDE;

protected:
  V_DECLARE_SERIAL_DLLEXP(VisMobileExitDialog, DECLSPEC_DLLEXPORT)
  VOVERRIDE void Serialize(VArchive &ar) {}

private:
  void CloseDialog();

  int m_iExitId;
  int m_iCancelId;

  bool m_bExitTriggered;
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
