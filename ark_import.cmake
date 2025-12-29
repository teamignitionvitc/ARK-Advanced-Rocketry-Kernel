#
#                                        ::                                                      
#                                        ::                                                      
#                                        ::                                                      
#                                        ::                                                      
#                                        ::                                                      
#    ..    ..........    :.      ::      ::     .........  ..    ..........    ...      .        
#    ::    ::            : .:.   ::     .::.       ::      ::    ::       :    :: :.    :        
#    ::    ::   ..:::    :   .:. ::    ::::::      ::      ::    ::       :    ::   ::  :        
#    ::    ::......::    :      :::    ::::::      ::      ::    ::.......:    ::     :::        
#                                      ::::::                                                    
#                                      :.::.:                                                    
#                         .::::          ::          ::::.                                       
#                       .::::::::.       ::       .:::::::::                                     
#                       ::::::::::::....::::.....:::::::::::                                     
#                        .:::::::::::::::::::::::::::::::::.                                     
#                                                                                                
#                  © Copyright of Ignition Software                                              
#                                                                                                
#************************************************************************************************
#* File:        ark_import.cmake
#* Brief:
#* User-facing ARK import entry point.
#*
#* Description:
#* This file is intended to be included by external projects using ARK.
#* It imports and initializes the Pico SDK and pulls in the ARK kernel
#* implementation. This file must be included exactly once.
#***************************************************************************************************

# Prevent double inclusion
if (DEFINED ARK_IMPORTED)
	return()
endif()
set(ARK_IMPORTED TRUE)

# Import Pico SDK
include(${CMAKE_CURRENT_LIST_DIR}/pico_sdk_import.cmake)
pico_sdk_init()

# Add ARK kernel
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/core)
