#ifndef VLUA_APIDOC

%nodefaultctor VisObjectKey_cl;
%nodefaultdtor VisObjectKey_cl;

class VisObjectKey_cl
{
public:
  %rename(SetKey) SetObjectKey(const char *pszChar);
  void SetObjectKey(const char *pszChar);
  
  %rename(GetKey) GetObjectKey() const;
  const char * GetObjectKey() const;
  
  %extend {
    bool HasKey(const char *szKey, bool bIgnoreCase=true)
    {
      return self->HasObjectKey(szKey, bIgnoreCase)==TRUE;
    }
	}
};

%nodefaultctor VisObject3D_cl;
%nodefaultdtor VisObject3D_cl;

class VisObject3D_cl : public VisTypedEngineObject_cl, public VisObjectKey_cl
{
public:

  void SetPosition( const hkvVec3& vPos );
  void IncPosition( const hkvVec3& vPos );
  
  %extend{
    hkvVec3 GetPosition()
    {
      return hkvVec3(self->GetPosition()); // return by value
    }
    
    void SetPosition(float x, float y, float z)
    {
      self->SetPosition(hkvVec3(x,y,z));
    }
    
    void IncPosition(float x, float y, float z)
    {
      self->IncPosition(hkvVec3(x,y,z));
    }
    
    void SetOrientation(float x, float y, float z)
    {
      self->SetOrientation(hkvVec3(x,y,z));
    }
    
    void IncOrientation(float x, float y, float z)
    {
      self->IncOrientation(hkvVec3(x,y,z));
    }
    
    VSWIG_CONVERT_BOOL_SETTER(SetUseEulerAngles);
    VSWIG_CONVERT_BOOL_GETTER_CONST(GetUseEulerAngles);
  }
  
  hkvVec3 TransformToObjectSpace(const hkvVec3& vWorldPos) const;

  void SetOrientation( const hkvVec3& vYPR );
  void IncOrientation( const hkvVec3& vYPR );
  %rename (GetOrientation) GetActualOrientation() const;
  hkvVec3 GetActualOrientation() const;
  
  void SetRotationMatrix(const hkvMat3 &matrix); 
  hkvMat3 GetRotationMatrix() const;
  hkvMat3 GetTransposedRotationMatrix() const;

  hkvVec3 GetDirection() const;
  void SetDirection(const hkvVec3& dir);

  void AttachToParent( VisObject3D_cl *pParent );
  void DetachFromParent();
  int GetNumChildren();

  void SetLocalPosition(const hkvVec3& pos);
  hkvVec3 GetLocalPosition() const;

  void SetLocalOrientation(const hkvVec3& vYPR);  
  hkvVec3 GetLocalOrientation() const;

  void ResetLocalTransformation();

  void SetMotionDeltaWorldSpace(const hkvVec3& vMotionDelta );
  void SetMotionDeltaLocalSpace(const hkvVec3& vMotionDelta );
  void IncMotionDeltaWorldSpace(const hkvVec3& vMotionDelta );
  void IncMotionDeltaLocalSpace(const hkvVec3& vMotionDelta );
  
  %extend{

    VSWIG_RETURN_BY_VALUE_CONST(hkvVec3, GetMotionDeltaWorldSpace);
    VSWIG_RETURN_BY_VALUE_CONST(hkvVec3, GetMotionDeltaLocalSpace);

    VSWIG_CONVERT_BOOL_GETTER_CONST(HasRotationDelta);
    
    VSWIG_RETURN_BY_VALUE_CONST(hkvVec3, GetRotationDelta);
  }

  void ResetMotionDeltaLocalSpace();
  void ResetMotionDeltaWorldSpace();
  virtual void ResetMotionDelta();
  
  void SetRotationDelta(const hkvVec3& vRotationDelta );
  void IncRotationDelta(const hkvVec3& vRotationDelta );

  void ResetRotationDelta();

  hkvVec3 GetObjDir() const;
  hkvVec3 GetObjDir_Right() const;
  hkvVec3 GetObjDir_Up() const;

};

//Implement this method native in order to return the concrete type instance
//instead of the base class VisObject3D_cl
%native(VisObject3D_cl_GetParent) int VisObject3D_cl_GetParent(lua_State *L);
%{
  SWIGINTERN int VisObject3D_cl_GetParent(lua_State *L)
  {
    IS_MEMBER_OF(VisObject3D_cl) //this will move this function to the method table of the specified class

    SWIG_CONVERT_POINTER(L, 1, VisObject3D_cl, pSelf)

    lua_settop(L, 0);
    LUA_PushObjectProxy(L, pSelf->GetParent()); //will handle NULL as well

    return 1;
  }
%} 

//Implement this method native in order to return the concrete type instance
//instead of the base class VisObject3D_cl
%native(VisObject3D_cl_GetChild) int VisObject3D_cl_GetChild(lua_State *L);
%{
  SWIGINTERN int VisObject3D_cl_GetChild(lua_State *L)
  {
    IS_MEMBER_OF(VisObject3D_cl) //this will move this function to the method table of the specified class  
    SWIG_CONVERT_POINTER(L, 1, VisObject3D_cl, pSelf)
    
    if(lua_isnumber(L, 2))  
    {
      int iIndex = (int)lua_tonumber(L, 2);
      
      const int iCount = pSelf->GetNumChildren();
      
      if(iIndex<iCount)
      {
        LUA_PushObjectProxy(L, (VisObject3D_cl *)pSelf->GetChild(iIndex)); //will handle NULL as well
        return 1;
      }
    }
    else if(lua_isstring(L, 2))
    {
      const char *  pszName = lua_tostring(L, 2);

      const int iCount = pSelf->GetNumChildren();
      for (int i=0;i<iCount;i++)
      {
        VisObject3D_cl *pChild = (VisObject3D_cl *)pSelf->GetChild(i);
        if (pChild && pChild->HasObjectKey(pszName))
        {
          LUA_PushObjectProxy(L, pChild);
          return 1;
        }
      }
    }
    
    //parameters invalid
    lua_pushnil(L);
  
    return 1;
  }
%} 

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisObject3D_cl, 128, "[%1.2f,%1.2f,%1.2f]", self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)
VSWIG_CREATE_TOSTRING(VisObject3D_cl, "%s: '%s' [%1.2f,%1.2f,%1.2f]", self->GetClassTypeId()->m_lpszClassName, self->GetObjectKey(), self->GetPosition().x, self->GetPosition().y, self->GetPosition().z)

#else

/// \brief Wrapper class fot objects having keys (in order to search for the object).
class VisObjectKey_cl
{
public:

  /// \brief Set an object key.
  /// \param key The new key for this object.
  void SetKey(string key);
  
  /// \brief Get the objects key.
  /// \return The object key or nil if not set.
  string GetKey();
  
  /// \brief Check whether this object has a specific key or not.
  /// \param key The object key to check.
  /// \param ignoreCase (\b optional) Set to true for case insensitive object key comparison, otherwise to false.
  /// \return true if the specified object key matches, otherwise false.
  boolean HasKey(string key, boolean ignoreCase=true);
};

/// \brief Base class for all objects in 3D space.
class VisObject3D_cl : public VisTypedEngineObject_cl, public VisObjectKey_cl {
public:

  /// @}
  /// @name Common Object 3D Functions
  /// @{
  
  /// \brief Gets the position of the object in world space.
  /// \return The position in world space.
  hkvVec3 GetPosition();

  /// \brief Sets the position of the object in world space.
  /// \param pos The new position in world space.
  void SetPosition(hkvVec3 pos);
  
  /// \brief Sets the position of the object in world space.
  /// \param x X word space position.
  /// \param y Y word space position.
  /// \param z Z word space position.
  void SetPosition(number x, number y, number z);

  /// \brief Increases the position of the object in world space.
  /// \param pos Position delta in world space.
  void IncPosition(hkvVec3 pos);
  
  /// \brief Increases the position of the object in world space.
  /// \param x Unit increment along the x axis.
  /// \param y Unit increment along the y axis.
  /// \param z Unit increment along the z axis.
  void IncPosition(number x, number y, number z);

  /// \brief Gets the orientation of the object in world space.
  /// \return The Euler orientation in world space.
  hkvVec3 GetOrientation();

  /// \brief Sets the orientation of the object in world space - \b only \b valid \b when \b using \b SetUseEulerAngles(true) \b !
  /// \param ori The new Euler orientation in world space.
  /// \see SetUseEulerAngles
  void SetOrientation(hkvVec3 ori);
  
  /// \brief Sets the orientation of the object in world space - \b only \b valid \b when \b using \b SetUseEulerAngles(true) \b !
  /// \param yaw Yaw orientation value.
  /// \param pitch Pitch orientation value.
  /// \param roll Roll orientation value.
  /// \see SetUseEulerAngles
  void SetOrientation(number yaw, number pitch, number roll);

  /// \brief Increases the orientation of the object in world space - \b only \b valid \b when \b using \b SetUseEulerAngles(true) \b !
  /// \param ori Euler Orientation delta in world space.
  /// \see SetUseEulerAngles
  void IncOrientation(hkvVec3 ori);
  
  /// \brief Increases the orientation of the object in world space - \b only \b valid \b when \b using \b SetUseEulerAngles(true) \b !
  /// \param yaw Yaw increment value.
  /// \param pitch Pitch increment value.
  /// \param roll Roll increment value.
  /// \see SetUseEulerAngles
  void IncOrientation(number yaw, number pitch, number roll);

  /// \brief Gets the direction of the X axis. 
  /// \return A vector representing the X-axis of the orientation matrix .
  /// \see SetDirection
  hkvVec3 GetDirection();

  /// \brief Sets the direction into which the object faces.
  /// \param dir The new forward directional vector of the object.
  /// \par Example
  ///    \code
  /// function OnThink(self)
  ///   -- Get other entity
  ///   local dragon = Game:GetEntity("dragon")
  /// 
  ///   -- Calculate look-at direction
  ///   local dragonPos = dragon:GetPosition() 
  ///   local myPos = self:GetPosition()
  ///   local viewDir = dragonPos - myPos
  /// 
  ///   -- Set look-at direction on object
  ///   self:SetDirection(viewDir)
  /// end
  ///    \endcode
  /// \see GetDirection
  void SetDirection(hkvVec3 dir);

  /// \brief Gets the forward directional vector of the object.
  /// \return The forward directional vector of the object.
  hkvVec3 GetObjDir();

  /// \brief Gets the rightward directional vector of the object.
  /// \return The rightward directional vector of the object.
  hkvVec3 GetObjDir_Right();

  /// \brief Gets the upward directional vector of the object.
  /// \return The upward directional vector of the object.
  hkvVec3 GetObjDir_Up();
    
  /// @}
  /// @name Euler Angles / Transformations
  /// @{
  
  /// \brief Sets euler resp. matrix orientation mode. 
  /// \param useEulerAngles Use true if Euler angles should be used, false if the matrices should be used. By default the status is true. 
  /// \details
  ///   In Euler angle mode (which is default) the orientation of an entity is specified by the three Euler angles
  ///   (yaw, pitch and roll) via SetOrientation/IncOrientation. When passing a matrix via SetRotationMatrix the
  ///   matrix is converted to euler angles using VisMath_cl::MatrixToEuler.
  ///   Alternatively, in matrix mode, any matrix passed to SetRotationMatrix will be copied directly. Any operations
  ///   with euler angles (SetOrientation, IncOrientation, ...) will be ignored until Euler mode is enabled again.
  /// \note
  ///   Also rotation delta from the animation is ignored when the object is in matrix mode.
  ///   Typically the Euler mode is preferable, but for implementing external physics the matrix mode might be more
  ///   suitable, because flipping caused by non-unique MatrixToEuler conversion is avoided.
  ///   The status of an object can be changed at any time.
  void SetUseEulerAngles(boolean useEulerAngles);
  
  /// \brief Gets the current status of using Euler angles or rotation matrix.
  /// \return true if Euler angles are used, false if the rotation matrix is used.
  boolean GetUseEulerAngles();
  
  /// \brief Sets the world space rotation matrix of the object. 
  /// \param matrix The new rotation matrix of the object.
  /// \details
  ///   When the object is in Euler angle mode (SetUseEulerAngles), the matrix will be converted
  ///   to Euler angles via MatrixToEuler. This conversion is unfortunately not unique,
  ///   so flipping of orientation might occur for even small changes in the matrix! not change the
  ///   actual orientation until Euler angle mode is enabled again
  /// \note
  ///   The virtual OnObject3DChanged function gets called when the orientation has been updated.
  ///   Override this function to keep track of orientation modifications.
  void SetRotationMatrix(hkvMat3 matrix); 
  
  /// \brief Gets the world space rotation matrix of the object in euler angles (via by reference value). 
  /// \return The current orientation of the object as hkvMat3.
  /// \details
  ///   When the object is in Euler angle mode (SetUseEulerAngles), the matrix will be created from the
  ///   Euler angles (this conversion is mathematically unique).
  hkvMat3 GetRotationMatrix();
  
  /// \brief Returns the transposed of the object's world space rotation matrix.
  /// It uses the hkvMat3::Transpose function to tranpose the matrix.
  /// \return Transposed version of th current rotation matrix.
  hkvMat3 GetTransposedRotationMatrix();
  
  /// \brief Helper function to transform a position into this object's local space. 
  /// \param worldPos Source position to transform.
  /// \return The vector in object space.
  hkvVec3 TransformToObjectSpace(hkvVec3 worldPos);
  
  /// @}
  /// @name Object Hierarchy
  /// @{

  /// \brief Attaches the object to another object.
  /// \details
  ///   Attaches the object to the specified parent, keeping the current world space position and orientation. The local space position/orientation gets initially computed from the current world space position/orientation.
  ///   If the object is already attached to an object this function will detach the object at first (using \e DetachFromParent). 
  ///   Attached objects keep their transformation internally in local space and update their world space transformation whenever the local transformation or the transformation of the parent object changes.
  ///   Attached objects bypass the physics module when modifying the position and orientation. Using parenting on physically active objects will thus lead to unpredictable results.
  /// \param parent The parent object.
  /// \par Example
  ///   \code
  ///     local entity = Game:GetObject("warrior")
  ///     local light = Game:GetObject("lamp")
  ///     light:AttachToParent(entity)
  ///   \endcode
  void AttachToParent(VisObject3D_cl parent);

  /// \brief Detaches the object from its current parent.
  /// \details
  ///    Detaches the object from its current parent. The function call is ignored if the object is not attached to any parent.
  void DetachFromParent();

  /// \brief Gets the parent of the object.
  /// \return The current parent, or nil if it has no parent.
  mixed GetParent();

  /// \brief Returns the number of children attached to an object. Can be used in combination with GetChild
  /// \return The number of children.
  /// \see GetChild
  number GetNumChildren();

  /// \brief Access a child via the index in the children list, where n must be in range 0..GetNumChildren()-1 or via the object key
  /// \param child The index to acces children table or the object key as string.
  /// \return The child object, can be nil.
  mixed GetChild(mixed child);

  /// \brief Gets the position of the object in local space. Ignored when this object has no parent.
  /// \return The position in local space
  hkvVec3 GetLocalPosition();

  /// \brief Sets the position of the object in local space. Ignored when this object has no parent.
  /// \param pos The new position in local space.
  void SetLocalPosition(hkvVec3 pos);

  /// \brief Gets the orientation of the object in local space. Ignored when this object has no parent.
  /// \return The Euler orientation in local space.
  hkvVec3 GetLocalOrientation();

  /// \brief Sets the orientation of the object in local space. Ignored when this object has no parent.
  /// \param ori The new Euler orientation in local space.
  void SetLocalOrientation(hkvVec3 ori);

  /// \brief Resets the local space position and orientation.
  void ResetLocalTransformation();

  ///
  /// @name Motion Delta
  /// @{
  ///

  /// \brief
  ///   Sets the world space motion delta of the entity.
  void SetMotionDeltaWorldSpace(hkvVec3 vMotionDelta);

  /// \brief
  ///   Sets the local space motion delta of the entity.
  void SetMotionDeltaLocalSpace(hkvVec3 vMotionDelta ); 

  /// \brief
  ///   Increases the world space motion delta of the entity.
  void IncMotionDeltaWorldSpace(hkvVec3 vMotionDelta );

  /// \brief
  ///   Increases the local space motion delta of the entity.
  void IncMotionDeltaLocalSpace(hkvVec3 vMotionDelta ); 

  /// \brief
  ///   Returns the world space motion delta of the object.
  hkvVec3 GetMotionDeltaWorldSpace() const;
  
  /// \brief
  ///   Returns the local space motion delta of the object.
  hkvVec3 GetMotionDeltaLocalSpace();

  /// \brief
  ///   Clears the world space motion delta of the object.
  void ResetMotionDeltaWorldSpace();

  /// \brief
  ///   Clears the local space motion delta of the object.
  void ResetMotionDeltaLocalSpace();

  /// \brief
  ///   Clears both local and world space motion delta of the object.
  virtual void ResetMotionDelta();

  /// @}
  /// @name Rotation Delta
  /// @{

  /// \brief Sets the rotation delta of the object. This function should be used when the entity has a physics object.
  /// \param delta The new rotation delta.
  void SetRotationDelta(hkvVec3 delta);

  /// \brief Increases the rotation delta of the object. This function should be used when the entity has a physics object.
  /// \param delta Rotation delta delta.
  void IncRotationDelta(hkvVec3 delta);

  /// \brief Gets the rotation delta of the object. This function should be used when the entity has a physics object.
  /// \return The rotation delta
  hkvVec3 GetRotationDelta();

  /// \brief Clears the rotation delta of the object.
  /// \return Nothing
  void ResetRotationDelta();

  /// \brief Checks whether the object has a rotation delta.
  /// \return true if the object has a rotation delta; otherwise false.
  boolean HasRotationDelta();
  
  /// @}
};

#endif
