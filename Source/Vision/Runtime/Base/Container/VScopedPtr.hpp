/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_SCOPED_PTR_HPP
#define V_SCOPED_PTR_HPP

/// \brief 
///  RAII pointer container, comparable to C++'s std::unique_ptr.
///
/// When the instance of this object is destroyed, the object the scoped pointer points to is deleted.
template <typename T>
class VScopedPtr
{
private:
  T* m_pPtr;

  VScopedPtr(VScopedPtr const& rh); //disable copy constructor
  void operator=(VScopedPtr const& rh); //disable assignment operator

public:

  /// \brief
  ///  Default constructor
  /// 
  /// Initializes the wrapped pointer with NULL.
  VScopedPtr()
  {
    m_pPtr = NULL;
  }


  /// \brief
  ///  Constructor
  /// 
  /// \param pPtr
  ///  Pointer to the target object on the heap
  explicit VScopedPtr(T* pPtr)
  {
    m_pPtr = pPtr;
  }

  /// \brief
  ///  Destructor, will call delete on the wrapped pointer
  ~VScopedPtr()
  {
    if(m_pPtr != NULL)
      delete m_pPtr;
  }

  /// \brief
  ///  Deletes the pointed-to object and assigns a new pointer.
  ///
  /// \param pPtr
  ///  The new pointer to hold.
  VScopedPtr& operator=(T* pPtr)
  {
    delete m_pPtr;
    m_pPtr = pPtr;
    return* this;
  }

  /// \brief
  ///  Returns the wrapped pointer.
  /// 
  /// \return
  ///  The wrapped pointer
  inline T* Get()
  {
    return m_pPtr;
  }

  /// \brief
  ///  Returns the wrapped pointer
  /// 
  /// \return
  ///  The wrapped pointer
  inline T const* Get() const
  {
    return m_pPtr;
  }

  /// \brief 
  ///   Returns the held reference and sets the internal pointer to NULL. DOES NOT delete the object.
  T* Release()
  {
    T* m_pTemp = m_pPtr;
    m_pPtr = NULL;
    return m_pTemp;
  }

  /// \brief
  ///  Dereferencing operator.
  T* operator->()
  {
    return m_pPtr;
  }

  /// \brief
  ///  Dereferencing operator.
  T const* operator->() const
  {
    return m_pPtr;
  }

  /// \brief
  ///  Dereferencing operator.
  T& operator*()
  {
    return *m_pPtr;
  }

  /// \brief
  ///  Dereferencing operator.
  T const& operator*() const
  {
    return *m_pPtr;
  }

  /// \brief
  ///   Returns the pointer of the object that the pointer points to.
  inline operator T*() const
  {
    return m_pPtr;
  }
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
