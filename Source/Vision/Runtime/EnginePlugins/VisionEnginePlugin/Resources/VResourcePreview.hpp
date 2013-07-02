/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VResourcePreview.hpp

#ifndef _VRESOURCEPREVIEW_HPP_INCLUDED_
#define _VRESOURCEPREVIEW_HPP_INCLUDED_

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Base/System/Resource/VResource.hpp>

/// \brief
///   Custom render loop for rendering resource previews. Used by VResourcePreview.
class VResourcePreviewRenderloop : public VisionRenderLoop_cl
{
public:
  /// \brief
  ///   Constructor of the preview render loop for the given IVResourcePreview.
  ///
  /// \param pPreview
  ///   The resource preview to be rendered.
  ///
  /// \param clearColor
  ///   The clear color (= background color) to use in the preview image.
  EFFECTS_IMPEXP VResourcePreviewRenderloop(IVResourcePreview *pPreview, VColorRef clearColor);

private:
  VResourcePreviewRenderloop(const VResourcePreviewRenderloop&);
  VResourcePreviewRenderloop& operator=(const VResourcePreviewRenderloop&);

public:
  EFFECTS_IMPEXP virtual void OnDoRenderLoop(void *pUserData) HKV_OVERRIDE;

private:
  IVResourcePreview *m_pPreview;
  VColorRef m_ClearColor;
};


/// \brief
///   Implements the IVResourcePreview interface from vBase, providing useful default implementations
///   for functions related to rendering and saving. The class also stores global settings that apply
///   to all its instances like the default thumbnail and camera configuration.
class VResourcePreview : public IVResourcePreview
{
public:
  /// \brief
  ///   Constructor; initializes this resource preview instance.
  ///
  /// \param pOwner
  ///   The managed resource for which this is the preview.
  VResourcePreview(VManagedResource *pOwner);
private:
  VResourcePreview(const VResourcePreview&);
  VResourcePreview& operator=(const VResourcePreview&);

public:
  /// \brief
  ///   Tries to create a resource preview for \c pResource.
  ///
  /// This method first asks the resource itself whether it can create its on preview; and
  /// then, if no preview could be generated, tests the resource's manager against known
  /// resource managers and tries to create the preview this way.
  ///
  /// \param pResource
  ///   The resource for which the preview is created.
  ///
  /// \return
  ///   The resource preview, or \c null, if none could be created.
  EFFECTS_IMPEXP static IVResourcePreview *CreateResourcePreview(VManagedResource *pResource);

  EFFECTS_IMPEXP virtual bool SupportsImageThumbnails() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual bool SaveImageThumbnail(const char* szFileName = NULL, int iResolutionX = 0, int iResolutionY = 0, VColorRef* backgroundColor = NULL) HKV_OVERRIDE;

  /// \brief
  ///   Overwritten from base class. Overwrites the default size and output file.
  ///
  /// \param sAbsDestFilenameOut
  ///   \c GetThumbnailFilename is stored in here.
  ///
  /// \param iPreferredResXOut
  ///   This implementation does not change the variable.
  ///
  /// \param iPreferredResYOut
  ///   This implementation does not change the variable.
  ///
  /// \sa IVResourcePreview, GetThumbnailFilename
  EFFECTS_IMPEXP virtual void QueryImageThumbnailParameters(VString &sAbsDestFilenameOut, int &iPreferredResXOut, int &iPreferredResYOut) HKV_OVERRIDE;

  /// \brief
  ///   Overrides the base implementation. Returns the filename in which the thumbnail is stored. This implementation returns [OwnerFilename]_data/Thumbnail.[jpg/bmp].
  EFFECTS_IMPEXP virtual VString GetThumbnailFilename() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual bool ThumbnailAlreadySaved() HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual bool DeleteThumbnail() HKV_OVERRIDE;

  /// \brief
  ///   Pauses/unpauses the animation in resource previews.
  ///
  /// \param bPause
  ///   Whether the animation should be paused.
  EFFECTS_IMPEXP static void PauseAnimation(bool bPause);

  /// \brief
  ///   Returns whether resource preview animations are paused.
  /// \return
  ///   \c true if preview animations are paused; \c false otherwise.
  EFFECTS_IMPEXP static bool IsAnimationPaused();

  /// \brief
  ///   Increments the distance from which resource previews are looked at.
  /// \param fDelta
  ///   The distance to increase by; in Vision units.
  EFFECTS_IMPEXP static void IncrementDistance(float fDelta);

  /// \brief
  ///   Sets the distance from which resource previews are looked at.
  /// \param fValue
  ///   The distance from which to look; in Vision units.
  EFFECTS_IMPEXP static void SetDistance(float fValue);

  /// \brief
  ///   Returns the distance from which resource previews are looked at.
  /// \return
  ///   The distance from which resource previews are looked at.
  EFFECTS_IMPEXP static float GetDistance();

  /// \brief
  ///   Sets whether the preview rendering should overwrite (and later restore) the
  ///   scene camera.
  /// \param bValue
  ///   \c true to overwrite the scene camera; \c false to leave it as is.
  EFFECTS_IMPEXP static void SetOverwriteSceneCamera(bool bValue);

  /// \brief
  ///   Returns whether the preview rendering should overwrite (and later restore) the
  ///   scene camera.
  /// \return
  ///   \c true if the camera should be overwritten; \c false otherwise.
  EFFECTS_IMPEXP static bool GetOverwriteSceneCamera();

  /// \brief
  ///   Returns the background color used for rendering resource thumbnails.
  /// \return
  ///   The background color used for rendering resource thumbnails.
  EFFECTS_IMPEXP static VColorRef GetThumbnailBackgroundColor();

  /// \brief
  ///   Sets the background color to use for rendering resource thumbnails.
  /// \param color
  ///   The background color to use for rendering resource thumbnails.
  EFFECTS_IMPEXP static void SetThumbnailBackgroundColor(VColorRef color);

  /// \brief
  ///   Returns the default X resolution for rendering resource thumbnails.
  /// \return
  ///   The default X resolution for rendering resource thumbnails.
  EFFECTS_IMPEXP static int GetDefaultThumbnailResolutionX();

  /// \brief
  ///   Sets the default X resolution for rendering resource thumbnails.
  /// \param iResX
  ///   The default X resolution for rendering resource thumbnails.
  EFFECTS_IMPEXP static void SetDefaultThumbnailResolutionX(int iResX);

  /// \brief
  ///   Returns the default Y resolution for rendering resource thumbnails.
  /// \return
  ///   The default Y resolution for rendering resource thumbnails.
  EFFECTS_IMPEXP static int GetDefaultThumbnailResolutionY();

  /// \brief
  ///   Sets the default Y resolution for rendering resource thumbnails.
  /// \param iResY
  ///   The default Y resolution for rendering resource thumbnails.
  EFFECTS_IMPEXP static void SetDefaultThumbnailResolutionY(int iResY);

  /// \brief
  ///   Returns whether thumbnails should be saved as JPG images (instead of BMP).
  /// \return
  ///   \c true if thumbnails are saved as JPG; \c false if they are saved as BMP.
  EFFECTS_IMPEXP static bool GetSaveThumbnailsAsJPG();

  /// \brief
  ///   Sets whether thumbnails should be saved as JPG images (instead of BMP).
  /// \param bAsJPG
  ///   \c true to save thumbnails as JPG; \c false to save them as BMP.
  EFFECTS_IMPEXP static void SetSaveThumbnailsAsJPG(bool bAsJPG);

  /// \brief
  ///   Returns whether meshes are constantly rotated in the resource preview.
  /// \return
  ///   \c true if meshes are rotated; \c false if not.
  EFFECTS_IMPEXP static bool GetThumbnailRotateMeshes();

  /// \brief
  ///   Sets whether meshes should be constantly rotated in the resource preview.
  /// \param bRotate
  ///   \c true to rotate meshes, \c false to not rotate them.
  EFFECTS_IMPEXP static void SetThumbnailRotateMeshes(bool bRotate);

  /// \brief
  ///   Saves a render target to a file.
  ///
  /// The file format is determined from the extension of the file name passed to this
  /// function; so it must be one of (BMP, JPG, TGA, DDS). Comparison of file extensions
  /// is not case-sensitive.
  ///
  /// \param pTex
  ///   The render target to be saved.
  /// \param szAbsFilename
  ///   The name of the file to save the render target to.
  /// \return
  ///   \c true if the render target was successfully saved; \c false otherwise.
  EFFECTS_IMPEXP static bool SaveRenderTarget(VisRenderableTexture_cl *pTex, const char *szAbsFilename);

protected:
  /// \brief
  ///   Configures a transformation matrix to look at a bounding box from an angle and from a distance.
  /// \param transform
  ///   The transformation matrix to write the computed transformation to.
  /// \param fYaw
  ///   The yaw angle from which to look at the bounding box.
  /// \param bbox
  ///   The bounding box to look at.
  /// \param offset
  ///   The distance from which to look at the bounding box.
  EFFECTS_IMPEXP static void LookAtBBox(hkvMat4 &transform, float fYaw, const hkvAlignedBBox &bbox, const hkvVec3& offset);

  /// \brief
  ///   Recalculates the camera distance so the whole bbox is inside the camera frustum.
  EFFECTS_IMPEXP static void UpdateDistanceFromBBox(const hkvAlignedBBox& bbox);
  
  /// \brief
  ///   Helper to be used during preview rendering to simulate a directional light for light-grid based shading.
  EFFECTS_IMPEXP static void TrackDirectionalLight();

private:
  static bool m_bPauseAnimation;
  static float m_fDistance;
  static bool m_bOverwriteSceneCamera;

  static VColorRef m_thumbnailBackgroundColor;
  static int m_iThumbnailResolutionX;
  static int m_iThumbnailResolutionY;
  static bool m_bSaveThumbnailsAsJPG;
  static bool m_bThumbnailRotateMeshes;
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
