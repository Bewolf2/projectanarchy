/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VFollowPathComponent.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


/// =========================================================================== ///
/// Follow Path Component Methods                                               ///
/// =========================================================================== ///

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VFollowPathComponent,IVObjectComponent,0,&g_VisionEngineModule);

// One global instance of our manager
VFollowPathComponentManager VFollowPathComponentManager::g_GlobalManager;

VFollowPathComponent::VFollowPathComponent(int iComponentFlags) : IVObjectComponent(0, iComponentFlags)
{
  m_pPath = NULL;
  Looped = TRUE;    
  Direction = FALSE;
  ConstantSpeed = TRUE;
  Time = 5.f;
  InitialOffset = 0.f;
  m_bFirstFrame = true;
}

VFollowPathComponent::~VFollowPathComponent()
{
}

void VFollowPathComponent::Init()
{
  // Get path by path key
  if (PathObject.GetReferencedObject() != NULL)
    m_pPath = (VisPath_cl*)PathObject.GetReferencedObject();
  else if (!PathKey.IsEmpty())
    m_pPath = Vision::Game.SearchPath(PathKey.AsChar());  

  if (!m_pPath)
    Vision::Error.Warning("Follow Path Component: The follow path component does not have any reference to a path object.");

  // Enable euler angles
  if (m_pOwner)
    ((VisObject3D_cl *)m_pOwner)->SetUseEulerAngles(TRUE);

  m_fCurrentTime = InitialOffset * Time;
}

void VFollowPathComponent::PerFrameUpdate()
{
  if (!GetOwner())
    return;

  // Check whether path object is set. If not look for the respective path and if no path
  // object can be found output a warning. This is only done in the first frame update of 
  // this component as we do not want to call the init function each frame.
  if (!m_pPath)
  {
    if (m_bFirstFrame)
    {
      Init();
      m_bFirstFrame = false;
      if (!m_pPath)
        return;
    }
    return;
  }

  // Get owner 3D object
  VisObject3D_cl *pObject3D = (VisObject3D_cl *)m_pOwner;

  // Get time since last frame
  m_fCurrentTime += Vision::GetTimer()->GetTimeDifference();
  if (m_fCurrentTime > Time)
  {
    if (Looped)
      m_fCurrentTime = 0.f;
    else
      return;
  }

  // Calculate current relative parameter value [0..1]
  float fCurrentParam = m_fCurrentTime / Time;

  // Evaluate current position on path
  hkvVec3 vPos;
  hkvVec3 vDir;

  if (Direction)
  {
    if (ConstantSpeed)
      m_pPath->EvalPointSmooth(fCurrentParam, vPos, &vDir, NULL);
    else
      m_pPath->EvalPoint(fCurrentParam, vPos, &vDir, NULL);
    pObject3D->SetUseEulerAngles(true); 
    pObject3D->SetPosition(vPos + PositionOffset);
    pObject3D->SetDirection(vDir);
    pObject3D->IncOrientation(OrientationOffset);
  }
  else
  {
    if (ConstantSpeed)
      m_pPath->EvalPointSmooth(fCurrentParam, vPos, NULL, NULL);
    else
      m_pPath->EvalPoint(fCurrentParam, vPos, NULL, NULL);
    pObject3D->SetPosition(vPos + PositionOffset);
  }
}


/// =========================================================================== ///
/// IVObjectComponent Overrides                                                 ///
/// =========================================================================== ///

BOOL VFollowPathComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return false;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisObject3D_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisObject3D_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

void VFollowPathComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner(pOwner);

  // Insert code here to respond to attaching this component to an object
  // This function is called with pOwner==NULL when de-attaching.
  if (pOwner != NULL)
  {
    VFollowPathComponentManager::GlobalManager().Instances().AddUnique(this);
  }
  else
  {
    VFollowPathComponentManager::GlobalManager().Instances().SafeRemove(this);
  }
}

void VFollowPathComponent::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
  if (!strcmp(pVar->GetName(), "PathObject"))
  {
    if (value == NULL)
      return;

    VTypedObject* pTemp = NULL;
    if (sscanf(value, "%p", &pTemp) == -1)
      return;

    if (pTemp == NULL)
      return;

    PathObject.SetReferencedObject(pTemp);
    Init();
  }
}

void VFollowPathComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = FOLLOWPATHCOMPONENT_VERSION_CURRENT;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    m_pPath = NULL;
    if (iLocalVersion>=FOLLOWPATHCOMPONENT_VERSION_1)
      ar >> m_pPath;
    if (m_pPath==NULL)
      ar >> PathKey;
    ar >> Time;
    ar >> InitialOffset;
    ar >> Looped;
    ar >> Direction;
    if (iLocalVersion>=FOLLOWPATHCOMPONENT_VERSION_2)
      ar >> ConstantSpeed;
    PositionOffset.SerializeAsVisVector (ar);
    OrientationOffset.SerializeAsVisVector (ar);

    PathObject.SetReferencedObject(m_pPath);
  } 
  else
  {
    ar << iLocalVersion;
    ar << m_pPath;
    if (m_pPath==NULL) // only serialize key in this case
      ar << PathKey;
    ar << Time;
    ar << InitialOffset;
    ar << Looped; 
    ar << Direction;
    ar << ConstantSpeed;
    PositionOffset.SerializeAsVisVector (ar);
    OrientationOffset.SerializeAsVisVector (ar);
  }
}


void VFollowPathComponentManager::SetHandleOnUpdateSceneBegin(bool bOnUpdateSceneBegin)
{
  // unregister old callback
  if(m_bHandleOnUpdateSceneBegin)
  {
    Vision::Callbacks.OnUpdateSceneBegin -= this;
  }
  else
  {
    Vision::Callbacks.OnUpdateSceneFinished -= this;
  }

  // register new callback
  if(bOnUpdateSceneBegin)
  {
    Vision::Callbacks.OnUpdateSceneBegin += this;
  }
  else
  {
    Vision::Callbacks.OnUpdateSceneFinished += this;
  }

  m_bHandleOnUpdateSceneBegin = bOnUpdateSceneBegin;
}

/// =========================================================================== ///
/// VFollowPathComponentManager IVisCallbackHandler_cl Overrides                ///
/// =========================================================================== ///

void VFollowPathComponentManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin || pData->m_pSender == &Vision::Callbacks.OnUpdateSceneFinished)
  {
    // call update function on every component
    if (Vision::Editor.IsPlaying())
    {
      const int iCount = m_Components.Count();
      for (int i=0;i<iCount;i++)
        m_Components.GetAt(i)->PerFrameUpdate();
    }
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  {
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
      m_Components.GetAt(i)->Init();
  }
}


/// =========================================================================== ///
/// VFollowPathComponent Variable Table                                         ///
/// =========================================================================== ///

START_VAR_TABLE(VFollowPathComponent,IVObjectComponent, "Follow Path Component. Can be attached to any 3D object that will follow a path shape specified with the path key.", VVARIABLELIST_FLAGS_NONE, "Follow Path" )
  DEFINE_VAR_VSTRING(VFollowPathComponent, PathKey, "The object key of the path shape to follow. Obsolete if a PathObject is specified directly", "", 0,0, NULL);
  DEFINE_VAR_OBJECT_REFERENCE(VFollowPathComponent, PathObject, "Reference to the path the owner object should move on", NULL, 0, 0);
  DEFINE_VAR_FLOAT(VFollowPathComponent, Time, "The time the owner object needs to move along the path", "5.0", 0, NULL);
  DEFINE_VAR_FLOAT(VFollowPathComponent, InitialOffset, "Initial offset on the path [0..1]", "0.0", 0, "Slider(0,1)");
  DEFINE_VAR_BOOL(VFollowPathComponent, Looped, "If true, the owner object will move along the path in loop", "TRUE", 0, NULL);
  DEFINE_VAR_BOOL(VFollowPathComponent, Direction, "If true, the owner will rotate according to the path (first derivative)", "TRUE", 0, NULL);
  DEFINE_VAR_BOOL(VFollowPathComponent, ConstantSpeed, "If true, the speed along the path is constant regardless of the path node distribution", "TRUE", 0, NULL);
  DEFINE_VAR_VECTOR_FLOAT(VFollowPathComponent, PositionOffset, "Position offset relative to the evaluated path point position", "0.0/0.0/0.0", 0, 0); 
  DEFINE_VAR_VECTOR_FLOAT(VFollowPathComponent, OrientationOffset, "Orientation offset relative to the evaluated path point direction (yaw, pitch, roll)", "0.0/0.0/0.0", 0, 0); 
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
