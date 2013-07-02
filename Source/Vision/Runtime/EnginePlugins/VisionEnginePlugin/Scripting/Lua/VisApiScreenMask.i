
#ifndef VLUA_APIDOC

%nodefaultctor VisScreenMask_cl;
%nodefaultdtor VisScreenMask_cl;

class VisScreenMask_cl
{
public:

  %extend{
    void SetBlending(int iTranspMode)
    {
      self->SetTransparency((VIS_TransparencyType)iTranspMode);
    }
    
    int GetBlending()
    {
      return (int) self->GetTransparencyType();
    }
    
    void SetWrapping(bool bWrapU, bool bWrapV)
    {
      self->SetWrapping(bWrapU, bWrapV);
    }
    
    void SetVisible(bool bVisible)
    {
      self->SetVisibleBitmask(bVisible ? 0xffffffff:0);
    }
    
    bool IsVisible()
    {
      return self->GetVisibleBitmask()!=0;
    }
  
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetFiltering);
    VSWIG_CONVERT_BOOL_SETTER(SetFiltering);
    
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetDepthWrite);
    VSWIG_CONVERT_BOOL_SETTER(SetDepthWrite);
  
    VSWIG_BITMASK_SETTER(SetVisibleBitmask);
    VSWIG_BITMASK_GETTER_CONST(GetVisibleBitmask);
  }
  
  inline void SetColor(VColorRef iColor);
  inline VColorRef GetColor() const;

  inline void SetTargetSize(float xSize, float ySize);
  
  inline void SetTextureRange(float uLower, float vLower, float uUpper, float vUpper);
  
  inline void SetPos(float xPos, float yPos, float zVal);
  inline void SetPos(float xPos, float yPos);
  
  void SetZVal(float zVal);
  inline float GetZVal() const;
   
  void SetKey(const char *pszNewKey);
  inline const char *GetKey() const;

  inline void SetOrder(int newOrder);
  inline int GetOrder() const;

  inline void SetRotationAngle(float fAngle);
  inline float GetRotationAngle() const;

  inline void SetRotationCenter(float fCenterX, float fCenterY);

  
  inline VTextureObject* GetTextureObject();
  void SetTextureObject(VTextureObject* pTexture);
  
  void Unload();

};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisScreenMask_cl, 64, "[%s]", self->GetTextureObject()==NULL?"empty":self->GetTextureObject()->GetFilename())

//native to string
%native(VisScreenMask_cl___tostring) int VisScreenMask_cl_ToString(lua_State *L);
%{
  SWIGINTERN int VisScreenMask_cl_ToString(lua_State *L)
  {
    //this will move this function to the method table of the specified class
    IS_MEMBER_OF(VisScreenMask_cl)
    
    SWIG_CONVERT_POINTER(L, -1, VisScreenMask_cl, pSelf)
    
    char pszBuffer[512];
    
    float x,y,w,h;
    pSelf->GetPos(x,y);
    pSelf->GetTargetSize(w,h);
    
    sprintf(pszBuffer, "VisScreenMask_cl x:%0.0f y:%0.0f (%0.0fx%0.0f) '%s'", x, y, w, h, pSelf->GetTextureObject()==NULL ? "empty" : pSelf->GetTextureObject()->GetFilename()); //format as requested
      
    lua_pushstring(L, pszBuffer);
    
    return 1;
  }
%}

//implement GetTextureSize native because it returns two values at once
%native(VisScreenMask_cl_GetTextureSize) int VisScreenMask_cl_GetTextureSize(lua_State *L);
%{
  SWIGINTERN int VisScreenMask_cl_GetTextureSize(lua_State *L)
  {
    IS_MEMBER_OF(VisScreenMask_cl) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VisScreenMask_cl, pSelf)
    
    int x,y;
    pSelf->GetTextureSize(x,y);
    
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
    
    return 2; //we leave two values on the stack
  }
%}

//implement GetPos native because it returns two values at once
%native(VisScreenMask_cl_GetPos) int VisScreenMask_cl_GetPos(lua_State *L);
%{
  SWIGINTERN int VisScreenMask_cl_GetPos(lua_State *L)
  {
    IS_MEMBER_OF(VisScreenMask_cl) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, VisScreenMask_cl, pSelf)
    
    float x,y;
    pSelf->GetPos(x,y);
    
    lua_pushnumber(L, (lua_Number)x);
    lua_pushnumber(L, (lua_Number)y);
    
    return 2; //we leave two values on the stack
  }
%}

#else

/// \brief Screen mask wrapper class for 2-dimensional screen overlays
/// 
/// \par Example
///   \code
///     function OnAfterSceneLoaded(self)
///       local screenMask = Game:CreateScreenMask(0,0, "Textures/hud.dds", "SCREENMASK_KEY")
///       screenMask:SetBlending(Vision.BLEND_ADDITIVE)
///     end
///     
///     function OnBeforeSceneUnloaded(self)
///       Game:DeleteAllUnrefScreenMasks()
///     end
///   \endcode
class VisScreenMask_cl
{
public:

  /// @name Appearance Setup
  /// @{
  
  /// \brief Set the blending mode of this screen mask. 
  /// \param blendMode The belnd modeused to draw the screen mask:
  ///   - BLEND_OPAQUE: Draw the texture opaque.
  ///   - BLEND_ADDITIVE: Add the color values of the texture.
  ///   - BLEND_ADDITIVE_NOA: Add the color values of the texture ignoring source and destination alpha.
  ///   - BLEND_MULTIPLY: Multiply color values of the texture.
  ///   - BLEND_ALPHA: Just draw values passing the alpha test.
  ///   - BLEND_ALPHA_TEST: Use alpha test when draing the texture
  ///   - BLEND_ADD_MODULATE: Draw the modulated result (dest=dest*(1+src)).
  /// \par Example
  ///   \code
  ///     ...
  ///       local screenMask = Game:CreateScreenMask(100,100, "hello.dds")
  ///       screenMask:SetBlending(Vision.BLEND_ALPHA)
  ///     ...
  ///   \endcode
  /// \see VisScreenMask_cl::GetBlending
  void SetBlending(number blendMode);
  
  /// \brief get the current blending mode of the screen mask
  /// \note If the returned value does not match one of the BLEND_* values,
  /// the screenmask has not been created via the scripting system.
  /// \return The blending mode as number, having one of the following values:
  ///   - BLEND_OPAQUE: Draw the texture opaque.
  ///   - BLEND_ADDITIVE: Add the color values of the texture.
  ///   - BLEND_ADDITIVE_NOA: Add the color values of the texture ignoring source and destination alpha.
  ///   - BLEND_MULTIPLY: Multiply color values of the texture.
  ///   - BLEND_ALPHA: Just draw values passing the alpha test.
  ///   - BLEND_ALPHA_TEST: Use alpha test when draing the texture
  ///   - BLEND_ADD_MODULATE: Draw the modulated result (dest=dest*(1+src)).
  /// \see VisScreenMask_cl::SetBlending
  number GetBlending();

  /// \brief Sets the modulation color (tints the base texture) for the screen mask
  /// \param color The new color tinting value.
  /// \see VisScreenMask_cl::GetColor
  void SetColor(VColorRef color);

  /// \brief Gets the current modulation color of the screen mask
  /// \return The modulation color of the screen mask.
  /// \see VisScreenMask_cl::SetColor
  VColorRef GetColor();
  
  /// \brief Set the wrapping mode of the screen mask in u- and v-direction.
  /// \note If set to false, the texture is clamped at its border. If set to true
  /// the texture is repeated.
  /// \param wrapU Wrapping mode for the texture's u-direction (default is ture).
  /// \param wrapV Wrapping mode for the texture's v-direction (default is ture).
  void SetWrapping(boolean wrapU, boolean wrapV);

  /// \brief Enables or disables bi-linear texture filtering when rendering the mask.
  /// \param status Set to true to enable bilinear texture filtering, false to diable (default is true).
  void SetFiltering(boolean status);

  /// \brief Get the current bi-linear filtering status of the screen mask.
  /// \return true if if using bi-linear filtering, otherwise false.
  boolean GetFiltering();
    
  /// @}
  /// @name Visiblity and Rendering Setup
  /// @{
  
  /// \brief Makes the screen mask visible or invisible in all contexts.
  /// \param visible The new visible status.
  /// \note This is a convenience function for SetVisibleBitmask.
  /// \see SetVisibleBitmask
  void SetVisible(boolean visible);

  /// \brief Checks whether the screen mask is visible in all contexts.
  /// \return true if visible; other false.
  /// \note This is a convenience function for GetVisibleBitmask.
  /// \see GetVisibleBitmask
  boolean IsVisible();

  /// \brief Makes the screen mask visible in certain rendering contexts only.
  /// \details
  ///    This screen mask will only be rendered in render contexts,
  ///    if the logical AND result of this bitmask and the context's bitmask is not zero.
  /// \param mask The new visible bitmask.
  /// \see VBitmask
  void SetVisibleBitmask(VBitmask mask);

  /// \brief Gets the visible bitmask of the screen mask.
  /// \return The visible bitmask as VBitmask.
  /// \see VBitmask
  VBitmask GetVisibleBitmask();

  /// \brief Defines whether the screen mask rendering writes to the depth buffer.
  /// \param status If set to true, the screen masks writes to the depth buffer.
  void SetDepthWrite(boolean status);

  /// \brief Returns whether the screen masks writes to the depth buffer.
  /// \return true if the screen masks writes to the depth buffer, otherwise false.
  boolean GetDepthWrite();
  
  /// \brief Sets the drawing order of the screen mask.
  /// \param newOrder The new order sorting key value for this mask.
  void SetOrder(number newOrder);
  
  /// \brief Gets the drawing order of the screen mask.
  /// \return The sorting key as numeric value.
  number GetOrder() const;
  
  /// @}
  /// @name Alignment
  /// @{

  /// \brief Sets the size of the screen mask on the screen in pixels.
  /// \note By default the target size is set to the texture's size.
  /// \param xSize x-size of screen mask in pixels.
  /// \param ySize y-size of screen mask in pixels.
  void SetTargetSize(number xSize, number ySize);
  
  /// \brief Gets the size of the texture object associated with this screen mask.
  /// \note This can be different from the actual on-screen size of the mask.
  /// \return The texture size as tuple.
  /// \par Example
  ///   \code
  ///     ...
  ///     local screenMask = Game:CreateScreenMask(100,200, "frog.dds")
  ///     local x,y = screenMask:GetTextureSize()
  ///     Debug:PrintLine("frog.dds: " .. x .. "x" .. y .. " px")
  ///     ...
  ///   \endcode
  multiple GetTextureSize();
  
  /// \brief Sets the texture coordinate range (source UV rectangle) of the screen mask.
  /// \note Texture coordinates are specified in pixels. By default, the
  /// range is set to the full texture size.
  /// \param uLower u-position of the upper left corner on the texture.
  /// \param vLower v-position of the upper left corner on the texture.
  /// \param uUpper u-position of the lower right corner on the texture.
  /// \param vUpper v-position of the lower right corner on the texture.
  void SetTextureRange(number uLower, number vLower, number uUpper, number vUpper);
  
  /// \brief Sets the 2D position and the z-depth of the screen mask on the screen
  /// \note The z-depth value can be used to define the z value relative to the camera,
  /// this allows you to "clip" masks into the world. 
  /// \param x x-position of the screen mask in pixels.
  /// \param y y-position of the screen mask in pixels.
  /// \param zDepth [\b optional] z-depth of the screen mask relative to the camera.
  void SetPos(number x, number y, number zDepth = nil);
  
  /// \brief Gets the current 2D screen position of the screen mask.
  /// \return The x and y position as tuple.
  /// \par Example
  ///   \code
  ///     ...
  ///     local screenMask = Game:CreateScreenMask(100,200, "frog.dds")
  ///     local x,y = screenMask:GetPos()
  ///     Vision.Assert(x==100 and y==200, "The screen mask position is wrong")
  ///     ...
  ///   \endcode
  multiple void GetPos();
  
  /// \brief Sets the z-depth of the screen mask.
  /// \note The zVal value can be used to define the z value relative to the camera,
  /// this allows you to "clip" masks into the world. The z value doesn't influence the
  /// order the screen masks are rendered, but rather the z position of the screen mask
  /// in the world. To change the order of the screen mask rendering the function
  /// VisScreenMask_cl::SetOrder can be used instead.
  /// \param zDepth z-depth of the screen mask relative to the camera.
  void SetZVal(float zDepth);
  
  /// \brief Get the current z-depth of the screen mask.
  /// \return The z-depth of the screen mask relative to the camera.
  /// \see VisScreenMask_cl::SetZVal
  number GetZVal();
  
  /// \brief Sets the rotation angle of this mask around the rotation center.
  /// \param angle The rotation angle in degrees of the screen mask (0 is default).
  /// \see VisScreenMask_cl::SetRotationCenter
  /// \see VisScreenMask_cl::GetRotationAngle
  void SetRotationAngle(number angle);
    
  /// \brief Returns the current rotation angle of the mask (in degree).
  /// \return The current rotation angle in degrees.
  /// \see VisScreenMask_cl::SetRotationAngle
  number GetRotationAngle();
  
  /// \brief Sets the relative rotation center of the screen mask when rotating it.
  /// \param centerX The rotation center x-coordinate, relative to the mask's upper left corner.
  /// \param centerY The rotation center y-coordinate, relative to the mask's upper left corner.
  /// \see VisScreenMask_cl::SetRotationAngle
  void SetRotationCenter(number centerX, number centerY);

  /// @}
  /// @name Utility Fucntions
  /// @{
  
  /// \brief Sets the screen mask key.
  /// \param key Identification key. Can be nil.
  void SetKey(string key);
  
  /// \brief Gets the screen mask key.
  /// \return dentification key of the screen mask, can be nil.
  string GetKey();
  
  /// \brief Returns the texture object associated with this screen mask.
  /// \return A VTextureObject isntance.
  /// \see VTextureObject
  VTextureObject GetTextureObject();
  
  /// \brief Sets the texture object for this screen mask.
  /// \note When settings a new texture, the screenmask first calls the Unload function
  /// to free memory used by memory masks etc.
  /// \param texture The new texture object. A nil value is not allowed.
  void SetTextureObject(VTextureObject texture);
  
  /// \brief This function unloads the mask (i.e. releases the texture reference) and
  /// resets the screen mask settings.
  /// \see VisGame_cl::DeleteAllUnrefScreenMasks
  void Unload();
 
  /// @}

};

#endif
