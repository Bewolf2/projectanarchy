/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef DEFINE_VISAPIPROFILING
#define DEFINE_VISAPIPROFILING

#include <string.h>
#include <stdlib.h>

#include <Vision/Runtime/Engine/Profiling/VisApiProfElements.hpp>


/// \brief
///   Flags for the VisProfiling_cl::SetDebugRenderFlags function
enum VDebugRenderFlags
{
  DEBUGRENDERFLAG_VISION_VERSION           = V_BIT(0),    ///< Displays the vision version.
  DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT     = V_BIT(1),    ///< Displays the object triangle count.
  DEBUGRENDERFLAG_OBJECT_VISBBOX           = V_BIT(2),    ///< Renders the current visibility bounding box for each entity.
  DEBUGRENDERFLAG_OBJECT_VISIBILITYZONES   = V_BIT(3),    ///< Displays a list of visibility zones each entity is assigned to.
  DEBUGRENDERFLAG_VISIBILITYOBJECTS        = V_BIT(4),    ///< Renders all visibility objects (bounding box). Same as settings VISTESTFLAGS_DEBUG_RENDER flag per visibility object, but easier to toggle.
  DEBUGRENDERFLAG_VISIBILITYZONES          = V_BIT(5),    ///< Renders the visibility zones.
  DEBUGRENDERFLAG_PORTALS                  = V_BIT(6),    ///< Renders the portals.
  DEBUGRENDERFLAG_LIGHT_INFLUENCEBOX       = V_BIT(7),    ///< Renders each light's box of influence.
  DEBUGRENDERFLAG_TRACELINES               = V_BIT(8),    ///< Renders all trace-lines as red lines (performed in that frame, thus also just visible one frame).
  DEBUGRENDERFLAG_POLYGONCOUNT             = V_BIT(9),    ///< Displays the overall polygon count.
  DEBUGRENDERFLAG_FRAMERATE                = V_BIT(10),   ///< Displays the current frame rate.
  DEBUGRENDERFLAG_PERFORMANCECOUNTERS      = V_BIT(11),   ///< Displays performance counters, e.g. number of draw calls etc.
  DEBUGRENDERFLAG_SCRIPTSTATISTICS         = V_BIT(12),   ///< Display script manager status. This is implementation specific.
  DEBUGRENDERFLAG_OBJECTRENDERORDER        = V_BIT(13),   ///< Display the index of objects inside the render queues. Useful to debug sorting criteria.
  DEBUGRENDERFLAG_THREADWORKLOAD           = V_BIT(14),   ///< Displays the workload of all CPU threads of the ThreadManager.
  DEBUGRENDERFLAG_ZONES                    = V_BIT(15),   ///< Displays streaming zone specific output.
  DEBUGRENDERFLAG_RESOURCE_STATISTICS      = V_BIT(16),   ///< Displays information about all active resources.
  DEBUGRENDERFLAG_MEMORY_STATISTICS        = V_BIT(17),   ///< Displays memory statistics (only available in debug).
  DEBUGRENDERFLAG_HW_COUNTERS              = V_BIT(19),   ///< Displays hardware performance counters. Not available on all hardware.

  DEBUGRENDERFLAG_ALL                      = V_BIT(20) - 1
};


#ifdef PROFILING
  #ifdef WIN32
    #define VISION_USE_GPA_ITT
  #endif

  #define VISION_INC_PERFCOUNT(x) \
    Vision::Profiling.PerformanceCounters.m_Counters.GetDataPtr()[x].m_iCount++;

  #define VISION_ADD_PERFCOUNT(x, y) \
    Vision::Profiling.PerformanceCounters.m_Counters.GetDataPtr()[x].m_iCount+=y;
#else
  #define VISION_INC_PERFCOUNT(x)
  #define VISION_ADD_PERFCOUNT(x, y)
#endif

class VisFont_cl;

/// \brief
///   Performance counter element. Currently intended for internal use only.
struct VisPerfCountElement_cl
{
  VisPerfCountElement_cl() 
  {
    m_iCount = 0; 
    m_szDesc = NULL; 
  }

  unsigned int m_iCount;
  char* m_szDesc;
};

/// \brief
///   Low-overhead functionality for counting performance-relevant events.
///   Currently intended for internal use only.
class VisPerformanceCounters_cl
{
public:
  VisPerformanceCounters_cl();
  ~VisPerformanceCounters_cl() {DeInit();}

  /// \brief Sets up all performance counters.
  void Init();

  /// \brief Clears all performance counters.
  void DeInit();

  /// \brief Resets the values of all performance counters to zero.
  void InitFrame();

  /// \brief Draws the values of all performance counters on screen.
  void Render();

  DynArray_cl<VisPerfCountElement_cl> m_Counters;

private:
  /// \brief Adds a new performance counter at \c iIndex with the help text \c pDesc.
  void AddCounter(unsigned int iIndex, const char* pDesc);

  unsigned int m_iNumElements;
};

#if defined(_VR_GX2)

  /// \brief Initializes the hardware performance metric collection. (Called internally)
  void InitPerfMetrics();

  /// \brief deinitializes the hardware performance metric collection. (Called internally)
  void DeInitPerfMetrics();

  /// \brief Starts the performance measurement. (Called internally)
  void StartPerfMeasurement();

  /// \brief Ends the performance measurement. (Called internally)
  void EndPerfMeasurement();

  /// \brief Renders the performance measurement. (Called internally)
  void RenderPerfMeasurements();

#endif


/// \brief
///   Profiling class which can be used to register profiling items and define profiling-related
///   settings.
/// 
/// The VisProfiling_cl instance provides access to the real-time profiling features of the Vision
/// Engine. This includes low-level, low-overhead profiling methods, text- and graph- based
/// visualization of profiling results, and various additional profiling-related settings and
/// helper functions.
/// 
/// Profiling elements (see below) are structured in groups.
/// 
/// In order to add custom profiling elements, simply use the
/// VISION_START_PROFILING/VISION_STOP_PROFILING macros around the code you want to measure and
/// register your element using the VisProfiling_cl::AddElement method. Vision has a total set of
/// 4096 profiling counters, of which counters 0 to 2047 are reserved for internal purposes. The
/// remaining counters (2048 to 4095) can be used in your own code.  Physics Modules may use the
/// profiling registers from 1900 to 2000.
/// 
/// In order to be able to use profiling in your own code, you have to add the preprocessor
/// definition "PROFILING" to your project, otherwise no profiling code will be generated. It is
/// recommended to switch off profiling for release builds using Vision::Profiling.SetMethod, as 
/// using profiling is typically associated with a 2-15% performance impact, depending on the
/// platform.
/// 
/// In addition to the text-based display of profiling results, profiling results can also be
/// visualized as charts. 
class VisProfiling_cl
{
public:

  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the profiling class, only used internally
  VisProfiling_cl();

  
  /// \brief
  ///   Destructor of the profiling class, only used internally
  ~VisProfiling_cl();

  ///
  /// @}
  ///

  ///
  /// @name Setup Functions
  /// @{
  ///

  /// \brief
  ///   Adds a new profiling group
  /// 
  /// All elements added subsequently will belong to this group.
  /// 
  /// \param groupName
  ///   name of the group
  VISION_APIFUNC VProfilingNode* AddGroup(const char *groupName);


  /// \brief
  ///   Adds a new profiling element
  /// 
  /// This method adds a new profiling element to the current group. A profiling element consists
  /// of a name and an element ID. The ID (basically an index into the profiling table) is also
  /// used as input to the VISION_START_PROFILING/VISION_STOP_PROFILING macros.
  /// 
  /// \param elementID
  ///   element ID between 2048 and 4095 (typically a symbolic constant)
  /// 
  /// \param elementName
  ///   custom element name
  /// 
  /// \param showChart
  ///   if TRUE this element will be added to the list of elements which are displayed. By default,
  ///   this value is set to TRUE.
  ///
  /// \param pParent
  ///   Can be used to specify an (optional) parent node for this node.
  /// 
  /// \return
  ///   int elementNumber:       internal index of the profiling element, can be used for the
  ///   GetElement* functions.
  /// 
  /// \note
  ///   In most cases you will need the internal profiling element number instead of the ID for the
  ///   other methods of the VisProfiling_cl class. You can use the method
  ///   VisProfiling_cl::GetElementNumberByID to translate IDs to internal profiling element
  ///   numbers.
  /// 
  /// \note
  ///   Physics Modules may use the profiling elements 1900-2000.
  VISION_APIFUNC VProfilingNode* AddElement(int elementID, const char *elementName, BOOL showChart = TRUE, VProfilingNode* pParent=NULL);


  /// \brief
  ///   Removes a profiling group
  /// 
  /// All the elements which belong to this group will be removed  as well.
  /// 
  /// \param pNode
  ///   Pointer to the profiling node to remove
  /// 
  /// \note
  ///   Behavior for text output: If the current group is removed while currently being displayed,
  ///   the previous group will be displayed instead (if available)
  VISION_APIFUNC void RemoveGroup(VProfilingNode* pNode);
  

  /// \brief
  ///   Removes a profiling element.
  /// 
  /// \param pNode
  ///   The profiling node to remove.
  VISION_APIFUNC void RemoveElement(VProfilingNode* pNode);

  /// \brief
  ///   Returns a free profiling element ID
  /// 
  /// for custom usage.
  /// 
  /// Thus the returned ID is between 2048 and 4095.
  /// 
  /// The function returns -1 in case there is no valid ID left.
  /// 
  /// \return
  ///   int elementID: Valid ID
  VISION_APIFUNC int GetFreeElementID();

  /// \brief
  ///   Returns the first profiling node that has the specified ID
  /// 
  /// This function recursively traverses the profiling graph starting with the root node
  /// (GetProfilingRootNode), so this function should not be called every frame.
  /// 
  /// This function finds engine profiling nodes as well as custom nodes that are attached to the
  /// graph.
  /// 
  /// \param iID
  ///   the integer ID to search for, e.g. VIS_PROFILE_GAMELOOP
  /// 
  /// \return
  ///   VProfilingNode* pNode: The first node with specified ID, or NULL
  VISION_APIFUNC VProfilingNode* GetProfilingNodeByID(int iID) const;

  /// \brief
  ///   Returns the first profiling node that has the specified name
  /// 
  /// This function recursively traverses the profiling graph starting with the root node
  /// (GetProfilingRootNode), so this function should not be called every frame.
  /// 
  /// This function finds engine profiling nodes as well as custom nodes that are attached to the
  /// graph.
  /// 
  /// \param szName
  ///   The name string to search for
  /// 
  /// \return
  ///   VProfilingNode* pNode: The first node with specified name, or NULL
  VISION_APIFUNC VProfilingNode* GetProfilingNodeByName(const char *szName) const;

  /// \brief
  ///   Returns the root element of the profiling graph
  /// 
  /// In pure release mode without profiling information, the return value is NULL.
  /// 
  /// The root element also represents the node that measures the duration of a full frame.
  /// 
  /// Groups can be added to the root node (See VProfilingNode::AddChild). The
  /// VisProfiling_cl::AddElement function for instance creates a new node and adds it to the root
  /// 
  /// \return
  ///   VProfilingNode* pNode: The root node, or NULL
  VISION_APIFUNC VProfilingNode* GetProfilingRootNode() const;

  ///
  /// @}
  ///

  ///
  /// @name Information Functions
  /// @{
  ///

  /// \brief
  ///   Gets the number of registered profiling groups.
  /// 
  /// You can add new profiling groups with the VisProfiling_cl::AddGroup function.
  /// 
  /// \return
  ///   unsigned int numOfGroups
  VISION_APIFUNC unsigned int GetNumOfGroups() const;


  /// \brief
  ///   Returns the number of total triangles drawn during the last frame
  /// 
  /// \return
  ///   int: Number of polygons
  VISION_APIFUNC int GetOverallPolygonCount () const;

  /// \brief
  ///   Returns the number of CPU cycles per millisecond
  /// 
  /// The return value depends on the CPU type and speed used. It is once determined at engine
  /// initialization time.
  /// 
  /// This value is the basis for any cycles to millisecond conversion in the profiling code.
  /// 
  /// \return
  ///   int iCycles : Number of CPU cycles
  VISION_APIFUNC int GetNumCyclesPerMS() const;

  /// \brief
  ///   Returns the number of system ticks that are processed by the system in a millisecond.
  /// 
  /// On Win32 and Xbox360, the returned value is determined with the QueryPerformanceFrequency
  /// function.
  /// 
  /// The return value depends on the machine used. It is once determined at engine initialization
  /// time.
  /// 
  /// This value is the basis for any ticks to millisecond conversion in the profiling code.
  /// 
  /// \return
  ///   int iCycles : Number of Ticks per millisecond
  VISION_APIFUNC int GetNumTicksPerMS() const;

  ///
  /// @}
  ///

  ///
  /// @name Profile Displays
  /// @{
  ///

  /// \brief
  ///   Shows the profiling information as text output
  /// 
  /// This function shows the profiling results on screen (text output) until the
  /// VisProfiling_cl::Hide function is called
  VISION_APIFUNC void Show();
  
  
  /// \brief
  ///   Hide the profiling information
  /// 
  /// If the profiling results are currently shown, this method hides them again.
  VISION_APIFUNC void Hide();
  
  /// \brief
  ///   Returns whether profiling information is currently shown (text or chart).
  /// 
  /// This function returns the status of the profiling information.
  /// 
  /// It returns FALSE if either text (VisProfiling_cl::Show) or chart data
  /// (VisProfiling_cl::ShowChart) is currently shown.
  /// 
  /// \return
  ///   BOOL: TRUE if neither the profiling pages nor a chart is shown. FALSE if any of them is
  ///   shown.
  /// 
  /// \sa Show
  /// \sa Hide
  /// \sa ShowChart
  /// \sa HideChart
  VISION_APIFUNC BOOL IsHidden() const;
  
  
  /// \brief
  ///   Displays the next profiling group (text output)
  /// 
  /// This function shows the next profiling group. If profiling output is currently not shown, it
  /// will automatically be enabled. As soon as the last group is reached, the profiling
  /// information will be hidden. An additional call to this function will show the first group
  /// again.
  VISION_APIFUNC void NextPage();

  /// \brief
  ///   Displays the previous profiling group (text output)
  /// 
  /// This function shows the previous profiling group. If profiling output is currently not shown,
  /// it will automatically be enabled. As soon as the first group is reached, the profiling
  /// information will be hidden. An additional call to this function will show the last group
  /// again.
  VISION_APIFUNC void PreviousPage();

  /// \brief
  ///   Displays the specified profiling group.
  /// 
  /// If profiling output is currently not shown, it will automatically be enabled.
  /// 
  /// \param pageNr
  ///   page to display
  VISION_APIFUNC void SetPage(unsigned int pageNr);

  /// \brief
  ///   Specifies whether the frame rate is to be displayed
  /// 
  /// \param status
  ///   TRUE to show the frame rate as text output, otherwise FALSE.
  VISION_APIFUNC void DisplayFrameRate(BOOL status);

  /// \brief
  ///   Starts profiling an element
  ///   Use the macro VISION_START_PROFILING instead.
  VISION_APIFUNC void StartElementProfiling(int elementID);


  /// \brief
  ///   Stops profiling an element
  ///   Use the macro VISION_STOP_PROFILING instead.
  VISION_APIFUNC void StopElementProfiling(int elementID);

  /// \brief
  ///   Resets the maximum values of all profiling elements to zero
  VISION_APIFUNC void ResetProfilingMaxValues();
  



  ///
  /// @}
  ///

  ///
  /// @name Profiling Charts
  /// @{
  ///


  /// \brief
  ///   Sets the text color for profiling output on screen. Note that iColor.a should be 255.
  VISION_APIFUNC void SetTextColor(VColorRef iColor=VColorRef(255,255,255,255));

 

  ///
  /// @}
  ///

  ///
  /// @name Showing Object Triangle Counts
  /// @{
  ///

  /// \brief
  ///   Deprecated. Use EnableDebugRenderFlags(DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ShowObjectTriangleCounts(BOOL status = TRUE);

  /// \brief
  ///   Deprecated. Use EnableDebugRenderFlags(DEBUGRENDERFLAG_PERFORMANCECOUNTERS) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ShowPerformanceCounters(BOOL status = TRUE);


  /// \brief
  ///   Deprecated. Use EnableDebugRenderFlags(DEBUGRENDERFLAG_HW_COUNTERS) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ShowHardwareCounters(bool status = true);


  /// \brief
  ///   Deprecated. Use ToggleDebugRenderFlags(DEBUGRENDERFLAG_OBJECT_TRIANGLECOUNT) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ToggleObjectTriangleCounts();

  /// \brief
  ///   Deprecated. Use ToggleDebugRenderFlags(DEBUGRENDERFLAG_POLYGONCOUNT) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ToggleOverallPolygons();

  /// \brief
  ///   Deprecated. Use ToggleDebugRenderFlags(DEBUGRENDERFLAG_FRAMERATE) instead.
  HKV_DEPRECATED_2013_2 VISION_APIFUNC void ToggleFrameRate();

  /// \brief
  ///   Sets flags for rendering debug- and profiling-related information.
  /// 
  /// These flags are for debugging purposes and should always be disabled in release builds.
  /// 
  /// For performance reasons, some of the features are only available in profiling builds of the
  /// Vision Engine.
  /// 
  /// \param iFlags
  ///   Flags specifying which debug information to render.
  ///   See VDebugRenderFlags for all available flags.
  /// 
  /// \sa VDebugRenderFlags
  /// \sa VisProfiling_cl::GetDebugRenderFlags
  /// \sa VisProfiling_cl::ToggleObjectTriangleCounts
  VISION_APIFUNC void SetDebugRenderFlags(unsigned int iFlags);

  /// \brief
  ///   A helper function to enable or disable the given debug render flags.
  ///
  /// \param iFlags
  ///   The flags to enable or disable. See VDebugRenderFlags for all available flags.
  ///
  /// \param bEnable
  ///   Whether to add or remove the given flags.
  ///
  /// \sa VDebugRenderFlags
  inline void EnableDebugRenderFlags(unsigned int iFlags, bool bEnable = true)
  { 
    if (bEnable)
      SetDebugRenderFlags (GetDebugRenderFlags() | iFlags);
    else
      SetDebugRenderFlags (GetDebugRenderFlags() & ~iFlags);
  }

  /// \brief
  ///   A helper function to toggle the given debug render flags.
  ///
  /// \param iFlags
  ///   The flags to enable or disable. See VDebugRenderFlags for all available flags.
  ///
  /// \sa VDebugRenderFlags
  inline void ToggleDebugRenderFlags(unsigned int iFlags)  { SetDebugRenderFlags (GetDebugRenderFlags() ^ iFlags);   }

  /// \brief
  ///   Returns the bitmask that has been set via VisProfiling_cl::SetDebugRenderFlags
  VISION_APIFUNC unsigned int GetDebugRenderFlags() const;

  /// \brief
  ///   Set the filter bitmask for rendering profiling information only in some render contexts
  /// 
  /// Profiling information will only be rendered in render contexts, if the result of the logical AND
  /// operation of this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) 
  /// is not zero.
  ///
  /// @param iMask
  ///   the visibility bitmask to set
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask = iMask;
  }

  /// \brief
  ///   Get the filter bitmask that has been set via SetVisibleBitmask
  inline unsigned int GetVisibleBitmask() const
  {
    return m_iVisibleBitmask;
  }

  /// \brief
  ///   This callback is triggered when the debug render flags changed. External code can listen to this to set up debug output
  VisCallback_cl OnDebugRenderFlagsChanged;

  ///
  /// @}
  ///

  ///
  /// @name Profiling Method
  /// @{
  ///

  /// \brief
  ///   Sets the profiling method to use
  ///
  /// VIS_PROFILINGMETHOD_DISABLED will disable all element profiling and eliminate the profiling overhead
  /// Set this just before shipping
  /// 
  /// On WIN32, VIS_PROFILINGMETHOD_RDTSC is the default profiling method. This will use the CPU's
  /// time stamp counter to perform profiling. This is typically the fastest and most accurate
  /// profiling method. Note, however, that it can produce inconsistent results in case of varying
  /// processor speeds (SpeedStep, Cool&Quiet) or on multi-CPU systems.
  /// 
  /// VIS_PROFILINGMETHOD_QPC is an alternative method that uses the QueryPerformanceCounter
  /// function. Note that using this function can on some systems significantly degrade
  /// performance due to the overhead incurred by the QueryPerformanceCounter method.
  ///
  /// \param eMethod
  ///   Profiling method to use (see description).
  /// 
  /// \sa VisProfiling_cl::GetMethod()
  VISION_APIFUNC void SetMethod(VisProfilingMethod_e eMethod);

  /// \brief
  ///   Returns the current profiling method. 
  /// 
  /// \return
  ///   VisProfilingMethod_e: Profiling method used
  /// 
  /// \sa VisProfiling_cl::SetMethod()
  inline VisProfilingMethod_e GetMethod() const 
  { 
    return m_eUseQPCProfiling;
  } 

  /// \brief
  ///   Updates the profiling information
  /// 
  /// It resets the internal profiling counters.
  /// 
  /// Called from VisionApp_cl::OnUpdateScene.
  VISION_APIFUNC void Update();

  /// \brief
  ///   Initializes the profiling functions
  VISION_APIFUNC void Init();

  /// \brief
  ///   Deinitializes the profiling functions
  VISION_APIFUNC void DeInit();

  VisPerformanceCounters_cl PerformanceCounters;

  /// \brief
  ///   Displays a small table of current thread workload percentage of all active Vision threads
  VISION_APIFUNC void DisplayThreadWorkload(int iScreenX, int iScreenY, VisFont_cl *pFont=NULL);

  ///
  /// @}
  ///

  ///
  /// @name Resource Preview
  /// @{
  ///

  /// \brief
  ///   Used by vForge to set the current instance of the resource preview object. Only supported on WIN32 platforms.
  VISION_APIFUNC void SetCurrentResourcePreview(IVResourcePreview *pNewRes);

  /// \brief
  ///   Return the current instance of the resource preview object. Only set inside vForge
  inline IVResourcePreview *GetCurrentResourcePreview() const 
  {
    return m_spCurrentResourcePreview;
  }



  ///
  /// @}
  ///

private:
  friend class VisRenderLoopHelper_cl;
  friend void Vision_GL_RenderWorld();

  
  /// \brief
  ///   Draws the text of the chart (e.g. the description)
  void DrawChartText();

  /// \brief
  ///   Draws a string at the given position.
  void DrawTextArg2D(IVRenderInterface& ri, int& x, int& y, const char* msg, ...);


  /// \brief
  ///   Draws the resource statistics chart
  void DrawResourceStatistics();

  VisProfilingMethod_e m_eUseQPCProfiling;

  int oldMaxMsg;                                ///< backup variable for display functions
  float oldMsgDuration;                         ///< backup variable for display functions
  VSmartPtr<IVResourcePreview> m_spCurrentResourcePreview;

  unsigned int m_iVisibleBitmask;
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
