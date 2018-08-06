# some utility functions

# Define an object with position independent code (-fPIC)
function (add_object name source)
  add_library(${name} OBJECT ${source})
  set_property(TARGET ${name} PROPERTY POSITION_INDEPENDENT_CODE ON)
endfunction()


function (add_executable_h5 name)
  list(REMOVE_AT ARGV 0)
  add_executable(${name} ${ARGV})
  target_include_directories(${name} PUBLIC ${HDF5_INCLUDE_DIR})
  target_link_libraries(${name} ${HDF5_LIBRARIES})
endfunction()
