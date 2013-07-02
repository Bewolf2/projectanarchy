/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vCompiledShaderManager.hpp

#ifndef VCOMPILEDSHADERMANAGER_HPP_INCLUDED
#define VCOMPILEDSHADERMANAGER_HPP_INCLUDED

#include <Vision/Runtime/Base/Graphics/Shader/vShaderIncludes.hpp>


/// \brief
///   Class for storing a list of callback functions, i.e. function name/pointer pairs
class VCallbackList
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor
  VCallbackList()             {m_iCount=0;}

  /// \brief
  ///   Destructor
  virtual ~VCallbackList()    {UnregisterAll();}


  ///
  /// @}
  ///

  ///
  /// @name Access Functions
  /// @{
  ///


  /// \brief
  ///   Returns the number of registered functions
  inline int GetCount() const {return m_iCount;}

  /// \brief
  ///   Accesses the function name and pointer the at specified index
  inline void GetAtIndex(int iIndex, const char *&key, void*& rValue) const {VASSERT(iIndex>=0 && iIndex<m_iCount);key=m_FuncName.GetDataPtr()[iIndex];rValue=m_FuncPtr.GetDataPtr()[iIndex];}

  /// \brief
  ///   Registers a callback function. If a function with specified name exists, update its pointer
  VBASE_IMPEXP bool RegisterFunction(const char *szFunctionName, void *pFunctionPtr);

  /// \brief
  ///   Remove a callback function from this list
  VBASE_IMPEXP bool UnregisterFunction(const char *szFunctionName);

  /// \brief
  ///   Unregister all callback functions
  inline void UnregisterAll() {m_iCount=0;m_FuncName.Reset();m_FuncPtr.Reset();}

  /// \brief
  ///   Return the function's pointer (or NULL if not registered)
  VBASE_IMPEXP void *GetByName(const char *szFunctionName) const;
  ///
  /// @}
  ///

private:
  int m_iCount;
  DynObjArray_cl<VString> m_FuncName;
  DynArray_cl<void *> m_FuncPtr;
};



/// \brief
///   Class that manages a list of individual compiled shader pass instances (VCompiledShaderPass objects).
///
/// The VCompiledShaderManager class is used internally by the Vision shader system. For instance, the VCompiledShadereffect class uses
/// an instance of VCompiledShaderManager to keep track of its shader passes.
class VCompiledShaderManager : public VRefCountedCollection<VCompiledShaderPass>
{
public:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  /// \brief
  ///   Constructor that initializes this collection and determines which texture loader and
  ///   dynamic type manager should be used to create new shader instances
  VBASE_IMPEXP VCompiledShaderManager(IVTextureLoader &textureLoader, VTypeManager &typeManager)
    : TextureLoader(textureLoader), TypeManager(typeManager) 
  {
  }

  /// \brief
  ///   Destructor.
  VBASE_IMPEXP virtual ~VCompiledShaderManager() {}

  ///
  /// @}
  ///

  ///
  /// @name Looking up Shaders
  /// @{
  ///


  /// \brief
  ///   Finds a shader pass that has the same properties as the passed shader. Uses the
  ///   VCompiledShaderPass::IsEqual function for comparison
  VBASE_IMPEXP VCompiledShaderPass *FindShader(VCompiledShaderPass *pShader);

  /// \brief
  ///   Adds a new shader pass to this collection
  /// 
  /// The flags determine whether the passed instance is directly added to the collection, or whether it is cloned.
  /// 
  /// \param pShader
  ///   The shader pass to be registered
  /// 
  /// \param bForceUnique
  ///   If false, it will be tested if a matching instance already exists. If this is the case, the shader pass won't be added again. If true, the pass will always be added.
  /// 
  /// \param bClone
  ///   If true, a clone of this shader pass will be added instead of the instance itself
  /// 
  /// \return
  ///   VCompiledShaderPass *pRegistered : The registered instance. This can be a) a compatible one
  ///   that is already in the list, b) a clone of the passed instance or c) the instance itself.
  VBASE_IMPEXP VCompiledShaderPass *RegisterShader(VCompiledShaderPass *pShader, bool bForceUnique=false, bool bClone=false);

  /// \brief
  ///   Helper function to create a class instance.
  /// 
  /// This function uses the dynamic type manager to create shader class instances.
  /// 
  /// \param szClassName
  ///   The class name of the shader class
  /// 
  /// \param bAllowBaseFallback
  ///   If true, this function falls back to the VCompiledShaderPass base class if a class name of
  ///   the specified name is not registered. Otherwise this function returns NULL for unknown
  ///   class names.
  /// 
  /// \param pLog
  ///   Optional log for log output (e.g. warnings)
  /// 
  /// \return
  ///   VCompiledShaderPass *pNewInst : A new class instance of class szClassName (or VCompiledShaderPass)
  VBASE_IMPEXP VCompiledShaderPass* CreateCompiledShaderInstance(const char *szClassName,bool bAllowBaseFallback=true, IVLog *pLog=NULL) const;

  /// \brief
  ///   Removes all shader pass instances from this collection that are not referenced anymore
  ///   (refcounter==1)
  VBASE_IMPEXP int PurgeUnused();

  /// \brief
  ///   Overrides the Clear function of the collection
  VBASE_IMPEXP void Clear();

  /// \brief
  ///   Callback function which is called whenever a shader pass is destroyed. Internally used to
  ///   unbind shaders, states etc. from the pipeline before they are destroyed.
  VBASE_IMPEXP virtual void OnShaderPassDestroyed(VCompiledShaderPass *pPass);


  ///
  /// @}
  ///

  ///
  /// @name Callbacks
  /// @{
  ///

  /// \brief
  ///   Registers a per-primitive callback function. Wraps around VCallbackList::RegisterFunction on its m_PerPrimitiveCallbacks member
  inline bool RegisterPerPrimitiveCallback(const char *szFunctionName, VISSHADERCALLBACKFUNC functionPtr) {return m_PerPrimitiveCallbacks.RegisterFunction(szFunctionName, (void*)functionPtr);}

  /// \brief
  ///   Unregisters a per-primitive callback function. Wraps around VCallbackList::UnregisterFunction on its m_PerPrimitiveCallbacks member
  inline bool UnregisterPerPrimitiveCallback(const char *szFunctionName) {return m_PerPrimitiveCallbacks.UnregisterFunction(szFunctionName);}

  /// \brief
  ///   Returns a per-primitive callback function pointer that matches the passed function name.
  ///   Wraps around VCallbackList::GetByName
  inline VISSHADERCALLBACKFUNC GetPerPrimitiveCallbackByName(const char *szFunctionName) {return (VISSHADERCALLBACKFUNC)m_PerPrimitiveCallbacks.GetByName(szFunctionName);}

  /// \brief
  ///   Unregister all registered per-primitive callback functions. Wraps around
  ///   VCallbackList::UnregisterAll on its m_PerPrimitiveCallbacks member
  inline void UnregisterAllPerPrimitiveCallbacks() {m_PerPrimitiveCallbacks.UnregisterAll();}

  /// \brief
  ///   Returns a reference to the per-primitive callback list (VCallbackList class)
  inline VCallbackList& GetPerPrimitiveCallbackList() {return m_PerPrimitiveCallbacks;}

  IVTextureLoader &TextureLoader;
  VTypeManager &TypeManager;

  VCallbackList m_PerPrimitiveCallbacks;

  ///
  /// @}
  ///

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
