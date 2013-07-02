/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiShaderSet.hpp

#ifndef VISAPISHADERSET_HPP_INCLUDED
#define VISAPISHADERSET_HPP_INCLUDED


class VDynamicMesh;
class VTextureObject;
class VisShaderSet_cl;
typedef VSmartPtr<VisShaderSet_cl>   VisShaderSetPtr;

/// \brief
///   Class for handling sets of shader-to-submesh/surface assignments. Basically an array of
///   VisDrawCallInfo_t instances
/// 
/// Shader Sets are a convenient way of handling assignments of shaders to entity submeshes with specific surfaces. For instance,
/// the VisBaseEntity_cl and VDynamicMesh classes use Shader Sets for assigning shader techniques to
/// their submeshes.
/// An instance of this class can be used either to replace specific shaders on a per instance basis or even to replace surface pointers on a
/// per entity basis.
///
/// \sa VisDrawCallInfo_t
/// \sa VisBaseEntity_cl::SetShaderSet
class VisShaderSet_cl : public VRefCounter
{
public:

  /// \brief
  ///   Constructor of the Shader Set
  VISION_APIFUNC VisShaderSet_cl();

  /// \brief
  ///   Destructor of the Shader Set
  VISION_APIFUNC virtual ~VisShaderSet_cl();
  
  /// \brief
  ///   To avoid fragmentation, this function can be used to pre-allocate enough entries. This function should be used when building a list and the number of final elements can be estimated
  inline void EnsureCapacity(int iDrawcallCount)
  {
    m_DrawCalls.EnsureSize(iDrawcallCount);
  }

  /// \brief
  ///   When building draw call lists with a significant amount of entries, it is more efficient to put the setup code into a BeginUpdate/EndUpdate bracket. This is not mandatory though.
  inline void BeginUpdate()
  {
    m_iUpdateLock++;
  }

  /// \brief
  ///   Closes the BeginUpdate/EndUpdate bracket
  inline void EndUpdate()
  {
    VASSERT(m_iUpdateLock>0);
    m_iUpdateLock--;
    OnListChanged();
  }

  /// \brief
  ///   Appends a draw call to this Shader Set. 
  ///
  /// If many Add functions are called sequentially, the code should be put into BeginUpdate/EndUpdate brackets for more efficient handling.
  /// Furthermore, to avoid fragmentation, the EnsureCapacity function should be used.
  /// 
  /// \param pSubmesh
  ///   The submesh that should be rendered
  ///
  /// \param pSurface
  ///   Surface that provides material properties for this draw call. May be a different surface pointer than the submesh's own surface
  /// 
  /// \param pTechnique
  ///   Pointer to the technique to assign
  /// 
  VISION_APIFUNC void Add(VBaseSubmesh *pSubmesh, VisSurface_cl *pSurface, VCompiledTechnique *pTechnique);

  /// \brief
  ///   Appends a draw call to this Shader Set. 
  ///
  /// If many Add functions are called sequentially, the code should be put into BeginUpdate/EndUpdate brackets for more efficient handling.
  /// Furthermore, to avoid fragmentation, the EnsureCapacity function should be used.
  ///
  /// \param drawCall
  ///   Single structure that describes the draw call. The content of this structure will be copied.
  inline void Add(const VisDrawCallInfo_t &drawCall)
  {
    m_DrawCalls[m_iNumDrawCalls++] = drawCall;
    OnListChanged();
  }

  /// \brief
  ///   Removes a draw call instance with a specific index from the Shader Set
  /// 
  /// \param iIndex
  ///   Index into the internal assignment list
  VISION_APIFUNC void Remove(int iIndex);

  /// \brief
  ///   Removes all draw calls that are associated with the passed surface
  /// 
  /// \param pSurface
  ///   Pointer to the surface
  VISION_APIFUNC void RemoveSurfaceAssignments(VisSurface_cl *pSurface);

  /// \brief
  ///   Clear the list of draw calls
  VISION_APIFUNC void Clear();

  /// \brief
  ///   Returns the index of the first occurrence of the specified surface in this Shader Set
  /// 
  /// \param pSurface
  ///   Pointer to the surface to find
  /// 
  /// \return
  ///   int iIndex: index of the first occurrence of the specified surface 
  VISION_APIFUNC int IndexOf(VisSurface_cl *pSurface) const;


  /// \brief
  ///   Returns the current length of the draw call list. Use GetDrawCallList to obtain the list itself
  inline unsigned int Count() const
  {
    return m_iNumDrawCalls;
  }

  /// \brief
  ///   Returns the array of draw calls. The returned array holds Count() entries. Entries in this array may be modified but then the code must be put into BeginUpdate/EndUpdate brackets
  inline const VisDrawCallInfo_t* GetDrawCallList() const
  {
    return m_DrawCalls.GetDataPtr();
  }

  /// \brief
  ///   Clones the shader set
  /// 
  /// \return
  ///   VisShaderSet_cl *pSet: Cloned shader set
  VISION_APIFUNC VisShaderSet_cl* Clone() const;

  /// \brief
  ///   Returns the combined tracking mask of all shader passes in this Shader Set
  /// 
  /// \return
  ///   int iTrackingMask: The combined tracking mask (bitflags of type VSHADER_TRACKING_XYZ)
  inline unsigned int GetCombinedTrackingMask() const
  {
    return m_iCombinedTrackingMask;
  }

  /// \brief
  ///   Returns a list of VisDrawCallInfo_t structures
  /// 
  /// The VisDrawCallInfo_t structures can be fed into the RenderEntity/RenderModel methods
  /// of the VisRenderLoopHelper_cl class. This function has to loop through all entries to evaluate the ePassType parameter. It copies
  /// the matching entries into the passed array. There is an overloaded version of this function that does not copy and rather returns the
  /// raw array. In case that is sufficient (i.e. no bPreBasetex filtering is required), that function should be used for performance reasons.
  /// 
  /// \param ppAssignments
  ///   Pointer to an array of VisDrawCallInfo_t structures. If NULL, this method will
  ///   simply return the number of required elements in the array. It is the user's responsibility
  ///   to allocate a sufficiently sized array.
  /// 
  /// \param ePassType
  ///   the passtype for which shaders should be returned
  /// 
  /// \param iMaxEntries
  ///   Maximum number of entries to be filled into the target array. This can be used if you want
  ///   to work with a statically sized array, in order to ensure that the boundaries of the array
  ///   will not be exceeded.
  /// 
  /// \return
  ///   int iCount: Number of Shader Assignments.
  VISION_APIFUNC unsigned int GetShaderAssignmentList(VisDrawCallInfo_t *ppAssignments, VPassType_e ePassType, unsigned int iMaxEntries) const;

  /// \brief
  ///   Returns the raw list of VisDrawCallInfo_t structures in this set. Same as combination of Count() and GetDrawCallList();
  /// 
  /// The VisDrawCallInfo_t structures can be fed into the RenderEntity/RenderModel methods
  /// of the VisRenderLoopHelper_cl class. This method does not copy any elements so it is very fast.
  /// 
  /// \param ppAssignments
  ///   Pointer reference that will receive the pointer to an array of VisDrawCallInfo_t structures. Must not be NULL
  /// 
  /// \return
  ///   int iCount: Number of Shader Assignments.
  VISION_APIFUNC unsigned int GetShaderAssignmentList(const VisDrawCallInfo_t **ppAssignments) const;

  /// \brief
  ///   Returns whether this Shader Set contains pre or post-basepass shaders
  /// 
  /// The returned value may contain the following flags: 
  /// VPT_PrimaryOpaquePass: Shader Set contains primary opaque pass shaders 
  /// VPT_SecondaryOpaquePass: Shader Set contains secondary opaque pass shaders
  /// VPT_TransparentPass: Shader Set contains transparent pass shaders
  /// 
  /// \return
  ///   VPassType_e flags
  inline unsigned char GetPassTypes() const 
  { 
    return m_cPassTypes; 
  }

  /// \brief
  ///   Returns whether this Shader Set contains a surface with a custom shadow-map fill shader
  inline bool HasCustomShadowMapFillShader() const 
  { 
    return m_bHasCustomShadowMapFillShader; 
  }


  /// \brief
  ///   Re-sorts the list using each surface's sorting key. Performed internally
  VISION_APIFUNC void SortBySurfaceSortingKeys();


  /// \brief
  ///   Convenience function to build the draw call list for a specific dynamic mesh
  ///
  /// \param pMesh
  ///   Source mesh
  ///
  /// \param ppCustomSurfaceArray
  ///   Optional array to provide a custom list of material pointers. Can be NULL to use the mesh's own list of materials. If not NULL, the array
  ///   must hold at least pMesh->GetSurfaceCount() elements. Note that this array cannot be used to render only with a subset of materials. Instead
  ///   submeshes must be filtered out using the pSubmeshFilterMask parameter.
  ///
  /// \param pCustomTechnique
  ///   If NULL, each surface uses its own technique. Otherwise, all surfaces use the passed technique
  ///
  /// \param pSubmeshFilterMask
  ///   Optional bitmask that specifies which submesh should be used. If NULL, all subemshes are used, otherwise this must point to a bitmask that holds at least
  ///   pMesh->GetSubmeshCount() bits. Accordingly the number of 32bit integer values passed as an array must at least be (pMesh->GetSubmeshCount()+31)/32.
  ///
  VISION_APIFUNC void BuildForDynamicMesh(VDynamicMesh *pMesh, VisSurface_cl **ppCustomSurfaceArray=NULL, VCompiledTechnique *pCustomTechnique=NULL, const unsigned int *pSubmeshFilterMask=NULL);


private:
  VISION_APIFUNC void OnListChanged();

  unsigned char m_cPassTypes;
  bool m_bHasCustomShadowMapFillShader;
  bool m_bKeepListSorted;
  unsigned int m_iCombinedTrackingMask;
  int m_iUpdateLock;

  unsigned int m_iNumDrawCalls;
  DynObjArray_cl<VisDrawCallInfo_t> m_DrawCalls;

  // Additional structure to keep references to the contained shaders, as VisDrawCallInfo_t only contains a raw pointer
  DynObjArray_cl<VCompiledShaderPassPtr> m_shaderReferences; 
};



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
