;;; Directory Local Variables
;;; For more information see (info "(emacs) Directory Variables")

((c++-mode
   (compile-command . "cd ../../build && COLOR=0 make doctests && ./doctests -nc")
   (c-file-style . "stroustrup")))
