//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "CacheImageCodec.hpp"


//--------------------------------------------------------------------------------

CacheImageCodec::CacheImageCodec()
{
}

//--------------------------------------------------------------------------------
// Encoding not implemented, this is a dummy class

DataStreamPtr
CacheImageCodec::code(MemoryDataStreamPtr &, CodecDataPtr &) const
{
  CHECK_FALSE(string("CacheImageCodec:: Encoding not supported"));
  return SharedPtr<DataStream>(NULL);
}

void
CacheImageCodec::codeToFile(MemoryDataStreamPtr &, const String& , CodecDataPtr &) const
{
}

//--------------------------------------------------------------------------------
// Decode the DataStream

Codec::DecodeResult
CacheImageCodec::decode(DataStreamPtr &input) const
{
  ResourceGroupManager* rManager = ResourceGroupManager::getSingletonPtr();
  string rootName = input->getName().substr(0, input->getName().size() - 4);

  // Check if TGA files exists, then we use that one instead.
  string tgaName = rootName + ".tga";
  if(rManager->resourceExists(RESOURCE_GROUP_MAIN, tgaName)) {
    Codec* tgaCodec = ImageCodec::getCodec("tga");
    CHECK("Valid codec for TGA Images", tgaCodec != NULL);
    DataStreamPtr dataStream = rManager->openResource(tgaName, RESOURCE_GROUP_MAIN);
    return tgaCodec->decode(dataStream);
  }
  // Check if JPG files exists, then we use that one instead.
  string jpgName = rootName + ".jpg";
  if(rManager->resourceExists(RESOURCE_GROUP_MAIN, jpgName)) {
    Codec* jpgCodec = ImageCodec::getCodec("jpg");
    CHECK("Valid codec for JPG Images", jpgCodec != NULL);
    DataStreamPtr dataStream = rManager->openResource(jpgName, RESOURCE_GROUP_MAIN);
    return jpgCodec->decode(dataStream);
  }

  // Check if shc file exists. Then use it and decompress
  string shcName = rootName + ".shc";
  if(rManager->resourceExists(RESOURCE_GROUP_MAIN, shcName)) {
    Codec* shcCodec = ImageCodec::getCodec("shc");
    CHECK("Valid codec for ShortHike Compressed Images", shcCodec != NULL);
    DataStreamPtr dataStream = rManager->openResource(tgaName, RESOURCE_GROUP_MAIN);
    return shcCodec->decode(dataStream);
  }

  CHECK_FALSE(string("Unable to load image: ") + input->getName());
  return Codec::DecodeResult();
}


//--------------------------------------------------------------------------------
// Use the dummy type "shi" for ShortHike Image. Compressed files are "shc".

String
CacheImageCodec::getType() const
{
  return "shi";
}

