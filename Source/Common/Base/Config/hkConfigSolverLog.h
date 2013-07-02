/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_CONSTRAINTSOLVER2_SOLVER_LOG_H
#define HK_CONSTRAINTSOLVER2_SOLVER_LOG_H



#if !defined (HK_PLATFORM_SPU) 
//#	define USE_SOLVER_DEBUG_OSTREAM "havokSolverLog.txt"	
#endif

#if defined USE_SOLVER_DEBUG_OSTREAM
#include <Common/Base/System/Io/Writer/Array/hkArrayStreamWriter.h>
#endif

#if defined(USE_SOLVER_DEBUG_OSTREAM)
#	define USE_SOLVER_DEBUG_INTERNAL_STEPS
#	if defined(HK_PLATFORM_SPU)
#   include <Common/Base/Spu/Simulator/hkSpuSimulator.h>
	extern hkSpuSimulator::Client* debugOstream;
#	else
	extern hkArray<char>* debugBuffer;
	extern hkOstream* debugOstream;
	extern hkOfstream* debugOutputFile;
#	endif
#endif

#ifdef USE_SOLVER_DEBUG_INTERNAL_STEPS
#	define ON_SOLVER_DEBUG_INTERNAL_STEPS(a) a
#else
#	define ON_SOLVER_DEBUG_INTERNAL_STEPS(a) 
#endif

//#define HK_DO_DEBUG_CHAINS
#if defined HK_DO_DEBUG_CHAINS
	extern hkOstream* chainOstream;
#	define HK_CHAIN_DEBUG_PRINTF1(a) if(chainOstream)chainOstream->printf(a);
#else
#	define HK_CHAIN_DEBUG_PRINTF1(a) 
#endif


//#define USE_SOLVER_DEBUG_OSTREAM_FOR_CONTACT_POINTS

inline void createSolverDebugOstream()
{
#ifdef USE_SOLVER_DEBUG_OSTREAM
	if ( !debugOstream )
	{
#	if defined (HK_PLATFORM_SPU)
		debugOstream = hkSpuSimulator::Client::getInstance();
#	else
		debugBuffer = new hkArray<char>;
		debugOstream = new hkOstream(*debugBuffer);
		debugOutputFile = new hkOfstream(USE_SOLVER_DEBUG_OSTREAM);
#	endif
	}
#endif
}


inline void	flushSolverDebugOstream()
{
#ifdef USE_SOLVER_DEBUG_OSTREAM
	if (debugOstream)
	{
#	if defined (HK_PLATFORM_SPU)
		debugOstream->flushToFile();
#	else
		hkArrayStreamWriter* arrayWriter = static_cast<hkArrayStreamWriter*>(debugOstream->getStreamWriter());
		debugOutputFile->write( (char*)arrayWriter->getData(), arrayWriter->getDataSize() );
		arrayWriter->clear();
#	endif
	}
#endif
}


#endif // HK_CONSTRAINTSOLVER2_SOLVER_LOG_H

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
