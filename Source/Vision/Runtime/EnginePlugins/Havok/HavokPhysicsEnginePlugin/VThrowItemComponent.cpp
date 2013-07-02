/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/VThrowItemComponent.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

//-------------------------------------------------------------------------//
// Input Map                                                               //
//-------------------------------------------------------------------------//

enum THROW_ITEM_CONTROL 
{
  INPUTMAP_THROWITEM = 0,

  INPUTMAP_THROWITEM_LAST_ELEMENT = INPUTMAP_THROWITEM
};

#define INPUTMAP_THROWITEM_ALTERNATIVES   4


//-------------------------------------------------------------------------//
// Constructor & Destructor                                                //
//-------------------------------------------------------------------------//

VThrowItemComponent::VThrowItemComponent()
{
  // Register Callback
  Vision::Callbacks.OnUpdateSceneBegin += this;

  // Defaults;
  Velocity = 500.f;

  // Input map
  m_pInputMap = new VInputMap(INPUTMAP_THROWITEM_LAST_ELEMENT + 1, INPUTMAP_THROWITEM_ALTERNATIVES);

#if defined (SUPPORTS_KEYBOARD)

  m_pInputMap->MapTrigger(INPUTMAP_THROWITEM, V_KEYBOARD, CT_KB_Q, VInputOptions::OncePerFrame( ONCE_ON_PRESS, 0));

  //m_pInputMap->MapTrigger(INPUTMAP_THROWITEM, VInputManager::GetPad(0), CT_PAD_1, InputOptions::Alternative(0));

#endif
}


VThrowItemComponent::~VThrowItemComponent()
{
  // Register Callback
  Vision::Callbacks.OnUpdateSceneBegin -= this;

  V_SAFE_DELETE(m_pInputMap);
}


//-------------------------------------------------------------------------//
// IVObjectComponent Overrides                                             //
//-------------------------------------------------------------------------//

void VThrowItemComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner!=NULL)
    CommonInit();
  else
    CommonDeInit();
}

BOOL VThrowItemComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  // Define criteria here that allows the editor to attach this component to the passed object.
  // In our example, the object should be derived from VisObject3D_cl to be positionable.
  BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
  if (!bIsValidClass)
  {
    sErrorMsgOut += "Component can only be added to instances of VisBaseEntity_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VThrowItemComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
}

//-------------------------------------------------------------------------//
// Serialization                                                           //
//-------------------------------------------------------------------------//

// register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VThrowItemComponent, IVObjectComponent, 0, &g_vHavokModule);

void VThrowItemComponent::Serialize(VArchive &ar)
{
  char iLocalVersion = 1;

  IVObjectComponent::Serialize(ar);

  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion==1, "Invalid local version. Please re-export");

    ar >> ModelFile; 
    ar >> Velocity;
  }
  else
  {
    ar << iLocalVersion;

    ar << ModelFile;
    ar << Velocity;
  }
}


//-------------------------------------------------------------------------//
// IVisCallbackHandler_cl Overrides                                        //
//-------------------------------------------------------------------------//

void VThrowItemComponent::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  // Check whether we are in "PlayTheGame" mode in vForge
  if (!Vision::Editor.IsPlayingTheGame())
    return;

  PerFrameUpdate();
}


//-------------------------------------------------------------------------//
// Init / DeInit                                                           //
//-------------------------------------------------------------------------//

void VThrowItemComponent::CommonInit()
{
}

void VThrowItemComponent::CommonDeInit()
{
  RemoveAllItems();
}


//-------------------------------------------------------------------------//
// Update Method                                                           //
//-------------------------------------------------------------------------//

void VThrowItemComponent::PerFrameUpdate()
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (pOwner == NULL)
    return;

  // Gather user input
  if (m_pInputMap->GetTrigger(INPUTMAP_THROWITEM) != 0)
    ThrowItem();
}


//-------------------------------------------------------------------------//
// Throw and Remove Method                                                 //
//-------------------------------------------------------------------------//

void VThrowItemComponent::ThrowItem(const hkvVec3& vThrowDir)
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (pOwner == NULL)
    return;
  
  // Check for model filename
  if (ModelFile.IsEmpty())
    return;

  // Spawn entity at camera position
  hkvVec3 vPos = pOwner->GetPosition();
  hkvVec3 vThrowPos = vPos + vThrowDir * 100.f;  
  VisBaseEntity_cl* pEntity = Vision::Game.CreateEntity("VisBaseEntity_cl", vThrowPos, ModelFile);
  pEntity->SetObjectKey("Havok_ThrowItem");
 
  // Check whether model was successfully loaded
  if (!pEntity->HasMesh() || !pEntity->GetMesh()->IsLoaded())
    return;

  // Create Havok Rigid Body component
  vHavokRigidBody* pRigidBody = new vHavokRigidBody();
  pRigidBody->Shape_Type = ShapeType_CONVEX;
  
  // Add the component to the entity
  pEntity->AddComponent(pRigidBody);

  // Throw item
  {
    hkvVec3 vLinVel = vThrowDir * Velocity;
    pRigidBody->SetLinearVelocity(vLinVel);
    hkvVec3 vAngVel(5.0f, 3.0f, 4.0f);
    pRigidBody->SetAngularVelocity(vAngVel);
  }
}

void VThrowItemComponent::ThrowItem()
{
  VisBaseEntity_cl* pOwner = (VisBaseEntity_cl *)GetOwner();
  if (pOwner == NULL)
    return;
  
  ThrowItem(pOwner->GetDirection());
}

void VThrowItemComponent::RemoveAllItems()
{
  VisBaseEntity_cl* pEntity = Vision::Game.SearchEntity("Havok_ThrowItem");

  while (pEntity != NULL)
  {
    pEntity->RemoveAllComponents();
    V_SAFE_DISPOSEOBJECT(pEntity);
    pEntity = Vision::Game.SearchEntity("Havok_ThrowItem");
  }
}


//-------------------------------------------------------------------------//
// Variable Table                                                          //
//-------------------------------------------------------------------------//

START_VAR_TABLE(VThrowItemComponent, IVObjectComponent, "Throw Item Comnponent. Use the Q key to throw an item.", VVARIABLELIST_FLAGS_NONE, "Throw Item" )
  DEFINE_VAR_VSTRING(VThrowItemComponent, ModelFile, "Filename of the model to throw table (.model)", "", 0, 0, "assetpicker(Model)");
  DEFINE_VAR_FLOAT  (VThrowItemComponent, Velocity, "Velocity that is applied to the item", "500.0", 0, NULL); 
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
