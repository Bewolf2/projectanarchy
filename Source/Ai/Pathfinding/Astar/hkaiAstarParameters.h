/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_PARAMETERS_H
#define HKAI_ASTAR_PARAMETERS_H

	/// Basic output parameters for A* searches
struct hkaiAstarOutputParameters
{
	//+version(1)
	HK_DECLARE_REFLECTION();
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI_ASTAR, hkaiAstarOutputParameters);

	hkaiAstarOutputParameters()
	: m_numIterations(0)
	, m_goalIndex(-1)
	, m_status(SEARCH_IN_PROGRESS)
	, m_terminationCause(NOT_TERMINATED)
	{}

	hkaiAstarOutputParameters( hkFinishLoadedObjectFlag f ) {}

	/// Return codes for A* searches
	enum SearchStatus
	{
			/// The search still has potential paths to examine.
		SEARCH_IN_PROGRESS, 

			/// The search found a path from the start to the goal.
		SEARCH_SUCCEEDED, 

			/// The search could not find a path.
		SEARCH_UNREACHABLE,

			/// The search was stopped before a path was found (e.g., because it ran out of memory)
			/// The reason for stopping is set in described by the TerminationCause.
		SEARCH_TERMINATED,

			/// The search found a path from the start to the goal, but there wasn't enough memory to write all the outputs.
		SEARCH_SUCCEEDED_BUT_RESULTS_TRUNCATED,

			/// The search could not execute because the input parameters were invalid
		SEARCH_INVALID,
	};

		/// If the search is stopped, the reason for the stoppage.
	enum TerminationCause
	{
			/// The search wasn't terminated (default)
		NOT_TERMINATED = 0,

			/// The search was stopped because the maximum iterations were reached.
		TERMINATED_ITERATION_LIMIT,
		
			/// The search was stopped because the open set was full.
		TERMINATED_OPEN_SET_FULL,

			/// The search was stopped because the search state was full.
		TERMINATED_SEARCH_STATE_FULL,
		
			/// The user terminated the search. This is currently only possible in flood fills.
		TERMINATED_BY_USER,
	};

		/// The number of iterations that were performed before terminating.
	HK_ALIGN16(int m_numIterations); //+default(0)
	
		/// The index of the goal that was reached, or -1 if no goal reached
	int m_goalIndex; //+default(-1)

		/// The status of the search after it finished.
	hkEnum<hkaiAstarOutputParameters::SearchStatus, hkUint8> m_status; //+default(hkaiAstarOutputParameters::SEARCH_IN_PROGRESS)

		/// Why the search was terminated. Only set if m_status is SEARCH_TERMINATED.
	hkEnum<hkaiAstarOutputParameters::TerminationCause, hkUint8> m_terminationCause; //+default(hkaiAstarOutputParameters::NOT_TERMINATED)

	// Shouldn't need padding for alignment?
};

#endif // HKAI_ASTAR_PARAMETERS_H

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
