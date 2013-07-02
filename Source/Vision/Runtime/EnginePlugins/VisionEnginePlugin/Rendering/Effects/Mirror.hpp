/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Mirror.hpp

#ifndef MIRROR_HPP_INCLUDED
#define MIRROR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

class VisMirror_cl;
typedef VSmartPtr<VisMirror_cl> VisMirrorPtr;
class VisMirrorManager_cl;

#define MIRROR_MAX_VIEWCONTEXTS   32
#define MAX_SEPARATE_FRUSTA       4

/// \brief
///   This class represents a simple vertex for mirror rendering.
struct MirrorVertex_t
{
  float pos[3];               ///< Position
  float normal[3];            ///< Normal
  float tex[2];               ///< UV
  VColorRef color;            ///< Vertex Color
};

/// \brief
///   This callback data object is sent by the static VisMirrorManager_cl::OnMirrorRenderHook callback.
///   It holds information about the current mirror and the render hook entry point.
class VisMirrorRenderHookDataObject_cl : public VisRenderHookDataObject_cl
{
public:
  VisMirrorRenderHookDataObject_cl(VisCallback_cl *pSender, VisMirror_cl *pMirror, unsigned int iEntryConst) 
    : VisRenderHookDataObject_cl(pSender,iEntryConst), m_pMirror(pMirror)
  {
  }

  VisMirror_cl *m_pMirror; ///< The mirror that is currently being rendered
};

/// \brief
///   This class represents a mirror instance, i.e. the native counterpart of the vForge mirror shape.
class VisMirror_cl : public VisObject3D_cl
{
public:

  /// \brief
  ///   Enum that is passed to SetReflectionShaderMode.
  enum VReflectionShaderSets_e
  {
    AlwaysSimple = 0,         ///< Always use very simple shaders for geometry inside the mirror (best performance, low quality).
    SimpleForAUTO = 1,        ///< Use simple shaders only when the material is marked as AUTO shader assignment.
    AlwaysSurfaceShaders = 2, ///< Always use the scene geometry's shaders for mirror rendering (high quality, big performance impact).
  };

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor
  /// 
  /// \param pManager
  ///   The mirror manager to which to add this mirror. Typically the global instance accessible via VisMirrorManager_cl::GlobalManager().
  /// 
  /// \param iResolution
  ///   The resolution of the mirror's render target (square texture).
  ///
  /// \param bUseHDR
  ///   HDR flag. If true, the format of the render target is set to R16G16B16A16F. Otherwise it is set to R8G8B8A8.
  EFFECTS_IMPEXP VisMirror_cl(VisMirrorManager_cl *pManager,int iResolution=512, bool bUseHDR=false);

  /// \brief
  ///   Destructor.
  EFFECTS_IMPEXP virtual ~VisMirror_cl();

  /// \brief
  ///   Initializes the Mirror.
  EFFECTS_IMPEXP void Init();

  /// \brief
  ///   Overridden function.
  EFFECTS_IMPEXP virtual void DisposeObject();

  /// \brief
  ///   Overridden function to respond to transformation changes.
  EFFECTS_IMPEXP virtual void OnObject3DChanged(int iO3DFlags);

  /// \brief
  ///   Returns the index within the parent manager's instance list.
  EFFECTS_IMPEXP int GetNumber() const;

  ///
  /// @}
  ///

  ///
  /// @name Main Properties
  /// @{
  ///

  /// \brief
  ///   Sets the resolution of the render target (square texture).
  EFFECTS_IMPEXP void SetResolution(int iRes);

  /// \brief
  ///   Returns the resolution of the render target (square texture).
  inline int GetResolution() {return m_iResolution;}

  /// \brief
  ///   Sets the HDR flag. If true, the format of the render target is set to R16G16B16A16F. Otherwise it is set to R8G8B8A8. May impact performance and memory.
  EFFECTS_IMPEXP void SetUseHDR(bool bUseHDR);

  /// \brief
  ///   Returns the HDR flag. If true, the format of the render target is R16G16B16A16F. Otherwise, R8G8B8A8. May impact performance and memory.
  inline bool GetUseHDR() const {return m_bUseHDR;}

  /// \brief
  ///   Sets the extent of the mirror plane. This affects the mirror plane mesh and also the bounding box of the main visibility object.
  EFFECTS_IMPEXP void SetSize(float x, float y);

  /// \brief
  ///   Returns the extent of the mirror plane in x direction.
  inline float GetSizeX() const {return m_fSizeX;}

  /// \brief
  ///   Returns the extent of the mirror plane in y direction.
  inline float GetSizeY() const {return m_fSizeY;}

  /// \brief
  ///   Returns the extent of the mirror plane in each direction as a vector (z is always 1).
  inline hkvVec3 GetSize() const {return hkvVec3(m_fSizeX, m_fSizeY, 1);}

  /// \brief
  ///   Returns the mirror plane.
  inline const hkvPlane& GetPlane() const {return m_Plane;}

  /// \brief
  ///   In case a model is used, this defines the scaling for the mesh.
  inline void SetModelScale(float x, float y, float z)
  {
    m_vModelScale.set(x,y,z);
    UpdateMirror();
  }

  /// \brief
  ///   In case a model is used, this returns the scaling for the mesh.
  inline hkvVec3 GetModelScale() const {return m_vModelScale;}

  ///
  /// @}
  ///

  ///
  /// @name Position and Orientation
  /// @{
  ///

  /// \brief
  ///   Activates or deactivates the mirror (default is active).
  EFFECTS_IMPEXP void SetActive(bool bStatus);

  /// \brief
  ///   Indicates whether this mirror is active.
  inline bool IsActive() const {return m_bSupported && m_bActive && m_bValidFX;}

  /// \brief
  ///   Shows or hides the plane mesh.
  EFFECTS_IMPEXP void SetShowPlaneObject(bool bStatus);

  /// \brief
  ///   Sets the render filter mask for the underlying rendering context.
  EFFECTS_IMPEXP void SetRenderFilterMask(unsigned int uiMask);

  /// \brief
  ///   Sets the visible bitmask for the mirror mesh / the entity model.
  EFFECTS_IMPEXP void SetVisibleBitmask(unsigned int uiMask);

  /// \brief
  ///   Sets the render hook at which the mirror mesh should be rendered.
  EFFECTS_IMPEXP void SetRenderHook(unsigned int uiRenderHook);

  /// \brief
  ///   Sets the FOV scaling factor for the mirror. This value can be used to reduce texture clamping artifacts
  ///   caused by perturbing the mirror normal in a shader / by vertex displacements.
  inline void SetFovScale(float fFovScale) { m_fFovScale = fFovScale; }

  /// \brief
  ///   Returns the value previously set with SetFovScale. The default is 1.0f.
  inline float GetFovScale() const { return m_fFovScale; }

  /// \brief
  ///   Sets whether the LOD behavior from reference context should be used. The default value is true. When enabled,
  ///   the LOD instances in the mirror switches in parallel to those in the view context.
  EFFECTS_IMPEXP void SetUseLODFromRefContext(bool bLODFromRefContext);

  inline bool GetUseLODFromRefContext() const { return m_bLODFromRefContext; }

  ///
  /// @}
  ///

  ///
  /// @name Debugging
  /// @{
  ///

  /// \brief
  ///   Renders the extent of the mirror plane as lines.
  EFFECTS_IMPEXP void DebugRender(IVRenderInterface* pRenderer, VColorRef iColor=V_RGBA_BLUE) const;

  /// \brief
  ///   Shows or hides a screenmask overlay that shows the mirror's render target.
  EFFECTS_IMPEXP void SetShowDebugMirrorTexture(bool bStatus);

  ///
  /// @name Mirror Shader
  /// @{
  ///

  /// \brief
  ///   Sets the effect that is used for rendering the mirror mesh.
  EFFECTS_IMPEXP void SetEffect(VCompiledEffect *pEffect);

  /// \brief
  ///   Called when re-applying a shader through the engine.
  EFFECTS_IMPEXP void ReassignEffect();

  /// \brief
  ///   Returns the technique that is used for rendering.
  inline VCompiledTechnique *GetTechnique() const {return m_spMirrorTechnique;}

  /// \brief
  ///   Returns the resolved mirror render target texture.
  EFFECTS_IMPEXP VTextureObject* GetMirrorTexture();

  /// \brief
  ///   Optionally loads a model mesh file (.model) that receives the mirror. This mesh must have its local pivot at z=0.
  EFFECTS_IMPEXP virtual void SetModelFile(const char *szModel);

  /// \brief
  ///   Optionally applies this mirror to a world surface. The mirror's pivot position must be in the geometry's plane.
  EFFECTS_IMPEXP void ApplyToWorldSurface(VisSurface_cl *pWorldSrf, VisStaticMeshInstance_cl *pInst);

  /// \brief
  ///   Obsolete. Handled internally with dependent contexts.
  inline void SetSourceContext(VisRenderContext_cl *pContext) {m_pSourceContext=pContext;}

  /// \brief
  ///   Defines the shader accuracy for reflections (quality vs. performance). The default mode is 'AlwaysSimple'.
  inline void SetReflectionShaderMode(VReflectionShaderSets_e mode) {m_eReflectionShaderMode=mode;}

  /// \brief
  ///   Returns the quality mode that has been set via SetReflectionShaderMode.
  inline VReflectionShaderSets_e GetReflectionShaderMode() const {return m_eReflectionShaderMode;}

  /// \brief
  ///   If the mirror uses a model file, this function returns the associated model.
  inline VDynamicMesh *GetMesh() const { return m_spDynamicMesh; }

  /// \brief
  ///   Returns the current oblique clipping projection matrix for this mirror.
  inline const hkvMat4& GetObliqueClippingProjection() const { return m_ObliqueClippingProjection; }

  /// \brief
  ///   Returns the current oblique clipping plane for this mirror.
  inline const hkvPlane& GetMirrorPlane() const { return m_ObliqueClippingPlane; }

  /// \brief
  ///   Can be used to specify an offset for positioning the oblique clipping plane relative to the mirror surface.
  ///   Negative values will position the clipping plane below the mirror plane, thus reducing rendering artifacts
  ///   e.g. for water rendering close to the shore.
  inline void SetObliqueClippingPlaneOffset(float fOffset) { m_fObliqueClippingPlaneOffset = fOffset; }

  /// \brief
  ///   Returns the value previously set with SetObliqueClippingPlaneOffset. The default value is 0.0f.
  inline float GetObliqueClippingPlaneOffset() const { return m_fObliqueClippingPlaneOffset; }

  /// \brief
  ///   Specifies a reference object for the mirror. This function can be used to override the internal mesh object's
  ///   translation and rotation values used to configure the shader.
  EFFECTS_IMPEXP void SetReferenceObject(VisObject3D_cl *pRefObject);

  ///
  /// @}
  ///

  ///
  /// @name Visibility
  /// @{
  ///

  /// \brief
  ///   Sets a custom far clipping distance for this mirror. Can be left to -1 to use the scene's distance.
  ///   This feature can significantly improve performance.
  inline void SetFarClipDistance(float fDist) {m_fFarClipDist=fDist;}

  /// \brief
  ///   Returns the actual far clipping distance of the mirror's render context.
  EFFECTS_IMPEXP float GetActualFarClipDistance() const;

  /// \brief
  ///   Adds an additional visibility object to this mirror. The mirror is rendered if any attached visibility object appears visible.
  inline void AddVisibilityObject(VisVisibilityObject_cl *pVisObj, bool bRemoveDefault=true) {m_VisibilityObjects.AddUnique(pVisObj);if (bRemoveDefault) RemoveDefaultVisibilityObject();}

  /// \brief
  ///   Removes a visibility object.
  inline void RemoveVisibilityObject(VisVisibilityObject_cl *pVisObj) {if (m_VisibilityObjects.Contains(pVisObj)) m_VisibilityObjects.Remove(pVisObj);}

  /// \brief
  ///   Removes all attached visibility object and optionally creates the default one.
  inline void RemoveAllVisibilityObjects(bool bCreateDefault=true) {m_VisibilityObjects.Clear();if (bCreateDefault) AddDefaultVisibilityObject();}

  /// \brief
  ///   Returns the number of attached visibility objects.
  inline int GetVisibilityObjectCount() const {return m_VisibilityObjects.Count();}

  /// \brief
  ///   Returns the visibility object at index i.
  inline VisVisibilityObject_cl *GetVisibilityObject(int i) const {return m_VisibilityObjects.GetAt(i);}

  /// \brief
  ///   Adds the default visibility object that is always as large as the bounding box.
  EFFECTS_IMPEXP void AddDefaultVisibilityObject();

  /// \brief
  ///   Removes the default visibility object.
  EFFECTS_IMPEXP void RemoveDefaultVisibilityObject();

  /// \brief
  ///   Returns the absolute world space bounding box of this mirror.
  EFFECTS_IMPEXP const hkvAlignedBBox& GetBoundingBox();

  /// \brief
  ///   Internal update function.
  EFFECTS_IMPEXP void UpdateDefaultVisibilityObject();

  /// \brief
  ///   Picks the mirror (used by vForge).
  EFFECTS_IMPEXP float TraceMirror(const hkvVec3& vStart, const hkvVec3& vEnd, bool bDoubleSided) const;

  ///
  /// @}
  ///

  ///
  /// @name Render Hooks
  /// @{
  ///

  /// \brief
  ///   Specifies whether render hooks (e.g. for SpeedTrees) should be executed by the Mirror Render Loop.
  ///   Note that the implementation of this function depends on the individual render loop.
  inline void SetExecuteRenderHooks(bool bStatus) { m_bExecuteRenderHooks = bStatus; }

  /// \brief
  ///   Returns whether render hooks (e.g. for SpeedTrees) are executed by the Mirror Render Loop.
  inline bool GetExecuteRenderHooks() const { return m_bExecuteRenderHooks; }

  ///
  /// @}
  ///

#ifndef _VISION_DOC
  EFFECTS_IMPEXP virtual void HandleMirror(VisRendererNodeDataObject_cl &data);
protected:
  EFFECTS_IMPEXP virtual void SetTechnique(VCompiledTechnique *pEffect);
  EFFECTS_IMPEXP void InitMirror();
  EFFECTS_IMPEXP void UpdateMirror();
  EFFECTS_IMPEXP virtual void CreateMesh();
  EFFECTS_IMPEXP virtual void CreateMeshBufferObject(VisMeshBuffer_cl* pMesh);
  EFFECTS_IMPEXP void UpdateCamera(VisRenderContext_cl *pRefContext);

public:
  //serialization
  V_DECLARE_SERIAL_DLLEXP( VisMirror_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  // resource functions
#ifdef SUPPORTS_SNAPSHOT_CREATION
  EFFECTS_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot);
#endif
  hkvMat4  m_ObliqueClippingProjection;
  hkvPlane m_ObliqueClippingPlane;
  VReflectionShaderSets_e m_eReflectionShaderMode;
  
#ifdef _VISION_XENON
  inline VTextureObject *GetTargetTexture() {return m_spResolveTexture;}
#endif
  
#endif //_VISION_DOC

protected:
  EFFECTS_IMPEXP VisMirror_cl(); ///< reserved for serialization (doesn't call InitMirror)
  friend class VisMirrorManager_cl;
  void  GetWorldSpaceVertices(hkvVec3* pVert) const;
  void  SetupSingleShaderProjection(VCompiledShaderPass *shader, const hkvVec3& campos, const hkvMat3 &camrot);
  void  PrepareProjectionPlanes();
  float GetClosestDist(const hkvVec3& vCamPos);
  void  RecreateRenderTarget();
  void  FreeRenderTarget();
  IVisVisibilityCollector_cl *GetVisibilityCollectorForView(VisRenderContext_cl *pView);
  void  ClearViewVisibilityCollectors();
  float GetMirrorDistanceFromCameraPlane(const VisContextCamera_cl &mainCam);

  VisMirrorManager_cl *m_pParentManager;
  
  // Instance properties
  float m_fSizeX, m_fSizeY;
  int   m_iResolution;
  bool  m_bUseHDR;
  VDynamicMeshPtr m_spDynamicMesh;
  float m_fFarClipDist;
  unsigned int m_iContextBitmask;
  unsigned int m_iVisibleBitmask;
  unsigned int m_uiRenderHook;
  float m_fObliqueClippingPlaneOffset;

  // Mirror properties
  bool m_bVisibleThisFrame, m_bActive, m_bSupported, m_bValidFX, m_bLODFromRefContext;
  hkvVec3 m_vLocalMirrorVert[4];                  ///< Mirror vertices
  VisRenderContextPtr     m_spReflectionContext;
  VisRenderableTexturePtr m_spRenderTarget_Refl;  ///< Smart pointer to render target texture (also in m_ManipData.pTexture)
  VisRenderableTexturePtr m_spDepthStencilTarget_Refl;
  hkvPlane                m_Plane;                ///< The mirror plane
  hkvAlignedBBox          m_BoundingBox;          ///< The mirror vertex bounding box
  VisMeshBufferObjectPtr  m_spMeshObj;            ///< Mesh object with mirror geometry
  VCompiledTechniquePtr   m_spMirrorTechnique;    ///< Shader effect used for the mirror
  VisEffectConfig_cl      m_MirrorEffect;         ///< Source shader effect
  VisScreenMaskPtr        m_spDebugMask;          ///< DEBUG : screenmask for displaying render target

  VisRefCountedVisibilityObjectCollection_cl m_VisibilityObjects;
  VisVisibilityObjectAABoxPtr m_spDefaultBBoxVisObj;

  // Note: this may not be a smart pointer. Otherwise, it may keep references
  // to render targets which should actually be deleted.
  VisRenderContext_cl *m_pSourceContext;

  // Visibility handling
  int m_iViewContextCount;
  VisRenderContext_cl *m_pViewContext[MIRROR_MAX_VIEWCONTEXTS];
  IVisVisibilityCollectorPtr m_spViewVisibility[MIRROR_MAX_VIEWCONTEXTS];

#ifdef _VISION_XENON
  VTextureObjectPtr m_spResolveTexture;
#endif

  hkvVec3         m_vMirrorPos;
  hkvMat3         m_MirrorCamRot;
  VisSurface_cl  *m_pWorldSurface;                        ///< If mirror is applied to world surface...
  VSmartPtr<VisStaticMeshInstance_cl> m_spStaticMeshInst; ///< ..of a static mesh instance
  VisObject3D_cl *m_pReferenceObject;

  hkvVec3         m_vModelScale;
  float           m_fFovScale;

  bool            m_bExecuteRenderHooks;
  bool            m_bDoubleSided;
  bool            m_bCameraOnFrontSide;
};

/// \brief
///   Manager class that manages all mirrors in the scene.
///   One global instance can be accessed via VisMirrorManager_cl::GlobalManager()
class VisMirrorManager_cl : public IVisCallbackHandler_cl
{
public:
  VisMirrorManager_cl();

  EFFECTS_IMPEXP void OneTimeInit();
  EFFECTS_IMPEXP void OneTimeDeInit();
  EFFECTS_IMPEXP static VisMirrorManager_cl &GlobalManager();

  /// \brief
  ///   Finds the mirror instance with passed object key. This key string can be set in vForge.
  inline VisMirror_cl* FindByKey(const char *szKey) const
  {
    const int iCount = m_Instances.Count();
    for (int i=0;i<iCount;i++)
      if (m_Instances.GetAt(i)->HasObjectKey(szKey))
        return m_Instances.GetAt(i);
    return NULL;
  }

  /// \brief
  ///   Static callback that is triggered in various places inside the mirror render loop
  ///
  ///   Custom modules can register to this callback to implement custom rendering inside mirrors. The data object can be casted to
  ///   VisMirrorRenderHookDataObject_cl which holds the mirror instance and the render hook entry constant. This constant can have
  ///   the following values: VRH_PRE_PRIMARY_OPAQUE_PASS_GEOMETRY, VRH_PRE_OCCLUSION_TESTS, VRH_DECALS,
  ///   VRH_CORONAS_AND_FLARES, VRH_PRE_OCCLUSION_TESTS, VRH_PRE_TRANSPARENT_PASS_GEOMETRY.
  EFFECTS_IMPEXP static VisCallback_cl OnMirrorRenderHook;
  
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  VRefCountedCollection<VisMirror_cl> m_Instances;
private:
  static VisMirrorManager_cl g_MirrorManager;
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
