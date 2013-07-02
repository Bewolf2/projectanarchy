/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPListStack.hpp

#ifndef H_VISION_VSTACK
#define H_VISION_VSTACK

#ifndef _VISION_DOC

/// \brief
///   Memory-friendly stack implementation. Only supports pointers.
template <class T> class  VPListStack 
{
public:
  // default construct with default blocksize 4096
  VPListStack () 
  {
    m_blocksize = 4096;
    Init ();
  }

  // construct with setable blocksize. 
  // the blocksize sets the number of T elements allocated when growing
  VPListStack (int blocksize) 
  {
    m_blocksize = blocksize;
    Init ();   
  }

  // delete all memory  
  ~VPListStack () 
  {
    for (int i=0; i<m_totalblocks; i++) {
      VASSERT ( m_memblocklist[i] );
      delete [] (T*)m_memblocklist[i];
    }
    m_memblocklist.Reset ();
  }

  // push a value on the stack
  inline void Push (T value)
  {
    if (m_curentpos==m_blocksize) 
    {
      m_curentpos = 0;
      m_curentblock++;
      if (m_curentblock == m_totalblocks) 
      {
        // allocate a new block
        m_memblocklist.Append(new T[m_blocksize]);
        m_totalblocks++;
      }
      m_pcurentblock=(T*)m_memblocklist[m_curentblock];
      m_pcurentblock[m_curentpos] = value;
      m_curentpos++;
    } else {
      m_pcurentblock[m_curentpos] = value;
      m_curentpos++;
    }    
  }

  // pop a value from the stack
  // note: if the stack is empty this function will crash
  inline T Pop () 
  {
    if (m_curentpos!=0) 
    {
      // just return and pop
      m_curentpos--;      
      return m_pcurentblock[m_curentpos];     
    } else {
      // go down one block
      VASSERT ( m_curentblock>0 );
      m_curentblock--;
      m_pcurentblock = (T*)m_memblocklist[m_curentblock];
      m_curentpos = m_blocksize-1;      
      return m_pcurentblock[m_curentpos];     
    }    
  }

  // gets the total number of entries in the stack
  int GetDepth () 
  {
    return m_curentblock*m_blocksize + m_curentpos;
  }

  // get the total memory allocated (not used) for the stack.
  int GetMemory ()
  {
    return m_totalblocks * m_blocksize * sizeof(T);
  }

  // checks whether the stack is empty. you should always do that before popping
  inline bool IsEmpty () 
  {
    if (m_curentblock!=0) return false;
    if (m_curentpos!=0) return false;
    return true;    
  }

  // free all allocated but not used memory (within blocksize boundaries)
  void Compact ()
  {
    int oldtotal = m_totalblocks;
    m_totalblocks = m_curentblock+1;
    for ( int i=m_totalblocks; i<oldtotal; i++)
    {
      delete m_memblocklist[i];
      m_memblocklist[i] = NULL;
    }
  }

private:

  void Init () {    
    m_blocks = 0;
    m_curentblock = 0;
    m_curentpos = 0;    
    m_totalblocks = 1;
    // allocate one block to start with
    T* pnewblock = new T[m_blocksize];
    m_memblocklist.Append(pnewblock);
    m_pcurentblock = pnewblock;
  }

  VPList m_memblocklist;
  int m_blocksize;
  int m_blocks;
  int m_curentblock;
  int m_curentpos;
  int m_totalblocks;  
  T* m_pcurentblock;
};



#endif // _VISION_DOC

#endif   // H_VISION_VSTACK

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
