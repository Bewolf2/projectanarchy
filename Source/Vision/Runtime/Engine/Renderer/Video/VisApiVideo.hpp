/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVideo.hpp

/// \file VisApiVideo.hpp

#ifndef DEFINE_VISAPIVIDEO
#define DEFINE_VISAPIVIDEO

class VWindowConfig;

/// \brief
///   Class for initializing and handling the video mode.
/// 
/// There is only one instance of this class, which is part of the static main class Vision.
class VisVideo_cl : public VVideo
{
public:

  ///
  /// @name Constructors / Destructors
  /// @{
  ///

  /// \brief
  ///   Constructor of the video class.
  VisVideo_cl();


  /// \brief
  ///   Destructor of the video class
  virtual ~VisVideo_cl();

  
  ///
  /// @}
  ///

  ///
  /// @name Initialization / Deinitialization
  /// @{
  ///

  /// \brief
  ///   Initializes the video subsystem of the Vision Engine. Call this method before engine initialization.
  ///
  /// \param pWindowConfig
  ///   On Windows, this parameter describes the window configuration. On other platforms, this parameter can be NULL.
  ///   See the documentation for VWindowConfig for more information.
  ///
  /// \return
  ///   BOOL: TRUE if the initialization was successful; otherwise FALSE. Use
  ///   VisVideo_cl::GetLastError to obtain more information.
  VISION_APIFUNC BOOL Init(const VWindowConfig* pWindowConfig = NULL);


  /// \brief
  ///   Deinitializes the video subsystem of the Vision engine and restores the screen to its
  ///   default mode.
  /// 
  /// DeInit will restore the screen mode to the mode that was active when Vision::Init was called.
  /// 
  /// This function should be called before exiting your application.
  /// 
  /// Note that Vision::DeInit() must to be called before Vision::Video.DeInit(), not vice versa.
  VISION_APIFUNC void DeInit();
  
  
  /// \brief
  ///   Presents the back buffer contents to the front buffer.
  /// 
  /// Similar to the DirectX method Present. The method used for presenting the back buffer
  /// contents are transparent to the user (e.g. flipping, copying).
  VISION_APIFUNC void UpdateScreen();

  
  /// \brief
  ///   Sets a specified screen mode.
  /// 
  /// Initializes a screen mode specified through the passed VVideoConfig structure.
  /// 
  /// This method tries to find a match of the configuration in the list of supported graphics mode
  /// of the installed graphics adapters and initializes this mode.
  /// 
  /// \param vcfg
  ///   Reference to the VVideoConfig object to be used for initializing the screen. Note that the
  ///   passed video VVideoConfig object may be altered by this method. Specifically, information
  ///   about e.g. the window created for windowed mode may be added to the structure.
  /// 
  /// \return
  ///   BOOL: TRUE if the operation succeeded, otherwise false. Use the VVideo::GetLastError method
  ///   to obtain the error string.
  /// 
  /// \note
  ///   This method should only be used for video mode initialization, not for changing the screen
  ///   resolution.
  /// 
  /// \note
  ///   In order to change the screen resolution, use the ChangeScreenResolution method.
  VISION_APIFUNC BOOL SetMode(VVideoConfig &vcfg);


  /// \brief
  ///   Changes the screen resolution at runtime
  /// 
  /// \param newSizeX
  ///   new screen resolution in x-direction. 
  /// 
  /// \param newSizeY
  ///   new screen resolution in y-direction. 
  /// 
  /// \param refreshRate
  ///   new screen refresh rate; 0 for default refresh; for windowed mode default refresh rate is enforced
  /// 
  /// \return
  ///   BOOL: TRUE if the operation succeeded, otherwise false. Use the VVideo::GetLastError method
  ///   to obtain the error string.
  VISION_APIFUNC BOOL ChangeScreenResolution(int newSizeX, int newSizeY, SLONG refreshRate);


  /// \brief
  ///   Changes the screen resolution and position at runtime
  /// 
  /// \param newPosX
  ///   new screen position in x-direction.
  ///
  /// \param newPosY
  ///   new screen position in y-direction.
  ///
  /// \param newSizeX
  ///   new screen resolution in x-direction. 
  /// 
  /// \param newSizeY
  ///   new screen resolution in y-direction. 
  /// 
  /// \param bFullscreen
  ///   TRUE for full screen, FALSE for windowed mode.
  ///
  /// \param refreshRate
  ///   new screen refresh rate; 0 for default refresh; for windowed mode default refresh rate is enforced
  /// 
  /// \return
  ///   BOOL: TRUE if the operation succeeded, otherwise false. Use the VVideo::GetLastError method
  ///   to obtain the error string.
  VISION_APIFUNC BOOL ChangeScreenResolution(int newPosX, int newPosY, int newSizeX, int newSizeY, BOOL bFullscreen, SLONG refreshRate);


  /// \brief
  ///   Changes the resolution at runtime. Can also be used to toggle between windowed and
  ///   full screen mode.
  /// 
  /// \param newSizeX
  ///   new screen resolution in x-direction. 
  /// 
  /// \param newSizeY
  ///   new screen resolution in y-direction. 
  /// 
  /// \param bFullscreen
  ///   TRUE for fullscreen, FALSE for windowed mode.
  /// 
  /// \param refreshRate
  ///   new screen refresh rate; 0 for default refresh
  /// 
  /// \return
  ///   BOOL: TRUE if the operation succeeded, otherwise false. You may use the
  ///   VVideo::GetLastError method to obtain the error string.
  VISION_APIFUNC BOOL ChangeScreenResolution(int newSizeX, int newSizeY, BOOL bFullscreen, SLONG refreshRate);




  ///
  /// @}
  ///

  ///
  /// @name Getters and Querying Methods
  /// @{
  ///
  
  /// \brief
  ///   Returns information about the current graphics mode
  /// 
  /// Returns a VVideoConfig structure filled with information about the current graphics mode.
  /// 
  /// \return
  ///   VVideoConfig *pConfig: video mode configuration structure
  VISION_APIFUNC VVideoConfig *GetCurrentConfig();
  
  
  #ifdef _VR_GCM

  /// \brief
  ///   Returns the current global GCM initialization structure
  /// 
  /// Modifications to this structure have to made before calling Vision::Video.Init() in order to
  /// have an effect.
  /// 
  /// This method can be used to change debug settings before initializing the Vision Engine.
  /// 
  /// \return
  ///   VGcmConfig *pConfig: GCM initialization data structure
  VISION_APIFUNC VGcmConfig *GetCurrentGcmConfig();

  #endif
  


  /// \brief
  ///   Returns the width of the screen mode currently used by the Vision engine
  /// 
  /// \return
  ///   int width
  VISION_APIFUNC int GetXRes() const;
  
  /// \brief
  ///   Returns the height of the screen mode currently used by the Vision engine
  /// 
  /// \return
  ///   int height
  VISION_APIFUNC int GetYRes() const;

  /// \brief
  ///   Returns the x position of the window currently used by the Vision engine
  /// 
  /// \return
  ///   int x position
  VISION_APIFUNC int GetXPos() const;
  
  /// \brief
  ///   Returns the y position of the window currently used by the Vision engine
  /// 
  /// \return
  ///   int y position
  VISION_APIFUNC int GetYPos() const;

  
  /// \brief
  ///   Returns whether screen mode is initialized
  VISION_APIFUNC bool IsInitialized() const;

  /// \brief
  ///   Displays the contents of the current render target on the screen
  /// 
  /// The behavior can be controlled using the settings in the VVideoConfig class.
  /// 
  /// Unlike UpdateScreen, this method always immediately forces a Present operation (i.e. buffer
  /// swap or back buffer to front buffer copy). 
  ///
  /// \param pVideoConfig
  ///   VVideoConfig containing the information of the swap chain to present (has to be NULL on consoles).
  VISION_APIFUNC void Present(VVideoConfig *pVideoConfig = NULL);

  /// \brief
  ///   Suspends rendering for automatic flipping of the screen.
  /// 
  /// While rendering is suspended, the system will automatically take care of flipping the
  /// rendering buffers at least 15 times a second. This facilitates passing certification
  /// requirements from console manufacturers.
  /// 
  /// While rendering is suspended, do not call any rendering-related functions.
  /// 
  /// This method is currently only supported on Xbox360 and PlayStation 3.
  /// 
  /// Call ResumeRendering to resume rendering again.
  VISION_APIFUNC void SuspendRendering();

  /// \brief
  ///   Resumes rendering if it was previously suspended with VisVideo_cl::SuspendRendering.
  VISION_APIFUNC void ResumeRendering();



  
  #ifdef _VISION_PS3

  /// \brief
  ///   PS3 only: Returns a pointer to the global GCM initialization structure
  /// 
  /// Modify this structure ahead of video initialization to change the global GCM configuration
  /// options.
  /// 
  /// \return
  ///   VGcmConfig *: Pointer to the global GCM initialization structure.
  inline VGcmConfig *GetGcmConfig() { return &m_GcmConfig; }

  /// \brief
  ///   PS3 only: Returns the address of the mapped main memory 
  inline void* GetMappedMainMemoryAddress() { return m_pMappedMainMemoryAddress; }

  #endif
  


#ifdef _VISION_PSP2

  /// \brief
  ///   PSP2 only: Returns a pointer to the global GXM initialization structure
  /// 
  /// Modify this structure ahead of video initialization to change the global GXM configuration
  /// options.
  /// 
  /// \return
  ///   VGXMConfig *: Pointer to the global GXM initialization structure.
  inline VGXMConfig *GetGxmConfig() { return &m_GxmConfig; }

#endif

#ifdef _VISION_WINRT
  void queueScreenConfigUpdate(const VWinRTConfig& newConfig);
  VMutex& getWinRTVisibleLock() { return m_winRTVisibleLock; }
#endif

  ///
  /// @}
  ///

private:
  friend class Vision;
  friend class VisError_cl;
  friend void Vision_GL_RenderWorld();

  void InitTexturemanager();
  //void FillVideoConfig(VVideoConfig &vc, int sizeX, int sizeY, SLONG colorDepth, SLONG zBufferDepth, BOOL fullScreen, BOOL fullControl, int windowHandle, SLONG refreshRate);

  bool m_bSMInitialized;                 ///< TRUE if valid screen mode is initialized
  VVideoConfig currentConfig;            ///< information structures for the current screen config
   
#ifdef _VISION_WINRT
  VMutex m_winRTVisibleLock;
  VMutex m_winRTConfigUpdateLock;
  bool m_winRTConfigUpdateRequired; // Is there a screen config upodate pending
  VWinRTConfig m_winRTConfigUpdate; // State of screen, to be reflected at next chance
#endif	

};

#include <Vision/Runtime/Engine/Renderer/Video/VisApiVideo.inl>

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
