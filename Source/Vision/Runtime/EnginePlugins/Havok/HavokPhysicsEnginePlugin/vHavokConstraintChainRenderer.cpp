/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChain.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChainRenderer.hpp>

#include <Vision/Runtime/Engine/System/VisApiSerialization.hpp>


// ----------------------------------------------------------------------------
// vHavokConstraintChainRendererBase
// ----------------------------------------------------------------------------
V_IMPLEMENT_SERIAL(vHavokConstraintChainRendererBase, IVObjectComponent, 0, &g_vHavokModule);

// ----------------------------------------------------------------------------
const unsigned int vHavokConstraintChainRendererBase::s_iSerialVersion = 1;

// ----------------------------------------------------------------------------
vHavokConstraintChainRendererBase::vHavokConstraintChainRendererBase(
  int iComponentFlags/* =VIS_OBJECTCOMPONENTFLAG_NONE */)
: IVObjectComponent((const char*)NULL, iComponentFlags)
{
  m_pConstraintChain = NULL;
}

// ----------------------------------------------------------------------------
vHavokConstraintChainRendererBase::~vHavokConstraintChainRendererBase()
{
  CommonDeinit();
}

// ----------------------------------------------------------------------------
BOOL vHavokConstraintChainRendererBase::CanAttachToObject(
  VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(vHavokConstraintChain)))
  {
    sErrorMsgOut = "Component can only be added to Havok constraint chains.";
    return FALSE;
  }

  return TRUE;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::CommonDeinit()
{
  Vision::Callbacks.OnRenderHook -= this;
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  m_pConstraintChain = NULL;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::CommonInit()
{
  CommonDeinit();

  if (!DoInit())
    CommonDeinit();
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::DisposeObject()
{
  CommonDeinit();
  IVObjectComponent::DisposeObject();
}

// ----------------------------------------------------------------------------
bool vHavokConstraintChainRendererBase::DoInit()
{
  // Get the constraint chain that owns this component
  VisTypedEngineObject_cl *pOwner = GetOwner();
  m_pConstraintChain = (pOwner && pOwner->IsOfType(V_RUNTIME_CLASS(vHavokConstraintChain)))
    ? static_cast<vHavokConstraintChain*>(pOwner)
    : NULL;
  if (!m_pConstraintChain)
    return false;

  // Register as a rendering hook
  Vision::Callbacks.OnRenderHook += this;
  Vision::Callbacks.OnUpdateSceneBegin += this;

  return true;
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::OnDeserializationCallback(const VSerializationContext &context)
{
  // The owner constraint chain should now be established; we can init this 
  // renderer now.
  CommonInit();
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnRenderHook)
  {
    VisRenderHookDataObject_cl *pHookData =
      static_cast<VisRenderHookDataObject_cl*>(pData);
    if (pHookData->m_iEntryConst != VRH_PRE_OCCLUSION_TESTS)
      return;

    OnRender();
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    OnUpdate();
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::OnRender()
{
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::OnUpdate()
{
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::Serialize(VArchive &ar)
{
  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    unsigned int iVersion = 0;
    ar >> iVersion;
    VASSERT_MSG((iVersion > 0) && (iVersion <= s_iSerialVersion), "Invalid version of serialized data!");
  }
  else
  {
    ar << s_iSerialVersion;
  }
}

// ----------------------------------------------------------------------------
void vHavokConstraintChainRendererBase::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);
  CommonInit();
}

// ----------------------------------------------------------------------------
START_VAR_TABLE(vHavokConstraintChainRendererBase, IVObjectComponent, "Base class for all vHavok constraint chain renderers", VFORGE_HIDECLASS, "Havok Constraint Chain Renderer (Base Class)" )
END_VAR_TABLE

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
