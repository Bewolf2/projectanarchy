/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionData.hpp

#ifndef VBASE_VCOLLISIONDATA_HPP
#define VBASE_VCOLLISIONDATA_HPP

// We don't want to include this in the documentation.
#ifndef _VISION_DOC


#include <Vision/Runtime/Base/Physics/VCollisionNode.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionMesh32.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionSurface.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VChunkFile.hpp>


/// \brief
///   Container class for collision data used by the VCollisionMesh classes
/// 
/// It contains three flat lists of collision data of different levels:
/// \li The node list contains all the nodes of the scene graph. Each node is implemented as a
///   VCollisionNode object (q.v.) which contains the name, type, world matrix and blind data of
///   the node, as well as references to its child nodes (these nodes MUST be present in the node
///   list as well). Also, nodes of specific types may contain a reference to one of the collision
///   meshes in the mesh list of this data class.
/// 
/// \li The mesh list contains all collision meshes in the scene. Each mesh is implemented as a
///   VSimpleCollisionMesh32 object (q.v.) which contains the vertices for the mesh and for each triangle
///   in the mesh it contains three indices to the triangle's vertices and a reference to one of
///   the collision surfaces in the surface list of this data class to describe the triangle's
///   surface.
/// 
/// \li The surface list contains all collision surfaces in the scenes. Each surface is implemented
///   as a VCollisionSurface object (q.v.) which simply contains the material name of the surface
///   it represents. VCollisionData is derived from VChunkFile so that it can save and load its own
///   data to/form a file. It overloads the OnStartSaving and OnStartChunk callbacks from
///   VChunkFile to achieve this. A SPECIAL NOTE ON ADDING NODES, MESHES AND SURFACES: When
///   creating a node, mesh or surface then you'll have to add it to the data structure TWO TIMES.
///   One time with the AddNode/AddMesh/AddSurface function in this class and one time with a
///   function in the parent object to which you want to connect it (i.e. surfaces to meshes,
///   meshes to nodes, child nodes to parent nodes).
class VCollisionData : public VChunkFile {

public:

  // constructor/destructor
  // NOTE: the destructor will delete all the objects stored in its lists.
  VBASE_IMPEXP VCollisionData();
  VBASE_IMPEXP ~VCollisionData();
  
  // Data access functions
  
  // flat node list

  /// \brief
  ///   Get the number of nodes for this collision data.
  /// 
  /// \return
  ///   int: the number of nodes
  VBASE_IMPEXP inline int GetNodeCount() const {return m_collNodeList.GetLength();}
  
  /// \brief
  ///   Find the root node in the flat node list.
  /// 
  /// It does this by returning the first node that is found that has no parent.
  /// 
  /// \return
  ///   VCollisionNode*: the root node, or NULL if no root node was found.
  VBASE_IMPEXP inline VCollisionNode* GetRootNode() const
  {
    for (int i = 0; i < GetNodeCount(); i++)
    {
      if (GetNode(i)->GetParent() == NULL)
        return GetNode(i);
    }
    return NULL;
  }

  /// \brief
  ///   Get the node for a specific index from the flat list.
  /// 
  /// \param idx
  ///   the index of the node to retrieve.
  /// 
  /// \return
  ///   VCollisionNode*: the node at the specified index
  VBASE_IMPEXP inline VCollisionNode* GetNode(int idx) const {return m_collNodeList.Get(idx);}
  
  /// \brief
  ///   Add a new node to the collision data's flat node list.
  /// 
  /// WARNING: this function will not automatically add a child node to its parent. You must to do
  /// that additionally with the VCollisionNode::AddChild function.
  /// 
  /// \param node
  ///   the node to add to the flat list.
  VBASE_IMPEXP inline void AddNode(VCollisionNode* node) {m_collNodeList.AddUnique(node);}
  
  /// \brief
  ///   Get the index of the specified node in the flat node list.
  /// 
  /// \param node
  ///   the node to get the index for.
  /// 
  /// \return
  ///   int: the index of the specified node.
  VBASE_IMPEXP inline int GetNodeIndex(const VCollisionNode* node) const {return m_collNodeList.Find(node);}


  // mesh list

  /// \brief
  ///   Get the number of meshes in this collision data.
  /// 
  /// \return
  ///   int: the number of meshes.
  VBASE_IMPEXP inline int GetMeshCount() const {return m_collMeshList.GetLength();}

  /// \brief
  ///   Get the mesh for a specific index from the mesh list.
  /// 
  /// \param idx
  ///   the index of the mesh to retrieve.
  /// 
  /// \return
  ///   VSimpleCollisionMesh32*: the mesh at the specified index
  VBASE_IMPEXP inline VSimpleCollisionMesh32* GetMesh(int idx) const {return m_collMeshList.Get(idx);}

  /// \brief
  ///   Add a new mesh to the collision data's flat mesh list.
  /// 
  /// WARNING: this function will not automatically add a mesh to the correct node. You must to do
  /// that additionally with the VCollisionNode::SetMesh function.
  /// 
  /// \param mesh
  ///   the mesh to add to the mesh list.
  VBASE_IMPEXP inline void AddMesh(VSimpleCollisionMesh32* mesh) {m_collMeshList.AddUnique(mesh);}

  /// \brief
  ///   Get the index of the specified mesh in the surface list.
  /// 
  /// \param mesh
  ///   the mesh to get the index for.
  /// 
  /// \return
  ///   int: the index of the specified mesh.
  VBASE_IMPEXP inline int GetMeshIndex(const VSimpleCollisionMesh32* mesh) const {return m_collMeshList.Find(mesh);}


  // surface list

  /// \brief
  ///   Get the number of surfaces in this collision data.
  /// 
  /// \return
  ///   int: the number of surfaces.
  VBASE_IMPEXP inline int GetSurfaceCount() const {return m_collMaterialList.GetLength();}

  /// \brief
  ///   Get the surface for a specific index from the surface list.
  /// 
  /// \param idx
  ///   the index of the surface to retrieve.
  /// 
  /// \return
  ///   VCollisionSurface*: the surface at the specified index
  VBASE_IMPEXP inline VCollisionSurface* GetSurface(int idx) const {return m_collMaterialList.Get(idx);}

  /// \brief
  ///   Add a new surface to the collision data's flat surface list.
  /// 
  /// WARNING: this function will not automatically add a surface to the correct triangle in the
  /// correct mesh. You must to do that additionally with the VSimpleCollisionMesh32::SetCollisionSurface
  /// function.
  /// 
  /// \param surf
  ///   the surface to add to the surface list.
  VBASE_IMPEXP inline void AddSurface(VCollisionSurface* surf) {m_collMaterialList.AddUnique(surf);}

  /// \brief
  ///   Get the index of the specified surface in the surface list.
  /// 
  /// \param surf
  ///   the surface to get the index for.
  /// 
  /// \return
  ///   int: the index of the specified surface.
  VBASE_IMPEXP inline int GetSurfaceIndex(const VCollisionSurface* surf) const {return m_collMaterialList.Find(surf);}


  // loading/saving callbacks ... these should not be called directly
  virtual void OnStartSaving();
  virtual BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  
private:

  VPListT<VCollisionNode>    m_collNodeList;
  VPListT<VSimpleCollisionMesh32>  m_collMeshList;
  VPListT<VCollisionSurface> m_collMaterialList;

};


#endif // _VISION_DOC



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
