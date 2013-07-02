/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VVertexDescriptor.hpp

#ifndef _VVERTEXDESCRIPTOR_HPP_INCLUDED
#define _VVERTEXDESCRIPTOR_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/VShaderDefines.hpp>

/// \brief
///   Structure that defines the vertex format for mesh buffer vertices (VisMeshBuffer_cl class)
/// 
/// This structure allows you to define your own vertex format for rendering mesh buffers.
/// 
/// The m_iStride member has to be set to the size of the vertex structure.
/// 
/// Each member defines the offset (in bytes) within the structure; -1 indicates that the respective stream
/// is not defined (default). To calculate the offset, use e.g. offsetof(structurename,member).
/// Furthermore, the offset values for each attribute have to contain a format specifier, which has to be added to
/// the offset field (e.g. iOfs = offsetof(struct, structmember) | VERTEXDESC_FORMAT_FLOAT3).
///
/// Note that secondary color information is currently not supported.
/// 
/// The following format specifiers are supported:
/// \li VERTEXDESC_FORMAT_FLOAT : Single float component.
/// 
/// \li VERTEXDESC_FORMAT_FLOAT2 : float[2] components.
/// 
/// \li VERTEXDESC_FORMAT_FLOAT3 : float[3] components.
/// 
/// \li VERTEXDESC_FORMAT_FLOAT4 : float[4] components.
/// 
/// \li VERTEXDESC_FORMAT_UBYTE4N : unsigned byte[4] components, each remapped to [0..1] range.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_SHORT2N : signed short[2] components, each remapped to [-1..1] range.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_SHORT4N : signed short[4] components, each remapped to [-1..1] range.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_HALF2 : half[2] components (half=16bit floating point). Requires shader
///   model 2.0 or up. For proper conversion inside the vertex structure on the C++ side, you can use the VFloat16 class.
/// 
/// \li VERTEXDESC_FORMAT_HALF4 : half[4] components (half=16bit floating point). Requires shader
///   model 2.0 or up. For proper conversion inside the vertex structure on the C++ side, you can use the VFloat16 class.
/// 
/// \li VERTEXDESC_FORMAT_USHORT2N : unsigned short[2] components, each remapped to [0..1] range.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_USHORT4N : unsigned short[4] components, each remapped to [0..1] range.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_BYTE4 : unsigned byte [4] components, not remapped.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_SHORT2 : signed short[2] components, not remapped.
///   Requires shader model 2.0 or up. 
/// 
/// \li VERTEXDESC_FORMAT_SHORT4 : signed short[4] components, not remapped.
///   Requires shader model 2.0 or up. 
/// 
/// \sa VisMBSimpleVertex_t
/// \sa VisMeshBuffer_cl
/// \sa VisMeshBuffer_cl::AllocateVertices
/// \sa VisMeshBufferObject_cl
struct VisMBVertexDescriptor_t
{
  V_DECLARE_SERIALX( VisMBVertexDescriptor_t, VBASE_IMPEXP )
public:

  /// \brief
  ///   Constructor, resets all members
  inline VisMBVertexDescriptor_t()
  {
    Reset();
  }

  /// \brief
  ///   Flags all streams as undefined
  inline void Reset() 
  {
    m_iStride=0;
    m_iPosOfs=m_iColorOfs=m_iNormalOfs=m_iSecondaryColorOfs = -1;
    for (int i=0;i<MAX_MESHBUFFER_TEXCOORDS;i++) m_iTexCoordOfs[i]=-1;
    m_iHash=0;
    m_iLastTexCoord = 0;
    m_iFirstTexCoord = 255;
  }

  ///
  /// @name Descriptor Properties
  /// @{
  ///

  /// \brief
  ///   Determines whether this descriptor is valid. Does NOT perform a platform-specific validation; rather, it only performs a simple validation
  ///   of the structure members.
  VBASE_IMPEXP BOOL IsValidDescriptor() const;

  /// \brief
  ///   Indicates whether this descriptor has position information
  inline BOOL HasPosition() const
  {
    return m_iPosOfs!=-1;
  }

  /// \brief
  ///   Indicates whether this descriptor has primary color information
  inline BOOL HasColor() const
  {
    return m_iColorOfs!=-1;
  }

  /// \brief
  ///   Indicates whether this descriptor has secondary color information (not supported on all platforms)
  inline BOOL HasSecondaryColor() const 
  {
    return m_iSecondaryColorOfs!=-1;
  }

  /// \brief
  ///   Indicates whether this descriptor has normal vector information
  inline BOOL HasNormal() const 
  {
    return m_iNormalOfs!=-1;
  }

  /// \brief
  ///   Indicates whether this descriptor has texture coordinate information for a specific texture
  ///   channel.
  /// 
  /// The number of supported texture coordinate channels is defined by the global
  /// MAX_MESHBUFFER_TEXCOORDS constant (currently 16).
  /// 
  /// \param iChannel
  ///   texture channel to query. Must be [0..MAX_MESHBUFFER_TEXCOORDS-1]
  inline BOOL HasTexCoord(int iChannel=0) const
  {
    VASSERT(iChannel>=0&&iChannel<MAX_MESHBUFFER_TEXCOORDS);
    return m_iTexCoordOfs[iChannel]!=-1;
  }

  /// \brief
  ///   Returns the stream bitmask (each bit of type VERTEX_STREAM_xyz) for this descriptor. Naturally,
  ///   the VERTEX_STREAM_INDEXBUFFER bit is never set
  VBASE_IMPEXP unsigned int GetStreamMask() const;


  ///
  /// @}
  ///

  ///
  /// @name Stream Functions
  /// @{
  ///

  /// \brief
  ///   Writes the descriptor information to a binary file stream
  VBASE_IMPEXP BOOL WriteToStream(IVFileOutStream *pOutStream) const;

  /// \brief
  ///   Reads descriptor information from a binary file stream
  VBASE_IMPEXP BOOL ReadFromStream(IVFileInStream *pInStream);

  /// \brief
  ///   Serializes descriptor information from or to file
  VBASE_IMPEXP void SerializeX(VArchive &ar);

  /// \brief
  ///   Internal function that fills up flags for all members
  VBASE_IMPEXP void SetFormatDefaults();

  ///
  /// @}
  ///

  ///
  /// @name Static Helpers
  /// @{
  ///

  /// \brief
  ///   Copies n vertices from source buffer to destination buffer using distinct vertex descriptors for both buffers
  /// 
  /// All members that are defined in the destination descriptor but not in the source are initialized to zero. The same goes for members that have
  /// more components in the target buffer (e.g. source float2(1.0,2.0) will be copied as float3(1.0,2.0,0.0) if the target uses float3 as component format).
  /// 
  /// This function also takes care of all different combinations of formats, e.g. VFloat16, float, short or byte.
  /// 
  /// \param pDstBuffer
  ///   Destination system memory buffer
  /// 
  /// \param dstDesc
  ///   vertex format of destination vertices
  /// 
  /// \param pSrcBuffer
  ///   Source system memory buffer
  /// 
  /// \param srcDesc
  ///   vertex format of source vertices
  /// 
  /// \param iVertexCount
  ///   number of vertices to copy
  VBASE_IMPEXP static void CopyVertices(void *pDstBuffer, const VisMBVertexDescriptor_t &dstDesc, const void *pSrcBuffer, const VisMBVertexDescriptor_t &srcDesc, int iVertexCount);

  /// \brief
  ///   Returns a format constant of type VERTEXDESC_FORMAT_FLOAT3,.. for the specified descriptor
  ///   member (e.g. m_iPosOfs)
  static inline unsigned int GetComponentFormat(int iValue) {return iValue&VERTEXDESC_FORMAT_MASK;}

  /// \brief
  ///   Returns a format string for each constant of type VERTEXDESC_FORMAT_FLOAT3. Can be used for debug purposes
  static inline const char *GetComponentFormatString(int iFormat)
  {
    switch (iFormat)
    {
      case VERTEXDESC_FORMAT_FLOAT:return "float";
      case VERTEXDESC_FORMAT_FLOAT2:return "float2";
      case VERTEXDESC_FORMAT_FLOAT3:return "float3";
      case VERTEXDESC_FORMAT_FLOAT4:return "float4";
      case VERTEXDESC_FORMAT_BYTE4N:return "byte4N";
      case VERTEXDESC_FORMAT_SHORT2N:return "short2N";
      case VERTEXDESC_FORMAT_SHORT4N:return "short4N";
      case VERTEXDESC_FORMAT_HALF2:return "half2";
      case VERTEXDESC_FORMAT_HALF4:return "half4";
      case VERTEXDESC_FORMAT_USHORT2N:return "ushort2N";
      case VERTEXDESC_FORMAT_USHORT4N:return "ushort4N";
      case VERTEXDESC_FORMAT_BYTE4:return "byte4";
      case VERTEXDESC_FORMAT_SHORT2:return "short2";
      case VERTEXDESC_FORMAT_SHORT4:return "short4";
      //case VERTEXDESC_FORMAT_UBYTE4N:return "ubyte4N";
    }
    return "unknown";
  }

  /// \brief
  ///   Returns the offset (in bytes) for the specified descriptor member (e.g. m_iPosOfs)
  static inline int GetComponentOffset(int iValue) {return iValue&VERTEXDESC_OFFSET_MASK;}

  /// \brief
  ///   Returns the number of components for the passed format constant, e.g. 3 for
  ///   VERTEXDESC_FORMAT_FLOAT3. Also descriptor members can be passed here.
  VBASE_IMPEXP static int GetComponentCountForFormat(int iFormat);

  /// \brief
  ///   Returns the size of the components for the passed format constant, e.g. 4 for
  ///   VERTEXDESC_FORMAT_FLOAT3, and 2 for VERTEXDESC_FORMAT_SHORT4. Also descriptor members can be passed here.
  VBASE_IMPEXP static int GetComponentSizeForFormat(int iFormat);

  /// \brief
  ///   Static helper function to retrieve components from vertex raw data in float format
  ///
  /// This function performs format conversion so that the target format is always based on float, regardless of the passed format.
  /// The interpretation of non-float formats is the same as inside vertex shaders, i.e. VERTEXDESC_FORMAT_SHORT2N converts from -32767..32767 to -1..1 range.
  ///
  /// \param pRawVertexData
  ///   The source vertex data to read data from
  ///
  /// \param iOfsAndFormat
  ///   The format specifier, that is compatible with the m_iPosOfs,... members of the descriptor class. The offset is used as an offset in the array
  ///   and to format is used for format conversion.
  ///
  /// \param pDestFloat
  ///   Target array that receives the converted components. The array must be large enough for 1..4 floats, depending on the format
  ///
  /// \returns
  ///   The number of components written (e.g. 3 for VERTEXDESC_FORMAT_FLOAT3). Or 0 if iOfsAndFormat==-1 
  VBASE_IMPEXP static int GetAsFloatArray(const void *pRawVertexData, int iOfsAndFormat, float *pDestFloat);

  /// \brief
  ///   The exact opposite of GetAsFloatArray: Writes float values into a buffer with specified format
  VBASE_IMPEXP static int SetAsFloatArray(void *pRawVertexData, int iOfsAndFormat, const float *pSrcFloat);

  ///
  /// @}
  ///

  ///
  /// @name Comparison
  /// @{
  ///

  /// \brief
  ///   Returns the hash code for this vertex descriptor. Internally used to optimize rendering.
  inline unsigned int GetHash() const { return m_iHash; }

  /// \brief
  ///   Computes the internal hash value for this vertex descriptor. 
  VBASE_IMPEXP void ComputeHash();

  /// \brief
  ///   Compares two descriptors and returns true if they are absolutely identical. Requires that the hash is computed properly for both
  VBASE_IMPEXP bool Equals(const VisMBVertexDescriptor_t &other) const;

  /// \brief
  ///   Compares two descriptors and returns true if they are absolutely identical. Requires that the hash is computed properly for both
  inline bool operator == (const VisMBVertexDescriptor_t& other) const                          
  { 
    return Equals(other); 
  }

  /// \brief
  ///   Compares two descriptors and returns true if they differ in any property
  inline bool operator != (const VisMBVertexDescriptor_t& other) const                          
  { 
    return !Equals(other); 
  }

  ///
  /// @}
  ///

  ///
  /// @name Public members
  /// @{
  ///

  // data
  short m_iStride;            ///< stride of the vertex structure; must be set to the size of the vertex structure
  short m_iPosOfs;            ///< offset of the position vector in the structure. Use bitwise OR with format bitflag constants of type VERTEXDESC_FORMAT_xyz to force a specific format
  short m_iColorOfs;          ///< offset of the color vector in the structure. Use bitwise OR with format bitflag constants of type VERTEXDESC_FORMAT_xyz to force a specific format
  short m_iNormalOfs;         ///< offset of the normal vector in the structure. Use bitwise OR with format bitflag constants of type VERTEXDESC_FORMAT_xyz to force a specific format
  short m_iTexCoordOfs[16];   ///< offset of the sets of texture coordinates in the structure. Use bitwise OR with format bitflag constants of type VERTEXDESC_FORMAT_xyz to force a specific format
  short m_iSecondaryColorOfs; ///< offset of the secondary color. Not supported on all platforms
  
  // do not put members in between here

  unsigned char m_iFirstTexCoord;     ///< Index of the first used texture coordinate set. Set automatically by ComputeHash() or at serialization time.
  unsigned char m_iLastTexCoord;      ///< Index of the last used texture coordinate set. Set automatically by ComputeHash() or at serialization time.
  unsigned int m_iHash;               ///< Hash value. Set automatically when computing the hash or at serialization time.

  ///
  /// @}
  ///

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
