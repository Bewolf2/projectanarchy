/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Callback interface that can be hooked into mesh exports to write arbitrary output vertex formats. See VGProcessor_VisionExporter::SetCustomVertexFormatter
///
/// This interface allows maximum flexibility to write any vertex buffer format. That means however that it is
/// not guaranteed that standard engine shaders do work properly with this format (e.g. if normal vectors are packed differently).
/// Since this formatter does not associate any semantic with the raw output data, those custom files can typically not be read by vGeom2. The engine treats
/// this information as raw vertex buffers.
class IVGVertexFormatter
{
public:
  /// \brief
  ///   This function is called once at the beginning of the export process to retrieve the output vertex format
  virtual void GetOutputFormat(VisMBVertexDescriptor_t &desc) = 0;

  /// \brief
  ///   This function is called for every vertex in the list. Vertex data must be converted to target format and written to the out stream. The amount of bytes written must match the m_iStride member filled out in GetVertexFormat
  virtual void ConvertToOutputFormat(const VGVertexList &list, int iVertex, IVFileOutStream *pOut) = 0;
};

/// \brief
///   .vmesh, .model, .anim, .vcolmesh reader/writer helper class
///
/// Helper class used by reader/writer processors.
class VGEOM2_IMPEXP_CLASS VGVisionImporterExporter
{
public:
  enum DataFormat // Values can be used as a mask, although not intended when used with VGProcessor_Vision??porter::SetDataFormat() functions. DON'T change the values.
  {
    VDF_MESH            = V_BIT(0),
    VDF_MODEL           = V_BIT(1),             // Usually like a mesh but with bones, etc. (vision slang).
    VDF_COLLISION_MESH  = V_BIT(2),
    VDF_ANIM            = V_BIT(3),
    VDF_PREFAB          = V_BIT(4),
    VDF_VISIBILITY_INFO = V_BIT(5),
    VDF_MATERIAL        = V_BIT(6),
    VDF_MATERIAL_LIB    = V_BIT(7),
    VDF_PREFAB_LAYER    = V_BIT(8),

    VDFMASK_FULL = VDF_MESH|VDF_MODEL|VDF_COLLISION_MESH|VDF_ANIM|VDF_PREFAB|VDF_VISIBILITY_INFO|VDF_MATERIAL|VDF_MATERIAL_LIB|VDF_PREFAB_LAYER
  };

  enum CollisionType
  {
    VCT_NONE   = 0,  // Mesh/model should not be used as a collider.
    VCT_EXTERN = 1,  // Mesh/model's collision mesh should be loaded from an external data source (usually a .vcolmesh file).
    VCT_INTERN = 2   // Mesh/model's collision mesh is identical with render mesh.
  };


public:
  VGVisionImporterExporter(IVLog* log = NULL) : m_log(log)  { }
  ~VGVisionImporterExporter()                               { }



  /// \brief
  ///   Writes scene data to a stream
  ///
  /// \param scene
  ///   Mesh containing data to write
  ///
  /// \param ve
  ///   Export processor in use
  ///
  /// \param stream
  ///   Stream to write data to
  ///
  /// \return
  ///   False if writing data failed
  bool  Write(const VGScene& scene, const class VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;

  /// \brief
  ///   Reads scene data from a stream and _adds_ it to passed-in scene
  ///
  /// \param scene
  ///   Mesh that will be filled with read data
  ///
  /// \param vi
  ///   Import processor in use
  ///
  /// \param stream
  ///   Stream to read data from
  ///
  /// \return
  ///   False if reading data failed
  bool  Read(VGScene& scene, const class VGProcessor_VisionImporter* vi, IVFileInStream& stream);




  // Vision specific helper functions.


  /// \brief
  ///   Returns backend-specific path of default diffuse map
  ///
  /// \return
  ///   Vision specific path of default diffuse map, relative to data directory
  static inline const char* GetDefaultDiffuseMapPath() throw()                { return "\\PlainWhite.DDS"; }

  /// \brief
  ///   Returns backend-specific path of default specular map
  ///
  /// \return
  ///   Vision specific path of default specular map, relative to data directory
  static inline const char* GetDefaultSpecularMapPath() throw()               { return "\\PlainWhite.DDS"; }

  /// \brief
  ///   Returns backend-specific path of default normal map
  ///
  /// \return
  ///   Vision specific path of default normal map, relative to data directory
  static inline const char* GetDefaultNormalMapPath() throw()                 { return "\\NoNormals.dds"; }



private:
  IVLog* m_log;

  // Writer functions.
  bool (VGVisionImporterExporter::*GetWriter(DataFormat df) const)(const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteVMesh         (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteModel         (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteVColMesh      (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteAnim          (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WritePrefab        (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WritePrefabLayer   (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteVisibilityInfo(const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteMaterial      (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;
  bool WriteMaterialLib   (const VGScene& scene, const VGProcessor_VisionExporter* ve, IVFileOutStream& stream) const;

  // Reader functions.
  bool (VGVisionImporterExporter::*GetReader(DataFormat df) const)(VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadVMesh         (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadModel         (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadVColMesh      (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadAnim          (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadPrefab        (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadPrefabLayer   (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadVisibilityInfo(VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadMaterial      (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
  bool ReadMaterialLib   (VGScene& scene, const VGProcessor_VisionImporter* vi, IVFileInStream& stream);
};




/// \brief
///   Vision file import processor.
///
/// Read processor capable of reading .vmesh and related files (.vcolmesh, .anim, etc.).
/// This Vision-specific processor uses a couple of conventions on how to interprete
/// the incoming data:
///  - when loading animations into a scene, they get applied to a mesh if, and only if,
///    there is a single mesh only (to avoid ambiguity) - otherwise, a new empty mesh
///    gets created in order to store the skeleton and animations
///  - since vGeom2 allows programmers to specify events on a per bone level, which is
///    not possible in .anim files that store events per animation for the entire
///    skeleton, importing a .anim file containing events will result in the animations'
///    events added to the skeleton's root bone
///  - @@@ add comments in code marked with @@@
class VGEOM2_IMPEXP_CLASS VGProcessor_VisionImporter : public VGDynamicProcessor<VGProcessor_VisionImporter>
{
public:
  /// \brief
  ///   Callback base class for reading unknown chunks.
  class IChunkCallback
  {
  public:
    virtual ~IChunkCallback() { }

    /// \brief
    ///   Callback handler for reading - use 'in' to simply read you custom data. The chunk
    ///   specified by 'chunkID' is already open, no need to close the chunk, either.
    ///   Return true if all the data has been read, otherwise false and caller will skip
    ///   the chunk.
    virtual bool Handle(CHUNKIDTYPE chunkID, VChunkFile& in, VGScene& scene) = 0;
  };


public:
  VGProcessor_VisionImporter() : m_df(VGVisionImporterExporter::VDF_MESH), m_inStream(NULL), m_chunkCallback(NULL)   { }
  virtual ~VGProcessor_VisionImporter()                                                                              { }


  /// \brief
  ///   Processes a scene - reads data into a scene
  ///
  /// \param scene
  ///   Scene data to be filled in by reader
  ///
  /// \return
  ///   False if import failed
  virtual bool                                Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired format of the data to be read
  ///
  /// \param df
  ///   Data format
  inline void                                 SetDataFormat(VGVisionImporterExporter::DataFormat df) throw()  { m_df = df; }

  /// \brief
  ///   Gets the format of the data to be read
  ///
  /// \return
  ///   Data format
  inline VGVisionImporterExporter::DataFormat GetDataFormat() const throw()                                   { return m_df; }



  /// \brief
  ///   Sets the input stream to be used
  ///
  /// \param is
  ///   Input stream (make sure pointee outlives instance of this class)
  inline void                                 SetInStream(IVFileInStream* is) throw()                         { m_inStream = is; }

  /// \brief
  ///   Gets the input stream in use
  ///
  /// \return
  ///   Input stream in use
  inline IVFileInStream*                      GetInStream() const throw()                                     { return m_inStream; }



  /// \brief
  ///   Sets up optional callback to read chunks unknown to this reader (e.g. custom user chunks, etc.); only used for
  ///   VChunkFile-based file formats
  ///
  /// \param cb
  ///   Pointer to callback object (make sure pointee outlives instance of this class)
  inline void                                 SetUnknownChunkReaderCallback(IChunkCallback* cb) throw()       { m_chunkCallback = cb; }

  /// \brief
  ///   Gets the input stream in use
  ///
  /// \return
  ///   Pointer to callback object in use
  inline IChunkCallback*                      GetUnknownChunkReaderCallback() const throw()                   { return m_chunkCallback; }



private:
  VGVisionImporterExporter::DataFormat m_df;
  IVFileInStream*                      m_inStream;
  IChunkCallback*                      m_chunkCallback;
};




/// \brief
///   Vision file export processor.
///
/// Write processor capable of writing .vmesh and related files (.vcolmesh, .anim, etc.).
/// This Vision-specific processor uses a couple of conventions on how to interprete
/// the incoming data:
///  - when writing a Vision static mesh or Vision model, the vertex list must contain at
///    least vertex position data
///  - data will be written such that the model's transformation will be applied to the
///    vertices
///  - motion delta data gets extracted from the skeleton's trajectory bone (which is
///    animated the same way other bones are)
///  - some scene/mesh/... data gets discarded, depending on the model to be written,
///    namely:
///     * transformations won't be taken into account when writing meshes or models
///     * object instances will only be written to prefabs, not to meshes or models (but
///       one can "bake" instances, if writing meshes or models)
///  - prefab export path names to meshes are queried using VGInstantiable::GetPathName,
///    so make sure to fill this data in or use a processor which does it for you - this
///    decouples prefab writing from mesh writing
///  - analog to the event loading mentioned above, exporting an animation with events
///    on different bones will "flatten out" all the events and store them in the .anim
///    file per animation
///  - the user can choose freely what time units to use for animation keys, however,
///    this processor will use seconds as default unit
///  - the user can choose freely what space to use for animation transformations,
///    however, this processors will always interpret transformations as parent space
///    ones (scene/global space for root bones)
///  - animations that start at times t<0, will be offset during export so that the
///    first key is at time t=0
///  - visibility zone indices can be assigned freely, however, they'll be changed on
///    export to not be negative
///  - the space visibility zones and their portals' shapes are in is parent space
///    (equals world space in our case, b/c there is no zone/portal hierarchy)
///  - providing custom collision mesh data is possible, however, will only be used if
///    both, vertex list and triangle list contain data
///  - custom bounding boxes as set by VGVertexList::SetCustomBoundingBox are
///    interpreted as collision bounding boxes by Vision file format specific processors
///  - texture coordinate sets 0 and 1 are used by the vision import/export processor as
///    diffuse and lightmap channel, respectively
///
/// For more information and conventions, have a look at the official vGeom2
/// documentation.
///@@@ add comments in code marked with (to doc, too)@@@
class VGEOM2_IMPEXP_CLASS VGProcessor_VisionExporter : public VGDynamicProcessor<VGProcessor_VisionExporter>
{
public:
  /// \brief
  ///   Callback base class for writing custom chunks.
  class IChunkCallback
  {
  public:
    virtual ~IChunkCallback() { }

    /// \brief
    ///   Callback handler for writing - use 'out' inside the function as follows:
    ///     out.StartChunk('XXXX'); // Start custom chunk (replace XXXX with your ID)
    ///     out.Write....           // Use Write* functions to write chunk data.
    ///     out.EndChunk();         // Close chunk.
    ///     ...                     // Repeat for more custom chunks.
    ///   Return true if chunk got written successfully.
    virtual bool Handle(VChunkFile& out, const VGScene& scene) = 0;
  };


public:
  VGProcessor_VisionExporter();
  virtual ~VGProcessor_VisionExporter()   { }


  /// \brief
  ///   Output formats for vertex components.
  enum VertexComponentFormat
  {
    CF_HALF,        // IEEE 754-2008 binary16
    CF_FLOAT,       // IEEE 754-2008 binary32
//@@@ extend    CF_CHAR,        // Byte
    CF_UCHAR,       // Unsigned byte
//@@@ extend    CF_SHORT,       // Short
//@@@ extend    CF_UCHAR_NORM,  // Normalized unsigned char
//@@@ extend    CF_USHORT_NORM  // Normalized unsigned short
  };



  /// \brief
  ///   Processes a scene - writes scene data
  ///
  /// \param scene
  ///   Scene data to be written
  ///
  /// \return
  ///   False if export failed
  virtual bool                                    Process(VGScene& scene) const;



  /// \brief
  ///   Sets the desired format of the data to be written
  ///
  /// \param df
  ///   Data format
  inline void                                     SetDataFormat(VGVisionImporterExporter::DataFormat df) throw()                          { m_df = df; }

  /// \brief
  ///   Gets the format of the data to be written
  ///
  /// \return
  ///   Data format
  inline VGVisionImporterExporter::DataFormat     GetDataFormat() const throw()                                                           { return m_df; }



  /// \brief
  ///   Sets the vertex data to be excluded when writing the mesh.
  ///   All output vertices must always have a position. Trying to exclude
  ///   position data in this function will be ignored.
  ///
  /// \param exMask
  ///   Bitwise OR of the values that should not appear in the vertices of the output mesh
  ///   (ie. VGVertex::VGVM_TEXCOORD_01|VGVertex::VGVM_TEXCOORD_02).
  ///   
  inline void                                     SetExcludeVertexInfo(VGVertex::VertexMask exMask) throw()                               { m_vertexMask = (VGVertex::VertexMask)(exMask&~VGVertex::VGVM_POSITION); }

  /// \brief
  ///   Gets the vertex data to be excluded when writing the mesh.
  ///
  /// \return
  ///   Bitwise OR of the values that should not appear in the vertices of the output mesh.
  inline VGVertex::VertexMask                     GetExcludeVertexInfo() const throw()                                                    { return m_vertexMask; }



  /// \brief
  ///   Sets the desired collision type to be used for exported data (defaults to VCT_INTERN)
  ///
  /// \param ct
  ///   Collision type
  inline void                                     SetCollisionType(VGVisionImporterExporter::CollisionType ct) throw()                    { m_ct = ct; }

  /// \brief
  ///   Gets the collision type to be used for scene data to be written (default is VCT_INTERN)
  ///
  /// \return
  ///   Collision type
  inline VGVisionImporterExporter::CollisionType  GetCollisionType() const throw()                                                        { return m_ct; }



  /// \brief
  ///   Enables/disables motion delta extraction and sets options
  ///
  /// \param n
  ///   Animation name to set motion delta options for
  ///
  /// \param b
  ///   Enables/disables motion delta extraction
  ///
  /// \param odtaAxes
  ///   Axes bits to grab offset delta from (1<<2=x, 1<<1=y, 1<<0=z so 07 is all axes, 05 is x and z, etc.)
  ///
  /// \param rdtaAxis
  ///   Axis for rotation delta extraction (engine allows only one) - 0=none, 1=x, 2=y, 3=z
  inline void                                     SetMotionDeltaExtraction(const VString& n, bool b, int odtaAxes=07, int rdtaAxis=0)     { m_mdtaInfo[n] = (void*)(((b?1:0)<<5)|((rdtaAxis&3)<<3)|(odtaAxes&7)); }//@@@ test

  /// \brief
  ///   Gets options set for motion delta extraction
  ///
  /// \param n
  ///   Animation name to get motion delta options for
  ///
  /// \param b
  ///   Enable/disable motion delta extraction flag
  ///
  /// \param odtaAxes
  ///   Axes bits to grab offset delta from (1<<2=x, 1<<1=y, 1<<0=z so 07 is all axes, 05 is x and z, etc.)
  ///
  /// \param rdtaAxis
  ///   Axis for rotation delta extraction (engine allows only one) - 0=none, 1=x, 2=y, 3=z
  inline void                                     GetMotionDeltaExtraction(const VString& n, bool& b, int& odtaAxes, int& rdtaAxis) const { intptr_t p=0; m_mdtaInfo.Lookup(n, (void*&)p); b = !!(p&(1<<5)); rdtaAxis = (p>>3)&3; odtaAxes = p&7; }//@@@ test



  /// \brief
  ///   Sets whether or not a shadow mesh should be written or not (ignored by datatypes that don't write shadow meshes, anyways, e.g. anims). DEPRECATED - flag not used anymore
  ///
  /// \param writeShadowMesh
  ///   Flag
  inline void                                     SetWriteShadowMesh(bool writeShadowMesh) throw()                                        { m_writeShadowMesh = writeShadowMesh; }

  /// \brief
  ///   Gets whether or not a shadow mesh should be written or not. DEPRECATED - flag not used anymore
  ///
  /// \return
  ///   Flag
  inline bool                                     GetWriteShadowMesh() const throw()                                                      { return m_writeShadowMesh; }



  /// \brief
  ///   Sets the desired output data format of a vertex component. Ignored if a custom vertex formatter is specified via SetCustomVertexFormatter
  ///
  /// \param c
  ///   Component
  ///
  /// \param f
  ///   Format
  ///
  /// \return
  ///    False if format for component is read-only (some component formats are not allowed to be changed).
  bool                                            SetComponentDataFormat(VGVertex::VertexMask c, VertexComponentFormat f);

  /// \brief
  ///   Gets the currentlich set output data format of a vertex component
  ///
  /// \param c
  ///   Component
  ///
  /// \return
  ///   Format
  VertexComponentFormat                           GetComponentDataFormat(VGVertex::VertexMask c);

  /// \brief
  ///   Gets the currentlich set output data formats for all vertex components
  ///
  /// \return
  ///   Format
  inline const VMapIntToInt&                      GetComponentDataFormats() const throw()                                                 { return m_componentFormats; }



  /// \brief
  ///   Sets the upper shadow mesh vertex index limit (influences export - defaults to 32767)
  ///
  /// \param l
  ///   Highest possible shadow mesh vertex index
  inline void                                     SetMaximumShadowMeshVertexIndex(int l) throw()                                          { m_maxSMVertexIndex = l; }

  /// \brief
  ///   Gets the currently set upper shadow mesh vertex index limit (see setter)
  ///
  /// \return
  ///   Currently set highest possible shadow mesh vertex index
  inline int                                      GetMaximumShadowMeshVertexIndex() const throw()                                         { return m_maxSMVertexIndex; }



  /// \brief
  ///   Sets the output stream to be used
  ///
  /// \param os
  ///   Output stream
  inline void                                     SetOutStream(IVFileOutStream* os) throw()                                               { m_outStream = os; }

  /// \brief
  ///   Gets the output stream in use
  ///
  /// \return
  ///   Output stream in use
  inline IVFileOutStream*                         GetOutStream() const throw()                                                            { return m_outStream; }

  /// \brief
  ///   Sets whether the prefab shapes have been filled in with relative (to the prefab) file paths
  ///
  /// \param bValue
  ///   true or false
  inline void                                     SetPrefabRelativeFilenames(bool bValue)                                                 { m_prefabRelativeFilenames = bValue; }

  /// \brief
  ///   Returns whether the prefab shapes have been filled in with relative (to the prefab) file paths
  ///
  /// \return
  ///   true or false
  inline bool                                     GetPrefabRelativeFilenames() const                                                      { return m_prefabRelativeFilenames; }



  /// \brief
  ///   Sets up optional callback to write custom chunks - callbacks will be triggered at end of file format (we might extend this in the
  ///   future; only used for VChunkFile-based file formats
  ///
  /// \param cb
  ///   Pointer to callback object (make sure pointee outlives instance of this class)
  inline void                                     SetCustomChunkWriterCallback(IChunkCallback* cb) throw()                                { m_chunkCallback = cb; }

  /// \brief
  ///   Gets the input stream in use
  ///
  /// \return
  ///   Pointer to callback object in use
  inline IChunkCallback*                          GetCustomChunkWriterCallback() const throw()                                            { return m_chunkCallback; }


  /// \brief
  ///   Sets a custom vertex formatter for this export which allows for any kind of output format. See IVGVertexFormatter interface. 
  ///
  /// If a formatter is specified, the following values are ignored:
  ///   - the formats specified via SetComponentDataFormat.
  ///   - the excluded components specified via SetExcludeVertexInfo
  ///
  /// \param pFormatter
  ///   The new formatter, or NULL to fall back to 'fixed function' formats specified through SetComponentDataFormat
  inline void SetCustomVertexFormatter(IVGVertexFormatter* pFormatter) throw()      {m_customFormatter = pFormatter;}

  /// \brief
  ///   Returns the current vertex formatter, NULL for standard formatting
  inline IVGVertexFormatter* GetCustomVertexFormatter() const throw()               {return m_customFormatter;}


private:
  VGVisionImporterExporter::DataFormat    m_df;
  VGVisionImporterExporter::CollisionType m_ct;
  bool                                    m_writeShadowMesh;
  int                                     m_maxSMVertexIndex;
  IVFileOutStream*                        m_outStream;
  IChunkCallback*                         m_chunkCallback;
  VMapIntToInt                            m_componentFormats;
  IVGVertexFormatter*                     m_customFormatter;
  VGVertex::VertexMask                    m_vertexMask;

  // Motion delta.
  VMapStrToPtr/*used as str->int*/        m_mdtaInfo;
  
  //File settings
  bool                                    m_prefabRelativeFilenames;
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
