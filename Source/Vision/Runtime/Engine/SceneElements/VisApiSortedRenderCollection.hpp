/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSortedRenderCollection.hpp

#ifndef DEFINE_VISAPIRENDERCOLLECTION
#define DEFINE_VISAPIRENDERCOLLECTION

#include <Vision/Runtime/Engine/SceneElements/VisApiFrustum.hpp>
//#include "VisApiVertexBuffer.hpp"
#include <Vision/Runtime/Engine/SceneElements/VisApiScreenMask.hpp>

typedef unsigned int (VISION_FASTCALL * PVISCOLLECTIONSPLITFUNC) (void *pValue);
typedef unsigned int (VISION_FASTCALL * PVISCOLLECTIONSPLITMULTIPLEFUNC) (void *pValue);

class VisMeshBufferObject_cl;
class VisStaticGeometryInstance_cl;
class VisVisibilityObject_cl;


// helper macro for typed serialization
#define V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX \
{\
  unsigned int iCount = GetNumEntries();\
  if (ar.IsLoading())\
  {\
    ar >> iCount;\
    this->EnsureSize(iCount+GetNumEntries());\
    for (unsigned int i=0;i<iCount;i++)\
      VisRenderCollection_cl::AppendEntryFast(ar.ReadObject(NULL));\
  } else\
  {\
    ar << iCount;\
    for (unsigned int i=0;i<iCount;i++)\
      ar.WriteObject((VTypedObject *)GetEntry(i));\
  }\
}


/// \brief
///   RenderCollection base class
/// 
/// The RenderCollection_cl class is the base class for a number of subclasses providing
/// functionality for handling collections of scene elements. The various subclasses are used in
/// many methods of the VisRenderLoopHelper_cl class, and will typically be found in custom render
/// loop implementations.
/// 
/// The elements in a render collection are not reference counted.
/// 
/// In general, the order of elements in a Render Collection should not be changed, since the
/// Render Collections returned by the methods in VisRenderLoopHelper_cl are frequently pre-ordered
/// for reasons of performance and visual correctness (e.g. translucent geometry). Any change in
/// the order of the elements inside a Render Collection is likely to result in a performance drop
/// or even visual artefacts.
class VisRenderCollection_cl : public VBaseObject
{
public:

  /// \brief
  ///   Constructor of the Render Collection class
  /// 
  /// \param iSize
  ///   Initial size of the render collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too large for the currently allocated size of the collection, the collection is
  ///   re-allocated. This parameter defines the size increments of each re-allocation step.
  VISION_APIFUNC VisRenderCollection_cl(unsigned int iSize=0, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the Render Collection class taking an array of void pointers as input.
  /// 
  /// The array of void pointers will be copied into the collection
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of elements to copy
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisRenderCollection_cl(unsigned int iNumEntries, void * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Destructor of the VisRenderCollection_cl class
  /// 
  /// Cleans up the collection and deallocates all the memory associated with it
  VISION_APIFUNC ~VisRenderCollection_cl();

  /// \brief
  ///   Returns the number of elements currently in the collection
  /// 
  /// (note that this is not the size of the collection!)
  /// 
  /// \return
  ///   int: Number of elements in the collection
  /// 
  /// \sa VisRenderCollection_cl::GetSize
  /// \sa VisRenderCollection_cl::IsEmpty
  VISION_APIFUNC inline unsigned int GetNumEntries() const;

  /// \brief
  ///   Returns TRUE if the collection has no entries
  /// 
  /// \return
  ///   BOOL bEmpty: TRUE, if number of elements in the collection is zero, FALSE otherwise
  /// 
  /// \sa VisRenderCollection_cl::GetNumEntries
  VISION_APIFUNC inline BOOL IsEmpty() const;

  /// \brief
  ///   Returns the element of the collection stored at the position iIndex.
  /// 
  /// Does not perform a range check, i.e. you have to make sure that the index is still within the
  /// collection.
  /// 
  /// \param iIndex
  ///   The iIndex-th element of the collection will be returned
  /// 
  /// \return
  ///   void *: The element of the collection
  /// 
  /// \sa VisRenderCollection_cl::AppendEntry
  VISION_APIFUNC inline void* GetEntry(unsigned int iIndex) const;

  /// \brief
  ///   Appends a new element with the given value to the collection.
  /// 
  /// Automatically performs a range check, i.e. if the element doesn't fit into the collection any
  /// more, the collection is resized automatically.
  /// 
  /// If you know beforehand that the element will fit into the collection, you can instead use the
  /// AppendEntryFast method.
  /// 
  /// \param pValue
  ///   The element to be appended to the collection
  /// 
  /// \sa VisRenderCollection_cl::AppendEntryFast
  /// \sa VisRenderCollection_cl::GetEntry
  VISION_APIFUNC inline void AppendEntry(void *pValue);

  /// \brief
  ///   Appends a new element to the collection without performing a range check.
  /// 
  /// Does not perform a range check, i.e. you have to make sure that the new element still fits
  /// into the collection.
  /// 
  /// This method is slightly faster than AppendEntry.
  /// 
  /// \param pValue
  ///   The element to append to the collection
  /// 
  /// \sa VisRenderCollection_cl::AppendEntry
  /// \sa VisRenderCollection_cl::GetEntry
  VISION_APIFUNC inline void AppendEntryFast(void *pValue); 
   
  /// \brief
  ///   Same as the following statement: if (!Contains(pValue)) AppendEntry(pValue); Note that this
  ///   method iterates over the collection and thus may take a substantial amount of time.
  VISION_APIFUNC inline void AppendEntryUnique(void *pValue) {if (!Contains(pValue)) AppendEntry(pValue);}

  /// \brief
  ///   Appends parts of the passed collection to this collection
  VISION_APIFUNC void AppendCollection(const VisRenderCollection_cl &other,int iFirstIndex=0, int iCount=-1);

  /// \brief
  ///   Finds the entry in this collection and returns its 0-based index. If the element is not in
  ///   the collection, -1 is returned. Note that this method iterates over the collection and thus
  ///   may take a substantial amount of time.
  VISION_APIFUNC int GetIndexOf(const void *pElement) const; 

  /// \brief
  ///   Indicates whether this collction contains the element. Same as GetIndexOf(pElement)>=0.
  ///   Note that this method iterates over the collection and thus may take a substantial amount
  ///   of time.
  VISION_APIFUNC inline BOOL Contains(const void *pElement) const {return GetIndexOf(pElement)>=0;}

  /// \brief
  ///   Removes all occurances of the element from the collection. Returns TRUE if it has been
  ///   removed at least once. Note that this method iterates over the collection and thus may take
  ///   a substantial amount of time.
  VISION_APIFUNC BOOL Remove(const void *pElement);

  /// \brief
  ///   Removes the element at the specified 0-based index. Elements after iIndex will move up.
  ///   Note that this method may take a substantial amount of time.
  VISION_APIFUNC void RemoveAt(int iIndex);


  /// \brief
  ///   Marks all occurances of pElement with a NULL entry without changing the size of the
  ///   collection. Clean up with RemoveFlagged.
  VISION_APIFUNC void FlagForRemoval(const void *pElement);

  /// \brief
  ///   Marks the element at the specified index with a NULL entry without changing the size of the
  ///   collection. Clean up with RemoveFlagged.
  VISION_APIFUNC void FlagForRemoval(int iElementAt);

  /// \brief
  ///   Cleans up the collections, i.e. eliminates all NULL elements set via FlagForRemoval. This
  ///   function is fast if there is nothing to clean up.
  VISION_APIFUNC void RemoveFlagged();


  /// \brief
  ///   Returns the current size of the collection
  /// 
  /// (i.e. the total number of elements that fit into it).
  /// 
  /// This is not the number of elements in the collection!
  /// 
  /// \return
  ///   Current size of the collection in elements
  /// 
  /// \sa VisRenderCollection_cl::GetNumEntries
  VISION_APIFUNC inline unsigned int GetSize() const;

  /// \brief
  ///   Returns the current granularity value of the collection
  /// 
  /// \return
  ///   Current granularity of the collection in number of elements
  /// 
  /// \sa VisRenderCollection_cl::SetGranularity
  VISION_APIFUNC inline unsigned int GetGranularity() const;

  /// \brief
  ///   Sets the granularity value of the collection
  /// 
  /// This is the value by which collections will be extended when being automatically resized
  /// (e.g. after a call to AppendEntry which exceeds the current size of the collection).
  /// 
  /// \param iGranularity
  ///   The new granularity value
  /// 
  /// \sa VisRenderCollection_cl::GetGranularity
  VISION_APIFUNC inline void SetGranularity(unsigned int iGranularity);

  /// \brief
  ///   Resizes the collection to the passed size.
  /// 
  /// However, the new size may not be smaller than the number of elements
  /// currently in the collection.
  /// 
  /// \param iNewSize
  ///   New size of the collection
  VISION_APIFUNC void Resize(unsigned int iNewSize);

  /// \brief
  ///   Makes sure the collection has at least the size specified by iMinSize
  /// 
  /// If the current size is larger than iMinSize, nothing will be done.
  /// 
  /// Otherwise the collection will be resized.
  /// 
  /// \param iMinSize
  ///   The minimum size 
  VISION_APIFUNC inline void EnsureSize(unsigned int iMinSize);

  /// \brief
  ///   Clears the elements in the collection by setting the internal element counter to zero.
  /// 
  /// This method does not change the memory allocated for the collection; the size of the
  /// collection remains the same.
  VISION_APIFUNC inline void Clear();            

  /// \brief
  ///   Clones the collection and copies all the relevant data
  /// 
  /// ("deep copy").
  /// 
  /// \return
  ///   VisRenderCollection_cl *pCollection: Pointer to the cloned collection
  VISION_APIFUNC VisRenderCollection_cl *Clone() const;

  /// \brief
  ///   This method splits a single collection into an arbitrary number of target collections.
  /// 
  /// The target collections are specified as an array. 
  /// 
  /// The original collection is not modified.
  /// 
  /// Splitting is performed according to the user-defined split function, which takes the element
  /// value as input and has to return the index of the desired target collection in the array.
  /// 
  /// The split function may not return a value outside of the bounds of the target collection
  /// array.
  /// 
  /// \param pTargetCollections
  ///   Array of pointers to target collections
  /// 
  /// \param pSplitFunction
  ///   user-defined split function which effectively decides which elements end up in which target
  ///   collection.
  /// 
  /// \sa VisRenderCollection_cl::SplitCollectionMultiple
  VISION_APIFUNC void SplitCollection(VisRenderCollection_cl **pTargetCollections, PVISCOLLECTIONSPLITFUNC pSplitFunction) const;

  /// \brief
  ///   This method splits a single collection into an arbitrary number of target collections
  /// 
  /// The difference in comparison with SplitCollection is that each element of the source collection
  /// may end up in multiple target collections instead of in only one.
  /// 
  /// The target collections are specified as an array of up to 32 entries.
  /// 
  /// The original collection is not modified.
  /// 
  /// Splitting is performed according to the user-defined split function, which takes the element
  /// value as input and has to return a bit mask. The bit mask is interpreted as a set of 32
  /// values, each representing one of the target collections, with the first target collection
  /// (array index 0) being represented by the least significant bit.
  /// 
  /// \param pTargetCollections
  ///   Array of up to 32 target collections
  /// 
  /// \param iNumCollections
  ///   Number of target collections
  /// 
  /// \param pSplitFunction
  ///   The user-defined split function
  /// 
  /// \sa VisRenderCollection_cl::SplitCollection
  VISION_APIFUNC void SplitCollectionMultiple(VisRenderCollection_cl **pTargetCollections, unsigned int iNumCollections, PVISCOLLECTIONSPLITMULTIPLEFUNC pSplitFunction) const;

  /// \brief
  ///   Returns a pointer to the array of elements
  /// 
  /// \return
  ///   const int *pElementArray : pointer to elements
  VISION_APIFUNC inline void **GetDataPtr() const;

  /// \brief
  ///   Removes unused (i.e. NULL) elements from the collection. Actually same as RemoveFlagged
  /// 
  /// Calling this method is relatively expensive. It may change the number of entries in the
  /// collection, though it will keep the size unmodified (i.e. memory will not be reallocated).
  VISION_APIFUNC void RemoveUnused();

  /// \brief
  ///   Sorts all pointers in this collection by ascending pointer address. Useful for comparing
  ///   two collections 
  VISION_APIFUNC void SortByPointer();

  /// \brief
  ///   Copies all entries from another collection into this collection.
  VISION_APIFUNC void CopyFrom(const VisRenderCollection_cl &pOther);

  VISION_APIFUNC void SetNumEntries(unsigned int iNumEntries);

private:
  /// \brief
  ///   Prevent the accidental use of the deep copy operator. Rather assign a reference, e.g. const VisEntityCollection_cl &coll = Vision::Game.GetThinkingEntities();
  inline VisRenderCollection_cl& operator = (const VisRenderCollection_cl &other)
  {
    // do not use this type of assignment - deep copy the collection is not supported this way
    return *this;
  }

  /// \brief
  ///   Prevent the accidental use of the deep copy constructor. Rather assign a reference, e.g. const VisEntityCollection_cl &coll = Vision::Game.GetThinkingEntities();
  inline VisRenderCollection_cl(const VisRenderCollection_cl& other)
  {
    // do not use this type of assignment - deep copy the collection is not supported this way
  }


  protected:
    unsigned int m_iNumEntries;
    unsigned int m_iSize;
    unsigned int m_iMemGranularity;
    void **m_pEntries;
    bool m_bAnyRemoved;
    static DynArray_cl<char> g_SceneElementSortingTable;
  public: 
    mutable int m_iIterator;
};



/// \brief
///   Entity collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of entities.
class VisEntityCollection_cl : public VisRenderCollection_cl
{
  public:

  /// \brief
  ///   Constructor of the VisEntityCollection_cl class
  /// 
  /// ,taking an array of VisBaseEntity_cl pointers as input.
  /// 
  /// The array of integer values will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of entity pointers to be copied
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisEntityCollection_cl(unsigned int iNumEntries, VisBaseEntity_cl * const * pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisEntityCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the entity collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisEntityCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Destructor of the VisEntityCollection_cl class
  /// 
  /// Cleans up the collection and deallocates all the memory associated with it
  VISION_APIFUNC ~VisEntityCollection_cl();


  /// \brief
  ///   Determines the entities in the collection touching a given bounding box
  /// 
  /// The entities touching the bounding box are stored in the target collection.
  /// 
  /// \param box
  ///   The bounding box against which the entities are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the entities touching the box are stored
  VISION_APIFUNC void DetermineEntriesTouchingBox(const hkvAlignedBBox &box, VisEntityCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the entities touching the negative/positive halfspace of the given plane
  /// 
  /// The entities touching the desired halfspace are stored in the target collection.
  /// 
  /// \param plane
  ///   The plane against which the entities are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the entities touching the desired halfspace are stored.
  /// 
  /// \param bNegativeHalfspace
  ///   If TRUE, the entities touching the negative halfspace are added to the target collection;
  ///   otherwise, the entities touching the positive halfspace are added.
  VISION_APIFUNC void DetermineEntriesInHalfspace(const hkvPlane& plane, VisEntityCollection_cl &targetCollection, BOOL bNegativeHalfspace) const;

  /// \brief
  ///   Determines the entities in the collection touching a given frustum
  /// 
  /// The entities touching the frustum are stored in the target collection.
  /// 
  /// \param frustum
  ///   The frustum against which the entities are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the entities touching the frustum are stored
  VISION_APIFUNC void DetermineEntriesTouchingFrustum(const VisFrustum_cl &frustum, VisEntityCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the entities in the collection touching a given convex volume
  /// 
  /// The entities touching the convex volume are stored in the target collection.
  /// 
  /// \param convexVolume
  ///   The convex volume against which the entities are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the entities touching the convex volume are stored
  VISION_APIFUNC void DetermineEntriesTouchingConvexVolume(const VisConvexVolume_cl &convexVolume, VisEntityCollection_cl &targetCollection) const;

  /// \brief
  ///   Tags all entities in this collection. DEPRECATED: Use 'TagEntries' instead.
  /// 
  /// \sa VisRenderLoopHelper_cl::TagEntity
  /// \sa VisRenderLoopHelper_cl::ResetEntityTags
  /// \sa VisRenderLoopHelper_cl::IsEntityTagged
  HKV_DEPRECATED_2012_3 VISION_APIFUNC void TagEntities() const;

  /// \brief
  ///   Adds all tagged entities in the current collection to the destination collection. DEPRECATED: Use 'GetTaggedEntries' instead.
  /// 
  /// \param destCollection
  ///   Destination collection
  HKV_DEPRECATED_2012_3 VISION_APIFUNC void GetTaggedEntities(VisEntityCollection_cl &destCollection) const;

  /// \brief
  ///   Tags all entities in this collection.
  /// 
  /// \sa VisRenderLoopHelper_cl::TagEntity
  /// \sa VisRenderLoopHelper_cl::ResetEntityTags
  /// \sa VisRenderLoopHelper_cl::IsEntityTagged
  VISION_APIFUNC void TagEntries() const;

  /// \brief
  ///   Adds all tagged entities in the current collection to the destination collection.
  /// 
  /// \param destCollection
  ///   Destination collection
  /// \param bRequestedTagStatus
  ///   true, to get all tagged instances, false to get all untagged
  VISION_APIFUNC void GetTaggedEntries(VisEntityCollection_cl &destCollection, bool bRequestedTagStatus=true) const;

  /// \brief
  ///   Removes all entries from the collection that are tagged (via VisRenderLoopHelper_cl::TagEntity) or not tagged (bStatus=false)
  /// 
  /// \param bStatus
  ///   The tag status to test. All elements with VisbaseEntity_cl::IsTagged==bStatus are removed
  ///
  /// \sa VisRenderLoopHelper_cl::TagEntity
  /// \sa VisRenderLoopHelper_cl::ResetEntityTags
  /// \sa VisRenderLoopHelper_cl::IsEntityTagged
  VISION_APIFUNC void RemoveWithTagStatus(bool bStatus);

  /// \brief
  ///   Filters out all entities in this collection that did not pass the visibility check
  VISION_APIFUNC void FilterNonVisibleEntities();

  /// \brief
  ///   Prepares all entities in this collection for rendering.
  /// 
  /// On some hardware platforms, animated entities may require special preparation (e.g. skinned
  /// entities using memexport skinning on Xbox360 or Stream Out in DirectX10/11). For entities which
  /// pass the visibility determination (e.g., are contained in the collections of visible scene
  /// elements generated by the visibility collector of the current context), this preparation code
  /// is called automatically. However, if you want to render animated entities which have not gone
  /// through visibility determination, you have to call this function for a collection of these
  /// entities first.
  /// 
  /// Note that, if you have multiple entities requiring this kind of setup, they should be put
  /// into the same collection, and PrepareForRendering should be called on the whole collection
  /// rather than on individual one-entity collections. This will be significantly faster.
  VISION_APIFUNC void PrepareForRendering() const;


  /// \brief
  ///   Sorts the entity collection according to the specified criteria.
  /// 
  /// Valid criteria are: VIS_SORTING_DISTANCE: Sorts the elements front to back
  /// VIS_SORTING_PROXIMITY: Sorts the elements front to back VIS_SORTING_POLYGONCOUNT: Sorts the
  /// element by polygon count (high polygon counts first) VIS_SORTING_RENDERSTATE: Sorts the
  /// elements by render state VIS_SORTING_OBJECTKEY: Sorts the elements by object key
  /// 
  /// These flags can be combined, except for VIS_SORTING_DISTANCE and VIS_SORTING_PROXIMITY, which
  /// are mutually exclusive.
  /// 
  /// If multiple flags are set, the priorities of the criteria are as follows (highest to lowest):
  /// VIS_SORTING_OBJECTKEY - VIS_SORTING_RENDERSTATE - VIS_SORTING_POLYGONCOUNT -
  /// VIS_SORTING_PROXIMITY/VIS_SORTING_DISTANCE.
  /// 
  /// \param iFlags
  ///   Sorting criteria as specified in the function description.
  /// 
  /// \param pContext
  ///   Render context to use for the distance/proximity criteria. If set to NULL and the
  ///   distance/proximity criteria are specified, the currently active render context is used.
  VISION_APIFUNC void Sort(int iFlags, VisRenderContext_cl *pContext = NULL);



  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to entity pointer.
  inline VisBaseEntity_cl* GetEntry(unsigned int iIndex) const     {return (VisBaseEntity_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends an entity to this collection
  inline void AppendEntry(VisBaseEntity_cl *pEntity)      {VisRenderCollection_cl::AppendEntry(pEntity);}

  /// \brief
  ///   Appends an entity to this collection without range checking
  inline void AppendEntryFast(VisBaseEntity_cl *pEntity)  {VisRenderCollection_cl::AppendEntryFast(pEntity);}

  /// \brief
  ///   Returns the pointer to the entity collection list.
  inline VisBaseEntity_cl **GetDataPtr() const            {return (VisBaseEntity_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///
};




/// \brief
///   Visibility Object collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of visibility objects.
class VisVisibilityObjectCollection_cl : public VisRenderCollection_cl
{
  public:

  /// \brief
  ///   Constructor of the VisVisibilityObjectCollection_cl class
  /// 
  /// The array of integer values will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of pointers to copy
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisVisibilityObjectCollection_cl(unsigned int iNumEntries, VisVisibilityObject_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisVisibilityObjectCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the visibility object collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisVisibilityObjectCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Destructor of the VisVisibilityObjectCollection_cl class
  /// 
  /// Cleans up the collection and deallocates all the memory associated with it.
  VISION_APIFUNC ~VisVisibilityObjectCollection_cl();
  
  /// \brief
  ///   Determines the visibility objects in the collection touching a given bounding box
  /// 
  /// The visibility objects touching the bounding box are stored in the target collection.
  /// 
  /// \param box
  ///   The bounding box against which the visibility objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the visibility objects touching the box are stored
  VISION_APIFUNC void DetermineEntriesTouchingBox(const hkvAlignedBBox &box, VisVisibilityObjectCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the visibility objects touching the negative/positive halfspace of the given
  ///   plane
  /// 
  /// The visibility objects touching the desired halfspace are stored in the target collection.
  /// 
  /// \param plane
  ///   The plane against which the visibility objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the visibility objects touching the desired halfspace are stored
  /// 
  /// \param bNegativeHalfspace
  ///   If TRUE, the visibility objects touching the negative halfspace are added to the target
  ///   collection; otherwise, the visibility objects touching the positive halfspace are added.
  VISION_APIFUNC void DetermineEntriesInHalfspace(const hkvPlane& plane, VisVisibilityObjectCollection_cl &targetCollection, BOOL bNegativeHalfspace) const;

  /// \brief
  ///   Determines the visibility objects in the collection touching a given frustum
  /// 
  /// The visibility objects touching the frustum are stored in the target collection.
  /// 
  /// \param frustum
  ///   The frustum against which the visibility objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the visibility objects touching the frustum are stored
  VISION_APIFUNC void DetermineEntriesTouchingFrustum(const VisFrustum_cl &frustum, VisVisibilityObjectCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the visibility objects in the collection touching a given convex volume
  /// 
  /// The visibility objects touching the convex volume are stored in the target collection
  /// 
  /// \param convexVolume
  ///   The convex volume against which the visibility objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the visibility objects touching the convex volume are stored
  VISION_APIFUNC void DetermineEntriesTouchingConvexVolume(const VisConvexVolume_cl &convexVolume, VisVisibilityObjectCollection_cl &targetCollection) const;

  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to visibility object pointer.
  inline VisVisibilityObject_cl* GetEntry(unsigned int iIndex) const     {return (VisVisibilityObject_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a visibility object to this collection
  inline void AppendEntry(VisVisibilityObject_cl *pObject)      {VisRenderCollection_cl::AppendEntry(pObject);}

  /// \brief
  ///   Appends a visibility object to this collection without range checking
  inline void AppendEntryFast(VisVisibilityObject_cl *pObject)  {VisRenderCollection_cl::AppendEntryFast(pObject);}

  /// \brief
  ///   Returns the pointer to the visibility object collection list.
  inline VisVisibilityObject_cl **GetDataPtr() const            {return (VisVisibilityObject_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};

/// \brief
///   Light Source collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of light sources.
class VisLightSrcCollection_cl : public VisRenderCollection_cl
{
  public:


  /// \brief
  ///   Constructor of the VisLightSrcCollection_cl class
  /// 
  /// This is a constructor of the light source collection class taking an array of
  /// VisLightSource_cl pointers as input.
  /// 
  /// The array of integer values will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy.
  /// 
  /// \param pEntryList
  ///   Pointer to the array of pointers to copy.
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries).
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisLightSrcCollection_cl(unsigned int iNumEntries, VisLightSource_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);


  /// \brief
  ///   Constructor of the VisLightSrcCollection_cl class
  /// 
  /// This is a constructor of the VisLightSrcCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the light source collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisLightSrcCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);


  /// \brief
  ///   Destructor of the VisLightSrcCollection_cl class
  /// 
  /// Cleans up the collection and deallocates all the memory associated with it.
  VISION_APIFUNC ~VisLightSrcCollection_cl();


  /// \brief
  ///   Determines the light source in the collection touching a given bounding box
  /// 
  /// The light sources touching the bounding box are stored in the target collection.
  /// 
  /// \param box
  ///   The bounding box against which the light sources are compared.
  /// 
  /// \param targetCollection
  ///   The collection in which the light sources touching the box are stored.
  /// 
  /// \param bUseRange
  ///   If TRUE, the light source range is taken into account; otherwise, only the light source
  ///   position is used.
  VISION_APIFUNC void DetermineEntriesTouchingBox(const hkvAlignedBBox &box, VisLightSrcCollection_cl &targetCollection, BOOL bUseRange) const;


  /// \brief
  ///   Determines the lights touching the negative/positive halfspace of the given plane
  /// 
  /// The lights touching the desired halfspace are stored in the target collection.
  /// 
  /// \param plane
  ///   The plane against which the lights are compared.
  /// 
  /// \param targetCollection
  ///   The collection in which the lights touching the desired halfspace are stored.
  /// 
  /// \param bNegativeHalfspace
  ///   If TRUE, the lights touching the negative halfspace are added to the target collection;
  ///   otherwise, the lights touching the positive halfspace are added.
  /// 
  /// \param bUseRange
  ///   If TRUE, the light source range is taken into account; otherwise, only the light source
  ///   position is used.
  VISION_APIFUNC void DetermineEntriesInHalfspace(const hkvPlane& plane, VisLightSrcCollection_cl &targetCollection, BOOL bNegativeHalfspace, BOOL bUseRange) const;


  /// \brief
  ///   Determines the light source in the collection touching a given frustum
  /// 
  /// The light sources touching the frustum are stored in the target collection.
  /// 
  /// \param frustum
  ///   The frustum against which the light source are compared.
  /// 
  /// \param targetCollection
  ///   The collection in which the light source touching the frustum are stored.
  /// 
  /// \param bUseRange
  ///   If TRUE, the light source range is taken into account; otherwise, only the light source
  ///   position is used.
  VISION_APIFUNC void DetermineEntriesTouchingFrustum(const VisFrustum_cl &frustum, VisLightSrcCollection_cl &targetCollection, BOOL bUseRange) const;


  /// \brief
  ///   Determines the lights in the collection touching a given convex volume
  /// 
  /// The lights touching the convex volume are stored in the target collection.
  /// 
  /// \param convexVolume
  ///   The event area against which the lights are compared.
  /// 
  /// \param targetCollection
  ///   The collection in which the lights touching the convex volume are stored.
  /// 
  /// \param bUseRange
  ///   If TRUE, the light source range is taken into account; otherwise, only the light source
  ///   position is used.
  VISION_APIFUNC void DetermineEntriesTouchingConvexVolume(const VisConvexVolume_cl &convexVolume, VisLightSrcCollection_cl &targetCollection, BOOL bUseRange) const;


  /// \brief
  ///   Sorts the collection of light sources according to their components.
  /// 
  /// Sorting order is as follows:
  /// - normal light sources (without clip volume component and without shadow map component)
  /// - light sources with clip volume component
  /// - light sources with shadow map component
  /// - light sources with clip volume component and shadow map component
  VISION_APIFUNC void SortByComponents();


  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///


  /// \brief
  ///   Casts the entry with given index to light source pointer.
  inline VisLightSource_cl* GetEntry(unsigned int iIndex) const    {return (VisLightSource_cl*)VisRenderCollection_cl::GetEntry(iIndex);}


  /// \brief
  ///   Appends a light source to this collection
  inline void AppendEntry(VisLightSource_cl *pLight)      {VisRenderCollection_cl::AppendEntry(pLight);}


  /// \brief
  ///   Appends a light source to this collection without range checking
  inline void AppendEntryFast(VisLightSource_cl *pLight)  {VisRenderCollection_cl::AppendEntryFast(pLight);}


  /// \brief
  ///   Returns the pointer to the light source collection list
  inline VisLightSource_cl **GetDataPtr() const           {return (VisLightSource_cl **)m_pEntries;}


  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
 
  
  ///
  /// @}
  ///

};





/// \brief
///   Mesh buffer object collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of mesh buffer object instances.
class VisMeshBufferObjectCollection_cl : public VisRenderCollection_cl
{
public:

  /// \brief
  ///   Constructor of the VisMeshBufferObjectCollection_cl class
  /// 
  /// This is a constructor of the mesh buffer object collection class taking an array of
  /// VisMeshBufferObject_cl pointers as input.
  /// 
  /// The array of pointers will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of pointers to copy
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisMeshBufferObjectCollection_cl(unsigned int iNumEntries, VisMeshBufferObject_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisMeshBufferObjectCollection_cl class
  /// 
  /// This is a constructor of the VisMeshBufferObjectCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the mesh buffer object collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisMeshBufferObjectCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Determines the mesh buffer objects in the collection touching a given bounding box
  /// 
  /// For all mesh buffer objects that have a visibility object assigned, it uses the bounding box
  /// of the visibility object to check.
  /// 
  /// All other mesh buffer objects are always added.
  /// 
  /// \param box
  ///   The bounding box against which the mesh buffer objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the mesh buffer objects touching the box are stored
  VISION_APIFUNC void DetermineEntriesTouchingBox(const hkvAlignedBBox &box, VisMeshBufferObjectCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the mesh buffer objects touching the negative/positive halfspace of the given
  ///   plane
  /// 
  /// For all mesh buffer objects that have a visibility object assigned, it uses the bounding box
  /// of the visibility object to check.
  /// 
  /// All other mesh buffer objects are always added.
  /// 
  /// \param plane
  ///   The plane against which the mesh buffer objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the mesh buffer objects touching the desired halfspace are stored
  /// 
  /// \param bNegativeHalfspace
  ///   If TRUE, the mesh buffer objects touching the negative halfspace are added to the target
  ///   collection; otherwise, the mesh buffer objects touching the positive halfspace are added.
  VISION_APIFUNC void DetermineEntriesInHalfspace(const hkvPlane& plane, VisMeshBufferObjectCollection_cl &targetCollection, BOOL bNegativeHalfspace) const;
  
  /// \brief
  ///   Determines the mesh buffer objects in the collection touching a given frustum
  /// 
  /// For all mesh buffer objects that have a visibility object assigned, it uses the bounding box
  /// of the visibility object to check.
  /// 
  /// All other mesh buffer objects are always added.
  /// 
  /// \param frustum
  ///   The frustum against which the mesh buffer objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the mesh buffer objects touching the frustum are stored
  VISION_APIFUNC void DetermineEntriesTouchingFrustum(const VisFrustum_cl &frustum, VisMeshBufferObjectCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the mesh buffer objects in the collection touching a given convex volume
  /// 
  /// For all mesh buffer objects that have a visibility object assigned, it uses the bounding box
  /// of the visibility object to check.
  /// 
  /// All other mesh buffer objects are always added.
  /// 
  /// \param convexVolume
  ///   The convex volume against which the mesh buffer objects are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the mesh buffer objects touching the convex volume are stored
  VISION_APIFUNC void DetermineEntriesTouchingConvexVolume(const VisConvexVolume_cl &convexVolume, VisMeshBufferObjectCollection_cl &targetCollection) const;


  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to mesh buffer object pointer.
  inline VisMeshBufferObject_cl* GetEntry(unsigned int iIndex) const    {return (VisMeshBufferObject_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a mesh buffer instance to this collection
  inline void AppendEntry(VisMeshBufferObject_cl *pObject)     {VisRenderCollection_cl::AppendEntry(pObject);}

  /// \brief
  ///   Appends a mesh buffer instance to this collection without range checking
  inline void AppendEntryFast(VisMeshBufferObject_cl *pObject) {VisRenderCollection_cl::AppendEntryFast(pObject);}

  /// \brief
  ///   Returns the pointer to the mesh buffer instance collection list.
  inline VisMeshBufferObject_cl **GetDataPtr() const           {return (VisMeshBufferObject_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};



/// \brief
///   Screen mask collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of screen masks.
class VisScreenMaskCollection_cl : public VisRenderCollection_cl
{
public:

  /// \brief
  ///   This is a constructor of the screen mask collection class taking an array of VisScreenMask_cl
  ///   pointers as input.
  /// 
  /// The array of pointers will be copied into the collection
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy.
  /// 
  /// \param pEntryList
  ///   Pointer to the array of pointers to copy.
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries).
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisScreenMaskCollection_cl(unsigned int iNumEntries, VisScreenMask_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisScreenMaskCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the screen mask collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisScreenMaskCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Sorts the screen mask collection by the masks' sorting order
  /// 
  /// Since rendering a collection of screen masks does not resort the list of screen masks to
  /// render, this has to  be done manually before calling
  /// VisRenderLoopHelper_cl::RenderScreenMasks with a collection as input.
  /// 
  /// The sorting is the same as for standard screen mask rendering.
  /// 
  /// \sa VisScreenMask_cl
  /// \sa VisScreenMask_cl::SetOrder
  /// \sa VisRenderLoopHelper_cl::RenderScreenMasks
  VISION_APIFUNC void SortByOrder();

  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to screen mask pointer.
  inline VisScreenMask_cl* GetEntry(unsigned int iIndex) const    {return (VisScreenMask_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a screen mask to this collection
  inline void AppendEntry(VisScreenMask_cl *pMask)     {VisRenderCollection_cl::AppendEntry(pMask);}

  /// \brief
  ///   Appends a screen mask to this collection without range checking
  inline void AppendEntryFast(VisScreenMask_cl *pMask) {VisRenderCollection_cl::AppendEntryFast(pMask);}

  /// \brief
  ///   Returns the pointer to the screen mask collection list.
  inline VisScreenMask_cl **GetDataPtr() const           {return (VisScreenMask_cl **)m_pEntries;}
  ///
  /// @}
  ///

};


/// \brief
///   Particle group collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of particle groups.
class VisParticleGroupCollection_cl : public VisRenderCollection_cl
{
public:

  /// \brief
  ///   Constructor of the VisParticleGroupCollection_cl class
  /// 
  /// This is a constructor of the particle group collection class taking an array of
  /// VisParticleGroup_cl pointers as input.
  /// 
  /// The array of pointers will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of pointers to copy
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisParticleGroupCollection_cl(unsigned int iNumEntries, VisParticleGroup_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisParticleGroupCollection_cl class
  /// 
  /// This is a constructor of the VisParticleGroupCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the particle group collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisParticleGroupCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Sorts the collection of particle groups according to camera distance or a user sort key.
  /// 
  /// The camera distance is recalculated using the VisParticleGroup_cl::GetSortingDistance
  /// function and the camera position of the current context.
  /// 
  /// \param bAssignCamDist
  ///   If TRUE, the current camera distance is recalculated using the custom definable particle
  ///   group bounding box (see VisParticleGroup_cl::GetSortingCenterPos). If FALSE, the user sort
  ///   key is used for sorting the particle groups.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::GetSortingDistance
  VISION_APIFUNC void SortParticleGroups(BOOL bAssignCamDist);

  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to particle group pointer.
  inline VisParticleGroup_cl* GetEntry(unsigned int iIndex) const    {return (VisParticleGroup_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a particle group to this collection
  inline void AppendEntry(VisParticleGroup_cl *pGroup)     {VisRenderCollection_cl::AppendEntry(pGroup);}

  /// \brief
  ///   Appends a particle group to this collection without range checking
  inline void AppendEntryFast(VisParticleGroup_cl *pGroup) {VisRenderCollection_cl::AppendEntryFast(pGroup);}

  /// \brief
  ///   Returns the pointer to the particle group collection list.
  inline VisParticleGroup_cl **GetDataPtr() const           {return (VisParticleGroup_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};



/// \brief
///   Collection class for instances of type VisObject3D_cl
class VisObject3DCollection_cl : public VisRenderCollection_cl
{
public:

  VISION_APIFUNC VisObject3DCollection_cl(unsigned int iNumEntries, VisObject3D_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);
  VISION_APIFUNC VisObject3DCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to VisObject3D_cl group pointer.
  inline VisObject3D_cl* GetEntry(unsigned int iIndex) const    {return (VisObject3D_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a VisObject3D_cl this collection
  inline void AppendEntry(VisObject3D_cl *pObject3D)     {VisRenderCollection_cl::AppendEntry(pObject3D);}

  /// \brief
  ///   Appends a VisObject3D_cl to this collection without range checking
  inline void AppendEntryFast(VisObject3D_cl *pObject3D) {VisRenderCollection_cl::AppendEntryFast(pObject3D);}

  /// \brief
  ///   Returns the pointer to the VisObject3D_cl collection list.
  inline VisObject3D_cl **GetDataPtr() const           {return (VisObject3D_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};


/// \brief
///   Collection class for instances of type VisVisibilityZone_cl
class VisVisibilityZoneCollection_cl : public VisRenderCollection_cl
{
public:

  VISION_APIFUNC VisVisibilityZoneCollection_cl(unsigned int iNumEntries, VisVisibilityZone_cl * const *pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);
  VISION_APIFUNC VisVisibilityZoneCollection_cl(unsigned int iSize, unsigned int iMemGranularity=1024);

  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to VisVisibilityZone_cl pointer.
  inline VisVisibilityZone_cl* GetEntry(unsigned int iIndex) const    {return (VisVisibilityZone_cl*)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a VisVisibilityZone_cl to this collection
  inline void AppendEntry(VisVisibilityZone_cl *pNode)     {VisRenderCollection_cl::AppendEntry(pNode);}

  /// \brief
  ///   Appends a VisVisibilityZone_cl to this collection without range checking
  inline void AppendEntryFast(VisVisibilityZone_cl *pNode) {VisRenderCollection_cl::AppendEntryFast(pNode);}

  /// \brief
  ///   Returns the pointer to the VisVisibilityZone_cl collection list.
  inline VisVisibilityZone_cl **GetDataPtr() const           {return (VisVisibilityZone_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};


/// \brief
///   Static geometry instance collection class
/// 
/// Subclass of the VisRenderCollection_cl class providing functionality for handling collections
/// of static geometry instances.
class VisStaticGeometryInstanceCollection_cl : public VisRenderCollection_cl
{
  public:

  /// \brief
  ///   Constructor of the VisStaticGeometryInstanceCollection_cl class
  /// 
  /// This is a constructor of the static geometry instance collection class taking an array of
  /// static geometry instances as input.
  /// 
  /// The array of static geometry instance pointers will be copied into the collection.
  /// 
  /// \param iNumEntries
  ///   Number of elements to copy
  /// 
  /// \param pEntryList
  ///   Pointer to the array of elements (pointers) to copy
  /// 
  /// \param iSize
  ///   Size of the collection (where iSize>=iNumEntries)
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisStaticGeometryInstanceCollection_cl(unsigned int iNumEntries, const VisStaticGeometryInstance_cl **pEntryList, unsigned int iSize, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Constructor of the VisStaticGeometryInstanceCollection_cl class.
  /// 
  /// \param iSize
  ///   Initial size of the static geometry instance collection in numbers of elements.
  /// 
  /// \param iMemGranularity
  ///   Whenever additional elements are added to a collection, and the number of elements becomes
  ///   too big for the currently allocated size of the collection, the collection is re-allocated.
  ///   This parameter defines the increments of each re-allocation step.
  VISION_APIFUNC VisStaticGeometryInstanceCollection_cl(unsigned int iSize=0, unsigned int iMemGranularity=1024);

  /// \brief
  ///   Destructor of the VisStaticGeometryInstanceCollection_cl class
  /// 
  /// Cleans up the collection and deallocates all the memory associated with it.
  VISION_APIFUNC ~VisStaticGeometryInstanceCollection_cl();

  /// \brief
  ///   Determines the static geometry instances in the collection touching a given bounding box
  /// 
  /// The static geometry instances touching the bounding box are stored in the target collection.
  /// 
  /// \param box
  ///   The bounding box against which the static geometry instances are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the static geometry instances touching the box are stored
  VISION_APIFUNC void DetermineEntriesTouchingBox(const hkvAlignedBBox &box, VisStaticGeometryInstanceCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the static geometry instances touching the negative/positive halfspace of the
  ///   given plane
  /// 
  /// The static geometry instances touching the desired halfspace are stored in the target
  /// collection.
  /// 
  /// \param plane
  ///   The plane against which the static geometry instances are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the static geometry instances touching the desired halfspace are
  ///   stored
  /// 
  /// \param bNegativeHalfspace
  ///   If TRUE, the static geometry instances touching the negative halfspace are added to the
  ///   target collection; otherwise, the static geometry instances touching the positive halfspace
  ///   are added.
  VISION_APIFUNC void DetermineEntriesInHalfspace(const hkvPlane& plane, VisStaticGeometryInstanceCollection_cl &targetCollection, BOOL bNegativeHalfspace) const;

  /// \brief
  ///   Determines the static geometry instances in the collection touching a given frustum
  /// 
  /// The static geometry instances touching the frustum are stored in the target collection.
  /// 
  /// \param frustum
  ///   The frustum against which the static geometry instances are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the static geometry instances touching the frustum are stored
  VISION_APIFUNC void DetermineEntriesTouchingFrustum(const VisFrustum_cl &frustum, VisStaticGeometryInstanceCollection_cl &targetCollection) const;

  /// \brief
  ///   Determines the static geometry instances in the collection touching a given convex volume
  /// 
  /// The static geometry instances touching the convex volume are stored in the target collection.
  /// 
  /// \param convexVolume
  ///   The convex volume against which the static geometry instances are compared
  /// 
  /// \param targetCollection
  ///   The collection in which the static geometry instances touching the convex volume are stored
  VISION_APIFUNC void DetermineEntriesTouchingConvexVolume(const VisConvexVolume_cl &convexVolume, VisStaticGeometryInstanceCollection_cl &targetCollection) const;

  /// \brief
  ///   Non-virtual override
  inline BOOL Contains(VisStaticGeometryInstance_cl *pMeshInstance) const {return GetIndexOf((const void *)pMeshInstance)>=0;}

  /// \brief
  ///   Filters out all static geometry instances in this collection that did not pass the last
  ///   visibility check in the passed context.
  VISION_APIFUNC void FilterInvisibleEntries(VisRenderContext_cl *pContext=NULL);

  /// \brief
  ///   Tags this collection of static geometry instances
  /// 
  /// \sa VisStaticGeometryInstance_cl::Tag
  /// \sa VisStaticGeometryInstance_cl::IsTagged
  /// \sa VisStaticGeometryInstance_cl::ResetTags
  VISION_APIFUNC void TagEntries() const;

  /// \brief
  ///   Removes all entries from the collection that are tagged (via VisStaticGeometryInstance_cl::Tag) or not tagged (bStatus=false)
  /// 
  /// \param bStatus
  ///   The tag status to test. All elements with VisStaticGeometryInstance_cl::IsTagged==bStatus are removed
  ///
  /// \sa VisStaticGeometryInstance_cl::Tag
  /// \sa VisStaticGeometryInstance_cl::IsTagged
  /// \sa VisStaticGeometryInstance_cl::ResetTags
  VISION_APIFUNC void RemoveWithTagStatus(bool bStatus);

  /// \brief
  ///   Extracts the tagged static geometry instances from this collection into another collection.
  /// 
  /// The passed destination collection will be filled with the tagged static geometry instances,
  /// but it will not be cleared beforehand.
  /// 
  /// \param destCollection
  ///   The destination collection that gets the tagged static geometry instances appended.
  /// \param bRequestedTagStatus
  ///   true, to get all tagged instances, false to get all untagged
  ///
  /// \sa VisStaticGeometryInstance_cl::Tag
  /// \sa VisStaticGeometryInstance_cl::IsTagged
  /// \sa VisStaticGeometryInstance_cl::ResetTags
  VISION_APIFUNC void GetTaggedEntries(VisStaticGeometryInstanceCollection_cl &destCollection, bool bRequestedTagStatus=true) const;

  /// \brief
  ///   Same as GetTaggedEntries but additionally filters by geometry type
  ///   (VisStaticGeometryType_e)
  VISION_APIFUNC void GetTaggedEntriesOfType(VisStaticGeometryInstanceCollection_cl &destCollection, VisStaticGeometryType_e eGeomType, bool bRequestedTagStatus=true) const;

  /// \brief
  ///   Returns all entries in the collection, filtered by geometry type
  VISION_APIFUNC void GetEntriesOfType(VisStaticGeometryInstanceCollection_cl &destCollection, VisStaticGeometryType_e eGeomType) const;

  /// \brief
  ///   Computes the combined bounding box of all static geometry instances in this collection.
  /// 
  /// This operation can be quite expensive for large collections, so it is not recommended to call
  /// it too frequently.
  /// 
  /// \param Box
  ///   The bounding box of the static geometry instances in this collection.
  VISION_APIFUNC void ComputeBoundingBox(hkvAlignedBBox &Box) const;

  /// \brief
  ///   Sorts the static geometry instance collection according to the specified criterion.
  /// 
  /// Valid criteria are: VIS_SORTING_DISTANCE: Sorts the elements front to back
  /// VIS_SORTING_PROXIMITY: Sorts the elements front to back VIS_SORTING_RENDERSTATE: Sorts the
  /// elements by render state VIS_SORTING_VERTEXBUFFER: Sorts the elements by vertex buffer
  /// 
  /// These values are mutually exclusive.
  /// 
  /// \param iFlags
  ///   Sorting criterion as specified in the function description.
  /// 
  /// \param pContext
  ///   Render context to use for the distance/proximity criteria. If set to NULL and the
  ///   distance/proximity criteria are specified, the currently active render context is used.
  VISION_APIFUNC void Sort(int iFlags, VisRenderContext_cl *pContext = NULL);

  /// \brief
  ///   Splits the collection according to the elements' lighting modes.
  /// 
  /// Lightmapped static geometry instances will end up in the "lightMapInstances" collection;
  /// non-lightmapped ones in the "lightGridInstances" collection. To original collection remains
  /// unmodified.
  /// 
  /// \param lightMapInstances
  ///   Target collection for lightmapped static geometry instances.
  /// 
  /// \param lightGridInstances
  ///   Target collection for non-lightmapped static geometry instances.
  VISION_APIFUNC void SplitLightingModes(VisStaticGeometryInstanceCollection_cl &lightMapInstances, VisStaticGeometryInstanceCollection_cl &lightGridInstances) const;


  ///
  /// @name Non-virtual Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Casts the entry with given index to a VisStaticGeometryInstance_cl pointer.
  inline VisStaticGeometryInstance_cl *GetEntry(unsigned int iIndex) const {return (VisStaticGeometryInstance_cl *)VisRenderCollection_cl::GetEntry(iIndex);}

  /// \brief
  ///   Appends a VisStaticGeometryInstance_cl to this collection
  inline void AppendEntry(VisStaticGeometryInstance_cl *pMeshInstance)      {VisRenderCollection_cl::AppendEntry((VisStaticGeometryInstance_cl *)pMeshInstance);}

  /// \brief
  ///   Appends a VisStaticGeometryInstance_cl to this collection without range checking
  inline void AppendEntryFast(VisStaticGeometryInstance_cl *pMeshInstance)  {VisRenderCollection_cl::AppendEntryFast((VisStaticGeometryInstance_cl *)pMeshInstance);}

  /// \brief
  ///   Returns the pointer to the VisStaticGeometryInstance_cl collection list.
  inline VisStaticGeometryInstance_cl **GetDataPtr() const            {return (VisStaticGeometryInstance_cl **)m_pEntries;}

  /// \brief
  ///   Serializes all entries in this collection
  inline void SerializeX(VArchive &ar)
  {
    V_IMPLEMENT_RENDERCOLLECTION_SERIALIZEX
  }
  ///
  /// @}
  ///

};



#include <Vision/Runtime/Engine/SceneElements/VisApiSortedRenderCollection.inl>

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
