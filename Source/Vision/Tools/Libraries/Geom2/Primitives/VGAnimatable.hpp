/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Base class for animatable objects/entities
///
/// Stores list of animation keys and related information for a single animated object/entity.
template<class KeyPolicy>
class VGAnimatable : public KeyPolicy
{
public:
  VGAnimatable()                { }
  virtual ~VGAnimatable() = 0   { }


  /// \brief
  ///   Adds a new animation
  ///
  /// \param b
  ///   Animation to add - note that it will be copied
  inline void                           AddAnimation(const VGAnimation<KeyPolicy>& b) { m_anims.Append(b); }

  /// \brief
  ///   Creates a new animation and returns a reference to it (same as implicitly adding a new animation)
  ///
  /// \param name
  ///   Name of new animation
  ///
  /// \return
  ///   Reference to newly created animation
  inline VGAnimation<KeyPolicy>&        CreateAnimation(const VString& name="")       { AddAnimation(VGAnimation<KeyPolicy>(name)); return GetAnimation(GetNumAnimations()-1); }//@@@test

  /// \brief
  ///   Return number of animations
  ///
  /// \return
  ///   Number of animations
  inline int                            GetNumAnimations() const                      { return m_anims.GetSize(); }

  /// \brief
  ///   Return a reference to a animation
  ///
  /// \param i
  ///   Index
  ///
  /// \return
  ///   Animation
  inline       VGAnimation<KeyPolicy>&  GetAnimation(int i)                           { return m_anims[i]; }
  inline const VGAnimation<KeyPolicy>&  GetAnimation(int i) const                     { return m_anims[i]; }

  /// \brief
  ///   Return an animation by name lookup (not efficient)
  ///
  /// \param name
  ///   Name
  ///
  /// \return
  ///   Index into animations or -1 if no animation found with given name
  inline int                            GetAnimation(const VString& name)             { int n=m_anims.GetSize(); for(int i=0; i<n; ++i) {       VGAnimation<KeyPolicy>& a=m_anims[i]; if(a.GetName()==name) return i; } return -1; }
  inline int                            GetAnimation(const VString& name) const       { int n=m_anims.GetSize(); for(int i=0; i<n; ++i) { const VGAnimation<KeyPolicy>& a=m_anims[i]; if(a.GetName()==name) return i; } return -1; }

  /// \brief
  ///   Remove all animations
  inline void                           RemoveAnimations()                            { m_anims.RemoveAll(); }


  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Animatable to assign
  ///
  /// \return
  ///   Reference to this animatable
  inline VGAnimatable& operator=(const VGAnimatable& rhs)       { m_anims = rhs.m_anims; return *this; }

  /// \brief
  ///   Swap
  ///
  /// \param other
  ///   Animatable to swap with
  inline void Swap(VGAnimatable& other)                         { m_anims.Swap(other.m_anims); }


private:
  // Animation list.
  VArray<VGAnimation<KeyPolicy>, const VGAnimation<KeyPolicy>&> m_anims;
};

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
