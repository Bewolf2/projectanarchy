/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVisibilityLODHysteresis.hpp

#ifndef _VISAPIVISIBILITYLODHYSTERESIS_HPP_INCLUDED_
#define _VISAPIVISIBILITYLODHYSTERESIS_HPP_INCLUDED_

#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityData.hpp>


/// \brief
///   Enumeration used to specify which the scene element types that supports LOD hysteresis thresholding
enum VLODHysteresisType_e
{
  VLHT_WORLDGEOMETRY = 0, ///< world geometry type
  VLHT_ENTITIES,          ///< entity type
  
  VLHT_COUNT              ///< number of types
};


/// \brief
///   Enumeration used to specify the states of a LOD instance; initial value is always VLHS_UNINITIALIZED on first use
enum VLODHysteresisStates_e
{
  VLHS_UNINITIALIZED = 0,     ///< State is uninitialized and will be initialized on first use to one of the three following enums based on the current distance to camera
  VLHS_BEFORE_NEAR,           ///< LOD object is before the near clipping plane
  VLHS_IN_BETWEEN_NEAR_FAR,   ///< LOD object is in between the near and far clipping planes
  VLHS_BEHIND_FAR             ///< LOD object is behind the far clipping plane
};

/// \brief
///   Abstract component class that can be derived by other component classes that supports LOD hysteresis to retrieve the current LOD level
class IVLODHysteresisComponent : public IVObjectComponent
{
public:
  /// \brief
  ///   Constructor of the LOD hysteresis component interface
  VISION_APIFUNC IVLODHysteresisComponent(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Virtual destructor of the LOD hysteresis component interface
  VISION_APIFUNC virtual ~IVLODHysteresisComponent();

public:
  /// \brief
  ///   Virtual function that needs to be overridden by derived classes to provide the current LOD level
  VISION_APIFUNC virtual int GetLODLevel() const = 0;

  V_DECLARE_DYNAMIC_DLLEXP(IVLODHysteresisComponent, VISION_APIDATA)
};


/// \brief
///   Data used for workflow streams on PS3 (currently unused)
struct VLODHysteresisData_t
{
  unsigned int eType      : 3;  ///> Type of LOD object; casted to "VLodHysteresisType_e"
  unsigned int eState     : 2;  ///> Current state of LOD object; casted to "VLodHysteresisStates_e"
  unsigned int iElementID : 27; ///> Position in element manager
};
V_COMPILE_ASSERT(sizeof(VLODHysteresisData_t) == sizeof(unsigned int));


/// \brief
///   Manager class that handles that states of different object types supporting LOD hysteresis thresholding. See VisionVisibilityCollector_cl class.
class VLODHysteresisManager : public VRefCounter, public IVisCallbackHandler_cl
{
public:
  /// \brief
  ///   Constructor of the LOD hysteresis manager
  VISION_APIFUNC VLODHysteresisManager();

  /// \brief
  ///   Virtual destructor of the LOD hysteresis manager
  VISION_APIFUNC virtual ~VLODHysteresisManager();

  /// \brief
  ///   Sets the threshold in world space units for a specific object type (e.g. world geometry or entities).
  VISION_APIFUNC static void SetThreshold(VLODHysteresisType_e eType, float aValue);

  /// \brief
  ///   Gets the threshold in world space units for a specific object type (e.g. world geometry or entities).
  VISION_APIFUNC static float GetThreshold(VLODHysteresisType_e eType);

protected:
  friend class VSceneLoader;
  friend class VSceneExporter;

  /// \brief
  ///   Serializes the global threshold settings; may only be used by VSceneLoader and VSceneExporter.
  VISION_APIFUNC static void SerializeX(VArchive& ar);

protected:
  #ifndef _VISION_DOC
    typedef unsigned char StateValue;
    typedef DynArray_cl<StateValue> StateArray;

    static const int VIS_CURR_FRAME_SHIFT = ((sizeof(StateValue) * 8) - 2); // shift value to access bit which indicates far/near clip visibility in current frame
    static const int VIS_LAST_FRAME_SHIFT = ((sizeof(StateValue) * 8) - 1); // shift value to access bit which indicates far/near clip visibility in last frame
  #endif //_VISION_DOC

  StateArray* m_pStates[VLHT_COUNT];  ///< Current states for all type of objects

public:
  /// \brief
  ///   Performs near/far clipping equal to VVisibilityData::IsClipped, but takes LOD hysteresis thresholding into account
  inline bool IsClipped(VLODHysteresisType_e eType, int iElementID, int iLODLevel, const VVisibilityData* pVisData, int iFilterMask,
                        const hkvVec3& vCameraPos, float fLODScaleSqr)
  {
    VASSERT(pVisData);
    
    m_pStates[eType]->EnsureSize(iElementID + 1);
    return IsClipped(eType, m_pStates[eType]->GetDataPtr()[iElementID], iLODLevel, pVisData, iFilterMask, vCameraPos, fLODScaleSqr);
  }

  /// \brief
  ///   Performs near/far clipping equal to VVisibilityData::IsClipped, but takes LOD hysteresis thresholding into account
  inline bool IsClipped(VLODHysteresisType_e eType, StateValue& stateValue, int iLODLevel, const VVisibilityData* pVisData, int iFilterMask,
                        const hkvVec3& vCameraPos, float fLODScaleSqr)
  {
    VASSERT(pVisData);

    if ((pVisData->GetVisibleBitmask() & iFilterMask) == 0 || pVisData->GetClipMode(VIS_EXCLUDED_FROM_VISTEST | VIS_IS_INACTIVE))
      return true;

    if (fLODScaleSqr < 0.0f)
      return false;

    int iLODMode = pVisData->GetClipMode();
    float fDistSqr;

    switch (iLODMode)
    {
      case VIS_LOD_TEST_NONE:
        return false;
      case VIS_LOD_TEST_CLIPPOSITION:
        fDistSqr = vCameraPos.getDistanceToSquared(pVisData->GetClipReference());
        break;
      case VIS_LOD_TEST_CLIPPOSITION | VIS_LOD_TEST_APPLYLODSCALING:
        fDistSqr = vCameraPos.getDistanceToSquared(pVisData->GetClipReference()) * fLODScaleSqr;
        break;
      case VIS_LOD_TEST_BOUNDINGBOX:
        fDistSqr = pVisData->GetBoundingBox().getDistanceToSquared(vCameraPos);
        break;
      case VIS_LOD_TEST_BOUNDINGBOX | VIS_LOD_TEST_APPLYLODSCALING:
        fDistSqr = pVisData->GetBoundingBox().getDistanceToSquared(vCameraPos) * fLODScaleSqr;
        break;
      default:
        VASSERT_MSG(false, "Invalid combination of LOD flags");
        return false;
    }

    float fNearPlane = pVisData->GetNearClipDistance();
    float fFarPlane = pVisData->GetFarClipDistance();
    const float fThreshold = GetThreshold(eType);

    VLODHysteresisStates_e eState = (VLODHysteresisStates_e)(stateValue & 3);

    switch (eState)
    {
      case VLHS_UNINITIALIZED:
      {
        if (fDistSqr < (fNearPlane + fThreshold) * (fNearPlane + fThreshold))
          eState = VLHS_BEFORE_NEAR;
        else if (fDistSqr <= (fFarPlane + fThreshold) * (fFarPlane + fThreshold))
          eState = VLHS_IN_BETWEEN_NEAR_FAR;
        else /* (fDistSqr > (fFarPlane + fThreshold) * (fFarPlane + fThreshold)) */
          eState = VLHS_BEHIND_FAR;
        
        // --- no break - fall through
      }

      case VLHS_BEFORE_NEAR:
      {
        if (fDistSqr > (fFarPlane + fThreshold) * (fFarPlane + fThreshold))
          eState = VLHS_BEHIND_FAR;
        else if (fDistSqr > (fNearPlane + fThreshold) * (fNearPlane + fThreshold))
          eState = VLHS_IN_BETWEEN_NEAR_FAR;

        fNearPlane += (fNearPlane > 0.0f)? fThreshold : 0.0f;
        fFarPlane += (fFarPlane > 0.0f)? fThreshold : 0.0f;

        break;
      }

      case VLHS_IN_BETWEEN_NEAR_FAR:
      {
        if (fDistSqr > (fFarPlane + fThreshold) * (fFarPlane + fThreshold))
          eState = VLHS_BEHIND_FAR;
        else if (fDistSqr <= fNearPlane * fNearPlane)
          eState = VLHS_BEFORE_NEAR;

        fFarPlane += (fFarPlane > 0.0f)? fThreshold : 0.0f;

        break;
      }

      case VLHS_BEHIND_FAR:
      {
        if (fDistSqr <= fNearPlane * fNearPlane)
          eState = VLHS_BEFORE_NEAR;
        else if (fDistSqr <= fFarPlane * fFarPlane)
          eState = VLHS_IN_BETWEEN_NEAR_FAR;

        break;
      }

      default:
      {
        VASSERT_MSG(false, "Illegal hysteresis state");
        return false;
      }
    }

    stateValue = (stateValue & ~3) | (StateValue)eState;
    bool bClipped = ((fNearPlane > 0.0f) && (fDistSqr < (fNearPlane * fNearPlane))) ||
                    ((fFarPlane > 0.0f) && (fDistSqr >= (fFarPlane * fFarPlane)));

    #ifdef WIN32
      // Move current frame's visibility state to last frame's one
      // Note this is only required for LOD dissolve feature of simulation package, so no need to support it on other platforms than PC
      const StateValue mask = V_BIT(VIS_CURR_FRAME_SHIFT) | V_BIT(VIS_LAST_FRAME_SHIFT);
      const StateValue shiftedValue = ((stateValue & mask) << 1);
      stateValue = (stateValue & ~mask) | shiftedValue;
      if (bClipped)
        stateValue |= V_BIT(VIS_CURR_FRAME_SHIFT);

      int iLastLODLevel = (stateValue & 0x3c) >> 2;
      stateValue = (stateValue & ~0x3c) | (iLODLevel << 2);
      if (iLastLODLevel != iLODLevel)
        stateValue |= V_BIT(VIS_LAST_FRAME_SHIFT);
    #endif //WIN32

    return bClipped;
  }

  #ifdef WIN32
    /// \brief
    ///   Checks whether the object with the given element ID and type was near/far clipped last frame, but is visible in current frame
    inline bool CheckGetsVisible(VLODHysteresisType_e eType, int iElementID)
    {
      m_pStates[eType]->EnsureSize(iElementID + 1);
      return CheckGetsVisible(m_pStates[eType]->GetDataPtr()[iElementID]);
    }

    /// \brief
    ///   Checks whether the object with the given element ID and type was near/far clipped last frame, but is visible in current frame
    inline bool CheckGetsVisible(StateValue& stateValue)
    {
      return (stateValue & V_BIT(VIS_LAST_FRAME_SHIFT)) != 0;
    }
  #endif //WIN32

protected:
  /// \brief
  ///   Overridden OnHandleCallback function used to de-initialize states of destroyed objects.
  virtual void OnHandleCallback(IVisCallbackDataObject_cl* pData) HKV_OVERRIDE;

protected:
  VISION_APIDATA static float s_fLODHysteresisThresholds[VLHT_COUNT]; ///< Global threshold settings for all supported geometry types
};


#endif //_VISAPIVISIBILITYLODHYSTERESIS_HPP_INCLUDED_

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
