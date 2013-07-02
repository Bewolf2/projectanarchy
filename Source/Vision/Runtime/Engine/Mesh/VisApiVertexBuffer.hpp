/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexBuffer.hpp

#ifndef VISAPIVERTEXBUFFERS_HPP_INCLUDED
#define VISAPIVERTEXBUFFERS_HPP_INCLUDED

#include <Vision/Runtime/Engine/System/Resource/VisApiResource.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityObject.hpp>
#include <Vision/Runtime/Engine/Renderer/Buffer/VisApiGpuBuffers.hpp>

class VisMeshBuffer_cl;
class VisMeshBufferObject_cl;

typedef VSmartPtr<VisMeshBuffer_cl>       VisMeshBufferPtr;
typedef VSmartPtr<VisMeshBufferObject_cl> VisMeshBufferObjectPtr;
typedef VSmartPtr<VisVisibilityObject_cl> VisVisibilityObjectPtr;




/// \brief
///   Defines a simple (and commonly used) vertex structure. It supports an already initialised
///   descriptor as a static member
/// 
/// The vertex format supports position, color and a single set of 2D texture coordinates.
/// 
/// \sa VisMBVertexDescriptor_t
/// \sa VisMeshBuffer_cl
/// \sa VisMeshBuffer_cl::AllocateVertices
/// \sa VisMeshBufferObject_cl
struct VisMBSimpleVertex_t
{
public:

  /// \brief
  ///   Constructor, leaves all members uninitialized
  inline VisMBSimpleVertex_t() : pos(hkvNoInitialization),texcoord0(hkvNoInitialization) 
  {
  }

  hkvVec3 pos;              ///< position vector (x/y/z)
  DWORD color;                ///< primary color (red/green/blue/alpha, each [0..255]).
  hkvVec2 texcoord0;   ///< one set of texture coordinates
  static VISION_APIDATA VisMBVertexDescriptor_t VertexDescriptor; ///< static descriptor for this structure. It is initialised after Vision has been initialized
};



/// \brief
///   Enum that defines how a mesh buffer stores its data
enum VMeshBufferStorageMode_e
{
  V_MBSTORAGE_FROMFILE = 0,         ///< Loading from file - let the file format decide
  V_MBSTORAGE_FORCESINGLEBUFFER = 1,///< Use single buffered vertex buffers
  V_MBSTORAGE_FORCEDOUBLEBUFFER = 2 ///< Use double buffered vertex buffers
};


/// \brief
///   An instance of this class defines a mesh buffer consisting of a set of vertices, and
///   optionally an index list (for rendering indexed buffers).
/// 
/// It also defines a render state (either from shaders or alternatively a basic one with definable
/// transparency).
/// 
/// Mesh buffers are useful to render all kinds of custom geometry. They provide a relatively
/// low-level interface for rendering, but are highly versatile.
/// 
/// A mesh buffer can be rendered via a VisMeshBufferObject_cl instance which keeps position,
/// rotation and render order.
/// 
/// Alternatively mesh buffers can be rendered directly in a render loop inside a
/// VisRenderLoopHelper_cl::BeginMeshRendering() / VisRenderLoopHelper_cl::EndMeshRendering()
/// block.
/// 
/// \sa VisMBVertexDescriptor_t
/// \sa VisMeshBufferObject_cl
/// \sa VisRenderLoopHelper_cl::BeginMeshRendering
/// \sa VisRenderLoopHelper_cl::EndMeshRendering
class VisMeshBuffer_cl : public VManagedResource, public VisRMElementManager_cl<VisMeshBuffer_cl *>
{
  V_DECLARE_SERIALX( VisMeshBuffer_cl, VISION_APIFUNC )
public:

  /// \brief
  ///   Enumeration that defines the primitive type used for rendering the buffer
  enum MB_PrimitiveType_e
  {
    MB_PRIMTYPE_TRILIST   = 0,          ///< Renders vertices as a triangle list (up to vertexcount/3 primitives).
    MB_PRIMTYPE_TRISTRIP  = 1,          ///< Renders vertices as a triangle strip (up to vertexcount-2 primitives).
    MB_PRIMTYPE_INDEXED_TRILIST   = 2,  ///< Renders vertices as indexed triangle list. An index buffer has to be allocated for this mode.
    MB_PRIMTYPE_INDEXED_TRISTRIP  = 3,  ///< Renders vertices as indexed triangle strip. An index buffer has to be allocated for this mode.
    MB_PRIMTYPE_LINELIST  = 4,          ///< Renders vertices as lines (start/end pairs, up to vertexcount/2 lines).
    MB_PRIMTYPE_INDEXED_LINELIST  = 5,   ///< Renders vertices as indexed lines (index buffer contains start/end pairs).
    MB_PRIMTYPE_POINTLIST = 6            ///< Renders vertices as point list.
  };


  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor; nothing gets allocated, primitive type is default (MB_PRIMTYPE_TRILIST), no
  ///   Fast Copy used.
  VISION_APIFUNC VisMeshBuffer_cl();
  
  /// \brief
  ///   Constructor that defines the vertex format and allocates vertices and indices
  /// 
  /// \param descr
  ///   the descriptor of the vertex format; must be a valid descriptor
  /// 
  /// \param iVertexCount
  ///   Number of vertices to be allocated.
  /// 
  /// \param ePrimType
  ///   The primitive type. Consider that some types require indices to be allocated
  /// 
  /// \param iIndexCount
  ///   Number of indices to allocate (can be 0)
  /// 
  /// \param iPrimCount
  ///   Number of primitives to be rendered; If this parameter is <0, the maximum number of
  ///   primitives that derives from primitive type and allocated vertices/indices is taken  (e.g.
  ///   for MB_PRIMTYPE_TRILIST : max. prim count = iVertexCount/3)
  /// 
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for vertex data that is specified
  ///   only once and then never changed. STREAM should be used for vertex data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_UAV_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param bDoubleBufferedVertices
  ///   Specifies whether this mesh buffer should use a double-buffered vertex buffer. The default
  ///   value is true, which means that the vertex data will be double-buffered in system memory.
  ///   Consequently, the memory returned by any Lock operations on the vertex data will reside in
  ///   (cached) system memory, and there will be full support for serialization and
  ///   deserialization as well as read operations. If double-buffering is not used, there may be
  ///   no or limited support for serialization/ deserialization or read operations, and Lock
  ///   operations may return non-cached memory. Additionally, you will have to provide your own
  ///   handling for lost device cases. Single-buffered vertex data is recommended when a low
  ///   memory impact and best update performance is crucial and more important than flexibility.
  ///   It is important to note that when using DX10/11, Lock() operations are NOT permitted on
  ///   single-buffered vertex or index data with the usage flags VIS_MEMUSAGE_STATIC or
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED!
  /// 
  /// \param bDoubleBufferedIndices
  ///   Specifies whether the index list is double-buffered or not. If it isn't, you will have to
  ///   handle lost device events (PC DX9 only) yourself.
  /// 
  /// \param iAdditionalVertexBindFlag
  ///   In DX10/DX11 you can specify additional bind flags for the vertex buffer as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  /// 
  /// \param iAdditionalIndexBindFlag
  ///   In DX10/DX11 you can specify additional bind flags for the index buffer as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  /// 
  /// \note
  ///   Use this constructor to initialize the buffer. You can also reallocate vertex and index
  ///   counts at any time.
  /// 
  /// \note
  ///   The iPrimCount variable is passed to the OnRender function of the object; if you overload
  ///   the OnRender function you can pass a different primitive count to the base class to render
  /// 
  /// \sa VisMeshBuffer_cl::AllocateVertices
  /// \sa VisMeshBuffer_cl::AllocateIndexList
  /// \sa VisMeshBuffer_cl::SetPrimitiveCount
  /// \sa VisMeshBuffer_cl::SetPrimitiveType
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  /// \sa VisMeshBufferObject_cl::OnRender
  VISION_APIFUNC VisMeshBuffer_cl(const VisMBVertexDescriptor_t &descr, int iVertexCount, MB_PrimitiveType_e ePrimType=MB_PRIMTYPE_TRILIST, int iIndexCount=0, int iPrimCount=-1, int iUsageFlag = VIS_MEMUSAGE_STREAM, bool bDoubleBufferedVertices = true, bool bDoubleBufferedIndices = true, int iAdditionalVertexBindFlag=0, int iAdditionalIndexBindFlag = 0);

  /// \brief
  ///   Destructor; cleans up its resources.
  VISION_APIFUNC virtual ~VisMeshBuffer_cl();

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   Saves the mesh buffer data to a binary file. This includes mesh data, index data and render
  ///   states.
  /// 
  /// Mesh buffers can thus be swapped and reloaded using LoadFromFile/ReadFromStream.
  /// 
  /// \param szFilename
  ///   Filename of the mesh buffer binary file. The file extension VMB will be added
  /// 
  /// \param bSetFilename
  ///   If TRUE, the internal resource name will be set to the new filename
  /// 
  /// \return
  ///   BOOL bResult : TRUE if saving was successful
  /// 
  /// \sa VisMeshBuffer_cl::WriteToStream
  /// \sa VisMeshBuffer_cl::LoadFromFile
  /// \sa VisMeshBuffer_cl::ReadFromStream
  /// \sa VisMeshBuffer_cl::SerializeX
  VISION_APIFUNC BOOL SaveToFile(const char *szFilename, BOOL bSetFilename=TRUE);

  /// \brief
  ///   Saves the mesh buffer data to a binary stream. This includes mesh data, index data and
  ///   render states.
  /// 
  /// Same as SaveToFile but uses a file stream directly. Thus multiple buffers can be written to a
  /// file.
  /// 
  /// This function does not close the file in the end.
  /// 
  /// \param pOutStream
  ///   valid file stream to write the data to
  /// 
  /// \return
  ///   BOOL bResult : TRUE saving was successful
  /// 
  /// \sa VisMeshBuffer_cl::SaveToFile
  /// \sa VisMeshBuffer_cl::LoadFromFile
  /// \sa VisMeshBuffer_cl::ReadFromStream
  /// \sa VisMeshBuffer_cl::SerializeX
  VISION_APIFUNC BOOL WriteToStream(IVFileOutStream *pOutStream);

  /// \brief
  ///   Loads mesh buffer data from a binary file
  /// 
  /// The file must contain the mesh buffer data in a valid format used by
  /// WriteToStream/SaveToFile.
  /// 
  /// The old mesh buffer data is discarded. This mesh buffer will be re-constructed with new
  /// vertices, indices, primitive type, transparency and effect information.
  /// 
  /// \param szFilename
  ///   Filename of the mesh buffer binary file. The file extension VMB will be added
  /// 
  /// \param bSetFilename
  ///   If TRUE, the internal resource name will be set to the new filename
  /// 
  /// \return
  ///   BOOL bResult : TRUE if loading was successful
  /// 
  /// \sa VisMeshBuffer_cl::ReadFromStream
  /// \sa VisMeshBuffer_cl::SaveToFile
  /// \sa VisMeshBuffer_cl::WriteToStream
  /// \sa VisMeshBuffer_cl::SerializeX
  VISION_APIFUNC BOOL LoadFromFile(const char *szFilename, BOOL bSetFilename=TRUE);

  /// \brief
  ///   Reads mesh buffer data from a binary stream
  /// 
  /// Same as LoadFromFile but uses a file stream directly. Thus multiple buffers can be loaded
  /// from file.
  /// 
  /// The old mesh buffer data is discarded. This mesh buffer will be re-constructed with new
  /// vertices, indices, primitive type, transparency and effect information.
  /// 
  /// \param pInStream
  ///   Valid stream to read from
  /// 
  /// \param eStorageMode
  ///   If V_MBSTORAGE_FROMFILE, the information stored in the loaded file determines whether the vertex and index
  ///   data is stored in single-buffered or double-buffered mode. With V_MBSTORAGE_FORCESINGLEBUFFERED, you can force
  ///   the mesh data to be stored in single-buffered mode. V_MBSTORAGE_FORCEDOUBLEBUFFERED forces the mesh data to
  ///   be stored in double-buffered mode.
  ///
  /// \param iVertexUsageFlagsOverride
  ///   Can be used to override the usage flags for vertex data. -1 signifies that the usage flags are taken from the
  ///   file; otherwise, the passed value is used.
  /// 
  /// \param iIndexUsageFlagsOverride
  ///   Can be used to override the usage flags for index data. -1 signifies that the usage flags are taken from the
  ///   file; otherwise, the passed value is used.
  /// 
  /// \param iVertexBindFlagsOverride
  ///   Can be used to override the additional bind flags for vertex data. -1 signifies that the bind flags are taken from the
  ///   file; otherwise, the passed value is used.
  /// 
  /// \param iIndexBindFlagsOverride
  ///   Can be used to override the bind flags for index data. -1 signifies that the bind flags are taken from the
  ///   file; otherwise, the passed value is used.
  /// 
  /// \return
  ///   BOOL bResult : TRUE if loading was successful
  /// 
  /// \sa VisMeshBuffer_cl::LoadFromFile
  /// \sa VisMeshBuffer_cl::SaveToFile
  /// \sa VisMeshBuffer_cl::WriteToStream
  /// \sa VisMeshBuffer_cl::SerializeX
  VISION_APIFUNC BOOL ReadFromStream(IVFileInStream *pInStream, VMeshBufferStorageMode_e eStorageMode = V_MBSTORAGE_FROMFILE, int iVertexUsageFlagsOverride = -1, int iIndexUsageFlagsOverride = -1, int iVertexBindFlagsOverride = -1, int iIndexBindFlagsOverride = -1);

  /// \brief
  ///   SerializeX function for both reading and writing from/to archive.
  /// 
  /// The file format is compatible with the binary data used by the LoadFromFile/SaveToFile
  /// function.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   Archive to write to/ to load from
  /// 
  /// \sa VisMeshBuffer_cl::LoadFromFile
  /// \sa VisMeshBuffer_cl::ReadFromStream
  /// \sa VisMeshBuffer_cl::SaveToFile
  /// \sa VisMeshBuffer_cl::WriteToStream
  VISION_APIFUNC void SerializeX(VArchive &ar);


  ///
  /// @}
  ///

  ///
  /// @name Basic Mesh Properties
  /// @{
  ///

  /// \brief
  ///   Sets the primitive type of the mesh buffer
  /// 
  /// This type defines how to interprete the vertices resp. indices for rendering.
  /// 
  /// The primitive type can be changed at any time.
  /// 
  /// \param ePrimType
  ///   the new primitive type. See also MB_PrimitiveType_e
  /// 
  /// \sa MB_PrimitiveType_e
  /// \sa VisMeshBufferObject_cl::OnRender
  /// \sa VisMeshBuffer_cl::GetPrimitiveType
  inline void SetPrimitiveType(MB_PrimitiveType_e ePrimType);


  /// \brief
  ///   Get the primitive type of the buffer that has been set via SetPrimitiveType.
  /// 
  /// \return
  ///   MB_PrimitiveType_e: The current primitive type of the buffer
  /// 
  /// \sa VisMeshBuffer_cl::SetPrimitiveType
  inline MB_PrimitiveType_e GetPrimitiveType() const;

  /// \brief
  ///   Get the current vertex descriptor
  /// 
  /// \param descr
  ///   structure that receives a copy of the current vertex format descriptor
  /// 
  /// \sa VisMeshBuffer_cl::AllocateVertices
  /// \sa VisMBVertexDescriptor_t
  inline void GetVertexDescriptor(VisMBVertexDescriptor_t &descr) const;

  /// \brief
  ///   Evaluates the bitmask (each bit of type VERTEX_STREAM_xyz) for this mesh buffer
  inline unsigned int GetStreamMask() const
  {
    unsigned int iMask = m_VertexDescr.GetStreamMask();
    return (GetIndexCount()>0) ? (iMask|VERTEX_STREAM_INDEXBUFFER) : iMask;
  }

  ///
  /// @}
  ///

  ///
  /// @name Allocate and Free
  /// @{
  ///

  /// \brief
  ///   Allocates vertices for the buffer
  /// 
  /// This function allocates stride*iCount bytes of memory, where stride comes from the descriptor
  /// structure.
  /// 
  /// You can re-allocate a different number of vertices at any time; This function will also free
  /// vertices from a previous allocation.
  /// 
  /// \param descr
  ///   A reference to a valid descriptor. The buffer copies this descriptor and uses it for setting
  ///   up the streams at rendering time. If this descriptor is not valid, a fatal error occurs.
  /// 
  /// \param iCount
  ///   number of vertices to be allocated; if 16 Bit indices are used, this must not be larger
  ///   than 65535.
  /// 
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for vertex data that is specified
  ///   only once and then never changed. STREAM should be used for vertex data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_UAV_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param bDoubleBuffered
  ///   Specifies whether this mesh buffer should use a double-buffered vertex buffer. The default
  ///   value is true, which means that the vertex data will be double-buffered in system memory.
  ///   Consequently, the memory returned by any Lock operations on the vertex data will reside in
  ///   (cached) system memory, and there will be full support for serialization and
  ///   deserialization as well as read operations. If double-buffering is not used, there may be
  ///   no or limited support for serialization/ deserialization or read operations, and Lock
  ///   operations may return non-cached memory. Additionally, you will have to provide your own
  ///   handling for lost device cases. Single-buffered vertex data is recommended when a low
  ///   memory impact and best update performance is crucial and more important than flexibility.
  ///   It is important to note that when using DX10/11, Lock() operations are NOT permitted on
  ///   single-buffered vertex or index data with the usage flags VIS_MEMUSAGE_STATIC or
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED!
  /// 
  /// \param iAdditionalBindFlag
  ///   In DX10/DX11 you can specify additional bind flags as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  /// 
  /// \return
  ///   BOOL result: TRUE, if vertices could be allocated, FALSE if not
  /// 
  /// \sa VisMeshBuffer_cl::FreeVertices
  /// \sa VisMeshBuffer_cl::AllocateIndexList
  /// \sa VisMeshBuffer_cl::LockVertices
  VISION_APIFUNC BOOL AllocateVertices(const VisMBVertexDescriptor_t &descr, int iCount, int iUsageFlag=VIS_MEMUSAGE_STATIC, bool bDoubleBuffered = true, int iAdditionalBindFlag=0);

  /// \brief
  ///   Releases the vertex memory
  VISION_APIFUNC void FreeVertices();

  /// \brief
  ///   Allocates an index list for the buffer
  /// 
  /// Index lists are required for primitive type MB_PRIMTYPE_INDEXED_TRILIST and
  /// MB_PRIMTYPE_INDEXED_TRISTRIP For other types, indices are ignored.
  /// 
  /// A single index is either an unsigned short (16 Bits) or an unsigned int (32 Bits), depending
  /// on the buffer type.
  /// 
  /// You can allocate a new number of indices at any time; this function will call FreeIndexList
  /// for previously allocated index lists.
  /// 
  /// \param iIndexCount
  ///   Number of indices to allocate
  /// 
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for index data that is specified
  ///   only once and then never changed. STREAM should be used for index data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_UAV_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param eIndexFormat
  ///   Format of the index buffer. Use VIS_INDEXFORMAT_16 for unsigned short (16 bit) index values or
  ///   VIS_INDEXFORMAT_32 for unsigned int (32 bit) index values.
  /// 
  /// \param bDoubleBuffered
  ///   Specifies whether the index list is double-buffered or not. If it isn't, you will have to
  ///   handle lost device events (PC DX9 only) yourself. It is important to note that when using
  ///   DX10/11, Lock() operations are NOT permitted on single-buffered vertex or index data with the
  ///   usage flags VIS_MEMUSAGE_STATIC or VIS_MEMUSAGE_STATIC_UNMANAGED!
  /// 
  /// \param iAdditionalBindFlag
  ///   In DX10/DX11 you can specify additional bind flags as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  ///
  /// \return
  ///   BOOL: TRUE, if indices could be allocated, otherwise FALSE
  /// 
  /// \sa VisMeshBuffer_cl::FreeIndexList
  /// \sa VisMeshBuffer_cl::AllocateVertices
  /// \sa VisMeshBuffer_cl::LockIndices
  VISION_APIFUNC BOOL AllocateIndexList(int iIndexCount, int iUsageFlag=VIS_MEMUSAGE_STATIC, VIndexFormat eIndexFormat = VIS_INDEXFORMAT_16, bool bDoubleBuffered = true, int iAdditionalBindFlag=0 );

  /// \brief
  ///   Releases the memory for indices
  VISION_APIFUNC void FreeIndexList();

  /// \brief
  ///   Return the usage flags that have been passed to AllocateVertices
  inline unsigned int GetVertexUsageFlags() const 
  {
    return m_iMemUsageFlagVertices;
  }

  /// \brief
  ///   Return the bind flags that have been passed to AllocateVertices
  inline unsigned int GetVertexBindFlags() const 
  {
    return m_iBindFlagVertices;
  }

  /// \brief
  ///   Return the usage flags that have been passed to AllocateIndexList
  inline unsigned int GetIndexUsageFlags() const
  {
    return m_iMemUsageFlagIndices;
  }

  /// \brief
  ///   Return the bind flags that have been passed to AllocateIndexList
  inline unsigned int GetIndexBindFlags() const
  {
    return m_iBindFlagIndices;
  }

  /// \brief
  ///   Returns whether vertices/ indices are single or double buffered.
  /// 
  /// \param bVerticesDoubleBuffered
  ///   Storage mode for vertices.
  /// 
  /// \param bIndicesDoubleBuffered
  ///   Storage mode for indices.
  /// 
  /// \param bDoubleBufferingFromFile 
  ///   If value is true storage modes will be always determined from corresponding mesh file, global settings will be ignored.
  inline void GetDoubleBuffering(bool &bVerticesDoubleBuffered, bool &bIndicesDoubleBuffered, bool &bDoubleBufferingFromFile) const
  {
    bVerticesDoubleBuffered = m_bVerticesDoubleBuffered;
    bIndicesDoubleBuffered = m_bIndicesDoubleBuffered;
    bDoubleBufferingFromFile = m_bDoubleBufferingFromFile;
  }

  /// \brief
  ///   Sets whether vertices/ indices should be single or double buffered.
  /// 
  /// \param bVerticesDoubleBuffered
  ///   Storage mode for vertices.
  /// 
  /// \param bIndicesDoubleBuffered
  ///   Storage mode for indices.
  /// 
  /// \param bDoubleBufferingFromFile 
  ///   If value is set to true storage modes will be always determined from corresponding mesh file, global settings will be ignored.
  ///   Otherwise storage modes will be overwritten by global settings.
  inline void SetDoubleBuffering(bool bVerticesDoubleBuffered, bool bIndicesDoubleBuffered, bool bDoubleBufferingFromFile)
  {
    m_bVerticesDoubleBuffered = bVerticesDoubleBuffered;
    m_bIndicesDoubleBuffered = bIndicesDoubleBuffered;
    m_bDoubleBufferingFromFile = bDoubleBufferingFromFile;
  }

  ///
  /// @}
  ///

  ///
  /// @name Vertex and Primitive Count
  /// @{
  ///

  /// \brief
  ///   Set the default number of primitives to be rendered
  /// 
  /// By default, the maximum number of primitives is rendered
  /// 
  /// \param iNewCount
  ///   Number of primitives to be rendered. This number must not exceed the maximum number. If -1,
  ///   always the maximum number is rendered
  /// 
  /// \return
  ///   BOOL: TRUE, if the new number of primitives is valid, FALSE if not
  /// 
  /// \note
  ///   By default the maximum number is used.
  /// 
  /// \note
  ///   The number of primitives is passed to the OnRender function of the mesh buffer object, it
  ///   can still be changed there.
  /// 
  /// \note
  ///   The maximum number is calculated in the following manner:
  ///   \li for type MB_PRIMTYPE_TRILIST:  max count = numVertices / 3,
  /// 
  ///   \li for type MB_PRIMTYPE_TRISTRIP: max count = numVertices - 2,
  /// 
  ///   \li for type MB_PRIMTYPE_INDEXED_TRILIST:  max count = numIndices / 3,
  /// 
  ///   \li for type MB_PRIMTYPE_INDEXED_TRISTRIP: max count = numIndices - 2,
  /// 
  ///   \li for type MB_PRIMTYPE_LINELIST: max count = numVertices / 2
  /// 
  ///   \li for type MB_PRIMTYPE_INDEXED_LINELIST: max count = numIndices / 2
  /// 
  ///   \li for type MB_PRIMTYPE_POINTLIST: max count = numVertices
  ///
  /// \sa VisMeshBufferObject_cl::OnRender
  /// \sa VisMeshBuffer_cl::GetCurrentPrimitiveCount
  VISION_APIFUNC BOOL SetPrimitiveCount(int iNewCount=-1);

  /// \brief
  ///   Returns the number of primitives
  /// 
  /// This number is either the number defined via SetPrimitiveCount or (by default) the maximum
  /// number.
  /// 
  /// \return
  ///   int iPrimCount: The current primitive count
  /// 
  /// \sa VisMeshBufferObject_cl::OnRender
  /// \sa VisMeshBuffer_cl::SetCurrentPrimitiveCount
  VISION_APIFUNC int GetCurrentPrimitiveCount() const;

  
  /// \brief
  ///   Returns the allocated number of vertices
  /// 
  /// \return
  ///   int iCount: Number of allocated vertices 
  inline int GetVertexCount() const;

  /// \brief
  ///   Returns the allocated number of indices
  /// 
  /// \return
  ///   int iCount: Number of allocated indices 
  inline int GetIndexCount() const;

  ///
  /// @}
  ///

  ///
  /// @name Locking and Unlocking
  /// @{
  ///

  /// \brief
  ///   Returns true if the mesh buffer can be locked for reading
  inline bool CanBeLockedForReading() const;

  /// \brief
  ///   Locks the vertex array to get write access to the vertex data
  /// 
  /// This function returns a pointer that can be casted to a pointer to vertex format specified by
  /// the descriptor. 
  /// 
  /// The buffer can be modified until it is unlocked.
  /// 
  /// Unlock the buffer immediately after writing. Rendering with a locked buffer causes a fatal
  /// error.
  /// 
  /// Don't store the pointer, since it might be invalid after unlocking.
  /// 
  /// Note that partial locks generally improve performance, since only parts of the data have to
  /// be copied in this case. This is especially true for buffers that hold a copy in fast geometry
  /// memory.
  /// 
  /// Make sure to keep the number of lock/unlock operations as low as possible, especially when
  /// using mesh buffers that hold a copy in fast geometry memory.
  /// 
  /// On Xbox360 you always have to lock the whole range (first=0, count=-1).
  /// 
  /// On PC DX10/11, locking vertices is not permitted for single-buffered vertex data allocated as
  /// static. Use FillVerticesWithData instead.
  /// 
  /// \param flags
  ///   Flags for locking the buffer. This parameter can also be 0. Currently the following flags
  ///   are supported: 
  ///   \li VIS_LOCKFLAG_DISCARDABLE  : If specified, it is not guaranteed that the
  ///     vertex data is the same as last time (instead, the array might be uninitialized). However,
  ///     this flag might improve performance. 
  ///   \li VIS_LOCKFLAG_READONLY     : If specified, the user is
  ///     only allowed to read from the buffer. Unlocking will then not update the buffer, which
  ///     saves performance 
  ///   \li VIS_LOCKFLAG_NOOVERWRITE  : If specified, you have to guarantee that no
  ///     data still required for rendering by the GPU will be overwritten. Can not be combined with
  ///     the flag "discardable". Has no functionality on PS3/Xbox360.
  /// 
  /// \param first
  ///   The index of the first vertex to be locked. This can be used for only locking parts of a
  ///   mesh buffer, which usually improves performance. If not specified, this index is set to the
  ///   first vertex.
  /// 
  /// \param count
  ///   The number of vertices to be locked. This can be used for only locking parts of a mesh
  ///   buffer, which usually improves performance. If not specified, all remaining vertices
  ///   starting with the vertex specified by the "first" parameter are locked.
  /// 
  /// \return
  ///   void *pVertexData : pointer to vertex buffer (can be casted to used vertex structure). NULL
  ///   is returned if the locking failed (e.g. invalid number of vertices, invalid index values
  ///   specified). Note that if the "first" parameter is set to a non-zero value, the pointer
  ///   returned will point to the memory position of that vertex, NOT the first vertex in the
  ///   buffer.
  /// 
  /// \sa VisMeshBuffer_cl::UnLockVertices
  /// \sa VisMeshBuffer_cl::LockIndices
  VISION_APIFUNC void *LockVertices(int flags, int first=0, int count=-1);

  /// \brief
  ///   Unlocks the vertex buffer and uploads the modified data
  /// 
  /// \sa VisMeshBuffer_cl::LockVertices
  VISION_APIFUNC void UnLockVertices();

  /// \brief
  ///   Locks the index buffer to get write access to it.
  /// 
  /// This function always returns a pointer to unsigned short values.
  /// 
  /// Each index references a vertex in the vertex buffer (0-based).
  /// 
  /// The buffer can be modified until it is unlocked.
  /// 
  /// Unlock the index buffer immediately after writing to it.
  /// 
  /// On PC DX10/11, locking indices is not permitted for single-buffered index data allocated as
  /// static. Use FillIndicesWithData instead.
  /// 
  /// \param flags
  ///   Flags for locking the buffer. This parameter can also be 0. Currently the following flags
  ///   are supported:
  ///   \li VIS_LOCKFLAG_DISCARDABLE  : If specified, it is not guaranteed, that the index data is
  ///     the same as last time (instead, it might be uninitialized). However, this flag might
  ///     improve performance.
  /// 
  ///   \li VIS_LOCKFLAG_READONLY     : If specified, the user is only allowed to read from the
  ///     buffer. Unlocking will then not update the buffer, which saves performance
  /// 
  ///   \li VIS_LOCKFLAG_NOOVERWRITE  : If specified, you have to guarantee that no data still
  ///     required for rendering by the GPU will be overwritten. Can not be combined with the flag
  ///     "discardable". Has no functionality on PS3/Xbox360.
  /// 
  /// \param first
  ///   The first index to lock. 0 indicates the start of the index buffer.
  /// 
  /// \param count
  ///   The number of index values to lock.
  /// 
  /// \return
  ///   unsigned short *pIndices : a pointer to the indices (if allocated, otherwise NULL)
  /// 
  /// \sa VisMeshBuffer_cl::UnLockIndices
  /// \sa VisMeshBuffer_cl::LockVertices
  VISION_APIFUNC void *LockIndices(int flags, int first = 0, int count = -1);

  /// \brief
  ///   Unlocks the index buffer and uploads the modified data.
  /// 
  /// \sa VisMeshBuffer_cl::LockIndices
  /// \sa VisMeshBuffer_cl::UnLockVertices
  VISION_APIFUNC void UnLockIndices();

  /// \brief
  ///   Fills the vertex buffer with custom data.
  /// 
  /// \param pSourceData
  ///   Pointer to the source data with which to update the vertex buffer.
  /// 
  /// \param iMemSize
  ///   The number of bytes to update.
  /// 
  /// \param bNoOverwrite
  ///   If true, the application promises not to overwrite any data that may currently still be in
  ///   use by the GPU. Setting this flag to true when appropriate may have significant performance
  ///   advantages on some platforms.
  VISION_APIFUNC void FillVerticesWithData(const void* pSourceData, int iMemSize, bool bNoOverwrite = false);

  /// \brief
  ///   Fills the index buffer with custom data.
  /// 
  /// \param pSourceData
  ///   Pointer to the source data with which to update the index buffer.
  /// 
  /// \param iMemSize
  ///   The number of bytes to update.
  /// 
  /// \param bNoOverwrite
  ///   If true, the application promises not to overwrite any data that may currently still be in
  ///   use by the GPU. Setting this flag to true when appropriate may have significant performance
  ///   advantages on some platforms.
  VISION_APIFUNC void FillIndicesWithData(const void* pSourceData, int iMemSize, bool bNoOverwrite = false);

  /// \brief
  ///  Returns if the vertices of the mesh buffer are currently locked or not.
  ///
  inline bool AreVerticesLocked() const
  {
    return m_bVerticesLocked;
  }

  /// \brief
  ///  Returns if the indices of the mesh buffer are currently locked or not.
  ///
  inline bool AreIndicesLocked() const
  {
    return m_bIndicesLocked;
  }


  ///
  /// @}
  ///

  ///
  /// @name Default Render State
  /// @{
  ///

  /// \brief
  ///   Sets the base texture of the buffer
  /// 
  /// The base texture is used for textured vertex buffers (for all stages, if no shaders are
  /// applied)
  /// 
  /// It is also referred as "BaseTexture" when applying shader.
  /// 
  /// Use the SetChannelTexture function to set the texture for a specific sampler stage.
  /// 
  /// Setting the base texture will overwrite all MAX_MESHBUFFER_TEXCOORDS channel textures with
  /// this texture.
  /// 
  /// This version takes a Vision texture object directly.
  /// 
  /// \param pTextureObject
  ///   the texture object to use as base texture
  /// 
  /// \sa VisMeshBuffer_cl::GetBaseTexture
  /// \sa VisMeshBuffer_cl::SetChannelTexture
  inline void SetBaseTexture(VTextureObject *pTextureObject);

  /// \brief
  ///   Sets the base texture of the buffer
  /// 
  /// The base texture is used for textured vertex buffers (for all stages, if no shaders are
  /// applied)
  /// 
  /// It is also referred as "BaseTexture" when applying shader
  /// 
  /// Use the SetChannelTexture function to set the texture for a specific sampler stage.
  /// 
  /// Setting the base texture will overwrite all MAX_MESHBUFFER_TEXCOORDS channel textures with
  /// this texture
  /// 
  /// \param szFilename
  ///   filename of the texture to load
  /// 
  /// \return
  ///   VTextureObject* pTexture : pointer to Vision texture object, if loading was successful,
  ///   NULL otherwise
  /// 
  /// \sa VisMeshBuffer_cl::GetBaseTexture
  /// \sa VisMeshBuffer_cl::SetChannelTexture
  VISION_APIFUNC VTextureObject* SetBaseTexture(const char *szFilename);

  /// \brief
  ///   Returns the Vision texture object of the current base texture.
  /// 
  /// If no base texture has been specified, it returns NULL.
  /// 
  /// This function wraps around GetChannelTexture(0).
  /// 
  /// \return
  ///   VTextureObject* pTexture : pointer to the texture object
  /// 
  /// \sa VisMeshBuffer_cl::SetBaseTexture
  /// \sa VisMeshBuffer_cl::SetChannelTexture
  inline VTextureObject* GetBaseTexture() const;

  /// \brief
  ///   Sets the texture object for a specific texture sampler stage
  /// 
  /// It is referred as "BaseTexture" for the respective sampler stage when applying shader.
  /// 
  /// This version takes a Vision texture object directly.
  /// 
  /// \param pTextureObject
  ///   the texture object to use
  /// 
  /// \param iChannel
  ///   The channel number, must be in valid range of [0..MAX_MESHBUFFER_TEXCOORDS-1] (maximum
  ///   number of  supported channels for Vision mesh buffers, currently 8)
  /// 
  /// \sa VisMeshBuffer_cl::SetBaseTexture
  /// \sa VisMeshBuffer_cl::GetChannelTexture
  inline void SetChannelTexture(VTextureObject *pTextureObject, int iChannel);

  /// \brief
  ///   Loads a texture file and sets the texture for a specific texture sampler stage.
  /// 
  /// It is referred as "BaseTexture" for the respective sampler stage when applying shader.
  /// 
  /// This version takes a Vision texture object directly.
  /// 
  /// \param szFilename
  ///   filename of the texture to load
  /// 
  /// \param iChannel
  ///   The channel number, must be in valid range of [0..MAX_MESHBUFFER_TEXCOORDS-1] (maximum
  ///   number of  supported channels for Vision mesh buffers, currently 8)
  /// 
  /// \return
  ///   VTextureObject* pTexture : pointer to the texture object, if loading was successful, NULL
  ///   otherwise
  /// 
  /// \sa VisMeshBuffer_cl::SetBaseTexture
  /// \sa VisMeshBuffer_cl::GetChannelTexture
  VISION_APIFUNC VTextureObject* SetChannelTexture(const char *szFilename, int iChannel);

  /// \brief
  ///   Returns the texture object for a specific texture sampler stage
  /// 
  /// \param iChannel
  ///   The channel number, must be in valid range of [0..MAX_MESHBUFFER_TEXCOORDS-1]
  /// 
  /// \return
  ///   VTextureObject* pTexture : pointer to Vision texture object of the respective channel
  /// 
  /// \sa VisMeshBuffer_cl::SetChannelTexture
  /// \sa VisMeshBuffer_cl::SetBaseTexture
  /// \sa VisMeshBuffer_cl::GetBaseTexture
  inline VTextureObject* GetChannelTexture(int iChannel) const;


  /// \brief
  ///   Sets the default transparency for rendering (if no shaders are applied)
  /// 
  /// Any shader effect applied to the mesh buffer or the mesh buffer object will supersede this
  /// mode, unless it uses the "blending from surface" flag.
  /// 
  /// \param eTransp
  ///   transparency type to use
  /// 
  /// \sa VisMeshBuffer_cl::GetDefaultTransparency
  /// \sa VIS_TransparencyType
  inline void SetDefaultTransparency(VIS_TransparencyType eTransp);

  /// \brief
  ///   Returns the default transparency type
  /// 
  /// \return
  ///   VIS_TransparencyType eType: transparency type
  /// 
  /// \sa VisMeshBuffer_cl::GetDefaultTransparency
  /// \sa VIS_TransparencyType
  inline VIS_TransparencyType GetDefaultTransparency() const;

  /// \brief
  ///   Sets the default flag for writing to z-buffer (if no shaders are applied)
  /// 
  /// the default status is TRUE.
  /// 
  /// Any shader effect applied to the mesh buffer or the mesh buffer object will supersede this
  /// flag
  /// 
  /// \param bStatus
  ///   New status. If TRUE, the geometry will write to the z-buffer.
  /// 
  /// \sa VisMeshBuffer_cl::IsDefaultZBufferWritingEnabled
  /// \sa VisMeshBuffer_cl::GetDefaultTransparency
  /// \sa VisMeshBuffer_cl::SetDefaultTransparency
  inline void EnableDefaultZBufferWriting(BOOL bStatus);

  /// \brief
  ///   Returns the default flag for writing to z-buffer set by EnableDefaultZBufferWriting
  /// 
  /// \return
  ///   BOOL bStatus : The current z-buffer writing status
  /// 
  /// \sa VisMeshBuffer_cl::EnableDefaultZBufferWriting
  /// \sa VisMeshBuffer_cl::GetDefaultTransparency
  /// \sa VisMeshBuffer_cl::SetDefaultTransparency
  inline BOOL IsDefaultZBufferWritingEnabled() const;

  /// \brief
  ///   Sets the default face culling mode for this mesh buffer (if no shaders are applied)
  /// 
  /// The default status is culling back faces (CULL_BACK).
  /// 
  /// Any shader effect applied to the mesh buffer or the mesh buffer object will supersede this
  /// mode.
  /// 
  /// To render the mesh buffer double-sided, use the CULL_NONE mode.
  /// 
  /// \param eCullMode
  ///   New face culling mode. Supported values are CULL_NONE, CULL_FRONT, and CULL_BACK.
  /// 
  /// \sa VisMeshBuffer_cl::GetDefaultCullMode
  /// \sa VisMeshBuffer_cl::SetDefaultTechnique
  /// \sa VisMeshBufferObject_cl::SetTechnique
  inline void SetDefaultCullMode(VCullMode eCullMode=CULL_BACK);

  /// \brief
  ///   Returns the current default face culling mode
  /// 
  /// This mode is set by SetDefaultCullMode.
  /// 
  /// \return
  ///   VCullMode eCullMode : The current culling mode
  /// 
  /// \sa VisMeshBuffer_cl::SetDefaultFaceCullMode
  /// \sa VisMeshBuffer_cl::SetDefaultTechnique
  /// \sa VisMeshBufferObject_cl::SetTechnique
  inline VCullMode GetDefaultCullMode() const;

  /// \brief
  ///   Sets the default render state flags. The passed value encodes transparency type, z-buffer
  ///   writing, culling, fog.
  /// 
  /// The passed value encodes transparency type, z-buffer writing, culling, fog.
  /// 
  /// Setting a new value overwrites the flags set by any of the functions SetDefaultTransparency,
  /// EnableDefaultZBufferWriting, and SetDefaultCullMode.
  /// 
  /// In addition, the bitflags for alpha test (RENDERSTATEFLAG_ALPHATEST) and
  /// RENDERSTATEFLAG_ALWAYSVISIBLE to ignore the z-buffer can be set.
  /// 
  /// All flags are only considered for fixed function rendering. Any shader effect applied either
  /// to the mesh buffer or the mesh buffer object will supersede these flags.
  /// 
  /// \param state
  ///   The new state constants
  inline void SetDefaultRenderState(const VSimpleRenderState_t &state) {m_iRenderState=state;}

  /// \brief
  ///   Returns the state set via SetDefaultRenderState
  inline VSimpleRenderState_t GetDefaultRenderState() const {return m_iRenderState;}

  /// \brief
  ///   Sets the default shader technique for this mesh buffer.
  /// 
  /// Setting a shader technique in the VisMeshBufferObject_cl object will supersede this default
  /// technique.
  /// 
  /// \param pTechnique
  ///   The new default technique, or NULL if no default technique should be used
  inline void SetDefaultTechnique(VCompiledTechnique *pTechnique)
  {
    m_spDefaultTechnique = pTechnique;
  }

  /// \brief
  ///   Returns the default shader technique for this mesh buffer.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: The default technique, or NULL if no default technique is
  ///   in use for this mesh buffer
  inline VCompiledTechnique *GetDefaultTechnique() const
  {
    return m_spDefaultTechnique;
  }


  ///
  /// @}
  ///

  ///
  /// @name Projection
  /// @{
  ///

  
  /// \brief
  ///   Sets the status of using the projection matrix for rendering.
  /// 
  /// The default status is enabled, which means the mesh buffer is rendered as usual geometry in
  /// the scene.
  /// 
  /// When disabled, the mesh buffer is rendered screen aligned, which might be useful for GUI
  /// related rendering.
  /// 
  /// When disabled, the coordinate system for all vertex position coordinates ranges from -1 to 1.
  /// (0,0,0) means screen center, x=-1, y=1 means upper left screen corner. z=-1 specifies the
  /// distance of the near clip plane, z=1 is the far clip plane.
  /// 
  /// When disabled, the positioning and rotation of the referencing mesh buffer object is ignored.
  /// 
  /// \param bStatus
  ///   The new status for using the projection matrix
  /// 
  /// \sa VisMeshBuffer_cl::GetUseProjectionMatrix
  inline void SetUseProjectionMatrix(BOOL bStatus);


  /// \brief
  ///   Retrieves the current status of using the projection matrix for rendering.
  /// 
  /// \return
  ///   BOOL bStatus: The current status for using the projection matrix
  /// 
  /// \sa VisMeshBuffer_cl::SetUseProjectionMatrix
  inline BOOL GetUseProjectionMatrix() const;


  ///
  /// @}
  ///

  ///
  /// @name Tracing
  /// @{
  ///


  /// \brief
  ///   Fills in a collision mesh structure using this mesh buffer or parts of it
  /// 
  /// This function handles all primitive types properly.
  /// 
  /// Use this function only for 16 bit index buffers; for 32 bit index buffers, use
  /// CreateCollisionMesh32 instead.
  /// 
  /// \param pDestMesh
  ///   The destination collision mesh that will be filled with mesh information.
  /// 
  /// \param iFirstPrimitive
  ///   The first primitive in the collision mesh to start with.
  /// 
  /// \param iPrimitiveCount
  ///   The number of primitives to copy starting at iFirstPrimitive. Can be -1 to use all
  ///   remaining.
  /// 
  /// \param bFlipFaces
  ///   Specify this flag to flip each face
  /// 
  /// \return
  ///   BOOL bStatus: TRUE if successful
  VISION_APIFUNC BOOL CreateCollisionMesh(VSimpleCollisionMesh *pDestMesh, int iFirstPrimitive=0, int iPrimitiveCount=-1, BOOL bFlipFaces=FALSE);

  /// \brief
  ///   Fills in a collision mesh structure using this mesh buffer or parts of it
  /// 
  /// This function handles all primitive types properly.
  /// 
  /// Use this function only for 32 bit index buffers; for 16 bit index buffers, use
  /// CreateCollisionMesh instead.
  /// 
  /// \param pDestMesh
  ///   The destination collision mesh that will be filled with mesh information.
  /// 
  /// \param iFirstPrimitive
  ///   The first primitive in the collision mesh to start with.
  /// 
  /// \param iPrimitiveCount
  ///   The number of primitives to copy starting at iFirstPrimitive. Can be -1 to use all
  ///   remaining.
  /// 
  /// \param bFlipFaces
  ///   Specify this flag to flip each face
  /// 
  /// \return
  ///   BOOL bStatus: TRUE if successful
  VISION_APIFUNC BOOL CreateCollisionMesh32(VSimpleCollisionMesh32 *pDestMesh, int iFirstPrimitive, int iPrimitiveCount, BOOL bFlipFaces=FALSE);


  ///
  /// @}
  ///

  ///
  /// @name Replacing Vertex and Index Buffers
  /// @{
  ///

  /// \brief
  ///   Replaces a mesh buffer's vertex buffer
  /// 
  /// The passed vertex buffer object can either be a single-buffered or double-buffered vertex
  /// buffer. Both types are supported.
  /// 
  /// The previously set vertex buffer will have its reference count decreased and may, depending
  /// on further references, be physically deleted.
  /// 
  /// \param pVertexBuffer
  ///   The new vertex buffer.
  /// 
  /// \param descr
  ///   The vertex descriptor for the vertices in the vertex buffer.
  /// 
  /// \param iCount
  ///   The number of vertices in the vertex buffer.
  /// 
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for vertex data that is specified
  ///   only once and then never changed. STREAM should be used for vertex data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param iAdditionalBindFlag
  ///   In DX10/DX11 you can specify additional bind flags as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  /// 
  /// \sa VisMeshBuffer_cl::SetIndexBuffer
  VISION_APIFUNC void SetVertexBuffer(VVertexBuffer *pVertexBuffer, const VisMBVertexDescriptor_t &descr, int iCount, int iUsageFlag, int iAdditionalBindFlag);

  /// \brief
  ///   Returns the mesh buffers's vertex buffer.
  inline VVertexBuffer *GetVertexBuffer() { return m_spVertexBuffer; }

  /// \brief
  ///   Replaces a mesh buffer's index buffer
  /// 
  /// The previously set index buffer will have its reference count decreased and may, depending on
  /// further references, be physically deleted.
  /// 
  /// The index data in the passed index buffer is expected to be 16 Bits per index.
  /// 
  /// \param pIndexBuffer
  ///   The new index buffer.
  /// 
  /// \param iIndexCount
  ///   The number of indices in the index buffer.
  /// 
  /// \param iUsageFlag
  ///   Can either be VIS_MEMUSAGE_STATIC, VIS_MEMUSAGE_STATIC_UNMANAGED, VIS_MEMUSAGE_STREAM, or
  ///   VIS_MEMUSAGE_DYNAMIC. In general, you should use STATIC for vertex data that is specified
  ///   only once and then never changed. STREAM should be used for vertex data that is modified or
  ///   re-specified in run-time, but used for rendering more often than it is updated. DYNAMIC is
  ///   the best solution for data that has about a 1:1 update to render ratio. The difference
  ///   between VIS_MEMUSAGE_STATIC and VIS_MEMUSAGE_STATIC_UNMANAGED is that an unmanaged buffer
  ///   will never be double-buffered by the graphics API. It may therefore consume less memory on
  ///   some platforms with separate memory pools, but you will have to take care of lost device
  ///   handling etc. yourself. It is generally recommended NOT to use
  ///   VIS_MEMUSAGE_STATIC_UNMANAGED.
  ///   In addition to one of these basic settings (which can't be combined), you can specify 
  ///   VIS_MEMUSAGE_UAV_BYTEADDRESS or VIS_MEMUSAGE_STRUCTURED to determine the way this buffer
  ///   is addressed when using it as an unordered access resource in a compute shader DX11.
  ///
  /// \param iAdditionalBindFlag
  ///   In DX10/DX11 you can specify additional bind flags as follows:
  ///   \li VIS_BIND_STREAM_OUTPUT: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a target for stream output operations.
  ///   \li VIS_BIND_SHADER_RESOURCE: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as a shader resource (e.g. in order to perform random-access reads from the
  ///       buffer in a vertex or pixel shader). This can only be used in conjunction with VIS_MEMUSAGE_STATIC, not
  ///       with VIS_MEMUSAGE_STREAM or VIS_MEMUSAGE_DYNAMIC.
  ///   \li VIS_BIND_UNORDERED_ACCESS: In DX10/DX11, specifying this value will allow you to use
  ///       this buffer as an unordered access resource for compute shaders.
  /// 
  /// \sa VisMeshBuffer_cl::SetIndexBuffer
  VISION_APIFUNC void SetIndexBuffer(VIndexBuffer *pIndexBuffer, int iIndexCount, int iUsageFlag, int iAdditionalBindFlag);

  /// \brief
  ///   Returns the mesh buffers's index buffer.
  inline VIndexBuffer *GetIndexBuffer() { return m_spIndexBuffer; }

  /// \brief
  ///   Returns the index buffer format of this mesh buffer. The result is either VIS_INDEXFORMAT_32, VIS_INDEXFORMAT_16 or VIS_INDEXFORMAT_INVALID for no index buffer.
  VISION_APIFUNC VIndexFormat GetIndexType() const;


  /// \brief
  ///   Creates a new mesh buffer, that does not have an index buffer anymore. Vertices are duplicated so the appearance is the same. If this mesh does not have indices, the same mesh is returned.
  VISION_APIFUNC VisMeshBuffer_cl* FlattenOutIndexBuffer();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif
  VISION_APIFUNC static int GetCalcPrimitiveCount(MB_PrimitiveType_e eType, int iVertexCount, int iIndexCount);

protected:

  // overridden resource functions
  VISION_APIFUNC VOVERRIDE BOOL Reload();
  VISION_APIFUNC VOVERRIDE BOOL Unload();
  VISION_APIFUNC VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);
  

  ///
  /// @}
  ///

private:
  struct VertexAttributeEndianInfo_t
  {
    short iOffset;
    unsigned char iSize;
    unsigned char iCount;
  };

  friend class Vision;
  friend class VisRenderLoopHelper_cl;
  friend class VisMeshBufferObject_cl;
  friend void SetupVertexBufferTextures(VisMeshBuffer_cl *pBuffer, VisMeshBuffer_cl **pTexStreams, int iStreamMask = 0xFFFFFFFF);
  friend void RenderVertexBuffers(const class VisMeshBufferObjectCollection_cl &collection, int iMask, VCompiledShaderPass *pCustomShader);
  friend void RenderSingleBufferPrimitives(VisMeshBufferObject_cl *pMeshObj, char primtype, char iTransp, VisMeshBuffer_cl *pIndexBuffer, int iFirstPrim, int iPrimCount, VCompiledShaderPass *pShader);
  friend void SetupVertexBufferTextureArrays(VisMeshBufferObject_cl *pMeshObj, VisMeshBuffer_cl **pTexStreams);
  //friend char *GetVertexArrayPtr(VisMeshBuffer_cl *pBuffer);
  friend int AddMeshBufferStreams(VisMeshBuffer_cl *pMesh, int iStreamMasks, int iByteOffset, int iInstanceCount, int iStreamIndex = -1);
  friend void RenderParticleBuffer(VisParticleGroup_cl *pgroup, int iParticleCount);
  friend void RenderGeometryParticleBuffer_Instancing(VisParticleGroup_cl *pgroup, int iParticleCount);
  void VerifyVertexLockedStatus(bool bReqStatus) const;
  void VerifyIndexLockedStatus(bool bReqStatus) const;
  //void UpdateFastCopy();
  void Init();
  static void InitOneTime();
  static void DeInitOneTime();
  static BOOL ConvertFilename(const char *szSrc, char *szDst);

  int GetAttributeInfo(const VisMBVertexDescriptor_t &desc, VertexAttributeEndianInfo_t *pAttributeInfo);
  void ConvertEndianness(char *pData, int iNumVerts, int iVertexStride, int iNumAttributes, VertexAttributeEndianInfo_t *pAttributeInfo);
  void ReorderByteAttributes(char *pData, int iNumVerts, int iVertexStride, int iNumAttributes, VertexAttributeEndianInfo_t *pAttributeInfo);

  void EnsureBuffersExist();


  bool m_bIsFromFile;

  // mesh buffer data
  int m_iCurrentPrimCount;
  bool m_bMeshDataIsBigEndian;
  bool m_bVerticesDoubleBuffered;
  bool m_bIndicesDoubleBuffered;
  bool m_bDoubleBufferingFromFile; // force double buffering storage mode from file
 
  bool m_bIsModified;
  bool m_bVerticesLocked, m_bIndicesLocked;
  bool m_bUseProjection;
  char m_ePrimType; ///< casted to MB_PrimitiveType_e
  unsigned int m_iMemUsageFlagVertices;
  unsigned int m_iBindFlagVertices;
  unsigned int m_iMemUsageFlagIndices;
  unsigned int m_iBindFlagIndices;

  VSimpleRenderState_t m_iRenderState;

  int m_iAllocVertexCount;
  int m_iAllocIndexCount;

  VIndexBufferPtr m_spIndexBuffer;

  VisMBVertexDescriptor_t m_VertexDescr;

  VVertexBufferPtr m_spVertexBuffer;

  VTextureObjectPtr m_pChannelTex[MAX_MESHBUFFER_TEXCOORDS];

  VCompiledTechniquePtr m_spDefaultTechnique;

  //char *m_pVertexLockPtr;
  //unsigned short *m_pIndexLockPtr;
};




/// \brief
///   An instance of a mesh buffer object that references a mesh buffer
/// 
/// An instance of this class defines a mesh buffer object. Multiple objects can share the same
/// mesh buffer resources to render from, using distinct world position and orientation.
/// 
/// \sa VisMeshBuffer_cl
class VisMeshBufferObject_cl : public VisObject3D_cl, public VisElementManager_cl<VisMeshBufferObject_cl *>
{
public:

  V_DECLARE_SERIAL_DLLEXP( VisMeshBufferObject_cl, VISION_APIDATA )

  ///
  /// @name Constructors / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor of a mesh buffer object
  /// 
  /// \param pMeshBuffer
  ///   The mesh buffer resource to render from
  /// 
  /// \note
  ///   The mesh buffer to render geometry from can be changed at any time via SetMeshBuffer
  /// 
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  /// \sa VisMeshBufferObject_cl::GetMeshBuffer
  VISION_APIFUNC VisMeshBufferObject_cl(VisMeshBuffer_cl *pMeshBuffer=NULL);

  /// \brief
  ///   Destructor.
  VISION_APIFUNC virtual ~VisMeshBufferObject_cl();

  /// \brief
  ///   Dispose the object and remove it from scene
  VISION_APIFUNC VOVERRIDE void DisposeObject();



  ///
  /// @}
  ///

  ///
  /// @name Assigning Mesh Buffer
  /// @{
  ///


  /// \brief
  ///   Sets the mesh buffer for this object, i.e. the mesh resource to render.
  /// 
  /// The geometry defined in the mesh buffer will be transformed by position and rotation matrix
  /// of this object.
  /// 
  /// This function removes all other assigned vertex buffers.
  /// 
  /// A mesh buffer object can have more than one mesh (see AddMeshBuffer). This allows the
  /// programmer to set vertex streams or index buffer from different meshes.
  /// 
  /// \param pBuffer
  ///   The buffer to render from
  /// 
  /// \sa VisMeshBuffer_cl
  /// \sa VisMeshBufferObject_cl::GetMeshBuffer
  /// \sa VisMeshBufferObject_cl::AddMeshBuffer
  inline void SetMeshBuffer(VisMeshBuffer_cl *pBuffer);

  /// \brief
  ///   This function returns the first mesh buffer that is assigned
  /// 
  /// \return
  ///   VisMeshBuffer_cl *pBuffer : pointer to the mesh buffer 
  /// 
  /// \sa VisMeshBuffer_cl
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  /// \sa VisMeshBufferObject_cl::AddMeshBuffer
  inline VisMeshBuffer_cl *GetCurrentMeshBuffer() const;

  /// \brief
  ///   Adds a mesh buffer to this object.
  /// 
  /// The rendering uses the streams that are defined in the specified mesh, including the index
  /// buffer.
  /// 
  /// If more than one assigned mesh buffer has a stream (e.g. a position stream) the rendering
  /// uses the last buffer in the assignment order.
  /// 
  /// To replace the index buffer, a mesh buffer without vertices but with an index stream can be
  /// assigned.
  /// 
  /// The following properties are always used from the FIRST mesh buffer assigned: render flags
  /// (shader assignment,  transparency,...), primitive type, textures, projection and primitive
  /// count.
  /// 
  /// However, the low level VisRenderLoopHelper_cl::BeginMeshRendering() /
  /// VisRenderLoopHelper_cl::EndMeshRendering() functionality is more flexible than this construct
  /// and also much faster.
  /// 
  /// \param pBuffer
  ///   The mesh buffer to add
  /// 
  /// \return
  ///   int iIndex : The index at which the buffer has been added
  /// 
  /// \sa VisMeshBufferObject_cl::RemoveMeshBuffer
  /// \sa VisMeshBufferObject_cl::RemoveAllMeshBuffer
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  VISION_APIFUNC int AddMeshBuffer(VisMeshBuffer_cl *pBuffer);

  /// \brief
  ///   Removes a mesh buffer from this object
  /// 
  /// \param pBuffer
  ///   The mesh buffer to be removed
  /// 
  /// \return
  ///   BOOl bResult : TRUE, if the mesh buffer could be removed, i.e. if it was assigned
  /// 
  /// \sa VisMeshBufferObject_cl::AddMeshBuffer
  /// \sa VisMeshBufferObject_cl::RemoveAllMeshBuffer
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  VISION_APIFUNC BOOL RemoveMeshBuffer(VisMeshBuffer_cl *pBuffer);

  /// \brief
  ///   Removes a mesh buffer from this object using its list index
  /// 
  /// The passed index matches the return value of AddMeshBuffer only if no smaller index has been
  /// removed.
  /// 
  /// If a buffer has been removed from the list, all indices move up.
  /// 
  /// \param iIndex
  ///   The index of the mesh buffer in the object's buffer list to be removed.
  /// 
  /// \return
  ///   BOOl bResult : TRUE, if the mesh buffer could be removed, i.e. when the index was valid
  /// 
  /// \sa VisMeshBufferObject_cl::AddMeshBuffer
  /// \sa VisMeshBufferObject_cl::RemoveAllMeshBuffer
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  VISION_APIFUNC BOOL RemoveMeshBuffer(int iIndex);

  /// \brief
  ///   Removes all mesh buffers from this object
  /// 
  /// \sa VisMeshBufferObject_cl::AddMeshBuffer
  /// \sa VisMeshBufferObject_cl::RemoveAllMeshBuffer
  /// \sa VisMeshBufferObject_cl::SetMeshBuffer
  VISION_APIFUNC void RemoveAllMeshBuffer();


  /// \brief
  ///   Sets the render flags of the current mesh buffer object
  /// 
  /// A mesh buffer will only be rendered if an AND operation on the mesh buffer object's render
  /// flags and the current render context's render filter mask produces a nonzero value.
  /// 
  /// This mechnism is the analogue to VisBaseEntity_cl::SetVisibleBitMask.
  /// 
  /// \param iFlags
  ///   render flags for the mesh buffer object
  /// 
  /// \sa VisMeshBufferObject_cl::GetVisibleBitmask
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetVisibleBitmask(unsigned int iFlags);
  
  /// \brief
  ///   Returns the render flags of the current mesh buffer object
  /// 
  /// A mesh buffer will only be rendered if an AND operation on the mesh buffer object's render
  /// flags and the current render context's render filter mask produces a nonzero value. 
  /// 
  /// \return
  ///   int: The render flags of this mesh buffer
  /// 
  /// \sa VisMeshBufferObject_cl::SetVisibleBitmask
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline unsigned int GetVisibleBitmask() const;

  /// \brief
  ///   Sets a selective rendering range of primitives to render with this instance
  /// 
  /// This way it is possible to share a larger mesh buffer between multiple mesh objects, each
  /// using its own range of vertices.
  /// 
  /// Note that the parameters specify the primitive index resp. primitive count rather than number
  /// of indices.
  /// 
  /// The default is (0,-1), which means it renders the full range of the mesh buffer.
  /// 
  /// \param iFirstPrim
  ///   0-based number of first primitive index to render. For indexed tri-strips this is actually the index offset
  /// 
  /// \param iPrimCount
  ///   Number of primitives to render, where iFirstPrim+iPrimCount<=maximum available primitives.
  ///   If iPrimCount<0, the number of primitives set on the mesh buffer
  ///   (VisMeshBuffer_cl::GetCurrentPrimitiveCount) is used.
  /// 
  /// \sa VisMeshBufferObject_cl::GetRenderRange
  /// \sa VisMeshBuffer_cl::SetCurrentPrimitiveCount
  /// \sa VisMeshBuffer_cl::GetCurrentPrimitiveCount
  VISION_APIFUNC void SetRenderRange(int iFirstPrim, int iPrimCount);

  /// \brief
  ///   Returns the primitive render range of the mesh object.
  /// 
  /// \param iFirstPrim
  ///   Reference that receives the first primitive
  /// 
  /// \param iPrimCount
  ///   Reference that receives the number of primitives
  /// 
  /// \sa VisMeshBufferObject_cl::SetRenderRange
  inline void GetRenderRange(int &iFirstPrim, int &iPrimCount) const;

  ///
  /// @}
  ///

  ///
  /// @name Render Order, Position and Orientation
  /// @{
  ///

  /// \brief
  ///   Sets the rendering order for this object
  /// 
  /// The first order constant defines the rendering order relatively to the render loop. Supported
  /// constants are of type VRenderHook_e and can be combined to render the mesh more than
  /// once per frame.
  /// 
  /// A custom render loop implementation has to support triggering the mesh buffer rendering for
  /// the respective order constant. See implementation of VisionRenderLoop_cl::OnDoRenderLoop.
  /// 
  /// The second constant defines a sorting key value for visible objects with the same order.
  /// 
  /// To actually perform sub-order sorting for each group, the global SetEnableSubOrderSorting
  /// flag has to be enabled.
  /// 
  /// \param iOrderBits
  ///   A combination of order bits given in the VRenderHook_e enum (typically only one constant). The constants are in ascending
  ///   order, ranging from "at start rendering" up to "after rendering". The default order is
  ///   VRH_PRE_OCCLUSION_TESTS. 
  /// 
  /// \param iSubOrder
  ///   For objects with same order, this value defines a sorting key value for visible objects
  ///   with the same order. Objects with a high sub-order value get rendered first
  /// 
  /// \sa VisMeshBufferObject_cl::GetOrder
  /// \sa VisMeshBufferObject_cl::GetSubOrder
  /// \sa VisMeshBufferObject_cl::SetEnableSubOrderSorting
  /// \sa VisMeshBufferObject_cl::GetEnableSubOrderSorting
  VISION_APIFUNC void SetOrder(int iOrderBits, int iSubOrder);

  /// \brief
  ///   Returns the render order bit constant of this object (first parameter of SetOrder)
  /// 
  /// \return
  ///   int iBitMask : The bit mask defining the render order in the render loop implementation.
  ///   Can be any combination of VRenderHook_e constants.
  /// 
  /// \sa VisMeshBufferObject_cl::SetOrder
  /// \sa VisMeshBufferObject_cl::GetSubOrder
  /// \sa VisMeshBufferObject_cl::SetEnableSubOrderSorting
  /// \sa VisMeshBufferObject_cl::GetEnableSubOrderSorting
  inline int GetOrder() const;

  /// \brief
  ///   Returns the suborder constant of this object (second parameter of SetOrder)
  /// 
  /// \return
  ///   int iSubOrder : The sub order constant of this object. If sorting is enabled, a large value
  ///   means that the object gets rendered first
  /// 
  /// \sa VisMeshBufferObject_cl::SetOrder
  /// \sa VisMeshBufferObject_cl::GetOrder
  /// \sa VisMeshBufferObject_cl::SetEnableSubOrderSorting
  /// \sa VisMeshBufferObject_cl::GetEnableSubOrderSorting
  inline int GetSubOrder() const;


  ///
  /// @}
  ///

  ///
  /// @name Visible Status
  /// @{
  ///

  /// \brief
  ///   This function just wraps around SetVisibleBitmask and sets a mask of either 0 or 0xffffffff
  /// 
  /// \param bVisible
  ///   The new visibility status.
  /// 
  /// \sa VisMeshBufferObject_cl::SetVisibleBitmask
  inline void SetVisible(BOOL bVisible=TRUE);

  /// \brief
  ///   This function just wraps around GetVisibleBitmask and returns TRUE for
  ///   GetVisibleBitmask()!=0
  /// 
  /// \return
  ///   BOOL bStatus : The visible status
  /// 
  /// \sa VisMeshBufferObject_cl::GetVisibleBitmask
  inline BOOL IsVisible() const;

  /// \brief
  ///   Enables or disables visibility testing for mesh buffers and sets the bounding box
  /// 
  /// The bounding box is used for visibility tests.
  /// 
  /// Internally this function creates a VisVisibilityObject_cl instance. Note that this results in
  /// a certain computational and management overhead for visibility determination.
  /// 
  /// The bounding box should be chosen to contain all vertices, also if rotation is applied.
  /// 
  /// A visibility object that is created this way is "owned" by this object and moves with it.
  /// 
  /// A visibility object that is assigned via VisMeshBufferObject_cl::SetVisibilityObject will not
  /// move with the mesh object. However, specifying the visibility object via SetVisibilityObject
  /// has the advantage that the same visibility object can be used for multiple mesh buffer
  /// objects, potentially resulting in a lower overhead for visibility determination.
  /// 
  /// Note that mesh buffer objects have to direct way of querying whether they are visible or not.
  /// Querying their visibility status has to go through visibility objects.
  /// 
  /// \param pVisBox
  ///   pointer to the bounding box used for testing. If NULL, visibility  testing will be
  ///   disabled. Otherwise the bounding box will be copied and used for visibility testing. The
  ///   bounding box has to be specified relative to the mesh buffer object's position, e.g.
  ///   (-15,-15,-15) - (15,15,15).
  /// 
  /// \note
  ///   You can directly access and modify the VisVisibilityObject_cl via GetVisibilityObject.
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisMeshBufferObject_cl::SetVisibilityObject
  /// \sa VisMeshBufferObject_cl::GetVisibilityObject
  VISION_APIFUNC void SetVisibilityBoundingBox(const hkvAlignedBBox *pVisBox);

  /// \brief
  ///   Returns a pointer to the visibility bounding box
  /// 
  /// The return value will be NULL if no visibility object is assigned to the mesh buffer object.
  /// 
  /// This function wraps around GetVisibilityObject()->GetBoundingBoxPtr().
  /// 
  /// \return
  ///   const hkvAlignedBBox* pBox: Pointer to the bounding box instance of the visibility
  ///   object, or NULL
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisMeshBufferObject_cl::SetVisibilityBoundingBox
  /// \sa VisMeshBufferObject_cl::SetVisibilityObject
  inline const hkvAlignedBBox* GetVisibilityBoundingBox() const;

  /// \brief
  ///   Returns a pointer to the current visibility object
  /// 
  /// (or NULL if no visibility object is set).
  /// 
  /// A visibility object can be set via VisMeshBufferObject_cl::SetVisibilityObject. Such
  /// visibility objects can be shared by multiple mesh buffer objects. Alternatively, mesh buffer
  /// objects can create their "own" visibility objects using the SetVisibilityBoundingBox method.
  /// 
  /// \return
  ///   VisVisibilityObject_cl* pVisObj: Pointer to the current visibility object, or NULL
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisMeshBufferObject_cl::SetVisibilityBoundingBox
  /// \sa VisMeshBufferObject_cl::SetVisibilityObject
  inline VisVisibilityObject_cl* GetVisibilityObject() const;

  /// \brief
  ///   Sets a visibility object for this the mesh buffer object
  /// 
  /// Visibility objects set through this method can be shared by multiple mesh buffer objects.
  /// 
  /// The reference counter of the new object will be increased and the old one will be released.
  /// The old visibility object is destroyed if there is no other reference to it.
  /// 
  /// VisMeshBufferObject_cl::SetPosition will not move the visibility object if it has been
  /// assigned via SetVisibilityObject.
  /// 
  /// Calling this method will also add the mesh buffer object to the list of referenced
  /// VisObject3D_cl instances of the visibility object. References of previously set visibility
  /// objects to this mesh buffer object will be deleted.
  /// 
  /// \param pNewVisObject
  ///   Pointer to a visibility object, or NULL to release the current visibility object
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisMeshBufferObject_cl::GetVisibilityObject
  /// \sa VisMeshBufferObject_cl::SetVisibilityBoundingBox
  inline void SetVisibilityObject(VisVisibilityObject_cl* pNewVisObject);

  
  ///
  /// @}
  ///

  ///
  /// @name Render State Functions
  /// @{
  ///

  /// \brief
  ///   Defines swizzling of texture coordinate streams
  /// 
  /// Using this method, it is possible to assign texture streams defined in mesh buffers to an
  /// arbitrary target texture stream.
  /// 
  /// By default an object uses identity mapping, i.e. SetTextureStreamTarget(i,i). 
  /// 
  /// \param iTextureStream
  ///   Input Texture Stream. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
  /// 
  /// \param iTextureStreamTarget
  ///   Target Texture Stream. Must be in the range [0..MAX_MESHBUFFER_TEXCOORDS-1]
  /// 
  /// \sa VisMeshBufferObject_cl::GetTextureStreamTarget
  inline void SetTextureStreamTarget(unsigned int iTextureStream, unsigned int iTextureStreamTarget)
  {
    VASSERT(iTextureStream<MAX_MESHBUFFER_TEXCOORDS);
    VASSERT(iTextureStreamTarget<MAX_MESHBUFFER_TEXCOORDS);
    m_cTextureStreamTargets[iTextureStream] = (unsigned char)iTextureStreamTarget;
  }

  /// \brief
  ///   Returns the texture stream target for a specific texture stream
  /// 
  /// \param iTextureStream
  ///   Input texture stream index
  /// 
  /// \return
  ///   unsigned int iTextureStreamTarget: target texture stream index
  /// 
  /// \sa VisMeshBufferObject_cl::SetTextureStreamTarget
  inline unsigned int GetTextureStreamTarget(unsigned int iTextureStream) const
  {
    VASSERT(iTextureStream < MAX_MESHBUFFER_TEXCOORDS);
    return (unsigned int)m_cTextureStreamTargets[iTextureStream];
  }


  /// \brief
  ///   Sets a shader technique for this mesh buffer object
  /// 
  /// The shader technique assigned by this method supersedes the default shader technique assigned
  /// to the mesh buffer(s) via the VisMeshBuffer_cl::SetDefaultTechnique method.
  /// 
  /// \param pTechnique
  ///   Shader technique to use. Can be NULL
  /// 
  /// \sa VisMeshBuffer_cl::SetDefaultTechnique
  /// \sa VisMeshBufferObject_cl::GetTechnique
  /// \sa VisMeshBufferObject_cl::GetActiveTechnique
  inline void SetTechnique(VCompiledTechnique *pTechnique)
  {
    m_spTechnique = pTechnique;
  }

  /// \brief
  ///   Returns the shader technique assigned to this mesh buffer object
  /// 
  /// The shader technique can be assigned using the SetTechnique method.
  /// 
  /// Use the GetActiveTechnique function to get either the object's or the mesh buffer's default
  /// technique.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: The assigned technique, or NULL if no technique is assigned
  ///   to this object
  /// 
  /// \sa VisMeshBuffer_cl::SetDefaultTechnique
  /// \sa VisMeshBufferObject_cl::SetTechnique
  /// \sa VisMeshBufferObject_cl::GetActiveTechnique
  inline VCompiledTechnique *GetTechnique() const
  {
    return m_spTechnique;
  }

  /// \brief
  ///   Returns the shader technique to be used for rendering this mesh buffer object
  /// 
  /// Unlike GetTechnique (which will always return NULL if no technique is assigned to a mesh
  /// buffer object), this method returns the default technique assigned to the mesh buffer if the
  /// mesh buffer object does not have a technique assigned.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: The technique, or NULL if no technique is assigned either
  ///   to the object or mesh buffer.
  /// 
  /// \sa VisMeshBuffer_cl::SetDefaultTechnique
  /// \sa VisMeshBufferObject_cl::SetTechnique
  /// \sa VisMeshBufferObject_cl::GetTechnique
  inline VCompiledTechnique *GetActiveTechnique() const;


  ///
  /// @}
  ///

  ///
  /// @name Render Functions
  /// @{
  ///

  /// \brief
  ///   Virtual overridable render function
  /// 
  /// This function gets called by the engine when the buffer object is rendered.
  /// 
  /// This function can be overridden by custom mesh buffer object classes to pass a different
  /// number of primitives to the base implementation or to modify the shader in place.
  /// 
  /// \param flags
  ///   Internal rendering flags. Should be passed to the base implementation without changes.
  /// 
  /// \param iFirstPrim
  ///   The first primitive to render.
  /// 
  /// \param iPrimCount
  ///   The number of primitives to be rendered. Can be <0 to render the default number of
  ///   primitives, but it must not be larger than the maximum number of primitives supported.
  /// 
  /// \param pCustomShader
  ///   Pointer to custom shader to render the object with. If NULL, the current assigned shader
  ///   effect is used.
  /// 
  /// \sa VisMeshBuffer_cl::SetCurrentPrimitiveCount
  /// \sa VisMeshBuffer_cl::GetCurrentPrimitiveCount
  VISION_APIFUNC virtual void OnRender(int flags, int iFirstPrim, int iPrimCount, VCompiledShaderPass *pCustomShader);


  ///
  /// @}
  ///

  ///
  /// @name Global Properties
  /// @{
  ///

  /// \brief
  ///   Globally enables or disables "sub order sorting" of mesh buffer objects.
  /// 
  /// By default the sub order sorting is disabled.
  /// 
  /// Each mesh buffer object can has a sub order sorting key value. Objects that are rendered at
  /// the same place inside the render loop (e.g. same order bits) have an undefined rendering
  /// order.
  /// 
  /// This feature sorts objects with matching order bits according to their sub order sorting key.
  /// 
  /// Objects with a large sub order bits get rendered before objects with a smaller value.
  /// 
  /// \param bEnable
  ///   the new global status
  /// 
  /// \sa VisMeshBufferObject_cl::SetOrder
  /// \sa VisMeshBufferObject_cl::GetOrder
  /// \sa VisMeshBufferObject_cl::GetSubOrder
  /// \sa VisMeshBufferObject_cl::GetEnableSubOrderSorting
  VISION_APIFUNC static void SetEnableSubOrderSorting(BOOL bEnable=TRUE);

  /// \brief
  ///   Retrieves the global status of "sub order sorting"
  /// 
  /// \return
  ///   BOOL bStatus : TRUE if sorting is enabled, FALSE if not
  /// 
  /// \sa VisMeshBufferObject_cl::SetOrder
  /// \sa VisMeshBufferObject_cl::GetOrder
  /// \sa VisMeshBufferObject_cl::GetSubOrder
  /// \sa VisMeshBufferObject_cl::SetEnableSubOrderSorting
  VISION_APIFUNC static BOOL GetEnableSubOrderSorting();

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Overridden function that gets the resources used for this instance.
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  /// \brief
  ///   Global function, called after purging particles
  VISION_APIFUNC static void UpdateAvailableMask();


  VISION_APIFUNC VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  ///
  /// @}
  ///

protected:
  friend class VisRenderLoopHelper_cl;
  friend class VisVisibilityObject_cl;
  friend void RenderVertexBuffers(const class VisMeshBufferObjectCollection_cl &collection, int iMask, VCompiledShaderPass *pCustomShader);
  friend void SetMeshBufferMatrix(VisMeshBufferObject_cl *pMeshObj);
  friend void SetupVertexBufferTextureArrays(VisMeshBufferObject_cl *pMeshObj, VisMeshBuffer_cl **pTexStreams);
  friend class VisGame_cl;

  /// \brief
  ///   Static function to render an individual mesh buffer (may be called from OnRender() implementations
  static VISION_APIFUNC void RenderSingleMeshBuffer(VisMeshBufferObject_cl *pMeshObj, int iFirstPrim, int iPrimCount, int flags, VCompiledShaderPass *pCustomShader);

  void Init();

  VCompiledTechniquePtr m_spTechnique;
  int m_iOrderBits, m_iSubOrder;
  unsigned int m_iRenderFlags;
  bool m_bVisible, m_bMoveVisObj;
  short m_iMeshBufferCount;
  
  VisVisibilityObjectPtr m_spVisObj;
  
  DynArray_cl<VisMeshBuffer_cl *>m_pMeshBuffer;

  int m_iFirstPrimitive, m_iPrimitiveCount;
  static int g_iAvailableMask;
  static bool g_bSubOrderSorting;
  unsigned char m_cTextureStreamTargets[MAX_MESHBUFFER_TEXCOORDS];

  static DynArray_cl<VisMeshBufferObject_cl *>g_RenderList;

};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisMeshBufferObject_cl)

#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.inl>

#endif // VISAPIVERTEXBUFFERS_HPP_INCLUDED

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
