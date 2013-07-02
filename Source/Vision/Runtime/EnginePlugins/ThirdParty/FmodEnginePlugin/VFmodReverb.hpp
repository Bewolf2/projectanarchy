/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodReverb.hpp

#ifndef VFMODREVERB_HPP_INCLUDED
#define VFMODREVERB_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>

/// Serialization versions for VFmodReverbProps
#define V_FMODREVERBPROPS_VERSION_0         0x00000000                    // Initial version
#define V_FMODREVERBPROPS_VERSION_CURRENT   V_FMODREVERBPROPS_VERSION_0

/// \brief
///   VFmodReverb properties
/// 
/// This class holds all reverb properties, that currently are supported by Fmod. Since this class also serializes 
/// the current version, when any changes happen on the Fmod side in future, the properties of this class can be 
/// adapted accordingly. 
class VFmodReverbProps
{
public:
  VFmodReverbProps()
  {
    m_iEnvironment = -1;                  
    m_iRoom = -1000;
    m_iRoomHF = -100;
    m_iRoomLF = 0; 
    m_fDecayTime = 1.49f;    
    m_fDecayHFRatio = 0.83f;
    m_iReflections = -2602;
    m_fReflectionsDelay = 0.007f;      
    m_iReverb = 200;
    m_fReverbDelay = 0.011f; 
    m_fHFReference = 5000.0f;
    m_fLFReference = 250.0f;          
    m_fDiffusion = 100.0f;
    m_fDensity = 100.0f;
  }

  VFmodReverbProps(int iEnvironment, int iRoom, int iRoomHF, int iRoomLF, float fDecayTime, float fDecayHFRatio, int iReflections, float fReflectionsDelay,
                  int iReverb, float fReverbDelay, float fHFReference, float fLFReference, float fDiffusion, float fDensity)
  {
    m_iEnvironment = iEnvironment;                 
    m_iRoom = iRoom;
    m_iRoomHF = iRoomHF;
    m_iRoomLF = iRoomLF; 
    m_fDecayTime = fDecayTime;  
    m_fDecayHFRatio = fDecayHFRatio;
    m_iReflections = iReflections;
    m_fReflectionsDelay = fReflectionsDelay;     
    m_iReverb = iReverb;
    m_fReverbDelay = fReverbDelay; 
    m_fHFReference = fHFReference;
    m_fLFReference = fLFReference;         
    m_fDiffusion = fDiffusion;
    m_fDensity = fDensity;
  }

  operator FMOD_REVERB_PROPERTIES() const
  {
    FMOD_REVERB_PROPERTIES properties;
    properties.Instance = 0; // since the plugin uses the virtual reverb system of Fmod always the first instance is used
    properties.Environment = m_iEnvironment;
    properties.EnvDiffusion = 1.0f; // (Wii only/ not supported)  
    properties.Room =  m_iRoom; 
    properties.RoomHF = m_iRoomHF;                         
    properties.RoomLF =  m_iRoomLF;      
    properties.DecayTime = m_fDecayTime;       
    properties.DecayHFRatio =  m_fDecayHFRatio;                  
    properties.DecayLFRatio = 1.0f; // (not supported)
    properties.Reflections = m_iReflections;   
    properties.ReflectionsDelay =  m_fReflectionsDelay;             
    properties.Reverb = m_iReverb;         
    properties.ReverbDelay = m_fReverbDelay;    
    properties.ModulationTime = 0.25f; // (not supported) 
    properties.ModulationDepth = 0.0f; // (Wii only/ not supported)  
    properties.HFReference =  m_fHFReference;   
    properties.LFReference =  m_fLFReference;   
    properties.Diffusion = m_fDiffusion;        
    properties.Density = m_fDensity;         
    properties.Flags = FMOD_REVERB_FLAGS_DEFAULT; // (Wii only/ not supported)  
    return properties;
  }

  bool operator==(const VFmodReverbProps &rhs) const
  {
    if (m_iEnvironment!=rhs.m_iEnvironment)
      return false;
    if (m_iRoom!=rhs.m_iRoom)
      return false;
    if (m_iRoomHF!=rhs.m_iRoomHF)
      return false;
    if (m_iRoomLF!=rhs.m_iRoomLF)
      return false;
    if (!hkvMath::isFloatEqual (m_fDecayTime, rhs.m_fDecayTime))
      return false;
    if (!hkvMath::isFloatEqual (m_fDecayHFRatio, rhs.m_fDecayHFRatio))
      return false;
    if (m_iReflections!=rhs.m_iReflections)
      return false;
    if (!hkvMath::isFloatEqual (m_fReflectionsDelay, rhs.m_fReflectionsDelay))
      return false;
    if (m_iReverb!=rhs.m_iReverb)
      return false;
    if (!hkvMath::isFloatEqual (m_fReverbDelay, rhs.m_fReverbDelay))
      return false; 
    if (!hkvMath::isFloatEqual (m_fHFReference, m_fHFReference))
      return false;
    if (!hkvMath::isFloatEqual (m_fLFReference, m_fLFReference))
      return false;
    if (!hkvMath::isFloatEqual (m_fDiffusion, rhs.m_fDiffusion))
      return false;
    if (!hkvMath::isFloatEqual (m_fDensity, m_fDensity))
      return false;
    return true;
  }

  V_DECLARE_SERIALX(VFmodReverbProps, FMOD_IMPEXP)
  FMOD_IMPEXP void SerializeX( VArchive &ar );

  int m_iEnvironment;        ///< Sets all listener properties (-1 = OFF)                     
  int m_iRoom;               ///< Room effect level (at mid frequencies)   
  int m_iRoomHF;             ///< Relative room effect level at high frequencies 
  int m_iRoomLF;             ///< Relative room effect level at low frequencies   
  float m_fDecayTime;        ///< Reverberation decay time at mid frequencies       
  float m_fDecayHFRatio;     ///< High-frequency to mid-frequency decay time ratio    
  int m_iReflections;        ///< Early reflections level relative to room effect  
  float m_fReflectionsDelay; ///< Initial reflection delay time       
  int m_iReverb;             ///< Late reverberation level relative to room effect   
  float m_fReverbDelay;      ///< Late reverberation delay time relative to initial reflection   
  float m_fHFReference;      ///< Reference high frequency (hz) 
  float m_fLFReference;      ///< Reference low frequency (hz)           
  float m_fDiffusion;        ///< Value that controls the echo density in the late reverberation decay
  float m_fDensity;          ///< Value that controls the modal density in the late reverberation decay 
  
};


/// Serialization versions for VFmodReverb
#define V_FMODREVERB_VERSION_0         0x00000000                    // Initial version
#define V_FMODREVERB_VERSION_CURRENT   V_FMODREVERB_VERSION_0

/// \brief
///   3D reverb area object. Supports positioning etc.
/// 
/// The 3D reverb object is a sphere having 3D attributes (position, minimum distance, maximum distance) and reverb properties.
/// When the listener is within the sphere of effect of one or more 3d reverbs, the listener's 3D reverb properties are a weighted 
/// combination of such 3D reverbs. When the listener is outside all of the reverbs, the 3D reverb setting is set to the default 
/// ambient reverb setting.
///
/// This is the native counterpart of the vForge reverb shape.
class VFmodReverb : public VisObject3D_cl
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodReverb(VFmodManager *pManager=NULL);

  FMOD_IMPEXP virtual ~VFmodReverb();

  FMOD_IMPEXP VOVERRIDE void DisposeObject();

  ///
  /// @}
  ///


  ///
  /// @name Initialization
  /// @{
  ///

  /// \brief
  ///   Initializes reverb 
  FMOD_IMPEXP bool Init(VFmodReverbProps &reverbProperties, float fReverbMinDistance, float fReverbMaxDistance); 

  ///
  /// @}
  ///


  ///
  /// @name Overridden functions
  /// @{
  ///

  FMOD_IMPEXP VOVERRIDE void OnObject3DChanged(int iO3DFlags);

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIAL_DLLEXP( VFmodReverb, FMOD_IMPEXP );

  FMOD_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Reverb property functions
  /// @{
  ///

  /// \brief
  ///   Returns, whether reverb is active
  inline bool IsActive() const { return m_bActive; } 

  /// \brief
  ///   De-/ Activates reverb
  FMOD_IMPEXP void SetActive(bool bStatus);

  /// \brief
  ///   Gets radius within which the reverb has full effect 
  inline float GetReverbMinDistance() const { return m_fReverbMinDistance; }

  /// \brief
  ///   Gets radius outside of which the reverb has zero effect 
  inline float GetReverbMaxDistance() const { return m_fReverbMaxDistance; }

  /// \brief
  ///   Sets minimum and maximum reverb distances
  FMOD_IMPEXP void SetReverbDistances(float fReverbMinDistance, float fReverbMaxDistance);

  /// \brief
  ///   Gets reverb properties
  inline const VFmodReverbProps& GetReverbProperties() const { return m_reverbProp; }

  /// \brief
  ///   Sets reverb properties
  FMOD_IMPEXP void SetReverbProperties(VFmodReverbProps &reverbProperties);

  /// \brief
  ///   Returns whether the reverb has a valid Fmod event reverb 
  inline bool IsValid() const { return m_pReverb!=NULL; }

  ///
  /// @}
  ///


  ///
  /// @name Misc
  /// @{
  ///

  ///\brief
  /// Helper function to render the object in the scene. Used by vForge
  FMOD_IMPEXP void DebugRender(IVRenderInterface* pRI);

  ///
  /// @}
  ///

private:
  VFmodManager *m_pManager;
  bool m_bActive;
  float m_fReverbMinDistance, m_fReverbMaxDistance;
  VFmodReverbProps m_reverbProp;
  FMOD::EventReverb *m_pReverb;
};


/// \brief
///   Collection of reverb instances
class VFmodReverbCollection : public VRefCountedCollection<VFmodReverb>
{
public:
  /// \brief
  ///   Returns the first occurrence (or NULL) of a reverb in this collection with specified object key. The object key can be set in vForge.
  FMOD_IMPEXP VFmodReverb* SearchReverb(const char* szName) const;
};

#endif // VFMODREVERB_HPP_INCLUDED

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
