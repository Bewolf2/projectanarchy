/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/BaseEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionSerializer.h>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_IMPLEMENT_SERIAL(RPG_BaseEntity, VisBaseEntity_cl, 0, &g_RPGPluginModule);

START_VAR_TABLE (RPG_BaseEntity, VisBaseEntity_cl, "RPG Base Entity", 0, "")
END_VAR_TABLE

namespace
{
  // Transform synchronization
  class TransformHistory : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VisDataHistory_cl<hkvVec3, 3> m_positionData;
    VisDataHistory_cl<hkvVec3, 3, VLerpEulerAngles> m_rotationData;
  };

  class TransformSynchronizationGroup : public IVNetworkSynchronizationGroup
  {
    char const *GetName() const HKV_OVERRIDE;

    VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE;

    IVNetworkSynchronizationGroupInstanceData *CreatePerInstanceData() const HKV_OVERRIDE;

    bool QuerySynchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VMessageSettings& packetSettingsOut) HKV_OVERRIDE;

    void Synchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VArchive& archive) HKV_OVERRIDE;

    void TickFunction(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, float timeDelta) HKV_OVERRIDE;
  }
  g_transformSynchronizationGroup;

  // Visibility synchronization
  class VisibilityHistory : public IVNetworkSynchronizationGroupInstanceData
  {
  public:
    VisDataHistory_cl<unsigned int, 3, VStepFunction<unsigned int> > m_visibilityData;
  };

  class VisibilitySynchronizationGroup : public IVNetworkSynchronizationGroup
  {
  public:
    char const *GetName() const HKV_OVERRIDE;

    VGroupTypeFlags_e GetUsageTypeFlags() const HKV_OVERRIDE;

    IVNetworkSynchronizationGroupInstanceData *CreatePerInstanceData() const HKV_OVERRIDE;

    bool QuerySynchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VMessageSettings& packetSettingsOut) HKV_OVERRIDE;

    void Synchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VArchive& archive) HKV_OVERRIDE;

    void TickFunction(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, float timeDelta) HKV_OVERRIDE;
  }
  g_visibilitySynchronizationGroup;
}

// RPG_BaseEntity

/// Calls the OnInitProperties script method.
void RPG_BaseEntity::InitializeProperties()
{
  TriggerScriptEvent("OnInitProperties");
}

void RPG_BaseEntity::CalcPositionForTargeting(hkvVec3& targetOut) const
{
  targetOut = GetPosition();
}

int RPG_BaseEntity::GetSynchronizationGroupList(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t *destList)
{
  int iCount = VisTypedEngineObject_cl::GetSynchronizationGroupList(networkContext, destList);
  {
    destList[iCount++].Set(this, &g_transformSynchronizationGroup);
    destList[iCount++].Set(this, &g_visibilitySynchronizationGroup);
  }
  return iCount;
}

void RPG_BaseEntity::Serialize(VArchive& ar)
{
  VisBaseEntity_cl::Serialize(ar);

  if(ar.IsLoading())
  {
    RPG_VisionSerializer::ReadVariableList(this, ar);
  }
  else
  {
    RPG_VisionSerializer::WriteVariableList(this, ar);
  }
}

VBool RPG_BaseEntity::WantsDeserializationCallback(const VSerializationContext& context)
{
  return TRUE;
}

void RPG_BaseEntity::OnDeserializationCallback(const VSerializationContext& context)
{
  VisBaseEntity_cl::OnDeserializationCallback(context);

#if (RPG_SERIALIZATION_SCRIPT_PROPERTIES)
  // call the OnInitProperties script method in order to let scripts set any object properties
  InitializeProperties();
#endif

  PostInitialize();
}

void RPG_BaseEntity::PostInitialize()
{
}

namespace
{
  // TransformSynchronizationGroup
  char const *TransformSynchronizationGroup::GetName() const
  {
    return "Transformation";
  }

  IVNetworkSynchronizationGroup::VGroupTypeFlags_e TransformSynchronizationGroup::GetUsageTypeFlags() const
  {
    return VGroupTypeBit_Transformation;
  }

  IVNetworkSynchronizationGroupInstanceData *TransformSynchronizationGroup::CreatePerInstanceData() const
  {
    // TODO: VNetworkSynchronizationGroupInstanceInfo_t dtor calls V_SAFE_FREE on this; should be V_SAFE_DELETE
    return new TransformHistory;
  }

  bool TransformSynchronizationGroup::QuerySynchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VMessageSettings& packetSettingsOut)
  {
    packetSettingsOut.SetReliability(VMR_Unreliable_Sequenced);
    return true;
  }

  void TransformSynchronizationGroup::Synchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VArchive& ar)
  {
    if(ar.IsLoading())
    {
      TransformHistory *const data = static_cast<TransformHistory *>(instanceInfo.m_pCustomData);

      __int64 timeMS;
      {
        ar >> timeMS;
      }

      {
        float x, y, z;
        {
          ar >> x;
          ar >> y;
          ar >> z;
        }
        data->m_positionData.Write(hkvVec3(x, y, z), timeMS);
      }

      {
        VFloat16 x, y, z;
        {
          ar >> x;
          ar >> y;
          ar >> z;
        }
        data->m_rotationData.Write(hkvVec3(x, y, z), timeMS);
      }
    }
    else
    { 
      VisObject3D_cl const *const obj3D = static_cast<VisObject3D_cl *>(instanceInfo.m_pInstance);
      {
        ar << __int64(networkContext.m_iCurrentServerTimeMS);

        hkvVec3 const& pos = obj3D->GetPosition();
        {
          ar << pos.x;
          ar << pos.y;
          ar << pos.z;
        }

        hkvVec3 const& ori = obj3D->GetOrientation();
        {
          ar << VFloat16(ori.x);
          ar << VFloat16(ori.y);
          ar << VFloat16(ori.z);
        }
      }
    }
  }

  void TransformSynchronizationGroup::TickFunction(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, float timeDelta)
  {
    if(networkContext.m_eNetType == VNT_Client)
    {
      if(networkContext.m_iUserID != instanceInfo.m_pComponent->GetNetworkOwnerID())
      {
        VisObject3D_cl *const obj3D = static_cast<VisObject3D_cl *>(instanceInfo.m_pInstance);
        TransformHistory /*const*/ *const transformHistory = static_cast<TransformHistory *>(instanceInfo.m_pCustomData);
        {
          hkvVec3 newPosition(hkvNoInitialization);
          VReadResult_e const res = transformHistory->m_positionData.Interpolate(&newPosition, networkContext.m_iCurrentServerTimeMS - networkContext.m_iInterpolationDelayMS);
          if(res != VRR_None)
          {
            obj3D->SetPosition(newPosition);
          }
        }
        {
          hkvVec3 newRotation(hkvNoInitialization);
          VReadResult_e const res = transformHistory->m_rotationData.Interpolate(&newRotation, networkContext.m_iCurrentServerTimeMS - networkContext.m_iInterpolationDelayMS);
          if(res != VRR_None)
          {
            obj3D->SetOrientation(newRotation);
          }
        }
      }
    }
    else if(networkContext.m_eNetType == VNT_Server)
    {
      IVNetworkSynchronizationGroup::TickFunction(networkContext, instanceInfo, timeDelta);
    }
  }

  // VisibilitySynchronizationGroup
  char const *VisibilitySynchronizationGroup::GetName() const
  {
    return "Visible Status";
  }

  IVNetworkSynchronizationGroup::VGroupTypeFlags_e VisibilitySynchronizationGroup::GetUsageTypeFlags() const
  {
    return VGroupTypeBit_Visibility;
  }

  IVNetworkSynchronizationGroupInstanceData *VisibilitySynchronizationGroup::CreatePerInstanceData() const
  {
    // TODO: VNetworkSynchronizationGroupInstanceInfo_t dtor calls V_SAFE_FREE on this; should be V_SAFE_DELETE
    return new VisibilityHistory;
  }

  bool VisibilitySynchronizationGroup::QuerySynchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VMessageSettings& packetSettingsOut)
  {
    packetSettingsOut.SetReliability(VMR_Unreliable_Sequenced);
    return true;
  }

  void VisibilitySynchronizationGroup::Synchronize(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, VArchive& ar)
  {
    if(ar.IsLoading())
    {
      VisibilityHistory *const data = static_cast<VisibilityHistory *>(instanceInfo.m_pCustomData);

      __int64 timeMS;
      {
        ar >> timeMS;
      }

      unsigned int visibleMask;
      {
        ar >> visibleMask;
      }
      data->m_visibilityData.Write(visibleMask, timeMS);
    }
    else
    { 
      VisBaseEntity_cl const *const baseEnt = static_cast<VisBaseEntity_cl *>(instanceInfo.m_pInstance);
      {
        ar << __int64(networkContext.m_iCurrentServerTimeMS);

        unsigned int const visibleMask = baseEnt->GetVisibleBitmask();
        ar << visibleMask;
      }
    }
  }

  void VisibilitySynchronizationGroup::TickFunction(VNetworkViewContext const& networkContext, VNetworkSynchronizationGroupInstanceInfo_t& instanceInfo, float timeDelta)
  {
    if(networkContext.m_eNetType == VNT_Client)
    {
      if(networkContext.m_iUserID != instanceInfo.m_pComponent->GetNetworkOwnerID())
      {
        VASSERT(networkContext.m_eNetType == VNT_Client);
        VisBaseEntity_cl *const baseEnt = static_cast<VisBaseEntity_cl *>(instanceInfo.m_pInstance);
        VisibilityHistory /*const*/ *const visibilityHistory = static_cast<VisibilityHistory *>(instanceInfo.m_pCustomData);
        {
          unsigned int visibilityMask = 0;
          VReadResult_e const res = visibilityHistory->m_visibilityData.Interpolate(&visibilityMask, networkContext.m_iCurrentServerTimeMS - networkContext.m_iInterpolationDelayMS);
          if(res != VRR_None)
          {
            baseEnt->SetVisibleBitmask(visibilityMask);
          }
        }
      }
    }
    else if(networkContext.m_eNetType == VNT_Server)
    {
      IVNetworkSynchronizationGroup::TickFunction(networkContext, instanceInfo, timeDelta);
    }
  }
}

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
