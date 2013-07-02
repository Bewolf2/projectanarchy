/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Sky.hpp

#ifndef _SKY_HPP_INCLUDED
#define _SKY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/SkyLayer.hpp>

class VSky;
typedef VSmartPtr<VSky> VSkyPtr;

/// \brief
///   Sky management
///
class VSkyManager : public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Global singleton instance
  static VSkyManager &GlobalManager();

  /// \brief
  ///   Initialization
  void OneTimeInit();

  /// \brief
  ///   Deinitialization
  void OneTimeDeinit();

protected:
  /// \brief
  ///   Callbacks
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  static VSkyManager s_manager;
};

/// \brief
///   Base class for sky implementations.
///
class VSkyBase : public IVSky
{
public:
  /// \brief
  ///   Constructor
  VSkyBase() : IVSky(), m_iVisibleBitmask(0xffffffff) { }

  /// \brief
  ///   Destructor
  virtual ~VSkyBase() { }

  /// \brief
  ///   One-time initialization
  EFFECTS_IMPEXP virtual void Initialize() { }

  /// \brief
  ///   Overridable render function.
  EFFECTS_IMPEXP virtual void Render() { }

  /// \brief
  ///   Overridable funciton, called to activate this sky instance.
  EFFECTS_IMPEXP virtual void Activate() { } 

  /// \brief
  ///   Overridable funciton, called to deactivate this sky instance.
  EFFECTS_IMPEXP virtual void Deactivate() { }

  /// \brief
  ///   Overridable render function.
  EFFECTS_IMPEXP virtual void SetEffect(VCompiledEffect *pFX) { }

  /// \brief
  ///   Tick function.
  EFFECTS_IMPEXP virtual void Tick(float) { }

  /// \brief
  ///   Called before rendering. 
  EFFECTS_IMPEXP virtual void PrepareForRendering() { }

  /// brief
  ///   Called after frame started and before rendering the sky. 
  EFFECTS_IMPEXP virtual void ComputeDynamicLookups() {}

  /// \brief
  ///   Determines face visibility for skyboxes.
  EFFECTS_IMPEXP virtual void DetermineVisibleFaces() { }
    
  /// \brief
  ///   Serializes/deserializes this sky
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) { }

  /// \brief
  ///   Writes this sky instance to a binary file.
  EFFECTS_IMPEXP virtual void WriteToStream(IVFileOutStream *pOut) { }

  /// \brief
  ///   When overridden, creates the specified number of sky layers for this sky
  EFFECTS_IMPEXP virtual void CreateLayers(int iCount) 
  { 
    VASSERT_MSG(0, "Not implemented!");
  }

  /// \brief
  ///  To be called when a layer property has changed that requires
  ///  e.g. recreation of shader resources.
  EFFECTS_IMPEXP virtual void OnLayerChanged(VSkyLayer* pLayer) {}

  /// \brief
  ///   Accesses the sky layer with specified index
  /// 
  /// Any layer can be retrieved during runtime and then modified. No copy operation takes place.
  /// 
  /// \param iIndex
  ///   0-based layer index. Must be in valid range [0..GetLayerCount()-1]
  /// 
  /// \return
  ///   VSkyLayer &layer : Reference to the sky layer
  EFFECTS_IMPEXP virtual VSkyLayer& GetLayer(int iIndex) const
  { 
    static VSkyLayer dummy; // prevent warnings
    VASSERT_MSG(0, "Not implemented!");
    return dummy;
  }

  /// \brief
  ///   Returns the number of layers created via CreateLayers
  EFFECTS_IMPEXP virtual int GetLayerCount() const
  {
    return 0;
  }

  ///
  /// @name Visible State
  /// @{
  ///

  /// \brief
  ///   Sets the visible bitmask of the sky instance
  /// 
  /// If set to 0, the sky is not rendered. Otherwise the 32bit value gets compared to each
  /// context's render filter bitmask (See VisRenderContext_cl::GetRenderFilterMask)
  /// 
  /// The default value is 0xffffffff.
  /// 
  /// \param iBitmask
  ///   32bit bitmask value for logically AND operation with the context
  EFFECTS_IMPEXP virtual void SetVisibleBitmask(unsigned int iBitmask) HKV_OVERRIDE
  {
    m_iVisibleBitmask=iBitmask;
  }

  /// \brief
  ///   Returns the bitmask set via SetVisibleBitmask
  EFFECTS_IMPEXP virtual unsigned int GetVisibleBitmask() const HKV_OVERRIDE
  {
    return m_iVisibleBitmask;
  }


  ///
  /// @}
  ///

  /// \brief
  ///   If true, the sky instance requires precomputation and should display a progress bar in vForge
  EFFECTS_IMPEXP virtual bool RequiresPrecomputation() const { return false; }

  /// \brief
  ///   Performs the actual precomputation process (see RequiresPrecomputation())
  EFFECTS_IMPEXP virtual void UpdatePrecomputedData(const VString &path, bool bForce) { }

  /// \brief
  ///   Optional callback to cache any precomputed data.
  EFFECTS_IMPEXP virtual void OnSaveScene(const VString &path) { }

  /// \brief
  ///   Optional callback to create device objects for the precomputed data.
  EFFECTS_IMPEXP virtual void UploadLookupDeviceObjects() { }

  /// \brief
  ///   Callback that should be triggered during the precomputation process to update the progress bar in vForge
  VisCallback_cl OnPrecomputationProgressChanged;

  V_DECLARE_DYNCREATE_DLLEXP( VSkyBase, EFFECTS_IMPEXP )
  V_DECLARE_VARTABLE( VSkyBase, EFFECTS_IMPEXP );
  
  unsigned int m_iVisibleBitmask;

protected:

  /// \brief
  ///   Helper function to calculate the depthfog value for the sky. 
  ///   This value is relevant when the depthfog and the apply-fog-to-sky setting is active.
  ///   If the depthfog postprocessor is activated the function will return 0.
  virtual float CalculateFogDepth() const;

};


/// \brief
///   Class for multi-layer sky with up to four texture layers.
///
/// This class provides functionality for conveniently defining multi-layer skies.
/// 
/// Multiple instances of the VSky class may be present at the same time, though only one
/// instance may be active at a time. See the VSky::Activate resp. VisWorld_cl::SetActiveSky
/// method for further reference.
/// 
/// Skies can be created in vForge.
/// 
/// Skies are rendered with a shader effect. If no custom shader effect is applied, the engine uses
/// a default shader from the library Shaders/DefaultSky.ShaderLib that alpha-blends between the
/// layers.
/// 
/// Since the sky class is derived from VisTypedEngineObject_cl, it can fully serialize itself.
class VSky : public VSkyBase, public IVisCallbackHandler_cl
{
public:
  ///
  /// @name Constructors / Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the sky class
  /// 
  /// The constructor of the sky class initialises an empty sky instance. You have to define layers
  /// using the CreateLayers method in order to fill the sky instance with data.
  /// 
  /// \param boxSubDivs
  ///   This value specifies the number of subidivisions of the sky box. Skies with spherical
  ///   mapping typically require subdivisions in order to ensure sufficiently accurate texture
  ///   mapping. A value of 12 to 16 is recommended for such cases. If only cube-mapped sky layers
  ///   are used, you can set a value of 1 here. The number of triangles used for rendering the sky
  ///   is equal to 6 * boxSubDivs^2 * 2, so the fewer the subdivision level, the fewer triangles
  ///   have to be rendered.
  EFFECTS_IMPEXP VSky(int boxSubDivs=1);

  /// \brief
  ///   Constructor of the sky class
  ///
  /// This constructor creates a multi-layer sky box for use with the Time of Day system.  
  /// 
  /// \param szPrefixNoon
  ///   Filename prefix for the set of sky textures at noon. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixDawn
  ///   Filename prefix for the set of sky textures at dawn. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixDusk
  ///   Filename prefix for the set of sky textures at dusk. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szPrefixNight
  ///   Filename prefix for the set of sky textures at night. The suffixes "_right", "_left", "_top", "_bottom",
  ///   "_front", and "_back" will be added to the prefix to construct the final file names.
  /// 
  /// \param szExtension
  ///   Extension of the sky texture filenames (e.g. "dds" or "bmp").
  /// 
  /// \param bUseBottom
  ///   true if the bottom part of the sky texture should be loaded, otherwise false.
  /// 
  /// \param pFX
  ///   Optional effect override.
  EFFECTS_IMPEXP VSky(const char *szPrefixNoon, const char *szPrefixDawn, const char *szPrefixDusk, 
    const char *szPrefixNight, const char *szExtension, bool bUseBottom, VCompiledEffect *pFX);

  /// \brief
  ///   Creates a single layer cubemap sky with 5 or 6 sides
  /// 
  /// \param pszTextures
  ///   A string like "sky\\face_%s.dds" that contains a %s that will be replaced by the right,
  ///   left, front, back, top and (optionally) bottom for each side.
  /// 
  /// \param iNumSides
  ///   Must be 5 or 6. If 6 then the bottom will also be used.
  EFFECTS_IMPEXP VSky(const char* pszTextures, int iNumSides);
  
  /// \brief
  ///   Destructor of the sky class
  /// 
  /// The destructor of the sky class releases the allocated memory and deletes the sky instance.
  EFFECTS_IMPEXP virtual ~VSky();

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene.
  EFFECTS_IMPEXP virtual void DisposeObject() HKV_OVERRIDE;

  EFFECTS_IMPEXP virtual void Initialize() HKV_OVERRIDE;

  ///
  /// @}
  ///


  ///
  /// @name Create Layers
  /// @{
  ///

  /// \brief
  ///   Creates the specified number of sky layers for this sky
  /// 
  /// Each layer is represented by an instance of class VSkyLayer and can be retrieved via
  /// GetLayer(index).
  /// 
  /// \param iCount
  ///   Number of sky layers. Should be 1..4
  EFFECTS_IMPEXP virtual void CreateLayers(int iCount) HKV_OVERRIDE;

  /// \brief
  ///   Accesses the sky layer with specified index
  /// 
  /// Any layer can be retrieved during runtime and then modified. No copy operation takes place.
  /// 
  /// \param iIndex
  ///   0-based layer index. Must be in valid range [0..GetLayerCount()-1]
  /// 
  /// \return
  ///   VSkyLayer &layer : Reference to the sky layer
  EFFECTS_IMPEXP virtual VSkyLayer& GetLayer(int iIndex) const HKV_OVERRIDE
  {
    VASSERT(iIndex>=0 && iIndex<LayerCount);
    return m_pLayers[iIndex];
  }

  /// \brief
  ///   Returns the number of layers created via CreateLayers
  EFFECTS_IMPEXP virtual int GetLayerCount() const HKV_OVERRIDE
  {
    return LayerCount;
  }

  ///
  /// @}
  ///

  ///
  /// @name Shader Effect
  /// @{
  ///


  /// \brief
  ///   Sets the shader effect for rendering the sky. Pass NULL to create the default effect.
  /// 
  /// The effect may contain sub-techniques for distinct number of sky layers. For this, the
  /// inclusion tags of the technique are used. It uses the inclusion tag "1Layer" for 1 layer sky,
  /// "2Layer" for two layers and so on. See "DefaultSky.ShaderLib" in the basedata folder.
  /// 
  /// The default technique of the effect is used if no inclusion tags are set.
  /// 
  /// \param pFX
  ///   The shader effect used to render the sky
  EFFECTS_IMPEXP virtual void SetEffect(VCompiledEffect *pFX);

  /// \brief
  ///   Returns the current shader effect. See SetEffect
  inline VCompiledEffect* GetEffect() const
  {
    return m_spEffect;
  }

  /// \brief
  ///   Returns the shader technique, which is a sub-technique of the effect. See SetEffect
  EFFECTS_IMPEXP VCompiledTechnique* GetTechnique(int iLayerCount);


  ///
  /// @}
  ///

  ///
  /// @name Sky Update
  /// @{
  ///

  /// \brief
  ///   Virtual overridable to update the sky once per frame. This implementation scrolls the
  ///   textures of the layers
  EFFECTS_IMPEXP virtual void Tick(float dtime) HKV_OVERRIDE;
  

  ///
  /// @}
  ///

  ///
  /// @name Activation (deprecated)
  /// @{
  ///
  
  
  /// \brief
  ///   Activates the sky so it is rendered in the scene.
  /// 
  /// You have to call this function after initialising.
  /// 
  /// This function just calls Vision::World.SetActiveSky(this).
  /// 
  /// \sa VSky::Deactivate
  /// \sa VisWorld_cl::GetActiveSky
  /// \sa VisWorld_cl::SetActiveSky
  EFFECTS_IMPEXP virtual void Activate() HKV_OVERRIDE;  

  
  /// \brief
  ///   Deactivates the sky.
  /// 
  /// If this sky is the active sky (Vision::World.GetActiveSky()) then this function deactivates
  /// it by calling Vision::World.SetActiveSky(NULL).
  /// 
  /// Due to refcounting paradigm, the sky might be deleted afterwards since Vision::World does not
  /// hold a reference anymore.
  /// 
  /// \sa VSky::Activate
  /// \sa VisWorld_cl::GetActiveSky
  /// \sa VisWorld_cl::SetActiveSky
  EFFECTS_IMPEXP virtual void Deactivate() HKV_OVERRIDE;


  /// \brief
  ///   Indicates whether this sky instance is the active sky
  /// 
  /// same as Vision::World.GetActiveSky()==this.
  /// 
  /// \return
  ///   BOOL: true if this instance is the active one
  /// 
  /// \sa VSky::Activate
  /// \sa VisWorld_cl::GetActiveSky
  /// \sa VisWorld_cl::SetActiveSky
  EFFECTS_IMPEXP BOOL IsActive() const;

  /// \brief
  ///   Static helper function that determines whether there is an active sky that is visible in
  ///   the current context
  static EFFECTS_IMPEXP BOOL IsVisible();

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///


  /// \brief
  ///   Serializes/deserializes the current sky configuration
  /// 
  /// This is a convenient way to serialize a snapshot of the current sky configuration, e.g. for a
  /// savegame.
  /// 
  /// When the archive is saving, all sky properties of the active sky are serialized.
  /// 
  /// When loading, the sky is recreated.
  /// 
  /// A sky instance can also be serialized on its own.
  /// 
  /// \param ar
  ///   The archive for loading/saving
  /// 
  /// \return
  ///   BOOL bResult : TRUE if serialization was successful
  EFFECTS_IMPEXP static BOOL SerializeSkyConfig(VChunkFile &ar);

  /// \brief
  ///   Static function to read a sky instance from a binary stream. The file is compatible with files written by WriteToStream and with sky setups exported by vForge. It is not compatible with SerializeSkyConfig
  EFFECTS_IMPEXP static VSky* ReadFromStream(IVFileInStream *pIn);

  /// \brief
  ///   Writes this sky instance to a binary file (see ReadFromStream). This function is used by vForge when a single instance is exported. It is not compatible with SerializeSkyConfig
  EFFECTS_IMPEXP virtual void WriteToStream(IVFileOutStream *pOut) HKV_OVERRIDE;


  /// \brief
  ///   Serializes/deserializes this sky
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Overridden function that gets the resources used for this instance.
  EFFECTS_IMPEXP virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  /// \brief
  ///   Callbacks
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  // internal functions
  V_DECLARE_SERIAL_DLLEXP( VSky, EFFECTS_IMPEXP )

  /// \brief
  ///   RTTI setup
  V_DECLARE_VARTABLE( VSky, EFFECTS_IMPEXP )


  EFFECTS_IMPEXP virtual void PrepareForRendering() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DetermineVisibleFaces() HKV_OVERRIDE;


  // variables
  int BoxSubDivisions;          ///< number of box subdivisions (detail of the sky)
  int LayerCount;               ///< number of layers

protected:
  friend class VisRenderLoopHelper_cl;
  friend class VSkyLayer;
  friend class VSkyManager;

  void CreateSkyFaceVertices(int iFace, struct SkyVertex_t *pVertices, unsigned short *pIndices);
  void SetShaderConstants(VCompiledTechnique *pTech);
  static VShaderConstantBuffer &GetSkyConstantBuffer();

  EFFECTS_IMPEXP virtual void Render() HKV_OVERRIDE;

  // layers
  VSkyLayer *m_pLayers;

  // internal status
  bool m_bDefaultEffect;
  short m_iVisibleFaces; ///< determined in visibility

  // shader
  VCompiledEffectPtr m_spEffect;
  VShaderEffectLibPtr m_spShaderLib;
  int m_iLayerTechniqueCachedMask;
  int m_iTrackPSBufferMask;
  UBYTE m_iRenderLayerIndex[MAX_SKY_LAYERS];

#ifndef _VISION_DOC
  struct CachedTechniqueInfo
  {
    VCompiledTechniquePtr spCachedTechnique;
    VConstantBufferRegister fogCoordConstantRegister;
  };
#endif // _VISION_DOC

  CachedTechniqueInfo m_cachedTechniqueInfo[MAX_SKY_LAYERS+1];

  int m_iRenderLayerCount;

  // sky mesh buffer
  VisMeshBufferPtr m_spMesh;

  // sky constant buffer
  static VShaderConstantBuffer g_GlobalConstantBufferSky;

  ///
  /// @}
  ///

};

/// \brief
///   Provides naming compatibility with the old sky class.
///
class VisSky_cl : public VSky
{
  V_DECLARE_SERIAL_DLLEXP( VisSky_cl, EFFECTS_IMPEXP )

  /// \brief
  ///   RTTI setup
  V_DECLARE_VARTABLE( VisSky_cl, EFFECTS_IMPEXP )
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
