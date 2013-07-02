/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//  Basic Component Template
//  

#include "GamePluginPCH.h"
#include "myComponent.h"

//  Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL( MyComponent, IVObjectComponent, 0, &g_myComponentModule);

//  Start up our manager class, one global instance of our manager
MyComponent_ComponentManager MyComponent_ComponentManager::g_GlobalManager;

//============================================================================================================
//  MyComponent Body
//============================================================================================================
//  Give the component some in Game or vForge Functionality
//

void MyComponent::onStartup( VisTypedEngineObject_cl *pOwner )
{
  //  do the component start up code here.......
  //  [...]

  Vision::Message.Add(1,"myComponent - onStartup()");
  count = 0;
}

void MyComponent::onRemove(  VisTypedEngineObject_cl *pOwner )
{
  //  do the component Removal code here.......
  //  [...]
  Vision::Message.reset();
  Vision::Message.Add("myComponent - onRemove()");
}

void MyComponent::onFrameUpdate()
{
  if (!GetOwner())return;
  //  Add code here to be performed every frame update.
  //  [...]

  //Example
  hkvVec3 vPos = ((VisObject3D_cl *)GetOwner())->GetPosition();
  Vision::Message.DrawMessage3D("< myComponent Text >",vPos);
  Vision::Message.Print(1, 10, 100, "Counting : %i", count);
  count ++;
}


//============================================================================================================
//  MyComponent Overrides
//============================================================================================================
//  
void MyComponent::SetOwner( VisTypedEngineObject_cl *pOwner )
{
  IVObjectComponent::SetOwner( pOwner );
  if (pOwner!=NULL)
  {
    MyComponent_ComponentManager::GlobalManager().Instances().AddUnique(this);
    Vision::Message.Add("myComponent - Component Instance created");
    onStartup( pOwner );
  }
  else
  {
    onRemove( pOwner );
    Vision::Message.Add("myComponent - Removed Component Instance from component Manager");
    MyComponent_ComponentManager::GlobalManager().Instances().SafeRemove(this);    
  }
}

//----------------------------------------------------------------------------------------------------------  
//  MyComponent::Serialize(..)
//----------------------------------------------------------------------------------------------------------  
//  component's properties are automatically saved and restored on the vForge 
//  side via property reflection, custom code has to be implemented to 
//  serialize the component's properties into the exported vscene file. 
//
//  You should only save what is absolutely necessary for restoring the object's 
//  state and that cannot be recalculated at loading time. 
//----------------------------------------------------------------------------------------------------------
void MyComponent::Serialize( VArchive &ar )
{
  char iLocalVersion = MYCOMPONENT_VERSION_CURRENT ;
  IVObjectComponent::Serialize(ar);
  if (ar.IsLoading())
  {
    ar >> iLocalVersion;
    VASSERT_MSG(iLocalVersion == MYCOMPONENT_VERSION_CURRENT , "Invalid local version.");

    //  add your property variables here
    //  Load Data
    ar >> boolDemo;
    ar >> floatDemo;
    ar >> intDemo;
    ar >> colorDemo;    
  } 
  else
  {
    ar << iLocalVersion;
    
    //  Save Data
    ar << boolDemo;
    ar << floatDemo;
    ar << intDemo; 
    ar << colorDemo;   
  }
}


//============================================================================================================
//  Variable Table - Property variables can be exposed by the programmer 
//  and edited per instance by the artist  
//============================================================================================================
//
START_VAR_TABLE(MyComponent,IVObjectComponent,    "Component Description to show up in the editor", 
                          VVARIABLELIST_FLAGS_NONE, 
                          "myComponentWidget" )
  
  DEFINE_VAR_BOOL    (MyComponent, boolDemo,    "This is an example of using bool, initialises to true", "TRUE", 0, NULL);
  DEFINE_VAR_ENUM    (MyComponent, enumDemo,    "test the enum", "Two", "One,Two,Three", 0, 0);
  DEFINE_VAR_FLOAT  (MyComponent, floatDemo,  "Demo of the float", "5.0", 0, NULL);
  DEFINE_VAR_INT    (MyComponent, intDemo,    "Show the int in use", "3", 0, 0);
  DEFINE_VAR_COLORREF  (MyComponent, colorDemo,  "show a color selection", "255/255/255/255", 0, 0);
  //DEFINE_VAR_STRING  (MyComponent, stringDemo,  "String value", "hello", 0, 0,NULL);


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
