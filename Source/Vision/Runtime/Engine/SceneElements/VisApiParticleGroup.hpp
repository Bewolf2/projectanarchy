/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiParticleGroup.hpp

#ifndef DEFINE_VISAPIPARTICLEGROUP
#define DEFINE_VISAPIPARTICLEGROUP

#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityObject.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObject3D.hpp>
#include <Vision/Runtime/Engine/Renderer/Texture/VisApiTextureAnim.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

class VisParticleGroup_cl;
typedef VSmartPtr<VisParticleGroup_cl> VisParticleGroupPtr;

class VisParticleGroupCollection_cl;

/// \brief
///   Interface to implement custom algorithms for particle group sorting. See VisParticleGroup_cl::SetParticleGroupSorter to install it
class IVisParticleGroupSorter_cl
{
public:
  inline IVisParticleGroupSorter_cl() {}
  inline virtual ~IVisParticleGroupSorter_cl() {}

  /// \brief
  ///   The key function of this interface. Implements sorting for the passed collection of visible particle groups.
  /// 
  /// \param visibleGroups : Collection of particle groups that were detected as visible by the visibility context
  ///
  /// Custom implementations should take the VisParticleGroup_cl::GetSortParticleGroups() and the VisParticleGroup_cl::GetSortParticleGroupsDistanceSorting()
  /// flag into account. For each group the VisParticleGroup_cl::SetUserSortKey value can be temporarily filled and used by a qsort function.
  /// The array passed to qsort can be visibleGroups.GetDataPtr().
  /// This function is currently always called from the main thread, but future versions might als call this from inside visibility threads.
  /// If the particle groups have an updater task (VisParticleGroup_cl::GetUpdateTask) the function should wait for this if the sorting depends on the actual
  /// simulation result.
  /// The render loop will render the particle effects in the order they appear in this list after SortGroups. To minimize artifacts, this function should
  /// thus sort the groups back-to-front.
  virtual void SortGroups(VisParticleGroupCollection_cl &visibleGroups) = 0;
};


/// \brief
///   Base Particle Group Class
/// 
/// The VisParticleGroup_cl is the base class for all particle group implementations.
/// 
/// You can derive your own particle group classes from this class in order to implement new
/// particle groups.
/// 
/// You should always specify a bounding box for your particle groups. Without a bounding box a
/// particle group will not be included in the visibility checks and will be rendered all the time.
/// 
/// Use the VisParticleGroup_cl::Init function to initialise the particle group class and the
/// VisParticleGroup_cl::ModSysNotifyFunctionHandle to update the particles per tick.
class VisParticleGroup_cl : public VisObject3D_cl, public VisElementManager_cl<VisParticleGroup_cl *>
{
public:  

  /// \brief
  ///   Enum that determines the way particles are faded out by distance. See function
  ///   VisParticleGroup_cl::SetFadeMode
  enum FadeMode_e
  {
    FADEMODE_NONE = 0,          ///< does not fade out the particles
    FADEMODE_FOGDISTANCE = 1,   ///< always uses fog distance for fading
    FADEMODE_CUSTOM = 2         ///< uses custom start and end distance (set per particle group)
  };

  /// \brief
  ///   Enum that provides bitflags for particle shader effect features. See
  ///   VisRenderLoopHelper_cl::GetParticleEffectConfig
  enum ParticleShaderFlags_e
  {
    PARTICLESHADERFLAGS_NONE              = 0x00000000,
    PARTICLESHADERFLAGS_HARDWARESPANNING  = 0x00000001, ///< the shader must provide hardware vertex spanning
    PARTICLESHADERFLAGS_SMOOTHANIMATION   = 0x00000002, ///< the shader uses animation and smooth transition between the frames
#ifndef SUPPORTS_ALPHATEST_BLENDSTATE
    PARTICLESHADERFLAGS_ALPHATESTINSHADER = 0x00000004, ///< an instruction inside the pixel shader is used to perform the alpha test
#endif
    PARTICLESHADERFLAGS_ALLFLAGS          = (PARTICLESHADERFLAGS_HARDWARESPANNING |
                                            PARTICLESHADERFLAGS_SMOOTHANIMATION
#ifndef SUPPORTS_ALPHATEST_BLENDSTATE
                                            | PARTICLESHADERFLAGS_ALPHATESTINSHADER
#endif
                                            )
  };

  ///
  /// @name Constructors / Destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor of the particle group class.
  /// 
  /// Creates an empty particle group 
  VISION_APIFUNC VisParticleGroup_cl();


  /// \brief
  ///   Destructor of the particle group class
  /// 
  /// Use the DisposeObject function instead of directly deleting the particle group.
  VISION_APIFUNC virtual ~VisParticleGroup_cl();

  /// \brief
  ///   DEPRECATED; Use DisposeObject() instead
  HKV_DEPRECATED_2012_1 inline void Remove()
  {
    DisposeObject();
  }

  /// \brief
  ///   Overridden VisTypedEngineObject_cl function to remove this instance from the scene.
  VISION_APIFUNC VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///

  ///
  /// @name Setup Functions
  /// @{
  ///

  /// \brief
  ///   Sets the stride in bytes for custom particle data stuctures
  /// 
  /// Sets the stride in bytes for custom particle stuctures.
  /// 
  /// This feature allows creating a custom particle structure with additional per-particle
  /// information.
  /// 
  /// This function has to be called before any particles are allocated with the Init function.
  /// 
  /// The custom particle structure should be derived from the default Particle_t structure, or
  /// should at least contain it as a member.
  /// 
  /// For a derived structure, pass the sizeof(MyStructure) as a stride and 0 as an offset.
  /// 
  /// If the custom structure conatains the Particle_t structure as a member variable, pass the
  /// byte offset of the member as the structure offset using the offsetof macro.
  /// 
  /// \param iStructStride
  ///   New structure stride. Must be the size of the custom structure
  /// 
  /// \param iStructOfs
  ///   Offset of the Particle_t member
  inline void SetParticleStride(short iStructStride=sizeof(Particle_t), short iStructOfs=0);

  /// \brief
  ///   Returns the struct stride in bytes. By default this is sizeof(Particle_t) but it can be modified using SetParticleStride
  inline int GetParticleStride() const
  {
    return m_iStructStride;
  }

  /// \brief
  ///   Returns the struct offset in bytes. By default this is 0 but it can be modified using SetParticleStride
  inline int GetParticleStructOfs() const
  {
    return m_iStructOfs;
  }


  /// \brief
  ///   Initializes the particle group
  /// 
  /// This function initializes the particle group, sets the default transparency
  /// (VIS_TRANSP_ADDITIVE),  the default position (0, 0, 0), disables the shading and disables the
  /// visibility check.
  /// 
  /// This method does not set the texture, use the function VisParticleGroup_cl::SetTexture to do
  /// that.
  /// 
  /// \param unused
  ///   Obsolete parameter, ignore.
  /// 
  /// \param numOfParticles
  ///   number of particle which this particle group has.
  VISION_APIFUNC void Init(int unused, int numOfParticles);

  /// \brief
  ///   This override allows for using a custom memory pointer for the simulation array
  ///
  /// \param numOfParticles
  ///   number of particles in the array
  ///
  /// \param pCustomMem
  ///   pointer to custom memory. If NULL, then this group will allocate memory. Otherwise the user is responsible for freeing the memory.
  VISION_APIFUNC void Init(int numOfParticles, void *pCustomMem=NULL);

  /// \brief
  ///   Modifies the custom memory pointer of the particle simulation array
  ///
  /// The user is responsible for allocating and freeing an array that is large enough. That is: GetParticleStride() * GetNumOfParticles() bytes.
  /// This function waits for the updater task to finish.
  ///
  /// \param pCustomMem
  ///   pointer to custom memory. The pointer must not be NULL.
  ///
  /// \param bCopyArray
  ///   Optionally copies from old to new memory block to preserve the simulation state.
  VISION_APIFUNC void SetCustomParticleMemory(void *pCustomMem, BOOL bCopyArray=FALSE);

  /// \brief
  ///   Sets a flag to have the particle group not do any zbuffer checking
  /// 
  /// Use this function to have particles alway render in fron of everything else, effectively
  /// disabling the zbuffer checking of the rendering. You can use this function to have particles
  /// render as some kind of overlays. The default setting is FALSE, meaning not always in
  /// foreground.
  /// 
  /// \param status
  ///   TRUE/FALSE
  inline void SetAlwaysInForeGround (BOOL status);


  /// \brief
  ///   Sets the texture for the particle group
  /// 
  /// SetTexture sets the particle group for this texture and checks automatically whether the
  /// texture already exists. This function already requires the transparency to be set; if you
  /// want to change the transparency after loading the texture you will have to call the
  /// SetTexture function again.
  /// 
  /// \param textureFile
  ///   filename of the texture file
  VISION_APIFUNC void SetTexture(const char *textureFile);

  /// \brief
  ///   Sets the texture for the particle group.
  /// 
  /// Sets the texture object used for rendering the particles.
  /// 
  /// This texture is the "base texture" when rendering with a shader.
  /// 
  /// This function will also test if the texture is animated.
  /// 
  /// There are multiple ways to obtain a texture object, for instance via
  /// VisTextureManager_cl::Load2DTexture
  /// 
  /// \param pTexture
  ///   Texture object to use
  /// 
  /// \sa VTextureObject
  /// \sa VisTextureManager_cl::Load2DTexture
  /// \sa VisParticleGroup_cl::GetTextureObject
  /// \sa VisParticleGroup_cl::SetTexture
  VISION_APIFUNC void SetTextureObject(VTextureObject *pTexture);

  /// \brief
  ///   Sets a normalmap texture object that is tracked when the particle group uses a shader with
  ///   a Normalmap sampler.
  inline void SetNormalmap(VTextureObject *pTexture)
  {
    spNormalmap = pTexture;
    UpdateDefaultShaderFlags();
  }

  /// \brief
  ///   Returns the normal map texture object
  inline VTextureObject* GetNormalmap() const
  {
    return spNormalmap;
  }


  /// \brief
  ///   Sets a specular texture object that is tracked when the particle group uses a shader with a
  ///   Specularmap sampler.
  inline void SetSpecularmap(VTextureObject *pTexture)
  {
    spSpecularmap = pTexture;
  }

  /// \brief
  ///   Returns the normal map texture object
  inline VTextureObject* GetSpecularmap() const
  {
    return spSpecularmap;
  }

  /// \brief
  ///   Set the number of texture repetitions
  /// 
  /// SetTextureRepetitions sets the number of texture repetitions within one particle. By default
  /// the  number of texture repetitions is set to zero, which means that the particle texture will
  /// be stretched along the whole particle. The texture repetitions only works for stretched
  /// particles with an enabled distortion vector, textures of particles without distortion won't
  /// be repeated. 
  /// 
  /// \param numOfRepetitions
  ///   number of texture repetitions for one particle
  inline void SetTextureRepetitions(float numOfRepetitions = 1.0f);

  /// \brief
  ///   Sets a size aspect ratio for non square particle textures
  /// 
  /// In case a non square texture is used, it might be useful to display the screen-aligned
  /// particle as non-square also. This function allows the programmer to set the aspect manually.
  /// However, the actual size ratio of the texture is ignored; by default all textures are
  /// displayed as squares.
  /// 
  /// \param fAspectRatio
  ///   The ratio between height and width of the particle.  For instance use 0.5f if you have a
  ///   256x128 texture
  inline void SetParticleSizeAspectRatio(float fAspectRatio = 1.f);

  /// \brief
  ///   Subdivides a single particle texture into animation frames
  /// 
  /// For instance, if you have a texture with 16 animation frames in it which are evenly aligned
  /// in a 4x4 grid, you can set 4x4 subdivisions and assign a per-particle animation frame.
  /// 
  /// The Particle_t::animation_frame member is the 0-based index of the animation frame.
  /// 
  /// If no subdivisions are specified, the Particle_t::animation_frame member is ignored.
  /// 
  /// The engine will set up the texture coordinates properly for each particle.
  /// 
  /// This is a straightforward solution for animating particles (Note that an animated texture
  /// applies for the whole group)
  /// 
  /// \param iSubDivX
  ///   number of animation frames in x-direction of the texture
  /// 
  /// \param iSubDivY
  ///   number of animation frames in y-direction of the texture
  /// 
  /// \note
  ///   The animation frames must be evenly aligned on a grid.
  /// 
  /// \note
  ///   The grid has to fill the complete texture.
  /// 
  /// \note
  ///   The number of subdivisions does not have to be a power of two. For instance 3x5 subdivsions
  ///   are allowed.
  inline void SetAnimationSubdivisions(int iSubDivX,int iSubDivY);
  
  /// \brief
  ///   Defines whether the pixel shader should use smooth interpolation between animation frames
  /// 
  /// This flag is ignored if the particle effect does not use animation subdivisions (see
  /// SetAnimationSubdivisions)
  /// 
  /// The default status is false. Note that smooth interpolation costs additional pixel shader
  /// instructions which is critical for particles.
  ///
  /// \param bStatus
  ///   true to use smooth animation fading, otherwise false.
  inline void SetUseSmoothAnimationFading(BOOL bStatus);

  /// \brief
  ///   Returns the status previously set with SetUseSmoothAnimationFading
  inline BOOL GetUseSmoothAnimationFading() const;

  /// \brief
  ///   Set the transparency type of the particle group
  /// 
  /// This function sets the transparency type of the particle group. Some transparency types
  /// require the texture to be loaded in a certain way, which means that you might have to call
  /// the SetTexture again after you've changed the transparency.
  /// 
  /// \param newTransparencyType
  ///   transparency type of the particle group
  inline void SetTransparencyType(VIS_TransparencyType newTransparencyType);


  /// \brief
  ///   Set the depth offset for the particle group
  /// 
  /// This value moves each particle towards the camera (positive values) which affects the z-buffer test for the particles.
  /// That way artifacts caused by particles cutting the geometry can be reduced. The default value is 0.0
  /// 
  /// \param fScale
  ///   Fraction of the distance to subtract. Accordingly this value must be in -1..1 range, but only small values (e.g. 0.05) actually make sense.
  ///   Positive values move the particles towards the camera.
  inline void SetDepthOffset(float fScale)
  {
    VASSERT(fScale>-1.f && fScale<1.f);
    m_fDepthOffset = fScale;
  }

  /// \brief
  ///   Returns the value previously set with SetDepthOffset
  inline float GetDepthOffset() const
  {
    return m_fDepthOffset;
  }

  /// \brief
  ///   If enabled, particle group rendering tracks the transformation of this object rather than world space transformation matrix
  ///
  /// \param bStatus
  ///   New status. Default is FALSE
  ///
  /// If enabled, particles are considered in local space during rendering. The shader needs to consider this matrix additionally
  /// If disabled (which is default) then relevant particle groups are rendered with identity model matrix thus ussuming
  /// particles are in world space.
  inline void SetUseLocalSpaceMatrix(BOOL bStatus)
  {
    m_bTrackModelViewMatrix = (bStatus==TRUE);
  }

  /// \brief
  ///   Returns the value previously set with SetUseLocalSpaceMatrix. Default is FALSE
  inline BOOL GetUseLocalSpaceMatrix() const
  {
    return m_bTrackModelViewMatrix;
  }


  ///
  /// @}
  ///

  ///
  /// @name Particle Group Visibility
  /// @{
  ///

  /// \brief
  ///   Set the visibility flag of the particle group
  /// 
  /// This function sets the visibility status of the particle group. If the visibility check is
  /// set to TRUE then the visibility calculations will be applied to the particle group, otherwise
  /// the particle group will always be drawn.
  /// 
  /// As soon as the VisParticleGroup_cl::SetBoundingBox function has been called, the visibility
  /// check will automatically be enabled.
  /// 
  /// Internally, the particle group creates a visibility object of class
  /// VisVisibilityObjectAABox_cl (axis aligned bounding box) for performing the visibility test
  /// 
  /// \param checkVis
  ///   visibility flag of the particle group
  /// 
  /// \sa VisParticleGroup_cl::IsVisible
  /// \sa VisParticleGroup_cl::SetBoundingBox
  /// \sa VisParticleGroup_cl::SetVisibilityObject
  VISION_APIFUNC void SetCheckVisibility(BOOL checkVis);


  /// \brief
  ///   Set the bounding box of the particle group
  /// 
  /// SetBoundingBox sets the bounding box of the particle group which will be used for the
  /// visibility checks. It also automatically enables the visibility checks.
  /// 
  /// This function sets the global bounding box in world space
  /// 
  /// \param minX
  /// 
  /// \param minY
  /// 
  /// \param minZ
  /// 
  /// \param maxX
  /// 
  /// \param maxY
  /// 
  /// \param maxZ
  /// 
  /// \sa VisParticleGroup_cl::GetBoundingBox
  inline void SetBoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

  /// \brief
  ///   Set the bounding box of the particle group
  /// 
  /// SetBoundingBox sets the bounding box of the particle group which will be used for the
  /// visibility checks. It also automatically enables the visibility checks.
  /// 
  /// This function sets the global bounding box in world space
  /// 
  /// \param bbox
  ///   Reference to the new bounding box
  /// 
  /// \sa VisParticleGroup_cl::GetBoundingBox
  VISION_APIFUNC void SetBoundingBox(const hkvAlignedBBox &bbox);


  /// \brief
  ///   Set a visibility object
  /// 
  /// Set a new visibility object that should be used for visibility checks of this group.
  /// 
  /// The function will increase the reference count of the visibility object. Thus it is possible
  /// to share the same visibility object across multiple groups.
  /// 
  /// If a NULL pointer is passed, visibility will be disabled for this group
  /// 
  /// \param pVisObj
  ///   pointer to the new visibility object
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisParticleGroup_cl::GetVisibilityObject
  /// \sa VisParticleGroup_cl::GetCheckVisibility
  /// \sa VisParticleGroup_cl::SetBoundingBox
  void SetVisibilityObject(VisVisibilityObject_cl* pVisObj);

  /// \brief
  ///   Get the visibility object
  /// 
  /// that is used for visibility checks of this group.
  /// 
  /// This function will return a NULL pointer, if visibility is currently not used for this group.
  /// 
  /// \return
  ///   VisVisibilityObject_cl* pVisObj : pointer to the current visibility object
  /// 
  /// \sa VisVisibilityObject_cl
  /// \sa VisParticleGroup_cl::SetVisibilityObject
  /// \sa VisParticleGroup_cl::SetCheckVisibility
  /// \sa VisParticleGroup_cl::SetBoundingBox
  VisVisibilityObject_cl* GetVisibilityObject() const;



  ///
  /// @}
  ///

  ///
  /// @name Show/hide
  /// @{
  ///

  /// \brief
  ///   Set the render order flag for rendering in a certain render pass
  /// 
  /// By default, particles are rendered in the VRH_PARTICLES pass or, in case of
  /// wallmarks, in the VRH_DECALS pass.
  /// 
  /// \param iOrder
  ///   The new render order. A value of VRenderHook_e
  /// 
  /// \sa VisRenderLoopHelper_cl::RenderParticleSystems
  VISION_APIFUNC void SetRenderOrder(unsigned int iOrder);

  /// \brief
  ///   Get the render order that has been set via SetRenderOrder
  /// 
  /// \return
  ///   int iOrder : The current render order
  /// 
  /// \sa VisRenderLoopHelper_cl::RenderParticleSystems
  unsigned int GetRenderOrder() const;

  /// \brief
  ///   Set the filter bitmask for rendering only in some contexts
  /// 
  /// This particle group will only be rendered in render contexts, if the logical AND result of
  /// this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) is not
  /// zero.
  /// 
  /// By default, all bis are set so it will be rendered in all contexts with a non-zero bitmask.
  /// 
  /// \param iBitmask
  ///   The new filter bitmask
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline void SetVisibleBitmask(unsigned int iBitmask);

  /// \brief
  ///   Get the filter bitmask that has been set via SetVisibleBitmask
  /// 
  /// \return
  ///   int iBitmask : The current bitmask
  /// 
  /// \sa VisRenderContext_cl::SetRenderFilterMask
  inline unsigned int GetVisibleBitmask() const;


  ///
  /// @}
  ///

  ///
  /// @name Access Particles
  /// @{
  ///

  /// \brief
  ///   Get the overall number of particles
  /// 
  /// GetNumOfParticles returns the overall number of particles which this particle group has. The
  /// number of particles can be set with the VisParticleGroup_cl::Init function during the
  /// initialisation of the particle group.
  /// 
  /// \return
  ///   int numOfParticles
  inline int GetNumOfParticles() const;


  /// \brief
  ///   Get a pointer to the list of particle structures
  /// 
  /// GetParticles returns a pointer the the list of particle structure and points to the first
  /// particle. Use this function to get access to the particle of the particle group.
  /// 
  /// \return
  ///   Particle_t *firstP: pointer to the list particle structure which is also the pointer to the
  ///   first particle
  /// 
  /// \note
  ///   You can easily access the other particles by incrementing the returned pointer by 1 for
  ///   each particle
  inline Particle_t *GetParticles() const;



  ///
  /// @}
  ///

  ///
  /// @name Info Functions
  /// @{
  ///

  /// \brief
  ///   Get the bounding box of the particle group
  /// 
  /// GetBoundingBox allows you to get the bounding box of the particle group which is used for
  /// visibility checks. In case there is no bounding box defined for this group, then FALSE will
  /// be returned and the bounding box variables won't be set.
  /// 
  /// This function returns the global bounding box in world space
  /// 
  /// \param minX
  ///   minimum x value of the bounding box
  /// 
  /// \param minY
  ///   minimum y value of the bounding box
  /// 
  /// \param minZ
  ///   minimum z value of the bounding box
  /// 
  /// \param maxX
  ///   maximum x value of the bounding box
  /// 
  /// \param maxY
  ///   maximum y value of the bounding box
  /// 
  /// \param maxZ
  ///   maximum z value of the bounding box
  /// 
  /// \return
  ///   BOOL bBoxDefined: if TRUE then a bounding is defined for this particle group. If FALSE then
  ///   there is no bounding box and the bounding box variables won't be set.
  /// 
  /// \sa VisParticleGroup_cl::SetBoundingBox
  VISION_APIFUNC BOOL GetBoundingBox(float &minX, float &minY, float &minZ, float &maxX, float &maxY, float &maxZ) const;

  
  /// \brief
  ///   Get the name of the texture which has been applied to the particle group
  /// 
  /// GetTexture returns the filename of the texture which has been applied to the particle group.
  /// 
  /// \return
  ///   const char *textureName: filename of the texture (full path, with extension)
  inline const char *GetTexture() const;
  
  /// \brief
  ///   Returns the texture object of the group
  /// 
  /// The texture has either been set via SetTexture(filename) of via SetTextureObject.
  /// 
  /// \return
  ///   VTextureObject* pTexture: The current texture object.
  inline VTextureObject* GetTextureObject() const {return spTexture;}
  
  /// \brief
  ///   Get the transparency type of the particle group
  /// 
  /// \return
  ///   VIS_TransparencyType transparencyType: transparency type of the particle group
  inline VIS_TransparencyType GetTransparencyType() const;

  /// \brief
  ///   Get the visibility check flag of the particle group
  /// 
  /// GetCheckVisibility returns the current status of the visibility flag of the particle group.
  /// This flag can be set with the VisParticleGroup_cl::SetCheckVisibility function and defines
  /// whether visibility calculations will be performed for the particle group or whether the
  /// particle group will always be drawn.
  /// 
  /// \return
  ///   BOOL checkVisFlag: if TRUE then visibility calculations will be done for the particle group
  inline BOOL GetCheckVisibility() const;
  
  /// \brief
  ///   Returns the actual visibility status of the particle group
  /// 
  /// A hidden particle group is never visible.
  /// 
  /// A group marked as "always in foreground" is always visible.
  /// 
  /// Only groups with a VisibilityObject assigned actually perform a visibility test.
  /// 
  /// The VisibilityObject to be used can either be manually assigned or is created in the
  /// SetBoundingBox function.
  /// 
  /// This function does not consider the render filter mask of the current context (see
  /// SetVisibleBitmask)
  /// 
  /// \return
  ///   BOOL isVisible: if TRUE the particle group is currently visible
  /// 
  /// \sa VisParticleGroup_cl::SetBoundingBox
  /// \sa VisParticleGroup_cl::SetVisibilityObject
  /// \sa VisParticleGroup_cl::SetVisibleBitmask
  VISION_APIFUNC BOOL IsVisible() const;

  /// \brief
  ///   Returns whether this particle group is visible in any active render context
  /// 
  /// A hidden particle group is never visible.
  /// 
  /// A group marked as "always in foreground" is always visible.
  /// 
  /// Only groups with a VisibilityObject assigned actually perform a visibility test.
  /// 
  /// The VisibilityObject to be used can either be manually assigned or is created in the
  /// SetBoundingBox function.
  /// 
  /// This function does not consider the render filter mask of the contexts (see
  /// SetVisibleBitmask)
  /// 
  /// \return
  ///   BOOL isVisible: if TRUE then the particle group is currently hidden
  /// 
  /// \sa VisParticleGroup_cl::Hide
  /// \sa VisParticleGroup_cl::Show
  /// \sa VisParticleGroup_cl::SetBoundingBox
  /// \sa VisParticleGroup_cl::SetVisibilityObject
  /// \sa VisParticleGroup_cl::SetVisibleBitmask
  /// \sa VisParticleGroup_cl::IsVisible
  VISION_APIFUNC BOOL IsVisibleInAnyContext() const;


  ///
  /// @}
  ///

  ///
  /// @name Overridden Child Functions
  /// @{
  ///
  
  /// \brief
  ///   Virtual handle function of the module system
  /// 
  /// This function is the virtual handle function of the module system. Since the particle group
  /// is part of the Vision module system the virtual functions of the module system will be called
  /// by the parent entity. The handle function will be called by the parent entity once per frame
  /// to allow you doing regular updates. Use this function to update, move and change your
  /// particles.
  /// 
  /// Note that you still have to call the base implementation when overriding this function
  VISION_APIFUNC virtual void ModSysNotifyFunctionHandle() {}


  /// \brief
  ///   Virtual info function of the module system
  /// 
  /// This function is another virtual function of the module system which will be called whenever
  /// the parent's position or orientation changes. You should have a look at the
  /// VisModuleSystemChild_cl for more information about this function. Use this function to keep
  /// track of the position and orientation of the parent entity.
  /// 
  /// Note that you still have to call the base implementation when overriding this function
  /// 
  /// \param flags
  ///   Status flags which tell you which values of the parent entity have been changed (see
  ///   VisModuleSystemChild_cl::ModSysNotifyFunctionParentAltered function)
  VISION_APIFUNC virtual void ModSysNotifyFunctionParentAltered(int flags) { VisObject3D_cl::ModSysNotifyFunctionParentAltered(flags); }






  ///
  /// @}
  ///

  ///
  /// @name Texture Animations
  /// @{
  ///

  /// \brief
  ///   Trigger the texture animation of the particle texture
  /// 
  /// This function triggers the animated particle texture; this is normally used to halt and
  /// continue the texture animation.
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline void TriggerTextureAnim();
  
  
  /// \brief
  ///   Reset the texture animation of the particle texture
  /// 
  /// This function resets the animated particle texture.
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline void ResetTextureAnim();
  
  
  /// \brief
  ///   Get the number of animation phases for the animated particle texture
  /// 
  /// This function returns the number of animation phases the animated particle texture has.
  /// 
  /// \return
  ///   int numOfPhases
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline int GetTextureAnimNumOfPhases();
  
  
  /// \brief
  ///   Get the current animation phase of the texture animation
  /// 
  /// This function returns the current animation phase of the particle texture animation.
  /// 
  /// \return
  ///   float currentPhase: current animation phase, beginning with #0. The phase might be
  ///   interpolated.
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline float GetTextureAnimPhase();


  /// \brief
  ///   Set the current animation phase of the texture animation
  /// 
  /// This function sets the current animation phase of the particle texture animation.
  /// 
  /// \param newPhase
  ///   new animation phase, has to be between #0 and #(numOfPhases)
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline void SetTextureAnimPhase(float newPhase);
  
  
  /// \brief
  ///   Increase the current animation phase of the texture animation
  /// 
  /// This function increases the animation phase of the particle texture animation by the given
  /// value.
  /// 
  /// \param phaseDelta
  ///   animation phase delta which will be added to the current animation phase
  /// 
  /// \note
  ///   This function is obsolete since you can get the animation instance via
  ///   GetTextureAnimationInstance and use the VisTextureAnimInstance_cl class to trigger texture
  ///   animations
  inline void IncTextureAnimPhase(float phaseDelta);

  /// \brief
  ///   Get the texture animation instance ID of the animated particle system texture
  /// 
  /// If the texture of this group is not animated, this function will return 0
  /// 
  /// The returned handle can be used as a constructor for the VisTextureAnimInstance_cl class
  /// 
  /// Using the VisTextureAnimInstance_cl with this ID makes all the texture animation related
  /// functions of this class obsolete.
  /// 
  /// \return
  ///   int iTexAnimID : Texture animation instance ID of the animated texture, or 0 if texture is
  ///   not animated
  /// 
  /// \sa VisTextureAnimInstance_cl
  inline VisTextureAnimInstance_cl* GetTextureAnimationInstance() const;

  ///
  /// @}
  ///

  ///
  /// @name Sorting Groups
  /// @{
  ///

  /// \brief
  ///   Overridable function to retrieve the distance that is used for distance-based back to front particle group sorting
  ///
  /// This function is only called when both global flags VisParticleGroup_cl::SetSortParticleGroups and VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting are enabled.
  /// This default implementation returns the distance between camera position and the groups position (GetPosition()). The higher level
  /// particle effect system overrides this function to use the common effect pivot of the parent effect instead.
  /// Particle groups are sorted back to front using this distance value as a reference. Note that particle groups are only sorted within the same draw batch that can be set via
  /// VisParticleGroup_cl::SetRenderOrder. That means groups in the VRH_PARTICLES batch are always rendered after VRH_DECALS, regardless of the
  /// distance. Also note that this type of sorting is not related to sorting particles within one group
  ///
  /// \param pCamera
  ///   The camera object to test the distance
  ///
  /// \returns
  ///   The distance of this particle group. This distance is used for sorting particle effects in the same draw batch (VisParticleGroup_cl::SetRenderOrder)
  ///
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::SetRenderOrder
  VISION_APIFUNC virtual float GetSortingDistance(const VisObject3D_cl *pCamera);

  /// \brief
  ///   Static function to globally enable or disable sorting of particle groups
  /// 
  /// If enabled, particle groups are either sorted back to front according to camera distance or
  /// sorted by a user sort key.
  /// 
  /// The user should set the sorting type by calling the SetSortParticleGroupsDistanceSorting()
  /// function.
  /// 
  /// \param bStatus
  ///   New sorting status.
  /// 
  /// \note
  ///   This flag is only relevant when the default particle group sorter is used. It might not have any
  ///   effect in custom algorithms that have been hooked via SetParticleGroupSorter
  ///
  /// \note
  ///   This flag only affects sorting of all groups. Single particles in a particle group may be
  ///   sorted with the Particle Sorting functions.
  /// 
  /// \note
  ///   By default, sorting of particle groups is enabled.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::GetSortParticleGroups
  VISION_APIFUNC static void SetSortParticleGroups(BOOL bStatus);

  /// \brief
  ///   Static function get the global sorting flag
  /// 
  /// \return
  ///   BOOL bStatus : The current sorting status.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  VISION_APIFUNC static BOOL GetSortParticleGroups();

  /// \brief
  ///   Static function to globally enable distance sorting or user sorting of particle groups
  /// 
  /// If TRUE, particle groups are sorted back to front according to camera distance.
  /// 
  /// To determine the camera distance, the sorting routine needs a bounding box of the particle
  /// group.
  /// 
  /// The user is responsible to set the center properly by overriding the GetSortingDistance()
  /// function.
  /// 
  /// If FALSE, particle groups are sorted by a user sort key which the user should set with the
  /// SetUserSortKey() function.
  /// 
  /// \param bDistanceSort
  ///   New sorting type.
  /// 
  /// \note
  ///   This flag is only relevant when the default particle group sorter is used. It might not have any
  ///   effect in custom algorithms that have been hooked via SetParticleGroupSorter
  ///
  /// \note
  ///   This flag only affects sorting of all groups. Single particles in a particle group may be
  ///   sorted with the Particle Sorting functions.
  /// 
  /// \note
  ///   By default, distance sorting of particle groups is <b>enabled</b>.
  /// 
  /// \sa VisParticleGroup_cl::GetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::GetSortingDistance
  /// \sa VisParticleGroup_cl::SetUserSortKey
  VISION_APIFUNC static void SetSortParticleGroupsDistanceSorting(bool bDistanceSort);

  /// \brief
  ///   Static function to get the global sorting type flag
  /// 
  /// \return
  ///   bool bDistanceSorting : The current sorting type.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::GetSortingDistance
  /// \sa VisParticleGroup_cl::SetUserSortKey
  VISION_APIFUNC static bool GetSortParticleGroupsDistanceSorting();


  /// \brief
  ///   Static function to globally install a custom particle group sorter
  ///
  /// \param pSorter
  ///   Custom sorter instance that implements IVisParticleGroupSorter_cl::SortGroups. If this pointer is NULL then the default
  ///   sorter is installed
  ///
  /// A custom sorter instance can be globally installed with this function. This sorter is called by the default render loop for every
  /// context that is rendered. The default sorter uses the global GetSortParticleGroups()/GetSortParticleGroupsDistanceSorting() flags
  /// to sort particle groups (or not); custom classes can override this default behavior.
  /// Sorting particle groups against each other is a non-trivial matter in order to avoid rendering artifacts. A custom sorter allows
  /// for taking control over this process and use sophisticated algorithms.
  VISION_APIFUNC static void SetParticleGroupSorter(IVisParticleGroupSorter_cl *pSorter);

  /// \brief
  ///   Static function that returns the global sorter instance. This reference is always non-null and returns a default sorter instance
  VISION_APIFUNC static IVisParticleGroupSorter_cl& GetParticleGroupSorter();

  /// \brief
  ///   Sets the user sort key for this particle group
  /// 
  /// This is only useful when particle group sorting is switched on with the SetSortParticleGroups
  /// function and distance sorting is switched <b>off</b> with the
  /// SetSortParticleGroupsDistanceSorting function.
  /// 
  /// The higher the sort key, the later the group is rendered.
  /// 
  /// \param iUserSortKey
  ///   New sorting key.
  /// 
  /// \note
  ///   By default, the user sort key is set to 0.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::GetUserSortKey
  inline void SetUserSortKey(int iUserSortKey);

  /// \brief
  ///   Gets the user sort key for this particle group
  /// 
  /// \return
  ///   int iUserSortKey : The current user sort key.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticleGroups
  /// \sa VisParticleGroup_cl::SetSortParticleGroupsDistanceSorting
  /// \sa VisParticleGroup_cl::SetUserSortKey
  inline int GetUserSortKey();

  /// \brief
  ///   Specifies an update (simulation) task for this particle group
  /// 
  ///  Update tasks can be used to easily multi-thread the particle simulation: A VThreadedTask
  /// object takes care of asynchronously updating the particle data. Setting the update task via
  /// this method allows the renderer to wait for the update task to finish before starting to
  /// render the resulting data.
  /// 
  /// By default, no update task is specified. In this case, the renderer will process the particle
  /// data for rendering independently of the simulation state. This is not recommended when using
  /// multithreading.
  /// 
  /// \param pTask
  ///   The update task, or NULL if no update task is used (default)
  VISION_APIFUNC void SetUpdateTask(VThreadedTask *pTask);

  /// \brief
  ///   Returns the update (simulation) task for this particle group
  /// 
  ///  Update tasks can be used to easily multi-thread the particle simulation.
  /// 
  /// By default, no update task is specified. In this case, this method will return NULL.
  /// 
  /// \return
  ///   VThreadedTask *: The update task, or NULL if no update task is used (default)
  /// 
  /// \sa VisParticleGroup_cl::SetUpdateTask
  VISION_APIFUNC VThreadedTask *GetUpdateTask() const;

  /// \brief
  ///   Specifies whether the particle normals should be used for aligning particles
  /// 
  /// If set to false, all particles of this group will be screen-aligned.
  /// 
  /// If set to true, all particles of this group will be aligned using their respective normal
  /// vector.
  /// 
  /// \param bStatus
  ///   true for normal alignment, false for screen alignment
  VISION_APIFUNC void SetUseNormals(bool bStatus);

  /// \brief
  ///   Returns whether the particle normals should be used for aligning particles
  /// 
  /// If false, all particles of this group are screen-aligned.
  /// 
  /// If true, all particles of this group are aligned using their respective normal vector.
  /// 
  /// \return
  ///   bool: true for normal alignment, false for screen alignment
  VISION_APIFUNC bool GetUseNormals() const;

  /// \brief
  ///   Specifies whether the particles' distortion vector should be taken into account
  /// 
  /// If set to false, all particles of this group will have the aspect ratio of their texture, and
  /// they will be 2D-aligned to the x and y screen coordinates.
  /// 
  /// If set to true, all particles of this group will have the aspect ratio and orientation of
  /// their distortion vector.
  /// 
  /// By default, the center of distorted particles is on the center of the left edge of the quad
  /// as shown in the documentation. The horizontal alignment of the pivot can be influenced by the
  /// value passed to SetHorizontalDistortionOffset.
  /// 
  /// \param bStatus
  ///   true if distortion vector should be used, otherwise false.
  inline void SetUseDistortion(bool bStatus);

  /// \brief
  ///   Returns whether the particles' distortion vector should be taken into account
  /// 
  /// If false, all particles of this group have the aspect ratio of their texture, and they are
  /// 2D-aligned to the x and y screen coordinates.
  /// 
  /// If true, all particles of this group have the aspect ratio and orientation of their
  /// distortion vector.
  /// 
  /// \returns
  ///   bool bUseDistortion: true if distortion vector is used, otherwise false.
  inline bool GetUseDistortion() const;


  /// \brief
  ///   Sets a new relative center pivot position for particles (stretched and billboards)
  /// 
  /// If the particle rotates, this defines the rotation center for instance.
  /// 
  /// The default values are (0.5,0.5) which is the center of the particle. (0,0) defines the upper
  /// left corner.
  /// 
  /// When enabling stretched particles (SetUseDistortion(TRUE)) the center resets to (0,0.5),
  /// which is the center of the left edge.
  /// 
  /// This is to maintain backwards compatibility.
  /// 
  /// \param x
  ///   New relative center in x direction
  /// 
  /// \param y
  ///   New relative center in y direction
  inline void SetParticleCenter(float x, float y);

  /// \brief
  ///   Returns the value pair previously set with SetParticleCenter
  inline void GetParticleCenter(float &x, float &y) const;

  /// \brief
  ///   Specifies whether the particle vertices should be computed in the vertex shader or in
  ///   software
  /// 
  /// If set to true, the particles vertices will be fully computed in a vertex shader. Hardware
  /// instancing will additionally be used if avaialble.
  /// 
  /// Hardware spanning yields up to five times better performance than software spanning.
  /// 
  /// When using hardware spanning in conjunction with a custom particle shader, your vertex shader
  /// has to make sure that the particles' vertex positions are correctly calculated. Please refer
  /// to the respective documentation and shader code for details on how to do this.
  /// 
  /// \param bStatus
  ///   true if hardware spanning should be used, otherwise false.
  VISION_APIFUNC void SetHardwareSpanning(bool bStatus);

  /// \brief
  ///   Returns whether the particle vertices are computed in the vertex shader or in software
  /// 
  /// If true, the particles vertices are fully computed in a vertex shader. 
  /// 
  /// This feature is currently only supported on Xbox360, it will be ignored on PC.
  /// 
  /// \return
  ///   bool: true if hardware spanning is used, otherwise false.
  inline bool GetHardwareSpanning() const;

  /// \brief
  ///   Associates a mesh buffer with this particle group for 'geometry particles'
  ///
  /// If a mesh is specified, then the mesh will be rendered for every particle instance. The mesh buffer can be extracted from a model file or static mesh.
  /// See high level particle system implementation in the VisionEnginePlugin for sample code.
  /// Although instancing is used for rendering the single geometry particles, a single effect can have significant impact on performance due to enourmous amount of vertices.
  /// Accordingly only small meshes should be used.
  /// This function only extracts the mesh itself. Textures, shaders, blend mode etc. must be set on this particle group.
  /// The following particle properties apply to the mesh rendering 
  ///   \li Particle_t::pos : mesh pivot position
  ///   \li Particle_t::size : relative scaling of the mesh
  ///   \li Particle_t::normal : rotation axis (must be normalized)
  ///   \li Particle_t::color : tint color (including alpha)
  ///   \li Particle_t::angle : rotation angle (rad) around the rotation axis
  ///   \li Particle_t::animation_frame : shaders might use this to apply a texture animation to the mesh
  ///
  /// \param pGeometry: Mesh that should be used for the particles. NULL can be passed to switch back to billboard rendering
  VISION_APIFUNC void SetGeometry(VisMeshBuffer_cl *pGeometry);

  /// \brief
  ///   Returns the mesh buffer that has been set via SetGeometry
  inline VisMeshBuffer_cl* GetGeometry() const
  {
    return m_spGeometry;
  }

  ///
  /// @}
  ///

  ///
  /// @name Sorting Particles
  /// @{
  ///

  /// \brief
  ///   Allocates memory for a particle indexing list that is used by the renderer to determine the
  ///   render order for the particles.
  /// 
  /// The particle indexing list is an array of ParticleSort_t structs. Its size is set to the
  /// number of particles in this group, up to a maximum of 65536.
  /// 
  /// If the bCreate flag is set then the previous index list (if any) is deallocated and a new
  /// ParticleSort_t array is created. If it's false then the pointer is set to NULL.
  /// 
  /// In derived classes the index list should be automatically handled if the sort particles flag
  /// isd set to TRUE. In all other cases, the user is responsible for filling the list with
  /// values. Any index pointing to non-existent particles will result in unpredictable behavior.
  /// 
  /// \param bCreate
  ///   If TRUE, creates a new index list. Otherwise, it just deallocates the current index list if
  ///   it exists.
  /// 
  /// \note
  ///   By default, no index list is allocated.
  /// 
  /// \sa VisParticleGroup_cl::GetParticleIndexList
  /// \sa VisParticleGroup_cl::SetSortParticles
  /// \sa VisParticleGroup_cl::GetSortParticles
  VISION_APIFUNC void InitParticleIndexList(bool bCreate = true);

  /// \brief
  ///   Gets the pointer to the particle index list, or NULL if it wasn't initialized.
  /// 
  /// \return
  ///   ParticleSort_t *: The particle index list, or NULL if it wasn't initialized.
  /// 
  /// \sa VisParticleGroup_cl::InitParticleIndexList
  /// \sa VisParticleGroup_cl::SetSortParticles
  /// \sa VisParticleGroup_cl::GetSortParticles
  VISION_APIFUNC ParticleSort_t *GetParticleIndexList() const;

  /// \brief
  ///   Enables or disables automatic sorting of particles.
  /// 
  /// This flag does nothing in this class. Derived classes should handle the sorting when this
  /// flag is enabled.
  /// 
  /// Derived classes should fill the particle index list with the sorted particle indices.
  /// 
  /// The index list may not be set by the user if this flag is enabled.
  /// 
  /// \param bStatus
  ///   New sorting status.
  /// 
  /// \note
  ///   This flag only affects particle sorting. Particle groups can be sorted with the Group
  ///   Sorting Functions.
  /// 
  /// \note
  ///   By default, sorting of particles is disabled.
  /// 
  /// \sa VisParticleGroup_cl::GetSortParticles
  /// \sa VisParticleGroup_cl::InitParticleIndexList
  /// \sa VisParticleGroup_cl::GetParticleIndexList
  VISION_APIFUNC void SetSortParticles(bool bStatus);

  /// \brief
  ///   Gets the particle sorting flag
  /// 
  /// \return
  ///   bool bStatus : The current sorting status.
  /// 
  /// \sa VisParticleGroup_cl::SetSortParticles
  /// \sa VisParticleGroup_cl::InitParticleIndexList
  /// \sa VisParticleGroup_cl::GetParticleIndexList
  VISION_APIFUNC bool GetSortParticles() const;


  ///
  /// @}
  ///

  ///
  /// @name Assigning Shaders
  /// @{
  ///

  /// \brief
  ///   Sets the shader technique with this particle group is to be rendered
  /// 
  /// Passing NULL will result in the particle group being rendered with the default shader
  /// (texture or texture*color, default blending). This is also the default behavior.
  /// 
  /// \param pTechnique
  ///   Technique to assign
  VISION_APIFUNC inline void SetTechnique(VCompiledTechnique *pTechnique);

  /// \brief
  ///   Returns the technique previously set with SetTechnique
  /// 
  /// NULL is returned if no shader technique is assigned to this particle group.
  /// 
  /// \return
  ///   VCompiledTechnique *pTechnique: Technique to assign
  inline VCompiledTechnique *GetTechnique() const;

  /// \brief
  ///   Returns the shader instance that the particle group would use if no shader is assigned
  VISION_APIFUNC VCompiledShaderPass *GetDefaultShader() const;

  /// \brief
  ///   Enables light grid color tracking for this particle group
  /// 
  /// The user (resp. derived classes) is responsible for allocating the array of 6 hkvVec3 color
  /// values
  /// 
  /// This particle group class does not free the pointer.
  /// 
  /// To disable light grid tracking, NULL can be passed to this function.
  /// 
  /// \param pColors
  ///   Pointer that points to an array of 6 color values. See
  ///   VisRenderLoopHelper_cl::TrackLightGridInfo
  inline void SetLightGridColorPtr(const hkvVec3* pColors) 
  {
    m_pLightGridColors=pColors;
  }

  /// \brief
  ///   Returns the pointer previously set with SetLightGridColorPtr
  inline const hkvVec3*  GetLightGridColorPtr() const
  {
    return m_pLightGridColors;
  }

  ///
  /// @}
  ///

  ///
  /// @name Distance Fading
  /// @{
  ///


  /// \brief
  ///   Determines the way particles are faded out by camera distance. The default is FADEMODE_NONE
  /// 
  /// Distance fading of particles is a useful mechanism to save performance and to interact with
  /// linear depth fog.
  /// 
  /// The following fade modes are supported by this function: FADEMODE_NONE :  particles are not
  /// faded out (default) FADEMODE_FOGDISTANCE : always uses the fog distance for fading (See
  /// VisWorld_cl::SetLinearDepthFog). If depth fog is globally disabled (i.e.
  /// VisWorld_cl::IsLinearDepthFogEnabled returns false), particle fading is disabled.
  /// FADEMODE_CUSTOM : This particle group uses a custom fade range, that can be set via
  /// VisParticleGroup_cl::SetCustomFadeRange.
  /// 
  /// Fading does not make sense for all transparency types. In particular, fading works for
  /// additively and alpha blended particles.
  /// 
  /// Note that custom shaders which are applied to a particle group have to perform the fading.
  /// See implementation of the fixed function shader "ParticlesCPU" in the base data directory.
  /// 
  /// When fading is activated (either custom range or fog) the engine clips the particle group at
  /// the fade end distance if a visibility bounding box is defined for the particle group.
  /// 
  /// \param eMode
  ///   New fade mode, see Description
  /// 
  /// \sa VisParticleGroup_cl::GetFadeMode
  /// \sa VisParticleGroup_cl::SetCustomFadeRange
  /// \sa VisWorld_cl::SetLinearDepthFog
  /// \sa VisWorld_cl::IsLinearDepthFogEnabled
  VISION_APIFUNC void SetFadeMode(FadeMode_e eMode);


  /// \brief
  ///   Returns the fade mode that has been set via VisParticleGroup_cl::SetFadeMode
  inline FadeMode_e GetFadeMode() const
  {
    return (FadeMode_e)m_eFadeMode;
  }

  /// \brief
  ///   For custom fade mode (FADEMODE_CUSTOM) this function sets the fading range
  /// 
  /// \param fStart
  ///   Distance at which particles start fading out
  /// 
  /// \param fEnd
  ///   Distance at which particles have completely faded out. Also used as a clipping distance
  /// 
  /// \sa VisParticleGroup_cl::SetFadeMode
  /// \sa VisParticleGroup_cl::GetCustomFadeRange
  VISION_APIFUNC void SetCustomFadeRange(float fStart, float fEnd);

  /// \brief
  ///   Returns the fade range that has been set via VisParticleGroup_cl::SetCustomFadeRange
  inline void GetCustomFadeRange(float &fStart, float &fEnd) const
  {
    fStart = m_fFadeStart;
    fEnd = m_fFadeEnd;
  }

  ///
  /// @}
  ///

  ///
  /// @name Overrides
  /// @{
  ///


#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Overridden function that gets the resources used for this instance.
  VISION_APIFUNC VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///


  /// \brief
  ///   Global function, called after purging particles
  VISION_APIFUNC static void UpdateAvailableMask();

  /// \brief
  ///   RTTI
  V_DECLARE_DYNCREATE_DLLEXP(VisParticleGroup_cl, VISION_APIDATA);

  ///
  /// @}
  ///

protected:
  // overridden module system child functions
  VThreadedTask *m_pUpdateTask;
  unsigned char m_cUseDistortion;
  bool m_bUseNormals;
  bool m_bSortParticles;
  bool m_bUseHardwareSpanning;
  ParticleSort_t *m_pIndexList;
  int m_iHighWaterMark;

  int m_iLastRenderFrame;           ///< Last frame the particle system was rendered
  
private:
  friend int FillParticleInstanceBuffer(VisParticleGroup_cl *pgroup);
  friend void SetParticleVertexShaderConstants(VisParticleGroup_cl *pgroup);
  friend void RenderParticleGroups(const VisParticleGroupCollection_cl *pGroups, bool bUseFiltering, unsigned int iOrder, unsigned int iFilterMask);
  friend void RenderSingleParticleGroup(VisParticleGroup_cl *pgroup, VCompiledShaderPass *pShader);
  friend inline void RenderParticleGroupPrimitives(VisParticleGroup_cl *pgroup);
  friend inline void SetParticleFadingConstants(VisParticleGroup_cl *pgroup);
  friend inline void RenderGeometryParticlesNonInstanced(VisParticleGroup_cl *pgroup, VCompiledShaderPass *pShader);

  friend int CompareParticleGroupsDistanceSorting( const void *arg1, const void *arg2);
  friend int CompareParticleGroupsUserSorting( const void *arg1, const void *arg2);
  friend class VisParticleGroupCollection_cl;
  friend class VisRenderLoopHelper_cl;
  friend class WallmarkParticleGroup_cl;
  friend class VisStateHandler_cl;
  friend class VisVisibilityObject_cl;
  friend class VisRenderStates_cl;

  void CheckForAnimatedTexture();
  void AllocateParticles(int iCount, void *pCustomMem);
  void UpdateFarclipDistance();
  VISION_APIFUNC void UpdateDefaultShaderFlags();

  VCompiledTechniquePtr m_spTechnique;

  static bool g_bSortParticleGroups;
  static bool g_bUseDistanceSorting;  ///< flag for sorting by distance or by user sort key
  static unsigned int g_iAvailableMask; ///< analogue to mesh buffer objects
  static IVisParticleGroupSorter_cl *g_pParticleSorter;

  short m_iStructStride;            ///< stride value for particles
  short m_iStructOfs;               ///< structure ofs for particles
  void *particles;                  ///< pointer to the list of particles
  int numParticles;                 ///< overall number of particle of this group

  VTextureObjectPtr spTexture;      ///< texture to use
  VTextureObjectPtr spNormalmap;    ///< normalmap texture to use
  VTextureObjectPtr spSpecularmap;  ///< specular texture to use
  float numtexrepetitions;          ///< texture repetitions
  VisTextureAnimInstancePtr texanim_id; ///< animation instance
  VSimpleRenderState_t m_iRenderState; ///< transparency,...
  ParticleShaderFlags_e m_eDefaultShaderFlags; ///< defines which default shader to use (always up-to-date)

  int m_iUserSortKey;               ///< sort key that can be set by the user
  float m_fDistanceSortValue;       ///< internal sort value for distance-based sorting

  unsigned int m_iRenderOrder;      ///< render order flag
  unsigned int m_iVisibleBitmask;   ///< context render filter

  VisVisibilityObjectPtr m_spVisObject; ///< visibility object

  short m_iAnimSubDivX,m_iAnimSubDivY;  ///< texture animation frame subdivision
  float m_fSizeAspect;
  hkvVec2 m_vParticleCenter;      ///< pivot

  bool m_bUseSmoothAnimation;       ///< fading between animation frames
  char m_eFadeMode;                 ///< fade mode (casted to FadeMode_e)
  bool m_bTrackModelViewMatrix;
  bool m_bOwnsParticleArray;
  float m_fFadeStart, m_fFadeEnd;   ///< fading out the particles for FADEMODE_CUSTOM
  float m_fDepthOffset;                 ///< z-offset value

  const hkvVec3* m_pLightGridColors; ///< pointer to lightgrid colors
  VisMeshBufferPtr m_spGeometry;
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(VisParticleGroup_cl)

#include <Vision/Runtime/Engine/SceneElements/VisApiParticleGroup.inl>

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
