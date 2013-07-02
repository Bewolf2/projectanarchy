/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file

#ifndef VISAPICONTEXTVIEWPROPERTIES_HPP_INCLUDED
#define VISAPICONTEXTVIEWPROPERTIES_HPP_INCLUDED

#include <Vision/Runtime/Base/VBase.hpp>

/// \brief
///  A class that holds context properties such as FOV or clip planes required for computing the projection matrix.
///
/// This class can be shared by multiple contexts to keep projection properties synchronized.
class VisContextViewProperties : public VRefCounter
{
public:
  /// \brief Constructor.
  VISION_APIFUNC VisContextViewProperties();

  /// \brief Returns the computed projection matrix.
  ///
  /// \param yRange
  ///   Supply hkvClipSpaceYRange::OneToMinusOne to retrieve a projection that flips the y screen space coordinate.
  VISION_APIFUNC const hkvMat4& getProjectionMatrix(hkvClipSpaceYRange::Enum yRange = hkvClipSpaceYRange::MinusOneToOne);

  /// \brief Sets the projection type to be used to compute the projection matrix.
  VISION_APIFUNC void setProjectionType(VisProjectionType_e eType);
  
  /// \brief Returns the previously set projection type.
  inline VISION_APIFUNC VisProjectionType_e getProjectionType()
  {
    return m_eProjectionType;
  }

  /// \brief Supplies a custom projection matrix to be used when the projection type is VIS_PROJECTIONTYPE_CUSTOM.
  VISION_APIFUNC void setCustomProjection(const hkvMat4& projection);

  /// \brief Sets the field of view to be used when the projection type is VIS_PROJECTIONTYPE_PERSPECTIVE.
  ///
  /// If either FOV value is 0.0f, it will be computed automatically using the other value and the current
  /// display aspect ratio.
  ///
  /// \param fFovX
  ///   The horizontal field of view angle in degrees. Use 0.0f to derive it from fFovY via the display aspect ratio.
  ///
  /// \param fFovY
  ///   The vertical field of view angle in degrees. Use 0.0f to derive it from fFovX via the display aspect ratio.
  VISION_APIFUNC void setFov(float fFovX, float fFovY);

  /// \brief Retrieves the horizontal and vertical FOV angle previously set.
  VISION_APIFUNC void getFov(float& fFovX, float& fFovY) const
  {
    fFovX = m_fFovX;
    fFovY = m_fFovY;
  }

  /// \brief Retrieves non-zero horizontal and vertical FOV angles by computing previously set zero values from the display aspect ratio.
  VISION_APIFUNC void getFinalFov(float& fFovX, float& fFovY) const
  {
    fFovX = m_fFovX;
    fFovY = m_fFovY;
    hkvMathHelpers::adjustFovsForAspectRatio(fFovX, fFovY, m_fDisplayAspectRatio);
  }

  /// \brief Retrieves the horizontal FOV angle previously set.
  VISION_APIFUNC float getFovX() const
  {
    //VASSERT(m_eProjectionType == VIS_PROJECTIONTYPE_PERSPECTIVE);
    return m_fFovX;
  }

  /// \brief Retrieves the vertical FOV angle previously set.
  VISION_APIFUNC float getFovY() const
  {
    //VASSERT(m_eProjectionType == VIS_PROJECTIONTYPE_PERSPECTIVE);
    return m_fFovY;
  }

  /// \brief Sets the display aspect ratio to be used for auto-FOV computation.
  ///
  /// By default, it is set to the current display device's physical aspect ratio.
  ///
  /// \param fDisplayAspectRatio
  ///   The display aspect ratio that is to be used. The value is defined by width / height.
  VISION_APIFUNC void setDisplayAspectRatio(float fDisplayAspectRatio)
  {
    m_fDisplayAspectRatio = fDisplayAspectRatio;
    m_bIsDirty = true;
  }

  /// \brief Returns the display aspect ratio used for auto-FOV computation.
  VISION_APIFUNC float getDisplayAspectRatio() const
  {
    return m_fDisplayAspectRatio;
  }

  /// \brief Sets the near plane to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC or VIS_PROJECTIONTYPE_PERSPECTIVE.
  VISION_APIFUNC void setNear(float fNear);

  /// \brief Sets the far plane to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC or VIS_PROJECTIONTYPE_PERSPECTIVE.
  VISION_APIFUNC void setFar(float fFar);

  /// \brief Sets the clip planes to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC or VIS_PROJECTIONTYPE_PERSPECTIVE.
  VISION_APIFUNC void setClipPlanes(float fNear, float fFar);

  /// \brief Returns the near plane value previously set.
  VISION_APIFUNC float getNear() const
  {
    return m_fNear;
  }

  /// \brief Returns the far plane value previously set.
  VISION_APIFUNC float getFar() const
  {
    return m_fFar;
  }

  /// \brief Retrieves the clip plane values previously set.
  VISION_APIFUNC void getClipPlanes(float& fNear, float& fFar) const
  {
    fNear = m_fNear;
    fFar = m_fFar;
  }

  /// \brief Sets the projection width and height to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC.
  VISION_APIFUNC void setOrthographicSize(float fWidth, float fHeight);

  /// \brief Sets the projection width to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC.
  VISION_APIFUNC void setOrthographicWidth(float fWidth);

  /// \brief Sets the projection height to be used when the projection type is VIS_PROJECTIONTYPE_ORTHOGRAPHIC.
  VISION_APIFUNC void setOrthographicHeight(float fHeight);

  /// \brief Returns the projection width previously set.
  VISION_APIFUNC float getOrthographicWidth()
  {
    //VASSERT(m_eProjectionType == VIS_PROJECTIONTYPE_ORTHOGRAPHIC);
    return m_fOrthographicWidth;
  }

  /// \brief Returns the projection height previously set.
  VISION_APIFUNC float getOrthographicHeight()
  {
   // VASSERT(m_eProjectionType == VIS_PROJECTIONTYPE_ORTHOGRAPHIC);
    return m_fOrthographicHeight;
  }

private:
  VisProjectionType_e m_eProjectionType;
  bool m_bIsDirty;

  float m_fNear;
  float m_fFar;

  float m_fFovX;
  float m_fFovY;
  float m_fDisplayAspectRatio;

  float m_fOrthographicWidth;
  float m_fOrthographicHeight;

  // The cached projection matrix, as well as a version that flips the y-coordinate
  hkvMat4 m_cachedMatrix[2];
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
