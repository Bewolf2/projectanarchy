/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSet.hpp

#ifndef VSET_HPP_INCLUDED
#define VSET_HPP_INCLUDED 


#ifndef _VISION_DOC

/// \brief
///   Template class for red-black trees
/// 
/// TODO:  deletion
template <typename T>
class VRBTree
{
public:
  /// \brief
  ///   Default constructor
  VRBTree() : m_pRoot(0), m_pBegin(0), m_nCount(0)
  {
  }

  /// \brief
  ///   Destructor
  virtual ~VRBTree()
  {
    delete m_pRoot;
  }

  /// \brief
  ///   Insertion operator
  bool Insert(const T &x)
  {
    Node *pNode = new Node(x);
    VASSERT(pNode);

    if (!m_pRoot)
    {
      m_pRoot = m_pBegin = pNode;
    }
    else
    {
      if (x < m_pBegin->val)
      {
        m_pBegin = pNode;
      }

      if (!InsertUnbalanced(pNode, m_pRoot))
      {
        delete pNode;
        return false;
      }
    }
    m_nCount++;

    // Fix any invariants that were broken:
    // 1. The root is black
    // 2. All leaves are black
    // 3. Both children of every red node are black
    // 4. Every simple path from a given node to any of its descendant leaves contains the same number of black nodes

    while (true)
    {
      if (!pNode->parent)
      {
        // restore property 1
        pNode->color = Node::Black;
      }
      else if (pNode->parent->color == Node::Red)
      {
        // property 3 no longer holds
        Node *pGP = Grandparent(pNode);
        Node *pUncle = 0;
        if (pGP)
        {
          if (pNode->parent == pGP->link[0])
          {
            pUncle = pGP->link[1];
          }
          else
          {
            pUncle = pGP->link[0];
          }
        }

        if (pUncle && pUncle->color == Node::Red)
        {
          pNode->parent->color = pUncle->color = Node::Black;
          pGP->color = Node::Red;
          pNode = pGP;
          continue;
        }
        else
        {
          if ((pNode == pNode->parent->link[1]) && (pNode->parent == pGP->link[0]))
          {
            // the node is a right child, the parent is a left child
            Rotate(1, pNode->parent);
            pNode = pNode->link[0];
          }
          else if ((pNode == pNode->parent->link[0]) && (pNode->parent == pGP->link[1]))
          {
            // the node is a left child, the parent is a right child
            Rotate(0, pNode->parent);
            pNode = pNode->link[1];
          }

          pNode->parent->color = Node::Black;
          pGP->color = Node::Red;

          if ((pNode == pNode->parent->link[0]) && (pNode->parent == pGP->link[0]))
          {
            // the node is a left child, the parent is a left child
            Rotate(0, pGP);
          }
          else
          {
            // the node is a right child, the parent is a right child
            Rotate(1, pGP);
          }
        }
      }

      break;
    }
    return true;
  }

  /// \brief
  ///   Element count
  size_t Count() const
  {
    return m_nCount;
  }

protected:
  
  struct Node
  {
    enum Color { Red, Black };

    // Leaf
    Node(const T &x)
    : color(Red), val(x), parent(0)
    {
      link[0] = link[1] = 0;
    }

    ~Node()
    {
      delete link[0];
      delete link[1];
    }

    void SetChild(int idx, Node *pNode)
    {
      if (pNode)
      {
        pNode->parent = this;
      }
      link[idx] = pNode;
    }

    Node *Sibling()
    {
      if (!parent)
      {
        return 0;
      }
      else if (this == parent->link[0])
      {
        return parent->link[1];
      }
      else
      {
        return parent->link[0];
      }
    }

    Node *Min()
    {
      Node *pNode = this;
      while (pNode->link[0])
      {
        pNode = pNode->link[0];
      }
      return pNode;
    }

    Node *Max()
    {
      Node *pNode = this;
      while (pNode->link[1])
      {
        pNode = pNode->link[1];
      }
      return pNode;
    }

    Color color;
    T val;
    Node *parent;
    Node *link[2];
  };


  /// \brief 
  ///   Performs a left or right rotation depending on the pivot index
  inline Node *Rotate(size_t pivot, Node *pNode)
  {
    size_t sibling = 1 - pivot;

    Node *pPivot = pNode->link[pivot];
    pNode->SetChild(pivot, pPivot->link[sibling]);
    pPivot->parent = pNode->parent;
    if (!pNode->parent)
    {
      m_pRoot = pPivot;
    }
    else if (pNode == pNode->parent->link[0])
    {
      pNode->parent->link[0] = pPivot;
    }
    else
    {
      pNode->parent->link[1] = pPivot;
    }

    pPivot->SetChild(sibling, pNode);  
    return pPivot;
  }

  /// \brief 
  ///   BST insertion
  bool InsertUnbalanced(Node *pNode, Node *pAfter)
  {
    if (pNode->val < pAfter->val)
    {
      if (pAfter->link[0])
      {
        return InsertUnbalanced(pNode, pAfter->link[0]);
      }
      else
      {
        pAfter->link[0] = pNode;
        pNode->parent = pAfter;
        return true;
      }
    }
    else if (pNode->val > pAfter->val)
    {
      if (pAfter->link[1])
      {
        return InsertUnbalanced(pNode, pAfter->link[1]);
      }
      else
      {
        pAfter->link[1] = pNode;
        pNode->parent = pAfter;
        return true;
      }
    }
    return false;
  }

  /// \brief 
  ///   Parent node of parent node
  Node *Grandparent(Node *pNode)
  {
    if ((pNode != 0) && (pNode->parent != 0))
    {
      return pNode->parent->parent;
    }
    return 0;
  }

  /// \brief 
  ///   Sibling node of parent node
  Node *Uncle(Node *pNode)
  {
    if (!pNode->parent)
    {
      return 0;
    }
    return pNode->parent->Sibling();
  }

  Node *m_pRoot;
  Node *m_pBegin;
  size_t m_nCount;
};

/// \brief
///   VSet class
///
template <typename T>
class VSet : public VRBTree<T>
{
public:
  typedef VSet<T> SetType;
  typedef typename VRBTree<T>::Node Node;
  class Iterator;

  /// \brief 
  ///   Search for an element of the set
  Iterator Find(const T &x) const
  {
    Node *pNode = VRBTree<T>::m_pRoot;
    while (pNode)
    {
      if (x == pNode->val)
      {
        return Iterator(pNode);
      }
      pNode = pNode->link[x > pNode->val];
    }
    return End();
  }

  /// \brief 
  ///   Begin iterator
  Iterator Begin() const
  {
    return Iterator(VRBTree<T>::m_pBegin);
  }

  /// \brief 
  ///   End iterator
  Iterator End() const
  {
    return Iterator(0);
  }

  /// \brief 
  ///   Iterator class
  class Iterator
  {
  public:
    //! Default constructor
    Iterator() : m_pNode(0)
    { 
    }

    //! Copy constructor
    Iterator(const Iterator &other) : m_pNode(other.m_pNode)
    {
    }

    //! Equality
    bool operator == (const Iterator &other) const
    {
      return (m_pNode == other.m_pNode);
    }

    //! Inequality
    bool operator != (const Iterator &other) const
    {
      return (m_pNode != other.m_pNode);
    }

    //! Preincrement
    Iterator &operator ++()
    {
      Advance();
      return *this;
    }

    //! Postincrement
    Iterator operator ++(int)
    {
      Advance();
      return *this;
    }

    //! Dereference
    const T &operator *() const
    {
      return m_pNode->val;
    }

  protected:
    friend class VSet<T>;
    explicit Iterator(typename SetType::Node *start) : m_pNode(start)
    {
    }

    /// \brief 
    ///   Advances the iterator
    void Advance()
    {
      if (m_pNode)
      {
        // in an in-order traversal, the leftmost descendant of the right child, when it exists, is always next.  
        // otherwise, we're seeking an ancestor node that has not yet been traversed.  this is true if, and only if, 
        // there exists a path from the ancestor's left child to the current node -- so we just walk back up the 
        // tree until we find an ancestor that is its parent's left child.
        if (m_pNode->link[1])
        {
          m_pNode = m_pNode->link[1]->Min();
        }
        else
        {
          Node *pParent = m_pNode->parent;
          while (pParent && m_pNode == pParent->link[1])
          {
            m_pNode = pParent;
            pParent = m_pNode->parent;
          }
          m_pNode = pParent;
        }
      }
    }
    typename SetType::Node *m_pNode;
  };


protected:
  friend class Iterator;
};

/// \brief 
///   Key-value pair
template <typename A, typename B>
struct VPair
{
  VPair(const A &fst, const B &snd)
    : fst(fst), snd(snd)
  {
  }

  /// \brief 
  ///   Inherit the ordering from the first element type
  bool operator < (const VPair<A,B> &other)
  {
    return fst < other.fst;
  }

  A fst;
  B snd;
};

#endif  // _VISION_DOC

#endif //VSET_HPP_INCLUDED

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
