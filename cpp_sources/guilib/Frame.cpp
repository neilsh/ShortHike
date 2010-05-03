//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "Frame.hpp"
#include "GUIManager.hpp"
#include "Graphics.hpp"

#include "main/Main.hpp"
#include "rendering/Surface.hpp"
#include "rendering/RenderSystem.hpp"

//--------------------------------------------------------------------------------

const int INITIAL_TEXTURE_SIZE = 16;

Frame::Frame()
  : mSurface(NULL)
{
//   //----------------------------------------
//   // Create texture

//   ostringstream textureNameStream;
//   textureNameStream << "frame_texture_" <<name;
//   textureName = textureNameStream.str();

//   int textureSize = INITIAL_TEXTURE_SIZE;
//   image.loadDynamicImage(new uchar[textureSize * textureSize * 4], textureSize, textureSize, 1, TEXTURE_FORMAT, true);

//   texture = TextureManager::getSingleton().createManual(textureName, RESOURCE_GROUP_MAIN,
//                                                                    TEX_TYPE_2D, textureSize, textureSize, 0,
//                                                                    TEXTURE_FORMAT, TU_DYNAMIC_WRITE_ONLY);
//   updateTexture();

//   //----------------------------------------
//   // RenderOperation and vertex data

//   renderOperation.operationType = RenderOperation::OT_TRIANGLE_STRIP;
//   renderOperation.useIndexes = false;
//   renderOperation.indexData = NULL;
//   renderOperation.srcRenderable = NULL;

//   vertexData = new VertexData();
//   renderOperation.vertexData = vertexData;

//   // Create vertex declaration for our geometry
//   VertexDeclaration* vertexDeclaration = vertexData->vertexDeclaration;
//   size_t offset = 0;
//   vertexDeclaration->addElement(HW_BUFFER_BINDING, 0, VET_FLOAT3, VES_POSITION);
//   offset += VertexElement::getTypeSize(VET_FLOAT3);
//   vertexDeclaration->addElement(HW_BUFFER_BINDING, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

//   vertexData->vertexStart = 0;
//   vertexData->vertexCount = 4;
  
//   vertexBuffer = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDeclaration->getVertexSize(HW_BUFFER_BINDING), 
//                                                                           4, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
//   vertexData->vertexBufferBinding->setBinding(HW_BUFFER_BINDING, vertexBuffer);
//   updateVertexBuffer();
}


//--------------------------------------------------------------------------------

void
Frame::paintAll(Graphics*)
{
  if(isWidgetDirty() == true) {
//     int width = (getWidth() < static_cast<int>(image.getWidth()) ? getWidth() : image.getWidth());
//     int height = (getHeight() < static_cast<int>(image.getHeight()) ? getHeight() : image.getHeight());
    
//     Graphics frameGraphics(image.getPixelBox(), GRect(0, 0, width, height));
//     frameGraphics.clear();
//     Widget::paintAll(&frameGraphics);
//     updateTexture();

    if(mSurface != NULL) {
      Graphics surfaceGraphics(mSurface, GRect(0, 0, getWidth(), getHeight()));
      surfaceGraphics.clear();
      Widget::paintAll(&surfaceGraphics);
      mSurface->flush();
    }
  }  
}


//--------------------------------------------------------------------------------

// void
// Frame::render(RenderSystem* renderSystem, int deltaX, int deltaY)
// {
//   Real screenWidth = rGUIManager()->getScreenWidth();
//   Real screenHeight = rGUIManager()->getScreenHeight();
  
//   Real horizontalTexelOffset = renderSystem->getHorizontalTexelOffset();
//   Real verticalTexelOffset = renderSystem->getVerticalTexelOffset();
//   Real xPosition = deltaX + horizontalTexelOffset;
//   Real yPosition = deltaY + verticalTexelOffset;

//   Real screenX = xPosition / (screenWidth * 0.5) - 1.0;
//   Real screenY = yPosition / (screenHeight * 0.5) - 1.0;

//   Matrix4 frameTranslate = Matrix4::IDENTITY;
//   frameTranslate.makeTrans(Vector(screenX, -screenY, 0));
//   Matrix4 frameScale = Matrix4::IDENTITY;
//   frameScale.setScale(Vector((1 / (screenWidth * 0.5)), (1 / (screenHeight * 0.5)), 1));
//   Matrix4 frameTransform = frameTranslate * frameScale;
//   renderSystem->_setWorldMatrix(frameTransform);

//   renderSystem->_setTexture(0, true, texture->getName());
//   renderSystem->_render(renderOperation);

//   renderSystem->_setWorldMatrix(Matrix4::IDENTITY);

//   rRenderSystem()->renderSurface(mSurface, deltaX, deltaY);
// }

// --------------------------------------------------------------------------------
// New custom rendering system

void
Frame::render(RenderSystem* renderSystem, int deltaX, int deltaY)
{
  renderSystem->renderSurface(mSurface, deltaX, deltaY);
}

//--------------------------------------------------------------------------------

void
Frame::setSize(const int w, const int h)
{
  if(w == getWidth() && h == getHeight()) return;
  
//   int width = w;
//   int height = h;
//   int textureWidth = INITIAL_TEXTURE_SIZE;
//   while(textureWidth < width) textureWidth *= 2;
//   int textureHeight = INITIAL_TEXTURE_SIZE;
//   while(textureHeight < height) textureHeight *= 2;
  
//   image.loadDynamicImage(new uchar[textureWidth * textureHeight * 4], textureWidth, textureHeight, 1, TEXTURE_FORMAT, true);
//   TextureManager::getSingleton().unload(textureName);
//   TextureManager::getSingleton().remove(textureName);
//   texture = TextureManager::getSingleton().createManual(textureName, RESOURCE_GROUP_MAIN,
//                                                         TEX_TYPE_2D, textureWidth, textureHeight, 0,
//                                                         TEXTURE_FORMAT, TU_DYNAMIC_WRITE_ONLY);

  if(NULL != mSurface)
    rRenderSystem()->destroySurface(mSurface);
  mSurface = rRenderSystem()->createSurface(w, h);

  Widget::setSize(w, h);
  markDirty();
}


// void
// Frame::boundsUpdatedEvent()
// {
//   updateVertexBuffer();
// }


//--------------------------------------------------------------------------------


// void
// Frame::updateVertexBuffer()
// {
//   using Ogre::Rectangle;
  
//   Real positionZ = -1;

//   Rectangle screenArea;
//   screenArea.left = 0;
//   screenArea.right = getWidth();
//   screenArea.top = 0;
//   screenArea.bottom = getHeight();

//   // Switch Y coordinate
//   screenArea.top = -screenArea.top;
//   screenArea.bottom = -screenArea.bottom;

//   float* vertexPtr = static_cast<float*>(vertexBuffer->lock(HardwareBuffer::HBL_DISCARD));

//   // FIXME: This relationship can be something else as well
//   float textureRight = static_cast<float>(getWidth()) / texture->getWidth();
//   float textureBottom = static_cast<float>(getHeight()) / texture->getHeight();

//   // Top left
//   *vertexPtr++ = screenArea.left;
//   *vertexPtr++ = screenArea.top;
//   *vertexPtr++ = positionZ;
//   *vertexPtr++ = 0.0;
//   *vertexPtr++ = 0.0;

//   // Bottom left
//   *vertexPtr++ = screenArea.left;
//   *vertexPtr++ = screenArea.bottom;
//   *vertexPtr++ = positionZ;
//   *vertexPtr++ = 0.0;
//   *vertexPtr++ = textureBottom;

//   // Top right
//   *vertexPtr++ = screenArea.right;
//   *vertexPtr++ = screenArea.top;
//   *vertexPtr++ = positionZ;
//   *vertexPtr++ = textureRight;
//   *vertexPtr++ = 0.0;

//   // Bottom right
//   *vertexPtr++ = screenArea.right;
//   *vertexPtr++ = screenArea.bottom;
//   *vertexPtr++ = positionZ;
//   *vertexPtr++ = textureRight;
//   *vertexPtr++ = textureBottom;

//   vertexBuffer->unlock();
// }

//--------------------------------------------------------------------------------


// void
// Frame::updateTexture()
// {
//   texture->loadImage(image);
// }

//--------------------------------------------------------------------------------

void
Frame::centerOnScreen()
{
  Dimension screenSize = rGUIManager()->getScreenDimension();
  setPosition((screenSize.w - getWidth()) * 0.5f, (screenSize.h - getHeight()) * 0.5f);
}
