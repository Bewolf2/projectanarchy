/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VKDTree.hpp

#ifndef MU_VISION_VKDTREE_HPP
#define MU_VISION_VKDTREE_HPP

/// \brief
///   KDNode class for kd-trees
/// 
/// Simple node class.
template<class Vert> class VKDNode
{
public:
  VKDNode(int index, int discr) : 
    m_smaller(NULL), 
    m_greater(NULL), 
    m_iIndex(index),
    m_discr(discr)
    {};

  // data members
  
  int    m_iIndex; ///< index to vertex element
  int    m_discr;  ///< discriminator ( splitting dimension )

  VKDNode* m_smaller; ///< link to node with smaller value in m_discr dimension
  VKDNode* m_greater; ///< link to node with greater value in m_discr dimension
};



/// \brief
///   Class that builds a multidimensional binary tree (kd-tree).
/// 
/// \note
///   The constructor takes the data structure, sorts and splits the vertex list in the middle for the
///   first dimension, sorts and split the resulting 2 vertex lists in the middle in the next
///   dimension, and so forth.
template<class Vert, int Dim> class VKDTree
{
public:
  /// \brief
  ///   Builds the tree structure and initializes the index array. This is the only entry point from
  ///   outside for recursively building the tree.
  /// 
  /// \param n
  ///   number of vertex elements
  /// 
  /// \param v
  ///   pointer to vertex list
  VKDTree(int n, Vert* v);
  
  /// \brief
  ///   Destroys the tree and frees its data
  ~VKDTree();

  /// \brief
  ///   Returns a list of VKDNodes found in a k dimensional bounding shape with equal sides. The bounding shape is assumed to be axis-aligned.
  /// 
  /// \param range
  ///   k dimensional coordinates for bounding shape center
  /// 
  /// \param epsilon
  ///   Half the size of the n-dimensional bounding shape
  /// 
  /// \return
  ///   VPList* : list of VKDNodes found inside the bounding shape
  VPList* Range(Vert& range, float epsilon);

private:

  /// \brief
  ///   Private function for sorting all or a subsets of vertices in the specified dimension.
  /// 
  /// \param index
  ///   starting point in the index table
  /// 
  /// \param n
  ///   number of vertices to sort
  /// 
  /// \param discr
  ///   dimension in which to sort
  void MySort( int index, int n, char discr, int middle );

  /// \brief
  ///   Private function for sorting all or a subset of vertices in the specified dimension.
  /// 
  /// \param index
  ///   starting point in the index table
  /// 
  /// \param n
  ///   number of vertices to sort
  /// 
  /// \param discr
  ///   dimension in with to sort
  void QuickSort( int index, int n, char discr, int middle );
    
  /// \brief
  ///   Private function for recursive tree building
  /// 
  /// \param index
  ///   starting point in the index table
  /// 
  /// \param n
  ///   number of vertices to add
  /// 
  /// \param discr
  ///   dimension
  /// 
  /// \return
  ///   VKDNode* : resulting node
  VKDNode<Vert>* AddNode(int index, int n, int discr);
  
  /// \brief
  ///   Private function for recursive tree destruction. Used internally in the destructor for the
  ///   root node.
  /// 
  /// \param pKDNode
  ///   node to be destroyed
  void Delete(VKDNode<Vert>* pKDNode);

  /// \brief
  ///   Private function for recursive searching. This function adds the found nodes to the list.
  /// 
  /// \param index
  ///   starting point in the index table
  /// 
  /// \param n
  ///   number of vertices to add
  /// 
  /// \param discr
  ///   dimension
  void RangeSearch(VKDNode<Vert>* pKDNode);

// data members

  VKDNode<Vert>* m_pRoot; ///< the root of the tree
  Vert*  m_VertArray;     ///< pointers to the vertices list
  VPList m_RangeList;     ///< list of nodes founded in range
  float m_epsilon;        ///< epsilon for range searching
  Vert m_range;           ///< centre of range searching 
  int* m_IxTbl;           ///< index table for vertices list
  void (VKDTree::*m_pSort) (int, int, char, int); ///< pointer to the optimal sorting function
};


template <class Vert, int Dim>
VKDTree<Vert, Dim>::VKDTree(int n, Vert* v) : m_VertArray(v)
{
  if ( n > 0 )
  {
    m_IxTbl = new int[n];
  
    // initialize the index table
    for ( int i = 0; i < n; i++)
      m_IxTbl[i] = i;

    if ( Dim > 2 )
      m_pSort = &VKDTree::QuickSort;
    else
      m_pSort = &VKDTree::MySort;
  
    // build recursive the tree and hold root node
    m_pRoot = AddNode(0, n, 0);
  }
  else
  {
    m_pRoot = NULL;
    m_IxTbl = NULL;
  }
}

template <class Vert, int Dim>
VKDTree<Vert, Dim>::~VKDTree()
{
  // delete recursive the tree
  if ( m_pRoot != NULL)
    Delete(m_pRoot);

  // delete index table
  if (m_IxTbl != NULL)
    delete [] m_IxTbl;
}


template <class Vert, int Dim>
VPList* VKDTree<Vert, Dim>::Range(Vert& range, float epsilon)
{
  // reset node list
  m_RangeList.Reset();

  m_epsilon = epsilon;
  m_range = range;

  // recursive search
  RangeSearch(m_pRoot);

  return &m_RangeList;
}

  
// -------------------------------------------------------------------------------
// Notes:   This function must only rearrange the indexes so that the middle vertex
//          must be greater that all the adder left and smaller that all the vertices
//          right.
// -------------------------------------------------------------------------------
template <class Vert, int Dim>
void VKDTree<Vert, Dim>::MySort( int index, int n, char discr, int middle )
{
  VBool bSorted = FALSE;
  int count = 0;

  /*
  while (!bSorted)
  {
    bSorted = TRUE; count++;
    
    // get middle
    Vert vMiddle = m_VertArray[m_IxTbl[middle]];
    
    // sort the list one step 
    for ( int i = index; i < n / 2 + index; i++)
    {  
      // get element left
      Vert left = m_VertArray[m_IxTbl[i]];
      if (left.data[discr] > vMiddle.data[discr])
      {
        // swap indexes
        int temp = m_IxTbl[middle];
        m_IxTbl[middle] = m_IxTbl[i];
        m_IxTbl[i] = temp;

        // actiualize vMiddle
        vMiddle = m_VertArray[m_IxTbl[middle]];
        bSorted = FALSE;
      }

      // get element right
      int j = n - i - 1 + 2 * index;
      Vert right = m_VertArray[m_IxTbl[j]];
      if (right.data[discr] < vMiddle.data[discr])
      {
        // swap indexes
        int temp = m_IxTbl[middle];
        m_IxTbl[middle] = m_IxTbl[j];
        m_IxTbl[j] = temp;

        // actiualize vMiddle
        vMiddle = m_VertArray[m_IxTbl[middle]];
        bSorted = FALSE;
      }
    }
  }
  */
}
  
template <class Vert, int Dim>
VKDNode<Vert>* VKDTree<Vert, Dim>::AddNode(int index, int n, int discr)
{  
  if (n < 1)
    return NULL;

  int middle = index + n / 2;
  if ( n > 1) 
    (this->*m_pSort)( index, n, discr, middle );
 
  // add node
  VKDNode<Vert>* pNode = new VKDNode<Vert>(m_IxTbl[middle], discr);
  
  if (n > 1)
  {  
    discr = ( discr + 1 ) % Dim; ///< increase discriminant
    
    // add left branch
    pNode->m_smaller = AddNode( index, n / 2, discr );
    
    // add right branch
    pNode->m_greater = AddNode( n / 2 + index + 1, (n - 1) / 2, discr );
  }
  
  return pNode;
}

template <class Vert, int Dim>
void VKDTree<Vert, Dim>::Delete(VKDNode<Vert>* pKDNode)
{
  // delete left branch
  if (pKDNode->m_smaller != NULL)
    Delete( pKDNode->m_smaller);
  
  // delete right branch
  if (pKDNode->m_greater != NULL)
    Delete( pKDNode->m_greater);
  
  delete pKDNode;
}

template <class Vert, int Dim>
void VKDTree<Vert, Dim>::RangeSearch(VKDNode<Vert>* pKDNode)
{
  Vert& v = m_VertArray[pKDNode->m_iIndex];
  VBool bInRange = TRUE;
  // test if the vertex is in range
  for ( int i = 0; i < Dim; i++)
    if ( m_range.data[i] < v.data[i] - m_epsilon || 
      m_range.data[i] > v.data[i] + m_epsilon )
    {
      bInRange = FALSE;
      break;
    }

  if (bInRange)
    m_RangeList.Append(pKDNode);

  int discr = pKDNode->m_discr;
  
  // search in right branch
  if (pKDNode->m_greater && v.data[discr] <= m_range.data[discr] + m_epsilon)
    RangeSearch(pKDNode->m_greater);
    
  // search in left branch
  if (pKDNode->m_smaller && v.data[discr] >= m_range.data[discr] - m_epsilon)
    RangeSearch(pKDNode->m_smaller);
}

// -------------------------------------------------------------------------------
// Note:    Second sorting algorithm. Specially used for geometry vertices.
// -------------------------------------------------------------------------------
template <class Vert, int Dim>
void VKDTree<Vert, Dim>::QuickSort(int index, int n, char discr,  int middle)
{
  int lo = index; 
  int hi = index + n - 1;
 
  int pivot = m_IxTbl[(lo + hi) / 2];
  m_IxTbl[(lo + hi) / 2] = m_IxTbl[hi];
  m_IxTbl[hi] = pivot;
  
  while( lo < hi ) 
  {
    // Search forward until an element is found that
    // is greater than the pivot or lo >= hi 
    while (m_VertArray[m_IxTbl[lo]].data[discr] <= m_VertArray[pivot].data[discr] && lo < hi) 
      lo++;
    
    // Search backward until element is found that
    // is less than the pivot, or lo >= hi
    while (m_VertArray[pivot].data[discr] <= m_VertArray[m_IxTbl[hi]].data[discr] && lo < hi )
      hi--;
    
    // Swap elements a[lo] and a[hi]
    if( lo < hi ) 
    {
      int temp = m_IxTbl[lo];
      m_IxTbl[lo] = m_IxTbl[hi];
      m_IxTbl[hi] = temp;
    }
  }

  m_IxTbl[index + n - 1] = m_IxTbl[hi];
  m_IxTbl[hi] = pivot;
  
  if ( hi == middle )
    return;
  
  if ( lo > middle ) 
    QuickSort(index, lo - index, discr, middle);
  else if ( hi < middle )
    QuickSort(hi + 1, index + n - hi - 1, discr, middle);
}


#endif // MU_VISION_VKDTREE_HPP

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
