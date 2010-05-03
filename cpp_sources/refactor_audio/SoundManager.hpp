//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef AUDIO_SOUND_MANAGER_HPP
#define AUDIO_SOUND_MANAGER_HPP

class SoundEffect;
class Ambient;
class Track;
class SoundUpdate;

typedef vector<Track*> PlayList;

class SoundManager : public Singleton<SoundManager>
{
public:
  SoundManager();
  ~SoundManager();
  
  void addSoundEffect(string name, SoundEffect* effect);
  void addAmbient(string name, Ambient* ambient);
  void addTrack(Track* track);
  void toggleSilence();

  static void playSoundEffect(string name);
  static void setAmbient(string name);
  static void silence();
  
  static unsigned char handleFMODError(unsigned char success, string description, string file, int line);
  static string getFMODError();

  void update(Time deltaT);
private:
  void printDriverCaps();
  void printDiagnostics();
  
  void playSoundEffectImpl(string name);
  void setAmbientImpl(string name);
  void silenceImpl();

  bool mEnabled;

  enum AmbientState {
    SILENCE,
    AMBIENT,
    MUSIC
  };
  
  AmbientState ambientState;
  float silenceTimer;
  
  unsigned int currentTrack;
  PlayList playList;

  int ambientCounter;
  map<string, int> ambientMap;
  StateManager<Ambient> ambientManager;
  set<SoundUpdate*> updateTargets;

  map<string, SoundEffect*> effects;
};

#define checkFMODError(FMODCall) SoundManager::handleFMODError(FMODCall, #FMODCall, __FILE__, __LINE__)
#define reportFMODError(desc) SoundManager::handleFMODError(FALSE, desc, __FILE__, __LINE__)

extern void* loadResourceToMemory(string resourceName, string resourceGroup, string functionName, size_t* OUTfileSize);

#endif
