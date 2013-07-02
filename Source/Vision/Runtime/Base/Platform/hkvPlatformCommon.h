/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKVPLATFORMDEFINESCOMMON_H
#define HKVPLATFORMDEFINESCOMMON_H

typedef signed          long    int     SLONG;          ///<32 bit signed.
typedef unsigned        long    int     ULONG;          ///<32 bit unsigned.
typedef signed          short   int     SWORD;          ///<16 bit signed.
typedef unsigned        short   int     UWORD;          ///<16 bit unsigned.
typedef signed          char            SBYTE;          ///<8 bit signed.
typedef unsigned        char            UBYTE;          ///<8 bit unsigned.
typedef UBYTE                           BBOOL;    

// Defines the different swizzle modes for unsigned byte vertex attributes.
// Needs to be applied to loaded mesh data on each platform.

#define SWIZZLE_VERTEX_RGBA8_TO_RGBA8 0 ///< Swizzles RGBA to RGBA (no action performed)
#define SWIZZLE_VERTEX_RGBA8_TO_BGRA8 1 ///< Swizzles RGBA to BGRA
#define SWIZZLE_VERTEX_RGBA8_TO_ABGR8 2 ///< Swizzles RGBA to ABGR

#define SUPPORTS_PS_SAMPLERS

#endif // HKVPLATFORMDEFINESCOMMON_H

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
