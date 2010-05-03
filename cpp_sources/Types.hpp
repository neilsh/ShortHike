//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai-Peter Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

#ifndef TYPES_HPP
#define TYPES_HPP


typedef float Real;
typedef float Time;
typedef float FontSize;
using agg::int8u;

enum DiscreteResource {
  DISCRETE_INITIAL = 0,
  DISCRETE_LIFESUPPORT = 0,
  DISCRETE_STRESS,

  DISCRETE_FLOW_INITIAL = 2,
  DISCRETE_PASSENGERS = 2,
  DISCRETE_VISITORS,
  DISCRETE_CREW,
  DISCRETE_CARGO,
  DISCRETE_COMMODITIES,
  DISCRETE_SERVICE,
  DISCRETE_NUM
};


#endif
