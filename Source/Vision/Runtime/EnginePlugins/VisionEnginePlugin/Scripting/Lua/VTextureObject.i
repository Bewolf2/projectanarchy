
#ifndef VLUA_APIDOC

%nodefaultctor VTextureObject;
%nodefaultdtor VTextureObject;

class VTextureObject // hide inheritance : public VManagedResource, public VShaderResourceView, public VUnorderedAccessView
{
public:

  %extend{
    const char * GetType()
    {
      switch(self->GetTextureType())
      {
        case VTextureLoader::Texture2D: return "Texture2D";
        case VTextureLoader::Texture3D: return "Texture3D";
        case VTextureLoader::Cubemap: return "Cubemap";
        case VTextureLoader::Heightmap16bpp: return "Heightmap16bpp";
        case VTextureLoader::Texture2DArray: return "Texture2DArray";
        case VTextureLoader::CubemapArray: return "CubemapArray";
        default: return "Unknown";
      }
    }
    
    bool operator == (const VTextureObject& other)
    {
      return self==&other;
    }
  }
  
  virtual bool IsRenderable() const;
  
  %rename(GetWidth) GetTextureWidth() const;
  int GetTextureWidth() const;
  
  %rename(GetHeight) GetTextureHeight() const;
  int GetTextureHeight() const;

  %rename(GetColorDepth) GetTextureColorDepth() const;
  inline int GetTextureColorDepth() const;

  %rename(GetColorDepthUncompressed) GetTextureColorDepthUncompressed() const;
  inline int GetTextureColorDepthUncompressed() const;

  inline bool IsAnimated() const;
  inline bool IsCompressed() const;

  inline int GetMipMapLevelCount() const;
  inline bool HasMipMaps() const;
  bool HasAlphaChannel() const;

  inline bool IsUsed() const;
  inline bool IsTexturePowerOfTwo() const;
  
  %rename(GetName) GetFilename() const;
  const char * GetFilename() const;
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VTextureObject, 256, "[%s]", self->GetFilename())
VSWIG_CREATE_TOSTRING(VTextureObject, "VTextureObject '%s' %dx%d", self->GetFilename(), self->GetTextureWidth(), self->GetTextureHeight())

#else

/// \brief Wrapper class for VTextureObject.
class VTextureObject {
public:

  /// @name Common Texture Functions
  /// @{
 
  /// \brief Get the type of the texture as string.
  /// The texture type will be one of the following:
  ///  - "Texture2D"
  ///  - "Texture3D"
  ///  - "Cubemap"
  ///  - "Heightmap16bpp"
  ///  - "Texture2DArray"
  ///  - "CubemapArray"
  ///  - "Unknown"
  /// \return The type of the texture as string.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local tex = Game:CreateTexture("Textures/surface.dds")
  ///       Debug:PrinLine(tex:GetType())
  ///     end
  ///   \endcode
  string GetType();
  
  /// \brief Get the name of the texture (which is the file name).
  /// \return the name of the texture.
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
  string GetName();

  /// \brief Get the width of the texture.
  /// \return The width in pixel.  
  number GetWidth();
  
  /// \brief Get the height of the texture.
  /// \return The height in pixel.
  number GetHeight();

  /// \brief Returns the internal color depth of the texture.
  /// This does not necessarily correspond to the color depth defined in a texture file,
  /// since e.g. RGB textures are typically padded to 32bpp for the GPU. 
  /// \return The internal color depth.
  /// \see GetColorDepthUncompressed
  number GetColorDepth();

  /// \brief Gets the uncompressed bit depth of the texture. This will return 24 or 32 for DXT formats. 
  /// \return The uncompressed color depth.
  /// \see GetColorDepth
  number GetColorDepthUncompressed();

  /// \brief Indicates whether this texture has mipmaps or not. 
  /// \return tre if mipmaps are present, otherwise false.
  boolean HasMipMaps();
  
  /// \brief Returns the number of mipmap levels used by this texture (returns 1 for no mip-maps). 
  /// \return The number of present mipmap levels (>= 1).
  number GetMipMapLevelCount();

  /// \brief Checks whether the texture has an alpha channel. 
  /// \return ture if there is a alpha channel, otherwise false.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       local tex = Game:CreateTexture("Textures/c_map.dds")
  ///       if tex:HasAlphaChannel() then
  ///         Debug:PrinLine(tex:GetName().." has an alpha channel.")
  ///       end
  ///     end
  ///   \endcode
  boolean HasAlphaChannel();
 
  /// \brief Checks whether texture's dimensions are a power of two. 
  /// \return true if power of two, otherwise false.
  boolean IsTexturePowerOfTwo();
  
  /// \brief Indicates whether the texture is renderable. 
  /// \return true = renderable; false not renderable.
  boolean IsRenderable();
  
  /// \brief Indicates whether texture is an animated texture. 
  /// \return true if the texture is animated otherwise false.
  boolean IsAnimated();
  
  /// \brief Indicates whether the texture format is a compressed format (DXT etc.). 
  /// \return true if the texture format is a compressed, otherwise false.
  boolean IsCompressed();

  /// \brief Indicates whether the texture is currently referenced. 
  /// \return true if the reference count is larger than 1, otherwise false. 
  boolean IsUsed();
  
  /// @}
  /// @name Operators
  /// @{
  
  /// \brief Comparison operator.
  /// \param other The texture object to compare.
  /// \return true if it is the same texture object, otherwise false.
  boolean __eq(VTextureObject other);
  
  /// @}
};

#endif