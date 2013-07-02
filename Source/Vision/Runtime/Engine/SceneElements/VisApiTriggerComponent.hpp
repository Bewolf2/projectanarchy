/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiTriggerComponent.hpp

#ifndef _VISAPITRIGGERCOMPONENT_HPP_INCLUDED
#define _VISAPITRIGGERCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponent.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponentCollection.hpp>

class VisTriggerSourceComponent_cl;
class VisTriggerTargetComponent_cl;
class IVisTriggerBaseComponent_cl;



/// \brief
///   Common base class for source and target trigger components. See VisTriggerSourceComponent_cl
///   and VisTriggerTargetComponent_cl
class IVisTriggerBaseComponent_cl : public IVObjectComponent, public VisElementManager_cl<IVisTriggerBaseComponent_cl *>
{
public:

  /// \brief
  ///   Constructor
  IVisTriggerBaseComponent_cl(const char *szName, int iComponentFlags) :
      IVObjectComponent(szName,iComponentFlags)
  {
    AddToElementManager();
  }

  /// \brief
  ///   Constructor
  IVisTriggerBaseComponent_cl(int iID, int iComponentFlags) :
      IVObjectComponent(iID,iComponentFlags)
  {
    AddToElementManager();
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC virtual ~IVisTriggerBaseComponent_cl();

  /// \brief
  ///   String that defines the text to show for the link type in vForge. Not relevant inside the
  ///   engine (accordingly does not get serialized)
  VString m_sVForgeDisplayName;

  /// \brief
  ///   Optional string that defines the icon filename for the link icon in vForge. Not relevant
  ///   inside the engine (accordingly does not get serialized)
  VString m_sVForgeIconFilename;

  /// \brief
  ///   Global helper function to establish a trigger link between a source and a target.
  VISION_APIFUNC static void OnLink(VisTriggerSourceComponent_cl *pSource, VisTriggerTargetComponent_cl *pTarget);

  /// \brief
  ///   Link the current instance to all trigger targets (if the instance is a trigger source) or sources
  ///   (if the instance is a source) with the specified name.
  ///
  /// \param pszTriggerName
  ///   The name of the trigger to be linked
  /// \param pOptional
  ///   If specified the current instance will only link triggers of this VisTypedEngineObject_cl
  ///
  /// \returns
  ///   The number of linked trigger components or -1 for invalid parameters.
  ///
  VISION_APIFUNC int LinkTo(const char * pszTriggerName, VisTypedEngineObject_cl * pOptional = NULL);

  /// \brief
  ///   Global helper function to break up a trigger link between a source and a target.
  VISION_APIFUNC static void OnUnlink(VisTriggerSourceComponent_cl *pSource, VisTriggerTargetComponent_cl *pTarget);

  /// \brief
  ///   Unlink the current instance from all trigger targets (if the instance is a trigger source) or sources
  ///   (if the instance is a source) with the specified name.
  ///
  /// \param pszTriggerName
  ///   The name of the trigger to be unlinked
  /// \param pOptional
  ///   If specified the current instance will only unlink triggers of this VisTypedEngineObject_cl
  ///
  /// \returns
  ///   The number of unlinked trigger components or -1 for invalid parameters.
  ///
  VISION_APIFUNC int UnlinkFrom(const char * pszTriggerName, VisTypedEngineObject_cl * pOptional = NULL);

private:

  enum VTriggerLink_e
  {
    VTL_LINK,
    VTL_UNLINK
  };

  /// \brief
  ///   Internal helper to link or unlink sources and targets
  int Connect(enum IVisTriggerBaseComponent_cl::VTriggerLink_e link, const char * pszTriggerName, VisTypedEngineObject_cl * pOptional = NULL);
};


VISION_ELEMENTMANAGER_TEMPLATE_DECL(IVisTriggerBaseComponent_cl)


/// \brief
///   Object component that defines a trigger source
/// 
/// An object (e.g. entity) can have one or more trigger sources which are components that can
/// trigger an event on an arbitrary number of targets. For instance a light switch (source) can
/// toggle dynamic lights (targets).
/// 
/// vForge gathers components of this type via reflection and converts them to link sources in
/// vForge, so artists can graphically link trigger sources and targets.
/// All components of this type are added to the global VisTriggerTargetComponent_cl::ElementManager list so
/// all instances in the scene can be accessed through that list.
class VisTriggerSourceComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  /// \brief
  ///   Constructor
  /// 
  /// \param iComponentFlags
  ///   Component bitflags that define the behavior of the component. See constructor of
  ///   IVObjectComponent
  /// 
  /// \param szName
  ///   Sets the component name. For triggers this should be unique along all other trigger
  ///   components of the same object.
  VisTriggerSourceComponent_cl(const char *szName=NULL, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE) : 
      IVisTriggerBaseComponent_cl(szName,iComponentFlags)
  {
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC VOVERRIDE ~VisTriggerSourceComponent_cl();

  /// \brief
  ///   Overidden IVObjectComponent function
  VISION_APIFUNC VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overidden IVObjectComponent function
  VISION_APIFUNC VOVERRIDE bool IsRelevantForSerialization() const;


  /// \brief
  ///   de-attaches all associated targets from this source. To establish a link, use static
  ///   IVisTriggerBaseComponent_cl::OnLink function
  VISION_APIFUNC void UnlinkAllTargets();

  /// \brief
  ///   This is the key function of this component: It triggers the event on every associated
  ///   target, i.e. calls VisTriggerTargetComponent_cl::OnTrigger
  /// 
  /// It is up to the game code's responsibility to call this function when the targets are to be
  /// triggered.
  inline void TriggerAllTargets();
  
  //serialization
  V_DECLARE_SERIAL_DLLEXP( VisTriggerSourceComponent_cl,  VISION_APIDATA )
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
  V_DECLARE_VARTABLE(VisTriggerSourceComponent_cl, VISION_APIDATA)

  VObjectComponentCollection m_Targets;
};


/// \brief
///   Object component that defines a trigger target
/// 
/// An object (e.g. entity) can have one or more trigger targets which are components that can
/// receive trigger events sent by an arbitrary number of sources. For instance a light switch
/// (source) can toggle dynamic lights (targets).
/// 
/// vForge gathers components of this type via reflection and converts them to link targets in
/// vForge, so artists can graphically link trigger sources and targets.
/// 
/// All components of this type are added to the global VisTriggerTargetComponent_cl::ElementManager list so
/// all instances in the scene can be accessed through that list.
class VisTriggerTargetComponent_cl : public IVisTriggerBaseComponent_cl
{
public:

  /// \brief
  ///   Constructor
  /// 
  /// \param szName
  ///   Name of the component.
  /// 
  /// \param iComponentFlags
  ///   Component bitflags that define the behavior of the component. See constructor of
  ///   IVObjectComponent
  VisTriggerTargetComponent_cl(const char *szName=NULL, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE) : 
      IVisTriggerBaseComponent_cl(szName,iComponentFlags)
  {
  }

  /// \brief
  ///   Destructor
  VISION_APIFUNC VOVERRIDE ~VisTriggerTargetComponent_cl();

  /// \brief
  ///   Overidden IVObjectComponent function
  VISION_APIFUNC VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overidden IVObjectComponent function
  VISION_APIFUNC VOVERRIDE bool IsRelevantForSerialization() const ;

  /// \brief
  ///   This overridable is called when the component is triggered by a source component.
  /// 
  /// The default implementation sends a message with ID VIS_MSG_TRIGGER to the MessageFunction of
  /// the component's owner.
  /// 
  /// The iParamA member of the message is the pSrc pointer and iParamB is set to 'this'. The
  /// owner's MessageFunction implementation can thus react to any trigger combination.
  /// 
  /// \param pSrc
  ///   The source component that sends the trigger notification
  virtual void OnTrigger(VisTriggerSourceComponent_cl *pSrc)
  {
    // if the owner is derived from VisTypedEngineObject_cl (e.g. VisObject3D_cl for instance) notify via message function
    if (m_pOwner)
      m_pOwner->MessageFunction(VIS_MSG_TRIGGER, (INT_PTR)pSrc, (INT_PTR)this);
  }

  /// \brief
  ///   de-attaches this component from all sources that it is registered to. To establish a link,
  ///   use static IVisTriggerBaseComponent_cl::OnLink function
  VISION_APIFUNC void UnlinkAllSources();

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VisTriggerTargetComponent_cl,  VISION_APIDATA )
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
  V_DECLARE_VARTABLE(VisTriggerTargetComponent_cl, VISION_APIDATA)

  VObjectComponentCollection m_Sources; ///< we need references in both directions for proper cleanup

};



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void VisTriggerSourceComponent_cl::TriggerAllTargets()
{
  for (int i=0;i<m_Targets.Count();i++)
    ((VisTriggerTargetComponent_cl *)m_Targets.GetAt(i))->OnTrigger(this);
}


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
