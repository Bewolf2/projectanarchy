/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Policy for use with VGanimation
struct VGEOM2_IMPEXP_CLASS AnimKeyPolicy_Xform
{
public:
  /// \brief
  ///   Animation key
  struct Key
  {
    float   time;   // Time of key (unit unspecified, backends use seconds by convention).
    hkvMat4 xform;  // Transformation (space unspecified, backends use parent space by convention (scene/global space for root bones)).
  };

  /// \brief
  ///   Returns (possibly interpolated) translation vector for a given time
  ///
  /// \param t
  ///   Time to get (possibly interpolated) translation vector for
  ///
  /// \return
  ///   Translation vector (space unspecified, backends use parent space by convention (scene/global space for root bones))
  hkvVec3 GetTranslation(float t) const;

  /// \brief
  ///   Returns (possibly interpolated) rotation quaternion for a given time
  ///
  /// \param t
  ///   Time to get (possibly interpolated) rotation quaternion for
  ///
  /// \return
  ///   Rotation quaternion (space unspecified, backends use parent space by convention (scene/global space for root bones))
  hkvQuat     GetRotation(float t) const;

  /// \brief
  ///   Returns (possibly interpolated) scale vector for a given time
  ///
  /// \param t
  ///   Time to get (possibly interpolated) scale vector for
  ///
  /// \return
  ///   Scale vector (space unspecified, backends use parent space by convention (scene/global space for root bones))
  hkvVec3 GetScale(float t) const;

  /// \brief
  ///   Returns (possibly interpolated) transformation for a given time
  ///
  /// \param t
  ///   Time to get (possibly interpolated) transformation for
  ///
  /// \return
  ///   Transformation matrix (space unspecified, backends use parent space by convention (scene/global space for root bones))
  hkvMat4 GetTransformation(float t) const;


private:
  // Find keys left and right to time t.
  void FindKeysFor(float& t, int& l, int& r) const;


protected:
  VArray<Key, const Key&> m_keys;
};



/// \brief
///   Policy for use with VGanimation
struct VGEOM2_IMPEXP_CLASS AnimKeyPolicy_Vertex
{
public:
  /// \brief
  ///   Animation key
  struct Key
  {
    float time;   // Time of key (unit unspecified, backends use seconds by convention).

    /// \brief
    ///   Animated vertex
    struct AnimatedVertex
    {
      int i;        // Index of animated vertex (usually indexes into vertexlist/mesh).
      VGVertex v;   // New vertex value.
    };
    LinkedList_cl<AnimatedVertex> v;  // Transformed vertices (space unspecified, backends use parent space by convention (relative to unanimated vertex list)).
  };

protected:
  VArray<Key, const Key&> m_keys;
};



/// \brief
///   Animation class
///
/// Stores animation keys and related information for a single animated object/entity.
template<class AnimKeyPolicy>
class VGAnimation : public AnimKeyPolicy, public VGNameable
{
public:
  typedef typename AnimKeyPolicy::Key Key;

  /// \brief
  ///   Animation event
  struct Event
  {
    float   time;     // Time of event (unit unspecified, backends use seconds by convention).
    VString eventStr; // Event string (usually name, but this is user defined).
  };


public:
  VGAnimation()                                           { }
  VGAnimation(const VString& name) : VGNameable(name)     { }
  virtual ~VGAnimation()                                  { }


  /// \brief
  ///   Resets the animation (clears keys, etc.)
  inline void           Reset()                           { SetName(VString()); m_keys.Reset(); m_events.Reset(); }



  /// \brief
  ///   Gets the number of animation keys
  ///
  /// \return
  ///   Number of animation keys
  inline int            GetNumKeys() const                { return m_keys.GetSize(); }

  /// \brief
  ///   Gets the number of events
  ///
  /// \return
  ///   Number of events
  inline int            GetNumEvents() const              { return m_events.GetSize(); }



  /// \brief
  ///   Gets the start time of this animation
  ///
  /// \return
  ///   Animation start time in seconds (of -1.f if no animation data)
  inline float          GetStart() const                  { return GetNumKeys() ? m_keys.Get(0).time : -1.f; }

  /// \brief
  ///   Gets the end time of this animation
  ///
  /// \return
  ///   Animation end time in seconds (of -1.f if no animation data)
  inline float          GetEnd() const                    { int n=GetNumKeys(); return n ? m_keys.Get(n-1).time : -1.f; }

  /// \brief
  ///   Gets the length of this animation
  ///
  /// \return
  ///   Animation length in seconds
  inline float          GetLength() const                 { return GetEnd()-GetStart(); }



  /// \brief
  ///   Looks up an animation key by time
  ///
  /// \param time
  ///   Time value of key to lookup
  ///
  /// \return
  ///   Index of found animation key - returns -1 if there is no key with passed-in time
  int                   FindKey(float time);

  /// \brief
  ///   Inserts an animation key
  ///
  /// \param k
  ///   Animation key to insert - inserted elements will be sorted by keys' times
  ///
  /// \return
  ///   Index of inserted animation key - returns -1 if key with same time is already present
  int                   InsertKey(const Key& k);

  /// \brief
  ///   Inserts an event
  ///
  /// \param e
  ///   Event to insert - inserted elements will be sorted by events' times
  ///
  /// \return
  ///   Index of inserted event
  int                   InsertEvent(const Event& e);



  /// \brief
  ///   Gets an animation key
  ///
  /// \param i
  ///   Animation key index
  ///
  /// \return
  ///   Animation key at specified index
  inline       Key&     GetKey(int i)                     { return m_keys[i]; }
  inline const Key&     GetKey(int i) const               { return m_keys[i]; }

  /// \brief
  ///   Gets an event
  ///
  /// \param i
  ///   Event index
  ///
  /// \return
  ///   Event at specified index
  inline       Event&   GetEvent(int i)                   { return m_events[i]; }
  inline const Event&   GetEvent(int i) const             { return m_events[i]; }


private:
  VArray<Event, const Event&> m_events;
};




template<class AnimKeyPolicy>
int VGAnimation<AnimKeyPolicy>::FindKey(float t)
{
  int n = m_keys.GetSize();
  for(int i=0; i<n; ++i)
  {
    if(GetKey(i).time == t)
      return i;

    if(GetKey(i).time > t)
      break;
  }
  return -1;
}


template<class AnimKeyPolicy>
int VGAnimation<AnimKeyPolicy>::InsertKey(const Key& k)
{
  int n = m_keys.GetSize();
  for(int i=0; i<n; ++i)
  {
    if(GetKey(i).time == k.time)
      return -1;

    if(GetKey(i).time > k.time)
    {
      m_keys.InsertAt(i, k);
      return i;
    }
  }
  return m_keys.Append(k);
}


template<class AnimKeyPolicy>
int VGAnimation<AnimKeyPolicy>::InsertEvent(const Event& e)
{
  int n = m_events.GetSize();
  for(int i=0; i<n; ++i)
  {
    if(GetEvent(i).time > e.time)
    {
      m_events.InsertAt(i, e);
      return i;
    }
  }
  return m_events.Append(e);
}



// Convenience typedefs.
typedef VGAnimation<AnimKeyPolicy_Xform>  VGAnimation_Node;
typedef VGAnimation<AnimKeyPolicy_Xform>  VGAnimation_Bone;
typedef VGAnimation<AnimKeyPolicy_Vertex> VGAnimation_Vertex;

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
