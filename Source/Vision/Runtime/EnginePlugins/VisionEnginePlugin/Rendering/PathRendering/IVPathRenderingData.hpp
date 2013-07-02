/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file IVPathRenderingData.hpp

#ifndef IVPATHRENDERINGDATA_H_INCLUDED
#define IVPATHRENDERINGDATA_H_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/PathRendering/VPathRenderingModule.hpp>


/// \brief
///   Abstract interface class used to provide adapters for all kind of renderable pathes.
class IVPathRenderingData : public VRefCounter, public VTypedObject
{
public:
  /// \brief
  ///   Default constructor.
  PATHRND_IMPEXP IVPathRenderingData();

  /// \brief
  ///   Destructor.
  PATHRND_IMPEXP virtual ~IVPathRenderingData();

public:
  /// \brief
  ///   Sets the object instance containing the rendering data
  /// \param pObject
  ///   Typed Object that contains the path data.
  /// \return
  ///   Whether the set object is compatible with this adapter.
  PATHRND_IMPEXP virtual bool SetPathObject(VTypedObject* pObject) = 0;

  /// \brief
  ///   Indicates whether this instance contains valid rending data.
  /// \return
  ///   Whether this instance contains valid rending data.
  PATHRND_IMPEXP virtual bool IsValid() const { return true; }

  /// \brief
  ///   Returns the diameter of the path.
  PATHRND_IMPEXP virtual float GetDiameter() const = 0;

  /// \brief
  ///   Returns the length of each link of the path.
  PATHRND_IMPEXP virtual float GetLinkLength() const = 0;

  /// \brief
  ///   Returns the gap between the links of the path.
  PATHRND_IMPEXP virtual float GetLinkGap() const = 0;

  /// \brief
  ///   Returns the number of links of this path.
  PATHRND_IMPEXP virtual unsigned int GetNumLinks() const = 0;

  /// \brief
  ///   Returns the amount (in world units) by which the last path link is 
  ///   shorter or longer than required. The value is negative if the link
  ///   is longer, and positive if the link is shorter.
  PATHRND_IMPEXP virtual float GetLastLinkExtra() const = 0;

  /// \brief
  ///   Inquires the transformation of the specified link in world space.
  /// \param iIndex
  ///   index of the link whose transformation to get
  /// \param mRotation
  ///   output parameter; receives the rotation of the link
  /// \param vTranslation
  ///   output parameter; receives the translation of the link
  /// \return
  ///   \c true if a valid transformation could be determined, \c false 
  ///   if not. In the latter case, the contents of mRotation and vTranslation
  ///   are undefined.
  PATHRND_IMPEXP virtual bool GetLinkTransform(unsigned int iIndex, hkvMat3& mRotation, hkvVec3& vTranslation) const = 0;

  /// \brief
  ///   Inquires the transformation of all links in world space, optionally
  ///   removing the roll values or reversing the link order.
  /// \param pRotations
  ///   pointer to an array of rotation matrices. This array must be allocated
  ///   by the caller and be at least of the size returned by GetNumLinks().
  /// \param pTranslations
  ///   pointer to an array of translation vectors. This array must be allocated
  ///   by the caller and be at least of the size returned by GetNumLinks().
  /// \param bRemoveRoll
  ///   whether to remove the roll component in the output transforms
  /// \param bReverseOrder
  ///   whether to output the link transforms in reverse order, and with a yaw
  ///   of 180 degrees applied
  PATHRND_IMPEXP virtual void GetLinkTransforms(hkvMat3* pRotations, hkvVec3* pTranslations, bool bRemoveRoll, bool bReverseOrder) const = 0;

  /// \brief
  ///   Returns whether the path object has changed since last initialization. Needed to syncronize path renderer with path object.
  PATHRND_IMPEXP virtual bool HasDataChanged() { return false; }

protected:
  friend class VPathRendererBase;

  /// \brief
  ///   Gets called by renderer base class after all components of the owner object are attached and initialized.
  /// \param pOwner
  ///   Owner object that contains the path data.
  PATHRND_IMPEXP virtual void Init(VisTypedEngineObject_cl* pOwner) = 0;

public:
  V_DECLARE_SERIAL_DLLEXP(IVPathRenderingData, PATHRND_IMPEXP)

};

typedef VSmartPtr<IVPathRenderingData> IVPathRenderingDataPtr;


#endif //IVPATHRENDERINGDATA_H_INCLUDED

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
