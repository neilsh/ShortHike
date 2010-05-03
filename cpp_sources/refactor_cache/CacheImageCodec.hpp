//================================================================================
//
// ShortHike
//
//--------------------------------------------------------------------------------

#ifndef CACHE_CACHE_IMAGE_CODEC
#define CACHE_CACHE_IMAGE_CODEC

class CacheImageCodec : public ImageCodec
{
public:
  CacheImageCodec();
  
  virtual DataStreamPtr code(MemoryDataStreamPtr &input, CodecDataPtr &pData) const;
  virtual void codeToFile(MemoryDataStreamPtr &input, const String& outFileName, CodecDataPtr &pData) const;
  virtual Codec::DecodeResult decode(DataStreamPtr &input) const;
  virtual String getType() const;
};

#endif 
  


