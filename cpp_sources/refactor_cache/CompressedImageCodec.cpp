//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "CompressedImageCodec.hpp"


//--------------------------------------------------------------------------------

CompressedImageCodec::CompressedImageCodec()
{
}

//--------------------------------------------------------------------------------
// Encoding not implemented, this is a dummy class

DataStreamPtr
CompressedImageCodec::code(MemoryDataStreamPtr &, CodecDataPtr &) const
{
  return SharedPtr<DataStream>(NULL);
}

void
CompressedImageCodec::codeToFile(MemoryDataStreamPtr &, const String& , CodecDataPtr &) const
{
}

//--------------------------------------------------------------------------------
// Decode the DataStream

Codec::DecodeResult
CompressedImageCodec::decode(DataStreamPtr &) const
{
  // FIXME: Decompress using ECW library
  return Codec::DecodeResult();
}


//--------------------------------------------------------------------------------
// Use the dummy type "shi" for ShortHike Image. Compressed files are "shc".

String
CompressedImageCodec::getType() const
{
  return "shc";
}

