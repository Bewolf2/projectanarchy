/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleDescriptor.hpp

#ifndef PARTICLEDESCRIPTOR_HPP_INCLUDED
#define PARTICLEDESCRIPTOR_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEmitter.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/StochasticalParam.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>

// forward declaration
class ParticleGroupBase_cl;
class TiXmlElement;
class VisParticleGroupManager_cl;
class VisParticleGroupDescriptor_cl;
class VisParticleEffectFile_cl;
struct ParticleExt_t;

// smart pointer typedef
typedef VSmartPtr<VisParticleGroupDescriptor_cl> VisParticleGroupDescriptorPtr;


// XML name constants
#define XML_OLD_GROUPDESCRIPTOR_NAME      "groupdescriptor"
#define XML_GROUPDESCRIPTOR_NAME          "particlelayer"
#define XML_CONSTRAINT_NAME               "constraint"
#define XML_CURRENT_FILEVERSION           1,0,0,0

// flags for copying from source particle (e.g. when creating a new particle from destroyed)
#define PARTICLE_COPYFLAG_NONE            0x00000000
#define PARTICLE_COPYFLAG_POSITION        0x00000001
#define PARTICLE_COPYFLAG_VELOCITY        0x00000002
#define PARTICLE_COPYFLAG_SIZE            0x00000004
#define PARTICLE_COPYFLAG_TABLEROW        0x00000010
#define PARTICLE_COPYFLAG_ALL             0xffffffff

enum VIS_PARTICLE_ANIMATION_MODE_e
{
  PARTICLE_ANIMATION_NONE = 0,
  PARTICLE_ANIMATION_REPEAT = 1,
  PARTICLE_ANIMATION_LIFETIME = 2,

  PARTICLE_ANIMATION_MODE_e_SIZE = 0xFFFFFFFF // Unused, only ensures that sizeof(VIS_PARTICLE_ANIMATION_MODE_e) == sizeof(int)
};

enum VIS_PARTICLE_ROTATION_MODE_e
{
  PARTICLE_ROTATION_NONE = 0,
  PARTICLE_ROTATION_CONSTSPEED = 1,
  PARTICLE_ROTATION_ANGLECURVE = 2,
};

enum VIS_PARTICLE_TOPOLOGY_e
{
  PARTICLE_TOPOLOGY_BILLBOARDS        = 0,
  PARTICLE_TOPOLOGY_STRETCH_VELOCITY  = 1,
  PARTICLE_TOPOLOGY_STRETCH_FIXLENGTH = 2,
  PARTICLE_TOPOLOGY_RINGWAVE          = 3,
  PARTICLE_TOPOLOGY_MESH              = 4,
  PARTICLE_TOPOLOGY_TRAIL             = 5,

  // just for backwards compatibility
  PARTICLE_TOPOLOGY_UNKNOWN           = 99,
};


///\brief
///  A descriptor instance defines all properties of a particle effect layer. It keeps per-resource data, not per instance data
///
///A list of decriptors is bound to the resource VisParticleEffectFile_cl which describes the full effect with multiple layers.
///Properties in this class correspond to a single particle layer in vForge
///
///\see
///VisParticleEffectFile_cl
class VisParticleGroupDescriptor_cl : public VRefCounter, VUserDataObj
{
public:
  ///\brief
	///Constructor
	///
	///\param pOwner
	///Owner effect resource
	///
	PARTICLE_IMPEXP VisParticleGroupDescriptor_cl(VisParticleEffectFile_cl *pOwner);

  ///\brief
	///Destructor
	PARTICLE_IMPEXP virtual ~VisParticleGroupDescriptor_cl();

  ///\brief
	///Returns the layer's name as defined in vForge
	///
	///\returns
	///Layer name string
	inline const char *GetName() const {return m_sName;}

  ///\brief
	///Returns the layer's description as defined in vForge.
	///
	///\returns
  ///Layer description string (or NULL)
  inline const char *GetDescription() const {return m_sDescription;}

  ///\brief
	///Deprecated
	///
	///\returns
	///Memory in bytes
	///
	inline int GetMemSize() const {return sizeof(VisParticleGroupDescriptor_cl);}

  ///\brief
	///Not used
	///
	///\param szFilename
	///Filename
	///
	///\returns
	///Success
	PARTICLE_IMPEXP bool LoadFromXML(const char *szFilename);

  ///\brief
	///Not used
	///
	///\param szFilename
	///Filename
	///
	///\returns
	///Success
  PARTICLE_IMPEXP bool SaveToXML(const char *szFilename);

  ///\brief
	///XML serialization function. Can read and write descriptors
	///
	///\param pRoot
	///layer's XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true for success
	PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pRoot, bool bWrite);

  V_DECLARE_SERIALX( VisParticleGroupDescriptor_cl, PARTICLE_IMPEXP )

  ///\brief
  ///Binary serialization (not used)
	///
	///\param ar
	///Binary archive
	///
	PARTICLE_IMPEXP void SerializeX( VArchive &ar );

  ///\brief
	///Helper IO function
	///
	///\param szDest
	///Target string
	///
	///\param szFilename
	///Source filename
	PARTICLE_IMPEXP void MakeFilenameAbsolute(char *szDest, const char *szFilename);

  
  ///\brief
	///Sets a new texture file.
	///
	///\param szFilename
	///Texture filename
	inline void SetTextureFilename(const char *szFilename);

  ///\brief
	///Sets a new blend mode
	///
	///\param eTransp
	///New blend mode
	inline void SetTransparency(VIS_TransparencyType eTransp=VIS_TRANSP_ADDITIVE);

  ///\brief
	///Sets animation frame properties
	///
	///\param iSubDivX
	///Number of subdivisions
	///
	///\param iSubDivY
	///Number of subdivisions
	///
	///\param iUsedCount
	///Used number of frames
	///
	inline void SetAnimationSubdivisions(int iSubDivX,int iSubDivY, int iUsedCount);

  ///\brief
	///Sets particle life time properties
	///
	///\param fAverage
	///Average lifetime
	///
	///\param fVariation
  ///Variation [0..1]
	inline void SetParticleLifeTime(float fAverage, float fVariation);

  ///\brief
	///Unused
	///
	///\param fStartSize
	///Unused
	///
	///\param fSizeGrowth
	///Unused
	inline void SetSizeGrowth(float fStartSize, float fSizeGrowth);

  ///\brief
	///Initial speed properties
	///
	///\param fAverage
  ///Average initial speed (units/s)
	///
	///\param fVariation
  ///Speed variation (0..1)
	///
  inline void SetSpeed(float fAverage, float fVariation);

  ///\brief
  ///Sets physics friction value (0..1)
	///
	///\param fValue
	///New friction value (0..1)
	///
	inline void SetFriction(float fValue);

  ///\brief
  ///Sets gravity constant (negative for downwards)
	///
	///\param fValue
	///New gravity
	///
	inline void SetGravity(float fValue);

  ///\brief
	///Sets the layer's lifetime properties.
	///
	///\param fAverage
	///Average value
	///
	///\param fVariation
  ///Variation (0..1)
	inline void SetGroupLifeTime(float fAverage, float fVariation);

  ///\brief
	///Returns reference to the default emitter.
	///
	///\returns
	///emitter pointer
	///
	inline VisParticleEmitter_cl *GetDefaultEmitter() {return &m_DefaultEmitter;}

  ///\brief
	///Helper function to return a random value defined by average and variation
	///
  ///\param randGen
  /// Random number generator to use
  ///
	///\returns
	///Random particle lifetime
	inline float GetRandomLifeTime(const VRandom& randGen) const;

  ///\brief
	///Helper function to return a random value defined by average and variation
	///
  ///\param randGen
  /// Random number generator to use
  ///
	///\returns
	///Random layer lifetime
  inline float GetRandomGroupLifeTime(const VRandom& randGen) const;
 
  ///\brief
	///Helper function to return a random value defined by average and variation
	///
  ///\param randGen
  /// Random number generator to use
  ///
	///\returns
	///Random particle speed
  inline float GetRandomSpeed(const VRandom& randGen) const;

  ///\brief
	///Calculates a rough estimate for the number of particles needed. Internally used to allocate the particle array
	///
	///\param fLifeTime
  ///If >0 it overrides the default value
	///
	///\param fFreq
  ///If >0 it overrides the default value
	///
	///\returns
	///Particle count
	///
	inline int GetEstimatedMaxParticleCount(float fLifeTime=-1.f, float fFreq=-1.f) const;

  ///\brief
	///Number of available animation subdivisions
	///
	///\returns
	///subdivX*subdivY
	inline int GetAnimationFrameCount() const;

  ///\brief
	///Retrieves the local bounding box.
	///
	///\param dstBox
	///target box
	///
	///\returns
	///true if box is valid
	inline bool GetBoundingBox(hkvAlignedBBox &dstBox) {dstBox=m_BoundingBox;return dstBox.isValid();}

#ifndef _VISION_DOC
  PARTICLE_IMPEXP static void SetCurrentRelPath(const char *szFilename);
  PARTICLE_IMPEXP static const char* GetCurrentRelPath();
  PARTICLE_IMPEXP static bool CombineWithRelPath(char *szDest, const char *szFilename);
#endif

  ///\brief
	///Initializes a new random particle using this layer's properties
	///
	///\param pParticle
	///target particle
	///
	///\param pGroup
	///Owner group
	///
	///\param pSrcParticle
	///Source particle if iCopyFlags are specified
	///
	///\param iCopyFlags
	///Bitmask that determines which properties are retained from pSrcParticle. See constants PARTICLE_COPYFLAG_xyz
	///
	PARTICLE_IMPEXP void VISION_FASTCALL InitRandomParticle(ParticleExt_t *pParticle, ParticleGroupBase_cl *pGroup, const ParticleExt_t *pSrcParticle=NULL, int iCopyFlags=PARTICLE_COPYFLAG_NONE);

  ///\brief
	///Internal function that has to be called after manually created to update color curves etc..
	///
	PARTICLE_IMPEXP void Finish(bool bForceCreateLookups=true);

#ifdef SUPPORTS_SNAPSHOT_CREATION
  PARTICLE_IMPEXP void GetDependencies(VResourceSnapshot &snapshot, VManagedResource *pOwner);
#endif

  static bool TechniqueUsesLightGrid(VCompiledTechnique *pTech);

protected:
  inline int	binaryWeightSearch(float fAccumulatedWeight);

public:
  friend class ParticleGroupBase_cl;
  friend class VisParticleEmitter_cl;
  friend class VisParticleEffectFile_cl;
  friend class VisParticleGroupManager_cl;

  // static helper functions
  static inline float GetRandomValue(const VRandom& randGen, float fAverage, float fVariation);

  static char g_szCurrentRelPath[FS_MAX_PATH];

  VisParticleEffectFile_cl *m_pOwner; ///< Owner effect
  bool m_bActive; ///< Only active descriptors will be used for creating particle groups. Particle types that are used for destroyed particles are flagged as inactive.
  char m_eTopology; ///< casted to VIS_PARTICLE_TOPOLOGY_e
  VString m_sName;///< name of the layer
  VString m_sDescription;  ///< description of the layer

// group properties
  VString m_sTextureFilename;       ///< particle texture filename
  VString m_sNormalmapFilename;     ///< particle normal map filename
  VString m_sSpecularmapFilename;   ///< particle specular map filename
  VString m_sRandomColorFilename;   ///< filename for m_spRandomColorLookup member
  VString m_sMeshFilename;          ///< either .model or .vmesh

  VIS_TransparencyType m_eTransp;   ///< engine transparency value
  int m_iAnimSubDiv[2];             ///< subdivision of animation frames in the texture
  int m_iUsedAnimFrames;            ///< number of used animation frames
  bool m_bUseSmoothAnimation;       ///< smooth interpolation between animation frames
  bool m_bStartRandomAnimFrame;     ///< start with a random animation frame
  bool m_bAlwaysInForeground;       ///< z-buffer test?
  bool m_bHandleWhenVisible;        ///< if enabled, simulation of the group will only be performed if group is visible
  float m_fSizeAspect;              ///< texture size aspect
  int m_iFlags;                     ///< group flags
  VisStochasticalParam_cl m_GroupLifeTime;  ///< lifetime of group; <0 for infinite lifetime
  VisStochasticalParam_cl m_TimeScaling;    ///< global time scaling for this group
  int m_iGroupSortKey;              ///< sorting key (only works when particle group distance sorting is disabled)
  VisStochasticalParam_cl m_InitialDelay;  ///< delay before simulation starts
  hkvVec2 m_vParticleCenter;      ///< e.g. 0.5,0.5

// visibility 
  hkvAlignedBBox m_BoundingBox;  ///< particle group's local bounding box
  float m_fDynamicInflateInterval;  ///< time interval to recalc the bounding box from all particles
  float m_fDepthOffset;             ///< z-offset value
  VColorRef m_ModColor;             ///< additional modulation color
  float m_fApplySceneBrightness;    ///< if true, the scene brightness will be considered at particle group emitter position
  bool m_bSortParticles;            ///< sort particles
  bool m_bRepeatLifetime;           ///< if true, it has infinite lifetime
  bool m_bSoftParticles;            ///< if enabled, a shader is applied that renders soft particles
  int m_iVisibleBitmask;            ///< per layer filtering bitmask

  VisParticleGroup_cl::FadeMode_e m_eFadeMode;   ///< maps to VisParticleGroup_cl member
  float m_fFadeStart;               ///< maps to VisParticleGroup_cl member
  float m_fFadeEnd;                 ///< maps to VisParticleGroup_cl member
  int m_iRenderOrder;               ///< the render order in which to render the particle group
  
// Curves 
  VColorCurvePtr m_spColorCurve;    ///< color lookup curve [0..1] maximum value = 255
  VCurve2DPtr m_spSizeCurve;        ///< particle size curve [0..1] maximum value = maximum size
  VCurve2DPtr m_spAnimCurve;        ///< particle animation curve
  VisBitmapPtr m_spColorLookup;     ///< particle lookup for color Is generated from the m_spColorCurve member at runtime
  VisBitmapPtr m_spRandomColorLookup;  ///< Optional color table to initialize a random color at particle birth time
  DynArray_cl<float> m_spProbabilityFrameWeights; ///< array holding the accumulated weight of each frame in case the animation curve is used as probability curve (requires 'no animation mode' and multiple frames)
  float m_fProbabilityTotalWeight; ///< sum of all probability weights

// particle behavior
  VIS_PARTICLE_ANIMATION_MODE_e m_eParticleAnimMode; ///< animation mode of particles
  VisStochasticalParam_cl m_ParticleLifeTime;   ///< particle lifetime in sec.
  VisStochasticalParam_cl m_ParticleAnimTime;   ///< time for single loop animation
  VisStochasticalParam_cl m_ParticleSpeed;      ///< particle speed
  VisStochasticalParam_cl m_ParticleStartSize;  ///< size of particle at startup (if lookup is not specified)
  VisStochasticalParam_cl m_ParticleSizeGrowth; ///< size growth (units per sec) 

// rotation
  VIS_PARTICLE_ROTATION_MODE_e m_eRotationMode; ///< particle rotation behavior
  float m_fInitialAngle[2];                     ///< min/max value of initial rotation angle (degree), mode!=PARTICLE_ROTATION_ANGLECURVE
  float m_fConstRotationSpeed[2];               ///< min/max value of const rotation (degree), mode PARTICLE_ROTATION_CONSTSPEED
  VCurve2DPtr m_spRotationCurve;                ///< rotation angle over time, mode PARTICLE_ROTATION_ANGLECURVE
  float m_fRotationCurveDuration;               ///< time[s] of a curve cycle, <=0 for particle lifetime, mode PARTICLE_ROTATION_ANGLECURVE
  bool m_bRandomRotationCurveOffset;            ///< if true, curve starts at random position, mode PARTICLE_ROTATION_ANGLECURVE
  bool m_bInertiaAffectsGravity;                ///< physics: gravity affected by inertia
  bool m_bLocalSpace;                           ///< This maps to SetUseLocalSpaceMatrix
  bool m_bUseTransformationCurve;               ///< Use curves or constant offset

  // physics
  float m_fFriction;              ///< friction[0..1]
  hkvVec3 m_vGravity;        ///< gravity vector
  VisStochasticalParam_cl m_ParticleInertia;  ///< per particle value for wind influence etc. (1=no effect,0=full effect)
  unsigned int m_iConstraintAffectBitMask;    ///< bitmask for per-layer constraints (compared against VisParticleConstraint_cl::m_iAffectBitMask)
  float m_fApplyEmitterSpeed;                 ///< amount of emitter speed that is applied to each spawned particle

  // destroying particles
  VisStochasticalParam_cl m_OnDestroyCreateCount; ///< number of particles to create when particle is destroyed
  VString m_sDestroyCreateGroup;    ///< name of descriptor used to create particles that are destroyed
  int m_iOnDestroyCopyFlags;        ///< which particle properties should be copied? (PARTICLE_COPYFLAG_POSITION...)
  VisParticleGroupDescriptorPtr m_spDestroyCreateDesc; ///< the pointer to the descriptor (set from m_sDestroyCreateGroup at loading time)

// distortion
  bool m_bDistorted;          ///< Indicates whether distortion is used
  bool m_bDistortionSizeMode; ///< Size distortion mode
  bool m_bDistortionPlaneAligned;   ///< planar aligned
  bool m_bHasCustomCenter;          ///< indicates whether m_vParticleCenter should be used
  float m_fTrailOverlap;            ///< for trail particles this defines how much the segments overlap (1.0 = no)

  VisStochasticalParam_cl m_FixDistortionLength; ///< distortion uses a fix length
  VisStochasticalParam_cl m_SpeedMultiplier;     ///< distortion length is speed-relative
  hkvVec4 m_vSizeMultiplier;   ///< If m_bDistortionSizeMode is defined then this is the ratio between size and distortion vector

// normal
  bool m_bUseNormal;      ///< A custom normal vector is provided
  hkvVec3 m_vNormal; ///< Custom normal vector 
  float m_fRotationAxisRandomness; ///< randomness of rotation axis
  hkvVec3 m_vRelativePosition;     ///< local space position of this layer in the effect
  hkvVec3 m_vRelativeOrientation;  ///< local Euler orientation of this layer in the effect

// spawn
  VisParticleEmitter_cl m_DefaultEmitter;  ///< particle emitter

// shader
  VisEffectConfig_cl m_CustomEffectConfig; ///< Custom effect config

  // local position curve
  float m_fTransformationCurveTime, m_fPositionCurveRadius;
  VPositionCurvePtr m_spPositionCurve;
  VPositionCurvePtr m_spOrientationCurve;

// misc
// Custom curve: For arbitrary use. Ignored by standard simulation code.
// This pointer can be NULL or set to the custom curve defined per layer in the editor
// Values are normalized to [0..1] range on both axes
  VCurve2DPtr m_spCustomCurve; ///< Ignored (see comments above)
  VColorCurvePtr m_spCustomColorCurve; ///< Ignored (see comments above)
  int m_iCustomIntValue;  ///< representation of the custom value

  VisAnimEventList_cl m_EventList;
  VisMeshBufferPtr m_spGeometry;
  hkvAlignedBBox m_MeshBoundingBox; // bounding box of the mesh

  int m_iCustomRandomSeed;  ///< Override default random seed for emitter (runtime only, not serialized)
};



///\brief
///Collection class for descriptors (refcounted)
///
class VisParticleDescriptorList_cl : public VRefCountedCollection<VisParticleGroupDescriptor_cl>
{
public:

  ///\brief
	///Finds a descriptor by its name
	///
	///\param szName
	///Name to lookup
	///
	///\returns
	///Pointer to found instance or NULL
	PARTICLE_IMPEXP VisParticleGroupDescriptor_cl *FindDescriptor(const char *szName) const;

  // binary serialization
  V_DECLARE_SERIALX( VisParticleDescriptorList_cl, PARTICLE_IMPEXP )

  ///\brief
	///Binary serializes all instances in this collection
	///
	///\param ar
	///Binary archive
	///\param pOwner
	///Owner effect passed to the constructor
	PARTICLE_IMPEXP void SerializeX( VArchive &ar, VisParticleEffectFile_cl *pOwner=NULL );
};


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleDescriptor.inl>

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
