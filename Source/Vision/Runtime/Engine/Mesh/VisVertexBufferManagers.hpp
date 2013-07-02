/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisVertexBufferManagers.hpp

#ifndef VIS_VERTEXBUFFER_MANAGERS_HPP_INCLUDED
#define VIS_VERTEXBUFFER_MANAGERS_HPP_INCLUDED



// define smartpointer
class VisSystemMemoryBuffer_cl;
typedef VSmartPtr<VisSystemMemoryBuffer_cl> VisSystemMemoryBufferPtr;

/// \brief
///   ResourceManager with specified memory limitation and auto-unload interval.
class VisMemoryBufferResourceManager_cl:public VisResourceManager_cl
{
public:

  /// \brief
  ///   Constructor of the memory buffer resource manager.
  /// 
  /// It also calls constructor of parent class.
  /// 
  /// \param szManagerName
  ///   Name for the new created memory manager
  /// 
  /// \param iFlags
  ///   flags that get passed to the parent constructor
  /// 
  /// \param iMemLimit
  ///   defines the memory limitation
  /// 
  /// \param fUnloadInterval
  ///   specifies after what time interval the manager checks its resources
  /// 
  /// \param fUnloadTimeout
  ///   time after which non-used buffers get deleted
  inline VisMemoryBufferResourceManager_cl(const char *szManagerName, int iFlags, int iMemLimit, float fUnloadInterval, float fUnloadTimeout)
    :VisResourceManager_cl(szManagerName, iFlags, iMemLimit)
  {
    SetMemoryLimit(iMemLimit);
    SetAutoUnloadInterval(fUnloadInterval, fUnloadTimeout, false, 0.0f);
  }
};


/// \brief
///   Resource to keep a system memory buffer.
class VisSystemMemoryBuffer_cl : public VManagedResource
{
public:

  /// \brief
  ///   Constructor of the system memory buffer class.
  /// 
  /// \param pParentManager
  ///   manager that manages resource.
  /// 
  /// \param iByteCount
  ///   size of the buffer in bytes.
  inline VisSystemMemoryBuffer_cl(VisResourceManager_cl *pParentManager, int iByteCount):VManagedResource(pParentManager) {m_iByteCount = iByteCount; m_pSystemMemoryBuffer=NULL;}

  
  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisSystemMemoryBuffer_cl();


  /// \brief
  ///   Returns a pointer to the system memory buffer.
  inline void* GetBufferPointer(void)
  {
    return m_pSystemMemoryBuffer;
  }


  /// \brief
  ///   Returns a handle to the system memory buffer.
  /// 
  /// \return
  ///   int: handle for existing system memory buffer
  inline int GetBufferHandle()
  {
    return m_iSystemMemoryBufferHandle;
  }

protected:
  
  /// \brief
  ///   Implements the reloading of the resource.
  /// 
  /// \return
  ///   BOOL bResult: TRUE if reloading successful.
  VISION_APIFUNC BOOL Reload();

    
  /// \brief
  ///   Implements the unloading of the resource.
  /// 
  /// \return
  ///   BOOL bResult: TRUE if unloading successful.
  VISION_APIFUNC BOOL Unload();


  /// \brief
  ///   Overridden function to evaluate memory consumption
  VISION_APIFUNC virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
  {
    VManagedResource::AccumulateMemoryFootprint(iUniqueSys,iUniqueGPU,iDependentSys,iDependentGPU);
    if (m_pSystemMemoryBuffer)
      iUniqueSys += m_iByteCount;
  }

  int m_iByteCount;
  int m_iSystemMemoryBufferHandle;
  void* m_pSystemMemoryBuffer;
};


#endif //VIS_VERTEXBUFFER_MANAGERS_HPP_INCLUDED

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
