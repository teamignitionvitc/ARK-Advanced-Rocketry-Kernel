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
#* File:        CMakeLists.txt
#* Author:      Abhinav Kuruvila Joseph
#* Created On:  2025-12-29
#* Brief:
#* ARK kernel build definition.
#*
#* Description:
#* Defines the ARK kernel as a static library. This file does not perform
#* any SDK initialization or define executables. It is added to the build
#* exclusively through ark_import.cmake.
#***************************************************************************************************
#* HISTORY:
#* * +----- (NEW | MODify | ADD | DELete)
#* |
#* No#   |  Type  |       Date       | Author              | Description
#******+*********+*******************+*********************+***************************************
#* 000  |  NEW   |  2025-12-29        | Abhinav Kuruvila   | Initial creation
#**************************************************************************************************/

add_library(ark)

target_sources(ark
	PRIVATE
		ark.cpp
)

target_include_directories(ark
	PUBLIC
    	${CMAKE_CURRENT_LIST_DIR}
)

target_compile_features(ark
	PUBLIC
		cxx_std_17
)
