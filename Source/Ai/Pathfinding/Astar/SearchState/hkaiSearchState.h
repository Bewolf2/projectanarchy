/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HKAI_ASTAR_SEARCHSTATE_H
#define HKAI_ASTAR_SEARCHSTATE_H

/// A single node in an A* search.
struct hkaiSearchStateNode
{
		/// Flags for nodes, used during A*
	enum FlagValues
	{
		/// No flags set
		FLAGS_NONE = 0,

		/// Node has been opened but not closed yet
		FLAGS_OPEN = 1,

		/// Node has been examined and closed
		FLAGS_CLOSED = 2,

		/// Node is the start point in the search
		FLAGS_START = 4,

		/// Node is the end point in the search
		FLAGS_END = 8,
	};

	typedef hkFlags<FlagValues,hkInt16> Flags;

	/// Initialize the node with a large cost and no parent
	HK_FORCE_INLINE void init()
	{
		
		m_gCost = HK_REAL_MAX;
		m_hCost = HK_REAL_MAX;
		m_index = hkUint32(-1);
		m_parentIndex = -1;
		m_flags = FLAGS_NONE;
	}


	HK_FORCE_INLINE void setFlags( Flags f)
	{
		m_flags =  f;
	}

	HK_FORCE_INLINE void setStart()
	{
		m_flags.orWith( FLAGS_OPEN | FLAGS_START );
	}

	HK_FORCE_INLINE void setEnd()
	{
		m_flags.orWith( FLAGS_END );
	}

	HK_FORCE_INLINE void setOpen()
	{
		m_flags.setWithMask( FLAGS_OPEN, FLAGS_OPEN|FLAGS_CLOSED );
	}

	HK_FORCE_INLINE void setClosed()
	{
		m_flags.setWithMask( FLAGS_CLOSED, FLAGS_OPEN|FLAGS_CLOSED );
	}

	HK_FORCE_INLINE hkBool32 isOpen() const
	{
		return m_flags.get(FLAGS_OPEN);
	}

	HK_FORCE_INLINE hkBool32 isClosed() const
	{
		return m_flags.get(FLAGS_CLOSED);
	}

	HK_FORCE_INLINE hkBool32 isStart() const
	{
		return m_flags.get(FLAGS_START);
	}

	HK_FORCE_INLINE hkBool32 isEnd() const
	{
		return m_flags.get(FLAGS_END);
	}

	HK_FORCE_INLINE hkBool32 isNew() const
	{
		return m_flags.get(FLAGS_OPEN|FLAGS_CLOSED) == 0;
	}

	HK_FORCE_INLINE hkReal getGCost() const
	{
		return m_gCost;
	}

	HK_FORCE_INLINE void setGCost(hkReal g)
	{
		m_gCost = g;
	}

	/// The cost of travelling from the start node to this node
	HK_ALIGN16(hkReal m_gCost);

	/// The approximate (heuristic) cost of travelling from this node to the goal node
	hkReal m_hCost;

	/// This node's index (i.e. nav mesh edge, nav volume cell)
	hkUint32 m_index;

	/// The index of this node's parent, or -1 if it doesn't have one.
	/// This is an absolute index into the hkaiHashSearchState's m_node array.
	hkInt16 m_parentIndex;

protected:
	/// Flags representing the state of the node
	Flags m_flags;
};

/// A single node in an A* search.
/// This is deprecated and only used by hkaiIndexedSearchState, but kept around in case it becomes handy later
struct hkaiDeprecatedSearchStateNode
{
		/// Flags for nodes, used during A*
	enum FlagValues
	{
			/// No flags set
		FLAGS_NONE = 0,

			/// Node has been opened but not closed yet
		FLAGS_OPEN = 1,

			/// Node has been examined and closed
		FLAGS_CLOSED = 2,

			/// Node is the start point in the search
		FLAGS_START = 4,

			/// Node is the end point in the search
		FLAGS_END = 8,
	};

	typedef hkFlags<FlagValues,int> Flags;

		/// Initialize the node with a large cost and no parent
	HK_FORCE_INLINE void init()
	{
#ifdef HK_PLATFORM_PS3_SPU
		// HK_REAL_MAX in hex is 7F7FFFFF
		const vector unsigned int initVec = (vector unsigned int) { 0x7F7FFFFF, 0x7F7FFFFF, 0xFFFFFFFF, 0};
		*(vector unsigned int*)this = initVec;
#else
		m_gCost = HK_REAL_MAX;
		m_hCost = HK_REAL_MAX;
		m_parent = -1;
		m_flags = FLAGS_NONE;
#endif
	}


	HK_FORCE_INLINE void setFlags( Flags f)
	{
		m_flags =  f;
	}

	HK_FORCE_INLINE void setStart()
	{
		m_flags.orWith( FLAGS_OPEN | FLAGS_START );
	}

	HK_FORCE_INLINE void setEnd()
	{
		m_flags.orWith( FLAGS_END );
	}

	HK_FORCE_INLINE void setOpen()
	{
		m_flags.setWithMask( FLAGS_OPEN, FLAGS_OPEN|FLAGS_CLOSED );
	}

	HK_FORCE_INLINE void setClosed()
	{
		m_flags.setWithMask( FLAGS_CLOSED, FLAGS_OPEN|FLAGS_CLOSED );
	}

	HK_FORCE_INLINE hkBool32 isOpen() const
	{
		return m_flags.get(FLAGS_OPEN);
	}

	HK_FORCE_INLINE hkBool32 isClosed() const
	{
		return m_flags.get(FLAGS_CLOSED);
	}

	HK_FORCE_INLINE hkBool32 isStart() const
	{
		return m_flags.get(FLAGS_START);
	}

	HK_FORCE_INLINE hkBool32 isNew() const
	{
		return m_flags.get(FLAGS_OPEN|FLAGS_CLOSED) == 0;
	}

	HK_FORCE_INLINE int getParent() const
	{
		return m_parent;
	}

	HK_FORCE_INLINE void setParent(int p)
	{
		m_parent = p;
	}

	HK_FORCE_INLINE hkReal getGCost() const
	{
		return m_gCost;
	}

	HK_FORCE_INLINE void setGCost(hkReal g)
	{
		m_gCost = g;
	}

		/// The cost of travelling from the start node to this node
	hkReal m_gCost;
	
		/// The approximate (heuristic) cost of travelling from this node to the goal node
	hkReal m_hCost;
		
		/// The index of this node's parent, or -1 if it doesn't have one
	int m_parent;

protected:
		/// Flags representing the state of the node
	Flags m_flags;
};

#endif // HKAI_ASTAR_SEARCHSTATE_H

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
