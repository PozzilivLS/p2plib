cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("p2plib")

include(FetchContent)

FetchContent_Declare(
  packetslib
  GIT_REPOSITORY https://github.com/PozzilivLS/NetSocketsLib.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(packetslib)

#add_executable (p2plib 
#	"src/main.cpp"
#	"src/constants.h"
#	"src/server/server.h"
#	"src/server/server.cpp"
#	"src/client/client.h"
#	"src/client/client.cpp"
#	"src/network.h"
#	"src/network.cpp"
#	"src/utils.h"
# "src/server/servermaster.h" "src/server/servermaster.cpp" "src/client/clientmaster.h" "src/client/clientmaster.cpp")

add_library (p2plib
	"src/constants.h"
	"src/server/server.h"
	"src/server/server.cpp"
	"src/client/client.h"
	"src/client/client.cpp"
	"src/network.h"
	"src/network.cpp"
	"src/utils.h"
 "src/server/servermaster.h" "src/server/servermaster.cpp" "src/client/clientmaster.h" "src/client/clientmaster.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET p2plib PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(p2plib PUBLIC "${PACKETS_ROOT}" REQUIRED)

target_link_libraries(p2plib PUBLIC
  packetslib
)
if (WIN32)
    target_link_libraries(p2plib PUBLIC ws2_32)
endif()

install(FILES src/server/servermaster.h src/client/clientmaster.h DESTINATION include)
