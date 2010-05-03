//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

class Surface
{
}


// // --------------------------------------------------------------------------------

// Surface::Surface(int width, int height)
//   : mData(NULL), mWidth(width), mHeight(height)
// {
//   if(width > 0 && height > 0)
//     mData = new agg::int8u[width * height * 4];
// }

// Surface::~Surface()
// {
//   if(NULL != mData)
//     delete[] mData;
// }

// // --------------------------------------------------------------------------------

// bool
// Surface::load(const ChunkIterator& begin, const ChunkIterator& end, ChunkIterator& curr)
// {
//   curr = begin;
//   if(curr == end || curr.getChunkType() != "IRGB") return false;

//   curr.read(mWidth);
//   curr.read(mHeight);
//   delete[] mData;
//   mData = new agg::int8u[mWidth * mHeight * 4];
//   curr.readCompressed(mData, mWidth * mHeight * 4);
  
//   flush();
//   ++curr;
  
//   return true;
// }


// // --------------------------------------------------------------------------------

// bool
// Surface::save(ChunkStream* stream)
// {
//   stream->openChunk("IRGB");
//   stream->write(mWidth);
//   stream->write(mHeight);
//   stream->writeCompressedDeflate(mData, mWidth * mHeight * 4);
//   stream->closeChunk();
//   return true;
// }


// // --------------------------------------------------------------------------------

// void
// Surface::loadFromTGA(string fileName)
// {
//   if(loadImageTGA(fileName, reinterpret_cast<agg::int8u**>(&mData), &mWidth, &mHeight))
//     flush();
// }


// void
// Surface::loadFromPNG(string fileName)
// {
//   if(loadImagePNG(fileName, reinterpret_cast<agg::int8u**>(&mData), &mWidth, &mHeight))
//     flush();
// }


// // --------------------------------------------------------------------------------

// void
// Surface::loadCheckerBoard(int squareSize)
// {
//   if(NULL != mData)
//     delete[] mData;
//   mWidth = 128;
//   mHeight = 128;
//   mData = new agg::int8u[mWidth * mHeight * 4];
//   agg::int8u* destPtr = static_cast<agg::int8u*>(getData());
//   for(int x = 0; x < mWidth; ++x) {
//     for(int y = 0; y < mHeight; ++y) {
//       int value = ((x / squareSize) + (y / squareSize)) % 2;
//       if(0 == value) {
//         *destPtr++ = 0x3f;
//         *destPtr++ = 0x3f;
//         *destPtr++ = 0x3f;
//         *destPtr++ = 0xff;
//       }
//       else {
//         *destPtr++ = 0xcf;
//         *destPtr++ = 0xcf;
//         *destPtr++ = 0xcf;
//         *destPtr++ = 0xff;
//       }
//     }
//   }
//   flush();
// }


// // --------------------------------------------------------------------------------

// void
// Surface::flush()
// {
//   int i;
//   i = 1;
// }
