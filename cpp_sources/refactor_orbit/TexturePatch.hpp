//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef USER_INTERFACE_ORBIT_TEXTURE_PATCH_HPP
#define USER_INTERFACE_ORBIT_TEXTURE_PATCH_HPP

class CNCSFile;

class TexturePatch
{
  friend ostream& operator<<(ostream& out, TexturePatch*);
public:
  enum State {
    LOADING,
    FILTERING,
    ACTIVATING,
    PATCH_READY,
    ALL_READY
  };
  
  TexturePatch::TexturePatch(string fileName, string prefix, Real overdraw, int gridSize = 16, int imageBands = 3);
  ~TexturePatch();

  void startLoad(float left, float right, float top, float bottom);
  State loadIncremental();
  void loadAll();
  State getState() {return state;}

  TexturePtr getTexture(int xposition, int yposition);
  Image& getImage(int xposition, int yposition);
  
  int getPatchX() { return patchX; }
  int getPatchY() { return patchY; }
private:
  void enterLoadingState();
  void processLoading();
  
  void enterFilteringState();
  void processFiltering();
  
  void enterActivatingState();
  void processActivating();

  void enterPatchReadyState();

  State state;
  Real loadFactor;
  Real overDraw;

  CNCSFile* imageFile;

  int gridSize;
  int imageWidth;
  int imageHeight;
  int imageBands;

  int left;
  int right;
  int top;
  int bottom;

  unsigned char* rawTextureBuffer;
  vector<TexturePtr> textures;
  vector<Image> images;

  int patchX;
  int patchY;


  // Loading state data
  int loadingLine;
  int loadingLinesPerFrame;
  unsigned char* loadingDataPointer;
  int rawTextureSize;

  bool isFirstHalf;
  int firstHalfWidth;
  int secondHalfRight;
  int secondHalfTop;
  int secondHalfBottom;
};

ostream& operator<<(ostream& out, TexturePatch*);

#endif
