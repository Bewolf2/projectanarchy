
#ifndef VLUA_APIDOC

%nodefaultctor PathCameraEntity;
%nodefaultdtor PathCameraEntity;

class PathCameraEntity : public VisBaseEntity_cl
{
public:
  bool LoadScriptFile(const char *szFilename);

  void FreeScriptFile();

  bool Start();

  void Stop();

  void SetPause(bool bStatus);

  void SetTimeScaling(float fScale);

  float GetTimeScaling() const;

  bool IsPlaying() const;

  bool IsPaused() const;

  void AttachToRendererNode(int iRendererNodeIndex);
};

#else

/// \brief Lua wrapper class for PathCameraEntity.
class PathCameraEntity : public VisBaseEntity_cl
{
public:

  /// @name Common Path Functions
  /// @{
  
  ///\brief
  ///  Load an XML camera script file from file and start it
  ///
  ///\param szFilename
  ///  Filename of the XML file
  ///
  ///\returns
  ///  true if loading was successful
  bool LoadScriptFile(const char *szFilename);

  ///\brief
  ///  Unload the XML file
  void FreeScriptFile();

  ///\brief
  ///  Start the playback (i.e. attach camera and process the camera script)
  bool Start();

  ///\brief
  ///  Stop the playback
  void Stop();

  ///\brief
  ///  Pauses or resumes the playback
  void SetPause(bool bStatus);

  ///\brief
  ///  Sets the relative playback speed (1.0f = normal speed as defined in the XML, 2.0f is twice the speed)
  void SetTimeScaling(float fScale);

  ///\brief
  ///  Return the time scaling of this camera. The default is 1.0
  float GetTimeScaling() const;

  ///\brief
  ///  Indicates whether playback is running or not
  bool IsPlaying() const;

  ///\brief
  ///  Indicates whether playback is currently paused
  bool IsPaused() const;

  ///\brief
  ///  Attaches this camera to the renderer node of the given index.
  void AttachToRendererNode(int iRendererNodeIndex);

  /// @}
};

#endif
