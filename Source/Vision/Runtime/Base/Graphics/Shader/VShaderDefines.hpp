/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VShaderDefines.hpp

#ifndef _VSHADERDEFINES_HPP_INCLUDED
#define _VSHADERDEFINES_HPP_INCLUDED



#define VERTEXDESC_FORMAT_FLOAT        0x1000
#define VERTEXDESC_FORMAT_FLOAT2       0x2000
#define VERTEXDESC_FORMAT_FLOAT3       0x3000
#define VERTEXDESC_FORMAT_FLOAT4       0x4000
#define VERTEXDESC_FORMAT_BYTE4N       0x5000
#define VERTEXDESC_FORMAT_SHORT2N      0x6000
#define VERTEXDESC_FORMAT_SHORT4N      0x7000
#define VERTEXDESC_FORMAT_HALF2        0x8000
#define VERTEXDESC_FORMAT_HALF4        0x9000
#define VERTEXDESC_FORMAT_USHORT2N     0xA000
#define VERTEXDESC_FORMAT_USHORT4N     0xB000
#define VERTEXDESC_FORMAT_BYTE4        0xC000
#define VERTEXDESC_FORMAT_SHORT2       0xD000
#define VERTEXDESC_FORMAT_SHORT4       0xE000
#define VERTEXDESC_FORMAT_UBYTE4N      VERTEXDESC_FORMAT_BYTE4N


#define VERTEXDESC_FORMAT_MASK         0xf000
#define VERTEXDESC_OFFSET_MASK         0x0fff

#define VERTEXDESC_FORMATOF(val)      ((val)&VERTEXDESC_FORMAT_MASK)
#define VERTEXDESC_OFFSETOF(val)      ((val)&VERTEXDESC_OFFSET_MASK)
#define VERTEXDESC_FORMATINDEX(val)   ((val)>>12)

#define VERTEX_STREAM_POSITION        (1 <<  0)
#define VERTEX_STREAM_NORMAL          (1 <<  1)
#define VERTEX_STREAM_COLOR           (1 <<  2)
#define VERTEX_STREAM_SECONDARYCOLOR  (1 <<  3)
#define VERTEX_STREAM_TEX0            (1 <<  4)
#define VERTEX_STREAM_TEX1            (1 <<  5)
#define VERTEX_STREAM_TEX2            (1 <<  6)
#define VERTEX_STREAM_TEX3            (1 <<  7)
#define VERTEX_STREAM_TEX4            (1 <<  8)
#define VERTEX_STREAM_TEX5            (1 <<  9)
#define VERTEX_STREAM_TEX6            (1 << 10)
#define VERTEX_STREAM_TEX7            (1 << 11)
#define VERTEX_STREAM_TEX8            (1 << 12)
#define VERTEX_STREAM_TEX9            (1 << 13)
#define VERTEX_STREAM_TEX10           (1 << 14)
#define VERTEX_STREAM_TEX11           (1 << 15)
#define VERTEX_STREAM_TEX12           (1 << 16)
#define VERTEX_STREAM_TEX13           (1 << 17)
#define VERTEX_STREAM_TEX14           (1 << 18)
#define VERTEX_STREAM_TEX15           (1 << 19)

#define VERTEX_STREAM_BONEINDEX       VERTEX_STREAM_TEX3
#define VERTEX_STREAM_BONEWEIGHT      VERTEX_STREAM_TEX4

#define VERTEX_STREAM_INDEXBUFFER     0x10000000

#define VERTEX_STREAM_TANGENT         VERTEX_STREAM_TEX2

#define VERTEX_STREAM_ALL             0xffffffff
#define VERTEX_STREAM_TEX_MASK        0x000ffff0
#define VERTEX_STREAM_FIRST_TEX_BIT   4
#define VERTEX_STREAM_VERTEXRELEVANT  (VERTEX_STREAM_ALL & (~VERTEX_STREAM_INDEXBUFFER))


#define MAX_MESHBUFFER_TEXCOORDS      16


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
