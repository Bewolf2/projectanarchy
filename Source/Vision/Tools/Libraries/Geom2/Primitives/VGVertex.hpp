/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Vertex class
///
/// Stores data for a single vertex in a convenient way - not that this class makes working with single
/// vertices pretty easy, whereas it might be unefficient to store a large number of VGVertex instances.
/// Usually, this class helps in setting up vertices and hand the data over to VGVertexlist instances,
/// which store the data more efficiently.
class VGEOM2_IMPEXP_CLASS VGVertex
{
public:
  /// \brief
  ///   Vertex component mask
  ///
  /// Bits to be used to specify which vertex components should be used when working with
  /// vertices. We speak of 'compatible' vertex masks if two or more masks are identical.
  enum VertexMask
  {
    VGVM_NOTHING     =        0 ,
    VGVM_POSITION    = V_BIT( 0),
    VGVM_NORMAL      = V_BIT( 1),
    VGVM_TANGENT     = V_BIT( 2),
    VGVM_WEIGHT      = V_BIT( 3),
    VGVM_COLOR_1     = V_BIT( 4),
    VGVM_COLOR_2     = V_BIT( 5),
    VGVM_TEXCOORD_01 = V_BIT( 8), // Used by Vision import/export processor as diffuse channel.
    VGVM_TEXCOORD_02 = V_BIT( 9), // Used by Vision import/export processor as lightmap channel.
    VGVM_TEXCOORD_03 = V_BIT(10),
    VGVM_TEXCOORD_04 = V_BIT(11),
    VGVM_TEXCOORD_05 = V_BIT(12),
    VGVM_TEXCOORD_06 = V_BIT(13),
    VGVM_TEXCOORD_07 = V_BIT(14),
    VGVM_TEXCOORD_08 = V_BIT(15)
  };

  static const int MAX_NUM_COLORS    = 2;
  static const int MAX_NUM_TEXCOORDS = 8;
  static const int MAX_NUM_WEIGHTS   = 4;

  static const VertexMask FULL_VERTEX_MASK =
    VertexMask(VGVM_POSITION|VGVM_NORMAL|VGVM_TANGENT|VGVM_WEIGHT|VGVM_COLOR_1|VGVM_COLOR_2|
               VGVM_TEXCOORD_01|VGVM_TEXCOORD_02|VGVM_TEXCOORD_03|VGVM_TEXCOORD_04|
               VGVM_TEXCOORD_05|VGVM_TEXCOORD_06|VGVM_TEXCOORD_07|VGVM_TEXCOORD_08);

  /// \brief
  ///   Weight
  struct Weight
  {
    float               w;  // Weight.
    const class VGBone* p;  // Bone index (should point to bone in scene's skeletons).

    inline Weight& operator=(const Weight& rhs) { w=rhs.w; p=rhs.p; return *this; }
  };

  /// \brief
  ///   Vertex weights
  struct VGEOM2_IMPEXP_CLASS Weights
  {
    Weight weights[MAX_NUM_WEIGHTS];
    int    numWeights;

    void            Sort();
    bool            IsEqual(const Weights& w, float delta) const;
    bool            isEqual(const Weights& w, float delta) const { return IsEqual (w, delta); } // required for interface compatibility with the new math classes
    inline bool     operator==(const Weights& rhs) const { return IsEqual(rhs, 0.f); }
    inline bool     operator!=(const Weights& rhs) const { return !operator==(rhs); }
    inline Weights& operator= (const Weights& rhs) { numWeights=rhs.numWeights; for(int i=0; i<numWeights; ++i) weights[i]=rhs.weights[i]; return *this; }
  };


public:
  VGVertex(VertexMask vm = VertexMask(VGVM_POSITION|VGVM_NORMAL|VGVM_TEXCOORD_01));
  virtual ~VGVertex();



  /// \brief
  ///   Gets the currently set vertex mask
  ///
  /// \return
  ///   Currently set vertex mask
  inline VertexMask       GetVertexMask() const throw()               { return m_vm; }

  /// \brief
  ///   Sets the vertex mask to be used with this vertex
  ///
  /// \param vm
  ///   Vertex mask to set
  inline void             SetVertexMask(VertexMask vm) throw()        { m_vm = VertexMask(vm&VGVertex::FULL_VERTEX_MASK); SetDirty(); }



  /// \brief
  ///   Gets this vertex' position
  ///
  /// \return
  ///   Vertex position
  inline const hkvVec3&   GetPosition() const throw()                 { return m_position; }

  /// \brief
  ///   Sets this vertex' position
  ///
  /// \param p
  ///   Vertex position
  inline void             SetPosition(const hkvVec3& p)               { m_position = p; SetDirty(); }



  /// \brief
  ///   Gets this vertex' normal
  ///
  /// \return
  ///   Vertex normal
  inline const hkvVec3&   GetNormal() const throw()                   { return m_normal; }

  /// \brief
  ///   Sets this vertex' normal
  ///
  /// \param n
  ///   Vertex normal
  inline void             SetNormal(const hkvVec3& n)                 { m_normal = n; SetDirty(); }



  /// \brief
  ///   Gets this vertex' tangent
  ///
  /// \return
  ///   Vertex tangent - note that if tangent's w < 0, tangent space is considered to be right-handed
  inline const hkvVec4&   GetTangent() const throw()                  { return m_tangent; }

  /// \brief
  ///   Sets this vertex' tangent
  ///
  /// \param t
  ///   Vertex tangent - note that if tangent's w < 0, tangent space is considered to be right-handed
  inline void             SetTangent(const hkvVec4& t)                { m_tangent = t; SetDirty(); }



  /// \brief
  ///   Gets this vertex' texture coordinates
  ///
  /// \param index
  ///   Texture coordinate set index
  ///
  /// \return
  ///   Vertex texture coordinates
  inline const hkvVec2&   GetTexCoord(int index) const                { VASSERT(index>=0 && index<MAX_NUM_TEXCOORDS); return m_texCoords[index]; }

  /// \brief
  ///   Sets this vertex' texture coordinates
  ///
  /// \param index
  ///   Texture coordinate set index
  ///
  /// \param tc
  ///   Vertex texture coordinates
  inline void             SetTexCoord(int index, const hkvVec2& tc)   { VASSERT(index>=0 && index<MAX_NUM_TEXCOORDS); m_texCoords[index] = tc; SetDirty(); }



  /// \brief
  ///   Gets this vertex' colors
  ///
  /// \param index
  ///   Vertex color set index
  ///
  /// \return
  ///   Vertex color
  inline const hkvVec4&  GetColor(int index) const                   { VASSERT(index>=0 && index<MAX_NUM_COLORS); return m_colors[index]; }

  /// \brief
  ///   Sets this vertex' colors
  ///
  /// \param index
  ///   Vertex color set index
  ///
  /// \param c
  ///   Vertex color
  inline void             SetColor(int index, const hkvVec4& c)      { VASSERT(index>=0 && index<MAX_NUM_COLORS); m_colors[index] = c; SetDirty(); }



  /// \brief
  ///   Gets this vertex' weights
  ///
  /// \return
  ///   Vertex weights
  inline const Weights&   GetWeights() const throw()                  { return m_weights; }

  /// \brief
  ///   Sets this vertex' weights
  ///
  /// \param w
  ///   Vertex weights
  inline void             SetWeights(const Weights& w)                { VASSERT(w.numWeights >= 0 && w.numWeights <= 4); m_weights = w; SetDirty(); } // Assert if w.numWeights is not in 0-4 range.

  /// \brief
  ///   Gets number of weights (bone influences) on this vertex
  ///
  /// \return
  ///   Number of vertex weights
  inline int              GetNumWeights() const throw()               { return m_weights.numWeights; }

  /// \brief
  ///   Sets number of bone influences on this vertex
  ///
  /// \param n
  ///   Number of vertex weights
  inline void             SetNumWeights(int n) throw()                { m_weights.numWeights = n; SetDirty(); }

  /// \brief
  ///   Normalizes currently set weights
  inline void             NormalizeWeights() throw()                  { int n=m_weights.numWeights, i; Weight* w=m_weights.weights; float f=0.f; for(i=0;i<n;++i) f+=w[i].w; f=1.f/f; for(i=0;i<n;++i) w[i].w*=f; SetDirty(); }  // @@@ test

  /// \brief
  ///   Gets a vertex weight
  ///
  /// \param index
  ///   Weight index
  ///
  /// \return
  ///   Vertex position
  inline const Weight&    GetWeight(int index) const                  { VASSERT(index>=0 && index<MAX_NUM_WEIGHTS); return m_weights.weights[index]; }

  /// \brief
  ///   Sets a vertex weight
  ///
  /// \param index
  ///   Weight index
  ///
  /// \param w
  ///   Vertex position
  inline void             SetWeight(int index, const Weight& w)       { VASSERT(index>=0 && index<MAX_NUM_WEIGHTS); m_weights.weights[index] = w; SetDirty(); }



  /// \brief
  ///   Interpolate the values of this vertex with another vertex and write results to this vertex
  ///
  /// \param t
  ///   Interpolation factor (must be between 0 and 1 - if not it will get clamped)
  ///
  /// \param other
  ///   Vertex to interpolate with
  void                    InterpolateValues(float t, const VGVertex& other);

  /// \brief
  ///   Transform the values of this vertex (in this case, positions, normals and tangents are affected)
  ///
  /// \param mat
  ///   Transformation matrix
  void                    Transform(const hkvMat4& mat);



  /// \brief
  ///   Sets default vertex values
  void                    SetDefaults();

  /// \brief
  ///   Sets vertex data from other vertex as outlined by a vertex mask
  ///
  /// \param v
  ///   Vertex to copy data from
  ///
  /// \param vm
  ///   Vertex mask to be used in order to determine which data to copy from other vertex
  void                    SetFrom(const VGVertex& v, VertexMask vm);

  /// \brief
  ///   Returns a vertex that is compatible to the specified vertex mask
  ///
  /// \param vm
  ///   Vertex mask to be used in order to determine which data to copy
  ///
  /// \return
  ///   Vertex compatible to specified vertex mask
  VGVertex                GetCompatibleVertex(VertexMask vm) const;



  /// \brief
  ///   Sorts a vertex' weights by descending weight value
  void                    SortWeights();



  /// \brief
  ///   Returns a hash value for this vertex
  ///
  /// \return
  ///   Vertex hash
  int                     GetHash() const;



  /// \brief
  ///   Compares two vertices for equality
  ///
  /// \param rhs
  ///   Right hand side
  ///
  /// \return
  ///   True if vertices are equal, otherwise false - note: depends on vertex mask (only
  ///   vertex components included by the vertex mask are compared)
  bool                    operator==(const VGVertex& rhs) const;

  /// \brief
  ///   Compares two vertices for inequality
  ///
  /// \param rhs
  ///   Right hand side
  ///
  /// \return
  ///   False if vertices are equal, otherwise true - see operator== for details
  inline bool             operator!=(const VGVertex& rhs) const       { return !operator==(rhs); }



private:
  // Helpers to facilitate type and data handling via a bitmask bit.
  template<int   i> struct BitMaskType;
  template<int   i> void SetData(const typename BitMaskType<i>::t&);
  template<int   i> void SetData(const typename BitMaskType<i>::t& v, VertexMask vm) { if(i & vm) SetData<i>(v); }
  template<int   i> typename const BitMaskType<i>::t& GetData() const;

  template<class T> static int ComputeHash(const T&);
  template<int   i> int        ComputeHash() const { return (i & m_vm) ? ComputeHash(GetData<i>()) : 0; }

  template<int   i> inline bool Compare(const VGVertex& v, float delta) const { return !(i & m_vm) || (GetData<i>().isEqual(v.GetData<i>(), delta)); }

  inline void SetDirty(bool b=true) throw()  { m_dirty = b; }

private:
  // Storage for all different types
  VertexMask m_vm;
  hkvVec3    m_position;
  hkvVec3    m_normal;
  hkvVec4    m_tangent;
  hkvVec2    m_texCoords[MAX_NUM_TEXCOORDS];
  hkvVec4   m_colors   [MAX_NUM_COLORS];
  Weights    m_weights;

  mutable bool m_dirty;  // Dirty flag - used to avoid computing the hash over and over again.
  mutable int  m_hash;
};


  // @@@ think about weights indexing the bones differently, meaning getting rid of the pointer and using a path,
  // or something along the lines - this would make it unique and less error-prone than a dangling pointer
  // @@@ add to doc, that a design goal was to avoid dangling pointers

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
