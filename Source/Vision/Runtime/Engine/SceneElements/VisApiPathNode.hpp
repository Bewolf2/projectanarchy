/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPathNode.hpp

// VisApiPathNode.hpp: interface for the VisPathNode_cl class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DEFINE_VISAPIPATHNODE
#define DEFINE_VISAPIPATHNODE


#include <Vision/Runtime/Base/Math/Quaternion/hkvQuat.h>

typedef VSmartPtr<class VisPathNode_cl> VisPathNodePtr;


// In and Out type of the pathnode 
#define VT_LINEAR   0
#define VT_BEZIER   1

// helper macros to set and get the type
#define VECTOR_TYPE(in,out)   (UBYTE)((out << 4) + in)
#define IN_VECTOR_TYPE(x)     (x & 15)
#define OUT_VECTOR_TYPE(x)    (x >> 4)

#define IN_VECTOR 0
#define OUT_VECTOR 1


#define PATH_STEPS       100             ///< startvalue for the algorithm in VisPathNode_cl::GetLen
#define PATH_STARTVALUE  0.01f           ///< stepvalue for the algorithm in VisPathNode_cl::GetLen
#define PATH_STEPVALUE   0.01f           ///< number of steps for the algorithm in VisPathNode_cl::GetLen
#define PATH_DELTA       0.0001f         ///< length difference for the algorithm in VisPathNode_cl::EvalPointSmooth


/// \brief
///   Path node class
/// 
/// One instance of a VisPathNode_cl class represents a single node inside a VisPath_cl object and
/// is always attached to a path.
/// 
/// A path node is defined by its position and the in- and out- tangent control vertices.
/// 
/// \sa VisPath_cl
class VisPathNode_cl : public VRefCounter, public VUserDataObj, public VisObjectKey_cl
{
  V_DECLARE_SERIALX( VisPathNode_cl, VISION_APIFUNC )

public:

  enum PathNodeType_e { LINEAR = 0, BEZIER = 1}; ///< enumeration for the spline type
  
  ///
  /// @name Constructors, Destructors
  /// @{
  ///

  /// \brief
  ///   Constructor that takes the node position, 2 tangent control vertices and the in/out tangent
  ///   type
  /// 
  /// \param pos
  ///   Position of this node in world space
  /// 
  /// \param cv1
  ///   Position of the in - control vertex in world space
  /// 
  /// \param cv2
  ///   Position of the out - control vertex in world space
  /// 
  /// \param typeInOut
  ///   constant that defines the in and out type of the tangents. Use the VECTOR_TYPE macro to
  ///   combine the types, e.g. VECTOR_TYPE(LINEAR,LINEAR)
  VISION_APIFUNC VisPathNode_cl(const hkvVec3& pos = hkvVec3(), const hkvVec3& cv1 = hkvVec3(-8.0f,0.0f,0.0f), const hkvVec3& cv2 = hkvVec3(-8.0f,0.0f,0.0f), UBYTE typeInOut = VECTOR_TYPE(LINEAR,LINEAR));

  /// \brief
  ///   Copy constructor for VisPathNode_cl
  VISION_APIFUNC VisPathNode_cl(const VisPathNode_cl &copyNode);


  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~VisPathNode_cl();

  ///
  /// @}
  ///

  ///
  /// @name Node Position
  /// @{
  ///

  /// \brief
  ///   DEPRECATED, use SetPosition instead
  HKV_DEPRECATED_2012_1 inline void SetPos(const hkvVec3& pos) {SetPosition(pos);}

  /// \brief
  ///   Sets the position of this path node in world space
  /// 
  /// Internally, this function calculates the local space position relatively to the owner path so
  /// moving the path updates the node position properly.
  /// 
  /// \param pos
  ///   New node position in world space
  VISION_APIFUNC void SetPosition(const hkvVec3& pos);

  /// \brief
  ///   DEPRECATED, use GetPosition instead
  HKV_DEPRECATED_2012_1 inline const hkvVec3& GetPos() const {return GetPosition();}

  /// \brief
  ///   Gets the position of this path node in world space
  /// 
  /// \return
  ///   const hkvVec3& pos : position vector
  inline const hkvVec3& GetPosition() const {return m_vPosition;}


  ///
  /// @}
  ///

  ///
  /// @name Control Vertices
  /// @{
  ///


  /// \brief
  ///   Sets the position of the control vertices in world space
  /// 
  /// Internally, this function calculates the local space position relatively to the owner path so
  /// moving the path updates the node control vertices properly.
  /// 
  /// \param cv1
  ///   Absolute position of the IN control vertex
  /// 
  /// \param cv2
  ///   Absolute position of the OUT control vertex
  VISION_APIFUNC void SetControlVertices(const hkvVec3& cv1, const hkvVec3& cv2);
  
  /// \brief
  ///   Gets the absolute positions of both control vertices.
  /// 
  /// \param cv1
  ///   Reference that receives the absolute position of the IN control vertex
  /// 
  /// \param cv2
  ///   Reference that receives the absolute position of the OUT control vertex
  inline void GetControlVertices(hkvVec3& cv1, hkvVec3& cv2) const;

  /// \brief
  ///   Sets the tangent generation type for the IN tangent vector
  /// 
  /// \param typeIn
  ///   The tangent generation type for the in-tangent. Supported types are:
  ///   \li VisPathNode_cl::LINEAR: The in-control vertex is ignored; instead the tangent is
  ///     computed using the relative position of the neighbour node.
  /// 
  ///   \li VisPathNode_cl::BEZIER: The in-control vertex defines the in-tangent for bezier
  ///     interpolation.
  VISION_APIFUNC void SetTypeIn(PathNodeType_e typeIn);
  
  /// \brief
  ///   Sets the tangent generation type for the OUT tangent vector
  /// 
  /// \param typeOut
  ///   The tangent generation type for the out-tangent. Supported types are:
  ///   \li VisPathNode_cl::LINEAR: The out-control vertex is ignored; instead the tangent is
  ///     computed using the relative position of the neighbour node.
  /// 
  ///   \li VisPathNode_cl::BEZIER: The out-control vertex defines the out-tangent for bezier
  ///     interpolation.
  VISION_APIFUNC void SetTypeOut(PathNodeType_e typeOut);


  /// \brief
  ///   Makes the control vertices of this node colinear to each other by mirroring the relative
  ///   positions.
  /// 
  /// The parameter defines whether the IN vector is modifies according to the OUT vector or vice
  /// versa.
  /// 
  /// \param inOrOutNode
  ///   Defines which control vertex remains constant. Possible constants are:
  ///   \li IN_VECTOR: Make the in vector colinear to the out vector
  /// 
  ///   \li OUT_VECTOR: Make the out vector colinear to the in vector
  VISION_APIFUNC void MakeControlVerticesColinear(int inOrOutNode);

  ///
  /// @}
  ///

  ///
  /// @name Segment Length
  /// @{
  ///


  /// \brief
  ///   Evaluates the length between this node and the passed node
  /// 
  /// The length is approximated by subdividing the path into segments. The number of segments is
  /// defined by the PATH_STEPS constant, which is currently set to 100.
  /// 
  /// The length is not cached. Thus this operation is performace critical.
  /// 
  /// \param next
  ///   Reference to the next node
  /// 
  /// \return
  ///   float result: length of the spline in world units
  inline float GetLen(const VisPathNode_cl &next) const;

  ///
  /// @}
  ///

  ///
  /// @name Debug Functions
  /// @{
  ///


  /// \brief
  ///   Puts node information (pPosition, control vertex 1, control vertex 2, intype/outtype) into
  ///   a string for debug purposes
  /// 
  /// \return
  ///   VString result: node information
  VISION_APIFUNC VString ToString() const;


  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///


  /// \brief
  ///   Serializes this node object.
  /// 
  /// For more information about serialization, see the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to
  VISION_APIFUNC void SerializeX( VArchive &ar );
  VISION_APIFUNC void SerializeX( VArchive &ar, bool bHasVersion );
  
  ///
  /// @}
  ///

private:
  friend class VisPath_cl;   


  ///
  /// @name Private Helper Functions
  /// @{
  ///



  /// \brief
  ///   Evaluates a point on the spline between this node and the next node.
  /// 
  /// Evaluates a point on the basis of t. t is not linear to the length  of the spline, describe
  /// be this node an the next node.
  /// 
  /// \param next
  ///   Reference to the next node
  /// 
  /// \param t
  ///   float value between 0.0f and 1.0f indicating the point
  /// 
  /// \param pos
  ///   Reference to a vector, in which the evaluated  position is stored.
  /// 
  /// \param firstDerivative
  ///   If this pointer is not NULL, then the first derivative of the evaluated position is saved
  ///   there
  /// 
  /// \param secondDerivative
  ///   If this pointer is not NULL, then the second derivative of the evaluated position is saved
  ///   there
  inline void EvalPoint(const VisPathNode_cl &next, float t, hkvVec3& pos, hkvVec3* firstDerivative = NULL, hkvVec3* secondDerivative = NULL) const;
  
  /// \brief
  ///   Evaluates a point on the spline between this node and the next node.
  /// 
  /// Evaluates a point on the basis of t. t is linear to the length  of the spline, describe be
  /// this node an the next node.
  /// 
  /// \param next
  ///   Reference to the next node
  /// 
  /// \param t
  ///   float value between 0.0f and 1.0f indicating the point
  /// 
  /// \param pos
  ///   Reference to a vector, in which the evaluated  position is stored.
  /// 
  /// \param firstDerivative
  ///   If this pointer is not NULL, then the first derivative of the evaluated position is saved
  ///   there
  /// 
  /// \param secondDerivative
  ///   If this pointer is not NULL, then the second derivative of the evaluated position is saved
  ///   there
  inline void EvalPointSmooth(const VisPathNode_cl &next, float distance, hkvVec3& pos, hkvVec3* firstDerivative = NULL, hkvVec3* secondDerivative = NULL, float segLen = -1.0f ) const;

  /// \brief
  ///   Called internally when the owner path changed position
  VISION_APIFUNC void UpdateWorldSpacePosition();

  /// \brief
  ///   called internally by VisPath_cl::AddPathNode to attach it to a new parent
  VISION_APIFUNC void SetParentPath(VisPath_cl *pPath);

  // internal function
  VISION_APIFUNC void UpdateLinearTangents();

  // members
  int m_iNodeIndex;
  hkvVec3 m_vPosition;            ///< world space position of the path node
  hkvVec3 m_vControlVertices[2];  ///< position of two control certives
  UBYTE inoutType;                  ///< IN type (bits 0-3) and OUT type (bits 4-7) of the path node
  VisPath_cl *m_pParentPath;        ///< owner path

  // local position information, same as in object3d
  hkvVec3 m_vLocalPosition;
  hkvVec3 m_vControlLocalPosition[2];
  mutable int m_iLastDiv;
  mutable float m_fLastLen;

  ///
  /// @}
  ///

};

#define VISAPIPATHNODE_INL_INCLUDED_BY_VISAPIPATHNODE_H
#include <Vision/Runtime/Engine/SceneElements/VisApiPathNode.inl>
#undef VISAPIPATHNODE_INL_INCLUDED_BY_VISAPIPATHNODE_H

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
