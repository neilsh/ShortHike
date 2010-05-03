;;--------------------------------------------------------------------------------
;;
;; ShortHike, www.shorthike.com
;;
;; Copyright 2002-2006 by Kai Backman, Mistaril Ltd.
;;
;;--------------------------------------------------------------------------------


; //   const int CONTROL_WIDTH = 38;
; //   const int CONTROL_HEIGHT = 2 * 35 + 3;

; //   Rect dstRect = new Rect(0, 0, CONTROL_WIDTH, CONTROL_HEIGHT);

; //   // Render background box
; //   doTextureBox(id, "assets/gui/DockBackground.png", dstRect, 8, 8, 8, 8);

; //   const int CONTROL_BUTTON_SIZE = 32;
; //   WidgetID layerMenuID = cID(&doControl, 1010);
; //   WidgetID systemMenuID = cID(&doControl, 1020);
; //   Rect buttonRect = new Rect(dstRect.x + 3, dstRect.y + 3, CONTROL_BUTTON_SIZE, CONTROL_BUTTON_SIZE);

; //   // System button
; //   if(doButton(cID(&doControl, 200), cFrameTexture("assets/gui/ControlButtons.png", 4, 4), buttonRect, 4)) {
; //     rGUIManager.makeActive(systemMenuID);
; //   }
; //   buttonRect.y += CONTROL_BUTTON_SIZE + 3;

; //   // Layer button
; //   if(doButton(cID(&doControl, 10), cFrameTexture("assets/gui/ControlButtons.png", 4, 4), buttonRect)) {
; //     rGUIManager.makeActive(layerMenuID);
; //   }
; //   buttonRect.y += CONTROL_BUTTON_SIZE + 3;

; //   // Popup menus


; //   const char[][] SYSTEM_MENU_ITEMS = [
; //     "Quit"
; //   ];  
  
; //   int menuItem = doPopup(systemMenuID, SYSTEM_MENU_ITEMS);
; //   if(menuItem == 0) rMain().quit();

; //   Layer iLayer;
; //   if((iLayer = cast(Layer)doPopup(layerMenuID, LAYER_NAMES)) != -1)
; //     setLayer(iLayer);

; //   return Screen.STATION;

(define widget-control
  (let*
      ((CONTROL-WIDTH 38)
       (CONTROL-HEIGHT (+ (* 3 35) 3))
       (CONTROL-BUTTON-SIZE 32))
    (lambda ()
      (let
	  ((dst-rect (rect 0  0  CONTROL-WIDTH  CONTROL-HEIGHT))
	   (button-rect (rect 3 3 CONTROL-BUTTON-SIZE CONTROL-BUTTON-SIZE))
	   (system-menu-id (cid widget-control 1010))
	   (layer-menu-id (cid widget-control 1020))
	   (modder-menu-id (cid widget-control 1030))
	   (item-id -1))
	;; Bevel background
	(do-texture-box (cid widget-control 10)
			"assets/gui/DockBackground.png" dst-rect 8 8 8 8)

	;; System menu button
	(if (do-button (cid widget-control 20)
		       "assets/gui/ControlButtons.png" 4 4 button-rect 4)
	    (gui-make-active system-menu-id))

	;; Layer menu button
	(set-rect-y! button-rect (+ (rect-y button-rect) CONTROL-BUTTON-SIZE 3))
	(if (do-button (cid widget-control 30)
		       "assets/gui/ControlButtons.png" 4 4 button-rect 0)
	    (gui-make-active layer-menu-id))

	;; Modder menu button
	(set-rect-y! button-rect (+ (rect-y button-rect) CONTROL-BUTTON-SIZE 3))
	(if (do-button (cid widget-control 40)
		       "assets/gui/ControlButtons.png" 4 4 button-rect 8)
	    (gui-make-active modder-menu-id))
	
	;; Do pop up menu, should use case ..
	(if (not (= (do-popup system-menu-id '("Quit")) -1))
	    (exit))
	(if (not (= (do-popup layer-menu-id '("Energy" "Thermal" "Life Support")) -1))
	    (write "Layer selected"))
	(set! item-id (do-popup modder-menu-id
				'("New Prototype"
				  "Edit Prototype"
				  "Load Prototype"
				  "Save Prototype"
				  "Save As Prototype"
				  "Import SSM mesh"
				  "Import OBJ mesh")))
	))))
