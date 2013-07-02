/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiAnimSequenceSet.hpp

#ifndef VIS_ANIM_SEQUENCE_SET_HPP_INCLUDED
#define VIS_ANIM_SEQUENCE_SET_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiSkeleton.hpp>

class VisAnimSequence_cl;
typedef VSmartPtr<VisAnimSequence_cl> VisAnimSequencePtr;
class VisAnimSequenceSet_cl;
typedef VSmartPtr<VisAnimSequenceSet_cl> VisAnimSequenceSetPtr;

class VChunkFile; 
class VisSkeletalAnimSequence_cl; 
class VisVertexAnimSequence_cl;

// chunk definitions needed for loading from chunk file
#define ANIMATION_FILE_CHUNK    'ANIM'
#define ANIMATION_CHUNK_HEADER  'HEAD'
#define SKELETON_CHUNK          'SKEL'
#define BONE_ANIM_CHUNK         'BANI'
#define BONE_BOUNDINGBOX_CHUNK  'VSBX'
#define OFFSET_DELTA_CHUNK      'ODTA'
#define ROTATION_DELTA_CHUNK    'RDTA'
#define MOTION_DELTA_CHUNK      'MDTA'
#define BONE_TRANSLATION        'BPOS'
#define BONE_ROTATION           'BROT'
#define BONE_SCALING            'BSCL'
#define EVENT_CHUNK             'EVNT'
#define VERTEX_ANIM_CHUNK       'VANI'
#define VERTEX_DELTAS           'VDEL'
#define VERTEX_MASK_CHUNK       'VMSK'

#define MAX_SKELETONS_PER_FILE  16


/// \brief
///   Derived class for loading a VisAnimSequenceSet_cl resource 
class VSequenceSetSerializationProxy : public IVSerializationProxy
{
public:
  inline VSequenceSetSerializationProxy(VisAnimSequenceSet_cl *pSet) {VASSERT(pSet);m_pSet=pSet;}
  virtual IVSerializationProxyCreator *GetInstance();
#ifdef SUPPORTS_SNAPSHOT_CREATION
  virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif
protected:
  // serialization
  inline VSequenceSetSerializationProxy() {m_pSet=NULL;}
  V_DECLARE_SERIAL_DLLEXP( VSequenceSetSerializationProxy, VISION_APIDATA );
  virtual void Serialize( VArchive &ar );

private:
  VisAnimSequenceSet_cl *m_pSet;
};

/// \brief
///   The anim sequence manager is a resource manager that handles the animation sequence set
///   resources
/// 
/// There is one global manager accessible via Vision::Animations.GetSequenceSetManager.
/// 
/// This global instance can also be used to manually load animation files.
class VisAnimSequenceSetManager_cl : public VisResourceManager_cl
{
public:

  /// \brief
  ///   Constructor of the anim sequence set manager class.
  /// 
  /// \param szManagerName
  ///   name of the manager to create.
  /// 
  /// \param iFlags
  ///   flags set for resource management
  /// 
  /// \param iMemLimit
  ///   memlimit to set
  /// 
  /// \sa VisResourceManager_cl;
  VISION_APIFUNC VisAnimSequenceSetManager_cl(const char *szManagerName, int iFlags=VRESOURCEMANAGERFLAG_NONE, int iMemLimit=0);


  /// \brief
  ///   Loads an animation sequence set from a file and returns a pointer to it.
  /// 
  /// It first tries to find an existing set in the resource manager.
  /// 
  /// \param szFilename
  ///   name of the animation file
  /// 
  /// \return
  ///   VisAnimSequenceSet_cl* pSet: pointer to the loaded sequence
  VISION_APIFUNC VisAnimSequenceSet_cl* LoadAnimSequenceSet(const char *szFilename);

  /// \brief
  ///   Loads an animation sequence set from an arbitrary stream
  /// 
  /// It first tries to find an existing set in the resource manager.
  /// 
  /// \param szInternalName
  ///   name of the animation set resource to look up in the manager and to assign later as a resource name
  /// 
  /// \param pIn
  ///   The stream to read the data from. To read from plain memory, use the VMemBlockWrapperStream wrapper class
  ///
  /// \return
  ///   VisAnimSequenceSet_cl* pSet: pointer to the loaded sequence
  VISION_APIFUNC VisAnimSequenceSet_cl* LoadAnimSequenceSet(const char *szInternalName, IVFileInStream *pIn);

  /// \brief
  ///   Returns a pointer to the animation sequence of the specified filename if it already has
  ///   been loaded.
  /// 
  /// \param szFilename
  ///   name of the animation file
  /// 
  /// \return
  ///   VisAnimSequenceSet_cl* pSet: pointer to the sequence set or NULL
  VISION_APIFUNC VisAnimSequenceSet_cl* GetAnimSequenceSet(const char *szFilename);


  /// \brief
  ///   Overridden resource manager function
  VISION_APIFUNC virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
};


/// \brief
///   An instance of VisAnimSequenceSet_cl holds an entire set of anim sequences that are loaded
///   from one file (.anim file extension).
class VisAnimSequenceSet_cl : public VManagedResource
{
public:

  /// \brief
  ///   Constructor of the anim sequence set class.
  /// 
  /// \param pParentManager
  ///   manager that manages that sequence set.
  /// 
  /// \param szFilename
  ///   name of animation file
  VISION_APIFUNC VisAnimSequenceSet_cl(VisAnimSequenceSetManager_cl* pParentManager, const char *szFilename);

  
  /// \brief
  ///   Destructor
  ~VisAnimSequenceSet_cl();


  /// \brief
  ///   Tries to find a single sequence with the specified name and type.
  /// 
  /// \param szSeqName
  ///   name of the sequence to search
  /// 
  /// \param eType
  ///   type of the sequence to search; can be VIS_MODELANIM_UNKNOWN to search for all types
  /// 
  /// \return
  ///   VisAnimSequence_cl* pFound: pointer to found sequence, or NULL
  VISION_APIFUNC VisAnimSequence_cl* GetSequence(const char *szSeqName, VisModelAnimType_e eType = VIS_MODELANIM_UNKNOWN) const;

  
  /// \brief
  ///   Returns the sequence with the specified index
  /// 
  /// The index must be in valid range [0..GetSequenceCount()-1]
  /// 
  /// \param iIndex
  ///   index of the sequence inside this set
  /// 
  /// \return
  ///   VisAnimSequence_cl* pSeq: sequence pointer
  VISION_APIFUNC VisAnimSequence_cl* GetSequence(int iIndex) const;


  /// \brief
  ///   Returns the number of sequences in this set.
  /// 
  /// \return
  ///   int iCount: Number of sequences in this set
  inline int GetSequenceCount() const 
  {
    return m_iSequenceCount;
  }


  /// \brief
  ///   Returns the skeleton with the specified index.
  /// 
  /// The different sequences might have been built based on different skeletons.
  /// 
  /// Usually there is only one skeleton in a set of animations.
  /// 
  /// \param iIndexSkeleton
  ///   index of the requested skeleton. Must be in valid range [0..GetSkeletonCount()-1]
  /// 
  /// \return
  ///   VisSkeleton_cl* pSkeleton: pointer to skeleton.
  inline const VisSkeleton_cl* GetSkeleton(int iIndexSkeleton) const 
  {
    VASSERT((iIndexSkeleton>=0)&&(iIndexSkeleton<m_iSkeletonCount)); 
    return m_spSkeletonList[iIndexSkeleton];
  }

  /// \brief
  ///   Returns the number of distinct skeletons in this set. Usually 1.
  inline int GetSkeletonCount() const
  {
    return m_iSkeletonCount;
  }

protected:

  /// \brief
  ///   Implements the reloading of the resource.
  /// 
  /// \return
  ///   BOOL bResult: TRUE if reloading was successful.
  VISION_APIFUNC virtual BOOL Reload();

    
  /// \brief
  ///   Implements the unloading of the resource.
  /// 
  /// \return
  ///   BOOL bResult: TRUE if unloading was successful.
  VISION_APIFUNC virtual BOOL Unload();


  /// \brief
  ///   Overridden function
  VISION_APIFUNC virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);
    
  /// \brief
  ///   returns a proxy object for serialization
  VISION_APIFUNC virtual IVSerializationProxy *CreateProxy()
  {
    return new VSequenceSetSerializationProxy(this);
  }


  /// \brief
  ///   Loads an animation file with specified filename. Do not use
  /// 
  /// Loads an animation file with specified filename. This function bypasses the resource
  /// management and should only be used by tools.
  /// 
  /// \param pszFilename
  ///   name of animation file to load.
  /// 
  /// \return
  ///   VisAnimSequenceSet_cl: Loaded animation sequence set or NULL
  static VISION_APIFUNC VisAnimSequenceSet_cl* LoadDirect(const char *pszFilename);

private:

  /// \brief
  ///   LoadAnimFile loads animation file with specified filename.
  /// 
  /// \param pszFilename
  ///   name of animation file to load.
  /// 
  /// \return
  ///   bool bResult: True if loading has been successful.
  VISION_APIFUNC bool LoadAnimFile(const char *pszFilename);

  /// \brief
  ///   Loads the animation sequence set from the specified stream
  /// 
  /// \param pIn
  ///   the stream to load from.
  /// 
  /// \return
  ///   bool bResult: True if loading has been successful.
  VISION_APIFUNC bool LoadAnimFile(IVFileInStream *pIn);
  friend class VisAnimSequenceSetManager_cl; 

  int m_iSequenceCount;
  VisAnimSequencePtr* m_spAnimSequenceList; ///< pointer to sequence array
  int m_iSkeletonCount;
  VisSkeletonPtr m_spSkeletonList[MAX_SKELETONS_PER_FILE];

  /// \brief
  ///   Private constructor for LoadDirect
  VisAnimSequenceSet_cl();
  
};


/// \brief
///   Collection class for animation sets (instances of VisAnimSequenceSet_cl)
/// 
/// A VDynamicMesh instance keeps such a collection, so it's faster to find a sequence for a special
/// model.
class VisAnimSequenceSetCollection_cl : public VRefCountedCollection<VisAnimSequenceSet_cl>
{
public:

  /// \brief
  ///   Constructor of the anim sequence set collection class.
  VISION_APIFUNC VisAnimSequenceSetCollection_cl() 
  {
  }


  /// \brief
  ///   Adds a sequence set to this collection. Just wraps around AddUnique
  /// 
  /// \param pSequence
  ///   sequence set to be added.
  /// 
  /// \return
  ///   bool bResult: true if adding was successful.
  inline bool AddSequenceSet(VisAnimSequenceSet_cl *pSequence)
  {
    AddUnique(pSequence);
    return true;
  }


  /// \brief
  ///   Removes a sequence set instance from the collection.
  /// 
  /// \param pSequence
  ///   pointer to the sequence set to remove.
  /// 
  /// \return
  ///   bool bResult: true if removed successfully.
  inline bool RemoveSequenceSet(VisAnimSequenceSet_cl *pSequence)
  {
    if (Contains(pSequence))
      Remove(pSequence);
    return true;
  }


  /// \brief
  ///   Finds a sequence set by filename.
  /// 
  /// \param szFilename
  ///   filename of the sequence set to find.
  /// 
  /// \return
  ///   VisAnimSequenceSet_cl* pSet: Pointer to the sequence set (or NULL).
  VISION_APIFUNC VisAnimSequenceSet_cl* GetSequenceSet(const char *szFilename);


  /// \brief
  ///   Finds a single sequence by animation name and type.
  /// 
  /// This function loops through all sets in this collection to find the sequence.
  /// 
  /// \param szSeqName
  ///   name of the sequence.
  /// 
  /// \param eType
  ///   type of the sequence. Can be VIS_MODELANIM_UNKNOWN to search for all types
  /// 
  /// \return
  ///   VisAnimSequence_cl* pSequence: pointer to the sequence (or NULL).
  VISION_APIFUNC VisAnimSequence_cl* GetSequence(const char *szSeqName, VisModelAnimType_e eType = VIS_MODELANIM_SKELETAL);


  /// \brief
  ///   Returns a sequence set specified by index. Wraps around GetAt(i)
  /// 
  /// \param i
  ///   index of the sequence set. Must be in valid range [0..GetSequenceSetCount()-1]
  /// 
  /// \return
  ///   VisAnimSequenceSet_cl* pSet: Pointer to the sequence set
  inline VisAnimSequenceSet_cl* GetSequenceSet(int i)
  {
    return GetAt(i);
  }


  /// \brief
  ///   Returns the number of sequence sets. Wraps around Count().
  /// 
  /// \return
  ///   int: number of sequence sets.
  inline int GetSequenceSetCount() const
  {
    return Count();
  }


};

#endif //VIS_ANIM_SEQUENCE_SET_HPP_INCLUDED

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
