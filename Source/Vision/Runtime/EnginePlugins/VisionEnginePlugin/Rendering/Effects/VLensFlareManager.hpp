/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VLensFlareManager.hpp

#ifndef LENSFLAREMANAGER_HPP_INCLUDED
#define LENSFLAREMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VLensFlareComponent.hpp>

typedef VRefCountedCollection<VLensFlareComponent> VLensFlareCollection;

/// \brief
///   Defines what should be done in a call to VLensFlareManager::UpdateLensFlares.
enum VLensFlareUpdateFlags_e
{
  VLFUF_ADD = V_BIT(0),            
  VLFUF_REMOVE = V_BIT(1),         
  VLFUF_UPDATE = V_BIT(2),         
  VLFUF_FORCE_SCHEDULE = V_BIT(3), 
  VLFUF_FORCE_FETCH = V_BIT(4),    
  VLFUF_USE_OC_CONTEXT = V_BIT(5), 
};

/// \brief
///   Manager for lens flares. The global instance is accessible via VLensFlareManager::GlobalManager()
///
/// This class manages a collection of lens flares and hooks into an existing renderloop to render the them.
class VLensFlareManager :  public IVisCallbackHandler_cl
{
public:
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Lens flare manager constructor
  EFFECTS_IMPEXP VLensFlareManager();
  EFFECTS_IMPEXP virtual ~VLensFlareManager();

  ///
  /// @}
  ///


  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// \brief
  ///   Implements the listener function
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///


  ///
  /// @name Lens Flare Public Functions
  /// @{
  ///

  /// \brief
  ///   Called once by the plugin for initialization
  EFFECTS_IMPEXP void OneTimeInit();
  /// \brief
  ///   Called once by the plugin for de-initialization
  EFFECTS_IMPEXP void OneTimeDeInit();

  /// \brief
  ///   Adds a lens flare component instance (will be called by VLensFlareComponent::SetOwner)
  EFFECTS_IMPEXP int AddLensFlare(VLensFlareComponent *pLensFlare);

  /// \brief
  ///   Remove a lens flare component instance (will be called automatically by d'tor)
  EFFECTS_IMPEXP void RemoveLensFlare(VLensFlareComponent *pLensFlare);

  /// \brief
  ///   Clear the collection of instances (frees instances that do not have external references)
  EFFECTS_IMPEXP void PurgeLensFlares();

  /// \brief
  ///   Updates the state of all lens flare components (retrieves query results). This function is automatically called by a renderloop hook.
  ///
  /// \param iLensFlareUpdateFlags
  ///   Flags defined in VLensFlareUpdateFlags_e. Mainly used for teleportation of the camera to ensure no task is done twice.
  EFFECTS_IMPEXP void UpdateLensFlares(int iLensFlareUpdateFlags);

  /// \brief
  ///   Function to render all visible lens flares into the scene. This function is automatically called by a renderloop hook.
  EFFECTS_IMPEXP void RenderAllVisibleLensFlares();

  /// \brief
  ///   Defines the entry point inside renderloops for rendering the lens flares. Can be any value of VRenderHook_e. Default is VRH_CORONAS_AND_FLARES
  EFFECTS_IMPEXP void SetLensFlaresRenderHookConstant(int iConst = VRH_CORONAS_AND_FLARES) {m_iLensFlareRenderHookConst = iConst;}

  /// \brief
  ///   If true, each time the camera teleports occlusion queries will be forcefully redrawn and retrieved to ensure no visible artifacts occur.
  ///
  /// Note: On the PSP2 this setting will be ignored, as rendering and retrieving occlusion queries in the same frame is unsupported on these platforms.
  ///
  /// \param bForce
  ///   Whether to force occlusion queries to be rendered and retrieved on teleports.
  inline void SetForceQueryOnTeleport(bool bForce)
  {
    // Retrieving occlusions queries in the same frame they were rendered is not supported on these platforms.
  #if defined(_VISION_PSP2)
    m_bForceQueryOnTeleport = false;
  #else
    m_bForceQueryOnTeleport = bForce;
  #endif
  }

  /// \brief
  ///   Returns whether to force occlusion queries to be rendered and retrieved on teleports.
  ///
  /// Note: On the PSP2 and this setting will be ignored, as rendering and retrieving occlusion queries in the same frame is unsupported on these platforms.
  ///
  /// \return
  ///   Whether occlusion queries are forced to be rendered and retrieved on teleports.
  inline bool IsForceQueryOnTeleport() {return m_bForceQueryOnTeleport;}


  /// \brief
  ///   Access one global instance of a shadow manager
  EFFECTS_IMPEXP static VLensFlareManager& GlobalManager();

  ///
  /// @}
  ///


  /// \brief
  ///   Access the collection of all instances
  inline VLensFlareCollection &Instances() {return m_Instances;}

private:
  struct VLensFlareCandidate
  {
    VLensFlareCandidate() 
      : m_pLensFlare(NULL), m_fCurrentVisibility(0.0f), m_fLastVisibilityQuery(0.0f) {}

    VLensFlareCandidate(VLensFlareComponent* pLensFlare)
      : m_pLensFlare(pLensFlare), m_fCurrentVisibility(0.0f), m_fLastVisibilityQuery(0.0f) {}

    bool operator==(const VLensFlareCandidate& other) const
    { return m_pLensFlare == other.m_pLensFlare; }


    VLensFlareComponent* m_pLensFlare;
    float m_fCurrentVisibility;
    float m_fLastVisibilityQuery;
  };

  void RenderLensFlares (VLensFlareCandidate& lensFlareCandidate, IVRender2DInterface *pRI);

  static VLensFlareManager g_GlobalManager;

  // Helper struct to manage states in all render contexts.
  struct VLensFlareRenderContextState
  {
    VLensFlareRenderContextState()
      : m_Used(0,0), m_Candidates(0,0,0) {}

    inline bool IsBitSet(int iBitNumber) const
    {
      unsigned int *pField = m_Used.GetDataPtr();
      return (pField[iBitNumber>>5] & (1<<(iBitNumber&31)))>0;
    }

    inline void EnsureSize (int iBitCount)
    {
      int iIntCount = (iBitCount >> 5) + 1;
      m_Used.EnsureSize(iIntCount);
    }

    inline void SetBit (int iBitNumber)
    {
      unsigned int *pField = m_Used.GetDataPtr();
      pField[iBitNumber>>5] |= (1<<(iBitNumber&31));
    }

    inline void RemoveBit (int iBitNumber)
    {
      unsigned int *pField = m_Used.GetDataPtr();
      pField[iBitNumber>>5] &= (~(1<<(iBitNumber&31)));
    }

    DynArray_cl<unsigned int> m_Used;
    VArray<VLensFlareCandidate> m_Candidates;
  };
  VArray<VLensFlareRenderContextState> m_State;

  // lens flare instances
  VLensFlareCollection m_Instances;

  int m_iLensFlareRenderHookConst;
  bool m_bForceQueryOnTeleport;
  bool m_bTeleportedLastFrame;
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
