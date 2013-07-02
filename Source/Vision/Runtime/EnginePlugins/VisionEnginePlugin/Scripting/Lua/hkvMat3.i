#ifndef VLUA_APIDOC

class hkvMat3
{
public:
  
  void setIdentity ();
  void setZero ();

  bool isZero () const;
  bool isZero (float fEpsilon) const;

  bool isIdentity () const;
  bool isIdentity (float fEpsilon) const;
  
  void setRow (unsigned int iRow, hkvVec3 row);
  void setColumn (unsigned int iColumn, hkvVec3 column);
  
  const hkvVec3 getRow (unsigned int iRow) const;
  const hkvVec3 getColumn (unsigned int iColumn) const;

  hkvVec3 getScalingFactors () const;
  void setScalingFactors (const hkvVec3& vXZY);

  void transpose ();
  const hkvMat3 getTransposed () const;

  %extend {   
    bool invert()
    {
      return self->invert()==HKV_SUCCESS;
    }
    
    void setScalingFactors(float x, float y, float z)
    {
      self->setScalingFactors(hkvVec3(x,y,z));
    }
    
    float getElement(unsigned int iNthElement, bool bUseColumnMajor = true)
    {
      return self->getElement(iNthElement, (bUseColumnMajor ? hkvMat3::ColumnMajor : hkvMat3::RowMajor));
    }
    
    void setElement(unsigned int iNthElement, float f, bool bUseColumnMajor = true)
    {
      self->setElement(iNthElement, f, (bUseColumnMajor ? hkvMat3::ColumnMajor : hkvMat3::RowMajor));
    }
    
    void setRow (unsigned int iRow, float r0, float r1, float r2)
    {
      self->setRow(iRow, hkvVec3(r0, r1, r2));
    }
    
    void setColumn (unsigned int iColumn, float c0, float c1, float c2)
    {
      self->setColumn(iColumn, hkvVec3(c0, c1, c2));
    }
    
    void setAxis (unsigned int iAxisXYZ, float x, float y, float z)
    {
      self->setAxis(iAxisXYZ, hkvVec3(x, y, z));
    }
    
    void setScalingMatrix (float x, float y, float z)
    {
      self->setScalingMatrix(hkvVec3(x, y, z));
    }
    
    void setRotationMatrix (const hkvVec3 * pAxis, float fDegree)
    {
      if(pAxis!=NULL)
        self->setRotationMatrix(pAxis->getNormalized(), fDegree);
    }
  }
  
  void setLookInDirectionMatrix (hkvVec3 vLookDir, hkvVec3 vUpDir = hkvVec3 (0, 0, 1));
  
  const hkvVec3 getAxis (unsigned int iAxisXYZ) const;
  void setAxis (unsigned int iAxisXYZ, const hkvVec3& v);

  void setScalingMatrix (const hkvVec3& vScaleXYZ);

  void setRotationMatrixX (float fDegree);
  void setRotationMatrixY (float fDegree);
  void setRotationMatrixZ (float fDegree);

  bool isIdentical (const hkvMat3& rhs) const;
  bool isEqual (const hkvMat3& rhs, float fEpsilon) const;

  const hkvVec3 transformDirection (const hkvVec3& v) const;

  const hkvMat3 multiply (const hkvMat3& rhs) const;
  const hkvMat3 multiplyReverse (const hkvMat3& rhs) const;

  %extend {
    
    hkvMat3 (bool bInitialize = true)
    {
      hkvMat3 *pNew = new hkvMat3(hkvNoInitialization);
    
      if(!bInitialize) return pNew;
      
      pNew->setIdentity();
      return pNew;
    }
    
    hkvMat3 (const hkvMat3 * pOther)
    {     
      hkvMat3 *pNew = new hkvMat3(hkvNoInitialization);
    
      if(pOther!=NULL) pNew->set(pOther->m_ElementsCM, hkvMat3::ColumnMajor);
      
      return pNew;
    }
    
    hkvMat3 (float c0r0, float c1r0, float c2r0, float c0r1, float c1r1, float c2r1, float c0r2, float c1r2, float c2r2, bool bUseColumnMajor = false)
    {
      return new hkvMat3(c0r0, c1r0, c2r0, c0r1, c1r1, c2r1, c0r2, c1r2, c2r2, (bUseColumnMajor ? hkvMat3::ColumnMajor : hkvMat3::RowMajor) );
    }
    
    void set (float c0r0, float c1r0, float c2r0, float c0r1, float c1r1, float c2r1, float c0r2, float c1r2, float c2r2, bool bUseColumnMajor = false)
    {
      self->set(c0r0, c1r0, c2r0, c0r1, c1r1, c2r1, c0r2, c1r2, c2r2, (bUseColumnMajor ? hkvMat3::ColumnMajor : hkvMat3::RowMajor) );
    }
    
    hkvMat3 clone() const
    {
      return *self; //will call the copy constructor
    }
    
    bool __eq (const hkvMat3 * pRhs) const
    {
      if(pRhs==NULL) return false;
      return pRhs->isEqual(*self, HKVMATH_DEFAULT_EPSILON);
    }
    
    hkvMat3 __add (const hkvMat3 * pRhs)
    {
      if(pRhs==NULL) return *self;
      return (*self + *pRhs);
    }

    hkvMat3 __sub (const hkvMat3 * pRhs)
    {
      if(pRhs==NULL) return *self;
      return (*self - *pRhs);
    }
    
    hkvMat3 __mul (const hkvMat3 * pRhs)
    {
      %#ifdef HKVMATH_ENABLE_NEW_OPERATORS
        if(pRhs==NULL) return *self;
        return (*self * *pRhs);
      %#else
        Vision::Error.SystemMessage("Warning: Calling 'hkvMat3 operator* (const hkvMat3& lhs, const hkvMat3& rhs)' without enabling 'HKVMATH_ENABLE_NEW_OPERATORS'!");
        return *self;
      %#endif
    }
    
    hkvMat3 __mul (float rhs)
    {
      return (*self * rhs);
    }
    
    hkvMat3 __div (float rhs)
    {
      return (*self / rhs);
    }
    
    float __getitem__(int iElem)
    {
      if(iElem>=0 && iElem<9)  
        return self->m_ElementsCM[iElem];
        
      return 0;
    }
    
    void __setitem__(int iElem, float fValue)
    {
      if(iElem>=0 && iElem<9)      
        self->m_ElementsCM[iElem] = fValue;
    }
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(hkvMat3, 256, "[%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f][%1.2f,%1.2f,%1.2f]", self->m_ElementsCM[0],self->m_ElementsCM[1],self->m_ElementsCM[2], self->m_ElementsCM[3],self->m_ElementsCM[4],self->m_ElementsCM[5], self->m_ElementsCM[6],self->m_ElementsCM[7],self->m_ElementsCM[8])
VSWIG_CREATE_TOSTRING(hkvMat3, "hkvMat3 CM: c0[%1.2f,%1.2f,%1.2f]  c1[%1.2f,%1.2f,%1.2f] c2[%1.2f,%1.2f,%1.2f]", self->m_ElementsCM[0],self->m_ElementsCM[1],self->m_ElementsCM[2], self->m_ElementsCM[3],self->m_ElementsCM[4],self->m_ElementsCM[5], self->m_ElementsCM[6],self->m_ElementsCM[7],self->m_ElementsCM[8])


#else

/// \brief Wrapper for the hkvMat3 3x3 matrix that can be used to represent rotations and scalings.
///   The elements are stored in column-major order. 
///   That means first the data of column 0 is stored, then column 1, column 2.
///
/// \note Since all wrappers classes are handled as pointers in Lua you
///   cannot copy it's content by assignment. Please take a look at the code samples.
///
/// \par Example
///   \code
///     -- create a new matrix:
///     local m1 = Vision.hkvMat3(1,0,0, 0,1,0, 0,0,1)
///
///     -- this will not copy the content of m1 (just another pointer to it):
///     local m2 = m1
///
///     -- this will affect access to m1 and m2 since the pointers share the same object:
///     m2[5] = 3
///     -- this assertion will not fail
///     Vision.Assert(m1==m2, "When this assertions fails 'm1' and 'm2' are pointing to different matrix.")
///
///     -- in order to copy the content to another matrix, you have to clone:
///     local m3 = m1:clone()
///
///     -- or create a new matrix using the copy constructor
///     local m4 = Vision.hkvMat3(m1)
///
///     -- to make life easier, every Vision scripting method creates a matrix copy for you - e.g:
///     local mat1 = entity:GetRotationMatrix()
///     local mat2 = entity:GetRotationMatrix()
///     mat1[4] = 1.5
///     mat2[4] = -0.5
///
///     -- this assertion will not fail
///     Vision.Assert(mat1~=mat2, "When this assertion fails 'b1' and 'b2' are pointing to the same matrix.")
///
///   \endcode
/// \see clone
class hkvMat3
{
public:
  
  /// @name Initialization
  /// @{
  
  /// \brief Default Constructor.
  /// \param initialize (\b optional) Set to true to initialize with the identity matrix (default), false to create an un-initialized matrix.
  hkvMat3 (boolean initialize = true);
  
  /// \brief Copy Constructor.
  /// \param other The matrix to copy.
  hkvMat3 (hkvMat3 other);
    
  /// \brief Constructor with elemenent initialization.
  /// \note Internally data is stored in column-major format.
  ///   If you set the columnMajor parameter to false, the other parameters are considered
  ///   to be in row-major order (the expected default). This allows a 'natural' representation in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  /// \param columnMajor (\b optional) Set to true to use column-major, false for row-major (default)
  /// \par Example
  ///   \code
  ///   -- create a new hkvMat3 looking like this:
  ///   -- 1, 1, 1,
  ///   -- 2, 2, 2,
  ///   -- 3, 3, 3,
  ///   loca mat = Vision.hkvMat3(1, 1, 1, 2, 2, 2, 3, 3, 3); -- create the matrix using row-major
  ///   \endcode
  hkvMat3 (number c0r0, number c1r0, number c2r0, number c0r1, number c1r1, number c2r1, number c0r2, number c1r2, number c2r2, boolean columnMajor = false);

  /// \brief
  ///   Sets the matrix to the identity matrix.
  void setIdentity ();
  
  /// \brief
  ///   Sets the matrix to all zero.
  void setZero ();
  
  /// @}
  /// @name Creating Matrices
  /// @{
  
  /// \brief Transforms this matrix into a scaling matrix.
  /// \param scale A vector holding the scaling values for x,y and z axis.
  void setScalingMatrix (hkvVec3 scale);
  
  /// \brief Transforms this matrix into a scaling matrix.
  /// \param x Scaling values for x axis.
  /// \param y Scaling values for y axis.
  /// \param z Scaling values for z axis.
  void setScalingMatrix (number float x, float y, float z);

  /// \brief Transforms this matrix into a matrix that rotates objects around the X axis.
  /// \param degree The degrees to rotate.
  void setRotationMatrixX (number degree);
  
  /// \brief Transforms this matrix into a matrix that rotates objects around the Y axis.
  /// \param degree The degrees to rotate.
  void setRotationMatrixY (number degree);
  
  /// \brief Transforms this matrix into a matrix that rotates objects around the Z axis.
  /// \param degree The degrees to rotate.
  void setRotationMatrixZ (number degree);
  
  /// \brief Transforms this matrix into a matrix that rotates objects around an arbitrary specified axis.
  /// \param axis The rotation axis. Axis gets internally normalized (\b always).
  /// \param degree The degrees to rotate.
  void setRotationMatrix (hkvVec3 axis, number degree);
  
  /// \brief Creates an orthonormal rotation matrix which rotates an object such that it will 'look' in the given direction.
  /// \note The Vision Engine uses by default the X-axis as the 'forward' direction of objects, +Y as 'left' and +Z as 'up'.
  ///   In case that vLookDir and vUpDir are nearly equal an arbitrary up vector is used to create a valid matrix.
  /// \param lookDir The Look direction. Does not need to be normalized.
  /// \param upDir (\b optional) The up vector. Must be normalized! By default (0,0,1) is used.
  /// \par Example
  ///   \code
  ///   function OnCreate(self)
  ///     local lookAt = Vision.hkvMat3()
  ///     lookAt:setLookInDirectionMatrix(self:GetPosition()) --look at the self object
  ///     self.MyCamera:SetRotationMatrix(lookAt) -- let my camera look at the object
  ///   \endcode
  void setLookInDirectionMatrix(hkvVec3 lookDir, hkvVec3 upDir = hkvVec3 (0, 0, 1));
  
  /// @}
  /// @name Element Manipulation
  /// @{
  
  /// \brief Sets all the values in the specified row using a vector.
  /// \param rowIndex The row to set: 0, 1 or 2.
  /// \param row The vector with the row data.
  void setRow (number rowIndex, hkvVec3 row);

  /// \brief Sets all the values in the specified row.
  /// \param rowIndex The row to set: 0, 1 or 2.
  /// \param c0 Data for the specified row and colum 0.
  /// \param c1 Data for the specified row and colum 1.
  /// \param c2 Data for the specified row and colum 2.
  void setRow (number rowIndex, number c0, number c1, number c2);
    
  /// \brief Sets all the values in the specified column using a vector.
  /// \param colIndex The column to set: 0, 1 or 2.
  /// \param col The vector with the column data.
  void setColumn (number colIndex, hkvVec3 col);
  
  /// \brief Sets all the values in the specified column.
  /// \param colIndex The column to set: 0, 1 or 2.
  /// \param r0 Data for the specified column and row 0.
  /// \param r1 Data for the specified column and row 1.
  /// \param r2 Data for the specified column and row 2.
  void setColumn (number colIndex, number r0, number r1, number r2);
  
  /// \brief Returns all the values in the specified row.
  /// \param rowIndex The row to get: 0, 1 or 2.
  /// \return A vector with the row values.
  hkvVec3 getRow (number rowIndex);
  
  /// \brief Returns all the values in the specified column.
  /// \param colIndex The column to get: 0, 1 or 2.
  /// \return A vector with the column values.
  hkvVec3 getColumn (number colIndex);
  
  /// \brief Set all elements of the matrix.
  /// \note Internally data is stored in column-major format.
  ///   If you set the columnMajor parameter to false, the other parameters are considered
  ///   to be in row-major order (the expected default). This allows a 'natural' representation in a code editor.
  ///   If layout is ColumnMajor, the other parameters will be transposed. Ie. parameter c1r0 will be used as if 
  ///   it was actually c0r1, etc.
  /// \param c0r0
  ///   This is the value for column 0, row 0.
  /// \param c1r0
  ///   If layout is RowMajor, this is the value for column 1, row 0.
  ///   If layout is ColumnMajor, this is the value for row 1, column 0.
  /// \param c2r0
  ///   If layout is RowMajor, this is the value for column 2, row 0.
  ///   If layout is ColumnMajor, this is the value for row 2, column 0.
  /// \param c0r1
  ///   If layout is RowMajor, this is the value for column 0, row 1.
  ///   If layout is ColumnMajor, this is the value for row 0, column 1.
  /// \param c1r1
  ///   This is the value for column 1, row 1.
  /// \param c2r1
  ///   If layout is RowMajor, this is the value for column 2, row 1.
  ///   If layout is ColumnMajor, this is the value for row 2, column 1.
  /// \param c0r2
  ///   If layout is RowMajor, this is the value for column 0, row 2.
  ///   If layout is ColumnMajor, this is the value for row 0, column 2.
  /// \param c1r2
  ///   If layout is RowMajor, this is the value for column 1, row 2.
  ///   If layout is ColumnMajor, this is the value for row 1, column 2.
  /// \param c2r2
  ///   This is the value for column 2, row 2.
  /// \param columnMajor (\b optional) Set to true to use column-major, false for row-major (default)
  /// \par Example
  ///   \code
  ///   -- create a new hkvMat3 looking like this:
  ///   -- 1, 2, 3,
  ///   -- 4, 5, 6,
  ///   -- 7, 8, 9,
  ///   loca mat = Vision.hkvMat3(1, 2, 3, 4, 5, 6, 7, 8, 9); -- create the matrix using row-major
  ///   \endcode
  void set (number c0r0, number c1r0, number c2r0, number c0r1, number c1r1, number c2r1, number c0r2, number c1r2, number c2r2, boolean columnMajor = false);
    
  /// \brief Returns a specific element from the matrix.
  /// \note This function can be used to access elements in row-major or column-major fashion.
  ///   That can be very useful when working with other libraries that have other conventions.
  ///   You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  ///   or column-major index. Which element exactly is accessed, will then be handled internally.
  /// \param element
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like an array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  /// \param columnMajor (\b optional) Set to true (default) to access the matrix in colum-major, use false for row-major.
  /// \return The value of the requested element.
  number getElement(number element, boolean columnMajor = true);
  
  /// \brief Sets a specific element of the matrix.
  /// \note This function can be used to access elements in row-major or column-major fashion.
  ///   That can be very useful when working with other libraries that have other conventions.
  ///   You can keep the indices identical, and just need to specify, that it is meant to be a row-major
  ///   or column-major index. Which element exactly is accessed, will then be handled internally.
  /// \param element
  ///   The index, which element should be accessed. If layout is ColumnMajor, this is like accessing m_ElementsCM
  ///   which is a direct array lookup.
  ///   If layout is RowMajor, the index is interpreted as if it is meant for a matrix that is stored in row-major order.
  ///   Since this class stores its data in column-major order, the index is recalculated to access the proper element.
  /// \param value
  ///   The value to be set at the specified element index.
  /// \param columnMajor (\b optional) Set to true (default) to access the matrix in colum-major, use false for row-major.
  void setElement(number element, number value, boolean columnMajor = true);
    
  /// @}
  /// @name Checks
  /// @{
  
  /// \brief Checks whether this matrix is all zero (within some epsilon range).
  /// \param epsilon (\b optional) Specify an epsilon range for this range (default is Vision.MATH_DEFAULT_EPSILON).
  /// \return true if zero within the specified epsilon, otherwise false.
  boolean isZero (number epsilon = Vision.MATH_DEFAULT_EPSILON);

  /// \brief Checks whether this matrix is the identity (within some epsilon).
  /// \param epsilon (\b optional) Specify an epsilon range for this range (default is Vision.MATH_DEFAULT_EPSILON).
  /// \return true if proofen as identity matrix (within the epsilon), otherwise false.
  boolean isIdentity (number epsilon = Vision.MATH_DEFAULT_EPSILON);
  
  /// \brief Checks whether the two matrices are identical.
  /// \param other The matrix to check.
  /// \return true if identical (without epsilon), otherwise false.
  boolean isIdentical (hkvMat3 other);
  
  /// \brief Checks whether the two matrices are equal (within some epsilon range).
  /// \param other The matrix to check.
  /// \param epsilon Specify an epsilon range for this range (e.g Vision.MATH_LARGE_EPSILON).
  /// \return true if equal within the given epsilon, otherwise false.
  boolean isEqual (hkvMat3 other, number epsilon);
  
  /// @}
  /// @name Utility Methods
  /// @{

  /// \brief
  ///   Returns the scaling of the x,y,z axis.
  hkvVec3 getScalingFactors ();
  
  /// \brief Sets the scaling of the x,y,z axis.
  /// \param scaling The scaling values of all axes as vector.
  void setScalingFactors (hkvVec3 scaling);
  
  /// \brief Sets the scaling of the x,y,z axis.
  /// \param x Scaling along the x axis.
  /// \param y Scaling along the y axis.
  /// \param z Scaling along the z axis.
  void setScalingFactors (number x, number y, number z);

  /// \brief Transposes the matrix.
  void transpose ();
  
  /// \brief Get a copy of a transposed matrix.
  /// \return The transposed matrix.
  hkvMat3 getTransposed ();

  /// \brief Inverts this matrix. Works for all (mathematically) invertible matrices.
  /// \note Will do a full inversion of the whole matrix. Can invert all types of matrices, that
  ///   are mathematically invertible. A matrix multiplied with its inverse will always yield the
  ///   identity (within some epsilon range). If the matrix could not be inverted the values of
  ///   the matrix are undefined.
  /// \return true if operation was successful, otherwise false (in this case the matrix values are undefined!).
  boolean invert();
   
  /// \brief Returns the x,y,z values on the specified axis as hkvVec3.
  /// \param axis The axis to get: 0, 1 or 2.
  /// \return A hkvVec3 representing the requested axis.
  hkvVec3 getAxis (number axis);
  
  /// \brief Sets the x,y,z values as hkvVec3 on the specified axis.
  /// \param axis The axis to set: 0, 1 or 2.
  /// \param vector The new axis values as vector.
  void setAxis (number axis, hkvVec3 vector);
  
  /// \brief Sets the x,y,z values on the specified axis.
  /// \param axis The axis to set: 0, 1 or 2.
  /// \param x The x axis component.
  /// \param y The y axis component.
  /// \param z The z axis component.
  void setAxis (number axis, number x, number y, number z);

  /// \brief Returns the matrix-transformed vector.
  /// \note Since a 3x3 matrix does not have any translation part, it can only rotate or scale a vector!
  /// \param vector The vector to be multiplied by the matrix.
  /// \return The transformed vector.
  hkvVec3 transformDirection (hkvVec3 vector);

  /// \brief Clones the current matrix.
  /// \return An identical hkvMat3;
  /// \note Since all wrappers classes are handled as pointers in Lua you
  ///   cannot copy it's content by assignment. Please take a look at the code samples.
  /// \par Example
  ///   \code
  ///     -- create a new matrix:
  ///     local m1 = Vision.hkvMat3(1,0,0, 0,1,0, 0,0,1)
  ///
  ///     -- this will not copy the content of m1 (just another pointer to it):
  ///     local m2 = m1
  ///
  ///     -- this will affect access to m1 and m2 since the pointers share the same object:
  ///     m2[5] = 3
  ///     -- this assertion will not fail
  ///     Vision.Assert(m1==m2, "When this assertions fails 'm1' and 'm2' are pointing to different matrices.")
  ///
  ///     -- in order to copy the content to another matrix, you have to clone:
  ///     local m3 = m1:clone()
  ///
  ///     -- this will affect only m3:
  ///     m3[5] = 6
  ///     -- this assertion will not fail
  ///     Vision.Assert(m1~=m3, "When this assertions fails 'm1' and 'm3' are pointing to same matrix.")
  ///
  ///     -- instead of clone you can also create a new matrix using the copy constructor:
  ///     local m4 = Vision.hkvMat3(m1)
  ///   \endcode
  hkvMat3 clone();

  /// @}
  /// @name Operators and Math Methods
  /// @{
  
  /// \brief Returns a matrix that is the result of multiplying this matrix with another matrix (right-hand side).
  /// \note Prefer this function for matrix multiplications. Try not to use the * operator.
  ///   The * operator currently acts like multiplyReverse, since this is the old math classes.
  ///   At some point that operator will become deprecated. You can then use 'multiplyReverse' instead or rewrite formulas to use 'multiply'.
  ///   Therefore when writing new code it is recommended to use function calls instead of overloaded operators, at least for now.
  /// \param other The right-hand side matrix.
  /// \return The result of the multiplication.
  /// \see hkvMat3::multiplyReverse
  hkvMat3 multiply (hkvMat3 other);
  
  /// \brief Returns a matrix that is the result of multiplying another (right-hand side) with this matrix.
  /// \note Try not to use the * operator.The * operator currently acts like multiplyReverse, since this is the
  ///   old math classes. At some point that operator will become deprecated. You can then use 'multiplyReverse'
  ///   instead or rewrite formulas to use 'multiply'. Therefore when writing new code it is recommended to use
  ///   function calls instead of overloaded operators, at least for now.
  /// \param other The right-hand side matrix.
  /// \return The result of the 'reverse' multiplication.
  /// \see hkvMat3::multiply
  hkvMat3 multiplyReverse (hkvMat3 other);
  
  /// \brief Operator == for hkvMat3 comparison.
  /// \param other The other matrix to compare with.
  /// \return true if the matrices are equal within Vision.MATH_DEFAULT_EPSILON, otherwise false.
  boolean __eq (hkvMat3 other);
    
  /// \brief Operator + for hkvMat3, which adds the two matrices component-wise.
  /// \param other The matrix to add.
  /// \return The result matrix.
  hkvMat3 __add (hkvMat3 other);

  /// \brief Operator - for hkvMat3, which subtracts the two matrices component-wise.
  /// \param other The matrix to subtract.
  /// \return The result matrix.
  hkvMat3 __sub (hkvMat3 other);
  
  /// \brief Operator * to multiply two matrices. The behavor of this multiplication depends on the underlying C++ implmentation of the operator!
  /// \note You can just use this operator when the VisionEnginePlugin was build with 'HKVMATH_ENABLE_NEW_OPERATORS'!
  ///   Otherwise please use hkvMat3::multiplyReverse and hkvMat3::multiply for matrix multiplication!
  /// \param other The matrix to multiply with.
  /// \return The result of the multiplication.
  /// \see hkvMat3::multiplyReverse
  /// \see hkvMat3::multiply
  hkvMat3 __mul (hkvMat3 other);
  
  /// \brief Operator * to multiply each element with a scalar value.
  /// \param value The scalar value to multiply with.
  /// \return The result matrix.
  hkvMat3 __mul (number value);
    
  /// \brief Operator / to devide each element by a scalar value.
  /// \param value The scalar value used for the division.
  /// \return The result matrix.
  hkvMat3 __div (number value);
  
  /// \brief Returns a specific element from the matrix in column-major (local elem6 = mat[6]).
  /// \param element The index, which element should be accessed (0 to 8).
  /// \return The value of the requested element.
  number __getitem(number element);
  
  /// \brief Sets a specific element of the matrix in column-major (mat[6] = 5.343).
  /// \param element The index, which element should be accessed.
  /// \param value The value to be set at the specified element index.
  void __setitem(number element, number value);
    
  /// @}
};

#endif