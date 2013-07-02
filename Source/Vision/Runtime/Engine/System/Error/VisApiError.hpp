/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiError.hpp

#ifndef _VISIONAPIERROR_
#define _VISIONAPIERROR_


/// \brief
///   Error types (passed to the Vision::Error.FatalError function)
enum VisErrorType_e
{
  VIS_ERROR_UNKNOWN,
  VIS_ERROR_GENERALFAILURE,
  VIS_ERROR_FILENOTFOUND,
  VIS_ERROR_ARRAYOVERFLOW,
  VIS_ERROR_MALLOCFAILED
};


/// \brief
///   Message types
enum VisApiMessageType_e
{
  VIS_MESSAGETYPE_SYSTEMMESSAGE = 0,
  VIS_MESSAGETYPE_WARNING = 1,
  VIS_MESSAGETYPE_FATALERROR = 2
};



/// \brief
///   Function type of a callback for custom message handling
/// 
/// Custom message handling functions can be globally added via Vision::Error.AddCallback
/// 
/// \param szMessage
///   The message text
/// 
/// \param messageType
///   Type of the message (information, warning or error)
/// 
/// \param pUserData
///   User data pointer passed to AddCallback
/// 
/// \return
///   \c TRUE if the message should be further processed by the
///   default handler (e.g. log output or fatal error dialog) If more than one callback is
///   installed, the default handler will be processed if at least one callback returns TRUE.
typedef BOOL (*VISMESSAGE_CALLBACK)( const char* szMessage, VisApiMessageType_e messageType, void* pUserData );



class VisReportCollector_cl;

/// \brief
///   Class for error messages, warnings and system messages. Can be accessed via Vision::Error
/// 
/// Custom error messages can be triggered via this class.
/// 
/// Furthermore, internal error messages sent by the engine can be redirected via callbacks.
class VisError_cl
{
public:

  /// \brief
  ///   Helper class to temporarily change the setting whether errors should be
  ///   displayed in the Vision error dialog.
  class ScopedReportStatus
  {
  public:
    /// \brief
    ///   Constructor; saves the current status of the setting and applies the specified
    ///   new setting.
    /// \param err
    ///   the error handler on which this helper should operate. Typically Vision::Error.
    /// \param newStatus
    ///   the new setting for VisError_cl::SetReportStatus()
    ScopedReportStatus(VisError_cl &err, bool newStatus) 
    : m_err(err)
    {
      m_savedStatus = (m_err.m_bReportStatus != FALSE);
      m_err.SetReportStatus(newStatus);
    }

    /// \brief
    ///   Destructor; restores the previously saved setting for VisError_cl::SetReportStatus().
    ~ScopedReportStatus()
    {
      m_err.SetReportStatus(m_savedStatus);
    }

  private:
    VisError_cl& m_err;
    bool m_savedStatus;
  };


public:
  VisError_cl();
  ~VisError_cl();
  void Init();
  void DeInit();

  ///
  /// @name Error Reporting
  /// @{
  ///

  /// \brief
  ///   Sets the global output targets for fatal errors, warnings and system messages
  /// 
  /// The parameters influence the behavior of the default handler. It does however not affect
  /// custom handlers set via AddCallback.
  /// 
  /// \param logSysMsgs
  ///   if TRUE system messages will additionally be stored in a log file
  /// 
  /// \param windowSysMsgs
  ///   if TRUE system messages will additionally be displayed in the log window (resource viewer
  ///   tool)
  /// 
  /// \param logWarnings
  ///   if TRUE warnings will additionally be stored in a log file
  /// 
  /// \param windowWarnings
  ///   if TRUE warnings will additionally be displayed in the log window (resource viewer tool)
  /// 
  /// \param logErrors
  ///   if TRUE fatal errors will additionally be stored in a log file
  /// 
  /// \param windowErrors
  ///   if TRUE fatal errors will additionally be displayed in the log window (resource viewer
  ///   tool)
  /// 
  /// \param szLogFileName
  ///   name of the log file, default name is "vision.log"
  VISION_APIFUNC  void SetOutput(BOOL logSysMsgs, BOOL windowSysMsgs, BOOL logWarnings = FALSE, BOOL windowWarnings = FALSE, 
                                 BOOL logErrors = FALSE, BOOL windowErrors = FALSE, const char *szLogFileName = NULL);


  /// \brief
  ///   Returns the global output targets for fatal errors, warnings and system messages
  /// 
  /// These parameters only influence the behavior of the default handler, but not that of any 
  /// custom handlers set via AddCallback.
  /// 
  /// \param logSysMsgs
  ///   if TRUE system messages will additionally be stored in a log file
  /// 
  /// \param windowSysMsgs
  ///   if TRUE system messages will additionally be displayed in the log window (resource viewer
  ///   tool)
  /// 
  /// \param logWarnings
  ///   if TRUE warnings will additionally be stored in a log file
  /// 
  /// \param windowWarnings
  ///   if TRUE warnings will additionally be displayed in the log window (resource viewer tool)
  /// 
  /// \param logErrors
  ///   if TRUE fatal errors will additionally be stored in a log file
  /// 
  /// \param windowErrors
  ///   if TRUE fatal errors will additionally be displayed in the log window (resource viewer
  ///   tool)
  VISION_APIFUNC void GetOutput(bool& logSysMsgs, bool& windowSysMsgs, bool& logWarnings, bool& windowWarnings,
    bool& logErrors, bool& windowErrors);


  /// \brief
  ///   Triggers a fatal error.
  /// 
  /// It also calls the CustomHandlerFatalError function. If the
  /// custom handler returns TRUE the default handler will be called and a message box will be
  /// displayed.
  /// 
  /// By default the engine also shuts down whenever a fatal error occurs.
  /// 
  /// The different types of errors need different additional parameters:
  /// - \c VIS_ERROR_UNKNOWN
  ///   - No additional parameters
  /// - \c VIS_ERROR_GENERALFAILURE
  ///   - \c char \c *message: description of the general failure
  /// - \c VIS_ERROR_FILENOTFOUND
  ///   - \c char \c *fileType: type of file, e.g. "world".
  ///   - \c char \c *fileName: name of the file, e.g. "dungeon1"
  ///   - The resulting message will be fileType + " file not found:" + fileName
  /// - \c VIS_ERROR_ARRAYOVERFLOW
  ///   - \c char \c *description: description of the overflow, e.g. "too many lightsources generated"
  ///   - \c int \c currentSize  : current size of the array
  ///   - \c int \c neededSize   : needed size of the array
  /// - \c VIS_ERROR_MALLOCFAILED
  ///   - \c char \c *dataDescription: description of the data you tried to allocate, e..g "temporary buffer"
  ///   - \c int \c size             : size of the memory you tried to allocate
  ///
  ///  \param errorType
  ///    Defines the type of the error.
  ///
  /// \note
  ///   The additional parameters are not optional, they have to be passed for the different types
  ///   of errors as mentioned above.
  /// 
  /// \example
  ///   \code
  ///   if (filehandle==NULL) Vision::Error.FatalError(VIS_ERROR_FILENOTFOUND, "ImportantData", filename);
  ///   \endcode
  ANALYSIS_NO_RETURN VISION_APIFUNC void FatalError(VisErrorType_e errorType, ...);
  

  /// \brief
  ///   Triggers a fatal error.
  /// 
  /// It also calls the CustomHandlerFatalError function. If the
  /// custom handler returns TRUE the default handler will be called and a message box will be
  /// displayed.
  /// 
  /// By default the engine also shuts down whenever a fatal error occurs. Additional outputs of
  /// the default handler can be set with the SetOutput function.
  /// 
  /// Fatal errors should be used in case the application cannot recover from an error. E.g. a file
  /// is missing that is required to initialize the application. 
  /// 
  /// Fatal errors in the engine are used when memory allocation operations fail or if a map file
  /// cannot be loaded.
  /// 
  /// \param s
  ///   a printf-like format string
  /// 
  /// \note
  ///   You can use the SetOutput function to define whether the fatal errors shall be stored in a
  ///   log file and/or in a log window
  VISION_APIFUNC void FatalError(const char *s, ...);

  /// \brief
  ///   Outputs a warning.
  /// 
  /// It also calls the CustomHandlerWarning function. If the custom
  /// handler returns TRUE the default handler will be called. Additional outputs of the default
  /// handler can be set with the SetOutput function.
  /// 
  /// Warnings can be used for non-critical errors. The log window of the resource viewer
  /// highlights warnings in red color.
  /// 
  /// \param s
  ///   a printf-like format string
  /// 
  /// \note
  ///   You can use the SetOutput function to define whether the warning should be stored in a log
  ///   file and/or in a log window
  VISION_APIFUNC void Warning(const char *s, ...);


  /// \brief
  ///   Outputs a system message.
  /// 
  /// It also calls the CustomHandlerSystemMessage. If the
  /// custom handler returns TRUE the default handler will be called. Additional outputs of the
  /// default handler can be set with the SetOutput function.
  /// 
  /// System Messages are writing user information into the log file.
  /// 
  /// \param s
  ///   a printf-like format string
  /// 
  /// \note
  ///   You can use the SetOutput function to define whether the system message shall be stored in
  ///   a log file and/or in a log window
  /// 
  /// \example
  ///   \code
  ///   Vision::Error.SystemMessage("The current x position of the entity is: %f", entity->GetPosition().x);
  ///   \endcode
  VISION_APIFUNC void SystemMessage(const char *s, ...);  
  


  ///
  /// @}
  ///

  ///
  /// @name Custom Handler
  /// @{
  ///

  /// \brief
  ///   Internally called by the engine
  VISION_APIFUNC BOOL CustomHandlerFatalError(char const *message, VisErrorType_e ErrorType);

  /// \brief
  ///   Internally called by the engine
  VISION_APIFUNC BOOL CustomHandlerWarning(char const *message);

  /// \brief
  ///   Internally called by the engine
  VISION_APIFUNC BOOL CustomHandlerSystemMessage(char const *message);


  /// \brief
  ///   Copies the current log buffer
  /// 
  /// GetLogBuffer copies the current log buffer into the passed buffer. The passed buffer has to
  /// have a size of 65536, otherwise the function will fail. If the szStoreBuffer parameter is set
  /// to NULL, then the size of the needed buffer will be returned (currently always 65536)
  /// 
  /// \param szStoreBuffer
  ///   buffer of the size 65536
  /// 
  /// \param iSize
  ///   size of the buffer (currently always has to be 65536)
  /// 
  /// \return
  ///   the length of the log string
  VISION_APIFUNC int GetLogBuffer(char *szStoreBuffer = NULL, int iSize = 0);


  /// \brief
  ///   Adds a message callback function for custom message processing to the list of
  ///   installed message callback functions.
  /// 
  /// Each callback will be called for all messages sent.
  /// 
  /// \param pCallbackFunc
  ///   pointer to the callback function
  /// 
  /// \param pUserData
  ///   pointer to any user defined data. This could for instance be used as a pointer to an
  ///   instance of a class.
  /// 
  /// \return
  ///   \c TRUE if the callback has been added to the list
  VISION_APIFUNC BOOL AddCallback( VISMESSAGE_CALLBACK pCallbackFunc, void* pUserData = NULL );

  /// \brief
  ///   Removes a registered message callback function
  /// 
  /// from the list of intalled message callbacks.
  /// 
  /// \param pCallbackFunc
  ///   pointer to the callback function
  /// 
  /// \param pUserData
  ///   pointer to any user defined data. This could for instance be used as a pointer to an
  ///   instance of a class.
  /// 
  /// \return
  ///   \c TRUE if the callback was removed, i.e. was installed using AddCallback
  VISION_APIFUNC BOOL RemoveCallback( VISMESSAGE_CALLBACK pCallbackFunc, void* pUserData = NULL );



  ///
  /// @}
  ///

  ///
  /// @name Debug Reporting System
  /// @{
  ///


  /// \brief
  ///   Sets the status for the error/warning report dialog
  /// 
  /// The bug/warning report dialog appears after Vision::InitWorld() and after Vision::DeInitWorld()  (If
  /// any warning or error occurred)
  /// 
  /// The errors are grouped by error type such as missing textures, missing entity classes etc.
  /// 
  /// \param bStatus
  ///   The new status: If set to FALSE, the dialog will never be shown
  /// 
  /// \note
  ///   The default status is TRUE
  /// 
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC void SetReportStatus(BOOL bStatus);

  /// \brief
  ///   Deletes all report entries that accumulated since the last time the report dialog has been
  ///   displayed.
  /// 
  /// \note
  ///   This function will be called each time after the dialog has been shown.
  /// 
  /// \note
  ///   However, this function should not be called by the user.
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC void ResetReportEntries();

  /// \brief
  ///   Adds a single, un-grouped entry to the report dialog
  /// 
  /// This entry will be listed in the report dialog the next time it is shown.
  /// 
  /// \param reportType
  ///   Type of the entry; can be VIS_REPORTENTRY_INFO, VIS_REPORTENTRY_WARNING or
  ///   VIS_REPORTENTRY_ERROR
  /// 
  /// \param szInfo
  ///   Info string that appears in the list
  /// 
  /// \param szDescr
  ///   Info string that appears in the description box when the error is selected in the dialog
  /// 
  /// \param szDetailed
  ///   Info string that appears in the detailed box when the error is selected
  /// 
  /// \note
  ///   This Report will not be added to an error/warning group, but appear as a single list
  ///   element
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  /// 
  /// \example
  ///   \code
  ///   char detailed[256];
  ///   sprintf(detailed,"The following file is missing : '%s'", szFilename);
  ///   Vision::Error.AddReportEntry(VIS_REPORTENTRY_ERROR,  "File(s) not found", "At least one file could not be opened or does not exist!", detailed);
  ///   \endcode
  VISION_APIFUNC void AddReportEntry(VisReportEntryType_e reportType, const char *szInfo, const char *szDescr, const char *szDetailed);

  /// \brief
  ///   Adds a report entry to a predefined group (missing texture,...)
  /// 
  /// In the report list, each group only appears once and lists all members added in the detailed
  /// box.
  /// 
  /// \param type
  ///   The error/warning group
  /// 
  /// \param szSetName
  ///   The member string (e.g. single filename) that should be added to the group list
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  /// 
  /// \example
  ///   \code
  ///   Vision::Error.AddReportGroupEntry( VIS_REPORTGROUPTYPE_FILE_NOT_FOUND, "myFile.dat" );
  ///   \endcode
  VISION_APIFUNC void AddReportGroupEntry(VisReportGroupType_e type, const char *szSetName);

  /// \brief
  ///   Shows the error/warning report dialog
  /// 
  /// (only if it has any entries).
  /// 
  /// This dialog will automatically be shown after Vision::InitWorld() and after Vision::DeInitWorld() (if
  /// enabled by SetReportStatus, which is default)
  /// 
  /// \param bCheckStatus
  ///   If TRUE, the dialog will only be opened if it is activated by the SetReportStatus flag. If
  ///   FALSE, the dialog will be shown regardless of the status flag.
  /// 
  /// \return
  ///   \c TRUE, if the dialog has been shown, FALSE if the report status flag is disabled or
  ///   the dialog is empty 
  /// 
  /// \note
  ///   If the dialog is empty, it will not be shown and the function returns FALSE
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ReportHasEntries
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC BOOL ShowReportDialog(BOOL bCheckStatus = FALSE);

  /// \brief
  ///   Returns TRUE if there are any report entries since the last reset
  /// 
  /// This state is independent of the SetReportStatus state, so this function may also  return
  /// TRUE although the report dialog is disabled.
  /// 
  /// \return
  ///   \c TRUE, if the dialog collected any entries
  /// 
  /// \sa VisError_cl::SetReportStatus
  /// \sa VisError_cl::ResetReportEntries
  /// \sa VisError_cl::AddReportEntry
  /// \sa VisError_cl::AddReportGroupEntry
  /// \sa VisError_cl::ShowReportDialog
  /// \sa VisReportEntryType_e
  /// \sa VisReportGroupType_e
  VISION_APIFUNC BOOL ReportHasEntries();

  /// \brief
  ///   Gets the number of currently collected report entries since the last reset.
  /// 
  /// \return
  ///   int iEntryCount: number of report entries
  VISION_APIFUNC int GetNumReportEntries();

  ///
  /// @}
  ///

private:
  /// \brief
  ///   Default handler for fatal errors which by default opens a message box and shuts down the
  ///   engine
  void DefaultHandlerFatalError(char const *message, VisErrorType_e errorType);

  /// \brief
  ///   Default handler for warnings, doesn't do anything if SetOutput function has not been called
  void DefaultHandlerWarning(char const *message);

  /// \brief
  ///   Default handler for system messages, doesn't do anything if SetOutput function has not been
  ///   called
  void DefaultHandlerSystemMessage(char const *message);

  /// \brief
  ///   Add the specified message to the log buffer
  void AddMessageToLogBuffer(const char *message);

  /// \brief
  ///   Writes the passed message to the debug output (e.g. OutputDebugString on Win32)
  void OutputMessageToDebugLog(const char *message);

  /// \brief
  ///   Send messages to installed message callbacks
  /// 
  /// Calls each registered callback using the parameters
  /// 
  /// \return
  ///   \c TRUE if no callbacks were registered or at least one of them
  ///   returned TRUE after the call.
  BOOL SendMessageToCallbacks( const char* szMessage, VisApiMessageType_e messageType );


  BOOL fatalError_writeToLog;           ///< if TRUE fatal errors will be written to the log file
  BOOL fatalError_writeToWindow;        ///< if TRUE fatal errors will be displayed in the log window
  BOOL warning_writeToLog;              ///< if TRUE warnings will be written to the log file
  BOOL warning_writeToWindow;           ///< if TRUE warnings will be displayed in the log window
  BOOL sysMsg_writeToLog;               ///< if TRUE system messages will be written to the log file
  BOOL sysMsg_writeToWindow;            ///< if TRUE system messages will be displayed in the log window
  
  
  BOOL logCreated;                      ///< log file already exists
  BOOL windowCreated;                   ///< log window already created
  BOOL errorProcessed;                  ///< is an error already being processed!?
  VString m_sLogFile;
  char m_bReportHasEntries;

  char *m_szLogBuffer;                    ///< pointer to the beginning of the log buffer
  int m_iLogBufferPos;                     ///< current position in the log ring buffer
  BOOL m_bLogBufferWrapAround;             ///< if TRUE then at least one wrap around in the log ring buffer happend
 
  BOOL m_bReportStatus;
  BOOL m_bInitialized;
 
  DynArray_cl<VISMESSAGE_CALLBACK> m_callbackList; ///< list of added message callbacks
  DynArray_cl<void*> m_callbackUserData;

  VReport m_Report;
  class VisReportCollector_cl *m_pReportCollector;
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
