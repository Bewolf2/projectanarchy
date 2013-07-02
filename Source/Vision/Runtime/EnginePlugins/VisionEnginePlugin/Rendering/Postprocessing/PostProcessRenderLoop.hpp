/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


//
//  DO NOT USE THIS FILE!
//
//
// It is specific to a single engine sample and will be removed from the VisionEnginePlugin.
//


// HS#10363: This should be in the HeatHaze sample - hide it from the documentation system to prevent confusion.
#if !defined(_VISION_DOC)

/// \file PostProcessRenderLoop.hpp

#ifndef VIS_POSTPROCESSRENDERLOOP_HPP_INCLUDED
#define VIS_POSTPROCESSRENDERLOOP_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Param/VParam.hpp>


/// \brief
///   Class that represents a single tweakable parameter for post-processing effects
/// 
/// Each instance represents a tweakable, e.g. "bloomIntensity"
/// 
/// The GUI creates controls from a post processor's tweakables, e.g. sliders.
class PostProcessTweakable_cl
{
public:
  PostProcessTweakable_cl()
  {
    m_fMin=0.f;
    m_fMax=1.f;
    m_fValue=0.f;
    m_iSteps=0;
    m_iValue=0;
    m_eParamType = V_TYPE_NONE; ///< undefined
  }

  virtual ~PostProcessTweakable_cl(){}

  /// \brief
  ///   Sets the tweakable's name. Shows up in the GUI
  inline void SetParamName(const char *szName) {m_szParamName=szName;}
  inline const char *GetParamName() const {return m_szParamName;}

  /// \brief
  ///   Sets a typical slider based tweakable.
  void SetFloatRange(const char *szParamName, float fMin, float fMax, float fValue, int iSteps = 0)
  {
    if (szParamName) SetParamName(szParamName);
    m_fMin = fMin;
    m_fMax = fMax;
    m_iSteps = iSteps;
    m_fValue = fValue;
    m_eParamType = V_TYPE_FLOAT; ///< float -> slider
  }

  /// \brief
  ///   If tweakable is an enum (e.g. "blur mode") this must be overridden and return the number of
  ///   enum values.
  virtual int GetEnumCount() const {return 0;}

  /// \brief
  ///   If tweakable is an enum (e.g. "blur mode") this must be overridden and return the enum name
  ///   of specified index.
  virtual const char* GetEnumName(int iIndex) const {return NULL;}

  /// \brief
  ///   If tweakable is an enum (e.g. "blur mode") this must be overridden and return the enum
  ///   value of specified index.
  virtual int GetEnumValue(int iIndex) const {return 0;}


public:
  VString m_szParamName;
  float m_fMin;
  float m_fMax;
  int m_iSteps;
  float m_fValue; ///< set for float based params
  int m_iValue;   ///< set for int/enum based params. For enums, it is the enum value, not the index.
  VParamType m_eParamType; ///< currently supported: V_TYPE_FLOAT, V_TYPE_ENUM
};


class PostProcessRenderLoop_cl;
typedef VSmartPtr<PostProcessRenderLoop_cl> PostProcessRenderLoopPtr;

/// \brief
///   Base class of a post processing render loop
/// 
/// This class contains functions to obtain "tweakables" from the postprocessing effect.
/// 
/// These tweakables are mapped to the GUI as sliders etc.
class PostProcessRenderLoop_cl : public VisionRenderLoop_cl
{
public:

  /// \brief
  ///   Overridable that returns the name of the post-processing effect, e.g. "Glow"
  virtual const char *GetEffectName() const = 0;

  /// \brief
  ///   Overridable that returns the number of tweakables provided by this effect
  virtual int GetNumTweakables() const = 0;

  /// \brief
  ///   Overridable that returns a pointer to a tweakable. Index must be valid between 0 and
  ///   GetNumTweakables()-1
  virtual PostProcessTweakable_cl* GetTweakable(int iIndex) = 0;

  /// \brief
  ///   Overridable that gets called by the GUI if a tweakable has changed. Put the effect update
  ///   code in here.
  virtual void OnTweakableChanged(PostProcessTweakable_cl *pTweakable) = 0;

  /// \brief
  ///   Overridable that gets called by the GUI to reset all tweakables to their default value.
  virtual void Reset() {};

  /// \brief
  ///   Overridable that gets called by the GUI to check if a reset button should be displayed on
  ///   the dialog This should only return true if the Reset() function is implemented by the
  ///   renderloop
  virtual bool IsResetable() {return false;}

};

#endif

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
