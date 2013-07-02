/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VWeakPtr.hpp

#ifndef fr_VWEAKPTR_HPP
#define fr_VWEAKPTR_HPP

#include <Vision/Runtime/Base/System/Memory/VBaseMem.hpp>
#include <Vision/Runtime/Base/System/VRefTarget.hpp>



/// \brief
///   Weak reference target class that is used by weak pointers to retrieve a valid pointer to the
///   referenced object.
/// 
/// The VWeakRefTarget class template has the purpose of providing a valid pointer to a referenced
/// object. Since the weak reference target is referenced counted, the weak pointer instances use
/// the target to retrieve a valid pointer to the referenced object.
/// 
/// Any class which wants to support weak pointers has to provide a weak reference target instance
/// and has to reset the reference target when the object gets destroyed (VWeakRefTarget::Reset).
/// Since the ref target is ref counted, it is important that the object holds a smart pointer to the ref target as well.
/// In the sample code below it uses the nested templates VSmartPtr<VWeakRefTarget<MyObject> > m_spWeakRefTarget to accomplish that.
/// 
/// \sa VWeakPtr
/// 
/// \example
///   \code
///   // sample class which offers weak reference functionality
///   class MyObject
///   {
///   public:
///     MyObject::MyObject()
///     {
///       // m_spWeakRefTarget = NULL; // not required since it is a smart pointer
///     }
///
///     VWeakRefTarget<MyObject>* MyObject::GetWeakReference()
///     {
///       if (!m_spWeakRefTarget)
///       {
///         //Create the weak reference target
///         //It will be reference counted and shared between all objects that reference this
///         m_spWeakRefTarget = new VWeakRefTarget<MyObject>(this);
///       }
///       return m_spWeakRefTarget;
///     }
///     MyObject::~MyObject()
///     {
///       //reset the weak ref itself
///       if (m_spWeakRefTarget)
///        m_spWeakRefTarget->Reset();
///     }
///     VSmartPtr<VWeakRefTarget<MyObject> > m_spWeakRefTarget;
///   };
///   \endcode
template<class C> class VWeakRefTarget : public VRefTarget
{
public:

  /// \brief
  ///   Construct a new weak reference target to a specific object
  VWeakRefTarget(C *pRef = NULL)
  {
    m_pRef = pRef;
  }
  
  /// \brief
  ///   Returns the latest pointer of the object that the weak reference target points to 
  C* GetReference()
  {
    return m_pRef;
  }
  
  
  /// \brief
  ///   Sets the pointer to the referenced object to NULL. Call this function when the referenced
  ///   object gets deleted
  void Reset()
  {
    m_pRef = NULL;
  }
  
protected:
  C* m_pRef;  ///< pointer to the object
};



/// \brief
///   Weak pointer class that stores a weak reference.
/// 
/// The weak pointer class template stores a "weak reference" to an object. In comparison to usual
/// pointers a weak pointer returns NULL if the referenced object has been destroyed.
/// 
/// The weak pointer uses a weak reference target for retrieving the latest pointer to the object.
/// You can assign a pointer to a weak reference target class to the weak pointer and use it
/// exactly like you used the original object pointer. See VWeakRefTarget for more information
/// about setting up your own weak reference target.
/// 
/// \sa VWeakRefTarget
/// 
/// \example
///   \code
///   // use a weak pointer like a usual pointer
///   VWeakPtr<MyObject> m_wpRef = pObject->GetWeakReference();
///   m_wpRef->DoSomething();
///   // m_wpRef will return NULL if the object was deleted
///   delete pObject;
///   ASSERT( m_wpRef == NULL ); 
///   \endcode
template<class C> class VWeakPtr
{
public:

  /// \brief
  ///   Constructs a new weak reference to a specific object (specified via the reference target).
  VWeakPtr(VWeakRefTarget<C> *ptr = NULL)
  {
    m_sptr = ptr;
  }
  
  /// \brief
  ///   Returns a valid pointer to the referenced object (NULL if object got deleted).
  C* GetPtr() const
  {
    VWeakRefTarget<C>* t = m_sptr.GetPtr();
    if (!t)
      return NULL;
    return t->GetReference();
  }

  /// \brief
  ///   Assignment operator for the weak pointer from an reference target.
  VWeakPtr<C>& operator =(VWeakRefTarget<C> *ptr)
  {
    m_sptr = ptr;
    return *this;
  }
  
  /// \brief
  ///   Returns a valid pointer to the referenced object (NULL if object got deleted).
  operator C*() const
  {
    return GetPtr();
  }

  /// \brief
  ///   Returns a valid pointer to the referenced object (NULL if object got deleted).
  C* operator ->() const
  {
    return GetPtr();
  }

protected:
  VSmartPtr<VWeakRefTarget<C> > m_sptr;
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
