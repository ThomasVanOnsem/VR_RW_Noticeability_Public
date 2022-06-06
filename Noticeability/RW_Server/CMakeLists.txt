cmake_minimum_required(VERSION 3.21)
project(RW_Server)

set(CMAKE_CXX_STANDARD 17)

add_executable(RW_Server
        main.cpp
        Game.cpp Game.h
        Connection.cpp Connection.h
        Player.cpp Player.h
        algorithms/RWAlgorithm.h
        algorithms/APF.cpp algorithms/APF.h
        config.h config.cpp
        utils/utils.h utils/utils.cpp algorithms/Extrapolation.h algorithms/LinearExtrapolation.cpp algorithms/LinearExtrapolation.h utils/Logger.cpp utils/Logger.h)

if(WIN32)
    target_link_libraries(RW_Server wsock32 ws2_32)
endif()