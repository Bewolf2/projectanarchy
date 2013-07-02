/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTransitionBase.hpp

#ifndef VTRANSITIONBASE_HPP_INCLUDED
#define VTRANSITIONBASE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Animation/Transition/AnimationModule.hpp>

// Forward declarations
class VTransitionSet;
class VTransitionTable;
class VTransitionManager;

// Smart pointer typedef
typedef VSmartPtr<VTransitionTable> VTransitionTablePtr;

// These feature are only supported on PC
#ifdef WIN32
  #define TRANSITION_SUPPORTS_SAVING
#endif

// Specific Transition Definitions
#define DEFAULT_TRANSITION_ID     0

// File extension for binary files
#define TRANSITION_FILEEXTENSION  "vTransition"

// Versions
#define TRANSITION_TABLE_VERSION_0          0     // Initial version
#define TRANSITION_TABLE_VERSION_1          1     // Added support for "animation files", "animation events", "immediate blending", "follow up sequences"
#define TRANSITION_TABLE_VERSION_2          2     // Anim files are now loaded relative to the transition file location instead the model file location 
#define TRANSITION_TABLE_VERSION_3          3     // Introduces Intermediate Transitions for blending between two animations using an additional one
#define TRANSITION_TABLE_VERSION_4          4     // New version for 8.1.15
#define TRANSITION_TABLE_VERSION_CURRENT    4     // Current version


///
/// \brief
///   Enumeration of available animation end types.
///
/// This type defines the behavior of an animation sequence when it finishes.
///
/// \li ANIMATIONEND_TYPE_LOOP: Current sequence loops.
/// \li ANIMATIONEND_TYPE_IDLE: Current sequence blends to one of the idle animation sequences defined.
/// \li ANIMATIONEND_TYPE_NEXT: Current sequence blends to one of its follow-up animation sequences.
///
enum VAnimationEndType_e
{
  ANIMATIONEND_TYPE_LOOP = 0,
  ANIMATIONEND_TYPE_IDLE = 1,
  ANIMATIONEND_TYPE_NEXT = 2,
};


///
/// \brief
///   Enumeration of available transition types.
///
/// This type defines the blending type from one animation sequence to another.
///
enum VTransitionType_e
{
  TRANSITION_TYPE_NONE = 0,
  TRANSITION_TYPE_IMMEDIATE = 1,
  TRANSITION_TYPE_CROSSFADE = 2,
  TRANSITION_TYPE_IMMEDIATE_SYNC = 3,
  TRANSITION_TYPE_CROSSFADE_SYNC = 4,
  TRANSITION_TYPE_INTERMEDIATE = 5
};


///
/// \brief
///   Enumeration of available follow-up trigger types for the Intermediate Transition.
///
///   This enum describes three different types that can be used as follow-up triggers 
///   for Intermediate Transitions. They are described below:
///   \li \c TRANSITION_TRIGGER_IMMEDIATE \n
///     This follow-up trigger starts the final target sequence of the intermediate 
///     transition right after the source sequence has finished its transition. This means 
///     that the blending sequence and the final target sequence overlap for most of their 
///     parts(actually by an offset of the transition's (source->blend sequence) blend 
///     duration.
///   \li \c TRANSITION_TRIGGER_BEFOREENDOFANIM \n
///     This follow-up trigger starts the final target sequence of the intermediate 
///     transition just before the end of the transition (source ->blend sequence) where
///     the starting point of the follow-up sequence is at transition's(source->blend) lenght - 
///     transition's (blend->target) blend duration.
///   \li \c TRANSITION_TRIGGER_ENDOFANIM \n
///     This follow-up trigger starts the final target sequence of the intermediate 
///     transition when the previous transition (source->blend) has completely finished.
///
///
///
enum VTransitionTrigger_e
{
  TRANSITION_TRIGGER_IMMEDIATE = 0,
  TRANSITION_TRIGGER_BEFOREENDOFANIM = 1,
  TRANSITION_TRIGGER_ENDOFANIM = 2
};



///
/// \brief
///   The sequence set represents a single animation file proxy. 
///
/// The transition table holds a list of referenced sequence sets where all 
/// animation data is loaded from. The respective animation files are not
/// loaded at startup immediately. You can specify whether the respective 
/// animation files should be loaded at startup, manually by user or
/// automatically when needed.
///
class VSequenceSet
{
public:

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIALX( VSequenceSet, ANIMATION_IMPEXP )
  ANIMATION_IMPEXP void SerializeX( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  ///
  /// \brief
  ///   Default constructor that initializes the members variables.
  ///
  /// \note
  ///   Use the Init() function to initialize the animation set of a single
  ///   source animation sequence.
  ///
  ANIMATION_IMPEXP VSequenceSet();


  ///
  /// \brief
  ///   Destructor where the transition data of the source sequence gets deleted.
  ///
  ANIMATION_IMPEXP ~VSequenceSet();


  ///
  /// \brief
  ///   Initializes the members variables of this sequence set.
  ///
  /// \param szFilename
  ///   Filename of the animation file of this sequence set.
  ///
  /// \param pTable
  ///   Transition table this sequence set belongs to
  ///
  /// \param bLoaded
  ///   Specifies whether the respective sequence set should be loaded at initialization.
  ///
  inline void Init(const char* szFilename, VTransitionTable* pTable, bool bLoaded)
  {
    m_szFilename = vStrDup(szFilename);
    m_pTransitionTable = pTable;
    m_bLoaded = bLoaded;
  }


  ///
  /// @}
  ///


  ///
  /// @name Load Animation File
  /// @{
  ///


  /// 
  /// \brief
  ///   Loads the respective animation file for the given dynamic mesh.
  ///
  /// \param pMesh
  ///   Pointer to dynamic mesh the respective animation file is loaded for.
  ///
  /// \param szAnimFilePath
  ///   Filename of the animation file that should be loaded.
  ///
  ANIMATION_IMPEXP void Load(VDynamicMesh* pMesh, const char* szAnimFilePath);


  /// 
  /// \brief
  ///   Unloads the respective animation file for the given dynamic mesh.
  ///
  /// \param pMesh
  ///   Pointer to dynamic mesh the respective animation file is unloaded from.
  ///
  inline void Unload(VDynamicMesh* pMesh)
  {
    if (!m_szFilename || m_bLoaded == true)
      return;

    pMesh->GetSequenceSetCollection()->SafeRemove(m_pOwnerSet);
    m_bLoaded = false;
  }


  /// 
  /// \brief
  ///    Returns the state whether the respective animation file is loaded or not.
  ///
  /// \return
  ///   TRUE if the respective animation file is loaded, FALSE otherwise.
  ///
  inline bool IsLoaded() const
  {
    return m_bLoaded;
  }


  ///
  /// @}
  ///

  ///
  /// @name Sequence Set
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the filename of this sequence set.
  ///
  /// \return
  ///   Filename of this sequence set.
  ///
  inline const char* GetFilename()
  {
    return m_szFilename;
  }


  ///
  /// \brief
  ///   Gets the owner sequence set.
  ///
  /// \return
  ///   Pointer to owner sequence set.
  ///
  inline VisAnimSequenceSet_cl* GetOwnerSet() const
  {
    return m_pOwnerSet;
  }


  ///
  /// \brief
  ///   Sets the owner sequence set.
  ///
  /// \param pOwnerSet
  ///   Pointer to owner sequence set.
  ///
  inline void SetOwnerSet(VisAnimSequenceSet_cl* pOwnerSet)
  {
    m_pOwnerSet = pOwnerSet;
  }


  ///
  /// @}
  ///


private:

  friend class VTransitionTable;

  VTransitionTable* m_pTransitionTable;       ///< Pointer to parent transition table
  VisAnimSequenceSet_cl* m_pOwnerSet;         ///< Pointer to owner sequence set

  const char* m_szFilename;                   ///< Filename of the respective animation sequence set
  bool m_bLoaded;                             ///< Whether the respective animation sequence set is loaded at runtime
 
};



///
/// \brief
///   A sequence definition represents a single animation sequence with some additional
///   data generated with the Animation Tool. 
///
/// This class holds additional data for a single animation sequence, specifically 
/// extra animation events added with the Animation Tool and also the behavior type 
/// when the animation ends.
///
class VSequenceDef
{
public:

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIALX( VSequenceDef, ANIMATION_IMPEXP )
  ANIMATION_IMPEXP void SerializeX( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  ///
  /// \brief
  ///   Basic constructor that initializes the members variables.
  ///
  /// \note
  ///   Use the Init() function to initialize the sequence definition of a single
  ///   animation sequence.
  ///
  ANIMATION_IMPEXP VSequenceDef();


  ///
  /// \brief
  ///   Destructor where the additional data of the sequence definition gets deleted.
  ///
  ANIMATION_IMPEXP ~VSequenceDef();


  ///
  /// @}
  ///


  ///
  /// @name Initialization
  /// @{
  ///
  

  /// 
  /// \brief
  ///    Initializes the sequence definition for a single animation
  /// 
  /// This function is used by the transition table when adding a new animation 
  /// definition for a single animation to the table.
  ///
  /// \param pSequence
  ///   The owner animation of this animation definition.
  ///
  /// \param pTable
  ///   The owner transition table of this animation definition.
  ///
  /// \param pSet
  ///   The sequence set the reference sequence belongs to
  /// 
  inline void Init(VisAnimSequence_cl* pSequence, VTransitionTable* pTable, VSequenceSet* pSet)
  {
    VASSERT(pSequence);
    VASSERT(pTable);
    m_pSequence = pSequence;
    m_pSequenceSet = pSet;
    m_pTransitionTable = pTable;
  }


  ///
  /// @}
  ///


  ///
  /// @name Owner Sequence 
  /// @{
  ///

  
  ///
  /// \brief
  ///   Gets the owner animation sequence of this animation definition.
  /// 
  /// \returns
  ///   Pointer to owner animation sequence.
  ///
  inline VisAnimSequence_cl* GetOwnerSequence() const 
  {
    return m_pSequence;
  }


  ///
  /// \brief
  ///   Gets the owner animation sequence set of this animation definition.
  /// 
  /// \returns
  ///   Pointer to owner animation sequence set.
  ///
  inline VSequenceSet* GetOwnerSequenceSet() const 
  {
    return m_pSequenceSet;
  }


  ///
  /// @}
  ///


  ///
  /// @name Animation Events
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the animation event at the given index in the list.
  /// 
  /// \param iIndex
  ///   Index of the animation event in the list.
  ///
  /// \returns
  ///   Pointer to the animation event (or NULL, if animation event could not be found).
  ///
  /// \see 
  ///   VSequenceDef::GetNumAnimationEvents
  ///
  inline VisAnimEvent_cl* GetAnimationEventByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iAnimationEventCount && iIndex >= 0);
    return &m_pAnimationEvent[iIndex];
  }

 
  ///
  /// \brief
  ///   Gets the number of animation events associated with this animation sequence.
  /// 
  /// \note
  ///   This function wraps around VisAnimEventList_cl::GetEventCount.
  ///
  /// \returns
  ///   Number of animation events associated with this animation sequence.
  ///
  /// \see
  ///   VSequenceDef::GetAnimationEventByIndex
  ///
  inline int GetNumAnimationEvents() const 
  {
    return m_iAnimationEventCount;
  }

    
  ///
  /// \brief
  ///   Assigns the stored animation events to the owner animation sequence.
  ///
  inline void InitAnimEvents() 
  {
    if (!m_pSequence)
      return;

    // Add animation events to owner sequence
    if (m_pAnimationEvent != NULL && m_iAnimationEventCount > 0)
    {
      for (int i = 0; i < m_iAnimationEventCount; i++)
        m_pSequence->GetEventList()->AddEvent(m_pAnimationEvent[i].fTimeValue, m_pAnimationEvent[i].GetEventString());
    }
  }


  ///
  /// @}
  ///


  ///
  /// @name Animation Behavior 
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the end of animation behavior type of this animation sequence.
  /// 
  /// \returns
  ///   End of animation behavior type of this animation sequence.
  ///
  /// \see 
  ///   VSequenceDef::SetAnimationEndType
  ///   VSequenceDef::GetFollowUpAnimationByIndex
  ///   VSequenceDef::GetFollowUpAnimationRandom
  ///   VSequenceDef::GetNumFollowUpAnimations
  ///
  inline VAnimationEndType_e GetAnimationEndType() const
  {
    return m_eAnimationEndType;
  }


  ///
  /// \brief
  ///   Sets the end of animation behaviour type of this animation sequence.
  /// 
  /// \param type
  ///   End of animation behaviour type.
  ///
  /// \see 
  ///   VSequenceDef::GetAnimationEndType
  ///   VSequenceDef::GetFollowUpAnimationByIndex
  ///   VSequenceDef::GetFollowUpAnimationRandom
  ///   VSequenceDef::GetNumFollowUpAnimations
  ///
  inline void SetAnimationEndType(VAnimationEndType_e type)
  {
    m_eAnimationEndType = type;
  }


  ///
  /// \brief
  ///   Gets the follow-up animation for the specified index.
  /// 
  /// \note
  ///   Index has to be within the follow-up animation array bounds, meaning number of available 
  ///   follow-up animation sequences.
  ///
  /// \param iIndex
  ///   Index of the searched follow-up animation sequence.
  /// 
  /// \returns
  ///   Pointer to the follow-up animation sequence (or NULL, if the the follow-up animation sequence could not be found).
  ///
  /// \see 
  ///   VSequenceDef::GetAnimationEndType
  ///   VSequenceDef::SetAnimationEndType
  ///   VSequenceDef::GetNumFollowUpAnimations
  ///   VSequenceDef::GetFollowUpAnimationRandom
  ///
  inline VisAnimSequence_cl* GetFollowUpAnimationByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iFollowUpAnimationCount && iIndex >= 0);
    return m_pFollowUpAnimation[iIndex];
  }


  ///
  /// \brief
  ///   Gets a random follow-up animation sequence from the specified animation sequence list.
  /// 
  /// \note
  ///   If no follow-up animations are specified this function returns NULL. If only one follow-up animation
  ///   is specified also this one gets always returned. For multiple follow-up animation sequences a random
  ///   one gets returned.
  ///
  /// \returns
  ///   Pointer to the follow-up animation sequence (or NULL, if no follow-up animation sequences are specified).
  ///
  /// \see 
  ///   VSequenceDef::GetAnimationEndType
  ///   VSequenceDef::SetAnimationEndType
  ///   VSequenceDef::GetNumFollowUpAnimations
  ///   VSequenceDef::GetFollowUpAnimationByIndex
  ///
  inline VisAnimSequence_cl* GetFollowUpAnimationRandom() const
  {
    VASSERT_MSG(m_iFollowUpAnimationCount > 0, "m_iFollowUpAnimationCount is 0, can't select random animation.");
    int iIndex = Vision::Game.GetRand() % m_iFollowUpAnimationCount;
    return m_pFollowUpAnimation[iIndex];
  }


  ///
  /// \brief
  ///   Gets the number of follow-up animation sequences.
  /// 
  /// \returns
  ///   Number of follow-up animation sequences.
  ///
  /// \see
  ///   VSequenceDef::GetFollowUpAnimationByIndex
  ///
  inline int GetNumFollowUpAnimations() const 
  {
    return m_iFollowUpAnimationCount;
  }


  ///
  /// \brief
  ///   Gets value whether this animation sequence needs to be completed before any other state switches.
  /// 
  /// \param bFinishSequence
  ///   If TRUE, this animation sequence needs to be completed before any other state switches.
  ///
  /// \see
  ///   VSequenceDef::GetFinishSequence
  ///
  inline void SetFinishSequence(bool bFinishSequence)
  {
    m_bFinishSequence = bFinishSequence;
  }


  ///
  /// \brief
  ///   Gets value whether this animation sequence needs to be completed before any other state switches.
  /// 
  /// \returns
  ///   BOOL: If TRUE, this animation sequence needs to be completed before any other state switches.
  ///
  /// \see
  ///   VSequenceDef::SetFinishSequence
  ///
  inline bool GetFinishSequence()
  {
    return m_bFinishSequence;
  }


  ///
  /// @}
  ///


  ///
  /// @name Allocating Data
  /// @{
  ///


  ///
  /// \brief
  ///   Allocates array of animation sequences with the specified size.
  /// 
  /// \param iCount
  ///   Number of animation sequences to allocate.
  ///
  /// \returns
  ///   Pointer to array of animation sequences.
  ///
  inline VisAnimSequence_cl** AllocateFollowUpAnimations(int iCount)
  {
    if (m_iFollowUpAnimationCount == iCount)
      return m_pFollowUpAnimation;
    V_SAFE_DELETE_ARRAY(m_pFollowUpAnimation);
    m_iFollowUpAnimationCount = iCount;
    if (iCount > 0)
      m_pFollowUpAnimation = new VisAnimSequence_cl*[iCount];
    return m_pFollowUpAnimation;
  }

  
  ///
  /// \brief
  ///   Allocates array of animation events with the specified size.
  /// 
  /// \param iCount
  ///   Number of animation events to allocate.
  ///
  /// \returns
  ///   Pointer to array of animation events.
  ///
  inline VisAnimEvent_cl* AllocateAnimationEvents(int iCount)
  {
    if (m_iAnimationEventCount == iCount)
      return m_pAnimationEvent;
    V_SAFE_DELETE_ARRAY(m_pAnimationEvent);
    m_iAnimationEventCount = iCount;
    if (iCount > 0)
      m_pAnimationEvent = new VisAnimEvent_cl[iCount];
    return m_pAnimationEvent;
  }


  ///
  /// @}
  ///


private:

  friend class VTransitionTable;

  // Static Serialization Helper

  static VSequenceSet* ReadSequenceSetProxy(VArchive &ar, VTransitionTable *pTable);
  static void WriteSequenceSetProxy(VArchive &ar, VSequenceSet* pSequenceSet);
  static VisAnimSequence_cl* ReadSequenceProxy(VArchive &ar, VSequenceSet* pSequenceSet);
  static void WriteSequenceProxy(VArchive &ar, VisAnimSequence_cl* pSequence);

  // Member

  VTransitionTable* m_pTransitionTable;               ///< Pointer to parent transition table 
  VSequenceSet* m_pSequenceSet;                       ///< Pointer to owner sequence set
  VisAnimSequence_cl* m_pSequence;                    ///< Pointer to owner animation sequence

  VisAnimEvent_cl* m_pAnimationEvent;                 ///< Flat List of animation events (can be NULL)
  int m_iAnimationEventCount;                         ///< Number of animation events (can be 0)
  
  VisAnimSequence_cl** m_pFollowUpAnimation;          ///< Flat list of follow-up animation sequences (can be NULL)
  int m_iFollowUpAnimationCount;                      ///< Number of follow-up animation sequences (can be 0)
  
  VAnimationEndType_e m_eAnimationEndType;            ///< End of animation behavior type
  bool m_bFinishSequence;                             ///< If TRUE this sequence needs to be completed before any other state switches
  
};



///
/// \brief
///   Data structures for a single transition
/// 
/// The transition definition represents the actual data for a single transition. 
/// It keeps the information about the transition type, the duration and the 
/// starting position of the transition and an unique transition ID for referencing
/// this transition definition by multiple source - target animation pairs.
///
/// \remarks
///   The blending position is only used in the Animation Tool to define a specific 
///   starting position for previewing the animation blending.
///
struct VTransitionDef
{
  int m_iID;                                ///< Unique ID (allows sharing transitions, e.g. for default transition)
  VTransitionType_e m_eType;                ///< Type of the transition
  float m_fBlendDuration;                   ///< Duration of the transition
  float m_fBlendPosition;                   ///< Starting time (just important for editing)
  bool m_bFinishBlending;                   ///< Whether this transition allows immediate state switches
  char m_cPadding[3];                       ///< Ensure that the structure is aligned to a multiple of sizeof(int)
  VTransitionTrigger_e m_eFollowupTrigger;  ///< Trigger when the Followup-Transition of the Intermediate animation is started to blend to the target animation
  VisAnimSequence_cl* m_BlendSequence;      ///< The animation sequence used for blending

  ///
  /// \brief
  ///   Defines how data is aligned for saving/loading - do not change data layout.
  ///
  inline static const char *GetDataLayout(int iLocalVersion) 
  {
    switch (iLocalVersion)
    {
      case TRANSITION_TABLE_VERSION_0: return "iiff";
      case TRANSITION_TABLE_VERSION_1:
      case TRANSITION_TABLE_VERSION_2: return "iiffcccc";
      case TRANSITION_TABLE_VERSION_3:
      default: return "iiffcccci";
    }
  }

  inline static const int GetSize(int iLocalVersion)
  {
    switch (iLocalVersion)
    {
      case TRANSITION_TABLE_VERSION_0: return offsetof(VTransitionDef, m_bFinishBlending);
      case TRANSITION_TABLE_VERSION_1: 
      case TRANSITION_TABLE_VERSION_2: return offsetof(VTransitionDef, m_eFollowupTrigger);
      case TRANSITION_TABLE_VERSION_3:  
      default: return offsetof(VTransitionDef, m_BlendSequence);
    }
  }

  inline bool IsBrokenVersion3() const
  {
    if (m_iID<-1 || m_iID>0xffff) return true;
    if ((int)m_eType<0 || (int)m_eType>5) return true;
    if (m_fBlendDuration<0.f || !hkvMath::isValidFloat (m_fBlendDuration)) return true;
    if (m_fBlendPosition<0.f || !hkvMath::isValidFloat (m_fBlendPosition)) return true;
    return false;
  }
};

// old loading versions:
V_COMPILE_ASSERT(offsetof(VTransitionDef, m_bFinishBlending)==16);  // TRANSITION_TABLE_VERSION_0
V_COMPILE_ASSERT(offsetof(VTransitionDef, m_eFollowupTrigger)==20); // TRANSITION_TABLE_VERSION_1/2
V_COMPILE_ASSERT(offsetof(VTransitionDef, m_BlendSequence)==24);    // TRANSITION_TABLE_VERSION_3


/// \brief
///   Represents the transition data for one source animation
/// 
/// The transition set represents the transition data for one single source 
/// animation. It holds reference to its parent transition table and all 
/// target animations with the corresponding transition definitions which are
/// represented as an array of transition pairs.
/// 
/// \see 
///   VTransitionBase
///   VTransitionTable
///   VTransitionStateMachine
///
class VTransitionSet
{  
public:

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIALX( VTransitionSet, ANIMATION_IMPEXP )
  ANIMATION_IMPEXP void SerializeX( VArchive &ar );

  ///
  /// @}
  ///
  

  /// 
  /// \brief
  ///   Represents a single pair of a target animation and its corresponding 
  ///   transition definition.
  ///
  struct VTransitionPair_t
  {
    VTransitionPair_t() {pTargetSequence=NULL;pTransition=NULL;}
    VisAnimSequence_cl *pTargetSequence;
    VTransitionDef *pTransition;
  };


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  ///
  /// \brief
  ///   Basic constructor that initializes the members variables.
  ///
  /// \note
  ///   Use the Init() function to initialize the transition set of a single
  ///   source animation sequence.
  ///
  ANIMATION_IMPEXP VTransitionSet();


  ///
  /// \brief
  ///   Destructor where the transition data of the source sequence gets deleted.
  ///
  ANIMATION_IMPEXP ~VTransitionSet();


  ///
  /// @}
  ///
  
   
  ///
  /// @name Initialization / Deinitialization
  /// @{
  ///
  

  /// 
  /// \brief
  ///    Initializes the transition set for a single source animation
  /// 
  /// This function is used by the transition table when adding a new transition 
  /// set for a single source animation to the table.
  ///
  /// \param pSourceSequence
  ///   The source animation of this transition set.
  /// 
  /// \param pParentTable
  ///   The parent transition table this transition set belongs to.
  ///
  /// \param pSequenceSet
  ///   The sequence set the source sequence belongs to.
  ///
  inline void Init(VisAnimSequence_cl *pSourceSequence, VTransitionTable *pParentTable, VSequenceSet* pSequenceSet)
  {
    VASSERT(pSourceSequence);
    VASSERT(pParentTable);
    m_pTransitionSource = pSourceSequence;
    m_pTransitionTable = pParentTable;
    m_pSequenceSet = pSequenceSet;
  }


  ///
  /// @}
  ///


  ///
  /// @name Access to Members
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the source animation of this transition set.
  /// 
  /// \returns
  ///   Pointer to the source animation sequence.
  ///
  inline VisAnimSequence_cl *GetSourceSequence() const 
  {
    return m_pTransitionSource;
  }

   
  ///
  /// \brief
  ///   Gets the owner animation sequence set of this transition set.
  /// 
  /// \returns
  ///   Pointer to owner animation sequence set.
  ///
  inline VSequenceSet* GetOwnerSequenceSet() const 
  {
    return m_pSequenceSet;
  }


  ///
  /// \brief
  ///   Gets the target sequence at the specified index.
  /// 
  /// \param iIndex
  ///   Index of the target animation in the transition pair array.
  /// 
  /// \returns
  ///   Pointer to the target animation sequence at the specified index.
  ///
  /// \remarks 
  ///   Index has to be within the array bounds of the target animations.
  ///
  inline VisAnimSequence_cl *GetTargetSequenceByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iTargetCount && iIndex >= 0);
    return m_pTransitionPairs[iIndex].pTargetSequence;
  }


  /// \brief
  ///   Gets the transition definition for a specific target animation.
  ///
  /// \param pTargetSequence
  ///   Target animation whose corresponding transition definition is searched for.
  /// 
  /// \returns
  ///   Pointer to the transition definition (or NULL, if transition definition could not be found).
  ///
  ANIMATION_IMPEXP VTransitionDef *GetTransitionDef(VisAnimSequence_cl *pTargetSequence) const;


  ///
  /// \brief
  ///   Gets the number of targets that belong to the transition set.
  /// 
  /// \returns
  ///   Number of allocated target animations in the transition set.  
  ///
  inline int GetNumTargets() const
  {
    return m_iTargetCount;
  }


  ///
  /// @}
  ///


  ///
  /// @name Allocating Data
  /// @{
  ///


  ///
  /// \brief
  ///   Adds a target animation and the corresponding transition definition to the 
  ///   transition pair array.
  /// 
  /// The transition set holds the target animations of a single source animation 
  /// and its corresponding transition definitions in an array of transition pairs 
  /// (pointer to pointer map). This function is used by the transition table to 
  /// when setting up the transition data for the transition state machine.
  ///
  /// \note
  ///   Call VTransitionSet::AllocateTargets before adding new transition pairs.
  ///
  /// \param pTargetSequence
  ///   Target animation of the transition pair.
  /// 
  /// \param pTransition
  ///   Transition definition of the transition pair.
  /// 
  /// \param iIndex
  ///   Index at which the transition pair will be added in the transition map.
  /// 
  /// \returns
  ///   TRUE if adding of the transition pair was successful, FALSE otherwise.
  /// 
  /// \see
  ///   VTransitionSet::AllocateTargets.
  /// 
  ANIMATION_IMPEXP bool AddTransitionPair(VisAnimSequence_cl *pTargetSequence, VTransitionDef *pTransition, int iIndex);


  ///
  /// \brief
  ///   Allocates array of transition pairs with the specified number.
  /// 
  /// \param iCount
  ///   Number of transition pairs to allocate.
  /// 
  /// \see
  ///   VTransitionSet::AddTransitionPair
  ///
  inline void AllocateTargets(int iCount)
  {
    if (m_iTargetCount==iCount)
      return;
    V_SAFE_DELETE_ARRAY(m_pTransitionPairs);
    m_iTargetCount = iCount;
    if (iCount>0)
      m_pTransitionPairs = new VTransitionPair_t[iCount];
  }


  ///
  /// @}
  ///

  ANIMATION_IMPEXP VTransitionPair_t GetTransitionPairByIndex(int index){return m_pTransitionPairs[index];}

  ANIMATION_IMPEXP void SetTransitionTable(VTransitionTable* table){m_pTransitionTable = table;}
  ANIMATION_IMPEXP void SetTransitionSource(VisAnimSequence_cl* seqSrc){m_pTransitionSource = seqSrc;}
  ANIMATION_IMPEXP void SetSequenceSet(VSequenceSet* seqSet){m_pSequenceSet = seqSet;}


private:

  friend class VTransitionTable;

  static VSequenceSet* ReadSequenceSetProxy(VArchive &ar, VTransitionTable *pTable);
  static void WriteSequenceSetProxy(VArchive &ar, VSequenceSet* pSequenceSet);
  static VisAnimSequence_cl* ReadSequenceProxy(VArchive &ar, VSequenceSet* pSequenceSet, VDynamicMesh *pMesh);
  static void WriteSequenceProxy(VArchive &ar, VisAnimSequence_cl* pSequence);

  VTransitionTable* m_pTransitionTable;       ///< Parent transition table 
  VSequenceSet* m_pSequenceSet;               ///< Owner sequence set 
  VisAnimSequence_cl* m_pTransitionSource;    ///< Owner Source sequence  
  VTransitionPair_t* m_pTransitionPairs;      ///< Array of target sequences pointers
  int m_iTargetCount;                         ///< Number of target sequences
};


///
/// \brief
///   Represents one transition table that holds all transition data of an animation file
///
/// The transition table represents the data structure of all transitions that are
/// specified for a mesh in one animation file. This data is typically loaded from 
/// a transition file that can be generated with the animation tool. This class 
/// provides functionality for loading and saving the transition data into binary format. 
///
/// The transition table is used by the transition state machine to look up the
/// specified transition of a given source and target animation. Therefore it 
/// provides the corresponding access functions.
///
/// \see 
///   VTransitionBase
///   VTransitionTable
///   VTransitionStateMachine
///
class VTransitionTable : public VManagedResource
{
public:

  ///
  /// @name Serialization
  /// @{
  ///

  V_DECLARE_SERIALX( VTransitionTable, ANIMATION_IMPEXP )
  ANIMATION_IMPEXP void SerializeX( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name Constructor / Destructor
  /// @{
  ///


  ///
  /// \brief
  ///   Constructor - Takes the associated dynamic mesh as parameter and initializes the 
  ///   member variables.
  ///
  /// \param pParentManager
  ///   Pointer to parent transition manager of this transition table.
  /// 
  /// \param pMesh
  ///   Pointer to associated dynamic mesh of this transition table.
  /// 
  /// \note
  ///   Use the Loading functions to load and initialize the transition data of a dynamic mesh.
  ///
  /// \see
  ///   VTransitionTable::LoadFromFile
  ///
  ANIMATION_IMPEXP VTransitionTable(VTransitionManager *pParentManager, VDynamicMesh *pMesh);


  ///
  /// \brief
  ///   Destructor - Deletes the transitions sets and definitions stored in the table.
  ///
  ANIMATION_IMPEXP ~VTransitionTable();


  ///
  /// @}
  ///


  ///
  /// @name Saving / Loading
  /// @{
  ///


  ///
  /// \brief
  ///   Loads the transition table from a binary file and generates the data structures.
  /// 
  /// \param filePath
  ///   The absolute filepath to the binary transition file.
  /// 
  /// \returns
  ///   TRUE if loading was successful, FALSE otherwise.
  /// 
  /// \see
  ///   VTransitionTable::LoadFromFile
  ///
  ANIMATION_IMPEXP bool LoadFromFile(const char* filePath); 

#ifdef TRANSITION_SUPPORTS_SAVING

  ///
  /// \brief
  ///   Save the transition table to a binary file.
  /// 
  /// \param filePath
  ///   The absolute filepath to the transition file.
  /// 
  /// \returns
  ///   TRUE if loading was successful, FALSE otherwise.
  /// 
  /// \see
  ///   VTransitionTable::SaveToFile
  ///
  ANIMATION_IMPEXP bool SaveToFile(const char* filePath); 
  
#endif

  ///
  /// @}
  ///
  
  
  ///
  /// @name Access to Animation Data 
  /// @{
  ///

  
  ///
  /// \brief
  ///   Gets the animation set for the specified file name.
  /// 
  /// \param pSequence
  ///   Pointer of the animation sequence the sequence definition is searched for.
  /// 
  /// \returns
  ///   Pointer to the animation set (or NULL, if the the animation set could not be found).
  ///
  /// \see 
  ///   VTransitionTable::GetAnimationSetByIndex
  ///   VTransitionTable::GetNumAnimationSets
  ///
  ANIMATION_IMPEXP VSequenceDef* GetSequenceDef(VisAnimSequence_cl* pSequence) const;


  ///
  /// \brief
  ///   Gets the animation file name for the specified index.
  /// 
  /// \note
  ///   Index has to be within the animation set array bounds, meaning number of available 
  ///   animation sets.
  ///
  /// \param iIndex
  ///   Index of the searched animation set.
  /// 
  /// \returns
  ///   Pointer to the animation set (or NULL, if the the animation set could not be found).
  ///
  /// \see 
  ///   VTransitionTable::GetAnimationSet
  ///   VTransitionTable::GetNumAnimationSets
  ///
  inline VSequenceDef* GetSequenceDefByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iSequenceDefCount && iIndex >= 0);
    return &m_pSequenceDef[iIndex];
  }


  ///
  /// \brief
  ///   Gets the number of animation sets in the transition table.
  /// 
  /// \returns
  ///   Number of animation sets in the transition table.
  ///
  inline int GetNumSequenceDefs() const 
  {
    return m_iSequenceDefCount;
  }


  ///
  /// \brief
  ///   Gets the animation set for the specified file name.
  /// 
  /// \param szFilename
  ///   Name of the animation file the sequence set is searched for.
  /// 
  /// \returns
  ///   Pointer to the animation set (or NULL, if the the animation set could not be found).
  ///
  /// \see 
  ///   VTransitionTable::GetAnimationSetByIndex
  ///   VTransitionTable::GetNumAnimationSets
  ///
  ANIMATION_IMPEXP VSequenceSet* GetSequenceSet(const char* szFilename) const;


  ///
  /// \brief
  ///   Gets the animation sequence set collection associated with this transition table.
  ///
  /// \note
  ///   A sequence set is basically a single animation file (.anim).
  ///
  /// \param iIndex
  ///   Index of the sequence set in the list.
  /// 
  /// \returns
  ///   Pointer to the animation sequence set collection (or NULL, if data has not been set).
  ///
  inline VSequenceSet* GetSequenceSetByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iSequenceSetCount && iIndex >= 0);
    return &m_pSequenceSet[iIndex];
  }


  ///
  /// \brief
  ///   Gets the number of sequence sets in the transition table.
  /// 
  /// \returns
  ///   Number of sequence sets in the transition table.
  ///
  inline int GetNumSequenceSets() const 
  {
    return m_iSequenceSetCount;
  }


  ///
  /// \brief
  ///   Gets the mesh this transition table is created for.
  ///
  /// \returns
  ///   Pointer to the mesh data (or NULL, if mesh has not been set).
  ///
  inline VDynamicMesh* GetMesh() const 
  {
    return m_pMesh;
  }



  /// \brief
  ///   Finds the animation sequence by name and type. Searches in the sequence set collection associated with this transition table
  /// 
  /// This function loops through all sequence sets that are attached to this transition table.
  /// 
  /// To globally find a loaded animation sequence, the dedicated resource manager can be used. See
  /// VisAnimManager_cl::GetSequenceSetManager().
  /// 
  /// \param szSequenceName
  ///   Name of the sequence.
  ///
  /// \param eType
  ///   The animation type (either VIS_MODELANIM_SKELETAL or VIS_MODELANIM_VERTEX).
  /// 
  /// \return
  ///   VisAnimSequence_cl* pSeq: found sequence or NULL.
  ANIMATION_IMPEXP VisAnimSequence_cl* GetSequence(const char *szSequenceName, VisModelAnimType_e eType = VIS_MODELANIM_SKELETAL);


  ///
  /// @}
  ///


  ///
  /// @name Access to Transition Data 
  /// @{
  ///


  ///
  /// \brief
  ///   Gets the transition set for the specified source animation.
  /// 
  /// \param pSourceSequence
  ///   Source animation whose transition set is searched for.
  /// 
  /// \returns
  ///   Pointer to the transition set (or NULL, if the the transition set could not be found).
  ///
  /// \see VTransitionTable::GetTransitionSetByIndex
  ///
  ANIMATION_IMPEXP VTransitionSet* GetTransitionSet(VisAnimSequence_cl *pSourceSequence) const;
  

  ///
  /// \brief
  ///   Gets the transition set for the specified index.
  /// 
  /// \note
  ///   Index has to be within the transition set array bounds, meaning number of available
  ///   source animations.
  ///
  /// \param iIndex
  ///   Index of the searched transition set.
  /// 
  /// \returns
  ///   Pointer to the transition set (or NULL, if the the transition set could not be found).
  ///
  /// \see 
  ///   VTransitionTable::GetTransitionSetByIndex
  ///   VTransitionTable::GetNumTransitionSets
  ///
  inline VTransitionSet* GetTransitionSetByIndex(int iIndex) const
  {
    VASSERT(iIndex < m_iTransitionSetCount && iIndex >= 0);
    return &m_pTransitionSet[iIndex];
  }


  ///
  /// \brief
  ///   Gets the transition definition for the specified ID.
  /// 
  /// \param iID
  ///   Unique transition ID of the searched transition definition.
  /// 
  /// \returns
  ///   Pointer to the transition definition (or NULL, if the transition definition could not be found).
  ///
  /// \remarks
  ///   Multiple source and target animation pairs can share the same transition data. This
  ///   is primarily used for the default transition (see Animation Tool documentation).
  ///
  /// \see 
  ///   VTransitionTable::GetTransitionDefByIndex
  ///
  ANIMATION_IMPEXP VTransitionDef* GetTransitionDef(int iID) const;


  ///
  /// \brief
  ///   Gets the transition definition for the specified source and target animation
  /// 
  /// \note 
  ///   It is not possible to blend from and to the same animation sequence 
  ///   (source animation == target animation). 
  ///
  /// \param pSourceSequence
  ///   Source animation of the searched transition definition.
  /// 
  /// \param pTargetSequence
  ///   Target animation of the searched transition definition.
  /// 
  /// \returns
  ///   Pointer to the transition definition (or NULL, if the transition definition could not be found).
  ///
  ANIMATION_IMPEXP VTransitionDef* GetTransitionDef(VisAnimSequence_cl *pSourceSequence, VisAnimSequence_cl *pTargetSequence) const;


  ///
  /// \brief
  ///   Gets the available transition definition for the specified index.
  /// 
  /// \note
  ///   Index has to be within the transition definition array bounds, meaning number of available 
  ///   transition definitions.
  ///
  /// \param iIndex
  ///   Index of the searched transition definition.
  /// 
  /// \returns
  ///   Pointer to the transition definition (or NULL, if the transition definition could not be found).
  /// 
  /// \see 
  ///   VTransitionTable::GetTransitionDef
  ///   VTransitionTable::GetNumTransitionDefs
  ///
  inline VTransitionDef* GetTransitionDefByIndex(int iIndex) const 
  {
    VASSERT(iIndex>=0 && iIndex<m_iTransitionDefCount);
    return &m_pTransitionDef[iIndex];
  }


  ///
  /// \brief
  ///   Gets the number of source animations in the transition table.
  /// 
  /// \returns
  ///   Number of source animations in the transition table.
  ///
  inline int GetNumTransitionSets() const 
  {
    return m_iTransitionSetCount;
  }


  ///
  /// \brief
  ///   Gets the number of transition definitions in the transition table.
  /// 
  /// \returns
  ///   Number of transition definitions in the transition table.
  ///
  inline int GetNumTransitionDefs() const 
  {
    return m_iTransitionDefCount;
  }


  ///
  /// @}
  ///


  ///
  /// @name Allocating Data 
  /// @{
  ///


  ///
  /// \brief
  ///   Allocates array of animation file names with the specified size.
  /// 
  /// \param iCount
  ///   Number of animation file names to allocate.
  ///
  /// \returns
  ///   Pointer to array of animation file names.
  ///
  inline VSequenceDef* AllocateSequenceDefs(int iCount)
  {
    if (m_iSequenceDefCount == iCount)
      return m_pSequenceDef;
    V_SAFE_DELETE_ARRAY(m_pSequenceDef);
    m_iSequenceDefCount = iCount;
    if (iCount > 0)
      m_pSequenceDef = new VSequenceDef[iCount];
    return m_pSequenceDef;
  }


  ///
  /// \brief
  ///   Allocates array of animation file names with the specified size.
  /// 
  /// \param iCount
  ///   Number of animation file names to allocate.
  ///
  /// \returns
  ///   Pointer to array of animation file names.
  ///
  inline VSequenceSet* AllocateSequenceSets(int iCount)
  {
    if (m_iSequenceSetCount == iCount)
      return m_pSequenceSet;
    V_SAFE_DELETE_ARRAY(m_pSequenceSet);
    m_iSequenceSetCount = iCount;
    if (iCount > 0)
      m_pSequenceSet = new VSequenceSet[iCount];
    return m_pSequenceSet;
  }

  /// \brief
  ///   Indicates whether the passed set is referenced by any sequence in this table
  inline bool ReferencesSequenceSet(VisAnimSequenceSet_cl *pSet)
  {
    for (int i=0;i<m_iSequenceSetCount;i++)
      if (m_pSequenceSet[i].GetOwnerSet()==pSet)
        return true;
    return false;
  }


  ///
  /// \brief
  ///   Allocates array of transition definitions with the specified size.
  /// 
  /// \param iCount
  ///   Number of transition definitions to allocate.
  ///
  /// \returns
  ///   Pointer to array of transition definitions.
  ///
  inline VTransitionDef* AllocateTransitionDefs(int iCount)
  {
    if (m_iTransitionDefCount == iCount)
      return m_pTransitionDef;
    V_SAFE_DELETE_ARRAY(m_pTransitionDef);
    m_iTransitionDefCount = iCount;
    if (iCount > 0)
      m_pTransitionDef = new VTransitionDef[iCount];
    return m_pTransitionDef;
  }


  ///
  /// \brief
  ///   Allocates array of transition sets with the specified size.
  /// 
  /// \param iCount
  ///   Number of transition sets to allocate.
  ///
  /// \returns
  ///   Pointer to array of transition sets.
  ///
  inline VTransitionSet* AllocateTransitionSets(int iCount)
  {
    if (m_iTransitionSetCount == iCount)
      return m_pTransitionSet;
    V_SAFE_DELETE_ARRAY(m_pTransitionSet);
    m_iTransitionSetCount = iCount;
    if (iCount > 0)
      m_pTransitionSet = new VTransitionSet[iCount];
    return m_pTransitionSet;
  }

  ///
  /// @}
  ///

protected:

  friend class VTransitionSet;

  // Resource functions
  ANIMATION_IMPEXP virtual BOOL Reload();
  ANIMATION_IMPEXP virtual BOOL Unload();

  VisAnimSequence_cl* DeserializeBlendSequence(VArchive &ar, char iLocalVersion);

  VDynamicMesh *m_pMesh;                            ///< Owner entity model

  VSequenceSet* m_pSequenceSet;                     ///< Animation sequence sets associated with this transition table
  int m_iSequenceSetCount;                          ///< Number of allocated animation sets

  VSequenceDef* m_pSequenceDef;                     ///< Flat list of all associated sequence definitions
  int m_iSequenceDefCount;                          ///< Number of allocated sequence definitions

  VTransitionSet* m_pTransitionSet;                 ///< Flat list of available source animation sequences
  int m_iTransitionSetCount;                        ///< Number of allocated transition sets
    
  VTransitionDef* m_pTransitionDef;                 ///< Flat list of the available transition definitions
  int m_iTransitionDefCount;                        ///< Number of allocated transition definitions

  bool m_bBrokenVersion3;   // local version 3 had some compatibility issues
};

#endif // VTRANSITIONBASE_HPP_INCLUDED

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
