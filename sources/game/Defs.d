//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright 2002-2005 Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;
private import Mission;
private import Prototype;

void
createPrototypes(Mission mission)
{
  Prototype node = new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= node;
  node.setSimFloat(SimFloat.MASS, 11.6);
  node.setSimFloat(SimFloat.COST_HARDWARE, 8.7);
  node.setSimFloat(SimFloat.COST_MAINTENANCE, 0.36);
  node.setSimFloat(SimFloat.ENERGY, -4);
  node.setSimFloat(SimFloat.THERMAL, -2);
  node.setSimFloat(SimFloat.PROFIT_MAX, 5);
  
  node.setSimFloat(SimFloat.LIFESUPPORT_BASE, frand() * 10);
  node.setSimFloat(SimFloat.FLOW_CAPACITY, 3);
  node.setSimFloat(SimFloat.FLOW_PASSENGERS, cast(int)(fnrand() * 10.0) + 5);
  node.setSimFloat(SimFloat.FLOW_VISITORS, cast(int)(fnrand() * 10.0) + 5);
  node.setSimFloat(SimFloat.FLOW_CREW, cast(int)(fnrand() * 10.0) + 5);
  node.setSimFloat(SimFloat.FLOW_CARGO, cast(int)(fnrand() * 10.0) + 5);
  node.setSimFloat(SimFloat.FLOW_COMMODITIES, cast(int)(fnrand() * 10.0) + 5);
  node.setSimFloat(SimFloat.FLOW_SERVICE, cast(int)(fnrand() * 10.0) + 5);

  node.setSimString(SimString.NAME, "Node");
  node.setSimString(SimString.DESCRIPTION, "FIXME: Node Description");
  
//   Prototype service = new Prototype("assets/cislunar/service_module.shd", "assets/icons/Service_icon.png", false);  
//   mission.mPrototypes ~= service;
//   service.setSimFloat(SimFloat.MASS, 56.6);
//   service.setSimFloat(SimFloat.COST_HARDWARE, 52.0);
//   service.setSimFloat(SimFloat.COST_MAINTENANCE, 2.5);
//   service.setSimFloat(SimFloat.ENERGY, 20);
//   service.setSimFloat(SimFloat.THERMAL, 10);
//   service.setSimFloat(SimFloat.LIFESUPPORT_BASE, frand() * 10);
  
//   service.setSimFloat(SimFloat.FLOW_CAPACITY, 3);
//   service.setSimFloat(SimFloat.FLOW_PASSENGERS, cast(int)(fnrand() * 10.0 + 5));
//   service.setSimFloat(SimFloat.FLOW_VISITORS, cast(int)(fnrand() * 10.0 + 5));
//   service.setSimFloat(SimFloat.FLOW_CREW, cast(int)(fnrand() * 10.0 + 5));
//   service.setSimFloat(SimFloat.FLOW_CARGO, cast(int)(fnrand() * 10.0 + 5));
//   service.setSimFloat(SimFloat.FLOW_COMMODITIES, cast(int)(fnrand() * 10.0 + 5));
//   service.setSimFloat(SimFloat.FLOW_SERVICE, cast(int)(fnrand() * 10.0 + 5));

  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
  mission.mPrototypes ~= new Prototype("assets/modules/node.shd", "assets/icons/Node_icon.png", false);
}
