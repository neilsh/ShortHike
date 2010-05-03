//--------------------------------------------------------------------------------
//
// ShortHike
//
// Copyright in 2002-2005 by Kai Backman, Mistaril Ltd.
//
//--------------------------------------------------------------------------------

private import Common;

// game
private import Module;
private import Prototype;

// guilib
private import Font;
private import GUIManager;
private import Point;
private import Rect;

// rendering
private import RenderSystem;
private import Texture;

private import std.string;

// --------------------------------------------------------------------------------
// HUD

private Texture mBracketTexture;
const int BRACKET_SPACING = 128;

void doHUD(WidgetID id, Module target, Point pos)
{
  initHUD();
  
  doBracket(pos);
  Point topPos = pos;
  topPos.translate(-(BRACKET_SPACING / 2 + mBracketTexture.getWidth()) + 10,
                   -(BRACKET_SPACING / 2 + mBracketTexture.getHeight()) - 60);
  doTop(target, topPos);
}

// --------------------------------------------------------------------------------

void doTop(Module target, Point pos)
{
  Prototype prototype = target.getPrototype;

  // Each line is 15px apart (expect the first line, cause its a larger font, its offset by - 3.

  //// Line 1
  // Module name
  doText(cID(&doTop, 1000), prototype.getSimString(SimString.NAME), 15.0, Color.WHITE, pos.x, pos.y - 3);

  //// Line 2
  // Maintenance Cost
  float cost = prototype.getSimFloat(SimFloat.COST_MAINTENANCE);
  if(cost>0) {
    doText(cID(&doTop, 2000), "Cost:    $" ~ std.string.toString(cost) ~ " / day", 12.0, Color.WHITE, pos.x, pos.y + 15);
  }

  // Line 3
  // Energy status
  float energy = prototype.getSimFloat(SimFloat.ENERGY);
  if(energy < 0) {
    if(target.isEnergyOnline())
      doText(cID(&doTop, 3000), "E(OK)", 12.0, Color.WHITE, pos.x, pos.y);
    else
      doText(cID(&doTop, 3100), "E(FAIL)", 12.0, Color.WHITE, pos.x, pos.y + 30);
    doText(cID(&doTop, 3200), std.string.toString(-energy) ~ " kWe", 12.0, Color.WHITE, pos.x + 60, pos.y + 30);
  }

  // Thermal status
  float thermal = prototype.getSimFloat(SimFloat.THERMAL);
  if(thermal < 0) {
    if(target.isThermalOnline())
      doText(cID(&doTop, 4000), "T(OK)", 12.0, Color.WHITE, pos.x + 120, pos.y);
    else
      doText(cID(&doTop, 4100), "T(FAIL)", 12.0, Color.WHITE, pos.x + 120, pos.y + 30);
    doText(cID(&doTop, 4200), std.string.toString(-thermal) ~ " kWt", 12.0, Color.WHITE, pos.x + 180, pos.y + 30);
  }

  // Flow capacity
  float flow = prototype.getSimFloat(SimFloat.FLOW_CAPACITY);
  if(flow > 0) {
    doText(cID(&doTop, 5000), "Flow ", 12.0, Color.WHITE, pos.x + 240, pos.y + 30);
    doText(cID(&doTop, 5100), std.string.toString(flow) ~ " / day", 12.0, Color.WHITE, pos.x + 300, pos.y + 30);
  }

  //// Line 4
  float mass = prototype.getSimFloat(SimFloat.MASS);
  if(mass > 0) {
    doText(cID(&doTop, 6000), "Mass:    " ~ std.string.toString(mass), 12.0, Color.WHITE, pos.x, pos.y + 45);
  }
}


// --------------------------------------------------------------------------------

void doBracket(Point pos)
{
  int tw = mBracketTexture.getWidth;
  int th = mBracketTexture.getHeight;
  Rect dstRect = new Rect(0, 0, tw, th);
  Rect srcRect = new Rect(0, 0, tw, th);

  dstRect.setPosition(pos.x - BRACKET_SPACING / 2 - tw, pos.y - BRACKET_SPACING / 2 - th);
  rRenderSystem.renderTexture(mBracketTexture, dstRect, srcRect);

  dstRect.setPosition(pos.x + BRACKET_SPACING / 2, pos.y - BRACKET_SPACING / 2 - th);
  srcRect.flipX();
  rRenderSystem.renderTexture(mBracketTexture, dstRect, srcRect);  

  dstRect.setPosition(pos.x + BRACKET_SPACING / 2, pos.y + BRACKET_SPACING / 2);
  srcRect.flipY();
  rRenderSystem.renderTexture(mBracketTexture, dstRect, srcRect);  
  
  dstRect.setPosition(pos.x - BRACKET_SPACING / 2 - tw, pos.y + BRACKET_SPACING / 2);
  srcRect.flipX();
  rRenderSystem.renderTexture(mBracketTexture, dstRect, srcRect);
}



// --------------------------------------------------------------------------------

void initHUD()
{
  if(mBracketTexture is null) {
    mBracketTexture = rRenderSystem.createTexture();
    mBracketTexture.loadFromPNG("assets/gui/Bracket.png");
  }  
}
