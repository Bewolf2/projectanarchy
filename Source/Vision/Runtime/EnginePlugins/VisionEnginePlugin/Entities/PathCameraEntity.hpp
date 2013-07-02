/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file PathCameraEntity.hpp

#ifndef PATHCAMERAENTITY_HPP_INCLUDED
#define PATHCAMERAENTITY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>

class TiXmlElement;
class PathCameraEntity;

/// \brief
///   PathParameter : Information about the current position on a path. Used internally
class PathParameter
{
public:
  /// \brief
  ///   Constructor calls \c Clear for initialization.
  PathParameter() {Clear();}

  /// \brief
  ///   Clears all internal data.
  inline void Clear()
  {
    m_bFinished = false;
    m_pPath = NULL;
    m_fMinParam = 0.f;
    m_fMaxParam = 1.f;
    m_fTime = 5.f;
    m_fTimeScale = 1.f/m_fTime;
    m_fCurrentTime = 0.f;
    m_fRolliness = 0.f; // no roll
    m_fCurrentParam = m_fStartParam = m_fMinParam;
  }

  /// \brief
  ///   Resets the parameter to start again from the beginning.
  inline void Reset()
  {
    m_bFinished = false;
    m_fCurrentParam = m_fStartParam;
    m_fCurrentTime = 0.f;
  }

  /// \brief Returns the current progress in 0..1 range.
  inline float GetProgress() const
  {
    return (m_fCurrentParam-m_fMinParam)/(m_fMaxParam-m_fMinParam);
  }

  /// \brief Updates the parameter by advancing the values by the given time step.
  EFFECTS_IMPEXP void Handle(float dtime);

  /// \brief Returns the current camera position and orientation.
  EFFECTS_IMPEXP void EvaluatePosition(hkvVec3& vPos, hkvMat3 *pCamRot=NULL);

  /// \brief Reads the parameter state from an XML node.
  EFFECTS_IMPEXP void FromXMLNode(TiXmlElement *pNode);

  VisPath_cl *m_pPath;
  float m_fCurrentParam, m_fStartParam;
  float m_fMinParam,m_fMaxParam,m_fTimeScale,m_fTime,m_fCurrentTime,m_fRolliness;
  bool m_bFinished;
};

/// \brief
///   Class that represents a single playback within a the XML definition of the PathCameraEntityclass
///
/// An instance of this class corresponds to a "cameraaction" XML node. See dedicated XML description in the documentation
///
/// \see
///   PathCameraEntity
class PathCameraAction
{
public:
  inline PathCameraAction()
  {
    m_fFadeInTime = m_fFadeOutTime = 0.f;
    m_fDefaultFOV = -1.f;
    m_iEventCount = 0;
    m_pEventTime = NULL;
    m_pEventNode = NULL;
  }

  EFFECTS_IMPEXP ~PathCameraAction();

  /// \brief Advances the state of this object by the given time step.
  EFFECTS_IMPEXP void Handle(PathCameraEntity* pOwner, float dtime);

  /// \brief Returns whether the path has reached its end.
  inline bool IsFinished() const
  {
    if (m_PositionPath.m_pPath) 
      return m_PositionPath.m_bFinished;

    return m_LookatPath.m_bFinished;
  }

  /// \brief Returns whether the path has a valid position or look-at curve.
  inline bool IsValid() const
  {
    return m_PositionPath.m_pPath!=NULL || m_LookatPath.m_pPath!=NULL;
  }

  /// \brief Returns the camera fade-in/fade-out value for the current time.
  inline float GetBlendWeight() const
  {
    const PathParameter &path = m_PositionPath.m_pPath ? m_PositionPath:m_LookatPath;
    if (path.m_fCurrentTime<m_fFadeInTime)
      return path.m_fCurrentTime/m_fFadeInTime;
    if (m_fFadeOutTime>0.f && path.m_fCurrentTime>path.m_fTime-m_fFadeOutTime)
    {
      float fVal = (path.m_fTime-path.m_fCurrentTime)/m_fFadeOutTime;
      return hkvMath::Max(fVal,0.f);
    }
    return 1.f;
  }

  /// \brief Resets the camera position and orientation to the beginning.
  inline void Reset()
  {
    m_PositionPath.Reset();
    m_LookatPath.Reset();
  }

  /// \brief Computes the camera position and rotation for the current time.
  EFFECTS_IMPEXP void EvaluatePosition(hkvVec3& vPos, hkvMat3 &camRot);

  /// \brief Reads the data for the camera action from XML.
  EFFECTS_IMPEXP void FromXMLNode(TiXmlElement *pNode);


  ///\brief
	///  Static callback that triggers events inside the camera playback.
  ///
  ///The data object can be casted to class VPathEventCallbackDataObject. It provides information about the event XML node
  ///and the camera action that triggered the event.
	static EFFECTS_IMPEXP VisCallback_cl OnTriggerEvent;

  float m_fFadeInTime, m_fFadeOutTime;
  PathParameter m_PositionPath;
  PathParameter m_LookatPath;
  VCurve2DPtr m_spFOVCurve; ///< optional curve that defines the FOV over the path
  float m_fDefaultFOV;

  int m_iEventCount; ///< trigger actions
  float *m_pEventTime;
  TiXmlElement **m_pEventNode;
};


/// \brief
///   Callback data object that will be triggered by the PathCameraAction::OnTriggerEvent callback. It passes the full event node from the xml
///
/// \see
///   PathCameraAction::OnTriggerEvent
class VPathEventCallbackDataObject : public IVisCallbackDataObject_cl
{
public:
  VPathEventCallbackDataObject(TiXmlElement *pNode, float fTime, PathCameraAction *pAction)
    : IVisCallbackDataObject_cl(&PathCameraAction::OnTriggerEvent)
  {
    m_pEventNode = pNode;
    m_fPathPosition = fTime;
    m_pPathAction = pAction;
  }

  TiXmlElement *m_pEventNode;       ///< The full xml node that holds the event
  float m_fPathPosition;            ///< The current path parameter
  PathCameraAction *m_pPathAction;  ///< The playback action that triggered the event
};



///\brief
/// Entity class that attaches a camera to follow a camera spline path.
///   
///This entity class is the C++ counterpart for the path camera shape in vForge.
///The camera actions are defined in an XML file. See dedicated documentation.
class PathCameraEntity : public VisBaseEntity_cl
{
public:
  PathCameraEntity();
  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void DeInitFunction() HKV_OVERRIDE;

  ///\brief
	///  Load an XML camera script file from file and start it
	///
	///\param szFilename
	///Filename of the XML file
	///
	///\returns
	///true if loading was successful
	///
	EFFECTS_IMPEXP bool LoadScriptFile(const char *szFilename);

  ///\brief
	///  Unload the XML file
  EFFECTS_IMPEXP void FreeScriptFile();

  ///\brief
	///  Set the script filename without actually loading/starting it
  inline void SetScriptFilename(const char *szFilename) 
  {
    m_sScriptFile = szFilename;
  }

  ///\brief
  ///  Start the playback (i.e. attach camera and process the camera script)
  EFFECTS_IMPEXP bool Start();
  ///\brief
	///  Stop the playback
  EFFECTS_IMPEXP void Stop();
  ///\brief
	///  Pauses or resumes the playback
  EFFECTS_IMPEXP void SetPause(bool bStatus);
  ///\brief
	///  Internal update function
  EFFECTS_IMPEXP void TickFunction(float dtime);

  ///\brief
  ///  Sets the relative playback speed (1.0f = normal speed as defined in the XML, 2.0f is twice the speed)
  inline void SetTimeScaling(float fScale)
  {
    m_fTimeScaling = fScale;
  }

  ///\brief
  ///  Return the time scaling of this camera. The default is 1.0
  inline float GetTimeScaling() const
  {
    return m_fTimeScaling;
  }

  ///\brief
	///  Indicates whether playback is running or not
  inline bool IsPlaying() const {return m_iCurrentAction>=0;}

  ///\brief
	///  Indicates whether playback is currently paused
  inline bool IsPaused() const {return m_bPaused;}

  ///\brief
	///  Attaches this camera to the renderer node of the given index.
  EFFECTS_IMPEXP void AttachToRendererNode(int iRendererNodeIndex);

  V_DECLARE_SERIAL_DLLEXP( PathCameraEntity,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE {return TRUE;} // in editor and vscene
  EFFECTS_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context) HKV_OVERRIDE;

  // entity
  IMPLEMENT_OBJ_CLASS(PathCameraEntity);

  bool m_bPaused, m_bPlayLooped;
  VString m_sScriptFile;

  int m_iActionCount, m_iCurrentAction;
  PathCameraAction *m_pActions;

  VisScreenMaskPtr m_spFadeMask;
  float m_fStoreFOV[2];
  float m_fTimeScaling;
  int m_iRendererNodeIndex;
};


/// \brief
///   This is the analogue to the CameraPositionShape inside vForge. It can be used to attach the camera to fixed entities set in vForge
class CameraPositionEntity : public VisBaseEntity_cl
{
public:
  inline CameraPositionEntity()
  {
    NearClipDistance = FarClipDistance = FovX = 0.f;
  }

  ///\brief
  /// Takes the camera of the passed context and applies its own transformation and view settings to it
  ///
  /// \param pContext
  ///   The context that should receive a new camera position. When using a renderer node (e.g. deferred), pass pNode->GetReferenceContext()
  EFFECTS_IMPEXP void ApplyToContext(VisRenderContext_cl *pContext);

  ///\brief
  /// Static helper function to find a camera shape in the scene by key and then apply the position and view settings.
  ///
  /// \param pContext
  ///   The context that should receive a new camera position. When using a renderer node (e.g. deferred), pass pNode->GetReferenceContext()
  ///
  /// \param szKey
  ///   The key of the camera shape. Set as the ObjectKey string in vForge.
  EFFECTS_IMPEXP static CameraPositionEntity* ApplyToContext(VisRenderContext_cl *pContext, const char *szKey);

  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
  V_DECLARE_SERIAL_DLLEXP( CameraPositionEntity,  EFFECTS_IMPEXP );
  V_DECLARE_VARTABLE(CameraPositionEntity, EFFECTS_IMPEXP);

  // exposed members
  float NearClipDistance;
  float FarClipDistance;
  float FovX;
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
