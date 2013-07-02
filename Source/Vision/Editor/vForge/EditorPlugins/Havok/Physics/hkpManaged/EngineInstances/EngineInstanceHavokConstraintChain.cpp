/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokDebugConstraintChainRenderer.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraintChain.hpp>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokConstraintChain.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/BallAndSocketConstraintChainState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/StiffSpringConstraintChainState.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace HavokManaged
{
  // --------------------------------------------------------------------------
  EngineInstanceHavokConstraintChain::EngineInstanceHavokConstraintChain(ShapeBase ^ownerShape)
  : EngineInstanceHavokConstraintBase()
  {
    m_eType = HavokConstraintChainType_e::BallAndSocket;
    m_pConstraintChain = NULL;
    m_pState = nullptr;
    m_pOwnerShapeRef = gcnew WeakReference(ownerShape);
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintChain::CanAttachComponent(
    ShapeComponent ^component, String ^%sErrorMsgOut)
  {
    if (m_pConstraintChain)
    {
      return ConversionUtils::CanAttachComponent(m_pConstraintChain, component, sErrorMsgOut);
    }
    else
    {
      vHavokConstraintChain *pTempChain = CreateConstraintChainInstance();
      bool bRes = false;
      if (pTempChain)
        bRes = ConversionUtils::CanAttachComponent(pTempChain, component, sErrorMsgOut);
      V_SAFE_DELETE(pTempChain);
      return bRes;
    }
  }

  // --------------------------------------------------------------------------
  vHavokConstraintChain *EngineInstanceHavokConstraintChain::CreateConstraintChainInstance()
  {
    switch (m_eType)
    {
    case HavokConstraintChainType_e::BallAndSocket:
      return new vHavokBallAndSocketConstraintChain();
      break;
    case HavokConstraintChainType_e::StiffSpring:
      return new vHavokStiffSpringConstraintChain();
      break;
    default:
      VASSERT_MSG(false, "Invalid constraint chain type!");
      return NULL;
    }
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::DisposeObject()
  {
    // Do a ref/unref cycle for all native components owned by this shape. This 
    // allows us to get rid of components that have never been added to a native 
    // Vision class.
    ShapeBase ^pOwnerShape = static_cast<ShapeBase^>(m_pOwnerShapeRef->Target);
    if (pOwnerShape)
    {
      for (int i = 0; i < pOwnerShape->ComponentCount; ++i)
      {
        ShapeComponent ^pShapeComponent = pOwnerShape->Components->get_Item(i);
        if (!pShapeComponent)
          continue;
        IVObjectComponent *pNativeComponent = 
          static_cast<IVObjectComponent*>(pShapeComponent->NativePtr.ToPointer());

        if (pNativeComponent)
        {
          pNativeComponent->AddRef();
          pNativeComponent->Release();
        }
      }
    }

    V_SAFE_RELEASE(m_pConstraintChain);
    EngineInstanceHavokConstraintBase::DisposeObject();
  }

  // --------------------------------------------------------------------------
  float EngineInstanceHavokConstraintChain::GetApproximationError()
  {
    return m_pConstraintChain ? m_pConstraintChain->GetApproximationError() : 0.f;
  }

  // --------------------------------------------------------------------------
  unsigned int EngineInstanceHavokConstraintChain::GetNumLinks()
  {
    return m_pConstraintChain ? m_pConstraintChain->GetNumLinks() : 0;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::OnAttachComponent(ShapeComponent ^component)
  {
    if (m_pConstraintChain)
      ConversionUtils::OnAttachComponent(m_pConstraintChain, component);
  }

  // --------------------------------------------------------------------------
  bool EngineInstanceHavokConstraintChain::OnExport(SceneExportInfo ^info) 
  {
    if (m_pConstraintChain == NULL)
      return true;
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pConstraintChain;
    return true;
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::OnRemoveComponent(ShapeComponent ^component)
  {
    if (m_pConstraintChain)
      ConversionUtils::OnRemoveComponent(m_pConstraintChain, component);
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::SetConstraintChainType(
    HavokConstraintChainType_e type, ConstraintChainStateBase ^state)
  {
    // Remember the new constraint type
    m_pState = state;
    m_eType = type;

    RecreateNativeObject();
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::RecreateNativeObject()
  {
    // Make sure to keep the constraint chain's components alive during
    // recreation of the native object. If they are not used any more,
    // they will be released when this method exits.
    DynObjArray_cl<VSmartPtr<IVObjectComponent>> rgspSavedComponents;
    if (m_pConstraintChain)
    {
      for (int i = 0; i < m_pConstraintChain->Components().Count(); ++i)
        rgspSavedComponents[i] = m_pConstraintChain->Components().GetAt(i);
    }

    // First release the existing constraint
    V_SAFE_RELEASE(m_pConstraintChain);

    // Don't try to re-create if we don't have a pair of anchors
    bool bReCreate = AreBothAnchorsDefined();

    // Also cancel if constraint state is NULL for some reason
    if (bReCreate)
    {
      Debug::Assert(m_pState != nullptr);
      bReCreate &= (m_pState != nullptr);
    }

    // Get the native constraint description from the constraint state
    vHavokConstraintChainDesc *pDesc = NULL;
    if (bReCreate)
    {
      Vector3F position(m_posX, m_posY, m_posZ);
      Vector3F direction(m_dirX, m_dirY, m_dirZ);
      Vector3F orientation(m_oriY, m_oriP, m_oriR);
      pDesc = m_pState->GetNativeDescription(
        m_pFirstAnchor, m_pSecondAnchor, position, direction, orientation);
      bReCreate &= (pDesc != NULL);
    }

    // Create the constraint according to the configured type
    if (bReCreate)
    {
      m_pConstraintChain = CreateConstraintChainInstance();
      if (!m_pConstraintChain || !m_pConstraintChain->Init(*pDesc))
        V_SAFE_DELETE(m_pConstraintChain);
      V_SAFE_DELETE(pDesc);
    }

    // Verify that the constraint could be created. If this is the case, then most
    // likely the constraint configuration was invalid.
    m_bConfigurationValid = (m_pConstraintChain != NULL);

    // Don't continue if we couldn't create the constraint chain
    if (!m_pConstraintChain)
    {
      // If creating the constraint chain failed, keep the components 
      // alive - they may be needed later. If not, they will be freed
      // correctly at disposal.
      for (unsigned int i = 0; i < rgspSavedComponents.GetSize(); ++i)
      {
        IVObjectComponent *pComp = rgspSavedComponents[i];
        if (!pComp)
          continue;

        pComp->AddRef();
        rgspSavedComponents[i] = NULL;
        pComp->ReleaseNoDelete();
      }
      return;
    }

    m_pConstraintChain->AddRef();

    // Get the components of the shape that owns this engine instance, and add
    // them to the native constraint chain.
    ShapeBase ^pOwnerShape = static_cast<ShapeBase^>(m_pOwnerShapeRef->Target);
    if (pOwnerShape)
    {
      for (int i = 0; i < pOwnerShape->ComponentCount; ++i)
      {
        ShapeComponent ^pShapeComponent = pOwnerShape->Components->get_Item(i);
        if (!pShapeComponent)
          continue;
        IVObjectComponent *pNativeComponent = 
          static_cast<IVObjectComponent*>(pShapeComponent->NativePtr.ToPointer());
        if (!pNativeComponent)
          continue;

        m_pConstraintChain->AddComponent(pNativeComponent);
      }
    }

    UpdateObjectKey();
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::SetConstraintChainProperties(
    ConstraintChainStateBase ^state)
  {
    // Set the new constraint state, and recreate the constraint according to it.
    m_pState = state;
    RecreateNativeObject();
  }

  // --------------------------------------------------------------------------
  void EngineInstanceHavokConstraintChain::UpdateObjectKey()
  {
    if (!m_pConstraintChain)
      return;

    VString sKey;
    ConversionUtils::StringToVString(m_sCachedObjectKey, sKey);
    if (sKey.IsEmpty())
      m_pConstraintChain->SetObjectKey(NULL);
    else
      m_pConstraintChain->SetObjectKey(sKey);
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
