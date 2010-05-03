//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef CACHE_COMPRESSED_IMAGE_CODEC
#define CACHE_COMPRESSED_IMAGE_CODEC

class CompressedImageCodec : public ImageCodec
{
public:
  CompressedImageCodec();

  virtual DataStreamPtr code(MemoryDataStreamPtr &input, CodecDataPtr &pData) const;
  virtual void codeToFile(MemoryDataStreamPtr &input, const String& outFileName, CodecDataPtr &pData) const;
  virtual Codec::DecodeResult decode(DataStreamPtr &input) const;
  virtual String getType() const;
};

#endif
