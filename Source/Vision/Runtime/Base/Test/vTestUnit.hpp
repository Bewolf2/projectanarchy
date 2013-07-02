/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vTestUnit.hpp

#ifndef H_TEST_AUTOMATION_CLASS
#define H_TEST_AUTOMATION_CLASS

#include <Vision/Runtime/Base/System/IO/Stream/VDiskFileStreamManager.hpp>

#include <Vision/Runtime/Base/Test/vTestClassImpl.hpp>

#define MAX_TESTS 256
#define MAX_CONFIGS 256


// possible styles for the Printf function
enum ePRINTFSTYLE
{
  PS_NORMAL = 0,
  PS_BAD,
  PS_GOOD,
  PS_TITLE,
  PS_MAINTITLE,
  PS_CLIENT
};


#define AOUT_NONE        0
#define AOUT_STDOUT     (1<<0)
#define AOUT_WINCONSOLE (1<<1)
#define AOUT_HTMLFILE   (1<<2)

#define AOUT_TIMER


// forward declarations
class VTestClass;
class VTypeManager;
class VStrList;

/// \brief 
///   interface that represents callbacks for important test events
///
struct IVTestEventHandler
{
  /// \brief destructor
  virtual ~IVTestEventHandler() {}

  /// \brief
  ///   gets called whenever a message needs to be logged
  ///
  /// \param message
  ///   the message to be logged
  virtual void Log(const char* message) = 0;

  /// \brief
  ///   gets called whenever a message needs to be logged to the xml file
  ///
  /// \param message
  ///   the message to be logged
  virtual void Xml(const char* message) = 0;

  /// \brief
  ///   gets called when the whole test is finished
  /// 
  /// \param iExitCode
  ///   the exit code with which the test exited
  virtual void Exit(int iExitCode) = 0;
};

/// \brief
///   This class wraps test automation completly
/// 
/// This is the test unit to add, configure and run your tests.
/// 
/// You need one instance in your test project to add your own VTestClass derives classes.
/// 
/// After the adding you can display a nice Windows UI to configure the tests. Afterwards you have
/// to update the object state and then you can run all the activated tests with  the subtests. 
/// 
/// \example
///   \code
///   VTestUnit *tests = new VTestUnit(); // create VTestUnit object
///   tests->SetOutput ( AOUT_WINCONSOLE ); // define where the log messages are displayed
///   tests->RegisterTest ( new VLightTest ); // add first test
///   tests->RegisterTest ( new LoadTextureTest ); // add second test
///   if(SetupDialog(tests)) // display the userinterface
///   {
///     tests->Printf(0, AIS_MAINTITLE, "Vision Engine Tests"); // Print out a heading into the log
///     tests->RunAll(); // run all tests
///     tests->OutputStatistics(); // print out some satistics and version nubmers
///   }
///   delete tests; // cleans up everything (ATTENTION: your project must use the same RTL as the VTestUnit module)
///   \endcode
class VTestUnit 
{
public:

  ///
  /// @name Consturctor/destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor, sets the default values for the test uint stettings
  VBASE_IMPEXP VTestUnit(const char* szTestDirectory_read, const char* szTestDirectory_write, int partIndex = 1, int partCount = 1, bool bSetAsActive = true);


  /// \brief
  ///   Destructor which deletes all registred VTestClasses.
  VBASE_IMPEXP virtual ~VTestUnit();



  ///
  /// @}
  ///

  ///
  /// @name Important Functions
  /// @{
  ///
  

  /// \brief
  ///   Adds a new VTestClass derived object to the test suite.
  /// 
  /// The objects is deleted in the desctructor of VTestUnit!
  /// 
  /// \param pTest
  ///   pointer to the new VTestClass object
  /// 
  /// \example
  ///   \code
  ///   tests->RegistersTest( new YourTest );
  ///   \endcode
  VBASE_IMPEXP void RegisterTest( VTestClass *pTest );

  
  /// \brief
  ///   Registers all test classes which are derived from a specified base class
  /// 
  /// \param typeManager
  ///   your typemanager
  /// 
  /// \param pBaseType
  ///   all tests derived from this class will be added
  /// 
  /// \return
  ///   int: number of tests registered
  /// 
  /// \example
  ///   \code
  ///   VTypeManager typeManager;
  ///   typeManager.RegisterModule(&g_baseTestModule);  
  ///   VVERIFY( testUnit.RegisterTestsFromModule( typeManager, V_RUNTIME_CLASS(VTestClass) ) );
  ///   \endcode
  VBASE_IMPEXP int RegisterTestsFromModule(VTypeManager &typeManager, VType *pBaseType);

  /// \brief
  ///   sets the test event handler
  ///
  /// \param pHandler
  ///   the new test event handler, if NULL the current handler is removed
  VBASE_IMPEXP void SetEventHandler(IVTestEventHandler *pHandler);


  /// \brief
  ///   Runs the tests and returns the number of failed tests
  VBASE_IMPEXP int RunAll();
  
  /// \brief
  ///    Advances the tests by one frame, returns false if finished
  VBASE_IMPEXP bool RunTestCallback();

  /// \brief
  ///   Sets the handling of exceptions and assertions
  /// 
  /// \param bBreakOnAssert
  ///   TRUE if assertions should be handled (breakpoint)
  /// 
  /// \param bHandleExceptions
  ///   TRUE if vTestUnit should catch exceptions
  VBASE_IMPEXP void SetDebugFlags( VBool bBreakOnAssert, VBool bHandleExceptions );


  /// \brief
  ///   If skip skipOnFrameFail is enabled, then the test units stops the subtest as soon as the
  ///   first error has occured. Usually the subtest is only stopped when RunSubTest returns FALSE.
  ///   By default the test unit initializes bSkipOnFrameFail with FALSE.
  VBASE_IMPEXP void SetSkipOnFrameFail( VBool bSkipOnFrameFail );
  


  ///
  /// @}
  ///

  ///
  /// @name Logging
  /// @{
  ///

  /// \brief
  ///   Returns the currently set output mode.
  /// 
  /// \return
  ///   The currently set output mode.
  VBASE_IMPEXP int GetOutputMode ();


  /// \brief
  ///   Configures where the log messages are written to.
  /// 
  /// You can specify were the log messages are printed out.
  /// 
  /// \param iMode
  ///   choose one the following:
  ///   \li AOUT_NONE: no log output
  ///   \li AOUT_STDOUT: log output to the standard output
  ///   \li AOUT_HTMLFILE: log output into a HTML file (you have to pass the filename)
  ///   \li AOUT_VISIONLOG: log output to the vision log window (only possible with the engine)
  ///
  /// \param szHTML
  ///   If HTML output is desired: file name of the HTML output file. The file is saved into the tests' working directory.
  /// 
  /// \example
  ///   \code
  ///   tests->SetOutput ( AOUT_WINCONSOLE );
  ///   \endcode
  VBASE_IMPEXP void SetOutput( int iMode, const char *szHTML=NULL );


  /// \brief
  ///   Enables/Disables the JUnit compatible xml logging
  /// 
  /// \param bEnable
  ///   if false, no xml file is written 
  ///
  /// \param szXMLFile
  ///   file name of xml output file. The file is saved into the TestDirectory.
  VBASE_IMPEXP void SetXMLOutput( VBool bEnable, const char* szXMLFile=NULL );

  /// \brief
  ///   Dumps the content of the file to the std out
  /// 
  /// i.e. since it is not possible on to read a file from the Wii
  /// we need to dump it to the standard out to get its content via
  /// the log mechanism.
  /// 
  /// \param szFileName
  ///   The filename of the file which should be dumped
  VBASE_IMPEXP void DumpFile( const char* szFileName );

  /// \brief
  ///   If the argument is TRUE the html file is displayed 
  inline void ShowHTMLFile(VBool bShow) { m_bShowHTML = bShow; }
  

  /// \brief
  ///   Starts the statistic output, calls OutputStatistics, closes the HTML and XML document and
  ///   opens the HTML document (if m_bShowHTML is set)
  VBASE_IMPEXP void FinishOutputDocuments();



  /// \brief
  ///   Resets the statistics data
  VBASE_IMPEXP void ResetStatistics();


  /// \brief
  ///   Adds a log messages and works nearly as printf, but you can also specify an indentation level and
  ///   a font style for HTML output.
  /// 
  /// \param iLevel
  ///   Indentation level, 0 equals no indentation.
  /// 
  /// \param eStyle
  ///   The text style, choose one of the following:
  ///   \li PS_NORMAL = normal text
  ///   \li PS_BAD = red text
  ///   \li PS_GOOD = green text
  ///   \li PS_TITLE = bigger text
  ///   \li PS_MAINTITLE = much bigger and bold text
  ///   \li PS_CLIENT = italic text
  ///
  /// \param s
  ///   The string to print.
  /// 
  /// \example
  ///   \code
  ///   tests->Printf (0, PS_MAINTITLE, "Vision Engine Tests");
  ///   \endcode
  VBASE_IMPEXP void Printf ( int iLevel, ePRINTFSTYLE eStyle, const char *s, ... );  


  /// \brief
  ///   A printf like function that outputs the string to the log
  VBASE_IMPEXP void Printf ( const char *s, ... );  

  /// \brief 
  ///  A printf like function that outputs the string to the xml file
  void XMLPrintf ( const char *s, ... );

  ///
  /// @}
  ///

  ///
  /// @name Enable/disable, Select
  /// @{
  ///
  
  

  // set all test state (and obtionaly the subtests states) as enabled
  VBASE_IMPEXP void EnableAll(VBool bSubtestsToo = FALSE);    
  // set all test state (and obtionaly the subtests states) as disabled
  VBASE_IMPEXP void DisableAll(VBool bSubtestsToo = FALSE);
  // toggle all test state (and obtionaly the subtest states)
  VBASE_IMPEXP void ReverseState(VBool bSubtestsToo = FALSE);
  // set randomly test state (and obtionaly the subtests states)
  VBASE_IMPEXP void RandomState(VBool bSubtestsToo = FALSE, int iPercent = 50);
  
  // fill the m_iTestOrder array with a growing index
  VBASE_IMPEXP void OrderReset();   
  // fill it randomly
  VBASE_IMPEXP void OrderRandom();  
  // reset it and sort the array using the test name
  VBASE_IMPEXP void OrderAlphabetical(VBool bAZ = TRUE); 
  // fill it with the 'negative' value (TestCount - current index - 1)
  VBASE_IMPEXP void OrderReverse(); 
  


  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///
  
  // Sets/Gets the base directroy for the tests. All files of the tests should uses this directory.
  // eg. the settings file, the html,xml log file are created in that directory!
  VBASE_IMPEXP void SetTestDirectory(bool readDir, const char* szAbsolutePath);
  VBASE_IMPEXP const char* GetTestDirectory(bool readDir) const;
  
  // helper which takes the passed filename string and returns the TestDirectory + \\ + filename
  VBASE_IMPEXP const char* BuildTestFilePath(bool useReadDir, const char* szFilename) const;
  
  //the framework can save and load the status of each test and subtest and the order of the tests
  //the file is saved into the TestDirectory!
  VBASE_IMPEXP VBool LoadTests(const char *szFileName, bool bUseReadDir=true);
  VBASE_IMPEXP VBool SaveTests(const char *szFileName) const;
  
  //the framework has also some settings like output format and destination, assertions, special engine settings,
  // these settings can be also serialized.
  //the file is saved into the TestDirectory!
  VBASE_IMPEXP VBool LoadSettings(const char *szFileName, bool bUseReadDir=true);
  VBASE_IMPEXP VBool SaveSettings(const char *szFileName) const;

  
  ///
  /// @}
  ///

  ///
  /// @name Get Functions
  /// @{
  ///
  
  // return test by index (the index is for the test-array) (m_pTests[iTestIndex])
  VBASE_IMPEXP VTestClass *GetTest(int iTestIndex) const;
  // return test count (number of elements in the test-array)
  inline int GetTestCount() const { return m_iTests; }

  // return the test index found at specified position in the order-array (m_iTestOrder[iPos])
  VBASE_IMPEXP int GetTestIndexAt(int iPos) const;
  // return the test class founded in the specified position in the order-array (m_pTests[m_iTestOrder[iPos]])
  VBASE_IMPEXP VTestClass* GetTestAt(int iPos) const;

  // get the position in the test-array of the testcall pointer 
  VBASE_IMPEXP int GetTestArrayIndex(VTestClass *pTestClass);
  // get the position in the order-array of the testcall pointer 
  VBASE_IMPEXP int GetOrderArrayIndex(VTestClass *pTestClass);
  
  // return -1 if test name not found, else an index into the test array
  VBASE_IMPEXP int FindTestByName(const char* szTestName);
   
  VBASE_IMPEXP VBool MoveTest(int iOldPos, int iNewPos); ///< Moves a test between two position in the order-array and update the other test positions

  // Returns the VDiskFileStreamManager that is globally available for all unit tests
  VBASE_IMPEXP IVFileStreamManager* GetDiskFileStreamManager() const;

  // Returns the file stream manager which is used by default for all file I/O
  VBASE_IMPEXP virtual IVFileStreamManager* GetDefaultFileStreamManager() const;

  // \brief
  //    - Returns true if fast mode is active:
  //      = Tests are executed faster.
  //      This reduces the frames drawn by unit tests that would normally render several hundreds of frames,
  //      which makes sense if the graphical output isn't important.
  inline bool IsFastModeEnabled() const
  { 
    return m_bFastMode != 0; 
  }

  inline void SetFastModeEnabled(bool bEnabled)
  {
    m_bFastMode = bEnabled ? TRUE : FALSE;
  }

  // \brief
  //    Returns true if headless mode is active.
  inline bool IsHeadlessModeEnabled() const
  {
    return m_bHeadlessMode != 0;
  }

  // \brief
  //    Enables or disables the headless mode.
  inline void SetHeadlessModeEnabled(bool bEnabled)
  {
    m_bHeadlessMode = bEnabled ? TRUE : FALSE;
  }

  // \brief
  //   
  enum FeatureLevel
  {
    FeatureLevel_Default,   // don't modify any feature level specifiers
    FeatureLevel_11_0,
    FeatureLevel_10_1,
    FeatureLevel_10_0,
    FeatureLevel_9_3,
    FeatureLevel_9_2,
    FeatureLevel_9_1,

    FeatureLevel_Count
  };

  // \brief
  //   Set the feature level to be used for DX11 tests - this is ignored for everything else.
  VBASE_IMPEXP void SetFeatureLevel( FeatureLevel featureLevel );

  // \brief
  //   Retrieve the feature level to be used for DX11 tests. 
  //   On all other builds/platforms this returns FeatureLevel_Default.
  VBASE_IMPEXP FeatureLevel GetFeatureLevel() const;

  // \brief
  //   Should the DX11 feature level selector be displayed for this test unit?
  VBASE_IMPEXP virtual VBool DisplayFeatureLevelSelector() const;


  // \brief
  //   Add a tag for controlling test execution.
  void AddTag( const char* szTag );

  // \brief
  //   Remove a tag for controlling test execution.
  void RemoveTag( const char* szTag );

  // \brief
  //   Check whether the given tag is set.
  VBool HasTag( const char* szTag ) const;

  VBool m_bBreakOnAssert;
  
  inline int GetFailedTestCount() const { return m_iFailed; }
  inline void IncreaseFailCount() { m_iFailed++; }

  static VTestUnit* m_pActiveTestUnit;

protected:
  // tests have many friends :D 
  friend class CDialogSettings; ///< is always there if you need him
  friend class VTestClass;      ///< not a friend but is our child

  /// \brief
  ///   Initializes the test
  /// 
  /// This function calls the VTestClass::Init function and checks for an exception (if exception
  /// handling is enabled)
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoTestInit(VTestClass *pTest);

  
  /// \brief
  ///   Runs the test (implementation calls RunSubTest)
  /// 
  /// This function calls the VTestUnit::RunSubTests function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// This function gets called within the VTestUnit::RunAll function.
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise the result of
  ///   VTestUnit::RunSubTests is returned
  virtual VBASE_IMPEXP VBool DoTestRun(VTestClass *pTest);


  /// \brief
  ///   Deinitializes the test
  /// 
  /// This function calls the VTestClass::DeInit function and checks for an exception (if exception
  /// handling is enabled)
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoTestDeInit(VTestClass *pTest);

  
  /// \brief
  ///   Initializes, runs and deinitializes the subtest (VTestUnit::DoSubTestInit,
  ///   VTestUnit::DoSubTestRun, VTestUnit::DoSubTestDeInit) It also checks whether subtest is
  ///   enabled. This function calls the VTestUnit::DoSubTestRun function until it returns FALSE.
  ///   This is e.g. used by the engine tests to run the same subtest for multiple frames.
  void RunSubTests(VTestClass *pTest, int iSubTest);


  /// \brief
  ///   Initializes the subtest
  /// 
  /// This function calls the VTestClass::InitSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoSubTestInit(VTestClass *pTest, int iSubTest);

  
  /// \brief
  ///   Runs the subtest
  /// 
  /// This function calls the VTestClass::RunSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// This function gets called within the VTestUnit::RunSubTest function until FALSE is returned.
  /// This is e.g. used by the engine tests to run the same subtest for multiple frames.
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise the result of
  ///   VTestClass::RunSubTest is returned
  virtual VBASE_IMPEXP VBool DoSubTestRun(VTestClass *pTest, int iSubTest);


  /// \brief
  ///   Deinitializes the subtest
  /// 
  /// This function calls the VTestClass::DeInitSubTest function and checks for an exception (if
  /// exception handling is enabled).
  /// 
  /// \param pTest
  ///   pointer to the test class
  /// 
  /// \param iSubTest
  ///   number of the subtest
  /// 
  /// \return
  ///   VBool: returns FALSE if an exception was catched. Otherwise TRUE is returned
  virtual VBASE_IMPEXP VBool DoSubTestDeInit(VTestClass *pTest, int iSubTest);


  /// \brief
  ///   Adds some statistics to the log window. You can override this function if you want to add
  ///   your own statistics, don't forget to call the base class then ! You also get an overview of
  ///   you used DLL version if you use the engine.
  virtual VBASE_IMPEXP void DoOutputStatistics();

  // private helper functions
  void SwapTestOrder(int iPos1, int iPos2); ///< Swaps to tests in the order-array
  
  // change all character that may not appear in a win filename into an ape
  void FixString ( char *pszString );

  // used internaly to set the tests position in the order-array
  VBool SetTestPos(int iTestArrayIndex, int iPositionInOrderArray); 

  // internal function used by the two Printf functions
  void PrintfInternal ( int iLevel, ePRINTFSTYLE eStyle, const char *s, va_list args );  
  
  // HTML log functions
  void HTMLStartDoc ();
  void HTMLEndDoc ();
  void HTMLShowDoc ();
  void HTMLPrintf ( const char *s, ... );

  // add automatically generated tags (for the desired feature level, for example)
  void GenerateTags();

  // Platform-independent time function.
  // Returns time in milliseconds.
  unsigned int GetTime() const;
  
  // lots of private states

  int              m_iOutputMode;             ///< define for output (e.g. AOUT_WINCONSOLE), each combination
  
  char             m_szHTMLFile[FS_MAX_PATH]; ///< html outputfilename
  IVFileOutStream* m_fHTMLFile;               ///< filehandle for the html file
  
  VBool            m_bShowHTML;               ///< if true, the system will open the HTML output file in a window (requires HTML output)

  char             m_szXMLFile[FS_MAX_PATH];  ///< xml outputfilename
  IVFileOutStream* m_fXMLFile;                ///< filehandle for the xml file
  
  VBool m_bSkipOnFrameFail;     ///< skip current subtest as soon as the first frame in the subtest fails
  VBool m_bSkipOnSubTestFail;   ///< skip all remaining \b subtests if one subtest failed
  VBool m_bSkipOnTestFail;      ///< skip all remainint \b tests if one subtest failed

  VBool m_bFastMode;            ///< faster execution of unit tests (if supported)
  VBool m_bHeadlessMode;        ///< execute the tests in headless mode

  VBool m_bHandleExceptions;

  int m_iTestOrderSeed;
  int m_iTestOrder[MAX_TESTS];
  int m_iTests;                 ///< number of registered tests
  VTestClass *m_pTests[MAX_TESTS]; ///< array of registered pointers to VTestClass objects
  
  int m_iCurrentTest;           ///< index of current executed test
  int m_iCurrentTestIter;       ///< iterator index
  VTestClass *m_pCurrentTest;   ///< pointer to the current test
  int m_iCurrentSubTestNr;      ///< index of the current subtest (within a test)
  int m_iCurrentSubTestFrameNr; ///< frame number within the current subtest

  int m_iFailed; ///< number of failed test
  
  unsigned int m_uiStartTime;

  char m_szTestDirectory_read [FS_MAX_PATH];
  char m_szTestDirectory_write[FS_MAX_PATH];

  // test part index/count (e.g., index=2, count=3 runs the second third of the tests)
  int m_iPartIndex, m_iPartCount;

  // downlevel support on DX11 - this is ignored on everything else
  FeatureLevel  m_featureLevel;

  IVTestEventHandler *m_pTestEventHandler;

  ///
  /// @}
  ///

private:
  VStrList *m_pTagList;
  IVFileStreamManagerPtr m_spDiskFileStreamManager;
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
