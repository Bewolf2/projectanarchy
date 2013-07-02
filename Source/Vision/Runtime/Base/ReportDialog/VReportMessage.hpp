/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VReportMessage.hpp

#ifndef VISION_VREPORTMESSAGE_HPP
#define VISION_VREPORTMESSAGE_HPP

#ifndef _VISION_DOC

#include <Vision/Runtime/Base/ReportDialog/VReport.hpp>

class IVFileStreamManager;

/// \brief
///   Collecting the report messages
/// 
/// This class collects all errors and warnings of similar types and collates the data for a
/// compact presentation.
/// 
/// \note
///   To use this function, derive a class from VReportMessage and define the function
///   FillInReportMessage, which is virtual in VReportMessage.
/// 
/// \note
///   Define your own report type IDs (error codes). You pass these error codes to the AddEntry
///   function. The same codes are used by your own FillInReportMessages, where you can easily
///   distinguish them with a switch statement. In the switch statement, fill in the string data
///   with the message that belongs to the error code.
/// 
/// \example
///   \code
///   VReportMessage report;
///   report.AddEntry(0, "First entry of type 0");
///   report.AddEntry(1, "First entry of type 1");
///   report.AddEntry(0, "Second entry of type 0");
///   report.AddEntry(0, "Third  entry of type 0");
///   report.AddEntry(1, "Second entry of type 1");
///   report.AddEntriesToVReport();
///   // Check that the five report messages have been collated to two report entries.
///   VTEST(report.m_reportMessageList.GetLength() == 5);
///   VTEST(report.m_reportList.GetLength() == 2);
///   report.ShowReport("Error messages displayed");
///   report.ResetAll();
///   \endcode
class VReportMessage : public VReport
{
public:

  VBASE_IMPEXP virtual ~VReportMessage();

  /// \brief
  ///   Adds an entry with a specific error code and data to a report.
  /// 
  /// \param type
  ///   Error code. Defined by user.
  /// 
  /// \param szInfo
  ///   Additional error data
  /// 
  /// \note
  ///   To collect all these error codes, you have to call VReportMessage::AddEntriesToVReport.
  VBASE_IMPEXP void AddEntry(int type, const char *szInfo = NULL);

  

  /// \brief
  ///   Collates all entries to VReport
  /// 
  /// Collates all entries with the same error code to one VReport message with the data of all the
  /// messages merged.
  /// 
  /// \param eMinReportLevel
  ///   Minimum level of a report to be included. Can have these values:
  ///   \li V_REPORT_INFORMATION: Include infos, warnings and errors
  ///   \li V_REPORT_WARNING:     Include warnings and errors only
  ///   \li V_REPORT_ERROR:       Include errors only
  /// 
  /// \note
  ///   Call this function just before calling VReportMessage::ShowReport.
  /// 
  /// \note
  ///   Don't call this function twice without calling ResetVReportMessage or ResetAll inbetween
  VBASE_IMPEXP void AddEntriesToVReport(VReportType eMinReportLevel = V_REPORT_INFORMATION);



  /// \brief
  ///   Shows all report in an error report box.
  /// 
  /// \param szTitle
  ///   Title of error report window
  /// 
  /// \note
  ///   Call VReportMessage::AddEntriesToVReport before calling this function
  VBASE_IMPEXP void ShowReport(const char *szTitle);



  /// \brief
  ///   Resets all uncollated messages
  /// 
  /// \note
  ///   To reset everything, just call VReportMessage::ResetAll
  VBASE_IMPEXP void ResetVReportMessage();



  /// \brief
  ///   Resets all collated messages
  /// 
  /// \note
  ///   To reset everything, just call VReportMessage::ResetAll
  VBASE_IMPEXP void ResetVReport();


  /// \brief
  ///   Resets all messages
  VBASE_IMPEXP void ResetAll();



  /// \brief
  ///   Fills in report messages
  /// 
  /// This function takes an error code and replaces it with the appropriate information about the
  /// error or warning.
  /// 
  /// \param idCode
  ///   (IN) Error code
  /// 
  /// \param vsInfo
  ///   (IN) Data collected from the entries (added with AddEntry) of this error type
  /// 
  /// \param vsMainInfo
  ///   Short description
  /// 
  /// \param vsDetails
  ///   Displayable data, collected about this error message
  /// 
  /// \param vsDescription
  ///   Verbose description of error or warning
  /// 
  /// \param eReportType
  ///   Type of message
  /// 
  /// \note
  ///   This function should be implemented by the user.
  /// 
  /// \example
  ///   \code
  ///   See the test for this class
  ///   \endcode
  VBASE_IMPEXP virtual void FillInReportMessage(int idCode, VString &vsInfo, VString &vsMainInfo, VString &vsDetails, VString &vsDescription, VReportType &eReportType);

  /// \brief
  ///   Saves all the report entries to a file.
  /// 
  /// \param szFileName
  ///   File where the error log should be saved
  /// 
  /// \param szInitString
  ///   String containing message to write before error log
  /// 
  /// \param append
  ///   If TRUE, append to existing file. If FALSE, overwrite existing file
  /// 
  /// \param pMan
  ///   Custom file stream manager, or NULL
  /// 
  /// \note
  ///   You have to call FillInReportMessage before calling this function.
  /// 
  /// \example
  ///   \code
  ///   See the test for this class
  ///   \endcode
  VBASE_IMPEXP VBool SaveReport(const char *szFileName, const char *szInitString = NULL, VBool append = FALSE, IVFileStreamManager* pMan = NULL);

protected:
  VPList m_reportMessageList;

  static int VISION_CDECL CompareEntries(const void* entr1, const void *entr2);
};

#endif // _VISION_DOC

#endif // VISION_VREPORTMESSAGE_HPP

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
