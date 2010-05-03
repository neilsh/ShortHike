//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"
#include "PrototypeLayer.hpp"

#include "file/CacheManager.hpp"
#include "file/ChunkData.hpp"
#include "file/ChunkStream.hpp"
#include "game/Port.hpp"
#include "game/Prototype.hpp"
#include "guilib/Button.hpp"
#include "guilib/FlowLayout.hpp"
#include "guilib/Frame.hpp"
#include "guilib/FrameBackground.hpp"
#include "guilib/GridLayout.hpp"
#include "guilib/GUIManager.hpp"
#include "guilib/RasterImage.hpp"
#include "main/Entry.hpp"
#include "main/Main.hpp"
#include "main/Platform.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/RenderSystem.hpp"
#include "rendering/Surface.hpp"
#include "rendering/Texture.hpp"

// --------------------------------------------------------------------------------
// Singleton implementation

PrototypeLayer*
rPrototypeLayer()
{
  static PrototypeLayer* mPrototypeLayer = new PrototypeLayer();
  return mPrototypeLayer;
}

// --------------------------------------------------------------------------------

PrototypeLayer::PrototypeLayer()
  : mPrototype(new Prototype()), mFileName(""), mCurrentPort(0), mRightPanel(NULL),
    mRenderNormals(false), mRenderTexture(true), mRenderWireframe(false),
    mImportOBJButton(NULL), mImportSSMButton(NULL),
    mImportTGAButton(NULL), mImportPNGButton(NULL),
    mToggleNormalsButton(NULL), mToggleTextureButton(NULL),

    mPortNumLabel(NULL),
    mPortPosLabel(NULL),

    mPortNextButton(NULL),
    mPortPrevButton(NULL),
    mPortAddButton(NULL),
    mPortRemoveButton(NULL),

    mPortMovXAddButton(NULL),
    mPortMovXSubButton(NULL),
    mPortMovYAddButton(NULL),
    mPortMovYSubButton(NULL),
    mPortMovZAddButton(NULL),
    mPortMovZSubButton(NULL),

    mPortRotXAddButton(NULL),
    mPortRotXSubButton(NULL),
    mPortRotYAddButton(NULL),
    mPortRotYSubButton(NULL),
    mPortRotZAddButton(NULL),
    mPortRotZSubButton(NULL),

    mPortMesh(NULL),
    mNormalMesh(NULL)
{
  GridLayout* layout = new GridLayout(this, 32, 24);

  mRightPanel = new Frame();
  mRightPanel->setBackground(new FrameBackground());
  layout->add(mRightPanel, 27, 2, 5, 10);
  mRightPanel->setLayoutManager(new FlowLayout(FlowLayout::VERTICAL, FlowLayout::RIGHT, FlowLayout::TOP));
  mRightPanel->add(mNewPrototypeButton = new Button(L"New Module", this));
  mRightPanel->add(mLoadPrototypeButton = new Button(L"Load Module", this));
  mRightPanel->add(mSavePrototypeButton = new Button(L"Save Module", this));
  mRightPanel->add(mSaveAsPrototypeButton = new Button(L"Save Module As", this));
  mRightPanel->add(mImportOBJButton = new Button(L"Import OBJ Mesh", this));
  mRightPanel->add(mImportSSMButton = new Button(L"Import SSM Mesh", this));
  mRightPanel->add(mImportTGAButton = new Button(L"Import TGA Texture", this));
  mRightPanel->add(mImportPNGButton = new Button(L"Import PNG Texture", this));
  mRightPanel->add(mImportIconButton = new Button(L"Import PNG Icon", this));
  mRightPanel->add(mToggleNormalsButton = new Button(L"Toggle Normals", this));
  mRightPanel->add(mToggleTextureButton = new Button(L"Toggle Texture", this));
  mRightPanel->add(mToggleWireframeButton = new Button(L"Toggle Wireframe", this));
  mRightPanel->add(mQuitButton = new Button(L"Quit", this));
  
  mStatPanel = new Frame();
  mStatPanel->setBackground(new FrameBackground());
  layout->add(mStatPanel, 0, 0, 20, 1);
  mStatPanel->add(mStatLabel = new Label(L"Mesh stats"));

  mIconPanel = new Frame();
  mIconPanel->setBackground(new FrameBackground());
  layout->add(mIconPanel, 27, 12, 5, 5);
  mIconPanel->add(mIconImage = new RasterImage(mPrototype->getIconSurface128()));
  mIconImage->setPadding(32);

  mPortPanel = new Frame();
  mPortPanel->setBackground(new FrameBackground());
  layout->add(mPortPanel, 24, 20, 8, 4);
  GridLayout* portLayout = new GridLayout(mPortPanel, 8, 6);
  portLayout->set_borders(2);
  portLayout->set_padding(2);
  portLayout->add(mPortNumLabel = new Label(L"Port 1/2"), 0, 0, 6, 1);
  portLayout->add(mPortPosLabel = new Label(L"Pos: 31.0 32.0 33.0"), 0, 1, 8, 1);

  portLayout->add(mPortNextButton = new Button(L"Next", this), 6, 2, 2, 1);
  portLayout->add(mPortPrevButton = new Button(L"Prev", this), 6, 3, 2, 1);
  portLayout->add(mPortAddButton = new Button(L"Add", this), 6, 4, 2, 1);
  portLayout->add(mPortRemoveButton = new Button(L"Remove", this), 6, 5, 2, 1);

  portLayout->add(mPortMovXAddButton = new Button(L"Mov +X", this), 0, 2, 2, 1);
  portLayout->add(mPortMovXSubButton = new Button(L"Mov -X", this), 0, 3, 2, 1);
  portLayout->add(mPortMovYAddButton = new Button(L"Mov +Y", this), 2, 2, 2, 1);
  portLayout->add(mPortMovYSubButton = new Button(L"Mov -Y", this), 2, 3, 2, 1);
  portLayout->add(mPortMovZAddButton = new Button(L"Mov +Z", this), 4, 2, 2, 1);
  portLayout->add(mPortMovZSubButton = new Button(L"Mov -Z", this), 4, 3, 2, 1);

  portLayout->add(mPortRotXAddButton = new Button(L"Rot +X", this), 0, 4, 2, 1);
  portLayout->add(mPortRotXSubButton = new Button(L"Rot -X", this), 0, 5, 2, 1);
  portLayout->add(mPortRotYAddButton = new Button(L"Rot +Y", this), 2, 4, 2, 1);
  portLayout->add(mPortRotYSubButton = new Button(L"Rot -Y", this), 2, 5, 2, 1);
  portLayout->add(mPortRotZAddButton = new Button(L"Rot +Z", this), 4, 4, 2, 1);
  portLayout->add(mPortRotZSubButton = new Button(L"Rot -Z", this), 4, 5, 2, 1);  

  mPortMesh = rRenderSystem()->createMesh();
  mPortMesh->load("assets/user_interface/editor_port.shd");
  mNormalMesh = rRenderSystem()->createMesh();

  clear();
}

// --------------------------------------------------------------------------------

void
PrototypeLayer::clear()
{
  mFileName = "";
  mPrototype->clear();
  updateMeshInfo();
  mIconImage->markDirty();
}


// --------------------------------------------------------------------------------

void
PrototypeLayer::render(RenderSystem* renderSystem, int, int)
{
  if(NULL == mPrototype->getColorTexture() || NULL == mPrototype->getMesh()) return;

  mPrototype->touch();
  
  if(mRenderTexture) {
    renderSystem->enableColorTexture(true);
    renderSystem->setColorTexture(mPrototype->getColorTexture());
  }
  else {
    renderSystem->enableColorTexture(false);
  }
  
  renderSystem->enableAlphaBlending(false);
  renderSystem->setMesh(mPrototype->getMesh(), mRenderWireframe);
  renderSystem->renderMesh(Matrix::IDENTITY);

  renderSystem->enableColorTexture(false);
  renderSystem->setMesh(mPortMesh);

  PortList& ports = mPrototype->getPorts();
  for(unsigned int iPort = 0; iPort < ports.size(); ++iPort) {
    Port* port = ports[iPort];
    Matrix transform(port->getPosition(), port->getOrientation());
    if(iPort == static_cast<unsigned int>(mCurrentPort)) {
      renderSystem->setColor(0.9, 0.9, 1.0);
    }
    else {
      renderSystem->setColor(0.5, 0.5, 0.5);
    }    
    renderSystem->renderMesh(transform);
  }

  if(mRenderNormals) {
    renderSystem->setColor(0, 0, 1);
    renderSystem->setMesh(mNormalMesh, true, false);
    renderSystem->renderMesh(Matrix::IDENTITY);
  }
}

// --------------------------------------------------------------------------------

void
PrototypeLayer::update(float)
{
  PortList& ports = mPrototype->getPorts();

  if(0 == ports.size()) {
    mPortNumLabel->setLabel(L"No ports");
    mPortPosLabel->setLabel(L"");
  }
  else {
    mCurrentPort = clamp<int>(mCurrentPort, 0, ports.size() - 1);
    wostringstream wossPortStat;
    wossPortStat << "Port " << mCurrentPort + 1 << "/" << ports.size();
    mPortNumLabel->setLabel(wossPortStat.str());
    
    wostringstream wossPortPos;
    Vector pos = ports[mCurrentPort]->getPosition();
    wossPortPos << fixed << "Pos: x(" << setprecision(2) << pos.x << ") y("
                << setprecision(2) << pos.y << ") z("
                << setprecision(2) << pos.z;
    mPortPosLabel->setLabel(wossPortPos.str());
  }
}

// --------------------------------------------------------------------------------

void
PrototypeLayer::actionPerformed(Widget* source)
{
  if(source == mNewPrototypeButton) {
    clear();
  }
  else if(source == mLoadPrototypeButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mFileName = fileName;

      // Raw load to get around CacheManager.
      ifstream inFile(mFileName.c_str(), ios_base::in | ios_base::binary);
      inFile.seekg(0, ios::end);
      streampos fileSize = inFile.tellg();
      inFile.seekg(0, ios::beg);

      char* data = new char[fileSize];
      inFile.read(data, fileSize);
      ChunkData* chunkData = new ChunkData(data, fileSize);
      mPrototype->load(chunkData, true);
      delete chunkData;
      delete[] data;
      inFile.close();
      
      // Force recalculation of collision data and similar
      mPrototype->getMesh()->flush();
      
      updateMeshInfo();
      mIconImage->markDirty();
    }
  }
  else if(source == mSavePrototypeButton && mFileName != "") {
    ChunkStream cStream(mFileName);
    mPrototype->save(&cStream);
    cStream.close();
    updateMeshInfo();
  }
  else if(source == mSaveAsPrototypeButton || source == mSavePrototypeButton) {
    string fileName;
    if(rPlatform()->saveFileDialog(fileName)) {
      mFileName = fileName;
      ChunkStream cStream(mFileName);
      mPrototype->save(&cStream);
      cStream.close();
      updateMeshInfo();
    }
  }
  else if(source == mImportOBJButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mPrototype->getMesh()->loadFromOBJ(fileName);
      updateMeshInfo();
    }
  }
  else if(source == mImportSSMButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mPrototype->getMesh()->loadFromSSM(fileName);
      updateMeshInfo();
    }    
  }
  else if(source == mImportTGAButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mPrototype->getColorTexture()->loadFromTGA(fileName);
      updateMeshInfo();
    }
  }  
  else if(source == mImportPNGButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mPrototype->getColorTexture()->loadFromPNG(fileName);    
      updateMeshInfo();
    }
  }  
  else if(source == mImportIconButton) {
    string fileName;
    if(rPlatform()->openFileDialog(fileName)) {
      mPrototype->getIconSurface128()->loadFromPNG(fileName);    
      mPrototype->updateIcons();
      mIconImage->markDirty();
    }
  }  
  else if(source == mToggleNormalsButton) {
    mRenderNormals = !mRenderNormals;
  }
  else if(source == mToggleTextureButton) {
    mRenderTexture = !mRenderTexture;
  }
  else if(source == mToggleWireframeButton) {
    mRenderWireframe = !mRenderWireframe;
  }
  else if(source == mQuitButton) {
    rMain()->quitApplication();
  }

  // ----------------------------------------
  // Port controls

  PortList& ports = mPrototype->getPorts();

  if(source == mPortNextButton) {
    mCurrentPort = clamp<int>(mCurrentPort + 1, 0, ports.size());
  }
  else if(source == mPortPrevButton) {
    mCurrentPort = clamp<int>(mCurrentPort - 1, 0, ports.size());
  }  
  else if(source == mPortAddButton) {
    Port* newPort = new Port();
    mPrototype->addPort(newPort);
    mCurrentPort = ports.size() - 1;
  }
  else if(source == mPortRemoveButton && 0 != ports.size()) {
    ports.erase(ports.begin() + mCurrentPort);
    mCurrentPort = clamp<int>(mCurrentPort, 0, ports.size());    
  }
  

  // ----------------------------------------
  // Movement and Rotate
  if(0 == ports.size()) return;

  float deltaMove = 1;
  if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_SHIFT)
    deltaMove = 0.1;
  if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_CONTROL)
    deltaMove = 0.01;
 
  Vector& position = ports[mCurrentPort]->getPosition();
  if(source == mPortMovXAddButton) {
    position.x += deltaMove;
  }
  else if(source == mPortMovXSubButton) {
    position.x -= deltaMove;
  }
  else if(source == mPortMovYAddButton) {
    position.y += deltaMove;
  }
  else if(source == mPortMovYSubButton) {
    position.y -= deltaMove;
  }
  else if(source == mPortMovZAddButton) {
    position.z += deltaMove;
  }
  else if(source == mPortMovZSubButton) {
    position.z -= deltaMove;
  }

  Degree deltaRot = 45;
  if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_SHIFT)
    deltaRot = 45 / 4;
  if(rGUIManager()->getKeyModifier() & KEY_MODIFIER_CONTROL)
    deltaRot = 45 / 16;

  Quaternion& orientation = ports[mCurrentPort]->getOrientation();
  if(source == mPortRotXAddButton) {
    orientation = orientation * Quaternion(Vector::UNIT_X, deltaRot);
  }
  else if(source == mPortRotXSubButton) {
    orientation = orientation * Quaternion(Vector::UNIT_X, -deltaRot);
  }  
  else if(source == mPortRotYAddButton) {
    orientation = orientation * Quaternion(Vector::UNIT_Y, deltaRot);
  }  
  else if(source == mPortRotYSubButton) {
    orientation = orientation * Quaternion(Vector::UNIT_Y, -deltaRot);
  }  
  else if(source == mPortRotZAddButton) {
    orientation = orientation * Quaternion(Vector::UNIT_Z, deltaRot);
  }  
  else if(source == mPortRotZSubButton) {
    orientation = orientation * Quaternion(Vector::UNIT_Z, -deltaRot);
  }  
}


// --------------------------------------------------------------------------------

void
PrototypeLayer::updateMeshInfo()
{
  VertexData& vertices = mNormalMesh->getVertexData();
  IndexData& indices = mNormalMesh->getIndexData();
  vertices.clear();
  indices.clear();
  
  VertexData& sourceData = mPrototype->getMesh()->getVertexData();
  int index = 0;
  for(VertexData::iterator iVertex = sourceData.begin(); iVertex != sourceData.end(); ++iVertex) {
    const Vertex& vertex = *iVertex;
    Vertex base = vertex;
    Vertex normal = vertex;
    Vertex third = vertex;
    normal.position += normal.normal;
    third.position += Vector(0.0001);
    vertices.push_back(base);
    vertices.push_back(normal);
    vertices.push_back(third);

    // Push a bad triangle
    indices.push_back(index++);
    indices.push_back(index++);
    indices.push_back(index++);
  }
  mNormalMesh->flush();

  wostringstream wossStat;
  if(mFileName != "")
    wossStat << mFileName.c_str() << endl;
  else
    wossStat << "untitled" << endl;

  wossStat << " Vertices: " << mPrototype->getMesh()->getVertexCount()
           << " Triangles: " << (mPrototype->getMesh()->getIndexCount() / 3)
           << " Tex width: " << mPrototype->getColorTexture()->getWidth()
           << " Tex height: " << mPrototype->getColorTexture()->getHeight();
  mStatLabel->setLabel(wossStat.str());
}
