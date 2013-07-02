/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVertexAnimResult.hpp

#ifndef VIS_VERTEX_ANIM_RESULT_HPP_INCLUDED
#define VIS_VERTEX_ANIM_RESULT_HPP_INCLUDED

#include <Vision/Runtime/Engine/Animation/VisApiAnimResult.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiModel.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiAnimManager.hpp>
#include <Vision/Runtime/Engine/Mesh/VisVertexBufferManagers.hpp>

class VisMeshBuffer_cl;
typedef VSmartPtr<VisMeshBuffer_cl> VisMeshBufferPtr;
class VDynamicMesh;
typedef VSmartPtr<VDynamicMesh> VDynamicMeshPtr;
class VAnimatedCollisionMesh;

/// \struct SkinningStreamConfig_t
///   Data description of a stream.
struct SkinningStreamConfig_t
{
  int iNumBones;			///!< Bone count.
  int iNumVertices;			///!< Vertex count.
  int iInputVertexStride;	///!< Bytes per vertex.
  int iInputPositionOffset;	///!< Offset inside the stream's read pointer for the position data.
  int iInputNormalOffset;	///!< Offset inside the stream's read pointer for the normal data.
  int iInputTangentOffset;	///!< Offset inside the stream's read pointer for the tangent data.
  bool bCollisionOutStream;
  char padding[3];
};


/// \brief
///   Class that stores the vertex animation result
/// 
/// Every different type of animation also has a different animation result. The anim result class
/// is a base class which includes result that are needed for all animation result types.
class VisVertexAnimResult_cl: public VisAnimResult_cl
{
public:

  /// \brief
  ///   Constructor of the vertex anim result class.
  VISION_APIFUNC VisVertexAnimResult_cl(VDynamicMesh* pMesh);

    
  /// \brief
  ///   Destructor
  VISION_APIFUNC ~VisVertexAnimResult_cl();


  /// \brief
  ///   Resets all the vertex buffers (source and destinations).
  /// 
  /// Afterwards the source points to the vertices of the model in the initial pose without any
  /// vertex modifications applied.
  ///
  /// \param iFlag
  ///   Clear flags.
  ///
  VISION_APIFUNC void ClearResult(int iFlag);
  
  // this is necessary since the PS3 SDK complains about the inherited function is hidden. 
  inline void ClearResult(){ClearResult(0);}


  /// \brief
  ///   Returns a pointer to the used mesh.
  /// 
  /// \return
  ///   VDynamicMesh* pMesh: pointer to the mesh.
  inline VDynamicMesh* GetMesh() const 
  {
    return m_spMesh;
  }


  /// \brief
  ///   Sets a new dynamic mesh on the config.
  /// 
  /// \param pMesh
  ///   New mesh.
  /// 
  /// \return
  ///   Nothing.
  void SetMesh(VDynamicMesh* pMesh);

  /// \brief
  ///   Provides the pointer to a valid source of vertex positions.
  /// 
  /// Each vertex position consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pSourceVertexPosition
  ///   Out: pointer reference that receives the address of the first vertex position
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetSourceVertexPosition(float* &pSourceVertexPosition);
  

  /// \brief
  ///   Provides the pointer to a valid source of vertex normals.
  /// 
  /// Each vertex normal consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pSourceVertexNormal
  ///   pointer reference that receives the address of the first vertex normal
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetSourceVertexNormal(float* &pSourceVertexNormal);


  /// \brief
  ///   Provides the pointer to a valid source of vertex tangents.
  /// 
  /// Each vertex tangent consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pSourceVertexTangent
  ///   pointer reference that receives the address of the first vertex tangent
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetSourceVertexTangent(float* &pSourceVertexTangent);


  /// \brief
  ///   Provides the pointer to a valid destination to write vertex positions
  /// 
  /// Each vertex position consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pDestVertexPosition
  ///   pointer reference that receives the address of the first vertex position
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetDestVertexPosition(float* &pDestVertexPosition);


  /// \brief
  ///   Provides the pointer to a valid destination to write vertex normals
  /// 
  /// Each vertex normal consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pDestVertexNormal
  ///   pointer reference that receives the address of the first vertex normal
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetDestVertexNormal(float* &pDestVertexNormal);
  
  
  /// \brief
  ///   Provides the pointer to a valid destination to write vertex tangents
  /// 
  /// Each vertex tangent consists of 3 floats.
  /// 
  /// The return value defines the stride between two vertices.
  /// 
  /// \param pDestVertexTangent
  ///   pointer reference that receives the address of the first vertex tangent
  /// 
  /// \return
  ///   int iStride: vertex stride in bytes.
  VISION_APIFUNC int GetDestVertexTangent(float* &pDestVertexTangent);

  /// \brief
  ///   For internal use only!
  /// 
  /// TouchRenderBuffers is used by the engine to update the resources time to the current frame.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void TouchRenderBuffers(void) const; 

  /// \brief
  ///   For internal use only! Used by the engine to ensure that all render buffers exist
  void EnsureRenderBuffersExist(VisSkinningMode_e eSkinningMode);

  /// \brief
  ///   Used by the engine to ensure that all render buffers are freed
  void EnsureRenderBuffersFreed();


#if defined(_VR_DX11) || defined(_VISION_XENON) || defined(_VISION_WIIU)

  /// \brief
  ///   For internal use only!
  /// 
  /// PeformMemExportSkinning is used by the engine to perform memexport skinning
  ///
  /// \param bUseQuaternionSkinning
  ///   true if quaternion skinning should be used (currently only supported on Xbox360), otherwise false.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void PerformMemExportSkinning(bool bUseQuaternionSkinning);

#endif

#ifdef _VR_DX11

  /// \brief
  ///   For internal use only!
  /// 
  /// PerformComputeShaderSkinning is used by the engine to perform compute shader skinning
  ///
  /// \return
  ///   Nothing.
  VISION_APIFUNC void PerformComputeShaderSkinning();
#endif

// internal use only!


  #ifdef _VISION_PS3
  VISION_APIFUNC void KickoffSPUSkinning(const VisSkeletalAnimResult_cl *pSkeletalResult, VAnimatedCollisionMesh *pColMesh);
  VISION_APIFUNC void WaitForSPUSkinningResult();
  #endif


  inline VisMeshBuffer_cl *GetSkinningMeshBuffer() { return m_spSkinningMeshBuffer; }

  /// \brief
  ///   returns the flags set on this instance.
  inline int GetFlags() const { return m_iFlags; }
  
  /// \brief
  ///   returns wether tangents are skinned or not.
  inline int GetSkinTangents() const { return m_bSkinTangents; }

private:

  friend class VisAnimConfig_cl;
  VDynamicMeshPtr m_spMesh;
  
  bool m_bModelVertexPositionsTouched;
  bool m_bModelVertexNormalsTouched;
  bool m_bModelVertexTangentsTouched;
  int m_iFlags;
  bool m_bSkinTangents;   ///< turns tangent skinning on and off
  
  // optional buffers
  VisSystemMemoryBufferPtr m_spSystemMemoryBuffer;

  VisMeshBufferPtr m_spSkinningMeshBuffer;
  
  bool m_bUseExtraSystemBuffer;
  bool m_bUseCollisionSystemBuffer;

  /// \brief
  ///   Used by the engine to transfer all vertices to the final render buffer.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   Nothing.
  void TransferVerticesToRenderBuffer(void);


  /// \brief
  ///   Used by the engine to transfer all vertices to the collision mesh. For internal use only!
  void TransferVerticesToCollisionMesh(VAnimatedCollisionMesh *pColMesh);


  /// \brief
  ///   Indicates whether an extra system memory buffer is used for modifying the vertices.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   bool bResult: True if an extra system memory buffer is used.
  inline bool UsesExtraSystemBuffer() const
  {
    return m_bUseExtraSystemBuffer;
  }
  

  /// \brief
  ///   Indicates whether an extra collision memory buffer is used for modifying the vertices.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   bool bResult: True if an extra system memory buffer is used.
  inline bool UsesCollisionSystemBuffer() const 
  {
    return m_bUseCollisionSystemBuffer;
  }

  /// \brief
  ///   Indicates whether all used render buffers are valid or need to be recreated.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   bool bValid: True if all render buffers are valid.
  VISION_APIFUNC bool AreRenderBuffersValid() const;


  /// \brief
  ///   Indicates whether there are extra buffers and whether they are valid.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   bool bValid: True if all extra buffers are valid.
  inline bool AreSystemBuffersValid() const
  {
    return m_spSystemMemoryBuffer!=NULL && m_spSystemMemoryBuffer->IsLoaded();
  }


  /*
  /// \brief
  ///   Indicates whether there is a collision buffer and whether it is valid.
  /// 
  /// For internal use only!
  /// 
  /// \return
  ///   bool bValid: True if collision buffer is valid.
  inline bool IsCollisionBufferValid() const
  {
    return(m_spCollisionBuffer.m_pPtr&&m_spCollisionBuffer->IsLoaded());
  }
  */
 
  
  /// \brief
  ///   For internal use only!
  /// 
  /// TouchSystemBuffers is used by the engine to update the resources time to the current frame.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void TouchSystemBuffers(void) const 
  {
    if (m_spSystemMemoryBuffer != NULL)
      m_spSystemMemoryBuffer->EnsureLoaded();
  }
  
  /*
  /// \brief
  ///   For internal use only!
  /// 
  /// TouchCollisionBuffer is used by the engine to update the resources time to the current frame.
  /// 
  /// \return
  ///   Nothing.
  VISION_APIFUNC void TouchCollisionBuffer() const
  {
    if(m_spCollisionBuffer.m_pPtr)
      m_spCollisionBuffer->EnsureLoaded();
  }
  */

  VISION_APIFUNC bool UsesExtraSystemBuffer_Check() const;
  VISION_APIFUNC bool UsesCollisionSystemBuffer_Check() const;
  
#ifdef _VISION_PS3
  SkinningStreamConfig_t m_SkinningStreamConfig;
  VStreamProcessingWorkflow *m_pSkinningWorkflow;
#endif


  bool HasVertexPositionBeenTouched(void) const {return m_bModelVertexPositionsTouched;}
  bool HasVertexNormalBeenTouched(void) const {return m_bModelVertexNormalsTouched;}
  bool HasVertexTangentBeenTouched(void) const {return m_bModelVertexTangentsTouched;}

  // serialisation
  V_DECLARE_SERIAL_DLLEXP( VisVertexAnimResult_cl, VISION_APIDATA );
  VISION_APIFUNC virtual void Serialize( VArchive &ar );
  VISION_APIFUNC VisVertexAnimResult_cl();
  VISION_APIFUNC virtual void Debug_ShowHierarchy(IVRenderInterface *pRI, int iGeneration, float &x, float &y);

  VISION_APIFUNC void LockRenderBuffers();
  VISION_APIFUNC void UnlockRenderBuffers();

  void CommonInit();
public:
  hkvAlignedBBox m_ModifiedBBox; ///< bounding box that stores the modifications from all vertex animations
};

#endif //VIS_ANIM_RESULT_HPP_INCLUDED

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
