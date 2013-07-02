/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Processor applying a transformation to a scene in user-specified ways
///
/// Applies a transformation to all scene objects
class VGEOM2_IMPEXP_CLASS VGProcessor_Transformator : public VGDynamicProcessor<VGProcessor_Transformator>//@@@ test
{
public:
  enum TransformationMode
  {
    VGTF_ROOT_NODES,       // Transform (all) root nodes of scene relative to origin.
    VGTF_ROOT_NODES_LOCAL, // Transform (all) root nodes of scene relative to their own transform (in their own local space).
    VGTF_MESH_NODES,       // Transform (only) mesh and mesh proxy nodes of scene relative to origin.
    VGTF_MESH_NODES_LOCAL  // Transform (only) mesh and mesh proxy nodes of scene relative to their own transform (in their own local space).
  };


public:
  VGProcessor_Transformator();
  virtual ~VGProcessor_Transformator();


  /// \brief
  ///   Processes a scene - applies a transformation to all scene objects
  ///
  /// \param scene
  ///   Scene containing data to bake
  ///
  /// \return
  ///   False if transformation failed
  virtual bool              Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired transformation matrix
  ///
  /// \param m
  ///   Transformation matrix
  inline void               SetTransformation(const hkvMat4& m)             { m_m = m; }

  /// \brief
  ///   Gets the currently set transformation matrix
  ///
  /// \return
  ///   Currently set transformation matrix
  inline const hkvMat4&     GetTransformation() const throw()               { return m_m; }



  /// \brief
  ///   Sets the desired transformation mode for animations
  ///
  /// \param mode
  ///   Transformation mode for animation keys
  inline void               SetTransformationMode(TransformationMode mode)  { m_mode = mode; }

  /// \brief
  ///   Gets the currently set transformation mode for animations
  ///
  /// \return
  ///   Currently set transformation mode for animation keys
  inline TransformationMode GetTransformationMode() const throw()           { return m_mode; }



private:
  void TransformAnims(VGSkeleton& s) const;
  void TransformAnims(VGMesh&     m) const;


private:
  hkvMat4            m_m;
  TransformationMode m_mode;
};

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
