/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Shader assignment class
///
/// Describes and setups a shader assignment (usually referenced by a material) for usage and processing inside vGeom2.
/// A shader assignment consists of either a template name or a shader library filename, a shader effect name inside this lib
/// and optionally a list of parameter name/value pairs.
class VGEOM2_IMPEXP_CLASS VGShaderAssignment : public VGNameable
{
public:
  VGShaderAssignment();
  virtual ~VGShaderAssignment();



  /// \brief
  ///   Gets the shader lib's name
  ///
  /// \return
  ///   Shader lib name
  inline const VString& GetShaderLibName() const throw() { return m_shaderLibName; }

  /// \brief
  ///   Sets the shader lib's name
  ///
  /// \param name
  ///   Shader lib name
  inline void SetShaderLibName(const VString& name) { m_shaderLibName = name; }


  /// \brief
  ///   Gets the shader lib's name
  inline const VString& GetTemplateName() const throw() { return m_shaderTemplateName; }

  /// \brief
  ///   Sets the shader lib's name
  inline void SetTemplateName(const VString& name) { m_shaderTemplateName = name; }

  /// \brief
  ///   Gets the shader assignment's properties
  ///
  /// \return
  ///   Shader assignment property dictionary
  inline const VDictionary<VString>& GetProperties() const throw() { return m_properties; }

  /// \brief
  ///   Sets a shader assignment's property key/value pair
  ///
  /// \param key
  ///   Shader assignment propertie's key name
  ///
  /// \param value
  ///   Shader assignment propertie's value
  inline void SetProperty(const VString& key, const VString& value)  { m_properties.Set(key, value); }



  /// \brief
  ///   Compares two shader assignments for equality
  ///
  /// \param rhs
  ///   Operand on right hand side
  ///
  /// \return
  ///   True if shader assignments are equal, otherwise false - note: order or properties are crucial
  bool operator==(const VGShaderAssignment& rhs) const;

  /// \brief
  ///   Compares two shader assignments for inequality
  ///
  /// \param rhs
  ///   Operand on right hand side
  ///
  /// \return
  ///   False if shader assignments are equal, otherwise true - see operator== for details
  inline bool operator!=(const VGShaderAssignment& rhs) const { return !operator==(rhs); }



private:
  // Storage for all different types
  VString               m_shaderLibName;
  VString               m_shaderTemplateName;
  VDictionary<VString>  m_properties;
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
