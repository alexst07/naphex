##
## Position Independent Code for libraries
##
set(CMAKE_CXX_FLAGS             "-fPIC")

##
## Set the output path for the libraries
##
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

##
## Add ipv4 lib
##
add_library(ipv4 SHARED ipv4.c)
set_target_properties(ipv4 PROPERTIES PREFIX "")
set_target_properties(ipv4 PROPERTIES OUTPUT_NAME ipv4)
install(TARGETS ipv4 LIBRARY DESTINATION ${NAPHEX_INSTALL_LIB_PATH})

##
## Add ethernet lib
##
add_library(ether SHARED ether.c)
set_target_properties(ether PROPERTIES PREFIX "")
set_target_properties(ether PROPERTIES OUTPUT_NAME ether)
install(TARGETS ether LIBRARY DESTINATION ${NAPHEX_INSTALL_LIB_PATH})
