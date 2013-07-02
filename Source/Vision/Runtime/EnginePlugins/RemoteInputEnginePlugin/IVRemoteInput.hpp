/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef IVREMOTE_INPUT_HPP
#define IVREMOTE_INPUT_HPP

/// \brief 
///   Additional functionality provided by the RemoteInput plugin
class IVRemoteInput : public IVisPlugin_cl {
public:

  /// \brief
  ///   Starts the HTTP server that provides the RemoteInput app for HTML5 based devices.
  ///
  /// Before starting the remote input server, make sure that the vRemoteInput plugin has been loaded.
  ///
  /// \param dataDirectory
  ///   The directory the user GUI data is located in.
  ///
  /// \param iPort
  ///   The port the http server should use.
  ///
  /// \return
  ///   True on success, false otherwise.
  virtual bool StartServer(const char* dataDirectory, int iPort = 8080) = 0;

  /// \brief
  ///   Stops the HTTP server if it is running.
  virtual void StopServer() = 0;

  /// \brief
  ///  Draws all touch areas on the screen using lines
  ///
  /// \param color
  ///   The color the debugging lines should have.
  virtual void DebugDrawTouchAreas(VColorRef color) = 0;

  /// \brief
  ///  Draws all touch points on the screen using lines
  ///
  /// \param color
  ///   The color the debugging lines should have.
  virtual void DebugDrawTouchPoints(VColorRef color) = 0;

  /// \brief
  ///  Initialized the emulated devices, this is done automatically when the vRemoteInput plugin is loaded
  ///  however as vForge deinitializes the input system after each play game session this method needs to be recalled
  ///  at the start of a play game session.
  ///
  virtual void InitEmulatedDevices() = 0;

  /// \brief
  ///  Required in case you manually deactiveate the emulated devices.
  ///
  virtual void DeinitEmulatedDevices() = 0;

  /// \brief
  ///   Adds a new variable that is send to the remote device.
  ///
  /// \param varName
  ///   The name of the variable.
  /// 
  /// \param value
  ///   The value of the variable. This value is interpreted by the JavaScript engine of the device so you can use any JavaScript values
  ///   e.g. 3.45 or 'hello world'.
  ///
  /// \return
  ///   True on success, false otherwise.
  virtual bool AddVariableDirect(const char* varName, const char* value) = 0;

  virtual bool AddVariable(const char* varName, float value) = 0;

  virtual bool AddVariable(const char* varName, int value) = 0;

  virtual bool AddVariable(const char* varName, const char* value ) = 0;
  

  /// \brief
  ///   Updates an existing variable.
  ///
  /// \param varName
  ///   Name of the variable.
  ///
  /// \param value
  ///   New value of the variable.
  ///
  /// \return
  ///   True on success, false otherwise.
  virtual bool UpdateVariableDirect(const char* varName, const char* value) = 0;

  virtual bool UpdateVariable(const char* varName, float value) = 0;

  virtual bool UpdateVariable(const char* varName, int value) = 0;

  virtual bool UpdateVariable(const char* varName, const char* value) = 0;

  /// \brief
  ///   Removes an existing variable.
  ///
  /// \param varName
  ///   Name of the variable.
  ///
  /// \return
  ///   True on success, false otherwise.
  virtual bool RemoveVariable(const char* varName) = 0;

  /// \brief
  ///  Enables or disables the resizing of the screen to the mobile device resolution.
  ///
  /// \param bOn
  ///   True for on, false for off.
  virtual void SetResizeOnConnect(bool bOn) = 0;

  /// \brief
  ///  Enables or disables the smoothing of the incoming touch data.
  ///
  ///  Enabled by default.
  /// 
  /// \param bOn
  ///   True for on, false for off.
  virtual void SetSmoothTouchInput(bool bOn) = 0;

  /// \brief
  ///   If the mouse input should be disabled on remote device connection (on by default)
  ///
  /// \param bOn
  ///   true for on, false for off
  virtual void SetDisableMouseInput(bool bOn) = 0;

  /// \brief
  ///  callback that is triggered everytime a device connects
  VisCallback_cl OnDeviceConnected;
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
