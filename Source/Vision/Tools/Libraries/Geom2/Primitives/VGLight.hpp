/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Light class (for putting lights in a scene, etc.)
class VGEOM2_IMPEXP_CLASS VGLight : public VGNode, public VGInstantiable
{
public:
  /// \brief
  ///   Light types
  enum LightType
  {
    VGLT_POINT,
    VGLT_SPOT,
    VGLT_DIRECTIONAL
  };

  /// \brief
  ///   Shadow casting flags
  enum ShadowFlags
  {
    VGSF_NONE   = 0,
    VGSF_STATIC = V_BIT(0)
  };


public:
  VGLight();
  VGLight(const VGLight& other);
  virtual ~VGLight();



  /// \brief
  ///   Gets the light's type
  ///
  /// \return
  ///   Currently set light type
  inline LightType        GetType() const throw()                                   { return m_lightType; }

  /// \brief
  ///   Sets the light's type
  ///
  /// \param t
  ///   Light type to set
  inline void             SetType(LightType t) throw()                              { m_lightType = t; }



  /// \brief
  ///   Gets the shadow casting properties
  ///
  /// \return
  ///   Currently set shadow casting properties
  inline ShadowFlags      GetShadowCastingFlags() const throw()                     { return m_shadowFlags; }

  /// \brief
  ///   Sets the shadow casting properties
  ///
  /// \param s
  ///   Shadow casting properties to set
  inline void             SetShadowCastingFlags(ShadowFlags s) throw()              { m_shadowFlags = s; }



  /// \brief
  ///   Gets static geometry influence mask
  ///
  /// \return
  ///   Currently static geometry influence mask
  inline int              GetStaticGeometryInfluenceMask() const throw()            { return m_staticGeometryInfluenceMask; }// @@@ test

  /// \brief
  ///   Sets static geometry influence mask
  ///
  /// \param m
  ///   Static geometry influence mask
  inline void             SetStaticGeometryInfluenceMask(int m) throw()             { m_staticGeometryInfluenceMask = m; }// @@@ test

  /// \brief
  ///   Gets dynamic geometry influence mask
  ///
  /// \return
  ///   Currently dynamic geometry influence mask
  inline int              GetDynamicGeometryInfluenceMask() const throw()           { return m_dynamicGeometryInfluenceMask; }// @@@ test

  /// \brief
  ///   Sets dynamic geometry influence mask
  ///
  /// \param m
  ///   Dynamic geometry influence mask
  inline void             SetDynamicGeometryInfluenceMask(int m) throw()            { m_dynamicGeometryInfluenceMask = m; }// @@@ test



  /// \brief
  ///   Gets the light radius in world units
  ///
  /// \return
  ///   Current light radius
  inline float            GetRadius() const throw()                                 { return m_radius; }

  /// \brief
  ///   Sets the light radius of influence in world units. Ignored for directional lights
  ///
  /// \param r
  ///   Light radius
  inline void             SetRadius(float r) throw()                                { m_radius = r; }



  /// \brief
  ///   Gets the light multiplier
  ///
  /// \return
  ///   Currently set light multiplier
  inline float            GetMultiplier() const throw()                             { return m_multiplier; }

  /// \brief
  ///   Sets the light multiplier
  ///
  /// \param m
  ///   Light multiplier to set
  inline void             SetMultiplier(float m) throw()                            { m_multiplier = m; }



  /// \brief
  ///   Gets the light's color
  ///
  /// \return
  ///   Currently set light color
  inline const hkvVec4&  GetColor() const throw()                                  { return m_lightColor; }

  /// \brief
  ///   Sets the light's color
  ///
  /// \param c
  ///   Light color to set
  inline void             SetColor(const hkvVec4& c)                               { m_lightColor = c; }



  /// \brief
  ///   Gets the light fade start value
  ///
  /// \return
  ///   Currently set light fade start value
  inline float            GetFadeStart() const throw()                              { return m_fade[0]; }

  /// \brief
  ///   Sets the light fade start value
  ///
  /// \param f
  ///   Light fade start value to set
  inline void             SetFadeStart(float f) throw()                             { m_fade[0] = f; }

  /// \brief
  ///   Gets the light fade end value
  ///
  /// \return
  ///   Currently set light fade end value
  inline float            GetFadeEnd() const throw()                                { return m_fade[1]; }

  /// \brief
  ///   Sets the light fade end value
  ///
  /// \param f
  ///   Light fade end value to set
  inline void             SetFadeEnd(float f) throw()                               { m_fade[1] = f; }

  

  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@ (unit unspecified, backends use radians by convention)
  inline float            GetSpotAngle() const throw()                              { return m_spotAngle; }

  /// \brief
  ///   Sets the @@@
  ///
  /// \param f
  ///    @@@ (unit unspecified, backends use radians by convention)
  inline void             SetSpotAngle(float f) throw()                             { m_spotAngle = f; }



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline bool             GetIsDynamicLight() const throw()                         { return m_isDynamic; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param b
  ///   @@@
  inline void             SetIsDynamicLight(bool b) throw()                         { m_isDynamic = b; }//@@@ test



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline bool             GetEnableCorona() const throw()                           { return m_enableCorona; }//@@@ test

  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline const VString&   GetCoronaTexture() const throw()                          { return m_coronaTexture; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param ec
  ///   @@@
  ///
  /// \param ct
  ///   @@@
  inline void             SetCoronaParams(bool ec, const VString& ct)               { m_enableCorona = ec; m_coronaTexture = ct; }//@@@ test



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline bool             GetEnableLensflare() const throw()                        { return m_enableLensflare; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param b
  ///   @@@
  inline void             SetEnableLensflare(bool b) throw()                        { m_enableLensflare = b; }//@@@ test



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline const VString&   GetProjectedTexture() const throw()                       { return m_projectedTexture; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param t
  ///   @@@
  inline void             SetProjectedTexture(const VString& t)                     { m_projectedTexture = t; }//@@@ test



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline const VString&   GetAnimBitmap() const throw()                             { return m_animBitmap; }//@@@ test

  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline float            GetAnimTime() const throw()                               { return m_animTime; }//@@@ test

  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline float            GetAnimPhase() const throw()                              { return m_animPhase; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param at
  ///   @@@
  ///
  /// \param ap
  ///   @@@
  ///
  /// \param ab
  ///   @@@
  inline void             SetAnimParams(float at, float ap, const VString& ab)      { m_animTime = at; m_animPhase = ap; m_animBitmap = ab; }//@@@ test



  /// \brief
  ///   Gets the @@@
  ///
  /// \return
  ///   Currently set @@@
  inline const VString&   GetLightKey() const throw()                               { return m_lightKey; }//@@@ test

  /// \brief
  ///   Sets the @@@
  ///
  /// \param k
  ///   @@@
  inline void             SetLightKey(const VString& k)                             { m_lightKey = k; }//@@@ test

  /// \brief
  ///   Gets the decay rate
  ///
  /// \return
  ///   Currently set decay rate
  inline const short&     GetDecayRate() const throw()                               { return m_decayRate; }//@@@ test

  /// \brief
  ///   Sets the decay rate
  ///
  /// \param k
  ///   decay rate to set (0 = no decay, 1 = linear decay, 2 = quadratic decay, ...)
  inline void             SetDecayRate(const short& k)                             { m_decayRate = k; }//@@@ test



  /// \brief
  ///   Copies essential data to passed-in instance, e.g. initial name and transformation, etc..
  ///
  /// \param inst
  ///   Instance to copy data to
  virtual void            CopyEssentialsToInstance(VGInstance& inst) const          { inst.SetName(GetName()); inst.SetTransformation(GetTransformation()); }

  /// \brief
  ///   Copies essential data from passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data from
  virtual void            CopyEssentialsFromInstance(const VGInstance& inst)        { SetName(inst.GetName()); SetTransformation(inst.GetTransformation()); }



  /// \brief
  ///   Gets some brief information about the type of this instantiable
  ///
  /// \return
  ///   String with name of instantiable type
  virtual const char*     GetTypeString() const throw()                             { return "Light"; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Light to assign
  ///
  /// \return
  ///   Reference to this light
  VGLight&                operator=(const VGLight& rhs);



private:
  LightType   m_lightType;    // Light type (default is VGLT_POINT).
  ShadowFlags m_shadowFlags;  // Shadow casting flags (default is VGSF_STATIC).
  float       m_radius;       // Light radius (default is 3000.f).
  float       m_multiplier;   // Light multiplier (default is 1.f).
  hkvVec4    m_lightColor;   // Light color (default is white (1.f, 1.f, 1.f)).
  float       m_fade[2];      // Light fade start and end (default is 0.f for both - no fade).

  // @@@ test - NEW FEATURES
  int         m_staticGeometryInfluenceMask, m_dynamicGeometryInfluenceMask;
  float       m_spotAngle;
  bool        m_isDynamic;
  bool        m_enableCorona;
  VString     m_coronaTexture;
  bool        m_enableLensflare;
  VString     m_projectedTexture;
  VString     m_animBitmap;
  float       m_animTime, m_animPhase;
  VString     m_lightKey;
  short       m_decayRate;
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
