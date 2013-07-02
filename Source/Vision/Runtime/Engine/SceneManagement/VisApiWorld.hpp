/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiWorld.hpp

#ifndef DEFINE_VISAPIWORLD
#define DEFINE_VISAPIWORLD


#include <Vision/Runtime/Engine/System/VisApiMain.hpp>


/// \brief
///   Base class for defining a customized coordinate system in Vision
/// 
/// Gives a set of routines to help plugins develop coordinate system aware and agnostic routines.
/// In systems that use pivoting and relocating zones, which are capable of pivoting along the up axis
/// the coordinate system object also gives a method to get a coherent height above base method.
///
/// Note that this does not alter anything about the rendering system, matrices, or Vision objects.
/// Rather, plugins are expected to use information about the systems basis to implement coordinate system agnostic algorithms.
class VISION_APIFUNC VisCoordinateSystem_cl
{
public:

  VisCoordinateSystem_cl() {}

  virtual ~VisCoordinateSystem_cl() {}

  /// \brief
  ///   Query an accurate world position for an object
  /// 
  /// Query the true world space position for an object in pivoting and relocation systems.
  /// 
  /// \param[in] object
  ///   Reference object for the location of the query.
  /// 
  /// \param[out] worldPosition
  ///   The world position
  virtual void GetPosition( VisObject3D_cl* object, hkvVec3d& worldPosition ) const = 0;

  /// \brief
  ///   Query the basis vectors of the system given the position of an object
  /// 
  /// For some systems, notably spherical, the basis vectors change depending on the location of an object.
  /// This method returns an object aware basis.  If the system is constant for all objects, it just returns the system wide basis.
  /// 
  /// \param[in] worldPosition
	///    Location of the object (in world space) to fetch basis vectors for.
  /// 
  /// \param[out] basisVectors
  ///   The basis
  virtual void GetBasisVectors( hkvVec3d const& worldPosition, hkvMat3& basisVectors ) const = 0;

  /// \brief
  ///   Return a coherent, planar position for an object of the form (planarX, planarY, altitudeAboveBaseSurface)
  /// 
  /// Return a coordinate of the form (planarX, planarY, planarZ), where planarX and planarY describe planar coordinates for object,
  /// referenced to the current global pivot.  planarZ provides altitude above the systems surface.  For example, in a spherical system
  /// with radius r, a given world space position could look like:
  ///
  /// x = (sin(theta)sin(phi) * (r + viewerAltitude)) - pivotX;
  /// y = (cos(theta)sin(phi) * (r + viewerAltitude)) - pivotY;
  /// z = (cos(phi) * (r + viewerAltitude)) - pivotZ;
  ///
  /// It's difficult to understand what the coordinate means, even when adjusted from local space back to world space.  
  /// If we call the above, local triplet SphericalXyz and the three basis vectors XBasis, YBasis, and ZBasis.  The local frame 
  /// provides a planar system parameterized as:
  ///
  /// x' = (SphericalXyz).dot(XBasis)
  /// y' = (SphericalXyz).dot(YBasis)
  /// z' = viewerAltitude;
  ///
  /// Or, more intuitively, a planar coordinate for the tangent plane of the object given it's current zone local position as well as
  /// height above the reference surface.  This can be particular useful even in fairly vanilla systems if pivoting over the z axis is allowed
  /// for getting a nice value for height above the ground.
  /// 
  /// \param[in] worldPosition
	///    Location of the object (in world space) you want to convert.
  ///
  /// \param[out] localFrame
  ///   The local frame described above
  virtual void GetInLocalFrame( hkvVec3d const& worldPosition, hkvVec3& localFrame ) const = 0;

  /// \brief
  ///   Return a 4x4 matrix for converting from a local frame to the world space.
  ///
  /// \param[in] worldPosition
	///    Location of the object (in world space) you want to convert.
  ///
  /// \param[out] invLocalFrame
  ///   The matrix.
  ///
  /// \sa VisCoordinateSystem_cl::GetInLocalFrame
  virtual void GetLocalFrameInversionMatrix( hkvVec3d const& worldPosition, hkvMat4& invLocalFrame ) const = 0;

  /// \brief
  ///   Return a 4x4 matrix for converting from world space to local frame.
  ///
  /// \param[in] worldPosition
	///    Location of the object (in world space) you want to convert.
  ///
  /// \param[out] localFrame
  ///   The matrix.
  ///
  /// \sa VisCoordinateSystem_cl::GetInLocalFrame
  virtual void GetLocalFrameMatrix( hkvVec3d const& worldPosition, hkvMat4& localFrame ) const = 0;

  /// \brief
  ///   Return a fourth degree polynomial to describe the falloff of the base surface at a given location
  /// 
  /// The coefficients describe a polynomial to fit the curve of the underlying surface at a given location:
  ///
  /// x = XBasis.dot(targetObject - observerPosition)
  /// dx = coefficients.x + coefficients.y * x + coefficients.z * x^{2} + coefficients.w * x^{3}
  ///
  /// Where dx will approximate the falloff of the curve from the observerPosition.  This is useful for adjusting effects that need to be
  /// relative to the surface's base.  For example, if we are working in a local frame and reconstructing information from a depth buffer,
  /// the reconstructed position will be equal to (planarX, planarY, altitude + (sqrt(systemRadius^2 + sqrt(planarX^2 + planarY^2)) - systemRadius)))
  /// which is not always what we want.  If we take the above coordinate and add dx to the incorrect altitude, we will get the actual height above the system's
  /// base surface.
  ///
  /// As a second example, we can bend a plane around the viewer by solving the above polynomial.  Namely, for a vertex N meters away from the viewer, we just plug
  /// N in as our x and adjust the z value of the vertex by the resulting dx.
  ///
  /// For planar systems, this will be coefficients = hkvVec4::ZeroVector();
  /// 
  /// \param[in] observerPosition
  ///    Position to observe the coordinate system's surface
  ///
  /// \param[in] viewingDistance
  ///    Maximum distance that will be viewed (e.g., far plane)
  ///
  /// \param[out] coefficients
  ///   The resulting coefficients
  virtual void GetEllipticalPolynomialCoefficients( hkvVec3d const& observerPosition, double viewingDistance, hkvVec4& coefficients ) const = 0;

  /// \brief
  ///   Return the cosine of the angle between a viewer and the horizon of the system
  ///
  /// \param[in] observerPosition
  ///    Position to observe the coordinate system's surface
  ///
  /// \return 
  ///   cos(gamma) where gamma is the angle between the viewer and the horizon
  virtual float GetCosineHorizon( hkvVec3d const& observerPosition ) const = 0;

  /// \brief
  ///   Sets a scene reference position
  ///
  /// The scene reference position is an absolute position in that all scene elements are considered to be relative to.
  /// The actual semantics of this varies depending on the underlying coordinate system used-- and may not be applicable at all
  /// to the underlying system.  For instance, a local flat earth simulation can set this coordinate in Geodetic coordinates as a Lat/Long/Alt reference
  /// The runtime does not do anything explicit with this value
  /// vForge sets this value to the reference position property in the Main Layer (as Lat/Long/Alt). It is exported along with the scene and set by the
  /// scene loader class.
  ///
  /// \param refPos
  ///   The scene reference position to be used.
  virtual void SetSceneReferencePosition(const hkvVec3d& refPos) = 0;

  /// \brief
  ///   Get the scene reference position if applicable for a coordinate system
  virtual void GetSceneReferencePosition(hkvVec3d& referencePosition) const = 0;

private:

};

/// \brief
///   Default coordinate system for Vision
/// 
/// A vanilla implementation of the VisCoordinateSystem_cl for a planar system with z up and allowing pivoting over the z axis
class VisDefaultCoordinateSystem : public VisCoordinateSystem_cl
{
public:

  VisDefaultCoordinateSystem();

  virtual ~VisDefaultCoordinateSystem();

  /// \brief
  ///   Query the world space position of an object
  /// 
  /// \param[in] object
  ///   The object to query.
  /// 
  /// \param[out] worldPosition
  ///   worldPosition = object->GetPosition() + globalPivot
  virtual void GetPosition( VisObject3D_cl* object, hkvVec3d& worldPosition ) const HKV_OVERRIDE;


  /// \brief
  ///   Query the basis vectors of the system given the position of an object
  /// 
  /// \param[in] worldPosition
  ///   Disregarded.
  /// 
  /// \param[out] basisVectors
  ///   An identity matrix
  virtual void GetBasisVectors( hkvVec3d const& worldPosition, hkvMat3& basisVectors ) const HKV_OVERRIDE;

  /// \brief
  ///   Return a coherent, planar position for an object of the form (planarX, planarY, altitudeAboveBaseSurface)
  /// 
  /// This returns, in particular, (visionPosition.x, visionPosition.y, visionPosition.z + worldPivot.z)
  /// 
  /// \param[in] worldPosition
  ///    Location of the object to convert in world space.
  ///
  /// \param[out] localFrame
  ///   The local frame described above
  virtual void GetInLocalFrame( hkvVec3d const& worldPosition, hkvVec3& localFrame ) const HKV_OVERRIDE;

  /// \brief
  ///   Return a 4x4 matrix for converting from a local frame to the world space.
  ///
  /// \param[in] worldPosition
  ///    Location of a reference object in world space.
  ///
  /// \param[out] invLocalFrame
  ///   The matrix.
  ///
  /// \sa VisDefaultCoordinateSystem::GetInLocalFrame
  virtual void GetLocalFrameInversionMatrix( hkvVec3d const& worldPosition, hkvMat4& invLocalFrame ) const HKV_OVERRIDE;

  /// \brief
  ///   Return a 4x4 matrix for converting from world space to local frame.
  ///
  /// \param[in] worldPosition
  ///    Location of a reference object in world space.
  ///
  /// \param[out] localFrame
  ///   The matrix.
  ///
  /// \sa VisDefaultCoordinateSystem::GetInLocalFrame
  virtual void GetLocalFrameMatrix( hkvVec3d const& worldPosition, hkvMat4& localFrame ) const HKV_OVERRIDE;

  /// \brief
  ///   Polynomial to fit the local falloff of the underlying surface
  /// 
  /// The vanilla implementation is a plane, so no need to worry about the coefficients.
  ///
  /// \param[in] observerPosition
  ///   Ignored.
  ///
  /// \param[in] viewingDistance
  ///   Ignored.
  /// 
  /// \param[out] coefficients
  ///   hkvVec4::ZeroVector()
  virtual void GetEllipticalPolynomialCoefficients( hkvVec3d const& observerPosition, double viewingDistance, hkvVec4& coefficients ) const HKV_OVERRIDE;

  /// \brief
  ///   Cosine between the viewer and the horizon
  /// 
  /// The vanilla implementation ignores any horizon and returns 90 degrees.
  ///
  /// \param[in] observerPosition
  ///   Ignored.
  /// 
  /// \return
  ///   cos(pi) 
  virtual float GetCosineHorizon( hkvVec3d const& observerPosition ) const HKV_OVERRIDE
  { 
    return 0; 
  }

  /// \brief
  ///   Sets a scene reference position
  ///
  /// The scene reference position is an absolute position in that all scene elements are considered to be relative to.
  /// The actual semantics of this varies depending on the underlying coordinate system used-- and may not be applicable at all
  /// to the underlying system.  For instance, a local flat earth simulation can set this coordinate in Geodetic coordinates as a Lat/Long/Alt reference
  /// The runtime does not do anything explicit with this value
  /// vForge sets this value to the reference position property in the Main Layer (as Lat/Long/Alt). It is exported along with the scene and set by the
  /// scene loader class.
  ///
  /// \param refPos
  ///   The scene reference position to be used.
  virtual void SetSceneReferencePosition(const hkvVec3d& refPos);

  /// \brief
  ///   Get the scene reference position if applicable for a coordinate system
  virtual void GetSceneReferencePosition(hkvVec3d& referencePosition) const;

private:

  hkvVec3d m_vReferencePosition;
};

/// \brief
///   Wrapper class for easy use of VisCoordinateSystem_cl
/// 
/// This class supports all the general queries of a coordinate but behaves more like an object.
/// It wraps either a reference location (which will be pivoted into world space) or a VisObject3D_cl.
class VCoordinateSystemUtility
{
public:

  VISION_APIFUNC VCoordinateSystemUtility( VisObject3D_cl* object = 0 );

  VISION_APIFUNC VCoordinateSystemUtility( hkvVec3 const& referencePosition );

  VISION_APIFUNC ~VCoordinateSystemUtility() {}

  /// \brief
  ///   Sets the object to wrap
  /// 
  /// This function is mutually exclusive with SetReferencePosition and will override any associated data from that.
  /// It can be more precise to use this routine when possible rather than object->GetPosition()
  ///
  /// \param[in] object
  ///   The object
  VISION_APIFUNC void SetObject( VisObject3D_cl* object );

  /// \brief
  ///   Create a coordinate from a single precision location
  /// 
  /// Automatically pivots the position and creates the associated data for querying the coordinate system.
  /// This method will override any data from SetObject.  It also has the possibility of being less precise than SetObject.
  ///
  /// \param[in] reference
  ///   The reference position, world space will be calculated as reference + currentGlobalPivot
  VISION_APIFUNC void SetReferencePosition( const hkvVec3& reference );

  /// \brief
  ///   Query the object's local frame
  /// 
  /// \param[out] v
  ///   The local frame coordinate
  ///
  /// \sa VisCoordinateSystem_cl::GetInLocalFrame
	VISION_APIFUNC void GetLocalFrame(hkvVec3& v) const;

  /// \brief
  ///   Query for a conversion matrix from world space to local frame for the wrapped object/location
  /// 
  /// \param[out] m 
  ///   The matrix
  ///
  /// \sa VisCoordinateSystem_cl::GetLocalFrameMatrix
  VISION_APIFUNC void GetLocalFrameMatrix(hkvMat4& m) const;

  /// \brief
  ///   Query for a conversion matrix from local frame to world space for the wrapped object/location
  /// 
  /// \param[out] m 
  ///   The matrix
  ///
  /// \sa VisCoordinateSystem_cl::GetLocalFrameInversionMatrix
  VISION_APIFUNC void GetLocalFrameInversionMatrix(hkvMat4& m) const;

  /// \brief
  ///   Query for the world's basis vectors at the object's location
  ///
  /// These tend to be used a lot and be particularly handy, so they are automatically
  /// computed and cached on construction of the wrapper class.
  ///
  /// \sa VisCoordinateSystem_cl::GetBasis
  VISION_APIFUNC const hkvMat3& GetBasis() const;

  /// \brief
  ///   Query for the object's world space position given the object/reference location
  VISION_APIFUNC const hkvVec3d& GetWorldPosition() const;

  /// \brief
  ///   Set the position from a local frame coordinate.
  ///
  /// This can be a dangerous function over long distances, but will allow
  /// you to shift objects along the worlds up/right/forward vectors in a straight forward
  /// way for short distances.  E.g., SetLocalFrame(GetLocalFrame() + hkvVec3( 0, 0, -1 )) would
  /// move the object down 1 world space unit regardless of the world's up vector.
  ///
  /// If there is no associated object, it will just update the approximated world space coordinate.
  /// Else, it will update the position of the object.
  ///
  /// \param[in] lf 
  ///   The local frame coordinate
  VISION_APIFUNC void SetLocalFrame( const hkvVec3& lf );

  /// \brief
  ///   Set a world space coordinate.
  ///
  /// If there is no associated object, it will just update the approximated world space coordinate.
  /// Else, it will update the position of the object.
  ///
  /// \param[in] ws 
  ///   The world space coordinate
  VISION_APIFUNC void SetWorldSpace( const hkvVec3d& ws );

  /// \brief
  ///   Utility function to calculate the tangent space frustum far corners given the current location of the object (and current context's camera).
  ///
  /// \param[in] pNode
  ///   The render node to query the frustum from
  ///
  /// \param[out] tl
  ///   The top left frustum corner, in world space, transformed to tangent space
  ///
  /// \param[out] bl
  ///   The bottom left frustum corner, in world space, transformed to tangent space
  ///
  /// \param[out] br
  ///   The bottom right frustum corner, in world space, transformed to tangent space
  ///
  /// \param[out] tr
  ///   The top right frustum corner, in world space, transformed to tangent space
  ///
  /// \param[out] rightDir
  ///   tr - tl, not normalized
  ///
  /// \param[out] downDir
  ///   bl - tl, not normalized
  ///
	VISION_APIFUNC void GetTangentFrustumFarCorners( IVRendererNode *pNode, hkvVec3* tl, hkvVec3* bl, hkvVec3* br, hkvVec3* tr, hkvVec3* rightDir = 0, hkvVec3* downDir = 0 );

  /// \brief
  ///   Utility function to calculate the local frame frustum far corners given the current location of the object (and current context's camera).
  ///
  /// \param[in] pNode
  ///   The render node to query the frustum from
  ///
  /// \param[out] tl
  ///   The top left frustum corner, in world space, transformed to local frame
  ///
  /// \param[out] bl
  ///   The bottom left frustum corner, in world space, transformed to local frame
  ///
  /// \param[out] br
  ///   The bottom right frustum corner, in world space, transformed to local frame
  ///
  /// \param[out] tr
  ///   The top right frustum corner, in world space, transformed to local frame
  ///
  /// \param[out] rightDir
  ///   tr - tl, not normalized
  ///
  /// \param[out] downDir
  ///   bl - tl, not normalized
  ///
  VISION_APIFUNC void GetLocalFrameFrustumFarCorners( IVRendererNode *pNode, hkvVec3* tl, hkvVec3* bl, hkvVec3* br, hkvVec3* tr, hkvVec3* rightDir = 0, hkvVec3* downDir = 0 );

  /// \brief
  ///   Query the elliptical polynomial given the location of an object
  ///
  /// \param[in] viewingDistance
  ///   The maximum distance the polynomial will be queried for
  ///
  /// \param[out] coefficients
  ///   Coefficients for the polynomial
  /// 
  /// \sa VisCoordinateSystem_cl::GetEllipticalPolynomialCoefficients
  VISION_APIFUNC void GetEllipticalPolynomial( double viewingDistance, hkvVec4& coefficients );

  /// \brief
  ///   Query cosine of the angle to the world's horizon for an object
  /// 
  /// \sa VisCoordinateSystem_cl::GetCosineHorizon
  VISION_APIFUNC float CosineHorizon() const;

private:

	hkvVec3d m_vWorldSpace;
	hkvMat3 m_Basis;
	VisObject3D_cl* m_pSrcObject;
};




/// \brief
///   Class for accessing functionality related to the game world.
class VisWorld_cl
{
public:

  VisWorld_cl();  
  ~VisWorld_cl();

  ///
  /// @name World State
  /// @{
  ///

  /// \brief
  ///   Returns whether the world has been initialized
  VISION_APIFUNC BOOL IsWorldInitialized() const;
  

  ///
  /// @}
  ///

  ///
  /// @name Fog
  /// @{
  ///

  /// \brief
  ///   Sets the fog parameters.
  ///
  /// \param fog
  ///   The new fog parameters.
  VISION_APIFUNC void SetFogParameters(const VFogParameters &fog);

  /// \brief
  ///   Returns the current fog parameters.
  VISION_APIFUNC const VFogParameters &GetFogParameters() const;

  /// \brief
  ///   Returns whether there is any kind of depth fog enabled.
  VISION_APIFUNC bool IsLinearDepthFogEnabled() const ;

  ///
  /// @}
  ///

  ///
  /// @name Sky
  /// @{
  ///

  /// \brief
  ///   Sets the global sky instance that is rendered in the scene
  VISION_APIFUNC void SetActiveSky(IVSky *pSky);

  /// \brief
  ///   Gets the global sky instance that has been set via SetActiveSky.
  inline IVSky *GetActiveSky() const
  {
    return m_spCurrentSky;
  }


  /// @name World Settings
  /// @{

  /// \brief
  ///   Specifies whether accurate geometry tests are used to assign dynamic scene elements to
  ///   visibility zones by affecting the way VisVisibilityZone_cl::GetDistanceFromGeometry works.
  /// 
  /// If set to true, the Vision engine uses per-triangle distance tests to compute the results of
  /// VisVisibilityZone_cl::GetDistanceFromGeometry. This is computationally very expensive and
  /// requires a significant amount of memory, but is required for correct visibility
  /// classification in some complex combined indoor/outdoor scenes.
  /// 
  /// By default, this is set to false.
  /// 
  /// \param bStatus
  ///   true to enable per-triangle distance tests.
  VISION_APIFUNC void SetUseGeometryForVisibilityClassification(bool bStatus) { m_bUseGeometryForVisibilityClassification = bStatus; }

  /// \brief
  ///   Returns the value previously set with SetUseGeometryForVisibilityClassification.
  VISION_APIFUNC bool GetUseGeometryForVisibilityClassification() const { return m_bUseGeometryForVisibilityClassification; }

  /// \brief
  ///   Sets a global unit scaling factor relative to engine's default (1 unit ~ 1 cm)
  /// 
  /// This value does not apply any scaling to meshes etc. Instead it is just a hint for some
  /// external hard-coded values, e.g. scaling for debug rendering.
  /// 
  /// The default is 1.0. The value is changed by vForge's global settings.
  ///
  /// \param fScaling
  ///   The new global unit scaling to use.
  VISION_APIFUNC void SetGlobalUnitScaling(float fScaling);

  /// \brief
  ///   Returns the value previously set with SetGlobalUnitScaling.
  VISION_APIFUNC float GetGlobalUnitScaling() const;
  ///
  /// @}
  ///

  /// @name Coordinate System
  /// @{

  /// \brief
  ///   Returns the coordinate system. See VisCoordinateSystem_cl
  VISION_APIFUNC VisCoordinateSystem_cl* GetCoordinateSystem();

  /// \brief
  ///   Returns the coordinate system that can be used for specific world transformations. See VisCoordinateSystem_cl
  VISION_APIFUNC void SetCoordinateSystem( VisCoordinateSystem_cl* coordinateSystem );

  /// @}

private:
  friend class Vision;
  friend class World_cl;
  friend class VisBaseEntity_cl;

  /// \brief
  ///   Called by Vision::InitWorld to initialize the world and prepare to load a new scene
  void InitWorld();
 
  /// \brief
  ///   Called by Vision::DeInitWorld to deinitialize the world 
  void DeInitWorld();

  VisCoordinateSystem_cl* m_pExternalCoordinateSystem;
  VisCoordinateSystem_cl* m_pDefaultCoordinateSystem;

  IVSkyPtr m_spCurrentSky;

  bool m_bUseGeometryForVisibilityClassification;

  bool m_bWorldInitialized;
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
