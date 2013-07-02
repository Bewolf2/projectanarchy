
#ifndef VLUA_APIDOC

%nodefaultctor VisSurface_cl;
%nodefaultdtor VisSurface_cl;

class VisSurface_cl //hide complexity  : public VBaseObject, public VUserDataObj, public VisSurfaceTextures_cl 
{
public:

  %rename(GetDiffuseTexture) GetBaseTextureObject() const;
  inline VTextureObject *GetBaseTextureObject() const;
  %rename(GetNormalTexture) GetNormalMapTextureObject() const;
  inline VTextureObject *GetNormalMapTextureObject() const;
  %rename(GetSpecularTexture) GetSpecularMapTextureObject() const;
  inline VTextureObject *GetSpecularMapTextureObject() const;
  
  %rename(SetDiffuseTexture) SetBaseTexture(VTextureObject *pTex);
  void SetBaseTexture(VTextureObject *pTex);
  
  inline int GetAuxiliaryTextureCount() const;
  VTextureObject *GetAuxiliaryTexture(int iIndex) const ;
  void SetAuxiliaryTexture(int iIndex, VTextureObject *pTexture) const;

  inline int GetIndex() const;
  inline const char* GetName() const;
  inline void SetName(const char *szNewString);

  bool IsTranslucent();
  bool IsLightMapped() const;
  bool IsLightmappingSupported() const;
  bool IsDoubleSided() const;
  bool IsFullbright() const;

  VColorRef GetAmbientColor() const;
  void SetAmbientColor(const VColorRef& color);
  
  float GetSpecularMultiplier() const;
  void SetSpecularMultiplier (float f);

  float GetSpecularExponent() const;
  void SetSpecularExponent (float f);
  
  float GetAlphaTestThreshold() const;
  void SetAlphaTestThreshold(float f);

  %extend{
    void SetAutoShaderMode(bool bUseAutoMode)
    {
      self->SetShaderMode(bUseAutoMode?VisSurface_cl::VSM_Auto:VisSurface_cl::VSM_Manual);
    }
    
    bool HasAutoShaderMode()
    {
      return self->GetShaderMode() == VisSurface_cl::VSM_Auto;
    }
    
    bool operator == (const VisSurface_cl& other)
    {
      return self==&other;
    }
  }
};

//Implement GetAllSurfaces native because it returns 3 parameters
%native(VisSurface_cl_GetTextureLocations) int VisSurface_cl_GetTextureLocations(lua_State *L);
%{
  SWIGINTERN int VisSurface_cl_GetTextureLocations(lua_State *L)
  {
    IS_MEMBER_OF(VisSurface_cl) //this will move this function to the method table of the specified class
      
    SWIG_CONVERT_POINTER(L, 1, VisSurface_cl, pSurface)
    
    lua_pushstring(L, pSurface->GetBaseTexture());
    lua_pushstring(L, pSurface->GetNormalMapTexture());
    lua_pushstring(L, pSurface->GetSpecularMapTexture());
    
    return 3; //return 3 texture locations
  }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VisSurface_cl, 128, "[%s,%s]", self->GetName(), self->GetBaseTexture() )
VSWIG_CREATE_TOSTRING(VisSurface_cl, "VisSurface_cl: %d [%s,%s]", self->GetIndex(), self->GetName(), self->GetBaseTexture() )

#else

/// \brief Lua wrapper for surfaces.
class VisSurface_cl {
public:

  /// @name Common Surface Functions
  /// @{
  
  /// \brief Returns the surface name.
  /// \return The name of the surface.
  /// \par Example
  ///   \code
  ///   function OnAfterSceneLoaded()
  ///     -- we assume that there is a model mesh assigned to this entity
  ///     local surfaces = self.GetMesh():GetAllSurfaces()
  ///     for i=1,#surfaces do
  ///       Debug:PrintLine("Surface '" .. surfaces:GetName() .. "' : " .. surfaces:GetIndex());
  ///     end
  ///   end
  ///   \endcode
  string GetName();
  
  /// \brief Assign a new name to this surface.
  /// \param name The new name of the surface.
  void SetName(string name);

  /// \brief Get the 0-based index of this entry in the owner's list.
  /// \return The index of the surface.
  number GetIndex();
    
  /// @}
  /// @name Textures
  /// @{
  
  /// \brief Get the diffuse (base) texture of this surface.
  /// \return The texture as VTextureObject.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       -- we assume that there is a model having a surface named "diffuse_mat"
  ///       local surface = self:GetMesh():GetSurfaceByName("diffuse_mat")
  ///       local texture = surface:GetDiffuseTexture()
  ///       if texture ~= nil then
  ///         Debug:PrinLine("diffuse_mat is using the texture "..texture:GetName())
  ///       else
  ///         Debug:PrinLine("diffuse_mat has no assigned texture")
  ///       end
  ///     end
  ///   \endcode
  VTextureObject GetDiffuseTexture();
  
  /// \brief Get the normal map texture of this surface.
  /// \return The texture as VTextureObject.
  VTextureObject GetNormalTexture();
  
  /// \brief Get the specular map texture of this surface.
  /// \return The texture as VTextureObject.
  VTextureObject GetSpecularTexture();
  
  /// \brief Set the diffuse (base) texture of this surface.
  /// \param texture The new diffuse texture.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local texture = Game:CreateTexture("Textures/surface_diff.dds")
  ///       Vision.Assert(texture~=nil, "Unable to load texture")
  ///       local surface = self:GetMesh():GetSurfaceByName("diff")
  ///       Vision.Assert(surface~=nil, "Surface diff not found")
  ///
  ///       surface:SetDiffuseTexture(texture)
  ///     end
  ///   \endcode
  void SetDiffuseTexture(VTextureObject texture);
  
  /// \brief Returns the number of additional auxiliary textures associated with this material.
  /// \return The number of auxiliary textures.
  number GetAuxiliaryTextureCount();
  
  /// \brief Returns an additional auxiliary texture.
  /// \param index The index of the texture, which must be in valid range [0..GetAuxiliaryTextureCount()-1].
  /// \return The auxiliary texture object.
  /// \see GetAuxiliaryTextureCount
  VTextureObject GetAuxiliaryTexture(number index);
  
  /// \brief Set a specific auxiliary texture.
  /// \param index The index of the texture to replace, which must be in valid range [0..GetAuxiliaryTextureCount()-1].
  /// \param texture The new auxiliary texture object.
  /// \see GetAuxiliaryTextureCount
  void SetAuxiliaryTexture(number index, VTextureObject texture);
  
  /// \brief Returns the locations and file names of the diffuse, normal and specular texture.
  /// \return Three strings containing the diffuse, normal and specular texture locations (which could be nil)
  /// \par Example
  ///   \code
  ///     local diffuse, normal, specular = surface:GetTextureLocations()
  ///   \endcode
  multiple GetTextureLocations();
  
  /// @}
  /// @name Settings
  /// @{

  /// \brief Returns the ambient color of the surface. 
  /// \return The ambient color as VColorRef.
  VColorRef GetAmbientColor();
  
  /// \brief Set the ambient color of the surface. 
  /// \param color The new ambient color as VColorRef.
  void SetAmbientColor(VColorRef color);
  
  /// \brief Returns the specular multiplier of the surface.
  /// \return The sepcular multiplier.
  number GetSpecularMultiplier();
  
  /// \brief Set the specular multiplier of the surface.
  /// \param multiplier The new specular multiplier.
  void SetSpecularMultiplier(number multiplier);

  /// \brief Returns the specular exponent of the surface.
  /// \return The sepcular exponent, a return value of 0 means that the surface does not have specularity.
  number GetSpecularExponent();
  
  /// \brief Set the specular exponent of the surface.
  /// \param exponent The new exponent multiplier.
  void SetSpecularExponent (number exponent); 
  
  /// \brief Returns the alpha threshold of the surface.
  /// \return The alpha threshold.
  number GetAlphaTestThreshold();
  
  /// \brief Set the alpha threshold of the surface.
  /// \param alpha The new alpha threshold.
  void SetAlphaTestThreshold(number alpha);

  /// \brief Returns whether the surface is translucent.
  /// \details Check if the surface uses a blend mode other than VIS_TRANSP_NONE and VIS_TRANSP_ALPHATEST.
  /// \return true if the surface is translucent, otherwise false.
  boolean IsTranslucent();
  
  /// \brief Checks whether the surface is light-mapped.
  /// \return true if light-mapped, otherwise false.
  boolean IsLightMapped();
  
  /// \brief Determines whether lightmapping is (in theory) supported for this material.
  /// For instance it requires that the underlying mesh has UV1.
  /// \return true if lightmapping is sufforted, otherwise false.
  boolean IsLightmappingSupported();
  
  /// \brief Returns whether the surface is double-sided. 
  /// No backface culling will be performed for primitives using a double-sided surface.
  /// \return true if double-sided, otherwise false.
  boolean IsDoubleSided();
  
  /// \brief Returns whether the surface is fullbright. 
  /// Fullbright surfaces are not affected by any lighting.
  /// \return true if the surface is fullbright, otherwise false.
  boolean IsFullbright();
  
  /// @}
  /// @name Shader Mode
  /// @{
  
  /// \brief Switch between automatic and manual shader selection mode.
  /// \param useAutoMode Set to true for auto mode (default) or false for manual mode.
  void SetAutoShaderMode(boolean useAutoMode)
  
  /// \brief Check the shader mode.
  /// \return true = auto shader selection mode; false manual shader selection mode.
  boolean HasAutoShaderMode();
    
  /// @}
  /// @name Operators
  /// @{
  
  /// \brief Compare with another surface (pointer comparison)
  /// \param other The other surface to test.
  /// \return true if the surfaces are the same, otherwise false.
  boolean __eq(VisSurface_cl other);
  
  /// @}
};

#endif