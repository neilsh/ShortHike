//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#include "Common.hpp"

#include "OrbitReadyState.hpp"
#include "Orbit.hpp"

#include "widgets/TrackBallCamera.hpp"
#include "game/Module.hpp"
#include "game/Station.hpp"

//--------------------------------------------------------------------------------

OrbitReadyState::OrbitReadyState(Orbit* _orbitScreen)
  : OrbitState(_orbitScreen)
{
}

//--------------------------------------------------------------------------------

void
OrbitReadyState::enterState()
{
  Station* station = orbitScreen->getStation();
  if(station != NULL)
    station->setPortHandlesVisible(false);
}

void
OrbitReadyState::exitState()
{
}


//--------------------------------------------------------------------------------
// Output

void
OrbitReadyState::outputString(wostream& out)
{
}


