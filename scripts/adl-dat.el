;
;; Major mode for ADL DAT files.  Derived from text mode, so most of
;; the key strokes for text mode will work
;;

(defgroup adl-dat nil
  "Major mode for editing ADL DAT source in Emacs"
  :group 'languages)

(require 'derived)

(defvar adl-dat-mode-hook nil)

(defvar adl-dat-mode-map
  (let ((adl-dat-mode-map (make-keymap)))
    adl-dat-mode-map)
  "Keymap for ADL DAT major mode")

(defvar adl-dat-mode-syntax-table
   (let ((st (make-syntax-table)))
		 (modify-syntax-entry ?\" "\"" st)
		 (modify-syntax-entry ?\# "<"  st)
		 (modify-syntax-entry ?\n ">"  st)
     st)
   "Syntax table for `adl-dat-mode'.")

(defvar adl-dat-font-lock-keywords 
  (list
   '("^[a-zA-Z0-9_]+" . font-lock-builtin-face)       ; Card types
   '("[a-zA-Z0-9_]+=" . font-lock-variable-name-face) ; Keys
    )
  "Syntax highlighting in ADL DAT mode.")

(define-derived-mode adl-dat-mode text-mode "ADL DAT"
  "Major mode for editing ADL DAT files.
In order to enter this mode automatically, you can either place the
following code into your .emacs file:

  (setq auto-mode-alist (cons '(\"\\.dat$\" . adldat-mode)  auto-mode-alist))

where in this case, all files with the .dat extension will turn on
adldat-mode, or make sure that each ADL DAT source file has the following
as the first line in the file:

  # -*- adl-dat -*-

In addition, make sure that the following line is in your .emacs file:

  (require 'adldat)

This searches the load path for the file 'adldat.elc' or 'adldat.el'.

Special commands:
\\{adl-dat-mode-map}"
	:syntax-table adl-dat-mode-syntax-table
	(set (make-local-variable 'comment-start) "# ")
	(set (make-local-variable 'comment-start-skip) "#+\\s-*")
	(set (make-local-variable 'font-lock-defaults)
			 '(adl-dat-font-lock-keywords))
	(set (make-local-variable 'require-final-newline) t)
	(set (make-local-variable 'parse-sexp-ignore-comments) t)
	)
	   
(provide 'adl-dat)
