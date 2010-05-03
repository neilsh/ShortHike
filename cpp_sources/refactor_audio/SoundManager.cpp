//--------------------------------------------------------------------------------
//
// Mars Base Manager
//
// Copyright in 2004 Kai-Peter Baeckman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------


#include "Common.hpp"

#include "SoundManager.hpp"
#include "SoundEffect.hpp"
#include "Ambient.hpp"
#include "Track.hpp"


//--------------------------------------------------------------------------------
// Singleton implementation

template<> SoundManager*
Singleton<SoundManager>::ms_Singleton = 0;


//--------------------------------------------------------------------------------
// Initializing the main screen

SoundManager::SoundManager()
  : ambientCounter(0), ambientState(SILENCE), silenceTimer(0), currentTrack(0), mEnabled(false)
{
  logEngineInfo("--------------------------------------------------------------------------------");
  ostringstream fsoundInfo;
  double fsoundVersion = FSOUND_GetVersion();
  fsoundInfo << "SoundManager:: intializing fmod "  // << fixed << set_precision(2)
             << fsoundVersion;
  logEngineInfo(fsoundInfo.str());

  checkFMODError(FSOUND_SetOutput(-1));
  printDriverCaps();   
  if(FSOUND_Init(44100, 32, 0) == FALSE) {
    logEngineInfo(string("SoundManager:: - Error in FSOUND_Init: ") + getFMODError());
    logEngineInfo("SoundManager:: WARNING: SOUND DISABLED");
  }
  else {
    printDiagnostics();    
    checkFMODError(FSOUND_SetMute(FSOUND_ALL, FALSE));
    mEnabled = true;
    logEngineInfo("SoundManager:: initialization complete");
  }
  logEngineInfo("--------------------------------------------------------------------------------");
}


SoundManager::~SoundManager()
{
  FSOUND_Close();
}


//--------------------------------------------------------------------------------
// Prints a warning about the potential fsound error

unsigned char
SoundManager::handleFMODError(unsigned char success, string description, string file, int line)
{
  if(success == FALSE) {
    ostringstream errorStream;
    errorStream << file << "(" << line << ") : FMOD error: " << getFMODError() << endl
                << "  - " << description;
    logEngineError(errorStream.str());
  }
  return success;
}
 
//--------------------------------------------------------------------------------
// Return string for FSOUND error

string
SoundManager::getFMODError()
{
  int errorCode = FSOUND_GetError();
  if(errorCode == FMOD_ERR_NONE)
    return "No errors"; 
  else if(errorCode == FMOD_ERR_BUSY)
    return "Cannot call this command after FSOUND_Init. Call FSOUND_Close first."; 
  else if(errorCode == FMOD_ERR_UNINITIALIZED)
    return "This command failed because FSOUND_Init or FSOUND_SetOutput was not called"; 
  else if(errorCode == FMOD_ERR_INIT)
    return "Error initializing output device."; 
  else if(errorCode == FMOD_ERR_ALLOCATED)
    return "Error initializing output device, but more specifically, the output device is already in use and cannot be reused."; 
  else if(errorCode == FMOD_ERR_PLAY)
    return "Playing the sound failed."; 
  else if(errorCode == FMOD_ERR_OUTPUT_FORMAT)
    return "Soundcard does not support the features needed for this soundsystem (16bit stereo output)"; 
  else if(errorCode == FMOD_ERR_COOPERATIVELEVEL)
    return "Error setting cooperative level for hardware."; 
  else if(errorCode == FMOD_ERR_CREATEBUFFER)
    return "Error creating hardware sound buffer."; 
  else if(errorCode == FMOD_ERR_FILE_NOTFOUND)
    return "File not found"; 
  else if(errorCode == FMOD_ERR_FILE_FORMAT)
    return "Unknown file format"; 
  else if(errorCode == FMOD_ERR_FILE_BAD)
    return "Error loading file"; 
  else if(errorCode == FMOD_ERR_MEMORY)
    return "Not enough memory or resources"; 
  else if(errorCode == FMOD_ERR_VERSION)
    return "The version number of this file format is not supported"; 
  else if(errorCode == FMOD_ERR_INVALID_PARAM)
    return "An invalid parameter was passed to this function"; 
  else if(errorCode == FMOD_ERR_NO_EAX)
    return "Tried to use an EAX command on a non EAX enabled channel or output."; 
  else if(errorCode == FMOD_ERR_CHANNEL_ALLOC)
    return "Failed to allocate a new channel"; 
  else if(errorCode == FMOD_ERR_RECORD)
    return "Recording is not supported on this machine"; 
  else if(errorCode == FMOD_ERR_MEDIAPLAYER)
    return "Windows Media Player not installed so cannot play wma or use internet streaming."; 
  else if(errorCode == FMOD_ERR_CDDEVICE)
    return "An error occured trying to open the specified CD device ";
  else {
    ostringstream errorInfo;
    errorInfo << "FMOD: Warning - Unknown error type: " << errorCode;
    logEngineError(errorInfo.str());
    return "Unknown error";
  }
}


//--------------------------------------------------------------------------------
// Print sound system capabilities and diagnostics information

void
SoundManager::printDriverCaps()
{
  // Output information
  {
    ostringstream outputInfo;
    int outputID = FSOUND_GetOutput();
    if(outputID == FSOUND_OUTPUT_NOSOUND)
      outputInfo << " Output: NoSound driver, all calls to this succeed but do nothing."; 
    else if(outputID == FSOUND_OUTPUT_WINMM)
      outputInfo << " Output: Windows Multimedia driver."; 
    else if(outputID == FSOUND_OUTPUT_DSOUND)
      outputInfo << " Output: DirectSound driver. You need this to get EAX2 or EAX3 support, or FX api support."; 
    else if(outputID == FSOUND_OUTPUT_A3D)
      outputInfo << " Output: A3D driver."; 
    else if(outputID == FSOUND_OUTPUT_OSS)
      outputInfo << " Output: Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers."; 
    else if(outputID == FSOUND_OUTPUT_ESD)
      outputInfo << " Output: Linux/Unix ESD (Enlightment Sound Daemon) driver."; 
    else if(outputID == FSOUND_OUTPUT_ALSA)
      outputInfo << " Output: Linux Alsa driver."; 
    else if(outputID == FSOUND_OUTPUT_ASIO)
      outputInfo << " Output: Low latency ASIO driver"; 
    else if(outputID == FSOUND_OUTPUT_XBOX)
      outputInfo << " Output: Xbox driver"; 
    else if(outputID == FSOUND_OUTPUT_PS2)
      outputInfo << " Output: PlayStation 2 driver"; 
    else if(outputID == FSOUND_OUTPUT_MAC)
      outputInfo << " Output: Mac SoundManager driver"; 
    else if(outputID == FSOUND_OUTPUT_GC)
      outputInfo << " Output: Gamecube driver"; 
    else if(outputID == FSOUND_OUTPUT_NOSOUND_NONREALTIME)
      outputInfo << " Output: This is the same as nosound, but the sound generation is driven by FSOUND_Update ";    
    else
      outputInfo << " Output: WARNING - Unknown output type: " << outputID;
    logEngineInfo(outputInfo.str());
  }
  
  int driverNum = FSOUND_GetNumDrivers();
  ostringstream driverEnum;
  driverEnum << " Driver enum count: " << driverNum;
  logEngineInfo(driverEnum.str());
  
  for(int driverI = 0; driverI < driverNum; ++driverI) {
    ostringstream driverInfo;
    driverInfo << " Driver" << driverI << ": " << FSOUND_GetDriverName(driverI);
    logEngineInfo(driverInfo.str());
    unsigned int driverCaps;
    if(FSOUND_GetDriverCaps(driverI, &driverCaps) == TRUE) {
      if(driverCaps & FSOUND_CAPS_HARDWARE)
        logEngineInfo("  * Supports hardware accelerated 3d sound.");
      if(driverCaps & FSOUND_CAPS_EAX2)
        logEngineInfo("  * Supports EAX 2 reverb.");
      if(driverCaps & FSOUND_CAPS_EAX3)
        logEngineInfo("  * Supports EAX 3 reverb.");
    }
    else {
      logEngineWarn(string("  Error reading driver caps: ") + getFMODError());
    }
  }
  
  // Driver information
  {
    ostringstream selectedDriverInfo;
    int driverID = FSOUND_GetDriver();
    selectedDriverInfo << " Selected Driver" << driverID << ": " << FSOUND_GetDriverName(driverID);
    logEngineInfo(selectedDriverInfo.str());    
  }  
}


void
SoundManager::printDiagnostics()
{
  // Channel information
  {    
    ostringstream channelInfo;
    channelInfo << " Maximum Channels: " << FSOUND_GetMaxChannels();
    logEngineInfo(channelInfo.str());

    int channels2D, channels3D, channelsTotal;
    if(FSOUND_GetNumHWChannels(&channels2D, &channels3D, &channelsTotal) == TRUE) {
      ostringstream hw2Stream;
      hw2Stream << " HardwareChannels 2D: " << channels2D;
      logEngineInfo(hw2Stream.str());

      ostringstream hw3Stream;
      hw3Stream << " HardwareChannels 3D: " << channels3D;
      logEngineInfo(hw3Stream.str());

      ostringstream hwTotStream;
      hwTotStream << " HardwareChannels Total: " << channelsTotal;
      logEngineInfo(hwTotStream.str());
    }
    else {
      reportFMODError("FSOUND_GetMaxChannels");
    }
  }

  // Mixer information
  {
    ostringstream mixerInfo;
    int mixerID = FSOUND_GetMixer();
    if(mixerID == FSOUND_MIXER_AUTODETECT)
      mixerInfo << " Mixer: CE/PS2/GC Only - Non interpolating/low quality mixer."; 
    else if(mixerID == FSOUND_MIXER_BLENDMODE)
      mixerInfo << " Mixer: Removed / obsolete."; 
    else if(mixerID == FSOUND_MIXER_MMXP5)
      mixerInfo << " Mixer: Removed / obsolete."; 
    else if(mixerID == FSOUND_MIXER_MMXP6)
      mixerInfo << " Mixer: Removed / obsolete.";
    else if(mixerID == FSOUND_MIXER_QUALITY_AUTODETECT)
      mixerInfo << " Mixer: Autodetect";
    else if(mixerID == FSOUND_MIXER_QUALITY_FPU)
      mixerInfo << " Mixer: Win32/Linux only - Interpolating/volume ramping FPU mixer."; 
    else if(mixerID == FSOUND_MIXER_QUALITY_MMXP5)
      mixerInfo << " Mixer: Win32/Linux only - Interpolating/volume ramping P5 MMX mixer."; 
    else if(mixerID == FSOUND_MIXER_QUALITY_MMXP6)
      mixerInfo << " Mixer: Win32/Linux only - Interpolating/volume ramping ppro+ MMX mixer."; 
    else if(mixerID == FSOUND_MIXER_MONO)
      mixerInfo << " Mixer: CE/PS2/GC only - MONO non interpolating/low quality mixer. For spee"; 
    else if(mixerID == FSOUND_MIXER_QUALITY_MONO)
      mixerInfo << " Mixer: CE/PS2/GC only - MONO Interpolating mixer. For speed ";    
    else
      mixerInfo << " Mixer: WARNING - Unknown mixer type: " << mixerID;
    logEngineInfo(mixerInfo.str());
  }  
}


//--------------------------------------------------------------------------------
// Static wrapper functions. These are safe to call no matter what.

void
SoundManager::playSoundEffect(string name)
{
  if(getSingletonPtr() == NULL) return;
  if(!getSingleton().mEnabled) return;
  getSingleton().playSoundEffectImpl(name);
}

void
SoundManager::setAmbient(string name)
{
  if(getSingletonPtr() == NULL) return;
  if(!getSingleton().mEnabled) return;
  getSingleton().setAmbientImpl(name);
}

void
SoundManager::silence()
{
  if(getSingletonPtr() == NULL) return;
  if(!getSingleton().mEnabled) return;
  getSingleton().silenceImpl();
}


//--------------------------------------------------------------------------------

void
SoundManager::addSoundEffect(string name, SoundEffect* effect)
{
  effects[name] = effect;
}


void
SoundManager::playSoundEffectImpl(string name)
{
  SoundEffect* effect = effects[name];
  if(effect != NULL)
    effect->play();
}


//--------------------------------------------------------------------------------

void
SoundManager::addAmbient(string name, Ambient* ambient)
{
  ambientMap[name] = ambientCounter++;
  ambientManager.addState(ambientMap[name], ambient);
}

void
SoundManager::setAmbientImpl(string name)
{
  if(ambientMap.find(name) != ambientMap.end()) {    
    ambientManager.setCurrentState(ambientMap[name]);
    updateTargets.insert(ambientManager.getCurrentState());
    ambientState = AMBIENT;
  }  
}

//--------------------------------------------------------------------------------

void
SoundManager::addTrack(Track* newTrack)
{
  playList.push_back(newTrack);
  if(ambientState != MUSIC)
    currentTrack = static_cast<unsigned int>(clamp<int>(static_cast<int>(frand() * playList.size() - 0.5f), 0, playList.size() - 1));
}

//--------------------------------------------------------------------------------

void
SoundManager::silenceImpl()
{
  ambientManager.clearState();
  ambientState = SILENCE;
}

//--------------------------------------------------------------------------------

void
SoundManager::toggleSilence()
{
  if(mEnabled) {
    FSOUND_SetMute(FSOUND_ALL, TRUE);
  }
  else {
    FSOUND_SetMute(FSOUND_ALL, FALSE);
  }
  mEnabled = !mEnabled;
}


//--------------------------------------------------------------------------------

const float MAGIC_SILENCE_SECONDS = 5.0;

void
SoundManager::update(Time deltaT)
{  
  set<SoundUpdate*> toRemove;
  for(set<SoundUpdate*>::const_iterator targetI = updateTargets.begin(); targetI != updateTargets.end(); ++targetI) {
    SoundUpdate* target = *targetI;
    if(target->update(deltaT) == false)
      toRemove.insert(target);
  }

  // Remove those targets who no longer wish to receive updates
  for(set<SoundUpdate*>::const_iterator targetI = toRemove.begin(); targetI != toRemove.end(); ++targetI) {
    updateTargets.erase(*targetI);
  }  


  // Loop between the states AMBIENT -> MUSIC -> SILENCE
  if(ambientState == AMBIENT) {
    if(ambientManager.getCurrentState() != NULL) {
      if(!ambientManager.getCurrentState()->isPlaying()) {
        ambientState = MUSIC;
        playList[currentTrack]->play();
      }
    }
    else {
      ambientState = SILENCE;
    }
  }
  else if(ambientState == MUSIC) {
    if(playList.size() == 0) {
      ambientState = SILENCE;
      silenceTimer = MAGIC_SILENCE_SECONDS;
    }
    else {
      Track* track = playList[currentTrack];
      if(track->isPlaying() == false) {
        track->stop();
        
        // Shuffle
        if(playList.size() > 1) {
          unsigned int newTrack = currentTrack;
          while(newTrack == currentTrack)
            newTrack = static_cast<unsigned int>(clamp<int>(static_cast<int>(frand() * playList.size() - 0.5f), 0, playList.size() - 1));
          currentTrack = newTrack;
        }
        
        ambientState = SILENCE;
        silenceTimer = MAGIC_SILENCE_SECONDS;
      }
    }
  }
  else if(ambientState == SILENCE) {
    silenceTimer -= deltaT;
    if(ambientManager.getCurrentState() != NULL && silenceTimer < 0) {
      ambientManager.getCurrentState()->play();
      ambientState = AMBIENT;
    }
  }
}

//--------------------------------------------------------------------------------

void*
loadResourceToMemory(string resourceName, string resourceGroup, string functionName, size_t* oFileSize)
{
  void* buffer;
  ResourceGroupManager* rManager = ResourceGroupManager::getSingletonPtr();
  if(rManager->resourceExists(resourceGroup, resourceName) == true) {
    DataStreamPtr dataStream = rManager->openResource(resourceName, resourceGroup);
    *oFileSize = dataStream->size();
    buffer = malloc(*oFileSize);
    size_t bytesRead;
    
    // Do the memory allocation
    if(buffer == NULL) {
      ostringstream allocInfo;
      allocInfo << functionName << " WARNING: Memory allocation failed for " << *oFileSize << "bytes";
      logEngineError(allocInfo.str());
      return NULL;
    }
    
    // Do the read
    if((bytesRead = dataStream->read(buffer, *oFileSize)) != *oFileSize) {
      ostringstream readInfo;
      readInfo << functionName << " WARNING: Unable to read file into memory:  " << bytesRead << "/" << *oFileSize << " bytes read";
      logEngineError(readInfo.str());
      free(buffer);
      return NULL;
    }

    // All clear
    return buffer;    
  }
  else {
    ostringstream errorStream;
    errorStream << functionName << " WARNING: Resource not found: " << resourceName;
    logEngineError(errorStream.str());
    return NULL;
  }
}

