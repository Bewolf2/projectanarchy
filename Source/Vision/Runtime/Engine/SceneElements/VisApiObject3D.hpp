/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiObject3D.hpp

#ifndef FR_DEFINE_VISAPIOBJECT3D
#define FR_DEFINE_VISAPIOBJECT3D

#include <Vision/Runtime/Engine/System/VisApiBase.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemParent.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>
#include <Vision/Runtime/Engine/System/VisApiScripting.hpp>

class VisObject3D_cl;

#include <Vision/Runtime/Engine/Visibility/VisApiObject3DVis.hpp>


/// \brief
///   Helper class to provide derived classes with searchable object key. For instance
///   VisObject3D_cl class
class VisObjectKey_cl
{
public:

  ///
  /// @name Object Key
  /// @{
  ///

  /// \brief
  ///   Sets the key string of the object
  /// 
  /// The key is an arbitrary string that can be set on the object
  /// 
  /// Script and game code can use the key for searching
  /// 
  /// \param pszChar
  ///   The new key string or NULL
  /// 
  /// \sa VisObject3D_cl::GetObjectKey
  inline void SetObjectKey(const char *pszChar)
  {
    m_sObjectKey = pszChar;
  }

  /// \brief
  ///   Returns the key string of the object or NULL
  /// 
  /// The key is an arbitrary string that can be set on the object
  /// 
  /// Script and game code can use the key for searching
  /// 
  /// \return
  ///   The key string or NULL
  /// 
  /// \sa VisObject3D_cl::SetObjectKey
  inline const char *GetObjectKey() const
  {
    return m_sObjectKey.AsChar();
  }

  /// \brief
  ///   Returns the key string of the object
  /// 
  /// Will return an empty string instead of NULL
  /// 
  /// \return
  ///   The key string or an empty string.
  /// 
  /// \sa VisObject3D_cl::SetObjectKey
  /// \sa VisObject3D_cl::GetObjectKey
  inline const char *GetObjectKeySafe() const
  {
    return m_sObjectKey.AsChar();
  }

  /// \brief
  ///   Compares the passed key with the object's key. Handles all combinations (with NULL
  ///   pointers) gracefully.
  VISION_APIFUNC BOOL HasObjectKey(const char *szKey, BOOL bIgnoreCase=TRUE) const;

  /// \brief
  ///   Saves or retrieves object key from archive. Not called Serialize/SerializeX to avoid name
  ///   collisions
  inline void SerializeKey(VArchive &ar)
  {
    if (ar.IsLoading())
      ar >> m_sObjectKey;
    else
      ar << m_sObjectKey;
  }

protected:
  VString m_sObjectKey;                       ///< Object key. String value used when searching for specific objects from code or script
  ///
  /// @}
  ///

};


/// \brief
///   Data object that is sent by the static VisObject3D_cl::OnObject3DCreated resp.
///   VisObject3D_cl::OnObject3DDestroyed callback. It has a m_pObject3D member pointer. 
class VisObject3DDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  inline VisObject3DDataObject_cl(VCallback *pSender, VisObject3D_cl *pObj) : IVisCallbackDataObject_cl(pSender)
  {
    m_pObject3D = pObj;
  }
  VisObject3D_cl *m_pObject3D;
};


/// \brief
///   Common base class for 3D objects
/// 
/// The VisObject3D_cl class serves as base class for 3D engine objects such as entities,
/// lightsources, particle groups and paths. It stores information about the position, orientation
/// and motion delta of the object and provides functions for attaching the object to other
/// VisObject3D_cl instances.
/// 
/// Use VisObject3D_cl as a lightweight base class for your custom objects in case VisBaseEntity_cl
/// is not appropriate. Using the VisObject3D_cl class simplifies the vForge integration and
/// parenting support of your custom objects.
/// 
/// Please note that the engine is not responsible for processing motion delta of  VisObject3D_cl
/// instances. In case of entities the vHavokPhysicsModule takes care of this as long as a
/// physics object is available for the instance. When using a different physics system or other
/// VisObject3D_cl derived classes your game code has to take care of processing the motion delta.
class VisObject3D_cl : public VisTypedEngineObject_cl, public VUserDataObj, public VisObjectKey_cl, public VisModuleSystemParent_cl, public VisModuleSystemChild_cl
{
friend class VisModuleSystemParent_cl;
public:

  /// \brief
  ///   Enumerations of flags indicating the current state and modifications applied to the object
  enum VISOBJECT3D_FLAGS
  {
    VIS_OBJECT3D_NONE = 0,                    ///< no flag set 
    VIS_OBJECT3D_POSCHANGED = 1,              ///< the position of the object changed
    VIS_OBJECT3D_ORICHANGED = 2,              ///< the orientation of the object changed
    VIS_OBJECT3D_SCALINGCHANGED = 16,         ///< the scaling changed (in case supported by the class)

    // just a bit combination of all XYZchanged flags; do not touch other flags
    VIS_OBJECT3D_ALLCHANGED = (VIS_OBJECT3D_POSCHANGED|VIS_OBJECT3D_ORICHANGED|VIS_OBJECT3D_SCALINGCHANGED),
    
    VIS_OBJECT3D_USEEULERANGLES = 32,              ///< object is in Euler angles mode
    VIS_OBJECT3D_NO_VISIBILITY_SERIALIZATION = 64, ///< visibility assignment state should not be serialized
    VIS_OBJECT3D_REPOSITION = 128,                 ///< This Object3D Changed event was triggered due to a zone reposition

    // Bit combination of the flags that should be cleared after each change event
    VIS_OBJECT3D_CLEARCHANGED = (VIS_OBJECT3D_ALLCHANGED|VIS_OBJECT3D_REPOSITION)
  };
  
  /// \brief
  ///   Default constructor
  VISION_APIFUNC VisObject3D_cl();

  /// \brief
  ///   Default destructor
  VISION_APIFUNC virtual ~VisObject3D_cl();


  ///
  /// @name Position
  /// @{
  ///


  /// \brief
  ///   Sets the position of the object in world space.
  ///
  /// Important: If the object needs to be able to move "through walls" or if it is teleported from one location in a level to another over a large distance,
  /// use the VisObject3D_cl::ReComputeVisibility method after the setposition call to ensure that the visibility zone assignment is recomputed.
  /// If you don't do that, it might be possible that the object is not visible anymore. You can find further information on this topic in the Vision Documentation at Programmer's Documentation->Rendering->Visibility Determination
  ///
  /// It bypasses the physics module
  /// and will thus lead to unpredictable results when using it on physically active objects.
  /// 
  /// Recomputes the local space position of the object in case it is currently attached to another
  /// object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the position has been updated.
  /// Override this function to keep track of position modifications.
  /// 
  /// \param vPos
  ///   new world space position of the object
  /// 
  /// \sa VisObject3D_cl::IncPosition
  /// \sa VisObject3D_cl::GetPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void SetPosition( const hkvVec3& vPos );


  /// \brief
  ///   Increases the position of the object in world space.
  /// 
  /// It bypasses the physics
  /// module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// Recomputes the local space position of the object in case it is currently attached to another
  /// object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the position has been updated.
  /// Override this function to keep track of position modifications.
  /// 
  /// \param vPos
  ///   position delta in world space coordinates
  /// 
  /// \sa VisObject3D_cl::SetPosition
  /// \sa VisObject3D_cl::GetPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void IncPosition( const hkvVec3& vPos );


  /// \brief
  ///   Gets the position of the object in world space (via by reference value).
  /// 
  /// \param vPos
  ///   the current position of the object gets stored here
  /// 
  /// \sa VisObject3D_cl::SetPosition
  /// \sa VisObject3D_cl::IncPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void GetPosition( hkvVec3& vPos) const;


  /// \brief
  ///   Gets the position of the object in world space.
  /// 
  /// \return
  ///   hkvVec3: current position of the object
  /// 
  /// \sa VisObject3D_cl::SetPosition
  /// \sa VisObject3D_cl::IncPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC const hkvVec3& GetPosition() const;


  /// \brief
  ///   Sets the position of the object in world space (via three float values).
  /// 
  /// Important: If the object needs to be able to move "through walls" or if it is teleported from one location in a level to another over a large distance,
  /// use the VisObject3D_cl::ReComputeVisibility method after the setposition call to ensure that the visibility zone assignment is recomputed.
  /// If you don't do that, it might be possible that the object is not visible anymore. You can find further information on this topic in the Vision Documentation at Programmer's Documentation->Rendering->Visibility Determination
  ///
  /// It bypasses the physics module
  /// and will thus lead to unpredictable results when using it on physically active objects.
  /// 
  /// Recomputes the local space position of the object in case it is currently attached to another
  /// object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the position has been updated.
  /// Override this function to keep track of position modifications.
  /// 
  /// \param x
  ///   new position (x component)
  /// 
  /// \param y
  ///   new position (y component)
  /// 
  /// \param z
  ///   new position (z component)
  /// 
  /// \sa VisObject3D_cl::IncPosition
  /// \sa VisObject3D_cl::GetPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  HKV_FORCE_INLINE void SetPosition( float x, float y, float z ) { SetPosition (hkvVec3 (x, y ,z)); }


  /// \brief
  ///   Increases the position of the object in world space (via three float values).
  /// 
  /// It bypasses the physics
  /// module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// Recomputes the local space position of the object in case it is currently attached to another
  /// object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the position has been updated.
  /// Override this function to keep track of position modifications.
  /// 
  /// \param x
  ///   position delta (x component)
  /// 
  /// \param y
  ///   position delta (y component)
  /// 
  /// \param z
  ///   position delta (z component)
  /// 
  /// \sa VisObject3D_cl::SetPosition
  /// \sa VisObject3D_cl::GetPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  HKV_FORCE_INLINE void IncPosition( float x, float y, float z ) { IncPosition (hkvVec3 (x, y, z)); }


  /// \brief
  ///   Gets the position of the object in world space (via three reference float values).
  /// 
  /// \param x
  ///   current position gets stored here (x component)
  /// 
  /// \param y
  ///   current position gets stored here (y component)
  /// 
  /// \param z
  ///   current position gets stored here (z component)
  /// 
  /// \sa VisObject3D_cl::SetPosition
  /// \sa VisObject3D_cl::IncPosition
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void GetPosition( float &x, float &y, float &z ) const;  


  /// \brief
  ///   Helper function to transform a position into this object's local space
  /// 
  /// This function uses this object's position and rotation matrix to do the transformation.
  /// 
  /// \param vWorldPos
  ///   Source position to transform
  /// 
  /// \return
  ///   hkvVec3 : Local space position
  /// 
  /// \sa VisObject3D_cl::GetPosition
  /// \sa VisObject3D_cl::GetRotationMatrix
  HKV_FORCE_INLINE hkvVec3 TransformToObjectSpace(const hkvVec3& vWorldPos) const
  {
    return GetTransposedRotationMatrix () * (vWorldPos-GetPosition());
  }



  ///
  /// @}
  ///

  ///
  /// @name Euler Angles
  /// @{
  ///

  /// \brief
  ///   Sets euler resp. matrix orientation mode.
  /// 
  /// In Euler angle mode (which is default) the orientation of an entity is specified by the three
  /// Euler angles (yaw, pitch and roll) via SetOrientation/IncOrientation. When passing a matrix
  /// via SetRotationMatrix the matrix is converted to euler angles using
  /// VisMath_cl::MatrixToEuler.
  /// 
  /// Alternatively, in matrix mode, any matrix passed to SetRotationMatrix will be copied
  /// directly. Any operations with  euler angles (SetOrientation, IncOrientation, ...) will be
  /// ignored until Euler mode is enabled again.
  /// 
  /// Note, that also rotation delta from the animation is ignored when the object is in matrix
  /// mode
  /// 
  /// Typically the Euler mode is preferable, but for implementing external physics the matrix mode
  /// might be more suitable, because flipping caused by non-unique MatrixToEuler conversion is
  /// avoided
  /// 
  /// The status of an object can be changed at any time
  /// 
  /// \param bStatus
  ///   TRUE if Euler angles should be used, FALSE if the matrices should be used. By default the
  ///   status is TRUE.
  /// 
  /// \sa VisObject3D_cl::GetUseEulerAngles
  /// \sa VisObject3D_cl::SetRotationMatrix
  VISION_APIFUNC void SetUseEulerAngles(BOOL bStatus);

  /// \brief
  ///   Gets the current status of using Euler angles or rotation matrix
  /// 
  /// \return
  ///   BOOL bStatus : TRUE if Euler angles are used, FALSE if the rotation matrix is used
  /// 
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetRotationMatrix
  VISION_APIFUNC BOOL GetUseEulerAngles() const;



  ///
  /// @}
  ///

  ///
  /// @name Orientation (yaw, Pitch, Roll)
  /// @{
  ///

  /// \brief
  ///   Sets the world space orientation of the object in euler angles.
  /// 
  /// It bypasses
  /// the physics module and will thus lead to unpredictable results when using it on physically
  /// active objects.
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not change
  /// the actual orientation until Euler angle mode is enabled again
  /// 
  /// Recomputes the local space orientation of the object in case it is currently attached to
  /// another object. However, we highly recommend to not use this method if your object is attached
  /// to another object. Use IncLocalOrientation instead to avoid problems resulting from the
  /// nature of Euler angles (ambiguous results).
  /// 
  /// The virtual OnObject3DChanged function gets called when the orientation has been updated.
  /// Override this function to keep track of orientation modifications.
  /// 
  /// \param vYPR
  ///   new orientation of the object as yaw/pitch/roll
  /// 
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void SetOrientation( const hkvVec3& vYPR );
  
  
  /// \brief
  ///   Increases the world space orientation of the object in euler angles.
  /// 
  /// It bypasses the physics module and will thus lead to unpredictable results when using it
  /// on physically active objects.
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not change
  /// the actual orientation until Euler angle mode is enabled again
  /// 
  /// Recomputes the local space orientation of the object in case it is currently attached to
  /// another object. However, we highly recommend to not use this method if your object is attached
  /// to another object. Use IncLocalOrientation instead to avoid problems resulting from the
  /// nature of Euler angles (ambiguous results).
  /// 
  /// The virtual OnObject3DChanged function gets called when the orientation has been updated.
  /// Override this function to keep track of orientation modifications.
  /// 
  /// \param vYPR
  ///   orientation delta of the object as yaw/pitch/roll
  /// 
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void IncOrientation( const hkvVec3& vYPR );
  
  
  /// \brief
  ///   Gets the world space orientation of the object in euler angles (via by reference value).
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not return
  /// the actual orientation but the Euler angles from the last call of Euler angle based
  /// functions, e.g. SetOrientation
  /// 
  /// \param vYPR
  ///   the current orientation of the object as yaw/pitch/roll gets stored here
  /// 
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void GetOrientation( hkvVec3& vYPR ) const;

  
  
  /// \brief
  ///   Gets the world space orientation of the object in euler angles.
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not return
  /// the actual orientation but the Euler angles from the last call of Euler angle based
  /// functions, e.g. SetOrientation. If you want to get the actual orientation please use GetActualOrientation
  /// 
  /// \returns
  ///   hkvVec3 vYPR: the current orientation of the object as yaw/pitch/roll gets stored here
  /// 
  /// \sa VisObject3D_cl::GetActualOrientation
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC hkvVec3 GetOrientation() const;


  /// \brief
  ///   Gets the world space orientation of the object in euler angles.
  /// 
  /// This function will return the orientation independent of the UseEulerAngles state.
  /// 
  /// \returns
  ///   hkvVec3 vYPR: the current orientation of the object as yaw/pitch/roll gets stored here
  /// 
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC hkvVec3 GetActualOrientation() const;
  
  
  /// \brief
  ///   Set the world space orientation of the object in euler angles (as three float values).
  /// 
  /// It bypasses
  /// the physics module and will thus lead to unpredictable results when using it on physically
  /// active objects.
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not change
  /// the actual orientation until Euler angle mode is enabled again
  /// 
  /// Recomputes the local space orientation of the object in case it is currently attached to
  /// another object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the orientation has been updated.
  /// Override this function to keep track of orientation modifications.
  /// 
  /// \param fY
  ///   new orientation (yaw)
  /// 
  /// \param fP
  ///   new orientation (pitch)
  /// 
  /// \param fR
  ///   new orientation (roll)
  /// 
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  HKV_FORCE_INLINE void SetOrientation( float fY, float fP, float fR ) { SetOrientation (hkvVec3 (fY, fP, fR)); }
  
  
  /// \brief
  ///   Increases the world space orientation of the object in euler angles (as three float values).
  /// 
  /// It bypasses the physics module and will thus lead to unpredictable results when using it on
  /// physically active objects.
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not change
  /// the actual orientation until Euler angle mode is enabled again
  /// 
  /// Recomputes the local space orientation of the object in case it is currently attached to
  /// another object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the orientation has been updated.
  /// Override this function to keep track of orientation modifications.
  /// 
  /// \param fY
  ///   new orientation (yaw)
  /// 
  /// \param fP
  ///   new orientation (pitch)
  /// 
  /// \param fR
  ///   new orientation (roll)
  /// 
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  HKV_FORCE_INLINE void IncOrientation( float fY, float fP, float fR ) { IncOrientation (hkvVec3 (fY, fP, fR)); }
  
  
  /// \brief
  ///   Gets the world space orientation of the object in euler angles (via reference values).
  /// 
  /// When the object is not in Euler angle mode (SetUseEulerAngles), this function will not return
  /// the actual orientation but the Euler angles from the last call of Euler angle based
  /// functions, e.g. SetOrientation
  /// 
  /// \param fY
  ///   current orientation gets stored here (yaw)
  /// 
  /// \param fP
  ///   current orientation gets stored here (pitch)
  /// 
  /// \param fR
  ///   current orientation gets stored here (roll)
  /// 
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void GetOrientation( float &fY, float &fP, float &fR ) const;
  



  ///
  /// @}
  ///

  ///
  /// @name Rotation Matrix
  /// @{
  ///


  /// \brief
  ///   Sets the world space rotation matrix of the object.
  /// 
  /// It bypasses the
  /// physics module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// When the object is in Euler angle mode (SetUseEulerAngles), the matrix will be converted to
  /// Euler angles via VisMath_cl::MatrixToEuler. This conversion is unfortunately not unique, so
  /// flipping of orientation might occur for even small changes in the matrix! not change the
  /// actual orientation until Euler angle mode is enabled again
  /// 
  /// Recomputes the local space orientation of the object in case it is currently attached to
  /// another object.
  /// 
  /// The virtual OnObject3DChanged function gets called when the orientation has been updated.
  /// Override this function to keep track of orientation modifications.
  /// 
  /// \param matrix
  ///   new rotation matrix of the object
  /// 
  /// \note
  ///   To pass a rotation matrix without conversion, the object should be switched to using the
  ///   matrix directly by SetUseEulerAngles(FALSE)
  /// 
  /// \sa VisObject3D_cl::GetRotationMatrix
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void SetRotationMatrix(const hkvMat3& matrix);
  
  
  /// \brief
  ///   Gets the world space rotation matrix of the object in euler angles (via by reference value).
  /// 
  /// When the object is in Euler angle mode (SetUseEulerAngles), the matrix will be created from
  /// the Euler angles (this conversion is mathematically unique). If Euler angle mode is disabled
  /// (via SetUseEulerAngles(FALSE)) the rotation matrix will be set to the matrix passed to
  /// SetRotationMatrix.
  /// 
  /// \param matrix
  ///   matrix reference that will receive the current orientation of the object
  /// 
  /// \sa VisObject3D_cl::SetRotationMatrix
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation  
  VISION_APIFUNC void GetRotationMatrix( hkvMat3& matrix) const;
  
  /// \brief
  ///   Internal function to ensure that the protected m_cachedRotMatrix member is up-to-date afterwards
  VISION_APIFUNC void EnsureCachedRotationMatrixValid() const;

  
  /// \brief
  ///   Gets the world space rotation matrix of the object in euler angles.
  /// 
  /// When the object is in Euler angle mode (SetUseEulerAngles), the matrix will be created from
  /// the Euler angles (this conversion is mathematically unique). If Euler angle mode is disabled
  /// (via SetUseEulerAngles(FALSE)) the rotation matrix will be set to the matrix passed to
  /// SetRotationMatrix.
  /// 
  /// \return
  ///   hkvMat3: current rotation matrix of the object
  /// 
  /// \sa VisObject3D_cl::SetRotationMatrix
  /// \sa VisObject3D_cl::SetOrientation
  /// \sa VisObject3D_cl::IncOrientation
  /// \sa VisObject3D_cl::GetOrientation
  /// \sa VisObject3D_cl::SetUseEulerAngles
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation  
  VISION_APIFUNC const hkvMat3& GetRotationMatrix() const;

  /// \brief
  ///   Returns the transpose of the object's world space rotation matrix.
  /// 
  /// It uses the hkvMat3::Transpose function to tranpose the matrix.
  /// 
  /// \return
  ///   hkvMat3: transposed version of th current rotation matrix
  VISION_APIFUNC hkvMat3 GetTransposedRotationMatrix() const;

  /// \brief
  ///   Gets the direction of the X axis
  /// 
  /// The direction corresponds to the X-axis of the matrix (m[0], m[3], m[6])
  /// 
  /// \return
  ///   hkvVec3: the X-axis of the orientation matrix
  VISION_APIFUNC hkvVec3 GetDirection() const;

  /// \brief
  ///   Sets the direction the object is facing to (composes a matrix).
  /// 
  /// The SetDirection function composes a matrix from the passed direction and sets the matrix via
  /// VisObject3D_cl::SetRotationMatrix. It uses the function Vision::Math.MatrixLookAt and the
  /// (0,0,1) up vector to create the rotation matrix.
  /// 
  /// Please note that all restrictions of SetRotationMatrix also apply to this function (bypassing
  /// the physics system, euler angle mode, ...). See VisObject3D_cl::SetRotationMatrix for more
  /// details.
  /// 
  /// \param dir
  ///   direction the object is facing to
  VISION_APIFUNC void SetDirection(const hkvVec3& dir);


  ///
  /// @}
  ///

  ///
  /// @name Parent/child System
  /// @{
  ///

  /// \brief
  ///   Attaches the object to a specified parent.
  /// 
  /// Keeps the current world space position and orientation. The local space position/orientation
  /// gets initially computed from the current world space position/orientation. You can use
  /// VisObject3D_cl::ResetLocalTransformation to reset the local transformation of the object.
  /// 
  /// If the object is already attached to an object this function will detach the object at first
  /// (using VisObject3D_cl::DetachFromParent).
  /// 
  /// Attached objects keep their transformation internally in local space and update their world
  /// space transformation whenever the local transformation or the transformation of the parent
  /// object changes.
  /// 
  /// Attached objects bypass the physics module when modifying the position and orientation. Using
  /// parenting on physically active objects will thus lead to unpredictable results.
  ///
  /// Do not modify the world space orientation of an object (SetOrientation/IncOrientation) when it is 
  /// attached to another object. Due to the nature of Euler angles this can result in problems.
  /// (ambiguous results on matrix to euler conversions). Instead modify the local space orientation 
  /// via VisObject_cl::SetLocalOrientation in that case.
  /// 
  /// \param pParent
  ///   parent object
  /// 
  /// \sa VisObject3D_cl::DetachFromParent
  /// \sa VisObject3D_cl::GetParent
  /// \sa VisObject3D_cl::ResetLocalTransformation
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void AttachToParent( VisObject3D_cl *pParent );

  /// \brief
  ///   Detaches the object from the parent.
  /// 
  /// The function call is ignored if the object is not attached to any parent.
  /// 
  /// \sa VisObject3D_cl::AttachToParent
  /// \sa VisObject3D_cl::GetParent
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void DetachFromParent();

  /// \brief
  ///   Gets the parent of the object.
  /// 
  /// \return
  ///   VisObject3D_cl *: current parent (NULL if no parent is set)
  /// 
  /// \sa VisObject3D_cl::AttachToParent
  /// \sa VisObject3D_cl::DetachFromParent
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisModuleSystemParent_c::GetChild
  VISION_APIFUNC VisObject3D_cl *GetParent() const;

  /// \brief
  ///   Sets the local space position of the object.
  /// 
  /// It also recomputes the cached world
  /// space  position and orientation of the object. It bypasses the physics module and will thus
  /// lead to unpredictable results when using it on physically active objects.
  /// 
  /// This function has no effect if the object is not attached to a parent
  /// 
  /// \param pos
  ///   new position in local space
  /// 
  /// \sa VisObject3D_cl::GetLocalPosition
  VISION_APIFUNC void SetLocalPosition(const hkvVec3& pos);
  
  /// \brief
  ///   Sets the position of the object in local space (as three float values).
  /// 
  /// It also recomputes the cached world
  /// space  position and orientation of the object. It bypasses the physics module and will thus
  /// lead to unpredictable results when using it on physically active objects.
  /// 
  /// This function has no effect if the object is not attached to a parent
  /// 
  /// \param x
  ///   new X position in local space
  /// 
  /// \param y
  ///   new Y position in local space
  /// 
  /// \param z
  ///   new Z position in local space
  /// 
  /// \sa VisObject3D_cl::GetLocalPosition
  HKV_FORCE_INLINE void SetLocalPosition(float x, float y, float z) { SetLocalPosition (hkvVec3 (x, y, z)); }

  /// \brief
  ///   Get the position of the object in local space
  /// 
  /// Gets the position of the object in local space. The returned position is  only valid if the
  /// object is attached to a parent.
  /// 
  /// \return
  ///   hkvVec3: current position in local space
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  VISION_APIFUNC const hkvVec3& GetLocalPosition() const;

  /// \brief
  ///   Sets the local space orientation of the object in yaw/pitch/roll.
  /// 
  /// It also recomputes the cached world space position and orientation of the object. It bypasses the
  /// physics module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// This function has no effect if the object is not attached to a parent
  /// 
  /// \param vYPR
  ///   new local space orientation in yaw/pitch/roll
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void SetLocalOrientation(const hkvVec3& vYPR);  

  /// \brief
  ///   Sets the local space orientation of the object in yaw/pitch/roll.
  /// 
  /// It also recomputes the cached world space position and orientation of the object. It bypasses the
  /// physics module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// This function has no effect if the object is not attached to a parent
  /// 
  /// \param y
  ///   new local space orientation in Yaw
  /// 
  /// \param p
  ///   new local space orientation in Pitch
  /// 
  /// \param r
  ///   new local space orientation in Roll
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisObject3D_cl::GetLocalOrientation
  VISION_APIFUNC void SetLocalOrientation(const float y,const float p,const float r);

  /// \brief
  ///   Gets the local space orientation of the object in yaw/pitch/roll.
  /// 
  /// The returned orientation is only valid if the object is attached to a parent.
  /// 
  /// \return
  ///   const hkvVec3&: current local space orientation in yaw/pitch/roll
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisObject3D_cl::SetLocalOrientation
  VISION_APIFUNC hkvVec3 GetLocalOrientation() const;

  /// \brief
  ///   Sets the local space orientation matrix of the object.
  /// 
  /// It also recomputes the cached world space position and orientation of the object. It bypasses the
  /// physics module and will thus lead to unpredictable results when using it on physically active
  /// objects.
  /// 
  /// This function has no effect if the object is not attached to a parent
  /// 
  /// \param matrix
  ///   new local space orientation matrix
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientationMatrix
  VISION_APIFUNC void SetLocalOrientationMatrix(const hkvMat3& matrix);  

  /// \brief
  ///   Gets the local space orientation matrix of the object.
  /// 
  /// The returned orientation is only valid if the object is attached to a parent.
  /// 
  /// \return
  ///   hkvMat3: current local space orientation matrix of the object
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  /// \sa VisObject3D_cl::GetLocalPosition
  /// \sa VisObject3D_cl::SetLocalOrientation
  /// \sa VisObject3D_cl::GetLocalOrientation
  /// \sa VisObject3D_cl::SetLocalOrientationMatrix
  VISION_APIFUNC const hkvMat3& GetLocalOrientationMatrix() const;

  /// \brief
  ///   Resets the local space position and orientation.
  /// 
  /// Sets the local space position and orientation to (0/0/0).
  /// 
  /// \sa VisObject3D_cl::SetLocalPosition
  VISION_APIFUNC void ResetLocalTransformation();

  ///
  /// @}
  ///

  ///
  /// @name Motion Delta
  /// @{
  ///

  /// \brief
  ///   Sets the world space motion delta of the entity.
  /// 
  ///  An object's motion delta describes the difference between its current position
  ///  and the position that should be reached after applying the movement desired by
  ///  the animation system or e.g. character input control.
  ///  Two motion delta values are stored for each object: One is given in world space,
  ///  the other in local space relative to the object's orientation.
  ///  
  ///  Please note that the VisionEngine does not process motion delta directly.
  ///  Instead, the current physics system implementation is responsible for applying
  ///  the motion delta to each object and reconcile it with collision detection as appropriate.
  ///  
  ///  Typically, the work performed by the physics system amounts to
  ///  
  ///  \code
  ///    pObject->IncOrientation(pObject->GetRotationDelta());
  ///    pObject->ResetRotationDelta();
  ///    
  ///    hkvVec3 vMotionDelta = pObject->GetMotionDeltaWorldSpace() + pObject->GetRotationMatrix() * pObject->GetMotionDeltaLocalSpace();
  ///    pObject->ResetMotionDelta();
  ///  \endcode
  ///  
  ///  and then physically moving the entity by the resulting vMotionDelta.
  /// 
  /// \param vMotionDelta
  ///   The world space motion delta for this object.
  /// 
  /// \sa VisObject3D_cl::IncMotionDeltaWorldSpace
  /// \sa VisObject3D_cl::GetMotionDeltaWorldSpace
  /// \sa VisObject3D_cl::ResetMotionDeltaWorldSpace
  VISION_APIFUNC void SetMotionDeltaWorldSpace(const hkvVec3& vMotionDelta );

  /// \brief
  ///   Sets the local space motion delta of the entity.
  /// 
  ///  An object's motion delta describes the difference between its current position
  ///  and the position that should be reached after applying the movement desired by
  ///  the animation system or e.g. character input control.
  ///  Two motion delta values are stored for each object: One is given in world space,
  ///  the other in local space relative to the object's orientation.
  ///  
  ///  Please note that the VisionEngine does not process motion delta directly.
  ///  Instead, the current physics system implementation is responsible for applying
  ///  the motion delta to each object and reconcile it with collision detection as appropriate.
  ///  
  ///  Typically, the work performed by the physics system amounts to
  ///  
  ///  \code
  ///    pObject->IncOrientation(pObject->GetRotationDelta());
  ///    pObject->ResetRotationDelta();
  ///    
  ///    hkvVec3 vMotionDelta = pObject->GetMotionDeltaWorldSpace() + pObject->GetRotationMatrix() * pObject->GetMotionDeltaLocalSpace();
  ///    pObject->ResetMotionDelta();
  ///  \endcode
  ///  
  ///  and then physically moving the entity by the resulting vMotionDelta.
  /// 
  /// \param vMotionDelta
  ///   The local space motion delta for this object.
  /// 
  /// \note
  ///   As the local space motion delta is modified by the animation system, resetting or overwriting
  ///   the local space motion delta in user code will override the movement specified by the animation.
  ///   This should be taken into account if an entity is to be controlled both by the animation system and custom code.
  ///   You can remove the APPLY_MOTION_DELTA flag from the animation configuration to prevent the animation
  ///   system from setting the local space motion delta.
  ///
  ///   Local space motion delta is accumulated as if specified in the entity's orientation frame
  ///   at the time of each physics step and NOT in the orientation frame defined by the currently
  ///   accumulated rotation delta. This means that the following code, when executed in a single
  ///   frame without intermediate physics steps,
  ///   
  ///   \code
  ///     #define STEPS 100
  ///     for(int i = 0; i < STEPS; i++)
  ///     {
  ///       pObject->IncRotationDelta(hkvVec3(90.0f / STEPS, 0.0f, 0.0f));
  ///       pObject->IncMotionDeltaLocalSpace(hkvVec3(300.f / STEPS, 0.0f, 0.0f));
  ///     }
  ///   \endcode
  ///     
  ///   will not result in pObject moving to the end of a quarter-circle arc of length 300,
  ///   but rather have an effect equivalent to turning by 90 degrees and then moving forward by 300 units.
  ///   If a different behavior is desired, game code should transform the motion delta to world space and
  ///   apply it using the *MotionDeltaWorldSpace methods.
  /// 
  /// \sa VisObject3D_cl::IncMotionDeltaLocalSpace
  /// \sa VisObject3D_cl::GetMotionDeltaLocalSpace
  /// \sa VisObject3D_cl::ResetMotionDeltaLocalSpace
  VISION_APIFUNC void SetMotionDeltaLocalSpace(const hkvVec3& vMotionDelta ); 

  /// \brief
  ///   Increases the world space motion delta of the entity.
  /// 
  ///  See the documentation for SetMotionDeltaWorldSpace for more information.
  /// 
  /// \param vMotionDelta
  ///   The world space motion delta to add to this object's current world space motion delta.
  /// 
  /// \sa VisObject3D_cl::SetMotionDeltaWorldSpace
  VISION_APIFUNC void IncMotionDeltaWorldSpace( const hkvVec3& vMotionDelta );

  /// \brief
  ///   Increases the local space motion delta of the entity.
  /// 
  ///  See the documentation for SetMotionDeltaLocalSpace for more information.
  /// 
  /// \param vMotionDelta
  ///   The local space motion delta to add to this object's current local space motion delta.
  /// 
  /// \sa VisObject3D_cl::SetMotionDeltaLocalSpace
  VISION_APIFUNC void IncMotionDeltaLocalSpace( const hkvVec3& vMotionDelta ); 

  /// \brief
  ///   Returns the world space motion delta of the object.
  /// 
  ///  See the documentation for SetMotionDeltaWorldSpace for more information.
  ///
  /// \sa VisObject3D_cl::SetMotionDeltaWorldSpace
  VISION_APIFUNC const hkvVec3& GetMotionDeltaWorldSpace() const;
  
  /// \brief
  ///   Returns the local space motion delta of the object.
  /// 
  ///  See the documentation for SetMotionDeltaLocalSpace for more information.
  ///
  /// \sa VisObject3D_cl::SetMotionDeltaLocalSpace
  VISION_APIFUNC const hkvVec3& GetMotionDeltaLocalSpace() const;

  /// \brief
  ///   Clears the world space motion delta of the object.
  /// 
  ///  This method sets the world space motion delta to zero. The virtual ResetMotionDelta
  ///  method will handle additional tasks, such as clearing the accumulated motion delta
  ///  of an entity's animation.
  ///
  ///  See the documentation for SetMotionDeltaWorldSpace for more information.
  ///
  /// \sa VisObject3D_cl::SetMotionDeltaWorldSpace
  /// \sa VisObject3D_cl::ResetMotionDelta
  VISION_APIFUNC void ResetMotionDeltaWorldSpace();

  /// \brief
  ///   Clears the local space motion delta of the object.
  /// 
  ///  This method sets the local space motion delta to zero. The virtual ResetMotionDelta
  ///  method will handle additional tasks, such as clearing the accumulated motion delta
  ///  of an entity's animation.
  ///
  ///  See the documentation for SetMotionDeltaLocalSpace for more information.
  ///
  /// \sa VisObject3D_cl::SetMotionDeltaLocalSpace
  /// \sa VisObject3D_cl::ResetMotionDelta
  VISION_APIFUNC void ResetMotionDeltaLocalSpace();

  /// \brief
  ///   Clears both local and world space motion delta of the object.
  ///
  ///  This function is virtual. Derived classes may provide their own implementations.
  ///
  /// \sa VisBaseEntity_cl::ResetMotionDelta
  VISION_APIFUNC virtual void ResetMotionDelta();
  
  ///
  /// @}
  ///

  ///
  /// @name Rotation Delta
  /// @{
  ///

  /// \brief
  ///   Sets the rotation delta of the object.
  /// 
  /// The components of the vector specify the yaw, pitch
  /// and roll angle (in degrees) respectively.
  /// 
  /// \param vRotationDelta
  ///   new rotation delta
  /// 
  /// \note
  ///   The animation system takes care of applying rotation delta to animated objects. You can
  ///   remove the APPLY_MOTION_DELTA flag from the animation configuration to disable rotation
  ///   delta when playing animations.
  /// 
  /// \note
  ///   Please note that the engine is not responsible for processing rotation delta of
  ///   VisObject3D_cl instances. In case of entities the vHavokPhysicsModule takes care of this
  ///   as long as a physics object is available for the instance. When using a different physics
  ///   system or other VisObject3D_cl derived classes your game code has to take care of
  ///   processing the rotation delta. 
  /// 
  /// \sa VisObject3D_cl::GetRotationDelta
  /// \sa VisObject3D_cl::IncRotationDelta
  /// \sa VisObject3D_cl::ResetRotationDelta
  /// \sa VisObject3D_cl::HasRotationDelta
  /// \sa VisObject3D_cl::SetMotionDelta
  VISION_APIFUNC void SetRotationDelta( const hkvVec3& vRotationDelta );

  /// \brief
  ///   Increases the rotation delta of the object
  /// 
  /// in the current frame. The components of the vector specify the yaw, pitch and roll angle (in
  /// degrees) respectively.
  /// 
  /// \param vRotationDelta
  ///   rotation delta to be added component wise
  /// 
  /// \note
  ///   The animation system takes care of applying rotation delta to animated objects. You can
  ///   remove the APPLY_MOTION_DELTA flag from the animation configuration to disable rotation
  ///   delta when playing animations.
  /// 
  /// \note
  ///   Please note that the engine is not responsible for processing rotation delta of
  ///   VisObject3D_cl instances. In case of entities the vHavokPhysicsModule takes care of this
  ///   as long as a physics object is available for the instance. When using a different physics
  ///   system or other VisObject3D_cl derived classes your game code has to take care of
  ///   processing the rotation delta. 
  /// 
  /// \sa VisObject3D_cl::SetRotationDelta
  /// \sa VisObject3D_cl::GetRotationDelta
  /// \sa VisObject3D_cl::ResetRotationDelta
  /// \sa VisObject3D_cl::HasRotationDelta
  /// \sa VisObject3D_cl::SetMotionDelta
  VISION_APIFUNC void IncRotationDelta(const hkvVec3& vRotationDelta );

  /// \brief
  ///   Resets the rotation delta of the object
  /// 
  /// in the current frame. 
  /// 
  /// \sa VisObject3D_cl::SetRotationDelta
  /// \sa VisObject3D_cl::GetRotationDelta
  /// \sa VisObject3D_cl::IncRotationDelta
  /// \sa VisObject3D_cl::HasRotationDelta
  /// \sa VisObject3D_cl::SetMotionDelta
  VISION_APIFUNC void ResetRotationDelta();

  /// \brief
  ///   Returns the rotation delta of the object
  /// 
  /// in the current frame. The components of the vector specify the yaw, pitch and roll angle (in
  /// degrees) respectively.
  /// 
  /// The return vector receives the angles to be added to the current frame, so the values are
  /// framerate dependent
  /// 
  /// \return
  ///   hkvVec3& vRotationDelta: current rotation delta
  /// 
  /// \note
  ///   The animation system takes care of applying rotation delta to animated objects. You can
  ///   remove the APPLY_MOTION_DELTA flag from the animation configuration to disable rotation
  ///   delta when playing animations.
  /// 
  /// \note
  ///   Please note that the engine is not responsible for processing rotation delta of
  ///   VisObject3D_cl instances. In case of entities the vHavokPhysicsModule takes care of this
  ///   as long as a physics object is available for the instance. When using a different physics
  ///   system or other VisObject3D_cl derived classes your game code has to take care of
  ///   processing the rotation delta. 
  /// 
  /// \sa VisObject3D_cl::SetRotationDelta
  /// \sa VisObject3D_cl::IncRotationDelta
  /// \sa VisObject3D_cl::ResetRotationDelta
  /// \sa VisObject3D_cl::HasRotationDelta
  /// \sa VisObject3D_cl::SetMotionDelta
  VISION_APIFUNC const hkvVec3& GetRotationDelta() const;

  /// \brief
  ///   Indicates, whether the object has got rotation delta in the current frame
  /// 
  /// It is the same result as testing the rotation vector obtained with GetRotationDelta() to  be
  /// the zero vector
  /// 
  /// \return
  ///   BOOL status: TRUE, if the current rotation delta vector is other than (0,0,0)
  /// 
  /// \note
  ///   The animation system takes care of applying rotation delta to animated objects. You can
  ///   remove the APPLY_MOTION_DELTA flag from the animation configuration to disable rotation
  ///   delta when playing animations.
  /// 
  /// \note
  ///   Please note that the engine is not responsible for processing rotation delta of
  ///   VisObject3D_cl instances. In case of entities the vHavokPhysicsModule takes care of this
  ///   as long as a physics object is available for the instance. When using a different physics
  ///   system or other VisObject3D_cl derived classes your game code has to take care of
  ///   processing the rotation delta. 
  /// 
  /// \note
  ///   There is also a function VisAnimSequence_cl::HasRotationDelta, which returns TRUE, if the
  ///   sequence has got rotation delta at all. In contrast, this function returns TRUE if the
  ///   rotation of the current frame is other than (0,0,0)
  /// 
  /// \sa VisObject3D_cl::SetRotationDelta
  /// \sa VisObject3D_cl::IncRotationDelta
  /// \sa VisObject3D_cl::ResetRotationDelta
  /// \sa VisObject3D_cl::GetRotationDelta
  /// \sa VisAnimSequence_cl::HasRotationDelta
  VISION_APIFUNC BOOL HasRotationDelta() const;

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///
 
  
  /// \brief
  ///   Gets the flags of the 3D object.
  /// 
  ///  These flags indicate whether certain object properties (such as position, orientation) have
  /// been modified since the last frame.
  /// 
  /// The return value of this function is only valid within the OnObject3DChanged virtual function
  /// call. Use it there to determine which components of the object have been modified.
  /// 
  /// \return
  ///   int iFlags: currently set flags, see VISOBJECT3D_FLAGS
  /// 
  /// \sa VisObject3D_cl::SetO3DFlags
  inline int GetO3DFlags() const { return m_iFlags; }
  
    
  /// \brief
  ///   Sets the flags of the 3D object.
  /// 
  /// Sets the flags of the 3D object indicating whether certain properties have been modified
  /// since the last frame. This function is supposed to be for internal use only. Modifications to
  /// the flags can lead to unpredictable results.
  /// 
  /// \param iFlags
  ///   new flags, see VISOBJECT3D_FLAGS
  /// 
  /// \sa VisObject3D_cl::SetO3DFlags
  inline void SetO3DFlags( int iFlags ) { m_iFlags = iFlags; }
  

  /// \brief
  ///   Gets the counter that gets increased whenever the position or orientation is modified.
  /// 
  /// You can use this counter to detect whether the transformation of the object has been
  /// changed during a specific time span.
  /// 
  /// \return
  ///   int iCtr: integer counter
  inline int GetModifiedCtr() { return m_iModifiedCtr; }
  
  
  /// \brief
  ///   Serializes the object
  /// 
  /// This function stores all important data into an archive or reads from it to create a new
  /// object.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );

  /// \brief
  ///   Overridden function to perform post-serialization setup
  VISION_APIFUNC VOVERRIDE void OnSerialized(VArchive& ar);

  VISION_APIFUNC virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;
  VISION_APIFUNC virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE { return TRUE; }

#ifdef WIN32

  /// \brief
  ///   Internal function used by vForge
  VISION_APIFUNC VOVERRIDE void SetMarkInsideSerializationSession(bool bStatus);

#endif

  ///
  /// @}
  ///

  ///
  /// @name Object Directions
  /// @{
  ///
 

  /// \brief
  ///   Gets the direction the object is facing to (x-axis)
  /// 
  /// The direction corresponds to the front vector (x-axis) of the matrix (m[0], m[3], m[6])
  /// 
  /// \return
  ///   hkvVec3 frontDir
  inline hkvVec3 GetObjDir() const
  {
    EnsureCachedRotationMatrixValid();
    return m_cachedRotMatrix.getAxis(0);
  }  

  /// \brief
  ///   Gets the direction facing to the right from the object (y-axis)
  /// 
  /// The direction corresponds to the right vector (y-axis) of the matrix (m[1], m[4], m[7])
  /// 
  /// \return
  ///   hkvVec3& rightDir
  inline hkvVec3 GetObjDir_Right() const
  {
    EnsureCachedRotationMatrixValid();
    return m_cachedRotMatrix.getAxis(1);
  }  

  
  /// \brief
  ///   Gets the direction facing upwards from the object (z-axis)
  /// 
  /// The direction corresponds to the up vector (z-axis) of the matrix (m[2], m[5], m[8])
  /// 
  /// \return
  ///   hkvVec3& upDir
  inline hkvVec3 GetObjDir_Up() const
  {
    EnsureCachedRotationMatrixValid();
    return m_cachedRotMatrix.getAxis(2);
  }

  /// \brief
  ///   Returns the VisObject3DVisData_cl object associated with this VisObject3D_cl.
  /// 
  /// A VisObject3DVisData_cl has to be defined in VisObject3D_cl instances that are to be used in
  /// visibility determination, since the VisObject3DVisData_cl class takes care of visibility zone
  /// assignment and transitioning between visibility zones.
  /// 
  /// Dynamic scene elements in the Vision engine for which visibility determination is used
  /// automatically generate their own VisObject3DVisData_cl objects (e.g. entities, context
  /// cameras, light sources, and visibility objects). Custom VisObject3D_cl types have to  create
  /// their own VisObject3DVisData_cl if visibility zone assignments should be tracked for them.
  /// 
  /// \return
  ///   VisObject3DVisData_cl *: The visibility data object, or NULL if no VisObject3DVisData_cl is
  ///   set.
  inline VisObject3DVisData_cl *GetVisData() { return m_pVisData; }

  /// \brief
  ///   The new status determines whether visibility assignment is serialized along with the binary serialization. Default state is true.
  inline void SetSerializeVisibilityInfo(BOOL bStatus)
  {
    if (bStatus)
      m_iFlags &= (~VIS_OBJECT3D_NO_VISIBILITY_SERIALIZATION);
    else
      m_iFlags |= VIS_OBJECT3D_NO_VISIBILITY_SERIALIZATION;
  }

  /// \brief
  ///   Returns the state that has been set via SetSerializeVisibilityInfo
  inline BOOL GetSerializeVisibilityInfo() const
  {
    return (m_iFlags & VIS_OBJECT3D_NO_VISIBILITY_SERIALIZATION) == 0;
  }

  /// \brief
  ///   Returns the Scene Element Type of this VisObject3D_cl.
  /// 
  /// Scene element types are used to identify different types of scene elements, e.g. during
  /// visibility zone assignment.
  /// 
  /// For a list of scene element types, refer to the VisSceneElementType_e enumeration.
  /// 
  /// Vision's built-in scene element types automatically set this member. Custom types will have
  /// to set the m_iSceneElementType member to VIS_SCENEELEMENTTYPE_CUSTOM.
  /// 
  /// \return
  ///   unsigned int: One of the values in the VisSceneElementType_e enumeration.
  inline unsigned int GetSceneElementType() const { return m_iSceneElementType; }

  /// \brief
  ///   Forces a portal trace operation.
  /// 
  /// If two visibility zones are connected with a portal, the only legal way to get from one of
  /// these zones to the other is through the portal. After every game tick, the Vision engine
  /// performs a linear trace operation to detect such transitions through portals. However, in
  /// some (typically rare) cases, objects may perform complex movements across larger distances in
  /// a single tick, resulting in a linear trace being insufficient to detect the portal
  /// transition.
  /// 
  /// In such cases, you could of course simply call ReComputeVisibility. However, this can be
  /// quite expensive. Calling ForcePortalTrace after every (approximately) linear movement of your
  /// object within the same tick is typically a better solution. This will force the engine to
  /// perform multiple linear trace tests rather than a single one at the end of the frame.
  /// 
  /// This function only makes sense for VisObject3D_cl instances used in visibility determination
  /// (they need to have a VisObject3DVisData_cl object).
  inline void ForcePortalTrace() { if (m_pVisData != NULL) m_pVisData->HandleNodeTransition(); }
   
  V_DECLARE_SERIAL_DLLEXP( VisObject3D_cl, VISION_APIDATA )
  IMPLEMENT_OBJ_CLASS(VisObject3D_cl);

  /// \brief
  ///   Recomputes the visibility.
  /// 
  /// ReComputeVisibility sets a flag which causes the engine to recompute the visibility zone
  /// assignments for this object. You have to call this function whenever the object teleports to
  /// another location outside of the nodes it is currently assigned to, or when it crosses
  /// visibility zone boundaries in an "illegal" way.
  /// 
  /// All children of this entity will automatically recompute the visibility zone assignments as
  /// well. 
  VISION_APIFUNC void ReComputeVisibility();


  ///
  /// @}
  ///

  ///
  /// @name Misc Overrides
  /// @{
  ///

  VISION_APIFUNC VOVERRIDE int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  VISION_APIFUNC VOVERRIDE void OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos);

  VISION_APIFUNC VOVERRIDE void DisposeObject();

  VISION_APIFUNC VOVERRIDE bool GetZoneLocalSpacePosition(hkvVec3& vDest);


  VISION_APIFUNC void Release();

  ///
  /// @}
  ///

  ///
  /// @name Static Callbacks
  /// @{
  ///

  /// \brief
  ///   Static callback that gets triggered inside the constructor of a VisObject3D_cl instance.
  ///   The data object can be casted to VisObject3DDataObject_cl which contains the object
  ///   pointer.
  VISION_APIDATA static VisCallback_cl OnObject3DCreated;

  /// \brief
  ///   Static callback that gets triggered inside the destructor of a VisObject3D_cl instance. The
  ///   data object can be casted to VisObject3DDataObject_cl which contains the object pointer.
  VISION_APIDATA static VisCallback_cl OnObject3DDestroyed;

  ///
  /// @}
  ///

protected:
  friend void SetObject3DMatrix(VisObject3D_cl *pObject);

  /// \brief
  ///   Override this function to keep track of position or orientation modifications.
  /// 
  /// This virtual function gets called whenever the position, orientation or motion/rotation delta
  /// of the object has been modified. Override this function to listen to position and orientation
  /// changes.  The passed flags indicate which components of the object have been modified, see
  /// VISOBJECT3D_FLAGS enumeration for more details. 
  /// 
  /// Don't forget to call the base implementation when overriding this function. The base
  /// implementation takes care  of calling the
  /// VisModuleSystemParent_cl::ModSysNotifyFunctionParentAltered function when necessary.
  /// 
  /// Note that the object modifications don't get grouped, OnObject3DChanged might thus get called
  /// multiple times per frame.
  /// 
  /// \param iO3DFlags
  ///   VISOBJECT3D_FLAGS flags indicating the modified components
  VISION_APIFUNC virtual void OnObject3DChanged(int iO3DFlags);

  /// \brief
  ///   Recomputes the local space position and triggers the virtual OnObject3DChanged function.
  /// 
  /// This function recomputes the local space transformation and triggers the virtual
  /// OnObject3DChanged function (which e.g. informs the attached children about state changes).
  /// 
  /// However, the operation is skipped if the world space transformation is currently  being
  /// recomputed (=UpdateBinding is called). In that case UpdateBinding takes  care of updating
  /// necessary dependencies when all recomputations have been done.
  inline void UpdateDependencies()
  {
    // When modifying this function also make sure that you reflect the changes in the
    // UpdateBinding implementation.
    if (m_bUpdateBindingInProgress)
      return;

    if ( m_pParent )
      ComputeLocalSpaceData();
    OnObject3DChanged(m_iFlags);
  }

  /// \brief
  ///   Clears the modification related flags (VIS_OBJECT3D_ALLCHANGED) from the Object3D flags.
  VISION_APIFUNC void ClearO3DChangeFlags();

  /// \brief
  ///   Ensures that the orientation of the object is within the valid range of -180 and 180.
  VISION_APIFUNC void MakeValidOrientation();

  /// \brief
  ///   Recomputes the world space transformation from the current local space transformation
  VISION_APIFUNC void UpdateBinding();

  /// \brief
  ///   Recomputes the local space transformation from the current world space transformation
  VISION_APIFUNC void ComputeLocalSpaceData();

  /// \brief
  ///   Listens to parent modifications in order to update the world space transformation
  ///   accordingly.
  VISION_APIFUNC VOVERRIDE void ModSysNotifyFunctionParentAltered( int iFlags );

  /// \brief
  ///   Implements VisModuleSystemChild_cl::GetOwnerTypeId() and returns RTTI type for VisObject3D_cl  
  VISION_APIFUNC VOVERRIDE VType* GetOwnerTypeId();

  /// \brief
  ///   Overridden module system function
  VISION_APIFUNC VOVERRIDE void ModSysNotifyFunctionParentDestroyed();

  /// \brief
  ///   Overridden module system function
  VISION_APIFUNC VOVERRIDE void ModSysNotifyFunctionOnDisconnect(BOOL bDestroy);


  hkvVec3 m_vPosition;                        ///< x,y,z position
  hkvVec3 m_vOrientation;                     ///< orientation (yaw/pitch/roll)
  hkvVec3 m_vMotionDeltaWorldSpaceXYZ;        ///< x,y,z motion delta in world space
  hkvVec3 m_vMotionDeltaLocalSpaceXYZ;        ///< x,y,z motion delta in local space

  mutable hkvMat3 m_cachedRotMatrix;          ///< cached rotation matrix, gets recomputed when orientation changes
  hkvVec3 m_vRotationDeltaXYZ;                ///< x,y,z rotation delta

  int m_iFlags;                               ///< status flags
  mutable int m_iInternalFlags;               ///< internal flags
  int m_iModifiedCtr;                         ///< Counter that gets increased whenever the position or orientation is modified (not serialized)
  
  // local space data
  VisObject3D_cl *m_pParent;                  ///< parent object
  hkvVec3 m_vLocalPosition;                   ///< local space position
  hkvMat3 m_localOrientation;                 ///< local space orientation
  bool m_bUpdateBindingInProgress;            ///< if TRUE then the world space position is currently being recomputed  
  short m_iSceneElementType;                  ///< set in the constructor of every class type. VIS_SCENEELEMENTTYPE_XYZ

  VisObject3DVisData_cl *m_pVisData;          ///< visibility-related component; only needs to be present for objects using visibility handling (e.g. entities, lights, visibility objects)

  #ifdef HK_DEBUG_SLOW
  bool m_bDeserializationCallCheck;
  #endif
  ///
  /// @}
  ///

};



/// \brief
///   Class containing data used as a template when constructing a new Object3D instance
/// 
/// It is currently only used as the base class for VisEntityTemplate_cl
class VisObject3DTemplate_cl
{
public:

  /// \brief
  ///   Constructor for an object creation template
  VisObject3DTemplate_cl(const char *pszClassName = NULL)
  {
    m_pszClassName = pszClassName;
    m_pszObjectKey = NULL;
    m_vScale.set(1.f, 1.f, 1.f);
  }
  
  /// \brief
  ///   The class name of the new entity, e.g. "VisBaseEntity_cl"
  const char *m_pszClassName;
    
  /// \brief
  ///   The position of the new entity
  hkvVec3 m_vPosition;
  
  /// \brief
  ///   The rotation of the new entity
  hkvVec3 m_vRotation;
  
  /// \brief
  ///   The scale of the new entity
  hkvVec3 m_vScale;

  /// \brief
  ///   Optional string that initializes the object key (SetObjectKey).
  const char *m_pszObjectKey;

};


#endif  // FR_DEFINE_VISAPIOBJECT3D

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
