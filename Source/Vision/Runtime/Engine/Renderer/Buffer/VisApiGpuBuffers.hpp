/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VIS_API_GPU_BUFFER_HPP
#define VIS_API_GPU_BUFFER_HPP

#include <Vision/Runtime/Engine/Renderer/Buffer/VisApiGpuResource.hpp>

/// \brief
///   Defines a generic GPU buffer.
class VGpuBuffer : public VGpuResource
{
public:
  /// \brief
  ///   Constructor of the VGpuBuffer class.
  ///
  /// This constructor also creates default resource views if the corresponding binds flags have 
  /// been specified and the target platforms supports it.
  /// 
  /// \param pParentManager
  ///   Resource manager this resource will be added to.
  ///
  /// \param uiElementCount
  ///   Number of elements to be stored in this buffer.
  ///
  /// \param uiStride
  ///   Size (in bytes) of a single element to be stored in this buffer.
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
  /// \sa VGpuResource
  /// \sa VGpuResource::VGpuResource
  VISION_APIFUNC VGpuBuffer(VisResourceManager_cl* pParentManager, 
    unsigned int uiElementCount,
    unsigned int uiStride,
    unsigned int uiUsageFlags,
    unsigned int uiBindFlags,
    bool bHasSysmemCopy,
    bool bSafeCleanUp = true,
    const char* szInternalName = "VGpuBuffer" );

  /// \brief
  ///   Destructor - cleans up its resources.
  VISION_APIFUNC virtual ~VGpuBuffer();

  /// \brief
  ///   Retrieve the number of elements in this buffer.
  inline unsigned int GetElementCount() const { return m_uiElementCount; }

  /// \brief
  ///   Retrieve the stride used for this buffer (size per element in bytes).
  inline unsigned int GetStride() const { return m_uiStride; }

#if !defined( SUPPORTS_LOCKABLE_MEMORY ) || defined( _VR_GX2 )
  /// \brief
  ///   Ensures that the hardware resource exists. See VGpuResource::Reload() for more information.
  /// \sa VGpuResource::Reload
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;

  /// \brief
  ///   Unloads the hardware resource. See VGpuResource::Unload() for more information.
  /// \sa VGpuResource::Unload
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;
#endif

#if defined( _VR_GLES2 )
  /// \brief
  ///   Lock the GPU resource for reading and/or writing. See VGpuResource::Lock() for more information.
  ///
  /// \param uiLockFlags
  ///   The lockflags to use when accessing the memory. 
  ///
  /// \param uiOffset
  ///   The offset at which the locked memory should start.
  ///
  /// \param uiSize
  ///   How much memory should be locked. (Specify 0 for the whole buffer)
  ///
  /// \sa VGpuResource::Lock
  VISION_APIFUNC virtual void* Lock( unsigned int uiLockFlags, unsigned int uiOffset = 0, unsigned int uiSize = 0 ) HKV_OVERRIDE;

  /// \brief
  ///   Unlock the GPU resource. See VGpuResource::Unlock() for more information.
  /// \sa VGpuResource::Unlock
  VISION_APIFUNC virtual void Unlock() HKV_OVERRIDE;
#endif

  // internal use only
#if defined( _VR_DX11 )
  ID3D11Buffer* GetBuffer() const { return static_cast< ID3D11Buffer* >( m_pResource ); }
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS ) || defined( _VISION_DOC )
  /// \brief
  ///   Retrieve the default shader resource view for this buffer.
  VShaderResourceView* GetShaderResourceView() const { return m_spShaderResourceView.GetPtr(); }

  /// \brief
  ///   Retrieve the default unordered access view for this buffer.
  VUnorderedAccessView* GetUnorderedAccessView() const { return m_spUnorderedAccessView.GetPtr(); }
#endif

protected:
  // (de-)initialize platform-specific data
  inline void InitBuffer_Internal();
  inline void DeInitBuffer_Internal();

  unsigned int  m_uiElementCount;
  unsigned int  m_uiStride;

#if defined( _VR_DX9 )
  DWORD   m_d3dUsage;
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )
  VShaderResourceViewPtr  m_spShaderResourceView;
  VUnorderedAccessViewPtr m_spUnorderedAccessView;
#endif
};
typedef VSmartPtr< VGpuBuffer > VGpuBufferPtr;


/// \brief
///   Defines an index buffer.
class VIndexBuffer : public VGpuBuffer
{
public:
  /// \brief
  ///   Constructor of the VIndexBuffer class.
  ///
  /// \param pParentManager
  ///   Resource manager this resource will be added to.
  ///
  /// \param uiElementCount
  ///   Number of indices to be stored in this buffer.
  ///
  /// \param eIndexFormat
  ///   Format of the indices to be stored in this buffer.
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
  /// \param uiAdditionalBindFlags
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
  /// \sa VGpuBuffer
  /// \sa VGpuBuffer::VGpuBuffer
  /// \sa VIndexFormat
  VISION_APIFUNC VIndexBuffer(VisResourceManager_cl* pParentManager,
    unsigned int uiElementCount, 
    VIndexFormat eIndexFormat,
    unsigned int uiUsageFlags,
    unsigned int uiAdditionalBindFlags,
    bool bHasSysmemCopy,
    bool bSafeCleanUp = true,
    const char* szInternalName = "VIndexBuffer");
  VISION_APIFUNC virtual ~VIndexBuffer();

#if !defined( SUPPORTS_LOCKABLE_MEMORY )
  /// \brief
  ///   Ensures that the hardware resource exists. See VGpuResource::Reload() for more information.
  /// \sa VGpuResource::Reload
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;

  /// \brief
  ///   Unloads the hardware resource. See VGpuResource::Unload() for more information.
  /// \sa VGpuResource::Unload
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;
#endif

#if defined( _VR_GLES2 )
  /// \brief
  ///   Fills the resource's memory with the given data. See VGpuResource::FillWithData for more information.
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
  ///
  /// \sa VGpuResource::FillWithData
  VISION_APIFUNC virtual void FillWithData( const void* pSourceData, unsigned int uiOffset, unsigned int uiSize, unsigned int uiLockFlags = VIS_LOCKFLAG_NOOVERWRITE ) HKV_OVERRIDE;
#endif

#if defined( _VR_DX9 )
  /// \brief
  ///   Lock the GPU resource for reading and/or writing. See VGpuResource::Lock() for more information.
  ///
  /// \param uiLockFlags
  ///   The lockflags to use when accessing the memory. 
  ///
  /// \param uiOffset
  ///   The offset at which the locked memory should start.
  ///
  /// \param uiSize
  ///   How much memory should be locked. (Specify 0 for the whole buffer)
  ///
  /// \sa VGpuResource::Lock
  VISION_APIFUNC virtual void* Lock( unsigned int uiLockFlags, unsigned int uiOffset = 0, unsigned int uiSize = 0 ) HKV_OVERRIDE;
#endif

#if defined( _VR_DX9 ) || defined( _VR_GLES2 )
  /// \brief
  ///   Unlock the GPU resource. See VGpuResource::Unlock() for more information.
  /// \sa VGpuResource::Unlock
  VISION_APIFUNC virtual void Unlock() HKV_OVERRIDE;
#endif

  /// \brief
  ///   Retrieve this buffer's index format.
  /// \sa VIndexFormat
  inline VIndexFormat GetIndexFormat() const { return m_eIndexFormat; }

  // internal use only
#if defined( _VR_DX11 ) || defined( _VR_DX9 )
  D3DIndexBuffer* GetIndexBuffer() const { return static_cast< D3DIndexBuffer* >( m_pResource ); }
#elif defined( _VR_GLES2 )
  GLenum GetOGLIndexFormat() const { return m_eOGLIndexFormat; }
#elif defined( _VR_GXM )
  SceGxmIndexFormat GetGxmFormat() { return m_GxmIndexFormat; }
#elif defined( _VR_GX2 )
  inline GX2IndexFormat GetGX2Format() { return m_GX2IndexFormat; }
#endif

protected:
  // (de-)initialize platform-specific data
  inline void InitIndexBuffer_Internal();
  inline void DeInitIndexBuffer_Internal();

  VIndexFormat m_eIndexFormat;

#if defined( _VR_DX11 )
  DXGI_FORMAT m_d3dIndexFormat;

#elif defined( _VR_DX9 )
  D3DFORMAT m_d3dIndexFormat;

#elif defined( _VR_GLES2 )
  GLenum m_eOGLIndexFormat;

#elif defined( _VR_GCM )
  // nothing to see here...

#elif defined( _VR_GXM )
  SceGxmIndexFormat m_GxmIndexFormat;

#elif defined( _VR_GX2 )
  GX2IndexFormat  m_GX2IndexFormat;

#else
#error Undefined platform!

#endif
};
typedef VSmartPtr< VIndexBuffer > VIndexBufferPtr;


/// \brief 
///   Defines a vertex buffer.
class VVertexBuffer : public VGpuBuffer
{
public:
  /// \brief
  ///   Constructor of the VVertexBuffer class.
  ///
  /// \param pParentManager
  ///   Resource manager this resource will be added to.
  ///
  /// \param uiElementCount
  ///   Number of elements to be stored in this vertex buffer.
  ///
  /// \param uiStride
  ///   Size (in bytes) of a single element to be stored in this buffer.
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
  /// \param uiAdditionalBindFlags
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
  /// \sa VGpuBuffer
  /// \sa VGpuBuffer::VGpuBuffer
  VISION_APIFUNC VVertexBuffer(VisResourceManager_cl* pParentManager, 
    unsigned int uiElementCount,
    unsigned int uiStride,
    unsigned int uiUsageFlags,
    unsigned int uiAdditionalBindFlags,
    bool bHasSysmemCopy,
    bool bSafeCleanUp = true,
    const char* szInternalName = "VVertexBuffer" );

  VISION_APIFUNC virtual ~VVertexBuffer();

#if !defined( SUPPORTS_LOCKABLE_MEMORY )
  /// \brief
  ///   Ensures that the hardware resource exists. See VGpuResource::Reload() for more information.
  /// \sa VGpuResource::Reload
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;

  /// \brief
  ///   Unloads the hardware resource. See VGpuResource::Unload() for more information.
  /// \sa VGpuResource::Unload
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;
#endif

#if defined( _VR_GLES2 )
  /// \brief
  ///   Fills the resource's memory with the given data. See VGpuResource::FillWithData for more information.
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
  ///
  /// \sa VGpuResource::FillWithData
  VISION_APIFUNC virtual void FillWithData( const void* pSourceData, unsigned int uiOffset, unsigned int uiSize, unsigned int uiLockFlags = VIS_LOCKFLAG_NOOVERWRITE ) HKV_OVERRIDE;
#endif

#if defined( _VR_DX9 )
  /// \brief
  ///   Lock the GPU resource for reading and/or writing. See VGpuResource::Lock() for more information.
  ///
  /// \param uiLockFlags
  ///   The lockflags to use when accessing the memory. 
  ///
  /// \param uiOffset
  ///   The offset at which the locked memory should start.
  ///
  /// \param uiSize
  ///   How much memory should be locked. (Specify 0 for the whole buffer)  
  /// \sa VGpuResource::Lock
  VISION_APIFUNC virtual void* Lock( unsigned int uiLockFlags, unsigned int uiOffset = 0, unsigned int uiSize = 0 ) HKV_OVERRIDE;
#endif

#if defined( _VR_DX9 ) || defined( _VR_GLES2 )
  /// \brief
  ///   Unlock the GPU resource. See VGpuResource::Unlock() for more information.
  /// \sa VGpuResource::Unlock
  VISION_APIFUNC virtual void Unlock() HKV_OVERRIDE;
#endif

  // internal use only
#if defined( _VR_DX11 ) || defined( _VR_DX9 )
  D3DVertexBuffer* GetVertexBuffer() const { return static_cast< D3DVertexBuffer* >( m_pResource ); }
#endif

#if defined( _VISION_XENON )
  void* GetBufferPtr() { return m_pBufferPtr; }
#endif


protected:
  // (de-)initialize platform-specific data
  inline void InitVertexBuffer_Internal();
  inline void DeInitVertexBuffer_Internal();

#if defined( _VISION_XENON )
  void* m_pBufferPtr;
#endif
};
typedef VSmartPtr< VVertexBuffer > VVertexBufferPtr;

#endif // VIS_API_GPU_BUFFER_HPP

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
