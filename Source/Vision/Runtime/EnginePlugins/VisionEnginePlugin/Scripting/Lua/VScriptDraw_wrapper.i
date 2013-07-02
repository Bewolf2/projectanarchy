
#ifndef VLUA_APIDOC

%nodefaultctor VScriptDraw_wrapper;
%nodefaultdtor VScriptDraw_wrapper;

class VScriptDraw_wrapper
{
public:

  inline void Line(const hkvVec3* pPoint1, const hkvVec3* pPoint2, VColorRef color = V_RGBA_WHITE);

  inline void Line2D(float x1, float y1, float x2, float y2, VColorRef color = V_RGBA_WHITE);

  inline void Box(const hkvVec3* pCenter, float fSize, VColorRef color = V_RGBA_WHITE);
  
  inline void Box(const hkvVec3* pMin, const hkvVec3* pMax, VColorRef color = V_RGBA_WHITE);

  inline void BoundingBox(const hkvAlignedBBox *pBox, VColorRef color = V_RGBA_WHITE);

  inline void BoundingBox(VisBaseEntity_cl *pEntity, VColorRef color = V_RGBA_WHITE);
  
  inline void OrientedBoundingBox(const hkvAlignedBBox *pBox, const hkvMat3 *pRotMat, const hkvVec3* pPos, VColorRef color = V_RGBA_WHITE);
  
  inline void OrientedBoundingBox(VisBaseEntity_cl *pEntity, VColorRef color = V_RGBA_WHITE);
  
  inline void BoneBoundingBox(VisBaseEntity_cl *pEntity, const char * szSpecificBone = NULL, VColorRef color = V_RGBA_WHITE);

  inline void ParticleBoundingBox(VisParticleEffect_cl *pParticleEffect);

  inline void Path(VisPath_cl *pPath, VColorRef color=V_RGBA_GREEN);

  inline void Wallmark(const hkvVec3* pCurrentPos,
                           const hkvVec3* pDir,
                           const char * szTexture,
                           int iBlendMode = BLEND_ALPHA,
                           float fSize = 50.f,
                           float fRotation = 0.f,
                           float fLifeTime = 20.f      );

};

#else

/// \brief Class for draw operations (Bounding Boxes, Bones, etc.). Appears as member of the \b Debug and \b Renderer classes with the name \b Draw.
/// \par Example
///   \code
///     function OnThink(self)
///       Debug.Draw:BoundingBox(self, Vision.V_RGBA_GREEN)
///     end  
///   \endcode
class VScriptDraw_wrapper {
public:

 
  /// @name Draw Operations
  /// @{
 
  /// \brief Draw a line between two 3D points.
  /// \param p1 Start point.
  /// \param p2 End point.
  /// \param color (\b optional) The color of the line.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       -- draw a line from the origin to the entity position.
  ///       Renderer.Draw:Line(Vision.hkvVec3(), self:GetPosition())
  ///     end  
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaDraw.scene"
  void Line(hkvVec3 p1, hkvVec3 p2, VColorRef color = V_RGBA_WHITE);

  /// \brief Draw a line between two 2D points.
  /// \param x1 Start point X.
  /// \param y1 Start point Y.
  /// \param x2 End point X.
  /// \param y2 End point Y.
  /// \param color (\b optional) The color of the line.
  /// \par Example
  ///   \code
  ///     local width, height = Screen:GetViewportSize()
  ///     Renderer.Draw:Line2D(0,0,width,height, Vision.V_RGBA_PURPLE)
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaDraw.scene"
  void Line2D(number x1, number y1, number x2, number y2, VColorRef color = V_RGBA_WHITE);

  /// \brief Draw a wireframe box with a specified edge size (a cube).
  /// \param center The center point of the cube.
  /// \param size The size of the cube.
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:Box(self:GetPosition(), 30)
  ///     end  
  ///   \endcode
  void Box(hkvVec3 center, number size, VColorRef color = V_RGBA_WHITE);
  
  /// \brief Draw a wireframe box using a min and a max point.
  /// \param min The first corner of the box.
  /// \param max The second corner of the box.
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:Box(Vision.hkvVec3(-10,-10,10), Vision.hkvVec3(10,10,10))
  ///     end  
  ///   \endcode
  void Box(hkvVec3 min, const hkvVec3 max, VColorRef color = V_RGBA_WHITE);

  /// \brief Draw a wireframe bounding box.
  /// \param box The bounding box to draw.
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:BoundingBox(self:GetBoundingBox(), Vision.V_RGBA_YELLOW)
  ///     end  
  ///   \endcode
  void BoundingBox(hkvAlignedBBox box, VColorRef color = V_RGBA_WHITE);

  /// \brief Draw a wireframe bounding box.
  /// \param entity The entity from which the bounding box will be grabbed.
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:BoundingBox(self, Vision.V_RGBA_GREEN)
  ///     end  
  ///   \endcode
  void BoundingBox(VisBaseEntity_cl entity, VColorRef color = V_RGBA_WHITE);
  
  /// \brief Draw a wireframe an oriented bounding box.
  /// \param box The bounding box to draw after the transormations have been applied (local space, centered around 0,0,0).
  /// \param rotMat The roation matrix for the bounding box.
  /// \param pos The position of the bounding box (translation).
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:OrientedBoundingBox(self, Vision.V_RGBA_CYAN)
  ///     end  
  ///   \endcode
  void OrientedBoundingBox(hkvAlignedBBox box, hkvMat3 rotMat, hkvVec3 pos, VColorRef color = V_RGBA_WHITE);
  
  /// \brief Draw a wireframe an oriented bounding box.
  /// \param entity The entity from which the bounding box will be grabbed.
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:OrientedBoundingBox(self, Vision.V_RGBA_BLUE)
  ///     end  
  ///   \endcode
  void OrientedBoundingBox(VisBaseEntity_cl entity, VColorRef color = V_RGBA_WHITE);
  
  /// \brief Draw the bounding boxes of the model bones.
  /// \param entity The entity from which the bone bounding box will be grabbed.
  /// \param specificBone (\b optional) The name of a single bone, if you would like to draw a specific bone (nil = all bones).
  /// \param color (\b optional) The color of the lines.
  /// \par Example
  ///   \code
  ///     function OnThink(self)
  ///       Debug.Draw:BoneBoundingBox(self, "skeleton1:Head", Vision.V_RGBA_BLUE)
  ///     end  
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaDraw.scene"
  void BoneBoundingBox(VisBaseEntity_cl entity, string specificBone = nil, VColorRef color = V_RGBA_WHITE);

  /// \brief Draw the bounding box of a particle effect.
  /// \param effect The desired particle effect.
  void ParticleBoundingBox(VisParticleEffect_cl effect);
  
  /// \brief Draw a path.
  /// \param path The path to draw
  /// \param color (\b optional) The color used for the line.
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       self.myPath = Game:GetPath("circle")
  ///     end  
  ///
  ///     function OnThink(self)
  ///       Renderer.Draw:Path(self.myPath, Vision.V_RGBA_WHITE)
  ///     end  
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaDraw.scene"
  void Path(VisPath_cl path, VColorRef color=V_RGBA_GREEN);

  /// \brief Draw (project) wallmarks using a position and a direction vector.
  /// \param pos The (projection) start position for the wallmarks.
  /// \param dir The direction (projection origin) where to put the wallmarks.
  /// \param texture The file name of the texture to be used for the wallmark.
  /// \param blendMode (\b optional) The belnd mode of the wallmark texture, these methods are available:
  ///   - BLEND_OPAQUE: Draw the texture opaque.
  ///   - BLEND_ADDITIVE: Add the color values of the texture.
  ///   - BLEND_ADDITIVE_NOA: Add the color values of the texture ignoring source and destination alpha.
  ///   - BLEND_MULTIPLY: Multiply color values of the texture.
  ///   - BLEND_ALPHA: Just draw values passing the alpha test.
  ///   - BLEND_ALPHA_TEST: Use alpha test when draing the texture
  ///   - BLEND_ADD_MODULATE: Draw the modulated result (dest=dest*(1+src)).
  /// \param size (\b optional) The size of the wallmarks at the target position (where the dir vector points to).
  /// \param rotation (\b optional) The roation of the texture in degrees around the "pos-dir" axis.
  /// \param lifeTime (\b optional) The liftime of the wallmarks (will fade out).
  /// \par Example
  ///   \code
  ///     function OnCreate(self)
  ///       Input:SetKeyAsSingleHit(Vision.KEY_SPACE) -- setup SPACE as a single hit key
  ///     end  
  ///
  ///     function OnThink(self)
  ///       -- draw a wallmark on the floor (direction 0,0,-1)
  ///
  ///       if Input:IsKeyPressed(Vision.KEY_SPACE) then
  ///         local dir = Vision.hkvVec3(0,0,-1)
  ///         local pos = self:GetPosition()
  ///         
  ///         Debug.Draw:Wallmark(pos,dir, "Scripting/Textures/vision.dds")
  ///       end
  ///     end
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaDraw.scene", Lua scripts attached to the Entity "DrawCursor".
  void Wallmark(hkvVec3 pos, hkvVec3 dir, string texture, number blendMode = BLEND_ALPHA, number size = 50, number rotation = 0, number lifeTime = 20);
  
  /// @}
};

#endif