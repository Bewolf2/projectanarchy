/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VElementCache.hpp

#ifndef VELEMENTCACHE_HPP_INCLUDED
#define VELEMENTCACHE_HPP_INCLUDED



/// \brief
///   Template class for managing a list of cached elements.
/// 
/// Re-using objects by keeping a list of cached instances is a useful concept to avoid memory
/// fragmentation during runtime.
/// 
/// Template parameters are the class type of the instances and the size of the cache.
/// 
/// The cache has a static size.
/// 
/// The user can request new objects or remove objects analogous to the 'new' and 'delete' operator.
/// 
/// If the user requests an object it is either taken from the cache or created via the pure
/// virtual AllocateObject function. Thus object creation and delete functions have to be
/// implemented by derived classes.
template<class TCLASS, int CACHE_SIZE> class IVElementCache
{
public:

  /// \brief
  ///   Constructor
  inline IVElementCache()
  {
    m_iCachedCount = 0;
  }

  /// \brief
  ///   Destructor
  inline virtual ~IVElementCache()
  {
    VASSERT_MSG(m_iCachedCount==0,"Clear cache before destroying");
  }

  /// \brief
  ///   Clears the cache, i.e. destroys all remaining objects in it. Same as ShrinkToSize(0)
  inline void Clear()
  {
    ShrinkToSize(0);
  }

  /// \brief
  ///   Returns the current number of valid elements in the cache
  inline int GetFillSize() const 
  {
    return m_iCachedCount;
  }

  /// \brief
  ///   Returns the maxuimum number of elements in the cache. Naturally this is the template size
  ///   parameter.
  inline int GetMaxFillSize() const 
  {
    return CACHE_SIZE;
  }

  /// \brief
  ///   Removes objects until the cache contains at most iSize elements.
  inline void ShrinkToSize(int iSize)
  {
    VASSERT(m_iCachedCount <= CACHE_SIZE);
    for (int i=iSize;i<m_iCachedCount;i++)
      DeleteObject(m_pElements[i]);
    m_iCachedCount = hkvMath::Min (iSize,m_iCachedCount);
  }

  /// \brief
  ///   Guarantees that the cache contains at least iSize elements. Passing a negative value fills
  ///   the entire cache
  inline void FillCache(int iSize=-1)
  {
    if (iSize<0 || iSize>CACHE_SIZE)
      iSize = CACHE_SIZE;

    for (int i=m_iCachedCount;i<iSize;i++)
      m_pElements[i] = AllocateObject();
    m_iCachedCount = hkvMath::Max (iSize,m_iCachedCount);
  }

  /// \brief
  ///   This function is called by client code to remove an instance
  /// 
  /// If the cache is not yet full, the passed object will be added to the cache, otherwise it is
  /// deleted via the pure virtual DeleteObject function.
  /// 
  /// When the object is added to the cache, the virtual OnAddedToCache function is called so
  /// additional initialization can take place. 
  ///
  /// \param pElem
  ///   Element to remove.
  inline void RemoveInstance(TCLASS pElem)
  {
    if (m_iCachedCount<CACHE_SIZE)
    {
      m_pElements[m_iCachedCount++] = pElem;
      OnAddedToCache(pElem);
    }
    else
      DeleteObject(pElem);
  }

  /// \brief
  ///   This function is called by client code to retrieve a new instance (either from cache or a new
  ///   instance)
  /// 
  /// In case the cache does not contain any elements, a new instance is created via the pure virtual
  /// AllocateObject function.
  /// 
  /// Before the object is returned from cache, the virtual OnRemovedFromCache function is called
  /// so additional initialization can take place.
  inline TCLASS GetNewInstance()
  {
    if (m_iCachedCount>0)
    {
      m_iCachedCount--;
      OnRemovedFromCache(m_pElements[m_iCachedCount]);
      return m_pElements[m_iCachedCount];
    }
    return AllocateObject();
  }

protected:

  /// \brief
  ///   Pure virtual function that must be implemented to delete an instance. 
  ///   This function is called when the cache is full or it is cleared.
  virtual void DeleteObject(TCLASS pElem) = 0;

  /// \brief
  ///   Pure virtual function that must be implemented to create a new instance.
  ///   This function is called when the user requests a new object but the cache is empty.
  virtual TCLASS AllocateObject() = 0;

  /// \brief
  ///   Overridable that is called when an instance is added to the cache.
  ///   This callback can be used for class-specific initialization, e.g. increasing the refcount.
  virtual void OnAddedToCache(TCLASS pElem) {}

  /// \brief
  ///   Overridable that is called when an instance is removed from the cache
  ///   This callback can be used for class-specific deinitialization, e.g. releasing the reference
  ///   for refcounted objects.
  virtual void OnRemovedFromCache(TCLASS pElem) {}

  int m_iCachedCount;
  TCLASS m_pElements[CACHE_SIZE];
};


#endif // VELEMENTCACHE_HPP_INCLUDED

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
