/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTypeManager.hpp

#ifndef _VISION_VTYPEMANAGER_HPP
#define _VISION_VTYPEMANAGER_HPP

#include <Vision/Runtime/Base/Container/VMapStrToPtr.hpp>

DECLARE_LIST(VModuleList, VModule, VBASE_IMPEXP)


/// \brief
///   Pure virtual interface that allows for resolving class type names to VType pointers in the type manager
///
/// A use case for the associating a serialization binder with the global type manager are deprecated class names that
/// have been renamed in the source code but are still referenced in serialized archives (e.g. exported scenes). The
/// installed serialization binder allows for on-the-fly match-up of incoming class names.
/// This interface has only one key function: BindToType. This function translates incoming class names (as string) to type pointers (VType).
/// One binder can be installed for a type manager via VTypeManager::SetSerializationBinder.
class IVSerializationBinder
{
public:
  IVSerializationBinder()
  {
  }
  VBASE_IMPEXP virtual ~IVSerializationBinder()
  {
  }

  /// \brief
  ///   Key overridable of this interface. Translates incoming class names (as string) to type pointers (VType)
  ///
  /// This interface can return NULL to go through standard RTTI. Accordingly, implementations can concentrate on
  /// resolving deprecated class names that do not exist in the type manager (anymore).
  ///
  /// \param szClassName
  ///    Incoming class name
  ///
  /// \return
  ///    The type associated with the class name. This interface can return NULL to go through standard RTTI
  VBASE_IMPEXP virtual VType *BindToType(const char *szClassName) = 0;
};

/// \brief
///   Pure virtual interface that is used in cases where the user might want to hook into the object creation process and return instances of a derived class.
///   There is no global factory that can be installed, instead this is up to specific managers to provide a setter for a factory instance.
class IVObjectInstanceFactory
{
public:
  IVObjectInstanceFactory()
  {
  }

  VBASE_IMPEXP virtual ~IVObjectInstanceFactory()
  {
  }

  /// \brief
  ///   Key function of this interface. Override this function and return an instance (or NULL) that is derived from the passed base class
  ///
  /// \param pExpectedBaseType
  ///   The type of the base class
  ///
  /// \return
  ///   A new object that is of class pExpectedBaseType or derived from it. The interface may also return NULL to proceed with
  ///   default behavior (which usually is creating an instance of pExpectedBaseType)
  VBASE_IMPEXP virtual VTypedObject *CreateObjectInstance(VType *pExpectedBaseType) = 0;
};


/// \brief
///   Class that manage the type information of the complete application.
/// 
/// Because we now have type information per plugin and applications,  consisting of many plugins,
/// we have a class that manages the type  information of the complete application. An object of
/// the VTypeManager  is not filled automatically with the information. The application can/has to
/// add each plugin with function calls in the code.
/// 
/// The manager has a own list type information (references) which is the sum of all plugins added
/// to that manager instance. There are functions to add  and remove plugins and to get access to
/// the type information in multiple ways
class VTypeManager
{
public:

  // constructor
  VBASE_IMPEXP VTypeManager();
  
  /// \brief
  ///   Gets the VType static object pointer for the class specified by name.
  /// 
  /// Every class has an static object of type VType, holding important run time informations about
  /// the class. You can retrieve this object by name, using this function, from the global
  /// application type list.
  /// Unknown class names can be resolved by installing a IVSerializationBinder interface via SetSerializationBinder.
  /// In case a serialization binder is installed, its BindToType function is called first. If the return value is still NULL, the type
  /// is looked up in the registered modules.
  /// 
  /// \param szClassName
  ///   class name.
  /// 
  /// \return
  ///   VActionType *: NULL if the class isn't registered (or cannot be resolved by the installed serialization binder).
  VBASE_IMPEXP VType *GetType(const char *szClassName);
  
  /// \brief
  ///   Function for iterating trought the applications global type list.
  /// 
  /// \example
  ///   \code
  ///   VPOSITION pos = g_pApp->m_TypeManager.GetStartType();
  ///   while ( pos )
  ///   {
  ///     VType *pType = GetNextType(pos);
  ///     ... do something with pType ...
  ///   }
  ///   \endcode
  VBASE_IMPEXP VPOSITION GetStartType();

  /// \brief
  ///   Function for iterating trought the applications global type list.
  /// 
  /// \param pos
  ///   VPOSITION iterator representing the current position in the list.
  ///
  /// \example
  ///   \code
  ///   VPOSITION pos = g_pApp->m_TypeManager.GetStartType();
  ///   while ( pos )
  ///   {
  ///     VType *pType = GetNextType(pos);
  ///     ... do something with pType ...
  ///   }
  ///   \endcode
  VBASE_IMPEXP VType *GetNextType(VPOSITION &pos);
  
  /// \brief
  ///   Returns the registered class count in the type list.
  /// 
  /// \return
  ///   static int: Total count of all registered classes.
  VBASE_IMPEXP int GetTypeCount();
  
  /// \brief
  ///   Registers all class type informations from this plugin.
  /// 
  /// \param pModule
  ///   plugin to register.
  /// 
  /// \return
  ///   static VBool: TRUE if succesful.
  VBASE_IMPEXP VBool RegisterModule(VModule *pModule);

  /// \brief
  ///   Unregisters all class type informations from this plugin (if registered before).
  /// 
  /// \param pModule
  ///   plugin to unregister.
  /// 
  /// \return
  ///   static VBool: TRUE if succesful.
  VBASE_IMPEXP VBool UnregisterModule(VModule *pModule);

  /// \brief
  ///   Returns a pointer to the list of registered modules
  inline VModuleList* GetModuleList()  { return &m_moduleList; }

  /// \brief
  ///   Returns a list of registered modules references by a specific plugin
  VBASE_IMPEXP VModuleList* GetModuleList(const IVisPlugin_cl *pPlugin);

  /// \brief
  ///   Associates a serialization binder with this type manager. The pointer can be NULL. See class IVSerializationBinder
  inline void SetSerializationBinder(IVSerializationBinder *pBinder)
  {
    m_pBinder = pBinder;
  }

  /// \brief
  ///   Returns the serialization binder that is currently associated with this type manager via SetSerializationBinder. The default is NULL.
  inline IVSerializationBinder *GetSerializationBinder() const
  {
    return m_pBinder;
  }

protected:
  VMapStrToPtr m_classNameMap; ///< applications global class type list
  VModuleList m_moduleList; ///< list of registered plugins
  IVSerializationBinder *m_pBinder;
};

#endif // _VISION_VTYPEMANAGER_HPP

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
