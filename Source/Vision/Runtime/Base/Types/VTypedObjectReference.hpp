/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTypedObjectReference.hpp

#ifndef VTYPED_OBJECT_REFERENCE_HPP
#define VTYPED_OBJECT_REFERENCE_HPP

  /// \class  VTypedObjectReference
  ///
  /// \brief  Data type to hold an object reference and a valid type for objects hold by this data type. 
  ///
  class VTypedObjectReference
  {

    public:

      /// \fn VBASE_IMPEXP VTypedObjectReference()
      ///
      /// \brief Default constructor. 
      ///
      VBASE_IMPEXP VTypedObjectReference();

      /// \fn VBASE_IMPEXP VTypedObjectReference(VTypedObject* pObject)
      ///
      /// \brief  Constructor taking an initial value for the referenced object. 
      ///
      /// \param [in] pObject The initial value for the object reference. 
      ///
      VBASE_IMPEXP VTypedObjectReference(VTypedObject* pObject);

      /// \fn VBASE_IMPEXP VTypedObjectReference(VTypedObject* pObject, VType* pType)
      ///
      /// \brief  Constructor taking an initial value for the referenced object and an initial value for the valid type of assignable objects.
      ///
      /// \param [in] pObject The initial value for the object reference. 
      /// \param [in] pType   The initial value for valid types.
      ///
      VBASE_IMPEXP VTypedObjectReference(VTypedObject* pObject, VType* pType);

      /// \fn VBASE_IMPEXP bool CanAssign(VType* pType)
      ///
      /// \brief  
      ///   Queries if objects of type 'pType' can be assigned to this reference object. 
      ///
      /// \param 
      ///   pType The type which should be queried.
      ///
      /// \return 
      ///   true if it is a valid type of object to assign to this reference, false if not. 
      VBASE_IMPEXP bool CanAssign(VType* pType);

      /// \fn VBASE_IMPEXP bool CanAssign(VTypedObject* pObject)
      ///
      /// \brief  Queries if the object 'pObject' is assignable to this reference object. Note: NULL is always assignable.
      ///
      /// \param pObject 
      ///   The object to test. 
      ///
      /// \return 
      ///   true if it is a valid type of object to assign to this reference, false if not. 
      VBASE_IMPEXP bool CanAssign(VTypedObject* pObject);

      /// \fn inline VType* GetAllowedType() const
      ///
      /// \brief  Gets the allowed type of objects stored in this reference. 
      ///
      /// \return The allowed type for objects. 
      ///
      inline VType* GetAllowedType() const
      {
        return m_pAllowedType;
      }

      /// \fn inline void SetAllowedType(VType* pType)
      ///
      /// \brief  Sets the allowed type for object. 
      ///
      /// \param [in] pType The valid type for objects. 
      ///
      inline void SetAllowedType(VType* pType)
      {
        VASSERT(pType);
        m_pAllowedType = pType;
      }

      /// \fn inline VTypedObject* GetReferencedObject() const
      ///
      /// \brief  Gets the referenced object. 
      ///
      /// \return The referenced object. 
      ///
      inline VTypedObject* GetReferencedObject() const
      {
        return m_pObjectReference;
      }

      /// \fn inline void SetReferencedObject(VTypedObject* pObject)
      ///
      /// \brief  Sets the referenced object. This asserts in debug mode when the object is not of the valid type or derived from it.
      ///
      /// \param [in] pObject The object. 
      ///
      inline void SetReferencedObject(VTypedObject* pObject)
      {
        if(pObject)
        {
          VVERIFY_OR_RET(pObject->IsOfType(m_pAllowedType));
        }
        m_pObjectReference = pObject;
      }

      /// \fn inline void operator = (VTypedObject* pObject)
      ///
      /// \brief  Assignment operator. Helper operator which calls the SetReferencedObject() method.
      ///
      /// \param [in] pObject The object. 
      ///
      inline void operator = (VTypedObject* pObject)
      {
        SetReferencedObject(pObject);
      }

    protected:

      //! Stores a pointer to the referenced object. 
      VTypedObject* m_pObjectReference;

      //! Stores the allowed type for objects. 
      VType* m_pAllowedType;
  };

  /// \fn VBASE_IMPEXP VArchive& operator<<(VArchive& ar, const VTypedObjectReference& ObjectReference)
  ///
  /// \brief  Serialising operator.
  ///
  /// \param [in] ar              the archive. 
  /// \param [in] ObjectReference the object reference. 
  ///
  /// \return The archive. 
  ///
  VBASE_IMPEXP VArchive& operator<<(VArchive& ar, const VTypedObjectReference& ObjectReference);

  /// \fn VBASE_IMPEXP VArchive& operator>>(VArchive& ar, VTypedObjectReference& ObjectReference)
  ///
  /// \brief  Deserializing operator.
  ///
  /// \param [in] ar              the archive. 
  /// \param [out] ObjectReference the object reference. 
  ///
  /// \return The archive. 
  ///
  VBASE_IMPEXP VArchive& operator>>(VArchive& ar, VTypedObjectReference& ObjectReference);

  /// \brief
  ///  Template wrapper for VTypedObjectReference to provide type safe reference
  template<class T>
  class VObjectReference : public VTypedObjectReference
  {
  public:
    /// \brief
    ///  Default constructor
    VObjectReference() : VTypedObjectReference(NULL, T::GetClassTypeId()) {}

    /// \brief
    ///   Constructor
    ///
    /// \param pObject
    ///   the object to initialize the reference with
    VObjectReference(T* pObject) : VTypedObjectReference(pObject, T::GetClassTypeId()) {}

    /// \brief assignment operator
    ///
    /// \param pObject
    ///   the object to be assigned
    inline void operator = (T* pObject)
    {
      SetReferencedObject(pObject);
    }

    /// \brief cast operator
    inline operator T* ()
    {
      return static_cast<T*>(m_pObjectReference);
    }

    /// \brief -> operator
    inline T& operator -> ()
    {
      return *static_cast<T*>(m_pObjectReference);
    }

    /// \brief unary * operator
    inline T& operator * ()
    {
      return *static_cast<T*>(m_pObjectReference);
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
