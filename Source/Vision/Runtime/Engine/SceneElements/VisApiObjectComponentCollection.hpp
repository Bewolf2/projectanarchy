/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiObjectComponentCollection.hpp

#ifndef VISAPIOBJECTCOMPONENTCOLLECTION_HPP_INCLUDED
#define VISAPIOBJECTCOMPONENTCOLLECTION_HPP_INCLUDED

class IVObjectComponent;

// defines the number of elements that VObjectComponentCollection can have without re-allocation
#define COMPONENTCOLLECTIONSIZE_NOALLOC 1

/// \brief
///   Collection with reference counted object component instances (IVObjectComponent)
class VObjectComponentCollection
{
public:

  ///
  /// @name Collection Functions
  /// @{
  ///


  /// \brief
  ///   Constructor
  VISION_APIFUNC VObjectComponentCollection();

  /// \brief
  ///   Destructor
  inline ~VObjectComponentCollection()
  {
    Clear();
  }

  /// \brief
  ///   Clears the collection and releases the reference counter of all objects
  VISION_APIFUNC void Clear();

  /// \brief
  ///   Removes the entry at specified index and releases its reference. The index must be in valid
  ///   range.
  VISION_APIFUNC void RemoveAt(int iIndex);

  /// \brief
  ///   Removes the entry from the collection and releases its reference. The collection must
  ///   contain the object
  inline void Remove(IVObjectComponent *pElement) 
  {
    RemoveAt(IndexOf(pElement));
  }

  /// \brief
  ///   If the collection contains the passed element, it removes the entry from the collection and releases its reference.
  inline void SafeRemove(IVObjectComponent *pElement) 
  {
    int idx = IndexOf(pElement);
    if (idx>=0)
      RemoveAt(idx);
  }


  /// \brief
  ///   Returns the current array of pointers
  inline IVObjectComponent** GetPtrs() const
  {
    return (m_iElementCount>COMPONENTCOLLECTIONSIZE_NOALLOC) ? m_Elements.GetDataPtr() : (IVObjectComponent **)m_ElementsNoAlloc;
  }

  /// \brief
  ///   Returns the current number of elements
  inline int Count() const {return m_iElementCount;}

  /// \brief
  ///   Returns the element at specified index. The index must be in valid range.
  inline IVObjectComponent* GetAt(int iIndex) const 
  {
    VASSERT(iIndex>=0 && iIndex<m_iElementCount);
    return GetPtrs()[iIndex];
  }

  /// \brief
  ///   Returns the index of the specified component, or -1 if it is not in this collection
  inline int IndexOf(IVObjectComponent *pElement) const
  {
    IVObjectComponent** pPtrs = GetPtrs();
    for (int i=0;i<m_iElementCount;i++)
      if (pElement==pPtrs[i])
        return i;
    return -1;
  }

  /// \brief
  ///   Same as IndexOf(pElement)>=0
  inline bool Contains(IVObjectComponent *pElement) const
  {
    return IndexOf(pElement)>=0;
  }

  /// \brief
  ///   Adds the passed element to the collection and increases its reference
  VISION_APIFUNC void Add(IVObjectComponent *pElement);

  /// \brief
  ///   Same as the following statement: if (!Contains(pElement)) Add(pElement);
  inline void AddUnique(IVObjectComponent *pElement)
  {
    if (!Contains(pElement))
      Add(pElement);
  }

  /// \brief
  ///   Makes sure the collection reserves enough memory for iRequiredSize elements, so no
  ///   re-allocation takes place in Add()
  VISION_APIFUNC void EnsureCapacity(int iRequiredSize);

  ///
  /// @}
  ///

  ///
  /// @name Specialized Collection Functions
  /// @{
  ///

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) with the exact
  ///   specified type
  template<typename T>
  T* GetComponentOfType() const
  {
    return static_cast<T*>(GetComponentOfType(T::GetClassTypeId()));
  }

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) with the exact
  ///   specified type
  inline IVObjectComponent* GetComponentOfType(VType *pType) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) with the exact
  ///   specified type name and component name. Be aware that this method will not use cached results.
  inline IVObjectComponent* GetComponentOfTypeAndName(VType *pType, const char *szComponentName) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) with the exact
  ///   specified type name
  ///
  /// Use the overloaded function with the VType parameter for faster access.
  ///
  /// \param szTypeName
  ///   Class name (as used in RTTI) of the component to find
  VISION_APIFUNC IVObjectComponent* GetComponentOfType(const char *szTypeName) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) with the exact
  ///   specified type name and component name. Be aware that this method will not use cached results.
  ///
  /// Use the overloaded function with the VType parameter for faster access.
  ///
  /// \param szTypeName
  ///   Class name (as used in RTTI) of the component to find
  /// \param szComponentName
  ///   The name of the component
  VISION_APIFUNC IVObjectComponent* GetComponentOfTypeAndName(const char *szTypeName, const char *szComponentName) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) of specified type (or
  ///   derived class).
  template<typename T>
  T* GetComponentOfBaseType() const
  {
    return static_cast<T*>(GetComponentOfBaseType(T::GetClassTypeId()));
  }

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) of specified type (or
  ///   derived class).
  inline IVObjectComponent* GetComponentOfBaseType(VType *pBaseType) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) of specified type name
  ///  (or derived class) and component name. Be aware that this method will not use cached results.
  inline IVObjectComponent* GetComponentOfBaseTypeAndName(VType *pType, const char *szComponentName) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) of specified type
  ///   name (or derived class).
  ///   
  /// Use the overloaded function with the VType parameter for faster access.
  ///   
  /// \param szTypeName
  ///   Class name (as used in RTTI) of the component to find
  VISION_APIFUNC IVObjectComponent* GetComponentOfBaseType(const char *szTypeName) const;

  /// \brief
  ///   Loops through the collection and returns the first instance (or NULL) of specified type
  ///   name (or derived class) and component name. Be aware that this method will not use cached results.
  ///   
  /// Use the overloaded function with the VType parameter for faster access.
  ///   
  /// \param szTypeName
  ///   Class name (as used in RTTI) of the component to find
  /// \param szComponentName
  ///   The name of the component
  VISION_APIFUNC IVObjectComponent* GetComponentOfBaseTypeAndName(const char *szTypeName, const char *szComponentName) const;

  /// \brief
  ///   Loops through the collection and returns the first component (or NULL) that has the passed ID
  VISION_APIFUNC IVObjectComponent* GetComponentByID(int iID, VType *pBaseType=NULL) const;

  /// \brief
  ///   Implements serialization for all objects inside this collection. Checks each for
  ///   VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE flag
  VISION_APIFUNC void SerializeX( VArchive &ar );



  // serialization
  V_DECLARE_SERIALX( VObjectComponentCollection, VISION_APIFUNC )

  ///
  /// @}
  ///

private:
  friend class VisTypedEngineObject_cl;

  // yet unexposed, since these functions do not go via VisTypedEngineObject_cl::RemoveComponent. Rather add it for VisTypedEngineObject_cl
  int RemoveComponentsOfType(VType *pType);
  int RemoveComponentsOfBaseType(VType *pBaseType);

  // cached entry for faster access:
  mutable IVObjectComponent *m_pLastAccessedComp;
  mutable VType *m_pLastAccessedType;

  int m_iElementCount;
  DynArray_cl<IVObjectComponent *>m_Elements;
  IVObjectComponent* m_ElementsNoAlloc[COMPONENTCOLLECTIONSIZE_NOALLOC];
};

#endif  // VISAPIOBJECTCOMPONENT_HPP_INCLUDED

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
