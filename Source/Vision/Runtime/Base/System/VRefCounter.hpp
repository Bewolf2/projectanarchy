/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VRefCounter.hpp

#ifndef DC_VREFCOUNTER_HPP
#define DC_VREFCOUNTER_HPP

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>
#include <Vision/Runtime/Base/System/Threading/Atomic/VAtomic.hpp>


/// \brief
///   Class that provides common functionality for reference counting.
/// 
/// Reference counting allows you to pass object pointers to the engine that can be cleaned up by
/// the engine if no more references to it exist.
/// 
/// \sa VSmartPtr 
class VRefCounter : public VBaseObject
{
public:
  /// \brief
  ///   constructor; initialises the counter.
  VRefCounter()
  {
    m_iRefCount = 0;
  }
  
  /// \brief
  ///   Copy constructor; restarts new object with a refcount of 0.
  VRefCounter( const VRefCounter &other )
  {
    m_iRefCount = 0;
  }

  /// \brief
  ///   destructor; asserts when the refcounter is not 0.
  virtual ~VRefCounter()
  {
    VASSERT_MSG(m_iRefCount==0 , "Tried to delete a reference counted object with refcount!=0")
  }
  
  /// \brief
  ///   Special version of assignment operator that leaves the refcount unmodified.
  VRefCounter& operator=( const VRefCounter& other)
  {
    return *this;
  }


  /// \brief
  ///   Overridable function that is called when the refcounter reaches zero. The default
  ///   implementation calls "delete this".
  virtual void DeleteThis()
  {
    delete this;
  }

  /// \brief
  ///   Increases the reference counter, should be called when any object receives a pointer to a
  ///   reference counted object.
  ///
  /// \returns
  ///  The new ref count.
  inline unsigned long AddRef()
  {
    return VAtomic::Increment(m_iRefCount);;
  }

  /// \brief
  ///   Decreases the reference counter, should be called when objects remove their reference to
  ///   this object.
  /// 
  /// Checks that the counter isn't already zero. If this was the last reference, the
  /// object is deleted.
  ///
  /// \returns
  ///  The new ref count.
  inline unsigned long Release()
  {
    int iRefCount = ReleaseNoDelete();
    if (iRefCount == 0)
    {
      DeleteThis();
    }
    return iRefCount;
  }

  /// \brief
  ///   Decreases the reference counter, but does not delete this object if the ref counter
  ///   reaches 0.
  /// 
  /// Checks that the counter isn't already already zero.
  ///
  /// \returns
  ///  The new ref count.
  inline int ReleaseNoDelete()
  {
    VASSERT(m_iRefCount>0);
    return VAtomic::Decrement(m_iRefCount);
  }

  /// \brief
  ///   Gets the current number of references. Only for internal/debugging purposes.
  inline int GetRefCount() const
  { 
    return m_iRefCount;
  }

protected:

  int m_iRefCount;  ///<reference count

};



/// \brief
///   Smart pointer class that helps with reference counting.
/// 
/// You can assign a pointer to a reference counted class to the smart pointer and use it exactly
/// like you used the original object pointer.
/// 
/// This class automatically takes care of AddRef/Release for us during assignment or destruction.
/// Since the API makes extensive use of smart pointers, there is a dedicated chapter about reference counting in the Programmer's Documentation.
/// 
/// \sa VRefCounter
/// 
/// \example
///   \code
///   //New way with reference counting
///   VSmartPtr<MyObject> spObject = new MyObject();
///   spObject->DoSomething();
///   //Old way without reference counting
///   MyObject* pObject = new MyObject();
///   pObject->DoSomething();
///   \endcode
template<class C> class VSmartPtr
{
public:

  /// \brief
  ///   Empty constructor. Object pointer is set to NULL.
  inline VSmartPtr()
  {
    m_pPtr = NULL;
  }
  
  /// \brief
  ///   Constructor that takes an object reference. For pPtr!=NULL, the reference count of the object is incremented.
  ///
  /// \param pPtr
  ///   Object reference that should be set. Can be NULL
  inline VSmartPtr(C* pPtr)
  {
    m_pPtr = NULL;
    Set(pPtr);
  }

  /// \brief
  ///   Copy constructor. Initializes this smart pointer with the same reference as the passed smart pointer. Reference is increased.
  ///
  /// \param other
  ///   Object reference that should be set.
  inline VSmartPtr(const VSmartPtr<C>& other)
  {
    m_pPtr = NULL;
    Set(other.GetPtr());
  }
  
  /// \brief
  ///   Destructor. Decreases the reference counter so the object can be deleted eventually (if nothing else holds a reference to the object).
  inline ~VSmartPtr()
  {
    if (m_pPtr)
      m_pPtr->Release();
  }


  /// \brief
  ///   Assignment operator for the smart pointer.
  ///
  /// \param other
  ///   New Object reference that should be set
  ///
  /// \returns
  ///   This instance
  ///
  /// This assignment gracefully increases the reference of the new object and decreases the reference of the old instance
  inline VSmartPtr<C>& operator=(const VSmartPtr<C> &other)
  {
    Set(other.GetPtr());
    return *this;
  }

  /// \brief
  ///   Assignment operator for the smart pointer.
  ///
  /// \param pPtr
  ///   New Object reference that should be set
  ///
  /// \returns
  ///   This instance
  ///
  /// This assignment gracefully increases the reference of the new object and decreases the reference of the old instance
  inline VSmartPtr<C>& operator=(C *pPtr)
  {
    Set(pPtr);
    return *this;
  }

  /// \brief
  ///   Assigns an object to the smart pointer.
  ///
  /// \param pPtr
  ///   New Object reference that should be set
  ///
  /// This assignment gracefully increases the reference of the new object and decreases the reference of the old instance
  inline void Set(C *pPtr)
  {
    if (m_pPtr == pPtr) // avoid unnecessary AddRef/Release calls (and ref counter changes when data breakpoints are set)
      return;
    C* pOldPtr = m_pPtr;
    m_pPtr = pPtr;
    if (pPtr)
      pPtr->AddRef();
    if (pOldPtr)
      pOldPtr->Release();
  }

  /// \brief
  ///   Returns the pointer of the object that the smart pointer points to.
  inline operator C*() const
  {
    return m_pPtr;
  }

  /// \brief
  ///   Returns the pointer of the object that the smart pointer points to.
  inline C* operator->() const
  {
    return m_pPtr;
  }

  /// \brief
  ///   Returns the pointer of the object that the smart pointer points to.
  inline C* GetPtr() const
  {
    return m_pPtr;
  }


  C* m_pPtr;  ///< pointer to the object
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
