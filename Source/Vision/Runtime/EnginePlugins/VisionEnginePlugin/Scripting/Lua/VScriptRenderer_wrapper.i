/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLUA_APIDOC

class VScriptRenderer_wrapper
{
public:
  VScriptRenderer_wrapper();
  ~VScriptRenderer_wrapper();
  
  VScriptDraw_wrapper Draw;
  
  %extend{
    IVTimeOfDay * GetTimeOfDayHandler()
    {
      return Vision::Renderer.GetTimeOfDayHandler();
    }

    IVRendererNode* GetRendererNode(int iIndex = 1)
    {
      return (iIndex>0 && iIndex <= V_MAX_RENDERER_NODES) ? Vision::Renderer.GetRendererNode(iIndex-1) : 0;
    }

    void SetGlobalAmbientColor(VColorRef ambientColor)
    {
      Vision::Renderer.SetGlobalAmbientColor(hkvVec4((float)ambientColor.r/255.f, (float)ambientColor.g/255.f, (float)ambientColor.b/255.f, (float)ambientColor.a/255.f));
    }

    void SetWireframeMode(bool bStatus)
    {
      Vision::Renderer.SetWireframeMode(bStatus);
    }

    bool GetWireframeMode()
    {
      return Vision::Renderer.GetWireframeMode();
    }

    const char * GetVertexShaderVersion()
    {
      VShaderModel_e version = Vision::Renderer.GetVertexShaderVersion();

      switch(version)
      {
        case VSM_BEST: return "BEST";
        case VSM_SM11: return "SM11";
        case VSM_SM20: return "SM20";///< vs and ps
        case VSM_SM2a: return "SM2A";///< vs and ps
        case VSM_SM2b: return "SM2B";///< vs and ps
        case VSM_SM30: return "SM30";///< vs and ps
        case VSM_SM40: return "SM40";///< vs, ps and gs (DX10, DX11)
        case VSM_SM41: return "SM41";///< vs, ps and gs (DX10, DX11)
        case VSM_SM40_LEVEL_91: return "SM4L91";///< vs and ps (DX11 Level 9)
        case VSM_SM40_LEVEL_93: return "SM4L93";///< vs and ps (DX11 Level 9)
        case VSM_SM50: return "SM5";///< vs, ps, and gs (DX11)
        default: return "INVALID";
      }
    }
  }
};

#else

/// \brief This wrapper contains rendering related functions and appears in Lua as \b Renderer.
/// \par Example
///   \code
///     Renderer.Draw:Wallmark(pos,dir, "Scripting/Textures/vision.dds", Vision.BLEND_ALPHA, 100, -90)
///   \endcode
class VScriptRenderer_wrapper {
public:

  /// @name Public Members
  /// @{
  
  /// \brief Use this member to perform any draw operation.
  /// \par Example
  ///   \code
  ///   function OnThink(self)
  ///     Renderer.Draw:BoundingBox(self);
  ///   end
  ///   \endcode
  VScriptDraw_wrapper Draw;
  
  /// @}
  /// @name Time Of Day and Renderer Node Access
  /// @{

  /// \brief Returns time of day handler.
  /// \return The handler or nil.
  /// \par Example
  ///   \code
  ///     local handler = Renderer:GetTimeOfDayHandler()
  ///
  ///     if handler == nil then return end
  ///
  ///     local currentTime = handler:GetDayTime()
  ///   \endcode
  IVTimeOfDay GetTimeOfDayHandler();
  
  /// \brief Returns a specified render node. If no renderer node is set the function will return nil.
  /// \param index [\b Optional] 1 based index (0 in C++) of the required renderer node, limited by V_MAX_RENDERER_NODES, default is 1.
  /// \return The renderer node or nil.
  IVRendererNode GetRendererNode(int index = 1);

  /// @}
  /// @name Utility Functions
  /// @{

  /// \brief Set the global ambient color.
  /// \param ambientColor The new ambient color to set.
  void SetGlobalAmbientColor(VColorRef ambientColor);

  /// \brief Enable or disable the wireframe mode.
  /// \param enable Set to true to enable, false to disable.
  void SetWireframeMode(boolean enable);

  /// \brief Check wether the wirefrae mode is enabled.
  /// \return Returns true if enabled, false if disabled.
  boolean GetWireframeMode();

  /// \brief Returns the maximum vertex shader version supported on the graphics device.
  /// \return A string describing the supported vertex shader model ("SM11", "SM20", "SM2A", "SM2B", "SM30", "SM40", "SM41", "SM4L91", "SM4L93", "SM5", "INVALID").
  string GetVertexShaderVersion();

  /// @}
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
