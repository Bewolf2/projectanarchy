/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_IMAGE_FILE_HPP_INCLUDED
#define HKV_IMAGE_FILE_HPP_INCLUDED

class hkvEnumDefinition;

enum hkvImageFileFormat
{
  HKV_IMAGE_FILE_FORMAT_INVALID,
  HKV_IMAGE_FILE_FORMAT_BMP,
  HKV_IMAGE_FILE_FORMAT_DDS,
  HKV_IMAGE_FILE_FORMAT_JPG,
  HKV_IMAGE_FILE_FORMAT_PNG,
  HKV_IMAGE_FILE_FORMAT_TGA,
  HKV_IMAGE_FILE_FORMAT_COUNT
};

extern const char* hkvImageFileFormatNames[HKV_IMAGE_FILE_FORMAT_COUNT];
const hkvEnumDefinition& getImageFileFormatDefinition();

class hkStreamReader;

class hkvMipImage
{
public:
  hkvMipImage(hkUint32 width, hkUint32 height);
private:
  hkvMipImage(const hkvMipImage&);
  hkvMipImage& operator=(const hkvMipImage&);
public:
  ~hkvMipImage();

public:
  hkUint32 getWidth() const { return m_width; }
  hkUint32 getHeight() const { return m_height; }
  hkUint8* getData() const { return m_data; }

private:
  hkUint32 m_width;
  hkUint32 m_height;
  hkUint8* m_data;
};

class hkvImageFile : public hkReferencedObject
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefPtr<hkvImageFile> RefPtr;
  typedef hkRefPtr<const hkvImageFile> RefCPtr;
  typedef hkRefNew<hkvImageFile> RefNew;

protected:
  hkvImageFile();
private:
  hkvImageFile(const hkvImageFile&);
  hkvImageFile& operator=(const hkvImageFile&);
public:
  virtual ~hkvImageFile();

public:
  static RefPtr open(const char* fileName, bool readData);
  static RefPtr open(hkStreamReader& reader, hkvImageFileFormat format, bool readData);

  static hkvImageFileFormat guessFormatFromFileName(const char* fileName);

public:
  virtual hkvImageFileFormat getFileFormat() const = 0;

  // If false, the file could not be opened or the header format
  // was not valid.
  bool isHeaderValid() const { return m_headerValid; }
  
  virtual hkUint32 getWidth() const = 0;
  virtual hkUint32 getHeight() const = 0;
  // True, if the source file had a valid alpha channel. If false, the alpha
  // values of the image data array are set to 255.
  virtual bool hasAlpha() const = 0;

  bool isDataValid() const { return !m_mipImages.isEmpty(); }
  // Data is always B8G8R8A8, tightly packed in an array of size [width x height]
  hkUint8* getData() const { return !m_mipImages.isEmpty() ? m_mipImages[0]->getData() : NULL; }

  hkUint32 getMipImageCount() const { return m_mipImages.getSize(); }
  hkvMipImage* getMipImage(hkUint32 index) const { return (index < (hkUint32)m_mipImages.getSize()) ? m_mipImages[index] : NULL; }

protected:
  void clearData();

protected:
  hkBool m_headerValid;
  hkArray<hkvMipImage*> m_mipImages;
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
