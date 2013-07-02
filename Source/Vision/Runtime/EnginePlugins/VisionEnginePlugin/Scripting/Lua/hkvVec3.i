#ifndef VLUA_APIDOC

class hkvVec3
{
public:
  hkvVec3 (const hkvVec3& cc);
  explicit hkvVec3 (float xyz);
  hkvVec3 (float _x, float _y, float _z);

  void setZero ();
  void set (float xyz);
  void set (float _x, float _y, float _z);

  bool isZero () const;
  bool isZero (float fEpsilon) const;

  bool isIdentical (const hkvVec3& rhs) const;
  bool isEqual (const hkvVec3& rhs, float fEpsilon) const;

  bool isValid () const;
  bool isNormalized (float fEpsilon = HKVMATH_DEFAULT_EPSILON) const;

  float dot (const hkvVec3& rhs) const;
  float getLength () const;

  float getLengthSquared () const;
  float getLengthInverse () const;

  void normalize ();
  const hkvVec3 getNormalized () const;
  
  %extend {
    bool normalizeIfNotZero()
    {
      return self->normalizeIfNotZero()==HKV_SUCCESS;
    }
  }
  
  float normalizedEnsureUnitLength ();
  float getDistanceTo (const hkvVec3& rhs) const;
  float getDistanceToSquared (const hkvVec3& rhs) const;

  %extend {
    bool setLength(float f)
    {
      return self->setLength(f)==HKV_SUCCESS;
    }
  }

  void negate ();

  const hkvVec3 compMul (const hkvVec3& rhs) const;
  const hkvVec3 compDiv (const hkvVec3& rhs) const;

  void setMin (const hkvVec3& rhs);
  void setMax (const hkvVec3& rhs);
  void clampTo (const hkvVec3& MinVal, const hkvVec3& MaxVal);

  void setAbs (const hkvVec3& val);

  const hkvVec3 operator- (void) const;

  const hkvVec3 cross (const hkvVec3& rhs) const;
  void setInterpolate (const hkvVec3& lhs, const hkvVec3& rhs, float t);

  const hkvVec3 getOrthogonalVector () const;

  void reflect (const hkvVec3& vNormal);
  const hkvVec3 getReflected (const hkvVec3& vNormal) const;

  float x;
  float y;
  float z;
  
  %extend {
    hkvVec3 (bool bInitialize = true)
    {
      if(bInitialize) return new hkvVec3(0.0f);
      return new hkvVec3(hkvNoInitialization);
    }
    
    void makeOrthogonalTo(const hkvVec3& vNormal)
    {
      self->makeOrthogonalTo(vNormal.getNormalized());
    }
    
    hkvVec3 clone() const
    {
      return *self; //will call the copy constructor
    }
  
    const hkvVec3 compMul(float x, float y, float z)
    {
      return self->compMul(hkvVec3(x,y,z));
    }
    
    const hkvVec3 compDiv(float x, float y, float z)
    {
      return self->compDiv(hkvVec3(x,y,z));
    }

    float __len(const hkvVec3 * vec) const
    {
      //the query looks odd ,but in some cases this can happen in Lua with SWIG
      return self==NULL ? 0 : self->getLength();
    }

    bool __eq (const hkvVec3 * pRhs) const
    {
      if(pRhs==NULL) return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }

    hkvVec3 __add (const hkvVec3 * pRhs)
    {
     if(pRhs==NULL) return *self;
      return (*self + *pRhs);
    }

    hkvVec3 __sub (const hkvVec3 * pRhs)
    {
      if(pRhs==NULL) return *self;
      return (*self - *pRhs);
    }

    hkvVec3 __mul (float f)
    {
      return (*self * f);
    }

    hkvVec3 __div (float f)
    {
      return (*self / f);
    }
    
    float __getitem__(int iElem)
    {
      if(iElem>=0 && iElem<3)
        return self->data[iElem];
        
      return 0;
    }
    
    void __setitem__(int iElem, float fValue)
    {
      if(iElem>=0 && iElem<3) 
        self->data[iElem] = fValue;
    }
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvVec3, 128, "[%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z )
VSWIG_CREATE_TOSTRING(hkvVec3, "hkvVec3: [%1.2f,%1.2f,%1.2f]", self->x, self->y, self->z)

#else

/// \brief 
///   Wrapper class for the 3-component vector hkvVec3.
/// \note Since all wrappers classes are handled as pointers in Lua you
///   cannot copy it's content by assignment. Please take a look at the code samples.
/// \par Example
///   \code
///     -- create a new vector:
///     local vec1 = Vision.hkvVec3(1,3,7)
///
///     -- this will not copy the vector`s content (just another pointer to it):
///     local vec2 = vec1
///
///     -- this will affect access to vec1 and vec2 since the pointers share the same object:
///     vec2.x = 10
///     -- this assertion will not fail
///     Vision.Assert(vec1==vec2, "When this assertions fails vec1 and vec2 are pointing to different vectors.")
///
///     -- in order to copy the content to another vector, you have to clone:
///     local vec3 = vec1:clone()
///
///     -- or create a new vector using the copy constructor
///     local vec4 = Vision.hkvVec3(vec1)
///
///     -- to make life easier, every Vision scripting method creates a vector copy for you - e.g:
///     local pos1 = entity:GetPosition()
///     local pos2 = entity:GetPosition()
///     pos1.y = -23
///     pos2.y = 12
///
///     -- this assertion will not fail
///     Vision.Assert(pos1~=pos2, "When this assertion fails pos1 and pos2 are pointing to the same vector.")
///
///     -- if you have problems comparing vectors you can use 'isEqual' and specify an alternative epsilon.
///     -- The comparison operator uses Vision.MATH_DEFAULT_EPSILON (0.00001), but you can use any arbitrary value.
///     local result = pos1:isEqual(pos2, Vision.HKVMATH_HUGE_EPSILON) -- tolerance will be 0.001
///   \endcode
/// \see clone
class hkvVec3
{
public:

  /// @name Initialization
  /// @{
  
  /// \brief Default Constructor.
  /// \param initialize (\b optional) Set to true to initialize with 0 (default), false to create an un-initialized vector.
  hkvMat3 (boolean initialize = true);
  
  /// \brief Copy Constructor.
  /// \param other The vector to copy.
  hkvVec3 (hkvVec3 other);
  
  /// \brief Constructor with single value initialization.
  /// \param xyz The value for the x, y and z axis.
  hkvVec3 (number xyz);
  
  /// \brief Constructor with x,y and z value initialization.
  /// \param x Initial value for x.
  /// \param y Initial value for y.
  /// \param z Initial value for z.
  hkvVec3 (number x, number y, number z);

  /// \brief Sets all components to zero.
  void setZero();
  
  /// \brief Sets all components to the same specified value.
  /// \param xyz The value for the components.
  void set(number xyz);
  
  /// \brief Sets all components to a specified value.
  /// \param x Value for x.
  /// \param y Value for y.
  /// \param z Value for z.
  void set(number x, number y, number z);

  /// @}
  /// @name Checks
  /// @{

  /// \brief Checks whether this vector is zero (0,0,0 - within some epsilon range).
  /// \param epsilon (\b optional) Specify an epsilon range for this range (default is Vision.MATH_DEFAULT_EPSILON).
  /// \return true if zero within the specified epsilon, otherwise false.
  boolean isZero (number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief Checks whether the two vectores are identical.
  /// \param other The vector to check.
  /// \return true if identical (without epsilon), otherwise false.
  boolean isIdentical (hkvVec3 other);
  
  /// \brief Checks whether the two vectors are equal (within some epsilon range).
  /// \param other The vector to check.
  /// \param epsilon Specify an epsilon range for this range (e.g Vision.MATH_LARGE_EPSILON).
  /// \return true if equal within the given epsilon, otherwise false.
  boolean isEqual (hkvVec3 other, number epsilon);

  /// \brief  Checks whether all components are valid floats.
  /// \return true if any component is +/- infinity or NaN, false otherwise.
  boolean isValid();
  
  /// \brief Checks whether this vector has a length of 1, within some epsilon range.
  /// \param epsilon (\b optional) The maximum deviation that is allowed (default is Vision.MATH_DEFAULT_EPSILON).
  boolean isNormalized(number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// @}
  /// @name Utility Methods
  /// @{
  
  /// \brief Returns the dot product of two vectors.
  /// \param other The other vector for the dot product operation.
  /// \return The dot product: self.x*other.x + self.y*other.y + self.z*other.z
  number dot(hkvVec3 other);
  
  /// \brief Negates this vector - flips the sign of each component.
  void negate();

  /// \brief Normalizes this vector - makes sure its length is 1.
  /// \note If the vector was zero or degenerate before, it will have invalid data afterwards.
  ///   Normalizing zero-length vectors will give undefined results and may be very different on each platform.
  /// \see hkvVec3::getLength
  /// \see hkvVec3::getLengthInverse
  /// \see hkvVec3::getNormalized
  void normalize();
  
  /// \brief  Normalizes this vector under all circumstances and returns its previous length.
  /// \note If the vector can not be normalized, its x component will be set to 1 and all other components to 0.
  /// In this case the function will return 0, as the vectors previous length was 0 (or invalid).
  /// If the vector can be normalized, the previous length is returned.
  /// \see hkvVec3::normalize
  /// \see hkvVec3::normalizeIfNotZero
  number normalizedEnsureUnitLength();
  
  /// \brief Returns a normalized copy of this vector. The behavior is identical to normalize.
  /// \return The normalized vector.
  /// \see hkvVec3::normalize
  hkvVec3 getNormalized();
  
  /// \brief  Normalizes the vector, if its length is non-zero. Otherwise the vector will stay unmodified.
  /// \return If normalization was not possible, it returns false otherwise true.
  /// \note In case of failure the vector will not be modified. That means the vector will either be all zero
  /// (and thus zero-length) or some component is invalid (+/-Infinity or NaN).
  /// \see hkvVec3::normalize
  /// \see hkvVec3::normalizedEnsureUnitLength
  boolean normalizeIfNotZero();
  
  /// \brief Returns the distance between two vectos.
  /// \note Prefer to use getDistanceToSquared if you only need to compare relative distances,
  /// as this is more efficient to compute.
  /// \param vector The point to which to compute the distance.
  /// \see hkvVec3::getDistanceToSquared
  /// \see hkvVec3::getLength
  number getDistanceTo (hkvVec3 vector);
  
  /// \brief  Returns the squared distance between this vector and the other.
  /// \note This is more efficient to use than getLength, if only relative comparisons are needed.
  /// E.g. to check whether the distance between v1 and v2 is larger than between v1 and v3.
  /// \param vector The point to which to compute the distance.
  /// \see hkvVec3::getDistanceTo
  /// \see hkvVec3::getLengthSquared
  number getDistanceToSquared (hkvVec3 vector);

  /// \brief Returns the length of this vector.
  /// \note Prefer to use getLengthSquared, if you only want to compare relative lengths,
  /// since the computation of the squared length does not require a sqrt.
  /// Prefer to use getLengthInverse if you want to divide other values by the length,
  /// since that function might be implemented more efficiently.
  /// \return The length of the vector.
  /// \see hkvVec3::getLengthSquared
  /// \see hkvVec3::getLengthInverse
  /// \see hkvVec3::getDistanceTo
  /// \see hkvVec3::getDistanceToSquared
  number getLength ();
  
  /// \brief Normalizes this vector and then multiplies it by the supplied value.
  /// \note In case of a failure the vector stays unchanged and the function acts identical to normalizeIfNotZero.
  /// \param length The new length to aet.
  /// \return Returns false if the vector could not be normalized, otherwise true.
  /// \see hkvVec3::normalizeIfNotZero
  boolean setLength(number length);
  
  /// \brief  Returns the squared length of this vector.
  /// \note It is more efficient to use the squared length, instead of the real length,
  /// if one only wants to compare vectors for their relative length 
  /// \return The sqaured length of the vector.
  /// \see hkvVec3::dot
  /// \see hkvVec3::getLength
  /// \see hkvVec3::getDistanceToSquared
  number getLengthSquared ();
  
  /// \brief Returns ( 1.0f / getLength () ), but computes this more efficiently.
  /// \note If the length is supposed to be used for normalization purposes, prefer this function.
  /// \return The inverse length of the vector.
  /// \see hkvVec3::getLength
  number getLengthInverse ();
  
  /// \brief Returns the cross product of this vector and other, using a right-handed coordinate system.
  /// \note The cross product is not commutative, that means 
  ///   v1.cross (v2) will yield a different result than
  ///   v2.cross (v1) (it will be the negated vector).
  /// \param other The second vector to compute the cross product with.
  /// \return The cross product.
  const hkvVec3 cross (hkvVec3 other);
  
  /// \brief Sets this vector to the linear interpolation between two vectors.
  /// \param start The first vector for the interpolation.
  /// \param stop The second vector for the interpolation.
  /// \param value The interpolation factor between 0 and 1.
  void setInterpolate (hkvVec3 start, hkvVec3 stop, number value);

  /// \brief Makes this vector orthogonal to the specified normal if possible. 
  /// \param normal The normal used for the operation (will be normalized internally)!
  /// \note This vector will not be normalized afterwards.
  /// If this vector and specified normal are parallel, the resulting vector will be the zero vector.
  /// \see hkvVec3::getOrthogonalVector
  void makeOrthogonalTo (hkvVec3 normal);
  
  /// \brief Returns some arbitrary vector that is orthogonal to this vector.
  /// \note There is no guarantee into which direction the resulting vector will point, other
  /// than that it will be orthogonal to this vector.
  /// \return A vector which is orthogonal to this.
  /// \see hkvVec3::makeOrthogonalTo
  hkvVec3 getOrthogonalVector ();

  /// \brief Reflects this vector at the plane that goes through the origin with the given normal.
  /// \param normal The normal of a plane through the origin, which will be used to reflect this vector.
  /// \see hkvVec3::getReflected
  void reflect (hkvVec3 normal);
  
  /// \brief Returns a vector that is reflected at the plane that goes through the origin with the given normal.
  /// \param normal The normal of a plane through the origin, which will be used to reflect this vector.
  /// \return The reflected vector.
  /// \see hkvVec3::reflect
  hkvVec3 getReflected (hkvVec3 normal);

  /// \brief Each component of this vector is set to the specified minimum (if below the minimum).
  /// \param min The min values for this vector.
  /// \see setMax  
  /// \see clampTo
  void setMin (hkvVec3 min);
  
  /// \brief Each component of this vector is set to the specified maximum (if above the maximum).
  /// \param max The max values for this vector.
  /// \see setMin
  /// \see clampTo
  void setMax (hkvVec3 max);
  
  /// \brief
  ///   Each component of this vector is set to the minimum of lhs and the corresponding component of rhs.
  /// \param minVal
  /// \param maxVal
  /// \see setMin
  /// \see setMax
  void clampTo (hkvVec3 minVal, hkvVec3 maxVal);
  
  /// \brief All components of this vector are set to the absolute value of the corresponding component of val.
  /// \param value The new (absolute) values for x,y and z.
  void setAbs (hkvVec3 value);

  /// \brief Clones the current vector.
  /// \return An identical hkvVec3;
  /// \note Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy it's content by assignment. Please take a look at the code samples.
  /// \par Example
  ///   \code
  ///     -- create a new matrix:
  ///     local vec1 = Vision.hkvVec3(1,2,3)
  ///
  ///     -- this will not copy the content of vec1 (just another pointer to it):
  ///     local vec2 = vec1
  ///
  ///     -- this will affect access to vec1 and vec2 since the pointers share the same object:
  ///     vec2.x = 5
  ///     -- this assertion will not fail
  ///     Vision.Assert(vec1==vec2, "When this assertions fails 'vec1' and 'vec2' are pointing to different vectors.")
  ///
  ///     -- in order to copy the content to another matrix, you have to clone:
  ///     local vec3 = vec1:clone()
  ///
  ///     -- this will affect only m3:
  ///     vec3.y = 6
  ///     -- this assertion will not fail
  ///     Vision.Assert(vec1~=vec3, "When this assertions fails 'vec1' and 'vec3' are pointing to same vector.")
  ///
  ///     -- instead of clone you can also create a new vector using the copy constructor:
  ///     local vec4 = Vision.hkvVec3(vec1)
  ///   \endcode
  hkvVec3 clone();
  
  /// @}
  /// @name Operators and Math Methods
  /// @{

  /// \brief Returns a vector that is the component-wise multiplication of this vector and the specified vector.
  /// \param other The other vector used for the component-wise multiplication.
  /// \return The result vector.
  hkvVec3 compMul (hkvVec3 other);
  
  /// \brief Returns a copy of a component-wise multiplyed vector.
  /// \param x The x multiplication factor.
  /// \param y The y multiplication factor.
  /// \param z The z multiplication factor.
  /// \return The result vector.
  hkvVec3 compMul(float x, float y, float z);
    
  /// \brief Returns a vector that is the component-wise division of this vector and the specified vector.
  /// \param rhs The other vector used for the component-wise multiplication.
  /// \return The result vector.
  hkvVec3 compDiv (hkvVec3& rhs);

  /// \brief Returns a copy of a component-wise devided vector.
  /// \param x The x division factor.
  /// \param y The y division factor.
  /// \param z The z division factor.
  /// \return The result vector.
  hkvVec3 compDiv(float x, float y, float z);
  
  /// \brief Unary - Operator. Returns a negated copy of this vector (local vec = -anotherVec).
  /// \return The negated copy.
  hkvVec3 __unm();

  /// \brief Operator # for hkvVec3, which returns the length of the vector (equivalent to getLength).
  /// \note Prefer to use getLengthSquared, if you only want to compare relative lengths,
  /// since the computation of the squared length does not require a sqrt.
  /// Prefer to use getLengthInverse if you want to divide other values by the length,
  /// since that function might be implemented more efficiently.
  /// \return The length of the vector.
  /// \see hkvVec3::getLength
  /// \see hkvVec3::getLengthSquared
  /// \see hkvVec3::getLengthInverse
  /// \see hkvVec3::getDistanceTo
  /// \see hkvVec3::getDistanceToSquared
  number __len();

  /// \brief Operator == for hkvVec3 comparison.
  /// \note Use isEqual for a custom epsilon.
  /// \param other The other vector to compare with.
  /// \return true if the vectors are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  /// \see isEqual
  boolean __eq (hkvVec3 other);

  /// \brief Operator + for hkvVec3, which adds the two hkvVec3 component-wise.
  /// \param other The hkvVec3 to add.
  /// \return The result hkvVec3.
  hkvVec3 __add (hkvVec3 other);

  /// \brief Operator - for hkvVec3, which subtracts the two hkvVec3 component-wise.
  /// \param other The hkvVec3 to subtract.
  /// \return The result hkvVec3.
  hkvVec3 __sub (hkvVec3 other);

  /// \brief Operator * to multiply each element by a scalar value.
  /// \param value The scalar value multiplier.
  /// \return The result vector.
  hkvVec3 __mul (number value);

  /// \brief Operator / to devide each element by a scalar value.
  /// \param value The scalar value used for the division.
  /// \return The result vector.
  hkvVec3 __div (number value);
  
  /// \brief Returns a specific component from the vector (local vec = mat[6]).
  /// \param component The index, of the component (0 = x, 1 = y, 2 = z).
  /// \return The value of the requested component.
  number __getitem(number component);
  
  /// \brief Sets a specific component of the vector.
  /// \param component The index, of the component (0 = x, 1 = y, 2 = z).
  /// \param value The value to be set.
  void __setitem(number component, number value);
  
  /// @}
  /// @name Public Members
  /// @{
  
  // \brief The x component of the vector.
  number x;
  
  // \brief The y component of the vector.
  number y;
  
  // \brief The z component of the vector.
  number z;
    
  /// @}
  
}

#endif