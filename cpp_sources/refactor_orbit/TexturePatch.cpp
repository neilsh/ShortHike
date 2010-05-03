//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "TexturePatch.hpp"

#pragma warning(push, 3)
#pragma warning(disable: 4706)
#include "OgreNoMemoryMacros.h"
#include "NCSFile.h"
#include "OgreMemoryMacros.h"
#pragma warning(pop)

const PixelFormat TEXTURE_FORMAT = PF_R8G8B8A8;
const int TEXTURE_FILTER_SIZE = 2;
const int TEXTURE_SIZE_1 = 512;
const int TEXTURE_SIZE_2 = 256;
const int TEXTURE_SIZE_3 = 128;
const int TEXTURE_SIZE_4 = 64;
const int TEXTURE_SIZE_RAW = TEXTURE_SIZE_1 * TEXTURE_FILTER_SIZE;

const float TEXTURE_DISTANCE_1 = 0.35f;
const float TEXTURE_DISTANCE_2 = 0.60f;
const float TEXTURE_DISTANCE_3 = 0.80f;

const Real LOADING_PIXELS_PER_FRAME = 1024 * 32;

const int BYTES_IN_RAW_TEXTURE = 3;

//--------------------------------------------------------------------------------
// Map datastreams to the NCS file functions

const string TEXTURE_RESOURCE_GROUP = RESOURCE_GROUP_MAIN;

NCSError
openNCS(char* fileName, void** clientData)
{
  DataStreamPtr** streamPointer = reinterpret_cast<DataStreamPtr**>(clientData);
  *streamPointer = new DataStreamPtr(ResourceGroupManager::getSingleton().openResource(string(fileName), TEXTURE_RESOURCE_GROUP));
  return NCS_SUCCESS;
}

NCSError
closeNCS(void* clientData)
{
  DataStreamPtr streamPointer = *(static_cast<DataStreamPtr*>(clientData));
  streamPointer->close();
  return NCS_SUCCESS;
}

NCSError
readNCS(void* clientData, void* buffer, UINT32 length)
{
  DataStreamPtr streamPointer = *(static_cast<DataStreamPtr*>(clientData));
  streamPointer->read(buffer, length);
  return NCS_SUCCESS;
}

NCSError
seekNCS(void* clientData, UINT64 offset)
{
  DataStreamPtr streamPointer = *(static_cast<DataStreamPtr*>(clientData));
  // FIXME: OGRE uses 32 bit quantities
  streamPointer->seek(static_cast<size_t>(offset));
  return NCS_SUCCESS;
}

NCSError
tellNCS(void* clientData, UINT64* offset)
{
  DataStreamPtr streamPointer = *(static_cast<DataStreamPtr*>(clientData));
  *offset = static_cast<UINT64>(streamPointer->tell());
  return NCS_SUCCESS;
}

void
bindNCSIOtoOgre()
{
  CHECK("Check that binding IO functions works",
        NCS_SUCCESS ==
        NCSecwSetIOCallbacks(&openNCS, &closeNCS, &readNCS, &seekNCS, &tellNCS));
}




//--------------------------------------------------------------------------------
// Utility function

void
fillRawBuffer(unsigned char* imageData, int textureSize) 
{
  for(int line = 0; line < textureSize; line++) {
    for(int pixel = 0; pixel < textureSize; pixel++){
      if((line % 2 == 0 && pixel % 2 == 0) || (line % 2 == 1 && pixel % 2 == 1)) {
        *imageData++ = 0xff;
        *imageData++ = 0xbf;
        *imageData++ = 0x7f;
        *imageData++ = 0x5f;
      }
      else {
        *imageData++ = 0xff;
        *imageData++ = 0x8f;
        *imageData++ = 0x3f;
        *imageData++ = 0x2f;
      }  
    }
  }      
}

//--------------------------------------------------------------------------------
// This is a texture patch capable of loading huge image files from ERW/JPEG2000
// file and splice them out as patches of smaller textures. This loading process
// is incremental and suitable to be called each frame for loading a bit more.
// The texture patcher filters the data received from the ERW compressor. It also
// supports batching to match mesh creation.

TexturePatch::TexturePatch(string fileName, string prefix, Real _overDraw, int _gridSize, int _imageBands)
  : gridSize(_gridSize), imageBands(_imageBands), overDraw(_overDraw)
{
  bindNCSIOtoOgre();

  imageFile = new CNCSFile();
  loadFactor = 1;
  
  char* fileNameC = new char[1000];
  strncpy(fileNameC, fileName.c_str(), 1000);
  imageFile->Open(fileNameC, false);
  delete[] fileNameC;
  
  const NCSFileViewFileInfoEx *fileInfo = imageFile->GetFileInfo();
  imageWidth = fileInfo->nSizeX;
  imageHeight = fileInfo->nSizeY;
  rawTextureBuffer = new uchar[TEXTURE_SIZE_RAW * TEXTURE_SIZE_RAW * BYTES_IN_RAW_TEXTURE];

  float maxFocusDistance = gridSize * 0.5;
  Vector2 midPoint(gridSize * 0.5f, gridSize * 0.5f);
  float edgeBias = 1 / gridSize * 0.5;
  for(int ypos = 0; ypos < gridSize; ++ypos) {
    for(int xpos = 0; xpos < gridSize; ++xpos) {
      Image image;
      float gridDistance = (midPoint - Vector2(xpos + 0.5f, ypos + 0.5f)).length() - edgeBias;
      int textureSize;
      if(gridDistance < (TEXTURE_DISTANCE_1 * maxFocusDistance))
        textureSize = TEXTURE_SIZE_1;
      else if(gridDistance < (TEXTURE_DISTANCE_2 * maxFocusDistance))
        textureSize = TEXTURE_SIZE_2;
      else if(gridDistance < (TEXTURE_DISTANCE_3 * maxFocusDistance))
        textureSize = TEXTURE_SIZE_3;
      else
        textureSize = TEXTURE_SIZE_4;
      image.loadDynamicImage(new uchar[textureSize * textureSize * 4], textureSize, textureSize, 1, TEXTURE_FORMAT, true);

      // Fill the image with test data to clearly show dimension
      fillRawBuffer(image.getData(), textureSize);
      
      ostringstream textureName;
      textureName << prefix << "_" << fileName << "_" << xpos << "_" << ypos;
      TexturePtr texture = TextureManager::getSingleton().createManual(textureName.str(), RESOURCE_GROUP_MAIN,
                                                                       TEX_TYPE_2D, textureSize, textureSize, 0,
                                                                       TEXTURE_FORMAT, TU_DYNAMIC_WRITE_ONLY);

      texture->loadImage(image);
      
      textures.push_back(texture);
      images.push_back(image);
    }
  }
}

TexturePatch::~TexturePatch()
{
  for(vector<TexturePtr>::const_iterator textureI = textures.begin(); textureI != textures.end(); ++textureI) {
    TexturePtr texture = *textureI;
    TextureManager::getSingleton().unload(texture->getName());
    TextureManager::getSingleton().remove(texture->getName());
  }
  textures.clear();
  
  imageFile->Close();
  //  delete imageFile;
  delete[] rawTextureBuffer;
}


//--------------------------------------------------------------------------------

void
TexturePatch::startLoad(float _left, float _right, float _top, float _bottom)
{
  left = static_cast<int>(_left * imageWidth);
  right = static_cast<int>(_right * imageWidth);
  top = static_cast<int>(_top * imageHeight);
  bottom = static_cast<int>(_bottom * imageHeight);

  patchX = 0;
  patchY = 0;
  enterLoadingState();
}


TexturePatch::State
TexturePatch::loadIncremental()
{
  if(state == LOADING) {
    processLoading();
  }
  else if(state == FILTERING) {
    processFiltering();
  }
  else if(state == ACTIVATING) {
    processActivating();
  }
  else if(state == PATCH_READY) {
    patchX++;
    if(patchX >= gridSize) {
      patchY++;
      patchX = 0;
    }

    if(patchY >= gridSize) {
      state = ALL_READY;
    }
    else {
      enterLoadingState();
    }    
  }
  return state;
}


void
TexturePatch::loadAll()
{
  while(loadIncremental() != ALL_READY);
}


//--------------------------------------------------------------------------------
//

TexturePtr
TexturePatch::getTexture(int xposition, int yposition)
{
  return textures[xposition + yposition * gridSize];
}

Image&
TexturePatch::getImage(int xposition, int yposition)
{
  return images[xposition + yposition * gridSize];
}



//--------------------------------------------------------------------------------
// Loading the image into the read buffer

void
TexturePatch::enterLoadingState()
{
  INT32 bandList[3];  bandList[0] = 0;  bandList[1] = 1;  bandList[2] = 2;

  float horizontalGrid = (right - left) / gridSize;
  float horizontalOverDraw = horizontalGrid * overDraw * 0.5f;
  float verticalGrid = (bottom - top) / gridSize;
  float verticalOverDraw = verticalGrid * overDraw * 0.5f;

  int viewLeft = left + patchX * horizontalGrid - horizontalOverDraw;
  int viewRight = left + (patchX + 1) * horizontalGrid + horizontalOverDraw;
  int viewTop = top + patchY * verticalGrid - verticalOverDraw;
  int viewBottom = top + (patchY + 1) * verticalGrid + verticalOverDraw;

  rawTextureSize = min(viewRight - viewLeft, viewBottom - viewTop);
  rawTextureSize = min(static_cast<int>(getImage(patchX, patchY).getWidth()) * TEXTURE_FILTER_SIZE, rawTextureSize);

  if(viewLeft >= imageWidth) {
    viewLeft -= imageWidth;
    viewRight -= imageWidth;
  }

  // This limits overDraw at the poles. The PlanetMesh class has a corresponding limit for the
  // mesh geometry
  if(viewTop < 0) viewTop = 0;
  if(viewBottom >= imageHeight) viewBottom = imageHeight - 1;


  //----------------------------------------
  // Handle two part loading in case of dataset seam

  if(viewRight >= imageWidth) {
    isFirstHalf = true;
    firstHalfWidth = rawTextureSize * (imageWidth - viewLeft) / (viewRight - viewLeft) ;
    secondHalfRight = viewRight - imageWidth;
    viewRight = imageWidth - 1;
    secondHalfTop = viewTop;
    secondHalfBottom = viewBottom;
  }
  else {
    isFirstHalf = false;
    firstHalfWidth = rawTextureSize;
  }
  


  NCSError errorCode = imageFile->SetView(imageBands, bandList, firstHalfWidth, rawTextureSize, viewLeft, viewTop, viewRight, viewBottom);
  if(errorCode != NCS_SUCCESS) {
    logEngineError(string("ECW SetView Error: ") + imageFile->FormatErrorText(errorCode));
  }
  
  loadingLine = 0;
  loadingLinesPerFrame = static_cast<int>(loadFactor * LOADING_PIXELS_PER_FRAME / rawTextureSize);
  loadingDataPointer = rawTextureBuffer;

  state = LOADING;
}


void
TexturePatch::processLoading()
{
  int lineCount = 0;
  while(loadingLine < rawTextureSize && lineCount < loadingLinesPerFrame) {
    imageFile->ReadLineBGR(loadingDataPointer);
    loadingDataPointer += rawTextureSize * BYTES_IN_RAW_TEXTURE;
    loadingLine++;
    lineCount++;
  }

  if(loadingLine >= rawTextureSize) {
    if(isFirstHalf) {
      isFirstHalf = false;
      loadingLine = 0;
      loadingDataPointer = rawTextureBuffer + firstHalfWidth * BYTES_IN_RAW_TEXTURE;

      INT32 bandList[3];  bandList[0] = 0;  bandList[1] = 1;  bandList[2] = 2;
      NCSError errorCode = imageFile->SetView(imageBands, bandList, rawTextureSize - firstHalfWidth, rawTextureSize,
                                              0, secondHalfTop, secondHalfRight, secondHalfBottom);
      if(errorCode != NCS_SUCCESS) {
        logEngineError(string("ECW SetView Error: ") + imageFile->FormatErrorText(errorCode));
      }
    }
    else {
      enterFilteringState();
    }
  }
  

}

//--------------------------------------------------------------------------------
// Processing the image into the 

void
TexturePatch::enterFilteringState()
{
  state = FILTERING;
}

void
TexturePatch::processFiltering()
{
  unsigned char* pixelData;
  pixelData = getImage(patchX, patchY).getData();
  
  Image::scale(PixelBox(Box(0, 0, rawTextureSize, rawTextureSize), PF_R8G8B8, rawTextureBuffer),
               getImage(patchX, patchY).getPixelBox());
  enterActivatingState();
}

//--------------------------------------------------------------------------------
// Activating state

void
TexturePatch::enterActivatingState()
{
  state = ACTIVATING;
}

void
TexturePatch::processActivating()
{
  getTexture(patchX, patchY)->loadImage(getImage(patchX, patchY));
  enterPatchReadyState();
}

//--------------------------------------------------------------------------------
// Patch ready state

void
TexturePatch::enterPatchReadyState()
{
  state = PATCH_READY;
}


//--------------------------------------------------------------------------------
// Output

ostream& operator<<(ostream& out, TexturePatch* texturePatch)
{
  if(texturePatch->state != TexturePatch::ALL_READY) {
    out << "Completion: " << static_cast<int>(100 * (texturePatch->patchX + texturePatch->patchY * texturePatch->gridSize) /
                                              (texturePatch->gridSize * texturePatch->gridSize)) << "%" << endl;
  }
  else {
    out << "Completion: 100%" << endl;
  }
  
  out << "State: ";
  if(texturePatch->state == TexturePatch::LOADING) {
    out << "LOADING";
  }
  else if(texturePatch->state == TexturePatch::FILTERING) {
    out << "FILTERING";
  }
  else if(texturePatch->state == TexturePatch::ACTIVATING) {
    out << "ACTIVATING";
  }
  else if(texturePatch->state == TexturePatch::PATCH_READY) {
    out << "PATCH_READY";
  }
  else if(texturePatch->state == TexturePatch::ALL_READY) {
    out << "ALL_READY";
  }
  out << endl;

  return out;
}

