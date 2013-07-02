/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSky.hpp

#ifndef DEFINE_VISAPISKY
#define DEFINE_VISAPISKY

#include <Vision/Runtime/Engine/System/ElementManager/VisApiElementManager.hpp>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>

class IVSky;
typedef VSmartPtr<IVSky> IVSkyPtr;

#define MAX_SKY_LAYERS    8

/// \brief
///  Sky class interface.
///
class IVSky : public VisTypedEngineObject_cl, public VRefCounter, public VUserDataObj, public VisElementManager_cl<class IVSky *>
{
public:
  /// \brief
  ///   Overridable render function.
  VISION_APIFUNC virtual void Render() = 0;

  /// \brief
  ///   Tick function.
  VISION_APIFUNC virtual void Tick(float dtime) = 0;

  /// \brief
  ///   Called before rendering.
  ///
  /// No Rendering possible here.
  VISION_APIFUNC virtual void PrepareForRendering() = 0;

  /// brief
  ///   Called after frame started and before rendering the sky. 
  ///
  /// Rendering is possible here.
  VISION_APIFUNC virtual void ComputeDynamicLookups() = 0;

  /// \brief
  ///   Determines face visibility for skyboxes.
  VISION_APIFUNC virtual void DetermineVisibleFaces() = 0;
    
  /// \brief
  ///   Serializes/deserializes this sky
  VISION_APIFUNC virtual void Serialize( VArchive &ar ) = 0;

  /// \brief
  ///   Writes this sky instance to a binary file.
  VISION_APIFUNC virtual void WriteToStream(IVFileOutStream *pOut) = 0;

  /// \brief
  ///   Sets the visible bitmask of the sky instance
  VISION_APIFUNC virtual void SetVisibleBitmask(unsigned int iBitmask) = 0;

  /// \brief
  ///   Returns the visible bitmask.
  VISION_APIFUNC virtual unsigned int GetVisibleBitmask() const = 0;  

  /// \brief
  ///   Optional callback to cache any precomputed data.
  VISION_APIFUNC virtual void OnSaveScene(const VString &path) = 0;

  /// \brief
  ///   Non-virtual overload of Release
  inline void Release()
  {
    int iOldRef = m_iRefCount;
    VRefCounter::Release();
    if (iOldRef==2 && IsObjectFlagSet(VObjectFlag_AutoDispose) && !IsObjectFlagSet(VObjectFlag_Disposing))
      DisposeObject();
  }
};

VISION_ELEMENTMANAGER_TEMPLATE_DECL(IVSky)

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
