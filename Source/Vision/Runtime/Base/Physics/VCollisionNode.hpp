/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionNode.hpp

#ifndef VBASE_VCOLLISIONNODE_HPP
#define VBASE_VCOLLISIONNODE_HPP

#ifndef _VISION_DOC

#include <Vision/Runtime/Base/Container/VDictionary.hpp>
#include <Vision/Runtime/Base/Physics/VCollisionMesh32.hpp>

class VCollisionData;

// version number for the collision node chunk in the collision data file
#define VCOLLISIONNODE_CHUNK_VERSION 1

/// \brief
///   Enumeration holding the differtent available node types
enum eNodeDataType {
  NODEDATA_POLY_SHAPE = 0,          ///< polygonal shape
  NODEDATA_STATIC_MESH_SHAPE = 1,   ///< static mesh shape (e.g. imported from 3DS Max / Maya)
  NODEDATA_CSG_GROUP = 2,           ///< csg shape
  NODEDATA_PATCH_SHAPE = 3,         ///< bezier patch shape
  NODEDATA_PORTAL_SHAPE = 5,        ///< visibility portal shape
  NODEDATA_EVENT_AREA_SHAPE = 6,    ///< event area shape
  NODEDATA_ENTITY_SHAPE = 7,        ///< entity shape
  NODEDATA_XREF_SHAPE = 8,          ///< crossreference to another vEdit map
  NODEDATA_LIGHT_SHAPE = 9,         ///< lightsource shape
  NODEDATA_WALLMARK_SHAPE = 10,     ///< wallmark shape
  NODEDATA_PATH_SHAPE = 11,         ///< path shape
  NODEDATA_OTHER = 1000             ///< other shapes (e.g. root, group shape, ...)
};


/// \brief
///   VCollisionNode : A class representing a node in the scene graph that may contain collision
///   data. Each node may contain a collision and additional blind data which is simply implemented
///   as a string dictionary.
/// 
/// \note
///   Only the types NODEDATA_POLY_SHAPE, NODEDATA_STATIC_MESH_SHAPE and NODEDATA_CSG_SHAPE will
///   currently contain a collision mesh.
/// 
/// \note
///   This class is part of the VCollisionData hierarchy.
class VCollisionNode
{
public:
  
  // constructor/destructor
  VBASE_IMPEXP VCollisionNode();
  VBASE_IMPEXP ~VCollisionNode();
  
  /// \brief
  ///   Add a reference to a node as a child to this node.
  /// 
  /// The child node's parent will be set to this node with this call. In addition to setting the
  /// node as a child to this node, you MUST also add the child node to the overlying
  /// VCollisionData object, otherwise it will not be saved/deleted properly later on.
  /// 
  /// \param node
  ///   the reference to the node to add as child to this node.
  VBASE_IMPEXP inline void AddChild(VCollisionNode* node) {m_childList.Append(node); node->m_pParent = this;}

  /// \brief
  ///   Get a reference to the child node at a specific index.
  /// 
  /// \param idx
  ///   the index of the child node to retrieve.
  /// 
  /// \return
  ///   VCollisionNode*: the child node at the specified index
  VBASE_IMPEXP inline VCollisionNode* GetChild(int idx) const {return m_childList.Get(idx);}

  /// \brief
  ///   Get the number of children of this node.
  /// 
  /// \return
  ///   int: the number of children
  VBASE_IMPEXP inline int GetChildCount() const {return m_childList.GetLength();}

  /// \brief
  ///   Get a reference to the parent node of this node.
  /// 
  /// \return
  ///   VCollisionNode*: this node's parent, or NULL if this is a root node.
  VBASE_IMPEXP inline VCollisionNode* GetParent() const {return m_pParent;}

  /// \brief
  ///   Get a string describe the full path to this node (e.g. "root/CSG Group/Poly 1")
  /// 
  /// \param sFullPath
  ///   reference to a VString that should be filled with the full path string.
  VBASE_IMPEXP void GetFullPath(VString& sFullPath) const;



  /// \brief
  ///   Get the name of this node.
  /// 
  /// \return
  ///   const char*: the name of this node.
  VBASE_IMPEXP inline const char* GetName() const {return m_name;}

  /// \brief
  ///   Set the name of this node.
  /// 
  /// \param name
  ///   the new name of this node.
  VBASE_IMPEXP inline void SetName(const char* name) {m_name = name;}



  /// \brief
  ///   Get the type of geometry data that this node contains.
  /// 
  /// \return
  ///   eNodeDataType: the type of geometry data in this node.
  VBASE_IMPEXP inline eNodeDataType GetType() const {return m_type;}

  /// \brief
  ///   Set the type of geometry data that this node contains.
  /// 
  /// \param newType
  ///   the new type of geometry data in this node.
  VBASE_IMPEXP inline void SetType(eNodeDataType newType) {m_type = newType;}



  /// \brief
  ///   Get the world matrix for this node.
  /// 
  /// \return
  ///   const hkvMat4&: the world matrix for this node.
  /// 
  /// \note
  ///   This 4x4 matrix is the matrix that transforms each vertex in the mesh to world space.
  /// 
  /// \note
  ///   The rotation 3x3 part can be scaled. Thus, to get the matrix that transforms a world space
  ///   position to local position the Invert() function has to be used rather than just
  ///   transposing  its 3x3 part.
  /// 
  /// \note
  ///   To convert this matrix to a float based hkvMat4, an overload of the global AssignMatrix
  ///   function can be used.
  VBASE_IMPEXP inline const hkvMat4& GetMatrix() const {return m_matrix;}

  /// \brief
  ///   Set the world matrix for this node.
  /// 
  /// \param matrix
  ///   the new world matrix for this node.
  VBASE_IMPEXP inline void SetMatrix(const hkvMat4& matrix) {m_matrix = matrix;}


  /// \brief
  ///   Get the blind data in this node.
  /// 
  /// \return
  ///   const VStrDictionary&: the blind data in this node.
  VBASE_IMPEXP inline const VStrDictionary& GetBlindData() const {return m_blindData;}

  /// \brief
  ///   Set the blind data for this node.
  /// 
  /// \param blindData
  ///   the new blind data for this node.
  VBASE_IMPEXP inline void SetBlindData(const VStrDictionary& blindData) {m_blindData = blindData;}

  /// \brief
  ///   Get the blind data key at a certain index in the blind data dictionary.
  ///
  /// \param idx
  ///   The index in the blind data directory
  ///
  /// \return
  ///   const char*: the blind data key at the request index in the blind data dictionary.
  VBASE_IMPEXP inline const char* GetBlindDataKey(int idx) const {return m_blindData.GetKey(idx);}

  /// \brief
  ///   Get the blind data value at a certain index in the blind data dictionary.
  /// 
  /// \param idx
  ///   The index in the blind data directory
  ///
  /// \return
  ///   VString: the blind data value at the request index in the blind data dictionary.
  VBASE_IMPEXP inline VString GetBlindDataValue(int idx) const {return m_blindData.GetValue(idx);}
  //    - const char*: the blind data value at the request index in the blind data dictionary.
  //TODO: VBASE_IMPEXP inline const char* GetBlindDataValue(int idx) const {return m_blindData.GetValue(idx);}
  

  /// \brief
  ///   Get the number of key/value pairs of the blind data in this node.
  /// 
  /// \return
  ///   int: the number of key/value pairs of the blind data in this node.
  VBASE_IMPEXP inline int GetBlindDataLength() const {return m_blindData.GetLength();}



  /// \brief
  ///   Get the collision mesh in this node.
  /// 
  /// \return
  ///   VSimpleCollisionMesh*: the collision mesh in this node, or NULL if this node has no collision
  ///   mesh.
  VBASE_IMPEXP inline VSimpleCollisionMesh32* GetMesh() const {return m_pMesh;}

  /// \brief
  ///   Set a reference to a collision mesh in this node.
  /// 
  /// In addition to setting the reference to the mesh here, you MUST also add the collision mesh
  /// to the overlying VCollisionData object, otherwise it will not be saved/deleted properly later
  /// on.
  /// 
  /// \param pMesh
  ///   the reference to the collision mesh to set in this node.
  VBASE_IMPEXP inline void SetMesh(VSimpleCollisionMesh32* pMesh) {m_pMesh = pMesh;}


  // loading/saving callbacks ... these should not be called directly
  void OnStartSaving(VCollisionData* pCollData);
  void OnLoading(VCollisionData* pCollData, int*& piChildIndices, int& iNumChildren);

protected:

  VString                 m_name;
  eNodeDataType           m_type;
  hkvMat4                 m_matrix;
  mutable VStrDictionary  m_blindData;
  VSimpleCollisionMesh32* m_pMesh;

  VPListT<VCollisionNode> m_childList;
  VCollisionNode*         m_pParent;

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
