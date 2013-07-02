/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VIS_API_GPU_RESOURCE_HPP
#define VIS_API_GPU_RESOURCE_HPP

#if defined( _VISION_DOC )
  #define SUPPORTS_GPU_RESOURCE_VIEWS
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  #include <Vision/Runtime/Base/System/Resource/VGpuResourceView.hpp>
#endif

#if defined( SUPPORTS_LOCKABLE_MEMORY )
  #include <Vision/Runtime/Engine/Renderer/Buffer/VisApiLockableMemory.hpp>
#endif

//#define USE_UNIQUE_RESOURCE_ID

/// \brief
///   Defines a generic GPU resource.
///
/// \note
///   For now, this includes only GPU buffers, but will be extended to contain textures in the future.
class VGpuResource : public VManagedResource
{
public:
  /// \brief
  ///   Constructor of the VGpuResource class.
  /// 
  /// \param pParentManager
  ///   Resource manager this resource will be added to.
  ///
  /// \param uiSize
  ///   Size of this resource in bytes.
  ///
  /// \param uiUsageFlags
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for data that is specified
  ///   only once and then never changed. STREAM should be used for data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_UAV_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader in DX11.
  ///   Finally, specify VIS_MEMUSAGE_UAV_DRAWINDIRECT, VIS_MEMUSAGE_APPEND, and/or VIS_MEMUSAGE_COUNT
  ///   if appropriate for the intended use in a pixel and/or compute shader in DX11.
  ///
  /// \param uiBindFlags
  ///   Specify bind flags for this resource depending on its intended usage. Possible values are:
  ///   \li VIS_BIND_INDEX_BUFFER: Allows this resource to be used as an index buffer.
  ///   \li VIS_BIND_VERTEX_BUFFER: Allows this resource to be used as a vertex buffer.
  ///   \li VIS_BIND_SHADER_RESOURCE: Allows this resource to be bound as a shader resource via an
  ///     appropriate shader resource view (DX11 only).
  ///   \li VIS_BIND_UNORDERED_ACCESS_VIEW: Allows this resource to be bound for unordered access 
  ///     via an appropriate unordered access view (DX11 only).
  ///   \li VIS_BIND_STREAM_OUTPUT: Allows this resource to be used as a target for stream
  ///     output operations (DX11 only).
  ///
  /// \param bHasSysmemCopy
  ///   Specifies, whether this resource has a shadow copy of its data in system memory. This should
  ///   be chosen if read operations from the buffer are desired. However, this additional copy
  ///   results in twice the memory consumption and typically results in lower update performance.
  ///
  /// \param bSafeCleanUp
  ///   Specifies, whether internally a label is used for a safe clean-up of the encapsulated resource.
  ///   This is not necessary when it is guaranteed that the resource is not in used by the GPU when 
  ///   it is destroyed. (This applies only to platforms which support VLockableMemory.)
  ///
  /// \param szInternalName
  ///   String to send to the underlying rendering API for debugging purposes. This string will appear in 
  ///   GPU leak outputs, PIX, etc.
  ///
  /// \note
  ///   Depending on the target platform, the selected configuration of usage flags, bind flags, and/or
  ///   the use of a system memory copy has implications on the accessibility to this resource.
  ///   For example, not using a system memory copy means that resource data cannot be read on GLES2 and that 
  ///   a resource with VIS_MEMUSAGE_STATIC cannot be locked on DX11.
  ///
  /// \sa VLockableMemory
  /// \sa VShaderResourceView
  /// \sa VUnorderedAccessView
  VISION_APIFUNC VGpuResource(VisResourceManager_cl* pParentManager, 
    unsigned int uiSize,
    unsigned int uiUsageFlags, 
    unsigned int uiBindFlags,
    bool bHasSysmemCopy, 
    bool bSafeCleanUp = true,
    const char* szInternalName = "VisGpuResource");

  /// \brief
  ///   Destructor - cleans up its resources.
  virtual ~VGpuResource();

  /// \brief
  ///   Ensures that the hardware resource exists. Recreates its data from internal system memory copy if necessary 
  ///   and (re-)initializes resource views created from this resource (if applicable).
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;

  /// \brief
  ///   Unloads the hardware resource and de-initializes resource views created from this resource.
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;

  /// \brief
  ///   Fills the resource's memory with the given data.
  ///
  /// If a linear range of data is to be copied into the resource, this method provides the fastest way of doing so.
  ///
  /// \param pSourceData
  ///   Data to be copied into the resource.
  ///
  /// \param uiOffset
  ///   Byte offset into internal resource memory to copy the data to.
  /// 
  /// \param uiSize
  ///   Size of the data to be copied in bytes.
  /// 
  /// \param uiLockFlags
  ///   Some target platforms allow for some performance optimization by indicating the intended use 
  ///   through a combination of the following flags:
  ///   \li VIS_LOCKFLAG_DISCARDABLE: Specify this flag if _all_ data in the memory area is to be overwritten.
  ///     This allows for replacing all internal data with freshly allocated memory in order to avoid having to
  ///     wait for the GPU to finish using this resource. The current memory will be released as soon as it is not
  ///     in use anymore.
  ///   \li VIS_LOCKFLAG_NOOVERWRITE: Specify this flag if the locked memory area is guaranteed not to be in use 
  ///     by the GPU anymore. This allows for direct access to this memory area without having to wait for any labels
  ///     to be reached.
  VISION_APIFUNC virtual void FillWithData( const void* pSourceData, unsigned int uiOffset, unsigned int uiSize, unsigned int uiLockFlags = VIS_LOCKFLAG_NOOVERWRITE );

  /// \brief
  ///   Lock the GPU resource for reading and/or writing.
  /// 
  /// This method provides safe access to GPU resource's internal memory for reading and/or writing. By specifying appropriate
  /// lock flags, performance can be improved considerably as it allows for skipping explicit synchronization and/or additional
  /// data transfer.
  ///
  /// If this resource has a copy of its data in system memory, this method will return a pointer to this copy. Its content will
  /// be uploaded to the resource as soon as the resource is unlocked again (unless VIS_LOCKFLAG_READONLY has been specified).
  ///
  /// \note
  ///   While partial locking of a resource is possible by providing suitable values for uiOffset and uiSize, not all platforms support
  ///   this functionality. It is therefore recommended to always lock the whole buffer by setting both parameters to 0.
  ///
  /// \param uiLockFlags
  ///   In order to optimize performance, the intended use can be indicated through a combination of the following flags:
  ///   \li VIS_LOCKFLAG_DISCARDABLE: Specify this flag if _all_ data in the memory area is to be overwritten.
  ///     This allows for replacing all internal data with freshly allocated memory in order to avoid having to
  ///     wait for the GPU to finish using this resource. The current memory will be released as soon as it is not
  ///     in use anymore.
  ///   \li VIS_LOCKFLAG_READONLY: Specify this flag if there will be no data written to the memory area. This allows
  ///     for skipping GPU data upload.
  ///   \li VIS_LOCKFLAG_NOOVERWRITE: Specify this flag if the locked memory area is guaranteed not to be in use 
  ///     by the GPU anymore. This allows for direct access to this memory area without having to wait for any labels
  ///     to be reached.
  ///   \li VIS_LOCKFLAG_REPLACE_ALL: Specify this flag if the complete _locked_ memory area is to be overwritten.
  ///     Depending on the platform, this allows for skipping GPU data download into CPU-accessible memory.
  ///
  /// \param uiOffset
  ///   Starting offset of the memory area to be locked.
  ///
  /// \param uiSize
  ///   Size of the memory area to be locked.
  ///
  /// \returns
  ///   A pointer to the locked data.
  ///
  /// \sa VGpuResource::Unlock
  VISION_APIFUNC virtual void* Lock( unsigned int uiLockFlags, unsigned int uiOffset = 0, unsigned int uiSize = 0 );

  /// \brief
  ///   Unlock the GPU resource.
  /// 
  /// If this resource has a copy of its data in system memory, this method will trigger an upload of the shadow copy
  /// into video memory (unless VIS_LOCKFLAG_READONLY has been specified in the call to VGpuResource::Lock()).
  ///
  /// \sa VGpuResource::Lock
  VISION_APIFUNC virtual void Unlock();

  /// \brief
  ///   Returns whether this resource is currently locked.
  inline bool IsLocked() const { return m_pLockedPtr != NULL; }

  /// \brief
  ///   Retrieve a pointer to the locked memory.
  inline void* GetLockedPtr() const { return m_pLockedPtr; }

  /// \brief
  ///   Returns whether this resource can be locked for reading.
  VISION_APIFUNC bool CanBeLockedForReading() const;

  /// \brief
  ///   Returns whether this resource has a shadow copy of its data in system memory.
  inline bool HasSysmemCopy() const { return m_bHasSysmemCopy; }

  /// \brief
  ///   Retrieve a pointer to this resource's shadow copy in system memory.
  inline void *GetSysmemBufferPtr() { return m_pSystemCopy; }

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )

#if defined( _VR_DX11 )
  /// \brief
  ///   Create a shader resource view of this resource.
  ///
  /// If no shader resource view matching the given description exists, a new resource view is created and returned.
  /// In addition, it is stored internally and initialized/de-initialized appropriately when this resource is (re-)loaded
  /// and/or unloaded. If a shader resource view already exists for the given description, this method returns a pointer
  /// to this existing view.
  ///
  /// The resource view pointer should be stored using a VSmartPtr in order to ensure proper reference counting. Call
  /// VGpuResource::PurgeUnusedViews() to delete all resource views which are not used outside of this resource anymore.
  ///
  /// \note
  ///   In order for resource view management to work properly, always store the returned pointers using VSmartPtr.
  ///
  /// \param pDesc
  ///   A platform specific description of the shader resource view to be created.
  /// 
  /// \returns
  ///   A pointer to a shader resource view for this resource. Store it in a smart pointer to 
  ///   make sure that its reference count is incremented and decremented as appropriate.
  /// 
  /// \sa VGpuResource::PurgeUnusedViews
  /// \sa VGpuResource::Reload
  /// \sa VGpuResource::Unload
  /// \sa VShaderResourceView
  /// \sa VSmartPtr
  VISION_APIFUNC VShaderResourceView* CreateShaderResourceView( const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc  );

  /// \brief
  ///   Create an unordered access view of this resource.
  ///
  /// If no unordered access view matching the given description exists, a new view is created and returned.
  /// In addition, it is stored internally and initialized/de-initialized appropriately when this resource is (re-)loaded
  /// and/or unloaded. If an unordered access view already exists for the given description, this method returns a pointer
  /// to this existing view.
  ///
  /// The access view pointer should be stored using a VSmartPtr in order to ensure proper reference counting. Call
  /// VGpuResource::PurgeUnusedViews() to delete all views which are not used outside of this resource anymore.
  ///
  /// \note
  ///   In order for resource view management to work properly, always store the returned pointers using VSmartPtr.
  ///
  /// \param pDesc
  ///   A platform specific description of the unordered access view to be created.
  /// 
  /// \returns
  ///   A pointer to an unordered access view for this resource. Store it in a smart pointer to 
  ///   make sure that its reference count is incremented and decremented as appropriate.
  /// 
  /// \sa VGpuResource::PurgeUnusedViews
  /// \sa VGpuResource::Reload
  /// \sa VGpuResource::Unload
  /// \sa VUnorderedAccessView
  /// \sa VSmartPtr
  VISION_APIFUNC VUnorderedAccessView* CreateUnorderedAccessView( const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc );

  /// \brief
  ///   Create a depth stencil view of this resource.
  ///
  /// If no depth stencil view matching the given description exists, a new view is created and returned.
  /// In addition, it is stored internally and initialized/de-initialized appropriately when this resource is (re-)loaded
  /// and/or unloaded. If a depth stencil view already exists for the given description, this method returns a pointer
  /// to this existing view.
  ///
  /// The access view pointer should be stored using a VSmartPtr in order to ensure proper reference counting. Call
  /// VGpuResource::PurgeUnusedViews() to delete all views which are not used outside of this resource anymore.
  ///
  /// \note
  ///   In order for resource view management to work properly, always store the returned pointers using VSmartPtr.
  ///
  /// \param pDesc
  ///   A platform specific description of the depth stencil view to be created.
  /// 
  /// \returns
  ///   A pointer to a depth stencil view for this resource. Store it in a smart pointer to 
  ///   make sure that its reference count is incremented and decremented as appropriate.
  /// 
  /// \sa VGpuResource::PurgeUnusedViews
  /// \sa VGpuResource::Reload
  /// \sa VGpuResource::Unload
  /// \sa VDepthStencilView
  /// \sa VSmartPtr
  VISION_APIFUNC VDepthStencilView* CreateDepthStencilView( const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc  );

  /// \brief
  ///   Create a render target view of this resource.
  ///
  /// If no render target view matching the given description exists, a new view is created and returned.
  /// In addition, it is stored internally and initialized/de-initialized appropriately when this resource is (re-)loaded
  /// and/or unloaded. If an render target view already exists for the given description, this method returns a pointer
  /// to this existing view.
  ///
  /// The access view pointer should be stored using a VSmartPtr in order to ensure proper reference counting. Call
  /// VGpuResource::PurgeUnusedViews() to delete all views which are not used outside of this resource anymore.
  ///
  /// \note
  ///   In order for resource view management to work properly, always store the returned pointers using VSmartPtr.
  ///
  /// \param pDesc
  ///   A platform specific description of the render target view to be created.
  /// 
  /// \returns
  ///   A pointer to render target view for this resource. Store it in a smart pointer to 
  ///   make sure that its reference count is incremented and decremented as appropriate.
  /// 
  /// \sa VGpuResource::PurgeUnusedViews
  /// \sa VGpuResource::Reload
  /// \sa VGpuResource::Unload
  /// \sa VRenderTargetView
  /// \sa VSmartPtr

  VISION_APIFUNC VRenderTargetView* CreateRenderTargetView( const D3D11_RENDER_TARGET_VIEW_DESC* pDesc );
#endif

  //VISION_APIFUNC VShaderResourceView* CreateStructuredBufferSRV( ... );
  //VISION_APIFUNC VShaderResourceView* CreateStructuredBufferUAV( ... );

  /// \brief
  ///   Destroy all access views which are not used outside this resource anymore.
  ///
  /// \note
  ///   In order for resource view management to work properly, always store the pointers returned by
  ///   any of the view creation methods using VSmartPtr.
  /// \sa VGpuResource::CreateShaderResourceView
  /// \sa VGpuResource::CreateUnorderedAccessView
  /// \sa VGpuResource::CreateDepthStencilView
  /// \sa VGpuResource::CreateRenderTargetView
  /// \sa VGpuResourceView
  /// \sa VSmartPtr
  VISION_APIFUNC int PurgeUnusedViews();

#endif  // defined( SUPPORTS_GPU_RESOURCE_VIEWS )

  /// \brief
  ///   Retrieve the internal size of this resource in bytes.
  unsigned int GetSize() const { return m_uiSize; }

  /// \brief
  ///   Retrieve the usage flags of this resource.
  unsigned int GetUsageFlags() const { return m_uiUsageFlags; }

  /// \brief
  ///   Retrieve the bind flags of this resource.
  unsigned int GetBindFlags() const { return m_uiBindFlags; }

  // Internal use only
#if defined( _VR_DX11 )
  inline ID3D11Resource* GetResource() const { return m_pResource; }

#elif defined( _VR_DX9 )
  // nothing to see here

#elif defined( _VR_GLES2 )
  inline unsigned int GetHandle() const { return m_uiHandle; }

#elif defined( SUPPORTS_LOCKABLE_MEMORY )
  inline VLockableMemory* GetMemory() const { return &m_lockableMemory; }

#else
  #error Undefined platform!

#endif

  // usage marker - internal use only...
#if defined( SUPPORTS_LOCKABLE_MEMORY )
  inline void BeginUse() { m_lockableMemory.BeginUse(); } // set when binding / setting array
  inline void EndUse() { m_lockableMemory.EndUse(); }     // set when unbinding / disabling array
#else
  inline void BeginUse() {}
  inline void EndUse() {}
#endif

  /// \brief
  ///   Overridden function to evaluate memory consumption.
  VISION_APIFUNC virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;

protected:

  // (de-)initialize platform-specific data
  inline void InitResource_Internal();
  inline void DeInitResource_Internal();

#if defined( SUPPORTS_LOCKABLE_MEMORY )
  // allow derived classes to reinitialize data based on this resource / memory
  VISION_APIFUNC virtual void OnInternalMemoryDiscarded();
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  VISION_APIFUNC void ReInitResourceViewsOnReload();
  VISION_APIFUNC void DeInitResourceViewsOnUnload();
#endif

  void*   m_pSystemCopy;
  void*   m_pLockedPtr;

  unsigned int  m_uiSize;
  unsigned int  m_uiUsageFlags;
  unsigned int  m_uiBindFlags;

  unsigned int  m_uiLockFlags;
  unsigned int  m_uiLockedOffset;
  unsigned int  m_uiLockedSize;

  bool    m_bHasSysmemCopy;
  bool    m_bSafeCleanUp;

#ifdef USE_UNIQUE_RESOURCE_ID
  unsigned int  m_uiId;
#endif

#if defined( _VR_DX11 )
  ID3D11Resource*  m_pResource;
  D3D_USAGE   m_d3dUsage;
  DWORD       m_d3dCPUAccessFlags;
  DWORD       m_d3dBindFlags;
  DWORD       m_d3dMiscFlags;

#elif defined( _VR_DX9 )
//  D3D_USAGE   m_d3dUsage;
  void* m_pResource;

#elif defined( _VR_GLES2 )
  unsigned int m_uiHandle;
  GLenum m_eOGLUsage;

#elif defined( SUPPORTS_LOCKABLE_MEMORY )
  mutable VLockableMemory m_lockableMemory;

#else
  #error Undefined platform!

#endif

#if defined( _VR_GX2 )
  u32 m_GX2RResourceFlags;
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  VRefCountedCollection< VShaderResourceView >  m_shaderResourceViews;
  VRefCountedCollection< VUnorderedAccessView > m_unorderedAccessViews;
  VRefCountedCollection< VDepthStencilView >    m_depthStencilViews;
  VRefCountedCollection< VRenderTargetView >    m_renderTargetViews;
#endif

};


#endif // VIS_API_GPU_RESOURCE_HPP

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
