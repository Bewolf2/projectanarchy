/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvIProperties.hpp

#ifndef HKV_I_PROPERTIES_HPP_INCLUDED
#define HKV_I_PROPERTIES_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>

#include "hkvJsonStreamReadHandler.hpp"
#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>

#include <vector>

class hkStreamWriter;
class hkvJsonWriter;

class hkvVariantValue
{
public:
  enum Type
  {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_CONST_STRING,
  };

public:
  ASSETFRAMEWORK_IMPEXP hkvVariantValue();
  ASSETFRAMEWORK_IMPEXP hkvVariantValue(bool b);
  ASSETFRAMEWORK_IMPEXP hkvVariantValue(hkInt64 i64);
  ASSETFRAMEWORK_IMPEXP hkvVariantValue(double d);
  ASSETFRAMEWORK_IMPEXP hkvVariantValue(const char* str, bool copy = true);
  ASSETFRAMEWORK_IMPEXP hkvVariantValue(const hkvVariantValue& rhs);
  ASSETFRAMEWORK_IMPEXP ~hkvVariantValue();

public:
  ASSETFRAMEWORK_IMPEXP hkvVariantValue& operator=(const hkvVariantValue& rhs);
  ASSETFRAMEWORK_IMPEXP bool operator==(const hkvVariantValue& rhs) const;
  bool operator!=(const hkvVariantValue& rhs) const { return !(*this == rhs); }

public:
  Type getType() const { return m_type; }

  hkBool isBool() const { return m_type == TYPE_BOOL; }
  hkBool isDouble() const { return m_type == TYPE_DOUBLE; }
  hkBool isInt64() const { return m_type == TYPE_INT64; }
  hkBool isNumber() const { return isDouble() || isInt64(); }
  hkBool isNull() const { return m_type == TYPE_NULL; }
  hkBool isString() const { return m_type == TYPE_STRING; }

  hkBool getBool() const { return isBool() ? m_value.b : false; }
  double getDouble() const { return isDouble() ? m_value.d : (isInt64() ? (double)m_value.i64 : 0); }
  hkInt64 getInt64() const { return isInt64() ? m_value.i64 : (isDouble() ? (hkInt64)m_value.d : 0); }
  const char* getString() const { return (isString() && m_value.str != NULL) ? m_value.str : ""; }

  ASSETFRAMEWORK_IMPEXP void setBool(bool b);
  ASSETFRAMEWORK_IMPEXP void setDouble(double d);
  ASSETFRAMEWORK_IMPEXP void setInt64(hkInt64 i64);
  ASSETFRAMEWORK_IMPEXP void setNull();
  ASSETFRAMEWORK_IMPEXP void setString(const char* str, bool copy = true);

  ASSETFRAMEWORK_IMPEXP const char* toString(hkStringBuf& out_string) const;

private:
  void freeString();


private:
  Type m_type;
  union
  {
    bool b;
    hkInt64 i64;
    double d;
    const char* str;
  } m_value;
};


class hkvProperty
{
public:
  typedef std::vector<hkvVariantValue> VariantValueList;

public:
  enum Type
  {
    TYPE_INVALID = 0,
    TYPE_BOOL,
    TYPE_INT,
    TYPE_UINT,
    TYPE_INT64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_FILE,
    TYPE_TIMESTAMP,
    TYPE_ENUM,
    TYPE_CUSTOM,
    TYPE_ARRAY,

    TYPE_GROUP_START,
    TYPE_GROUP_END,

    TYPE_MAX = 0xffffffff
  };

  enum Purpose
  {
    PURPOSE_SERIALIZATION,
    PURPOSE_USER_INTERFACE,
    PURPOSE_USER_INTERFACE_READ_ONLY
  };

  enum Flags
  {
    FLAG_MULTIPLE_VALUES  = 1 << 0,
    FLAG_READ_ONLY        = 1 << 1,
    FLAG_INFORMATION_ONLY = 1 << 2, ///< Denotes a property whose value serves only informational purposes.
  };

  inline hkvProperty() : m_type(TYPE_INVALID), m_flags(0), m_description(NULL), m_icon(NULL)
  {
  }

  inline hkvProperty(const char* name, bool value, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(TYPE_BOOL), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setBool(value);
    m_defaultValue.setBool(false);
  }

  inline hkvProperty(const char* name, int value, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(TYPE_INT), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setInt64(value);
    m_defaultValue.setInt64(0);
  }

  inline hkvProperty(const char* name, unsigned int value, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(TYPE_UINT), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setInt64(value);
    m_defaultValue.setInt64(0);
  }

  inline hkvProperty(const char* name, hkInt64 value, Type type = TYPE_INT64, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(type), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setInt64(value);
    m_defaultValue.setInt64(0);
  }

  inline hkvProperty(const char* name, float value, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(TYPE_FLOAT), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setDouble(value);
    m_defaultValue.setDouble(0);
  }

  inline hkvProperty(const char* name, double value, hkUint32 flags = 0, const char* szDescription = NULL) : 
    m_name(name), m_type(TYPE_DOUBLE), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setDouble(value);
    m_defaultValue.setDouble(0);
  }

  inline hkvProperty(const char* name, const char* value, Type type = TYPE_STRING, hkUint32 flags = 0, const char* szDescription = NULL)
    : m_name(name), m_type(type), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setString(value);
    m_defaultValue.setString(NULL);
  }

  inline hkvProperty(const char* name, hkUint32 enumValue, const char* enumValues, hkUint32 flags = 0, const char* szDescription = NULL)
    : m_name(name), m_type(TYPE_ENUM), m_customType(enumValues), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setInt64(enumValue);
    m_defaultValue.setInt64(0);
  }

  inline hkvProperty(const char* name, const char* customType, const char* value, hkUint32 flags = 0, const char* szDescription = NULL)
    : m_name(name), m_type(TYPE_CUSTOM), m_customType(customType), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_value.setString(value);
    m_defaultValue.setString(NULL);
  }

  inline hkvProperty(const char* name, const VariantValueList& value, const char* customType = NULL, hkUint32 flags = 0, const char* szDescription = NULL)
    : m_name(name), m_type(TYPE_ARRAY), m_customType(customType), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    m_array = value;
  }

  inline hkvProperty(const char* name, const std::vector<hkStringPtr>& value, const char* customType = NULL, hkUint32 flags = 0, const char* szDescription = NULL)
    : m_name(name), m_type(TYPE_ARRAY), m_customType(customType), m_flags(flags), m_description(szDescription), m_icon(NULL)
  {
    size_t numValues = value.size();
    m_array.reserve(numValues);
    for (size_t valueIdx = 0; valueIdx < numValues; ++valueIdx)
    {
      m_array.push_back(hkvVariantValue(value[valueIdx]));
    }
  }

  inline hkvProperty(const hkvProperty& rhs)
  {
    *this = rhs;
  }

  inline ~hkvProperty() {}

  inline void operator=(const hkvProperty& rhs)
  {
    m_name = rhs.m_name;
    m_type = rhs.m_type;
    m_customType = rhs.m_customType;
    m_value = rhs.m_value;
    m_defaultValue = rhs.m_defaultValue;
    m_array = rhs.m_array;
    m_flags = rhs.m_flags;
    m_description = rhs.m_description;
    m_icon = rhs.m_icon;
  }

  ASSETFRAMEWORK_IMPEXP hkBool operator==(const hkvProperty& rhs) const;
  
  inline hkBool operator!=(const hkvProperty& rhs) const
  {
    return !(*this == rhs);
  }

  /// \brief
  ///   Returns true if type and name match.
  ASSETFRAMEWORK_IMPEXP bool hasMatchingSignature(const hkvProperty& rhs) const;

  /// \brief
  ///   Returns true if type value match.
  ASSETFRAMEWORK_IMPEXP bool hasMatchingValue(const hkvProperty& rhs) const;

  inline static hkvProperty groupStart(const char* groupName)
  {
    hkvProperty group;
    group.m_name = groupName;
    group.m_type = TYPE_GROUP_START;
    return group;
  }

  inline static hkvProperty groupEnd()
  {
    hkvProperty group;
    group.m_type = TYPE_GROUP_END;
    return group;
  }

  ASSETFRAMEWORK_IMPEXP bool isDefaultValue() const;

  inline const char* getName() const { return m_name; }
  inline Type getType() const { return m_type; }
  inline const char* getCustomType() const { return m_customType; }
  inline const char* getDescription() const { return m_description; }
  inline const char* getIcon() const { return m_icon; }

  inline hkBool getBool() const { VASSERT(m_type == TYPE_BOOL); return m_value.getBool(); }

  ASSETFRAMEWORK_IMPEXP hkInt32 getInt() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getUint() const;
  ASSETFRAMEWORK_IMPEXP hkInt64 getInt64() const;
  ASSETFRAMEWORK_IMPEXP hkFileSystem::TimeStamp getTimeStamp() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getEnumValue() const;
  
  ASSETFRAMEWORK_IMPEXP float getFloat() const;
  ASSETFRAMEWORK_IMPEXP double getDouble() const;

  ASSETFRAMEWORK_IMPEXP const char* getString() const;
  ASSETFRAMEWORK_IMPEXP const char* getFile() const;
  ASSETFRAMEWORK_IMPEXP const char* getCustom() const;
  ASSETFRAMEWORK_IMPEXP const VariantValueList& getArray() const;
  ASSETFRAMEWORK_IMPEXP void getStringArray(bool includeEmptyEntries, std::vector<hkStringPtr>& out_strings) const;
  ASSETFRAMEWORK_IMPEXP const char* getEnumMapping() const;

  ASSETFRAMEWORK_IMPEXP void copyValue(const hkvProperty& rhs);

  inline hkUint32 getFlags() const { return m_flags; }
  inline void setFlags(hkUint32 flags) { m_flags = flags; } 

  ASSETFRAMEWORK_IMPEXP void toString(const char* nameValueSeparator, hkStringBuf& out_string) const;

  ASSETFRAMEWORK_IMPEXP void setCustomType(const char* szCustomType) { m_customType = szCustomType; }
  ASSETFRAMEWORK_IMPEXP void setDescription(const char* szConstantString) { m_description = szConstantString; }
  ASSETFRAMEWORK_IMPEXP void setIcon(const char* szConstantString) { m_icon = szConstantString; }

  ASSETFRAMEWORK_IMPEXP void setDefaultValue(bool value);
  ASSETFRAMEWORK_IMPEXP void setDefaultValue(hkInt64 value);
  ASSETFRAMEWORK_IMPEXP void setDefaultValue(double value);
  ASSETFRAMEWORK_IMPEXP void setDefaultValue(const char* value, bool copy = true);

private:
  hkStringPtr m_name;
  Type m_type;
  hkStringPtr m_customType;
  hkvVariantValue m_value;
  hkvVariantValue m_defaultValue;
  VariantValueList m_array;
  const char* m_description; // a string embedded in the application that can be used as a tooltip or description
  const char* m_icon; // a string embedded in the application that defines which icon to use

  hkUint32 m_flags;
};


typedef std::vector<hkvProperty> hkvPropertyList;


class hkvIProperties
{
public:
  ASSETFRAMEWORK_IMPEXP hkvIProperties();
  ASSETFRAMEWORK_IMPEXP virtual ~hkvIProperties();

public:
  ASSETFRAMEWORK_IMPEXP virtual const char* getTypeName() const = 0;
  ASSETFRAMEWORK_IMPEXP virtual void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const = 0;
  ASSETFRAMEWORK_IMPEXP virtual void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) = 0;

  ASSETFRAMEWORK_IMPEXP void writeProperties(const hkvJsonWriter* writer) const;

  template <class T>
  void parseValue(const char* name, T val, const hkArray<hkStringPtr>& path, hkUint32 stackIndex)
  {
    setProperty(hkvProperty(name, val), path, stackIndex, hkvProperty::PURPOSE_SERIALIZATION);
  }

protected:
  hkCriticalSection& getPropertyProtect() const
  {
    return *s_protect;
  }

private:
  void writeArray(const hkvJsonWriter* writer, const hkvProperty& prop) const;
  void writeBool(const hkvJsonWriter* writer, const hkvProperty& prop) const;
  void writeInt64(const hkvJsonWriter* writer, const hkvProperty& prop) const;
  void writeDouble(const hkvJsonWriter* writer, const hkvProperty& prop) const;
  void writeString(const hkvJsonWriter* writer, const hkvProperty& prop) const;
  void writePropName(const hkvJsonWriter* writer, const hkStringPtr& propName) const;

private:
  static hkCriticalSection* s_protect;
  static hkUint32 s_instanceCount;
};


class hkvPropertyReader : public hkvJsonStreamReadHandler
{
public:
  ASSETFRAMEWORK_IMPEXP hkvPropertyReader() {}

  ASSETFRAMEWORK_IMPEXP void resetPropertyReader();

  ///   hkvJsonStreamReadHandler implementation (don't adapt to Havok types!)
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseNull();
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseBool(bool bValue);
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseInt(long long iValue);
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseDouble(double dValue);
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseString(const unsigned char* pszValue, size_t len);
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseMapKey(const unsigned char* pszValue, size_t len);
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseStartMap();
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseEndMap();
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseStartArray();
  ASSETFRAMEWORK_IMPEXP VOVERRIDE hkvJsonStreamReadHandler::ParsingResult ParseEndArray();

protected:
  hkInt32 m_loaderLevel;

  hkStringPtr m_loaderKey;
  hkvIProperties* m_loaderObject;

  hkInt32 m_loaderStackIndex;
  hkArray<hkStringPtr> m_loaderGroups;

  hkUint32 m_loaderArrayLevel;
  hkvProperty::VariantValueList m_loaderArray;
};

class hkvPropertiesHelper
{
private:
  hkvPropertiesHelper();
  hkvPropertiesHelper(const hkvPropertiesHelper&);
  hkvPropertiesHelper& operator=(const hkvPropertiesHelper&);

public:
  ASSETFRAMEWORK_IMPEXP static hkUint32 computeHash(const hkvPropertyList& properties, 
    const char* typeName = NULL, hkUint32 startValue = 0);
  ASSETFRAMEWORK_IMPEXP static void makePropertiesString(const hkvPropertyList& properties,
    const char* entrySeparator, const char* nameValueSeparator, bool flatten,
    hkStringBuf& out_string);

  ASSETFRAMEWORK_IMPEXP static void setProperties(const hkvPropertyList& properties, hkvIProperties& target);
  ASSETFRAMEWORK_IMPEXP static void copyProperties(const hkvIProperties& source, hkvIProperties& target);
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
