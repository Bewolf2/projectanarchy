#ifndef VLUA_APIDOC

class hkvAlignedBBox
{
public:
  hkvAlignedBBox (const hkvVec3& vMin, const hkvVec3& vMax);
  
  void setZero ();
  bool isZero () const;
  bool isZero (float fEpsilon) const;

  void set (const hkvVec3& vMin, const hkvVec3& vMax);
  void setCenterAndSize (const hkvVec3& vCenter, const hkvVec3& vHalfSizeXYZ);
  
  void setInvalid ();
  bool isValid (void) const;

  void expandToInclude (const hkvVec3& v);
  void expandToInclude (const hkvAlignedBBox& rhs);

  void translate (const hkvVec3& vTranslation);

  void scaleFromCenter (const hkvVec3& v);

  void scaleFromOrigin (const hkvVec3& vScaleXYZ);

  void addBoundary (const hkvVec3& vScaleXYZ);

  void expandToCube ();

  bool isIdentical (const hkvAlignedBBox& rhs) const;
  bool isEqual (const hkvAlignedBBox& rhs, float fEpsilon) const;


  const hkvVec3 getCenter () const;
  float getSizeX () const;
  float getSizeY () const;
  float getSizeZ () const;

  bool contains (const hkvVec3& v) const;
  bool contains (const hkvAlignedBBox& rhs) const;
  
  bool overlaps (const hkvAlignedBBox& rhs) const;
  
  float getDistanceToSquared (const hkvVec3& v) const;
  float getDistanceToSquared (const hkvAlignedBBox& rhs) const;
  
  float getDistanceTo (const hkvVec3& v) const;
  float getDistanceTo (const hkvAlignedBBox& rhs) const;
  
 
  hkvVec3 getClampedPosition (const hkvVec3& v) const;
  
  %extend{
  
    hkvAlignedBBox (bool bInitialize = true)
    {
      hkvAlignedBBox *pBox = new hkvAlignedBBox(hkvNoInitialization);
      if(!bInitialize) return pBox;
      pBox->setInvalid();
      return pBox;
    }
    
    hkvAlignedBBox (float x1, float y1, float z1, float x2, float y2, float z2)
    {
      return new hkvAlignedBBox(hkvVec3(x1, y1, z1), hkvVec3(x2, y2, z2));
    }
    
    void set(float x1, float y1, float z1, float x2, float y2, float z2)
    {
      self->set(hkvVec3(x1, y1, z1), hkvVec3(x2, y2, z2));
    }
    
    hkvAlignedBBox clone() const
    {
      return *self; //will call the copy constructor
    }
    
    bool __eq (const hkvAlignedBBox * pRhs) const
    {
      if(pRhs==NULL) return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }
  }
  
  hkvVec3 m_vMin;
  hkvVec3 m_vMax;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvAlignedBBox, 64, "[%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f]", self->m_vMin.x,self->m_vMin.y,self->m_vMin.z, self->m_vMax.x,self->m_vMax.y,self->m_vMax.z)
VSWIG_CREATE_TOSTRING(hkvAlignedBBox, "hkvAlignedBBox: Min[%1.2f,%1.2f,%1.2f] - Max[%1.2f,%1.2f,%1.2f]", self->m_vMin.x,self->m_vMin.y,self->m_vMin.z, self->m_vMax.x,self->m_vMax.y,self->m_vMax.z)

//Implement getCorners native because it returns an array (table) of hkvVec3s
%native(hkvAlignedBBox_getCorners) int hkvAlignedBBox_getCorners(lua_State *L);
%{
  SWIGINTERN int hkvAlignedBBox_getCorners(lua_State *L)
  {
    IS_MEMBER_OF(hkvAlignedBBox) //this will move this function to the method table of the specified class
    
    SWIG_CONVERT_POINTER(L, 1, hkvAlignedBBox, pBox)

    lua_newtable(L); //create an empty table (or an array if you would like to see it this way)
    
    hkvVec3 vecs[8];
    
    pBox->getCorners(vecs);
    
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[0]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 1);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[1]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 2);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[2]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 3);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[3]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 4);
    
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[4]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 5);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[5]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 6);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[6]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 7);
    SWIG_Lua_NewPointerObj(L,new hkvVec3(vecs[7]),SWIGTYPE_p_hkvVec3, VLUA_MANAGE_MEM_BY_LUA);
    lua_rawseti(L, -2, 8);
    
    return 1; //the table 
  }
%}

#else

/// \brief Bounding box wrapper class for hkvAlignedBBox.
/// \note Since all wrappers classes are handled as pointers in Lua you
///   cannot copy it's content by assignment. Please take a look at the code samples.
/// \par Example
///   \code
///     -- create a new color:
///     local bbox1 = Vision.hkvAlignedBBox(-1,-1,-1, 1,1,1)
///
///     -- this will not copy the content (just another pointer to it):
///     local bbox2 = bbox1
///
///     -- this will affect access to 'bbox2' and 'bbox1' since the pointers share the same object:
///     bbox2:set(-2,-2,-2, 2,2,2)
///
///     -- this assertion will not fail
///     Vision.Assert(bbox1==bbox2, "When this assertions fails 'bbox1' and 'bbox2' are pointing to different bounding boxes.")
///
///     -- in order to copy the content, you have to clone:
///     local bbox3 = bbox1:clone()
///
///     -- or create a new color using the copy constructor
///     local bbox4 = Vision.hkvAlignedBBox(bbox1)
///
///     -- to make life easier, every Vision scripting method creates a vector copy for you - e.g:
///     local b1 = entity:GetBoundingBox()
///     local b2 = entity:GetBoundingBox()
///     b1.m_vMin.x = 12
///     b2.m_vMin.x = -10
///
///     -- this assertion will not fail
///     Vision.Assert(b1~=b2, "When this assertion fails 'b1' and 'b2' are pointing to the same bounding box.")
///
///   \endcode
/// \see Clone
class hkvAlignedBBox {
public:

  /// @name Constructors
  /// @{
  
  /// \brief Default Constructor.
  /// \param initialize (\b optional) Set to true to initialize as invalid boundling box (max < min), false to create an un-initialized bounding box.
  hkvAlignedBBox(boolean initialize = true);
  
  /// \brief Constructor which takes two vectors as parameters to define the extents of the box.
  /// \param min Minimum point of the bounding box.
  /// \param max Maximum point of the bounding box.
  hkvAlignedBBox(hkvVec3 min, hkvVec3 max);
  
  /// \brief Constructor initializing the boudning box with the specified values.
  /// \param minX X position of the minimum point of the bounding box.
  /// \param minY Y position of the minimum point of the bounding box.
  /// \param minZ Z position of the minimum point of the bounding box.
  /// \param minX X position of the maximum point of the bounding box.
  /// \param minY Y position of the maximum point of the bounding box.
  /// \param minZ Z position of the maximum point of the bounding box.
  hkvAlignedBBox(number minX, number minY, number minZ, number minX, number minY, number minZ);

  /// @}
  /// @name Manipulation
  /// @{
  
  /// \brief Sets the box to all zero.
  /// \note This is a valid bounding box with a volume of zero.
  void setZero ();
  
  /// \brief Sets m_vMin to the maximum positive float value, and m_vMax to the maximum negative float value (min > max),
  /// making it an invalid box.
  /// \note The expandToInclude functions can then be used to enlarge the box until it is large enough.
  /// \see hkvAlignedBBox::expandToInclude
  void setInvalid ();

  /// \brief Sets min/max extents that make up the bounding box.
  /// \note In debug mode this will check that vMin <= vMax, to ensure a valid bbox!
  /// \param min Minimum point of the bounding box.
  /// \param max Maximum point of the bounding box.
  void set (hkvVec3 min, hkvVec3 max);
  
  /// \brief Sets min/max extents that make up the bounding box.
  /// \note In debug mode this will check that vMin <= vMax, to ensure a valid bbox!
  /// \param minX X position of the minimum point of the bounding box.
  /// \param minY Y position of the minimum point of the bounding box.
  /// \param minZ Z position of the minimum point of the bounding box.
  /// \param minX X position of the maximum point of the bounding box.
  /// \param minY Y position of the maximum point of the bounding box.
  /// \param minZ Z position of the maximum point of the bounding box.
  void set (number minX, number minY, number minZ, number minX, number minY, number minZ);
  
  /// \brief Sets the box's min and max values from the given center and half-size extents.
  /// \param center The new center position.
  /// \param halfSize The half-size extent of the new box.
  void setCenterAndSize (hkvVec3 center, hkvVec3 halfSize);
  
  /// \brief Enlarges the box (if necessary) such that the specified vector is contained in it.
  /// \param vector The vector to include.
  void expandToInclude (hkvVec3 vector);
  
  /// \brief Enlarges the bounding box (if necessary) such that the specified box is contained in it.
  /// \param box The bounding box to include.
  void expandToInclude (hkvAlignedBBox box);
 
  /// \brief Scales the box's size, but does not change its position.
  /// \param scale The scaling values for the x y and z axes.
  void scaleFromCenter (hkvVec3 scale);

  /// \brief Scales the box's min and max extents, thus changing its size and position.
  /// \param scale The scaling values for the x y and z axes.
  void scaleFromOrigin (hkvVec3 scale);

  /// \brief Changes the size in each direction, e.g. along the positive AND negative axis.
  /// \param scale The scaling values for the x y and z axes.
  void addBoundary (hkvVec3 scale);

  /// \brief Makes all sides of the box as long as its longest side, thus making it a cube.
  void expandToCube ();

  /// @}
  /// @name Checks
  /// @{
  
  /// \brief Checks whether the two boxes are identical.
  /// \param other The other box to check.
  /// \return true if identical (no epsilon involved), otherwise false.
  boolean isIdentical (hkvAlignedBBox other);
  
  /// \brief Checks whether the two boxes are equal withing some epsilon range.
  /// \param other The other box to check.
  /// \param epsilon Specify an epsilon range for this range (e.g Vision.MATH_LARGE_EPSILON).
  /// \return true if equal (within the epsilon), otherwise false.
  boolean isEqual (hkvAlignedBBox other, number epsilon);
  
  /// \brief Checks whether this bounding box is all zero (within some epsilon range).
  /// \param epsilon (\b optional) Specify an epsilon range for this range (default is Vision.MATH_DEFAULT_EPSILON).
  /// \return true if zero within the specified epsilon, otherwise false.
  boolean isZero (number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief Checks if the bounding box is valid (m_vMin <= m_vMax).
  /// \return Returns true if m_vMin <= m_vMax, otherwise false.
  boolean isValid ();

  /// \brief Returns true if the specified point is inside this bounding box.
  /// \param point The point to check.
  /// \return true if the box contains the specified point, otherwise false.
  boolean contains (hkvVec3 point);
  
  /// \brief Returns true if the specified bounding box is inside this bounding box.
  /// \param box The bounding box to check.
  /// \return true if the specified bounding box is inside this bounding box, otherwise false.
  boolean contains (hkvAlignedBBox box);
  
  /// \brief Checks if the given object is inside this box or partially overlaps it.
  /// \param box The bounding box to check.
  /// \return Returns true if the given object is inside this box or partially overlaps it.
  boolean overlaps (hkvAlignedBBox box);

  /// @}
  /// @name Utility Functions
  /// @{
  
  /// \brief Translates the box in world-space.
  /// \param translation The translation vector.
  void translate (hkvVec3 translation);

  /// \brief Returns the center point of the box.
  /// \return The center point of the bounding box.
  hkvVec3 getCenter ();
  
  /// \brief Returns the box's extent along the X axis.
  /// \return The x axis extent.
  number getSizeX ();
  
  /// \brief Returns the box's extent along the Y axis.
  /// \return The y axis extent.
  number getSizeY ();
  
  /// \brief Returns the box's extent along the Z axis.
  /// \return The z axis extent.
  number getSizeZ ();

  /// \brief  Returns the squared distance of a given point to this box.
  /// \note For points that are inside the box this function returns zero.
  /// \param vector The point for which the distance is computed.
  /// \return The (min) squared distance to the specified point.
  /// \see getDistanceTo
  number getDistanceToSquared (hkvVec3 vector);
  
  /// \brief Returns the squared distance of the given box to this box.
  /// \note If the two boxes overlap, zero is returned.
  /// \param box The box for which the distance is computed.
  /// \return The (min) squared distance between the two boxes.
  /// \see getDistanceTo
  number  (hkvAlignedBBox box);
  
  /// \brief Returns the distance of a given point to this box.
  /// \note For points that are inside the box this function returns zero.
  /// \param vector The point for which the distance is computed.
  /// \return The (min) distance to the specified point.
  /// \see getDistanceToSquared
  number getDistanceTo (hkvVec3 vector);

  /// \brief  Returns the distance of the given box to this box.
  /// \note If the two boxes overlap, zero is returned.
  /// \param box The box for which the distance is computed.
  /// \return The (min) distance between the two boxes.
  /// \see getDistanceToSquared
  number getDistanceTo (hkvAlignedBBox box);
 
  /// \brief Returns a vrctor, clamped to the volume of the box.
  /// \note If the vector is already inside the box, an identical copy will be returned.
  ///   If the vector is outside the box, the components of the returned copy will be
  ///   clamped to the min and max extents of the box.
  /// \param vector The point which shall be clamped to the box's volume.
  /// \return The clamped vector.
  hkvVec3 getClampedPosition (hkvVec3 vector);
  
  /// \brief Clone the current bounding box.
  /// \return The new duplicate.
  /// \note Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy it's content by assignment!
  /// \par Example
  ///   \code
  ///     -- create a new color:
  ///     local bbox1 = Vision.hkvAlignedBBox(-1,-1,-1, 1,1,1)
  ///
  ///     -- this will not copy the content (just another pointer to it):
  ///     local bbox2 = bbox1
  ///
  ///     -- this will affect access to 'bbox2' and 'bbox1' since the pointers share the same object:
  ///     bbox2:set(-2,-2,-2, 2,2,2)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bbox1==bbox2, "When this assertions fails 'bbox1' and 'bbox2' are pointing to different bounding boxes.")
  ///
  ///     -- in order to copy the content, you have to clone:
  ///     local bbox3 = bbox1:clone()
  ///     bbox3:set(-3,-3,-3, 3,3,3)
  ///
  ///     -- this assertion will not fail
  ///     Vision.Assert(bbox1~=bbox3, "When this assertions fails 'bbox1' and 'bbox3' are pointing to the same bounding box.")
  ///
  ///   \endcode
  hkvAlignedBBox clone();
  
  /// @}
  /// @name Operators
  /// @{
  
  /// \brief Comparison operator. Checks whether two boxes are identical (without epsilon). 
  /// \param other The other bounding box.
  /// \return true = bounding boxes are equal; false bounding boxes are not equal.
  boolean __eq (hkvAlignedBBox other);

  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief Minimum point of the bounding box.
  hkvVec3 m_vMin;
  
  /// \brief Maximum point of the bounding box.
  hkvVec3 m_vMax;
  
  /// @}
  
};

#endif