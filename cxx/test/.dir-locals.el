;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((c++-mode
   ;; (c-offsets-alist . (inextern-lang . 0))
   (compile-command . "cd ../../build && make doctests && ./doctests -nc")
   (c-file-style . "stroustrup")))
