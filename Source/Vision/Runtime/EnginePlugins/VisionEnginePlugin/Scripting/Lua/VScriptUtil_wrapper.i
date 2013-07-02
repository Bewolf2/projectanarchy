
#ifndef VLUA_APIDOC
  
class VScriptUtil_wrapper
{
public:
  VScriptUtil_wrapper();
  ~VScriptUtil_wrapper();

  inline int GetRandInt(int iMaxRand = RAND_MAX);
  inline int GetRandIntSym(int iMaxRand = RAND_MAX_2);

  inline float GetRandFloat(float fMaxRand = 1.0f);
  inline float GetRandFloatSym(float fMaxRand = 1.0f);
  
  inline hkvVec3 GetRandVector(float fMaxX, float fMaxY, float fMaxZ);
  inline hkvVec3 GetRandVectorSym(float fMaxX, float fMaxY, float fMaxZ);
  
  inline int Round(float fNumber);
  inline float Round(float fNumber, int iDecimalPlaces);
  
  inline float RadToDeg(float radValue);

  inline float DegToRad(float degValue);

  inline float AngleBetweenVectors(const hkvVec3& vec1, const hkvVec3& vec2);

  inline hkvMat3 CreateLookAtMatrix( const hkvVec3* pPos, const hkvVec3* pTarget, const hkvVec3* pUp = NULL );
};

#else

/// \brief Provides utility functions and is available in Lua as the global instance \b Util.
/// \par Example
///   \code
///     function OnThink(self)
///       local origin = Vision.hkvVec3()
///       local matrix = Util:CreateLookAtMatrix(origin, self:GetPosition)
///       ...
///     end
///   \endcode
class VScriptUtil_wrapper {
public:

  /// @name Number Utilities
  /// @{
  
  /// \brief Generate a random integer number in the range of [0,maxRand)
  /// \param maxRand (\b optional) The upper limit for your random numbers (excluding the specified number). The limit will be RAND_MAX (=0x7fff) when not specified. 
  /// \return A random integer number in the range of [0,maxRand).
  number GetRandInt(number maxRand = Vision.RAND_MAX);

  /// \brief Generate a random integer number in the symmetrical range of (-maxRand,maxRand)
  /// \param maxRand (\b optional) The upper and lower (negative) limit for your random numbers (excluding the specified number). The limit will be RAND_MAX/2 (=0x3fff) when not specified. 
  /// \return A random integer number in the range of (-maxRand,maxRand).
  number GetRandIntSym(number maxRand = Vision.RAND_MAX_2);

  /// \brief Generate a random float number in the range of [0,maxRand]
  /// \param maxRand (\b optional) The upper limit for your random numbers (including the specified number). The limit will be 1.0 when not specified. 
  /// \return A random float number in the range of [0,maxRand].
  number GetRandFloat(number maxRand = 1);

  /// \brief Generate a random float number in the symmetrical range of [-maxRand,maxRand]
  /// \param maxRand (\b optional) The upper and lower (negative) limit for your random numbers (including the specified number). The limit will be 1.0 when not specified. 
  /// \return A random float number in the range of [-maxRand,maxRand].
  number GetRandFloatSym(number maxRand = 1);
  
  /// \brief Generate a random vector, where each component has a random number in the range of [0, max].
  /// \param maxX The upper limit for your random numbers along the x axis. 
  /// \param maxY The upper limit for your random numbers along the y axis. 
  /// \param maxZ The upper limit for your random numbers along the z axis. 
  /// \return A vector with random values in the range of [0, max].
  hkvVec3 GetRandVector(number maxX, number maxY, number maxZ);

  /// \brief Generate a random vector, where each component has a random number in the symmetrical range of [-max, max].
  /// \param maxX The upper and lower limit for your random numbers along the x axis. 
  /// \param maxY The upper and lower limit for your random numbers along the y axis. 
  /// \param maxZ The upper and lower limit for your random numbers along the z axis. 
  /// \return A vector with random values in the range of [-max, max].
  hkvVec3 GetRandVectorSym(number maxX, number maxY, number maxZ);
  
  /// \brief Round a given number.
  /// \param number The number to round.
  /// \param decimalPlaces (\b optional) The decimal place to round. Not required for integer rounding.
  /// \return The rounded number.
  number Round(number number, number decimalPlaces = 0);

  /// \brief convert radian to degree.
  /// \param radValue A value in radian.
  /// \return A value in degree.
  number RadToDeg(number radValue);

  /// \brief convert degree to radian.
  /// \param degValue A value in degree.
  /// \return A value in radian.
  number DegToRad(number degValue);

  /// @}
  /// @name Vector & Matrix Utilities
  /// @{
  
  /// \brief Calculate the angle between two vectors.
  /// \param v1 The first vector.
  /// \param v2 The second vector.
  /// \return The angle in degree.
  number AngleBetweenVectors(hkvVec3 v1, hkvVec3 v2);
  
  /// \brief Create a 3x3 look-at matrix.
  /// \param pos The position of the observer.
  /// \param target The position of the target.
  /// \param up (\b optional) The up vector.
  /// \return A 3x3 matrix which could be used to transform 3D objects.
  hkvMat3 CreateLookAtMatrix(hkvVec3 pos, hkvVec3 target, hkvVec3 up = nil);
  
  /// @}
};

#endif