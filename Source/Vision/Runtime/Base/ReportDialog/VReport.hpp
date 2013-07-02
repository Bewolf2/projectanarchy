/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VReport.hpp

#ifndef VISION_VREPORT_HPP
#define VISION_VREPORT_HPP

// Should not appear in doc
#ifndef _VISION_DOC

/// \brief
///   Report type enum
enum VReportType
{
  V_REPORT_INFORMATION = 0,
  V_REPORT_WARNING,
  V_REPORT_ERROR
};



/// \brief
///   Class to hold a report entry
class VReportEntry
{
public:
  inline VReportEntry(VReportType eRepType) { m_eReportType = eRepType; }

  // create function
  VBASE_IMPEXP static VReportEntry *CreateNewEntry(VReportType eRepType);

  // get functions
  inline const char *GetInfoStr() const { return m_vsInformation.AsChar(); }
  inline const char *GetDecrStr() const { return m_vsDescription.AsChar(); }
  inline const char *GetDetailStr() const { return m_vsDetails.AsChar(); }
  inline VReportType GetReportType() const { return m_eReportType; }

  // set functions
  inline void SetInfoStr(const char *szInfo) { m_vsInformation = szInfo; }
  inline void SetDescrStr(const char *szDescr) { m_vsDescription = szDescr; }
  inline void SetDetailStr(const char *szDetail) { m_vsDetails = szDetail; }
  inline void SetReportType(VReportType eNewType) { m_eReportType = eNewType; }
    
private:
  VString m_vsInformation;
  VString m_vsDescription;
  VString m_vsDetails;
  VReportType m_eReportType;
};


/// \brief
///   Class to hold and display reports.
class VReport
{
public:
  VBASE_IMPEXP VReport();
  VBASE_IMPEXP virtual ~VReport();
  
  /// \brief
  ///   Adds a report entry.
  inline void AddEntry(VReportEntry *pEntry) { m_reportList.Append(pEntry); }

#if defined(WIN32) && !defined(_VISION_WINRT)
  /// \brief
  ///   Shows a modal dialog for this report.
  /// 
  /// \param pTitle
  ///   title of the dialog
  /// 
  /// \param hParent
  ///   parent window handle. If specified, the report dialog is modal.
  /// 
  /// \return
  ///   VBool: TRUE if succesfully.
  VBASE_IMPEXP VBool ShowDialog(const char *pTitle, VWND hParent = NULL );
#endif
  
  /// \brief
  ///   Deletes all the entries on the list and resets it.
  VBASE_IMPEXP void Reset();
  

  /// \brief
  ///   Returns the entry count in the report list.
  inline int GetReportCount() { return m_reportList.GetLength(); }
  

  /// \brief
  ///   Returns a report entry.
  inline VReportEntry *GetReport(int idx) { return (VReportEntry *)m_reportList[idx]; }
  

protected:
  VPList m_reportList;
};

#endif //_VISION_DOC

#endif //MU_VULPNE_VREPORT_HPP

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
