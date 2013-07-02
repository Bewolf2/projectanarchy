
#ifndef VLUA_APIDOC

%nodefaultctor VDynamicMesh;
%nodefaultdtor VDynamicMesh;

class VDynamicMesh //as far as possible we hide this relation : public VBaseMesh
{
public:

  inline int GetSubmeshCount() const;
  inline int GetSurfaceCount() const;

  %extend{
    VSWIG_RETURN_BY_VALUE_CONST(hkvAlignedBBox, GetBoundingBox);
  
    bool ReloadShaderAssignment(const char * szFilename)
    {
      return self->ReloadShaderAssignmentFile(szFilename)==TRUE;
    }
    
    bool SetTechnique(VisSurface_cl *pSurface, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, true);
    }

    bool SetTechnique(int iSurfaceIndex, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      if(iSurfaceIndex>=self->GetSurfaceCount() || iSurfaceIndex<0) return false;
      
      VisSurface_cl *pSurface = self->GetSurface(iSurfaceIndex);
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, true);
    }

    bool SetTechnique(const char * szSurfaceName, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      VisSurface_cl *pSurface = self->GetSurfaceByName(szSurfaceName);
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, true);
    }

    bool AddTechnique(VisSurface_cl *pSurface, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, false);
    }

    bool AddTechnique(int iSurfaceIndex, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      if(iSurfaceIndex>=self->GetSurfaceCount() || iSurfaceIndex<0) return false;
      
      VisSurface_cl *pSurface = self->GetSurface(iSurfaceIndex);
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, false);
    }

    bool AddTechnique(const char * szSurfaceName, const char * szShaderLib, const char * szTechnique, const char * szParamString = "")
    {
      VisSurface_cl *pSurface = self->GetSurfaceByName(szSurfaceName);
      return VScriptRenderer_wrapper::SetTechniqueForSurface(NULL, self, pSurface, szShaderLib, szTechnique, szParamString, false);
    }

    bool SetEffect(VisSurface_cl *pSurface, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, true);
    }

    bool SetEffect(int iSurfaceIndex, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      if(iSurfaceIndex>=self->GetSurfaceCount() || iSurfaceIndex<0) return false;
      
      VisSurface_cl *pSurface = self->GetSurface(iSurfaceIndex);
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, true);
    }

    bool SetEffect(const char * szSurfaceName, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      VisSurface_cl *pSurface = self->GetSurfaceByName(szSurfaceName);
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, true);
    }

    bool AddEffect(VisSurface_cl *pSurface, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, false);
    }

    bool AddEffect(int iSurfaceIndex, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      if(iSurfaceIndex>=self->GetSurfaceCount() || iSurfaceIndex<0) return false;
      
      VisSurface_cl *pSurface = self->GetSurface(iSurfaceIndex);
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, false);
    }

    bool AddEffect(const char * szSurfaceName, const char * szShaderLib, const char * szEffect, const char * szParamString = "")
    {
      VisSurface_cl *pSurface = self->GetSurfaceByName(szSurfaceName);
      return VScriptRenderer_wrapper::SetEffectForSurface(NULL, self, pSurface, szShaderLib, szEffect, szParamString, false);
    }
  }
  
  %rename(GetSurface) GetSurfaceByName(const char* szSurfaceName) const;
  inline VisSurface_cl* GetSurfaceByName(const char* szSurfaceName) const;
  inline VisSurface_cl* GetSurface(int iIndex) const;

  inline int GetSurfaceIndex(VisSurface_cl* pSurface);
  
  %rename(GetName) GetFilename() const;
  const char * GetFilename() const;
  
  %extend{
    bool operator == (const VDynamicMesh& other)
    {
      return self==&other;
    }
  }

};

//Implement GetAllSurfaces native because it returns an array of surfaces
%native(VDynamicMesh_GetAllSurfaces) int VDynamicMesh_GetAllSurfaces(lua_State *L);
%{
  SWIGINTERN int VDynamicMesh_GetAllSurfaces(lua_State *L)
  {
    IS_MEMBER_OF(VDynamicMesh) //this will move this function to the method table of the specified class
      
    SWIG_CONVERT_POINTER(L, 1, VDynamicMesh, pMesh)

    lua_newtable(L);                                        //stack: ..., table
    
    int iCount = pMesh->GetSurfaceCount();
    VisSurface_cl** ppSurfaces = pMesh->GetSurfaceArray();

    for(int i=0;i<iCount;++i)
    {
      VSWIG_PUSH_PROXY(L, VisSurface_cl, ppSurfaces[i]);    //stack: ..., table, surface
      lua_rawseti(L, -2, i+1);                         //stack: ..., table
    } 
    
    return 1; //return the table as stack param
  }
%}

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VDynamicMesh, 128, "[%s]", self->GetFilename())
VSWIG_CREATE_TOSTRING(VDynamicMesh, "VDynamicMesh: '%s' (%d surfaces, %d sub meshes)", self->GetFilename(), self->GetSurfaceCount(), self->GetSubmeshCount())


#else

/// \brief Dynamic mesh wrapper class.
/// \see VisBaseEntity_cl::GetMesh
/// \see VisGame_cl::CreateDynamicMesh
/// \see VisGame_cl::GetDynamicMesh
class VDynamicMesh {
public:

  /// @name Common Dynamic Mesh Functions
  /// @{

  /// \brief Returns the name of the mesh (= the file name)
  /// \return The mesh name.
  string GetName();
      
  /// \brief Get the number of submeshes.
  /// \return The number of submeshes (1 = there is only a single mesh).
  number GetSubmeshCount();
  
  /// \brief Returns the bounding box for this mesh.
  /// \return The bounding box.
  hkvAlignedBBox GetBoundingBox();

  /// @}
  /// @name Surfaces
  /// @{
  
  /// \brief Get the number of surfaces.
  /// \return The number of defined surfaces for this mesh.  
  number GetSurfaceCount();

  /// \brief Get a specific surfaces.
  /// \param identifier A \b number, indicating the index of the surface or a \b string specifying the name of the surface.
  /// \return The requested surface or nil if not present.
  VisSurface_cl GetSurface(mixed identifier);

  /// \brief Returns the index of the surface or -1 for alien surfaces.
  /// \note: Can be used to determine whether the passed surface is used by this mesh.
  /// \param surface The surface to look up.
  /// \return The index of the surface or -1 if the specified surface does not belong to this mesh.
  number GetSurfaceIndex(VisSurface_cl surface);
 
  /// \brief Returns all surfaces of the mesh.
  /// \return A table containing all surfaces from index 1 to LEN.
  /// \par Example
  ///   \code
  ///   local surfaces = self.GetMesh():GetAllSurfaces()
  ///   for i=1,#surfaces do
  ///     Debug:PrintLine("Surface '" .. surfaces:GetName() .. "' : " .. surfaces:GetIndex());
  ///   end
  ///   \endcode
  table GetAllSurfaces();
  
  /// @}
  /// @name Shader Assignment
  /// @{

  /// \brief Reloads the associated materials.xml file and reassigns the shaders in it.
  /// \param filename Path and file name of the XML file.
  /// \return Returns true if the assignment was successful, otherwise false.
  boolean ReloadShaderAssignment(string filename);
    
  /// \brief Apply a technique to a specified surface of the entity
  /// \details Will remove the current assignment from the specified surface and add your specified technique.
  /// \note If wou would like to apply the shader to a specific entity use VisBaseEntity_cl::SetTechnique
  /// \param surface The surface to assign the technique specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \param shaderLib The shader lib, where the techinque is defined (the file).
  /// \param technique The name of the technique to be used.
  /// \param paramString (\b optional) The parameter string for the used shader.
  /// \return true if the assignment was successful, otherwise false (invalid surface, technique not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success = self:GetMesh():SetTechnique("SurfaceTexture", "Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::SetTechnique
  boolean SetTechnique(mixed surface, string shaderLib, string technique, string paramString = "");
  
  /// \brief Add a technique to a specified surface of the entity
  /// \details Will add the technique in addition to the current assignment.
  /// \note If wou would like to add the shader to a specific entity use VisBaseEntity_cl::AddTechnique
  /// \param surface The surface to add the technique specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \param shaderLib The shader lib, where the techinque is defined (the file).
  /// \param technique The name of the technique to be used.
  /// \param paramString (\b optional) The parameter string for the used shader.
  /// \return true if the assignment was successful, otherwise false (invalid surface, technique not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success = self:GetMesh():AddTechnique(0, "Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::AddTechnique
  boolean AddTechnique(mixed surface, string shaderLib, string technique, string paramString = "");
  
  /// \brief Apply an effect to a specified surface of the entity (the default technique will be applied)
  /// \details Will remove the current assignment from the specified surface and add your effect.
  /// \note If wou would like to apply the shader to a specific entity use VisBaseEntity_cl::SetEffect
  /// \param surface The surface to assign the effect specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \param shaderLib The shader lib, where the effect is defined (the file).
  /// \param effect The name of the effect to be used.
  /// \param paramString (\b optional) The parameter string for the used shader.
  /// \return true if the assignment was successful, otherwise false (invalid surface, effect not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success = self:GetMesh():SetEffect("SurfaceTexture", "Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::SetEffect
  boolean SetEffect(mixed surface, string shaderLib, string effect, string paramString = "");

  /// \brief Add an effect to a specified surface of the entity (the default technique will be applied)
  /// \details Will add the effect in addition to the current assignment.
  /// \note If wou would like to add the shader to a specific entity use VisBaseEntity_cl::AddEffect
  /// \param surface The surface to add the effect specified by one of the following types:
  ///   - \b VisSurface_cl instance (of the mesh)
  ///   - \b number indicating the index of the surface
  ///   - \b string containing the name of the surface.
  /// \param shaderLib The shader lib, where the effect is defined (the file).
  /// \param effect The name of the effect to be used.
  /// \param paramString (\b optional) The parameter string for the used shader.
  /// \return true if the assignment was successful, otherwise false (invalid surface, effect not found).
  /// \par Example
  ///   \code
  ///     ...
  ///     local success = self:GetMesh():AddEffect(surfaceInstance, "Scripting/LuaDemo", "Color", "Color=0,1,0,0")
  ///     ...
  ///   \endcode
  /// \see vForge Sampe "Scripting/LuaShader.scene".
  /// \see VisBaseEntity_cl::AddEffect
  boolean AddEffect(mixed surface, string shaderLib, string effect, string paramString = "");
  
  /// @}
};

#endif
