/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTransitionManager.hpp

#ifndef VTRANSITIONMANAGER_HPP_INCLUDED
#define VTRANSITIONMANAGER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/AnimationModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/VTransitionStateMachine.hpp>

///
/// \brief
///   The VTransitionManager class manages the transition state machine resources
///
class VTransitionManager : public IVisCallbackHandler_cl, public VisResourceManager_cl
{
public:

  ///
  /// @name CONSTRUCTOR / DESTRUCTOR
  /// @{

  ///
  /// \brief
  ///   Constructor
  ///
  ANIMATION_IMPEXP VTransitionManager(const char *szName="Transition");

  ///
  /// @}
  ///


  ///
  /// @name INITIALISATION / DEINITIALISATION
  /// @{
  ///

  ///
  /// \brief
  ///   Called once by plugin init
  /// 
  /// \see VTransitionManager::OneTimeDeInit
  /// 
  ANIMATION_IMPEXP void OneTimeInit();

  ///
  /// \brief
  ///   Called once by plugin de-init
  ///
  /// \see VTransitionManager::OneTimeInit
  /// 
  ANIMATION_IMPEXP void OneTimeDeInit();


  /// \brief
  ///   DEPRECATED: Only contained for backwards compatibility if desired. The default behavior is to have TSM instance specific events
  ///   Returns the global status of animation event association
  /// 
  /// \return
  ///   bool bStatus: If true, animation events from transition tables are associated with the respective animation sequences directly.
  ///                 If false, animation events from a transition table are only associated per instance with the respective transition state machine.
  ANIMATION_IMPEXP bool GetUseGlobalAnimationEvents();

  /// \brief
  ///   DEPRECATED: Only contained for backwards compatibility if desired. The default behavior is to have TSM instance specific events
  ///    
  ///   Sets the global status if animation events from transition tables are globally associated with the respective animation sequences
  ///   or if they are associated with the respective transition state machine on a per instance basis. 
  /// 
  /// \param bGlobalEvents
  ///   The new status.
  ANIMATION_IMPEXP void SetUseGlobalAnimationEvents(bool bGlobalEvents);

  ///
  /// \brief
  ///   Loads the transition data for the specified model
  ///
  ///   This function loads a vTransition file that contains all the relevant transition data
  ///   that can be generated with the vAnimTool. If there is no transition file present you 
  ///   can use the "CreateDefaultTransitionTable" function that creates a default transition table.
  ///
  /// \param pTargetMesh
  ///   Model the transition data gets loaded for 
  /// 
  /// \param szFilename
  ///   Filename of the transition file
  /// 
  /// \returns
  ///   Pointer to the loaded transition table 
  ///
  /// \note
  ///   A vTransition file must be present. This file can be generated with the vAnimTool.
  /// 
  /// \see
  ///   CreateDefaultTransitionTable
  ///
  ANIMATION_IMPEXP VTransitionTable *LoadTransitionTable(VDynamicMesh *pTargetMesh, const char *szFilename);

  /// 
  /// \brief
  ///   Creates a default transition table with a single default transition definition
  ///
  ///   This function is useful for prototyping where no vTransition file is needed for 
  ///   generating the transition data for an animated model. A default transition table 
  ///   gets created based on all skeletal animation sequences that are assigned to the 
  ///   model. Only one single transition definition is used for all animation blendings.
  ///   A crossfade transition with a duration 0.5 seconds. If you want to specify this 
  ///   default transition you can use the equivalent function that lets you specify this
  ///   transition definition.
  /// 
  /// \param pTargetMesh
  ///   Dynamic Mesh the transition data gets created for
  /// 
  /// \returns
  ///   Pointer to the newly created transition table 
  /// 
  /// \note
  ///   Models must have skeletal animation sequences 
  /// 
  ANIMATION_IMPEXP VTransitionTable *CreateDefaultTransitionTable(VDynamicMesh *pTargetMesh);

  /// 
  /// \brief
  ///   Creates a default transition table with a single default transition definition
  ///
  ///   This function is useful for prototyping where no vTransition file is needed for 
  ///   generating the transition data for an animatated model. A default transition table 
  ///   gets created based on all skeltal animation sequences that are assigned to the 
  ///   model. Only one single transition definition is used for all animation blendings.
  ///   This transition can be specified and passed as a paramter.
  /// 
  /// \param pTargetMesh
  ///   Model the transition data gets created for
  /// 
  /// \param transitionDef 
  ///   Default transition definition that is used for all animation blendings
  /// 
  /// \returns
  ///   Pointer to the newly created transition table 
  /// 
  /// \note
  ///   Models must have skeletal animation sequences 
  /// 
  ANIMATION_IMPEXP VTransitionTable *CreateDefaultTransitionTable(VDynamicMesh *pTargetMesh, VTransitionDef transitionDef);


  /// ANIMATION_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);


  ///
  /// @}
  ///


  ///
  /// @name ACCESS MEMBERS
  /// @{
  ///

  ///
  /// \brief
  ///   Returns a reference to the transition state machine collection
  /// 
  /// \returns
  ///  Reference to the transition state machine collection
  ///
  VTransitionStateMachineCollection &Instances() 
  {
    return m_Instances;
  }

  ///
  /// \brief
  ///   Static function to access the global manager
  /// 
  /// \returns
  /// Reference to the the global transition manager
  ///
  ANIMATION_IMPEXP static VTransitionManager& GlobalManager();

  ///
  /// @}
  ///


  ///
  /// @name OVERRIDE
  /// @{
  ///

  ///
  /// \brief
  ///   Overridden function to listen to some events
  /// 
  ANIMATION_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///

private:
  static VTransitionManager g_GlobalManager;

  bool m_bGlobalAnimationEvents; /// < If global, animation events are associated with animation sequences. If not, they are instance based per TSM

  VTransitionStateMachineCollection m_Instances;

};

#endif // VTRANSITIONMANAGER_HPP_INCLUDED

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
