/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file LogWrapper.hpp

#pragma once

#using <mscorlib.dll>
#include <vcclr.h>

///\brief
/// Helper class that implements the IVLog interface and forwards messages to the C# IEditorConsole object
class LogWrapper : public IVLog
{
public:
  ///\brief
  /// Constructor that takes a pointer to the C# log instance
  LogWrapper(IEditorConsole ^pLog)
  {
    m_pLog = pLog;
  }

  virtual void Error(const char *szMessage) HKV_OVERRIDE
  {
    if (m_pLog) 
      m_pLog->PrintError(gcnew System::String(szMessage, 0, (int)strlen(szMessage), System::Text::Encoding::UTF8));
  }

  virtual void Warning(const char *szMessage) HKV_OVERRIDE
  {
    if (m_pLog) 
      m_pLog->PrintWarning(gcnew System::String(szMessage, 0, (int)strlen(szMessage), System::Text::Encoding::UTF8));
  }

  virtual void Info(const char *szMessage) HKV_OVERRIDE
  {
    if (m_pLog) 
      m_pLog->Print(gcnew System::String(szMessage, 0, (int)strlen(szMessage), System::Text::Encoding::UTF8));
  }

  virtual void ParseMessage(const char *szMessage) HKV_OVERRIDE
  {
    if (m_pLog) 
      m_pLog->ParseMessage(gcnew System::String(szMessage, 0, (int)strlen(szMessage), System::Text::Encoding::UTF8));
  }

  gcroot<IEditorConsole ^> m_pLog;
  
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
