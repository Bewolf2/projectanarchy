/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ParticleEmitter.hpp

#ifndef PARTICLEEMITTER_HPP_INCLUDED
#define PARTICLEEMITTER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/StochasticalParam.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>

// forward declarations
struct ParticleExt_t;
class ParticleGroupBase_cl;
class TiXmlElement;
class VisParticleEmitter_cl;
class VisParticleGroupDescriptor_cl;

// smart pointer typedef
typedef VSmartPtr<VisParticleEmitter_cl> VisParticleEmitterPtr;

///////////////////////////////////////////////////////////////////////////
// enum type for particle emitter types
///////////////////////////////////////////////////////////////////////////


///\brief
///Basic emitter type for class VisParticleEmitter_cl
///
enum VIS_EMITTER_TYPE_e
{
  EMITTER_TYPE_UNKNOWN = -1,  ///< Unknown emitter type
  EMITTER_TYPE_POINT   = 0,   ///< Point emitter
  EMITTER_TYPE_SPHERE   = 1,  ///< Sphere emitter
  EMITTER_TYPE_BOX      = 2,  ///< Box emitter
  EMITTER_TYPE_PLANE    = 3,  ///< Plane emitter
  EMITTER_TYPE_PATH     = 4,  ///< Path emitter
  EMITTER_TYPE_RAY      = 5,  ///< Ray emitter
  EMITTER_TYPE_MESH     = 6,  ///< Entity mesh
};

///\brief
///Internal enum for class VisParticleEmitter_cl
///
///\internal
enum VIS_CONE_ANGLE_MODE_e
{
  CONE_ANGLE_MODE_ALL_DIMENSIONS = 0,
  CONE_ANGLE_MODE_YAW_ONLY = 1,
  CONE_ANGLE_MODE_PITCH_ONLY = 2
};


///\brief
///  Single particle emitter instance - bound to class ParticleGroupBase_cl
class VisParticleEmitter_cl : public VRefCounter
{
public:
  ///\brief
  ///  Constructor
  PARTICLE_IMPEXP VisParticleEmitter_cl();
  ///\brief
  ///  Destructor
  PARTICLE_IMPEXP ~VisParticleEmitter_cl();

  ///\brief
  ///  Returns a cloned instance of this emitter
  PARTICLE_IMPEXP VisParticleEmitter_cl *Clone(const VRandom& randGen) const;

  ///\brief
  ///  Returns the emitter type. See VIS_EMITTER_TYPE_e
  inline VIS_EMITTER_TYPE_e GetType() const {return m_eType;}

  ///\brief
  ///  Changes type to a point emitter
  inline void SetType_Point();
  ///\brief
  ///  Changes type to a sphere emitter with specified radius
  inline void SetType_Sphere(float fRadius);
  ///\brief
  ///  Changes type to a box emitter with specified extent
  inline void SetType_Box(const hkvVec3& vBoxDim);
  ///\brief
  ///  Changes type to a plane emitter with specified extent
  inline void SetType_Plane(float fPlaneDimU,float fPlaneDimV);
  ///\brief
  ///  Changes type to a ray emitter with specified length
  inline void SetType_Ray(float fRayLen);
  ///\brief
  ///  Changes type to a path emitter attached to specified path
  inline void SetType_Path(VisPath_cl *pPath,float fRadius=0.f);
  ///\brief
  ///  Changes type to a path emitter that looks up path by path key
  inline void SetType_Path(const char *szPathKey,float fRadius=0.f);

  ///\brief
  ///  Associate a mesh emitter with an entity instance. The emitter will spawn particles from random vertices of the animated trace mesh
  inline void SetMeshEmitterEntity(VisBaseEntity_cl *pEntity);

  ///\brief
  ///  Internal update function
  inline void UpdateMeshEmitterEntity();
  
  ///\brief
  /// Modifies the spawn frequency (particles per second) for this emitter
	///
	///\param fParticlesPerSecond
	///Particles per second
	///
	///\param fVariation
	///Variation over time
	inline void SetSpawnFrequency(float fParticlesPerSecond, float fVariation);

  ///\brief
  /// Intensity factor; additionally modulates the spawn frequency
  inline void SetIntensity(float fIntensity) {m_fIntensity=fIntensity;}

  ///\brief
  /// Modifies the cone angle for this emitter. Negative values for all directions
  inline void SetConeAngle(float fNewAngle=-1.f, float fMinAngle=0.f);
  
  ///\brief
  /// Returns the cone angle
  inline float GetConeAngle() const;

  ///\brief
  ///Evaluates the emitter's local bounding box (type dependent)
	///
	///\param dstBox
	///Target reference
	///
	///\returns
	///true if bounding box is valid
	///
	PARTICLE_IMPEXP bool GetBoundingBox(hkvAlignedBBox &dstBox) const;

  ///\brief
	///Spawns a new particle
	///
	///\param pParticle
	///New particle to receive data
	///
	///\param pGroup
	///Owner group
	///
  ///\param iColor
  /// Might be multiplied with emitter mask
	///
  ///\param emitCS
  /// Matrix reference that receives the coordinate system in which the particles shall be emitted. Only set when bEmitCSValid is set to true inside this function
	///
  ///\param bEmitCSValid
  /// Receives the status whether emitCS has been initialized by this function
	PARTICLE_IMPEXP void SpawnSingleParticle(ParticleExt_t *pParticle, ParticleGroupBase_cl *pGroup, VColorRef &iColor, hkvMat3 &emitCS, bool &bEmitCSValid);

  ///\brief
	///Must be called once per frame; Spawns new particles
	///
	///\param pGroup
	///Owner particle group
	///
	///\param fTimeDelta
	///Time delta since last simulation update
	///
	///\param iRemainingCount
	///Reference that receives remaining particle count if the group has a fixed budget
	///
	PARTICLE_IMPEXP void HandleEmitter(ParticleGroupBase_cl *pGroup, float fTimeDelta, int &iRemainingCount);

  ///\brief
  ///  Helper XML serialization function
  PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pEmitter, bool bWrite);
  PARTICLE_IMPEXP void OnXMLExchangeFinished(VisParticleGroupDescriptor_cl *pDesc);

  ///\brief
  ///  Math helper function
  static inline void Helper_GetRandomSpherePosition(hkvVec3& vDst, float fRadius, const VRandom& randGen);
  ///\brief
  ///  Math helper function
  static inline void Helper_GetRandomOnSpherePosition(hkvVec3& vDst, float fRadius, const VRandom& randGen);

  V_DECLARE_SERIALX( VisParticleEmitter_cl, PARTICLE_IMPEXP )
  void SerializeX( VArchive &ar );

  PARTICLE_IMPEXP void ModulateEmitterMask(float u, float v, VColorRef &iDestColor) const;
public:
  friend class VisParticleGroupDescriptor_cl;
  friend class ParticleGroupBase_cl;

  PARTICLE_IMPEXP const char *GetTypeName() const;
  PARTICLE_IMPEXP bool GetTypeFromName(const char *szName);

// emitter member vars: don't forget to copy them in Clone function!

  //emitter type properties
  VIS_EMITTER_TYPE_e m_eType; ///< Emitter type
  hkvVec4 m_vParam;           ///< Depends on actual type
  VisPathPtr m_spPath;        ///< Path pointer if path type is defined

  // other emitter properties
  float m_fConeAngle, m_fMinConeAngle;        ///< cone angle to spawn particles in group direction. <0 for all directions
  VIS_CONE_ANGLE_MODE_e m_eConeMode;          ///< cone mode determining in which dimension the particles are spawned within the cone
  VisStochasticalParam_cl m_ParticlesPerSec;  ///< number of particles per second
  VisStochasticalParam_cl m_FixParticleCount; ///< group has fixed number of particles. group creates a per-instance actual count from it
  VisStochasticalParam_cl m_StartupFillPercentage; ///< number of particles at group startup time
  float m_fCountLookupTime;                   ///< time in sec to pass the count lookup texture horizontally, <0 to use lifetime
  bool m_bRandomOffset;                       ///< if true, the initial offset (m_fSpawnCountLookupPos) is a random value, so the instances are not in phase
  bool m_bInterpolateEmitterPos;              ///< if true (default) emitter position is interpolated if multiple particles are created within one frame
  bool m_bEmitFromSurface;                    ///< If true, particles will be emitted from the emitter shape's surface rather than its volume (default: false)
  float m_fIntensity;                         ///< intensity factor [0..1] to multiply the birth rate

  // count lookup curve 
  VCurve2DPtr m_spCountCurve;       ///< particle count lookup curve [0..1] maximum value = lookup time
  
  // emitter internal counter variables
  float m_fSpawnCountLookupPos;     ///< current lookup position (per emitter instance)
  float m_fSpawnTimeCtr;            ///< current position for number of new particles
  hkvVec3 m_vLastEmitterPos;   ///< caches the last emitter position
  float m_fParticleAge;

  IVCollisionMeshPtr m_spEmitterMesh;
  VWeakPtr<VisBaseEntity_cl> m_wpEntityMeshEmitter;

  VisBitmapPtr m_spEmitterMask;     ///< a bitmap that defines the the color tinting for the emitter shape
  VString m_sEmitterMaskFilename;   ///< the filename of this mask - only required inside vForge context
};


///\brief
///Collection class for a list of particle emitters
///
class VisParticleEmitterList_cl : public VRefCountedCollection<VisParticleEmitter_cl>
{
public:
  // binary serialization
  V_DECLARE_SERIALX( VisParticleEmitterList_cl, PARTICLE_IMPEXP )
  PARTICLE_IMPEXP void SerializeX( VArchive &ar );
};

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleEmitter.inl>

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
