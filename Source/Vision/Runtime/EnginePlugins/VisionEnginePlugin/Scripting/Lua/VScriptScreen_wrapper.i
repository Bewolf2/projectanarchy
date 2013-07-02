
#ifndef VLUA_APIDOC

class VScriptScreen_wrapper
{
public:
  VScriptScreen_wrapper();
  ~VScriptScreen_wrapper();
  
  hkvVec3* PickPoint(float x, float y, float fMaxDist=10000.f, bool bIgnoreEntities=false, bool bIgnoreStaticGeomerty=false);

  hkvVec3* Project3D(float x, float y, float fDist);

  VisBaseEntity_cl * PickEntity(float x, float y, float fMaxDist=10000.f, bool bIgnoreStaticGeomerty=false);

  bool SaveToFile(const char * szFileName = NULL, bool bJpgInsteadBmp = true);

  float GetDeviceDpi();
};

//we return two parameter in Project2D, so it has to be implemented native
%native(Project2D) int VScriptScreen_wrapper_Project2D(lua_State *L);
%{
  SWIGINTERN int VScriptScreen_wrapper_Project2D(lua_State *L)
  {
    IS_MEMBER_OF(VScriptScreen_wrapper) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 2, hkvVec3, pVec)

    float x,y;
    if(Vision::IsInitialized() && Vision::Contexts.GetCurrentContext()->Project2D(*pVec,x,y))
    {
      lua_pushnumber(L, (lua_Number)x);
      lua_pushnumber(L, (lua_Number)y);
    }
    else
    {
      lua_pushnil(L);
      lua_pushnil(L);
    }

    return 2; 
  }
%}
   
//we return two parameter in GetViewportSize, so it has to be implemented native
%native(GetViewportSize) int VScriptScreen_wrapper_GetViewportSize(lua_State *L);
%{
  SWIGINTERN int VScriptScreen_wrapper_GetViewportSize(lua_State *L)
  {
    IS_MEMBER_OF(VScriptScreen_wrapper) //this will move this function to the method table of the specified class

    lua_settop(L,0);

    if(Vision::IsInitialized() && Vision::Contexts.GetMainRenderContext())
    {
      int x, y, w, h;
      Vision::Contexts.GetMainRenderContext()->GetViewport(x, y, w, h);
      lua_pushnumber(L, (lua_Number)w);
      lua_pushnumber(L, (lua_Number)h);
    }
    else
    {
      lua_pushnil(L);
      lua_pushnil(L);
    }

    return 2; 
  }
%}

#else

/// \brief Deals with screen space operations. Appears in Lua as the global instance \b Screen.
/// \par Example
///   \code
///  function OnThink(self)
///    if Input:IsMouseButtonPressed(Vision.BUTTON_LEFT) then
///      local x,y = Input:GetMousePosition()
///      local entity = Sceen:PickEntity(x,y, 1000, true)
///      if Input:IsMouseButtonPressed(Vision.BUTTON_LEFT)entity ~= nil then
///        Debug:PrintAt(x,y, "Picked '"..tostring(entity).."'")
///      end
///    end
///  end
///   \endcode
class VScriptScreen_wrapper {
public:

  /// @name Screen Space Transformations
  /// @{
 
  /// \brief Pick a point in 3D space from 2D screen coordinates.
  /// \param x Screen space x.
  /// \param y Screen space y.
  /// \param maxDist (\b optional) farthest test distance.
  /// \param ignoreEntities (\b optional) Set to true if you would like to ignore entity picking.
  /// \param ignoreStaticGeomerty (\b optional) Set to true if you would like to ignore static geometry picking.
  /// \return A vector containing the 3D point where where gemoetry has been hit, or nil if there was no hit.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    local x,y = Input:GetMousePosition()
  ///    local position = Screen:PickPoint(x,y)
  ///    if position ~= self.oldPosition then
  ///      self.oldPosition = position
  ///    end
  ///    Debug:PrintAt(self.oldPosition, "< Current 3D mouse position!")
  ///  end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaScreen.scene"
  hkvVec3 PickPoint(number x, number y, number maxDist=10000, boolean ignoreEntities=false, boolean ignoreStaticGeomerty=false);
 
  /// \brief Project a point to 3D space from 2D screen coordinates.
  /// \param x Screen space x.
  /// \param y Screen space y.
  /// \param Dist projection distance from camera
  /// \return A vector containing the 3D projected point in space
  hkvVec3* Project3D(float x, float y, float Dist);

  /// \brief Pick an entity from 2D screen coordinates.
  /// \param x Screen space x.
  /// \param y Screen space y.
  /// \param maxDist (\b optional) farthest test distance.
  /// \param ignoreStaticGeomerty (\b optional) Set to true if you would like to ignore static geometry.
  /// \return An entity if picking was successful, otherwise nil.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    if Input:IsMouseButtonPressed(Vision.BUTTON_LEFT) then
  ///      local x,y = Input:GetMousePosition()
  ///      local entity = Sceen:PickEntity(x,y)
  ///      if entity ~= nil then
  ///        Debug:PrintLine("You picked "..tostring(entity))
  ///      end
  ///    end
  ///  end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaScreen.scene"
  VisBaseEntity_cl PickEntity(number x, number y, number maxDist=10000, boolean ignoreStaticGeomerty=false);
 
  /// \brief Transform a 3D position into a 2D screen position.
  /// \param point The 3D world space position to transform to screen space.
  /// \return Two numbers representing the x and y screen space position of the specified 3D point.
  /// \par Example
  ///   \code
  ///  function OnThink(self)
  ///    if self:WasVisibleLastFrame() then
  ///      local x,y = Screen:Project2D()
  ///      Debug:PrintAt(x,y, "I'm here!")
  ///    end
  ///  end
  ///   \endcode
  multiple Project2D(hkvVec3 point);
  
  /// @}
  /// @name Misc. Functions
  /// @{

  /// \brief Save a screen shot.
  /// \param fileName (\b optional) The filename of the screen shot.
  /// \param jpgInsteadBmp (\b optional) Set to true to save as JPG or set to false to save as BMP.
  /// \return true on succees, otherwise false.
  boolean SaveToFile(string fileName = nil, boolean jpgInsteadBmp = true);

  /// \brief Get the size of the viewport.
  /// \return Two numbers containing width and height of the viewport.
  /// \par Example
  ///   \code
  ///  function OnCreate(self)
  ///    local x,y = Screen:GetViewportSize()
  ///    Debug:PrintLine("The current vieport size is: "..x..", "..y)
  ///  end
  ///   \endcode
  multiple GetViewportSize();


  /// \brief
  ///   For devices supporting querying device display resolutions, this function returns the display resolution in dots per inch (dpi).
  /// 
  /// Supported on Android, iOS, and Windows RT. Returns a default value of 96 on other platforms, which is the value used by Windows.
  float GetDeviceDpi();
  
  /// @}
};

#endif