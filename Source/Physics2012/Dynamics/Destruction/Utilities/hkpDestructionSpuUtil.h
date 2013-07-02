/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_DYNAMICS2_DESTRUCTION_SPU_UTIL_H
#define HK_DYNAMICS2_DESTRUCTION_SPU_UTIL_H

//
//	Declares a local pointer on SPU

#if defined(HK_PLATFORM_SPU)
#	define HK_DECLARE_SPU_LOCAL_DESTRUCTION_PTR(varType, spuVarName, typeSize)\
		HK_ALIGN16(hkUint8 spuVarName##_stackComp[HK_NEXT_MULTIPLE_OF(16, typeSize)]);\
		varType* spuVarName = (varType*)spuVarName##_stackComp
#else
#	define HK_DECLARE_SPU_LOCAL_DESTRUCTION_PTR(varType, spuVarName, typeSize)	varType* spuVarName
#endif

//
//	Downloads a pointer from PPU to SPU

#if defined(HK_PLATFORM_SPU)
#	define HK_DOWNLOAD_DESTRUCTION_PTR_TO_SPU(spuVarName, ppuAddress, downloadSize)\
		{\
			const int spuVarName##_sizeof_ = HK_NEXT_MULTIPLE_OF(16, downloadSize);\
			hkSpuDmaManager::getFromMainMemory(spuVarName, ppuAddress, spuVarName##_sizeof_, hkSpuDmaManager::READ_COPY);\
			HK_SPU_DMA_DEFER_FINAL_CHECKS_UNTIL_WAIT(ppuAddress, spuVarName, spuVarName##_sizeof_);\
			hkSpuDmaManager::waitForDmaCompletion();\
		}

#else
#	define HK_DOWNLOAD_DESTRUCTION_PTR_TO_SPU(spuVarName, ppuAddress, downloadSize) spuVarName = ppuAddress
#endif

#endif	//	HK_DYNAMICS2_DESTRUCTION_SPU_UTIL_H

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
