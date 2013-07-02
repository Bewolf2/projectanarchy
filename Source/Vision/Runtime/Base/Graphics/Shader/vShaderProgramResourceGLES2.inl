/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */


#if defined(_VR_GLES2) && !defined _VISION_DOC

  class VLoadedGLES2Program : public VRefCounter
  {
  public:
    struct VRegisterToUniformMappingEntry
    {
      int iRegister;
      int iUniform;
      unsigned int uiUniformType;
      int iSize;
    };

    VLoadedGLES2Program();
    ~VLoadedGLES2Program();

    bool LoadAndCompile(bool bFragmentShader, void* pByteCode, unsigned int uiByteCodeLength, const char* pFileName);

    inline unsigned int GetShaderHandle() const
    {
      return m_uiShaderHandle;
    }

    inline void AddMapping(const VRegisterToUniformMappingEntry& entry)
    {
      m_regToUniMappings[m_uiNumMappings++] = entry;
    }

    inline const DynArray_cl<VRegisterToUniformMappingEntry>& GetMappings() const
    {
      return m_regToUniMappings;
    }

    inline unsigned int GetNumMappings() const 
    {
      return m_uiNumMappings;
    }

    inline void EnsureMappingSize(unsigned int size)
    {
      m_regToUniMappings.EnsureSize(size);
    }
    
    inline unsigned int GetProgramHandle() const
    {
      return m_uiShaderHandle;
    }
    
    inline bool IsCompiled() const
    {
      return m_uiShaderHandle != 0;
    }

  private:  
    unsigned int m_uiShaderHandle;

    DynArray_cl<VRegisterToUniformMappingEntry> m_regToUniMappings;
    unsigned int m_uiNumMappings;
  };

  struct ConstantToRegisterMappingInByteCode
  {
    char szConstantName[128];
    int iRegisterIndex;
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
