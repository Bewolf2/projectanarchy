/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VProgressStatus.hpp

#ifndef VPROGRESSSTATUS_HPP_INCLUDED
#define VPROGRESSSTATUS_HPP_INCLUDED

#define VPROGRESSSTATUS_MAXSTACKDEPTH_NOALLOC    16
#define VPROGRESSSTATUS_MAXSTACKDEPTH            256

/// \brief
///   An instance of this class allows for implementation of progress bars while loading a scene
/// 
/// It triggers a virtual overridable function whenever the progress percentage value changes.
/// 
/// A custom object of this class can thus be passed to loading code to update the progress bar.
class VProgressStatus
{
public:

  ///
  /// @name Init /deinit
  /// @{
  ///

  /// \brief
  ///   Constructor
  VBASE_IMPEXP VProgressStatus();

  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VProgressStatus();

  /// \brief
  ///   Resets the progress, including the range stack
  VBASE_IMPEXP void Reset();

  ///
  /// @}
  ///

  ///
  /// @name Properties
  /// @{
  ///

  /// \brief
  ///   Sets the sensitivity for calling the virtual OnProgressChanged function.
  /// 
  /// For instance, if set to 1.0f, the OnProgressChanged is called whenever progress changes
  /// accumulate to 1.0 since the last call.
  /// 
  /// This is useful if rendering the status bar is performance critical.
  /// 
  /// The default value is 0.5
  ///
  ///\param fPercentage
  ///   New granularity
  inline void SetGranularity(float fPercentage) 
  {
    m_fGranularity=fPercentage;
  }

  ///
  /// @}
  ///

  ///
  /// @name Update Function
  /// @{
  ///

  /// \brief
  ///   Creates a new percentage range so upcoming calls of SetProgress with range 0..100% map to
  ///   the specified new range
  /// 
  /// This works like a stack, so the PopRange has to be called at some point.
  /// 
  /// This stack makes it very easy to implement loading and modifying sub-ranges of a loading
  /// progress.
  /// 
  /// As an example, consider an entity that's about to be loaded and updates the progress status from
  /// 10% to 20%. Now this entity loads a .model file which shall also call the progress while
  /// model loading. Instead of telling the model loading that it should update range 10% to 20%,
  /// just define a new range via PushRange(10.f,20.f) so the model loading can always pass the
  /// full update range 0%..100% which is internally mapped to 10..20 and passed to the user via
  /// OnProgressChanged. When the model loading is finished, just call PopRange.
  /// 
  /// If no range is defined, the range naturally defaults to 0..100.
  /// 
  /// \param fNewStart
  ///   New percentage start value that is used for subsequent calls of SetProgress(0.f)
  /// 
  /// \param fNewEnd
  ///   New percentage end value that is used for subsequent calls of SetProgress(100.f)
  VBASE_IMPEXP void PushRange(float fNewStart, float fNewEnd);

  /// \brief
  ///   Another overload of PushRange that defines a range starting at the current percentage
  ///   value.
  VBASE_IMPEXP void PushRange(float fRangeLength);

  /// \brief
  ///   The PushRange counterpart, that activates the last range on the stack again
  VBASE_IMPEXP void PopRange(bool bFinished=true);

  /// \brief
  ///   Sets the current progress percentage value. This is a relative value and will be mapped to
  ///   the active range defined via PushRange.
  VBASE_IMPEXP void SetProgress(float fPercentage);

  /// \brief
  ///   Optional information string that can be passed to the progress object
  /// 
  /// \param szMsg
  ///   Message string constant (can be NULL)
  /// 
  /// \param bCopyString
  ///   If false (default) the status object just holds the szMsg pointer so no allocation takes
  ///   place.  The string pointer must remain valid in that case until the end (i.e. not just
  ///   created in a local function)
  VBASE_IMPEXP void SetProgressStatusString(const char *szMsg, bool bCopyString=false);

  ///
  /// @}
  ///

  ///
  /// @name Query Status Functions
  /// @{
  ///

  /// \brief
  ///   Returns the current percentage progress value mapped to the current active range. This
  ///   value can be used directly for status display. See also OnProgressChanged.
  inline float GetCurrentProgress() const {return m_fCurrentProgress;}

  /// \brief
  ///   Returns the status string that has been set via SetProgressStatusString. See also
  ///   OnStatusStringChanged.
  inline const char* GetProgressStatusString() const {return m_szStatusString;}

  /// \brief
  ///   Returns the current stack depth of the PushRange/PopRange stack.
  inline int GetStackPos() const {return m_iCurrentStackPos;}

  /// \brief
  ///   Helper function to remap the passed [0..100] percentage value to the absolute value defined
  ///   by the current range stack
  inline float MakePercentageAbsolute(float fPercentage) const
  {
    return m_ProgressStack[m_iCurrentStackPos].MakeAbsolute(fPercentage);
  }


  ///
  /// @}
  ///

  ///
  /// @name Virtual Overridables
  /// @{
  ///

  /// \brief
  ///   Virtual overridable that is triggered by the loading code when a loading process starts
  ///   (e.g. scene loading)
  VBASE_IMPEXP virtual void OnStart();

  /// \brief
  ///   Virtual overridable that is triggered by the loading code when a loading process has
  ///   finished (e.g. scene loading)
  VBASE_IMPEXP virtual void OnFinish();

  /// \brief
  ///   Virtual overridable that is triggered when the current progress percentage value has
  ///   changed by a certain amount defined via SetGranularity.
  /// 
  /// The actual progress value can be queried via GetCurrentProgress.
  VBASE_IMPEXP virtual void OnProgressChanged();

  /// \brief
  ///   Virtual overridable that is triggered when the current status string
  ///   (GetProgressStatusString) has changed
  VBASE_IMPEXP virtual void OnStatusStringChanged();

  /// \brief
  ///   Virtual overridable that can be triggered by users if they want to abort a running progress. This base implementation just sets the m_bWantsAbort flag
  VBASE_IMPEXP virtual void SetAbort(bool bState = true);

  /// \brief
  ///   Returns the current abort status (previously set via SetAbort)
  VBASE_IMPEXP virtual bool WantsAbort() const;
  

protected:
  
  /// \brief
  ///   Structure that represents a sub range in the progress stack.
  struct VProgressStatusStackEntry
  {
    /// \brief
    ///   Constructor
    VProgressStatusStackEntry() {fRangeStart=0.f;fRangeScale=1.f;}

    /// \brief
    ///   Sets the progress range.
    inline void Set(float fStart, float fEnd) {fRangeStart=fStart;fRangeScale=0.01f*(fEnd-fStart);}

    /// \brief
    ///   Transforms the passed value.
    inline float MakeAbsolute(float fVal) const
    {
      if (fVal<0.f) return fRangeStart;
      if (fVal>100.f) fVal=100.f;
      return fRangeStart+fVal*fRangeScale;
    }
    /// \brief
    ///   Makes the passed absolute value a relative 0..1 value inside this range.
    inline float MakeRelative(float fVal)
    {
      fVal -= fRangeStart;
      if (fVal<=0.f || fRangeScale<=0.f)
        return 0.f;
      return fVal/fRangeScale;
    }

    float fRangeStart; ///< Range start value
    float fRangeScale; ///< 1.0 / (end-start);
  ///
  /// @}
  ///

  };

  /// \brief
  ///   Returns the current stack entry
  inline VProgressStatusStackEntry& CurrentRange() {return m_ProgressStack[hkvMath::Min(m_iCurrentStackPos,VPROGRESSSTATUS_MAXSTACKDEPTH-1)];}

  float m_fGranularity; ///< update granularity
  float m_fCurrentProgress, m_fLastUpdateProgress;  ///< internal values
  int m_iCurrentStackPos; ///< Stack depth
  VProgressStatusStackEntry *m_ProgressStack; ///< pointer to stack array
  VProgressStatusStackEntry m_ProgressStackNoAlloc[VPROGRESSSTATUS_MAXSTACKDEPTH_NOALLOC]; ///< internal temp mem
  
  VString m_sStatusString;     ///< Current status string
  const char *m_szStatusString;///< Current status string (shallow pointer)

  bool m_bWantsAbort;   ///< Flag that determines whether the user wants to abort loading. 
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
