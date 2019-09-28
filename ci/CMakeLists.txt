# some utility functions

# Define an object with position independent code (-fPIC)
function (add_object name source)
  add_library(${name} OBJECT ${source})
  set_property(TARGET ${name} PROPERTY POSITION_INDEPENDENT_CODE ON)
endfunction()
