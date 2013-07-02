/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiModuleSystemChild.hpp

#ifndef DEFINE_VISAPIMODULESYSTEMCHILD
#define DEFINE_VISAPIMODULESYSTEMCHILD


#ifndef DEFINE_VISAPIMODULESYSTEMFLAGS
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemFlags.hpp>
#endif

/// \brief
///   Module System Child class
/// 
/// This class is the module system child class for the child side. If you derive your class from
/// this class, then your class will automatically behave as a child. This class offers a couple of
/// NotifyFunctions which will be called at certain times by the parent, which allowes your class
/// to react on certain events.
/// 
/// In order to register your class as a child class of an entity, you have to derive your class
/// from this VisModuleSystemChild_cl class and either pass the parent in the constructor, or set
/// it later by calling the ModSysConnect function.
/// 
/// In the Vision Engine the VisObject3D_cl class behaves as a parent (thus all entities), which
/// allows you to easily attach other objects to entities. Many classes of the API already work as
/// modules, which means that they are derived from this class and are children of their parent
/// class (e.g. VisScreenMask_cl, VisLightSource_cl etc.)
class VisModuleSystemChild_cl : public VRefCounter
{
public:
  
  ///
  /// @name Constructors / Destructor
  /// @{
  ///
  
  /// \brief
  ///   Constructor of the module system child class
  /// 
  /// This constructor of the module system child class registers the child to the parent by
  /// calling the ModSysConnect function. If NULL is passed as a parent, the child won't be
  /// connected and has to be connect manually later.
  /// 
  /// \param parent
  ///   parent class to registered at
  VISION_APIFUNC VisModuleSystemChild_cl(class VisModuleSystemParent_cl *parent = NULL);

  
  /// \brief
  ///   Destructor of the module system child class
  /// 
  /// The destructor of the module system child class disconnects the child from the parent if its
  /// still registered.
  VISION_APIFUNC virtual ~VisModuleSystemChild_cl();  



  ///
  /// @}
  ///

  ///
  /// @name Manual Setup Functions
  /// @{
  ///

  /// \brief
  ///   Register the child manually to the parent
  /// 
  /// ModSysConnect registers the child to the passed parent. 
  /// 
  /// You are responsible for disconnecting it from the previous parent first
  /// 
  /// \param parent
  ///   parent class to register at
  VISION_APIFUNC void ModSysConnect(class VisModuleSystemParent_cl *parent);
  
  
  /// \brief
  ///   Disconnects the registered child from the parent.
  /// 
  /// ModSysDisconnect checks whether the child is still registered at the parent and disconnects
  /// the child if it's necessary.
  VISION_APIFUNC void ModSysDisConnect();


  /// \brief
  ///   Gets the current parent.
  /// 
  /// \return
  ///   VisModuleSystemParent_cl *parent: parent of the child, NULL if child is not connected
  VISION_APIFUNC class VisModuleSystemParent_cl *ModSysGetParent() const;


  ///
  /// @}
  ///

  ///
  /// @name Virtual Functions
  /// @{
  ///

  /// \brief
  ///   You can implement this function in a class that inherits from VisModuleSystemChild_cl to determine which type the child is of.
  /// 
  /// \return
  ///   Returns the RTTI type of the child. Returns NULL if this function is not reimplemented in the class that inherits.

  /// 
  VISION_APIFUNC virtual VType* GetOwnerTypeId()
  {
    return NULL;
  }

  ///
  /// @}
  ///

  ///
  /// @name Virtual Notify Functions
  /// @{
  ///

  /// \brief
  ///   Notification which will be called whenever certain properties of the parent changed
  /// 
  /// The flag parameter defines which data has been modified (see Inputs)
  /// 
  /// \param flags
  ///   Available flags are:
  ///   \li VIS_MODSYSFLAG_POSITION: position has changed, might be called multiple times per frame
  /// 
  ///   \li VIS_MODSYSFLAG_ORIENTATION: orientation has changed, might be called multiple times per
  ///     frame
  /// 
  ///   \li VIS_MODSYSFLAG_FRAMEPOSITION: position has changed, called only once per frame
  /// 
  ///   \li VIS_MODSYSFLAG_FRAMEORIENTATION: orientation has changed, called only once per frame
  /// 
  /// \note
  ///   The available flags are defined in the file "VisApiModuleSystemFlags.hpp".
  /// 
  /// \note
  ///   Since this function is a virtual NotifyFunction, you can use it to get informed whenever
  ///   certain data like the position of the parent changed.
  VISION_APIFUNC virtual void ModSysNotifyFunctionParentAltered(int flags)
  {
  }


  /// \brief
  ///   NotityFunction which will be called whenever the parent is going to be deleted.
  /// 
  /// \note
  ///   Since this function is a virtual NotifyFunction, you can use it to get informed whenever
  ///   the parent is going to be destroyed.
  VISION_APIFUNC virtual void ModSysNotifyFunctionParentDestroyed()
  {
  }

  
  /// \brief
  ///   Notification function which will be called when the child is connected to the parent
  /// 
  /// Any class derived from VRefCounter (e.g. resources) should override this function and
  /// increment the refcounter.
  VISION_APIFUNC virtual void ModSysNotifyFunctionOnConnect()
  {
  }

  /// \brief
  ///   Notification function which will be called when the child is disconnected from the parent
  ///   Any class derived from VRefCounter (e.g. resources) should override this function and
  ///   decrement the refcounter instead of deleting it
  VISION_APIFUNC virtual void ModSysNotifyFunctionOnDisconnect(BOOL bDestroy)
  { 
    if (bDestroy && GetRefCount()==0)
      delete this;
  }

  
  /// \brief
  ///   NotifyFunction which will be called by the parent once per frame
  /// 
  /// It can be used for regular calculations for the child class.
  /// 
  /// \note
  ///   Since this function is a virtual NotifyFunction, you can use it to do regular calculations
  ///   once per frame.
  VISION_APIFUNC virtual void ModSysNotifyFunctionHandle()
  {
  }


  /// \brief
  ///   This notify function will be called by the parent to send a command to the child.
  /// 
  /// \param command
  ///   Available flags are:
  ///   \li VIS_MODSYSCMD_RECOMPUTEVISIBILITY: Child has to recompute the visibility
  ///     (ReComputeVisibility function)

  VISION_APIFUNC virtual void ModSysNotifyFunctionCommand(int command)
  {
  }


  ///
  /// @}
  ///

private:
  class VisModuleSystemParent_cl *modSysParent;          ///< pointer to the parent
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
