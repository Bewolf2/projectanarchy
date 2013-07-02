/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   UV coordinate generator
///
/// Generates UV coordinates for meshes in a scene. This processor uses D3DX for computation,
/// thus introducing an external dependency on Direct3D. Generates lightmap coordinates (uv set
/// index 1) by default.
/// This generates distinct UV coordinate sets per per mesh and referenced material.
class VGEOM2_IMPEXP_CLASS VGProcessor_UVGenerator : public VGDynamicProcessor<VGProcessor_UVGenerator>
{
public:
  VGProcessor_UVGenerator();
  virtual ~VGProcessor_UVGenerator();


  enum UVGenerationMode
  {
    VGUV_PER_MATERIAL,          // Generates UVs from 0 to 1 for all meshes in the scene per material (default) @@@ test
    VGUV_PER_MESH_PER_MATERIAL  // Generates UVs from 0 to 1 per mesh and per material
  };




  /// \brief
  ///   Processes a scene - generates UV coordinates
  ///
  /// \param scene
  ///   Scene containing meshes to generate UVs for
  ///
  /// \return
  ///   False if generation failed
  virtual bool            Process(VGScene& scene) const;



  /// \brief
  ///   Sets the index of the UV channel to generate coordinates for
  ///
  /// \param i
  ///   UV set index (0-7)
  inline void             SetUVSetIndex(unsigned int i) throw()       { m_iUVSet = i; }

  /// \brief
  ///   Gets the currently set index of the UV channel to generate coordinates for
  ///
  /// \return
  ///   UV set index (0-7)
  inline unsigned int     GetUVSetIndex() const throw()               { return m_iUVSet; }



  /// \brief
  ///   Sets the lightmap stretch factor to be used
  ///
  /// \param f
  ///   Lightmap stretch factor
  inline void             SetPreferredStretchFactor(float f) throw()  { m_fPreferredStretchFactor = f; }

  /// \brief
  ///   Gets the currently set lightmap stretch factor
  ///
  /// \return
  ///   Lightmap stretch factor
  inline float            GetPreferredStretchFactor() const throw()   { return m_fPreferredStretchFactor; }



  /// \brief
  ///   Sets the lightmap chart size to be used
  ///
  /// \param i
  ///   Lightmap chart size
  inline void             SetChartSize(unsigned int i) throw()        { m_fChartSize = i; }

  /// \brief
  ///   Gets the currently set lightmap chart size
  ///
  /// \return
  ///   Lightmap chart size
  inline unsigned int     GetChartSize() const throw()                { return m_fChartSize; }

  /// \brief
  ///   Sets the lightmap Gutter size to be used
  ///
  /// \param f
  ///   Lightmap Gutter size
  inline void             SetGutterSize(float f) throw()           { m_fGutterSize = f; }

  /// \brief
  ///   Gets the currently set lightmap Gutter size
  ///
  /// \return
  ///   Lightmap Gutter size
  inline float            GetGutterSize() const throw()        { return m_fGutterSize; }



  /// \brief
  ///   Sets whether or not generator should update lightmap page IDs of referenced materials
  ///
  /// \param b
  ///   Enable/disable
  inline void             SetUpdateLightmapPageIDs(bool b) throw()    { m_updateLightmapPageIDs = b; }

  /// \brief
  ///   Gets whether or not generator is set to update lightmap page IDs of referenced materials, or not
  ///
  /// \return
  ///   Lightmap chart size
  inline bool             GetUpdateLightmapPageIDs() const throw()    { return m_updateLightmapPageIDs; }



  /// \brief
  ///   Sets the desired generation mode
  ///
  /// \param m
  ///   Generation mode
  inline void             SetMode(UVGenerationMode m) throw()         { m_mode = m; }

  /// \brief
  ///   Gets the currently set generation mode
  ///
  /// \return
  ///   Currently set generation mode
  inline UVGenerationMode GetMode() const throw()                     { return m_mode; }



  /// \brief
  ///   This processor uses D3DX functions for internal computations - this functions allows users to
  ///   use an already existing Direct3D device for those
  ///
  /// \param f
  ///   Function pointer to device getter (getter is allowed to return NULL)
  inline void SetCustomD3DDeviceGetter(IDirect3DDevice9*(*f)()) /*@@@ put in some base class to share with tangent generator*/ { m_customD3DDeviceGetter = f; }


private:
  // Helpers.
  void Process_PerMeshPerMaterial(VGScene& scene, const class D3DDeviceHelper& d3dDevHelper) const;
  void Process_PerMaterial(VGScene& scene, const class D3DDeviceHelper& d3dDevHelper) const;
  bool ProcessMeshData(const class D3DDeviceHelper& d3dDevHelper, VGVertexList& vertexList, VGTriangleList& triList) const;
private:
  unsigned int     m_iUVSet;
  float            m_fPreferredStretchFactor;
  unsigned int     m_fChartSize;
  bool             m_updateLightmapPageIDs;
  UVGenerationMode m_mode;
  float            m_fGutterSize;
  IDirect3DDevice9*(*m_customD3DDeviceGetter)();
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
