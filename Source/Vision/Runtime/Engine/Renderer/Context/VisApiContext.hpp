/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef DEFINE_VISAPICONTEXT
#define DEFINE_VISAPICONTEXT

#include <Vision/Runtime/Engine/Renderer/Context/VisApiRenderContext.hpp>

/// \brief
///   Container class for managing render contexts
/// 
/// This container class contains functionality for managing and sorting render contexts. It serves as a base class for
/// the VisRenderContextManager_cl and the IVRendererNode classes.
///
/// The IVRenderContextContainer class also takes care of priority handling: It sorts the
/// registered VisRenderContext_cl objects inside a single IVRenderContextContainer instance by their priorities, so the VisionApp_cl::Run method can
/// render them in the appropriate order. This is important since render contexts may depend on
/// each other (e.g. the main scene render context may depend on the result of a shadow render context).
class IVRenderContextContainer
{
public:

  ///
  /// @name Construction/destruction
  /// @{
  ///

  VISION_APIFUNC IVRenderContextContainer();

  ///
  /// @}
  ///

  ///
  /// @name Context handling
  /// @{
  ///

  /// \brief
  ///   Returns the context with the specified index
  /// 
  /// Use this method in conjunction with the GetContextCount function in order to iterate over the
  /// registered render contexts.
  /// 
  /// \param iIndex
  ///   Index of the desired render context
  /// 
  /// \return
  ///   VisRenderContext_cl* : Pointer to the render context with the specified index
  inline VisRenderContext_cl* GetContext(int iIndex)  {return m_Contexts.GetAt(iIndex);}

  /// \brief
  ///   Register a render context object
  /// 
  /// Adds a render context object to the internal list of render contexts. 
  /// 
  /// The render context will be sorted into the list of render contexts based on its priority, so
  /// there is no guarantee about the position (index) the context will be added at.
  /// 
  /// \param pContext
  ///   render context object to add
  VISION_APIFUNC void AddContext(VisRenderContext_cl *pContext);

  /// \brief
  ///   Unregisters a render context object from the internal list of render contexts. 
  /// 
  /// As the list of render contexts will be resorted according to their priorities, the render
  /// context indices may change when this function is called.
  /// 
  /// \param pContext
  ///   render context object to remove from the list
  /// 
  /// \return
  ///   bool bRes: true if the render context could be successfully removed, otherwise false.
  VISION_APIFUNC bool RemoveContext(VisRenderContext_cl *pContext);

  /// \brief
  ///   Gets the number of currently registered render contexts.
  /// 
  /// Use this method in conjunction with the GetContext function in order to iterate over the
  /// currently registered render contexts.
  /// 
  /// \return
  ///   int iCount: number of currently registered render contexts.
  VISION_APIFUNC int GetContextCount() const { return m_Contexts.Count(); }

  /// \brief
  ///   Returns whether the passed render context is already registered with the context container.
  VISION_APIFUNC bool IsContextRegistered(VisRenderContext_cl *pContext) const { return m_Contexts.Contains(pContext); }

  /// \brief
  ///   Removes all currently referenced contexts from the list
  VISION_APIFUNC void RemoveAllContexts();

  /// \brief
  ///  Starts visibility computation for all contained contexts.
  VISION_APIFUNC void PerformVisibilityTests();

  /// \brief
  ///  Renders all contexts in the range fMinPriority (inclusive) to fMaxPriority (exclusive).
  VISION_APIFUNC void RenderContexts(float fMinPriority, float fMaxPriority);

  /// \brief
  ///   Sorts the contexts by priority. This is performed automatically in RenderContexts.
  VISION_APIFUNC void SortByPriorities();

  /// \brief
  ///   Iterates over all contexts and calls \c Reset on their VisDebugRenderQueue_cl objects.
  VISION_APIFUNC void ResetDebugRenderQueue();

  VisRenderContextCollection_cl m_Contexts;       ///< The internal list of contexts.

  ///
  /// @}
  ///
};


/// \brief
///   Render Context handler for global render contexts
/// 
/// Helper class for managing the global VisRenderContext_cl objects. VisRenderContext_cl can either be registered
/// globally (by adding them to this class) or inside one or more IVRendererNode instances. The default implementation
/// of the VisionApp_cl::Run will take care of performing visibility determination and rendering both for the
/// globally registered render contexts and the render contexts inside renderer nodes. You may however implement this
/// yourself to optimize it for your use-case.
/// Please refer to the source code of the VisionApp_cl class for further reference.
/// 
/// Note that the VisRenderContextManager_cl is provided for reasons of backwards compatibility. Applications using
/// renderer nodes typically do not register any render contexts with the global VisRenderContextManager_cl.
class VisRenderContextManager_cl : public IVRenderContextContainer
{
public:

  /// \brief
  ///   Constructor of the VisRenderContextManager_cl. Internal function; do not use.
  VISION_APIFUNC VisRenderContextManager_cl();

  ///
  /// @name Access Global Instances
  /// @{
  ///

  /// \brief
  ///   Returns a pointer to the currently active render context.
  /// 
  /// This method internally calls the VisRenderContext_cl::GetCurrentContext() method. Usage of this function is
  /// only valid inside the OnDoRenderLoop() method of render loops. Note that this function will return the currently
  /// active context, regardless of whether it is assigned to a renderer node or to the VisRenderContextManager_cl.
  /// 
  /// \return
  ///   VisRenderContext_cl *: Pointer to the currently active render context.
  VISION_APIFUNC VisRenderContext_cl *GetCurrentContext();

  /// \brief
  ///   Returns the main render context
  /// 
  /// Vision always has a main render context that is automatically created when the engine is
  /// initialized. The main render context should never be deleted by the application; this is
  /// always done by the engine. The default render target of the main render context is the back
  /// buffer that has been created using the methods provided by the VisVideo_cl class.
  /// 
  /// This method internally calls the VisRenderContext_cl::GetMainRenderContext method.
  /// 
  /// \return
  ///   VisRenderContext_cl *: Pointer to the main render context
  VISION_APIFUNC VisRenderContext_cl *GetMainRenderContext();

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
