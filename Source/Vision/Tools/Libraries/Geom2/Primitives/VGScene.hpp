/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Scene class
///
/// Describes and setups a scene for usage and processing inside vGeom2.
class VGEOM2_IMPEXP_CLASS VGScene
{
public:
  VGScene();
  VGScene(const VGScene& other);
  virtual ~VGScene();



  /// \brief
  ///   Adds a new mesh
  ///
  /// \param m
  ///   Mesh to add - will be copied
  inline void                  AddMesh(const VGMesh& m)                { m_meshes.Add(new VGMesh(m)); }

  /// \brief
  ///   Creates a new mesh and returns a reference to it (same as implicitly adding a new mesh)
  ///
  /// \param vm
  ///   Vertex mask to create the mesh with
  ///
  /// \return
  ///   Reference to newly added mesh
  inline VGMesh&               CreateMesh(VGVertex::VertexMask vm)     { m_meshes.Add(new VGMesh(vm)); return GetMesh(GetNumMeshes()-1); }

  /// \brief
  ///   Returns number of meshes
  ///
  /// \return
  ///   Number of meshes
  inline int                   GetNumMeshes() const                    { return m_meshes.GetSize(); }

  /// \brief
  ///   Returns a reference to a mesh
  ///
  /// \param i
  ///   Mesh index
  ///
  /// \return
  ///   Mesh
  inline       VGMesh&         GetMesh(int i)                          { return *m_meshes[i]; }
  inline const VGMesh&         GetMesh(int i) const                    { return *m_meshes[i]; }

  /// \brief
  ///   Removes a mesh
  ///
  /// \param i
  ///   Mesh index
  inline void                  RemoveMesh(int i)                       { delete m_meshes[i]; m_meshes.RemoveAt(i); }



  /// \brief
  ///   Adds a new light
  ///
  /// \param l
  ///   Light to add - will be copied
  inline void                  AddLight(const VGLight& l)              { m_lights.Add(new VGLight(l)); }

  /// \brief
  ///   Creates a new light and returns a reference to it (same as implicitly adding a new light)
  ///
  /// \return
  ///   Reference to newly added light
  inline VGLight&              CreateLight()                           { m_lights.Add(new VGLight()); return GetLight(GetNumLights()-1); }

  /// \brief
  ///   Returns number of lights
  ///
  /// \return
  ///   Number of lights
  inline int                   GetNumLights() const                    { return m_lights.GetSize(); }

  /// \brief
  ///   Returns a reference to a light
  ///
  /// \param i
  ///   Light index
  ///
  /// \return
  ///   Light
  inline       VGLight&        GetLight(int i)                         { return *m_lights[i]; }
  inline const VGLight&        GetLight(int i) const                   { return *m_lights[i]; }

  /// \brief
  ///   Removes a light
  ///
  /// \param i
  ///   Light index
  inline void                  RemoveLight(int i)                      { delete m_lights[i]; m_lights.RemoveAt(i); }



  /// \brief
  ///   Adds a new mesh proxy
  ///
  /// \param m
  ///   Mesh proxy to add - will be copied
  inline void                  AddMeshProxy(const VGMeshProxy& m)      { m_meshProxies.Add(new VGMeshProxy(m)); }

  /// \brief
  ///   Creates a new mesh proxy and returns a reference to it (same as implicitly adding a new mesh proxy)
  ///
  /// \return
  ///   Reference to newly added mesh proxy
  inline VGMeshProxy&          CreateMeshProxy()                       { m_meshProxies.Add(new VGMeshProxy()); return GetMeshProxy(GetNumMeshProxies()-1); }

  /// \brief
  ///   Returns number of mesh proxies
  ///
  /// \return
  ///   Number of mesh proxies
  inline int                   GetNumMeshProxies() const               { return m_meshProxies.GetSize(); }

  /// \brief
  ///   Returns a reference to a mesh proxy
  ///
  /// \param i
  ///   Mesh proxy index
  ///
  /// \return
  ///   Mesh proxy
  inline       VGMeshProxy&    GetMeshProxy(int i)                     { return *m_meshProxies[i]; }
  inline const VGMeshProxy&    GetMeshProxy(int i) const               { return *m_meshProxies[i]; }

  /// \brief
  ///   Removes a mesh proxy
  ///
  /// \param i
  ///   Mesh proxy index
  inline void                  RemoveMeshProxy(int i)                  { delete m_meshProxies[i]; m_meshProxies.RemoveAt(i); }



  /// \brief
  ///   Adds a new dummy
  ///
  /// \param m
  ///   Dummy to add - will be copied
  inline void                  AddDummy(const VGDummy& m)              { m_dummies.Add(new VGDummy(m)); }

  /// \brief
  ///   Creates a new dummy and returns a reference to it (same as implicitly adding a new dummy)
  ///
  /// \return
  ///   Reference to newly added dummy
  inline VGDummy&              CreateDummy()                           { m_dummies.Add(new VGDummy()); return GetDummy(GetNumDummies()-1); }

  /// \brief
  ///   Returns number of dummies
  ///
  /// \return
  ///   Number of dummies
  inline int                   GetNumDummies() const                   { return m_dummies.GetSize(); }

  /// \brief
  ///   Returns a reference to a dummy
  ///
  /// \param i
  ///   Dummy index
  ///
  /// \return
  ///   Dummy
  inline       VGDummy&        GetDummy(int i)                         { return *m_dummies[i]; }
  inline const VGDummy&        GetDummy(int i) const                   { return *m_dummies[i]; }

  /// \brief
  ///   Removes a dummy
  ///
  /// \param i
  ///   Dummy index
  inline void                  RemoveDummy(int i)                      { delete m_dummies[i]; m_dummies.RemoveAt(i); }



  /// \brief
  ///   Adds a new instance
  ///
  /// \param i
  ///   Instance to add - instance object will be copied
  inline void                  AddInstance(const VGInstance& i)        { m_instances.Add(new VGInstance(i)); }

  /// \brief
  ///   Creates a new instance and returns a reference to it (same as implicitly adding a new instance)
  ///
  /// \param i
  ///   Instantiable object to create instance for
  ///
  /// \return
  ///   Reference to newly added instance
  inline VGInstance&           CreateInstance(const VGInstantiable& i) { m_instances.Add(new VGInstance(i)); return GetInstance(GetNumInstances()-1); }

  /// \brief
  ///   Returns number of instances
  ///
  /// \return
  ///   Number of instances
  inline int                   GetNumInstances() const                 { return m_instances.GetSize(); }

  /// \brief
  ///   Returns a reference to an instance
  ///
  /// \param i
  ///   instance index
  ///
  /// \return
  ///   Instance
  inline       VGInstance&     GetInstance(int i)                      { return *m_instances[i]; }
  inline const VGInstance&     GetInstance(int i) const                { return *m_instances[i]; }

  /// \brief
  ///   Removes an instance
  ///
  /// \param i
  ///   Instance index
  inline void                  RemoveInstance(int i)                   { delete m_instances[i]; m_instances.RemoveAt(i); }

  /// \brief
  ///   Removes all instances
  inline void                  RemoveAllInstances()                    { int n=GetNumInstances(); for(int i=0; i<n; ++i) delete m_instances[i]; m_instances.RemoveAll(); }

  /// \brief
  ///   Turns all instances into concrete objects and "moves" them to the correct containers
  ///   of the scene
  ///
  /// This function loops over all instances in this scene, and turns them into clones of the
  /// object they are referring to, then setting the instance's transformation on the latter.
  /// If an instance refers to an instantiable that can't be found in this scene, it is ignored.
  /// It is a good idea to check the number of instances of the scene after calling this
  /// function - if it is not 0, there are instances referring to objects that are not part of
  /// the scene. This is most likely an error.
  void                         BakeInstances();



  /// \brief
  ///   Adds a new material
  ///
  /// \param m
  ///   Material to add - will be copied
  inline void                  AddMaterial(const VGMaterial& m)        { m_materials.Add(new VGMaterial(m)); }

  /// \brief
  ///   Creates a new material and returns a reference to it (same as implicitly adding a new material)
  ///
  /// \return
  ///   Reference to newly added material
  inline VGMaterial&           CreateMaterial()                        { m_materials.Add(new VGMaterial()); return GetMaterial(GetNumMaterials()-1); }

  /// \brief
  ///   Returns number of materials
  ///
  /// \return
  ///   Number of materials
  inline int                   GetNumMaterials() const                 { return m_materials.GetSize(); }

  /// \brief
  ///   Returns a reference to a material
  ///
  /// \param i
  ///   Material index
  ///
  /// \return
  ///   Material
  inline       VGMaterial&     GetMaterial(int i)                      { return *m_materials[i]; }
  inline const VGMaterial&     GetMaterial(int i) const                { return *m_materials[i]; }

  /// \brief
  ///   Removes a material (handle with care, b/c removing a material will change the indices of other materials, of course)
  ///
  /// \param i
  ///   Material index
  inline void                  RemoveMaterial(int i)                   { delete m_materials[i]; m_materials.RemoveAt(i); }



  /// \brief
  ///   Adds a new geometry info
  ///
  /// \param g
  ///   Geometry info to add - will be copied
  inline void                  AddGeometryInfo(const VGGeometryInfo& g){ m_geometryInfos.Add(new VGGeometryInfo(g)); }

  /// \brief
  ///   Adds a new geometry info uniquely
  ///
  /// \param g
  ///   Geometry info to add - will be copied
  ///
  /// \return
  ///   Index of newly added geometry info
  int                          AddGeometryInfo_Unique(const VGGeometryInfo& g);

  /// \brief
  ///   Creates a new geometry info and returns a reference to it (same as implicitly adding a new geometry info)
  ///
  /// \return
  ///   Reference to newly added geometry info
  inline VGGeometryInfo&       CreateGeometryInfo()                    { m_geometryInfos.Add(new VGGeometryInfo()); return GetGeometryInfo(GetNumGeometryInfos()-1); }

  /// \brief
  ///   Returns number of geometry infos
  ///
  /// \return
  ///   Number of geometry infos
  inline int                   GetNumGeometryInfos() const             { return m_geometryInfos.GetSize(); }

  /// \brief
  ///   Returns a reference to a geometry info
  ///
  /// \param i
  ///   Geometry info index
  ///
  /// \return
  ///   Geometry info
  inline       VGGeometryInfo& GetGeometryInfo(int i)                  { return *m_geometryInfos[i]; }
  inline const VGGeometryInfo& GetGeometryInfo(int i) const            { return *m_geometryInfos[i]; }

  /// \brief
  ///   Removes a geometry info (handle with care, b/c removing a geometry info will change the indices of other geometry infos, of course)
  ///
  /// \param i
  ///   Geometry info index
  inline void                  RemoveGeometryInfo(int i)               { delete m_geometryInfos[i]; m_geometryInfos.RemoveAt(i); }



  /// \brief
  ///   Adds a new physics info
  ///
  /// \param p
  ///   Physics info to add - will be copied
  inline void                  AddPhysicsInfo(const VGPhysicsInfo& p)  { m_physicsInfos.Add(new VGPhysicsInfo(p)); }

  /// \brief
  ///   Adds a new physics info uniquely
  ///
  /// \param p
  ///   Physics info to add - will be copied
  ///
  /// \return
  ///   Index of newly added physics info
  int                          AddPhysicsInfo_Unique(const VGPhysicsInfo& p);

  /// \brief
  ///   Creates a new physics info and returns a reference to it (same as implicitly adding a new physics info)
  ///
  /// \return
  ///   Reference to newly added physics info
  inline VGPhysicsInfo&        CreatePhysicsInfo()                     { m_physicsInfos.Add(new VGPhysicsInfo()); return GetPhysicsInfo(GetNumPhysicsInfos()-1); }

  /// \brief
  ///   Returns number of physics infos
  ///
  /// \return
  ///   Number of physics infos
  inline int                   GetNumPhysicsInfos() const              { return m_physicsInfos.GetSize(); }

  /// \brief
  ///   Returns a reference to a physics info
  ///
  /// \param i
  ///   Physics info index
  ///
  /// \return
  ///   Physics info
  inline       VGPhysicsInfo&  GetPhysicsInfo(int i)                   { return *m_physicsInfos[i]; }
  inline const VGPhysicsInfo&  GetPhysicsInfo(int i) const             { return *m_physicsInfos[i]; }

  /// \brief
  ///   Removes a physics info (handle with care, b/c removing a physics info will change the indices of other physics infos, of course)
  ///
  /// \param i
  ///   Physics info index
  inline void                  RemovePhysicsInfo(int i)                { delete m_physicsInfos[i]; m_physicsInfos.RemoveAt(i); }



  /// \brief
  ///   Adds a new visibility portal
  ///
  /// \param z
  ///   Visibility portal to add - will be copied
  inline void                  AddVisibilityPortal(const VGPortal& z)  { m_visPortals.Add(new VGPortal(z)); }

  /// \brief
  ///   Creates a new visibility portal and returns a reference to it (same as implicitly adding a new visibility portal)
  ///
  /// \return
  ///   Reference to newly added visibility portal
  inline VGPortal&             CreateVisibilityPortal()                { m_visPortals.Add(new VGPortal()); return GetVisibilityPortal(GetNumVisibilityPortals()-1); }

  /// \brief
  ///   Returns number of visibility portals
  ///
  /// \return
  ///   Number of visibility portals
  inline int                   GetNumVisibilityPortals() const         { return m_visPortals.GetSize(); }

  /// \brief
  ///   Returns a reference to a visibility portal
  ///
  /// \param i
  ///   Visibility portal index
  ///
  /// \return
  ///   Visibility portal
  inline       VGPortal&       GetVisibilityPortal(int i)              { return *m_visPortals[i]; }
  inline const VGPortal&       GetVisibilityPortal(int i) const        { return *m_visPortals[i]; }

  /// \brief
  ///   Removes a visibility portal
  ///
  /// \param i
  ///   Visibility portal index
  inline void                  RemoveVisibilityPortal(int i)           { delete m_visPortals[i]; m_visPortals.RemoveAt(i); }



  /// \brief
  ///   Adds a new light grid
  ///
  /// \param l
  ///   Light grid to add - will be copied
  inline void                  AddLightGrid(const VGLightGrid& l)      { m_lightGrids.Add(new VGLightGrid(l)); }

  /// \brief
  ///   Creates a new light grid and returns a reference to it (same as implicitly adding a new light grid)
  ///
  /// \return
  ///   Reference to newly added light grid
  inline VGLightGrid&          CreateLightGrid()                       { m_lightGrids.Add(new VGLightGrid()); return GetLightGrid(GetNumLightGrids()-1); }

  /// \brief
  ///   Returns number of light grids
  ///
  /// \return
  ///   Number of light grids
  inline int                   GetNumLightGrids() const                { return m_lightGrids.GetSize(); }

  /// \brief
  ///   Returns a reference to a light grid
  ///
  /// \param i
  ///   Light grid index
  ///
  /// \return
  ///   Light grid
  inline       VGLightGrid&    GetLightGrid(int i)                     { return *m_lightGrids[i]; }
  inline const VGLightGrid&    GetLightGrid(int i) const               { return *m_lightGrids[i]; }

  /// \brief
  ///   Removes a light grid
  ///
  /// \param i
  ///   Light grid index
  inline void                  RemoveLightGrid(int i)                  { delete m_lightGrids[i]; m_lightGrids.RemoveAt(i); }



  /// \brief
  ///   Adds a new path
  ///
  /// \param p
  ///   Path to add - will be copied
  inline void                  AddPath(const VGPath& p)                { m_paths.Add(new VGPath(p)); }

  /// \brief
  ///   Creates a new path and returns a reference to it (same as implicitly adding a new path)
  ///
  /// \return
  ///   Reference to newly added path
  inline VGPath&               CreatePath()                            { m_paths.Add(new VGPath()); return GetPath(GetNumPaths()-1); }

  /// \brief
  ///   Returns number of paths
  ///
  /// \return
  ///   Number of paths
  inline int                   GetNumPaths() const                     { return m_paths.GetSize(); }

  /// \brief
  ///   Returns a reference to a path
  ///
  /// \param i
  ///   Path index
  ///
  /// \return
  ///   Path
  inline       VGPath&         GetPath(int i)                          { return *m_paths[i]; }
  inline const VGPath&         GetPath(int i) const                    { return *m_paths[i]; }

  /// \brief
  ///   Removes a path
  ///
  /// \param i
  ///   Path index
  inline void                  RemovePath(int i)                       { delete m_paths[i]; m_paths.RemoveAt(i); }



  /// \brief
  ///   Return a reference to a skeleton
  ///
  /// \return
  ///   Skeleton
  inline       VGSkeleton&     GetSkeleton()                           { return m_skeleton; }
  inline const VGSkeleton&     GetSkeleton() const                     { return m_skeleton; }

  /// \brief
  ///   Prunes the scene's skeleton (removing leaf bones not influencing any vertices)
  ///
  /// \param mask
  ///   Mask for bone subset to work on (bones not matching mask won't be checked for removal)
  void                         PruneSkeleton(VGBone::BoneMask mask = VGBone::FULL_BONE_MASK);



  /// \brief
  ///   Generates a bounding box encompassing all mesh data in the scene, taking into account
  ///   mesh transformations - note that this will compute the unanimated bounding box, neither
  ///   vertex nor bone animations will influence the result
  ///
  /// \param bbox
  ///   Bounding box to be generated/filled in
  ///
  /// \return
  ///   Returns false if there is no mesh data in the scene
  bool                         ComputeBBox(hkvAlignedBBox& bbox) const; // @@@ test



  /// \brief
  ///   Clears the scene
  void                         Reset();



  /// \brief
  ///   Assignment operator - makes a deep copy
  ///
  /// \param rhs
  ///   Scene to assign
  ///
  /// \return
  ///   Reference to this scene
  VGScene& operator=(const VGScene& rhs);



private:
  // Scene data.
  VArray<VGMesh*,         VGMesh*        > m_meshes;
  VArray<VGLight*,        VGLight*       > m_lights;
  VArray<VGMeshProxy*,    VGMeshProxy*   > m_meshProxies;
  VArray<VGDummy*,        VGDummy*       > m_dummies;
  VArray<VGInstance*,     VGInstance*    > m_instances;
  VArray<VGMaterial*,     VGMaterial*    > m_materials;
  VArray<VGGeometryInfo*, VGGeometryInfo*> m_geometryInfos;
  VArray<VGPhysicsInfo*,  VGPhysicsInfo* > m_physicsInfos;
  VArray<VGPortal*,       VGPortal*      > m_visPortals;
  VArray<VGLightGrid*,    VGLightGrid*   > m_lightGrids;
  VArray<VGPath*,         VGPath*        > m_paths;

  VGSkeleton m_skeleton;
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
