//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#ifndef RENDERING_LOAD_IMAGE_HPP
#define RENDERING_LOAD_IMAGE_HPP

bool loadImageTGA(string fileName, agg::int8u** data, int* width, int* height);
bool loadImagePNG(string fileName, agg::int8u** data, int* width, int* height);
bool createMipMaps(int8u* originalImage, int width, int height, vector<int8u*>& oMipMaps);

#endif
