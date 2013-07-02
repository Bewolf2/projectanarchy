/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//  Basic Component Template
//  look in the EngineSamples Examples90.sln -> VisionEnginePlugin90 -> components -> VFollowPathCamera.h
//
//  18/01/2011
//  SAMPLEPLUGIN_IMPEXP prefix added + include to new GamePluginModule.h
//

#ifndef MYCOMPONENT_H_INCLUDED
#define MYCOMPONENT_H_INCLUDED

#include "GamePluginModule.h"

// Versions
#define MYCOMPONENT_VERSION_0          0     // Initial version
#define MYCOMPONENT_VERSION_CURRENT    1     // Current version

enum MyTestEnum
{
  V_MYTESTENUM_0 = 0,
  V_MYTESTENUM_1 = 1,
  V_MYTESTENUM_2 = 2,
};

//============================================================================================================
//  MyComponent Class
//============================================================================================================
class MyComponent : public IVObjectComponent
{

public:
  V_DECLARE_SERIAL  ( MyComponent, SAMPLEPLUGIN_IMPEXP ); // for RTTI
  V_DECLARE_VARTABLE( MyComponent, SAMPLEPLUGIN_IMPEXP );

  SAMPLEPLUGIN_IMPEXP MyComponent()
  {
    Vision::Message.reset();
    Vision::Message.Add(1,"myComponent - Class Constructor");
  }

  SAMPLEPLUGIN_IMPEXP ~MyComponent(){}
  
//----------------------------------------------------------------------------------------------------
//  MyComponent::SetOwner(...)
//  Overridden function to respond to owner changes
//----------------------------------------------------------------------------------------------------
  SAMPLEPLUGIN_IMPEXP VOVERRIDE void SetOwner( VisTypedEngineObject_cl *pOwner );


//----------------------------------------------------------------------------------------------------
//  MyComponent::CanAttachToObject(...)
//  Overridden function. Cpmponent can be attached to specified type instances
//----------------------------------------------------------------------------------------------------
  SAMPLEPLUGIN_IMPEXP VOVERRIDE BOOL CanAttachToObject( VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut )
  {
    if ( !IVObjectComponent::CanAttachToObject( pObject, sErrorMsgOut ))return FALSE;  

    //  Define criteria here that allows the editor to attach this component to 
    //  the passed object.In this example, the object should be derived from 
    //  VisObject3D_cl eg it can be positionable. 
    if (!pObject->IsOfType( V_RUNTIME_CLASS( VisObject3D_cl )))
    {
      sErrorMsgOut = "Component can only be added to instances of VisObject3D_cl or derived classes.";
      return FALSE;
    }
    return TRUE;
  }
//----------------------------------------------------------------------------------------------------
//  MyComponent::OnVariableValueChanged(...)
//  Overridden function to respond to variable changes
//----------------------------------------------------------------------------------------------------
  SAMPLEPLUGIN_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
  {}
//----------------------------------------------------------------------------------------------------
//  MyComponent::Serialize(...)
//----------------------------------------------------------------------------------------------------
  SAMPLEPLUGIN_IMPEXP VOVERRIDE void Serialize( VArchive &ar );


//----------------------------------------------------------------------------------------------------
//
//  MyComponent Body 
//
//----------------------------------------------------------------------------------------------------
//  You do not have to call this function manually, since the VPlayableCharacterComponentManager
//  class will take care of this.  
  SAMPLEPLUGIN_IMPEXP void onFrameUpdate();

  SAMPLEPLUGIN_IMPEXP void onStartup( VisTypedEngineObject_cl *pOwner );

  SAMPLEPLUGIN_IMPEXP void onRemove(  VisTypedEngineObject_cl *pOwner );


protected:
  //
  //  Define the variables to use as properties.
  //
  //  Exposed to vForge:
  MyTestEnum  enumDemo;
  BOOL    boolDemo;
  float    floatDemo;
  int      intDemo;
  VColorRef  colorDemo;


  // Not exposed:
  int count;

};


//  Collection for handling playable character component
class MyComponent_Collection : public VRefCountedCollection<MyComponent> {};

//============================================================================================================
//  MyComponent_ComponentManager Class
//============================================================================================================
/// This manager class has a list of all available MyComponent instances
/// and takes care of calling their MyComponent::PerFrameUpdate function
/// on each frame.

class MyComponent_ComponentManager : public IVisCallbackHandler_cl
{
public:

  //   Gets the singleton of the manager
  static MyComponent_ComponentManager &GlobalManager(){  return g_GlobalManager;  }

  //   Should be called at plugin initialization time.
  void OneTimeInit(){  Vision::Callbacks.OnUpdateSceneFinished += this;} // listen to this callback
  
  //   Should be called at plugin de-initialization time
  void OneTimeDeInit(){ Vision::Callbacks.OnUpdateSceneFinished -= this;} // de-register

  //   Callback method that takes care of updating the managed instances each frame
  VOVERRIDE void OnHandleCallback( IVisCallbackDataObject_cl *pData )
  {
    VASSERT( pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished );
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++){ m_Components.GetAt(i)->onFrameUpdate(); }
  }

  //   Gets all VPlayableCharacterComponent instances this manager holds
  inline MyComponent_Collection &Instances() { return m_Components; }

protected:

  /// Holds the collection of all instances of MyComponent
  MyComponent_Collection m_Components;

  /// One global instance of our manager
  static MyComponent_ComponentManager g_GlobalManager;
};


#endif // MYCOMPONENT_H_INCLUDED

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
