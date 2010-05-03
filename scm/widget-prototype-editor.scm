;;--------------------------------------------------------------------------------
;;
;; ShortHike, www.shorthike.com
;;
;; Copyright 2002-2006 by Kai Backman, Mistaril Ltd.
;;
;;--------------------------------------------------------------------------------

(define widget-prototype-editor-visible #f)

;(define show-prototype-editor
;  (lambda

(define widget-prototype-editor
  (lambda ()
    (if widget-prototype-editor-visible
	(begin
	  (do-texture-box (cid widget-control 10)
			  "assets/gui/DockBackground.png"
			  (rect 100 100 100 100)
			  8 8 8 8)
	  ))))
  
;   (let*
;       ((CONTROL-WIDTH 38)
;        (CONTROL-HEIGHT (+ (* 2 35) 3))
;        (CONTROL-BUTTON-SIZE 32))
;     (lambda ()
;       (let
; 	  ((dst-rect (rect 0  0  CONTROL-WIDTH  CONTROL-HEIGHT))
; 	   (button-rect (rect 3 3 CONTROL-BUTTON-SIZE CONTROL-BUTTON-SIZE))
; 	   (system-menu-id (cid widget-control 1010))
; 	   (layer-menu-id (cid widget-control 1020)))
; 	;; Bevel background
; 	(do-texture-box (cid widget-control 10)
; 			"assets/gui/DockBackground.png" dst-rect 8 8 8 8)
; 	;; System menu button
; 	(if (do-button (cid widget-control 20)
; 		       "assets/gui/ControlButtons.png" 4 4 button-rect 4)
; 	    (gui-make-active system-menu-id))
; 	;; Layer menu button
; 	(set-rect-y! button-rect (+ (rect-y button-rect) CONTROL-BUTTON-SIZE 3))
; 	(if (do-button (cid widget-control 30)
; 		       "assets/gui/ControlButtons.png" 4 4 button-rect 0)
; 	    (gui-make-active layer-menu-id))
	
; 	;; Do pop up menu, should use case ..
; 	(if (not (= (do-popup system-menu-id '("Quit")) -1))
; 	    (exit))
; 	(if (not (= (do-popup layer-menu-id '("Energy" "Thermal" "Life Support")) -1))
; 	    (write "Layer selected"))
; 	))))
