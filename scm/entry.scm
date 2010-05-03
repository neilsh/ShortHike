;; Simple entry function

(write "Scheme loaded")

(load "scm/widget-control.scm")
(load "scm/widget-prototype-editor.scm")


(define test-button
  (lambda ()
    (if (do-button (cid test-button 0)
		   "assets/gui/ControlButtons.png" 4 4 
		   (rect 55 65 32 32)
		   4)
	(write "Scheme button pressed!"))))

(add-widget (lambda () (widget-control)))
(add-widget (lambda () (widget-prototype-editor)))
