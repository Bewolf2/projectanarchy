/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VBBoxOctree.hpp

#ifndef VISION_VBBOXOCTREEBBOX_HPP
#define VISION_VBBOXOCTREEBBOX_HPP


// We don't want to include this in the documentation.
#ifndef _VISION_DOC


#include <Vision/Runtime/Base/Math/BoundingVolume/hkvAlignedBBox.h>


/// \brief
///   Description for one element (bounding box and element ID). This structure is only required
///   for setting up the octree (VBBoxOctree::Build). When later getting the relevant elements only
///   the element IDs will be returned.
struct VBBoxOctreeNodeElemDesc
{
  int m_elementID;
  hkvAlignedBBox m_bBox;
};


/// \brief
///   Represents one octree node which can be either the root node or any of the children. Each
///   node either contains a list of children or a list of elements - in case it is a leaf node.
///   This class is used by the VBBoxOctree class and usually doesn't have to be accessed
///   externally.
class VBBoxOctreeNode
{
public:
  // Constructor/Destructor
  VBASE_IMPEXP VBBoxOctreeNode( hkvAlignedBBox &bBox );
  VBASE_IMPEXP ~VBBoxOctreeNode();


  /// \brief
  ///   Filters the relevant elements for this node and either passes them down to new child nodes
  ///   (in case there are more relevant elements than specified in iMaxElementsInNode) or stores
  ///   the elements within this node and stops traversing.
  VBASE_IMPEXP void Build(VArray<VBBoxOctreeNodeElemDesc*,VBBoxOctreeNodeElemDesc*> &elemDescs, int iMaxElementsInNode, int maxOctreeDepth = 6);


  /// \brief
  ///   Adds the element of this node to the passed array in case the passed bounding box touches
  ///   the bounding box of this node. Additionally the call is passes down to the child nodes.
  VBASE_IMPEXP void GetRelevantElements( hkvAlignedBBox &bBox, VPList &foundElements );

  // Accessing child nodes
  VBASE_IMPEXP int GetNumChildrenRecursive() const;
  VBASE_IMPEXP int GetNumChildren() const;
  inline VBool HasChildren() const { return GetNumChildren() > 0; }
  inline VBBoxOctreeNode *GetChild(int iIndex) const { return m_pChildren[iIndex]; }
  VBASE_IMPEXP int GetNumLevelsRecursive() const;
  VBASE_IMPEXP void DumpOctreeToStringRecursive(VString &out, int iIndentation) const;
  VBASE_IMPEXP int GetMemoryConsumption() const;

  // Accessing this node
  inline const hkvAlignedBBox& GetBBox() const { return m_bBox; }

  // Accessing elements
  inline int GetNumElements() const { return m_pElementIDs->GetSize(); } 
  inline int GetElementID(int iIndex) const { return m_pElementIDs->GetAt(iIndex); }

private:
  hkvAlignedBBox GetChildNodeBox(int iChildNr);

  hkvAlignedBBox m_bBox;
  VArray<int,int> *m_pElementIDs;
  VBBoxOctreeNode *m_pChildren[8];
};


/// \brief
///   Octree with a list of elements where each element is represented by an axis aligned bounding
///   box. Setting up the octree is done with the Build function, getting the relevant elements for
///   a specified bounding box is done with the GetRelevantElements functions.
class VBBoxOctree
{
public:
  // Constructor/Destructor
  VBASE_IMPEXP VBBoxOctree();
  VBASE_IMPEXP ~VBBoxOctree();
  
  /// \brief
  ///   Build the octree
  /// 
  /// Take the passed element descriptions and generate a octree containing the elements.
  /// 
  /// \param pElementDescriptions
  ///   array of element descriptions containing the element bounding boxes and IDs. This data will
  ///   be copied within this function and thus can be deleted afterwards.
  /// 
  /// \param iNumElements
  ///   size of the pElementDescriptions array
  /// 
  /// \param iMaxElementsPerNode
  ///   Maximum number of elements in one node. If there are more elements in a node then child
  ///   nodes are created instead.
  /// 
  /// \return
  ///   BOOL: returns FALSE if iMaxElementsPerNode == 0 
  VBASE_IMPEXP BOOL Build(VBBoxOctreeNodeElemDesc *pElementDescriptions, int iNumElements, int iMaxElementsPerNode);


  /// \brief
  ///   Get the IDs of all elements which are near the passed bounding box
  /// 
  /// Traverse the octree, find the octree (leaf) nodes which touch the passed bounding box and add
  /// all the element IDs of these nodes to the passed array. The function makes sure that no
  /// duplicated element IDs are stored in the array.
  /// 
  /// \param bBox
  ///   relevant bounding box
  /// 
  /// \param foundElements
  ///   pointer to an vplist where the found element integer IDs are stored in
  /// 
  /// \return
  ///   int: number of found elements
  /// 
  /// \note
  ///   Attention: Since this function performs the check on the node-bbox level you will get all
  ///   elements which are "near" the passed bounding box.
  VBASE_IMPEXP int GetRelevantElements( hkvAlignedBBox &bBox, VPList &foundElements );

  /// \brief
  ///   Calculates the memory consumption for the complete octree in bytes
  VBASE_IMPEXP int GetMemoryConsumption() const;

  // Accessing the nodes
  VBASE_IMPEXP int GetNumNodes() const;
  VBASE_IMPEXP int GetNumLevels() const;
  VBASE_IMPEXP VBBoxOctreeNode *GetRootNode() { return m_pRootNode; }
  VBASE_IMPEXP VString DumpOctreeToString() const;

private:
  VBBoxOctreeNode *m_pRootNode;
};

#endif  //_VISION_DOC

#endif // VISION_VBBOXOCTREEBBOX_HPP

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
