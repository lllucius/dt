cmake_minimum_required(VERSION 3.16)

project(DECtalk VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(
  DECTALK_CMAKE_ENABLE_LEGACY_TARGETS
  "Allow configuring the initial CMake build on non-Linux legacy/non-current targets"
  OFF
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT DECTALK_CMAKE_ENABLE_LEGACY_TARGETS)
  message(FATAL_ERROR
    "The initial side-by-side CMake build is Linux-only. "
    "Set DECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON to experiment with non-current targets."
  )
endif()

include(CheckFunctionExists)
check_function_exists(iconv HAVE_ICONV)

configure_file(
  cmake/config.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/generated/config.h"
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/include/dtk")
configure_file(
  src/dapi/src/api/ttsapi.h
  "${CMAKE_CURRENT_BINARY_DIR}/generated/include/dtk/ttsapi.h"
  COPYONLY
)
configure_file(
  src/dapi/src/osf/dtmmedefs.h
  "${CMAKE_CURRENT_BINARY_DIR}/generated/include/dtk/dtmmedefs.h"
  COPYONLY
)

set(DECTALK_DAPI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/dapi/src")

set(DECTALK_CORE_SOURCES
  src/dapi/src/api/ttsapi.c
  src/dapi/src/api/decstd97.c
  src/dapi/src/api/init.c
  src/dapi/src/api/crypt2.c
  src/dapi/src/cmd/cm_char.c
  src/dapi/src/cmd/cm_main.c
  src/dapi/src/cmd/cm_text.c
  src/dapi/src/cmd/cm_pars.c
  src/dapi/src/cmd/cm_cmd.c
  src/dapi/src/cmd/cm_util.c
  src/dapi/src/cmd/cm_phon.c
  src/dapi/src/cmd/cm_copt.c
  src/dapi/src/cmd/par_pars.c
  src/dapi/src/cmd/par_ambi.c
  src/dapi/src/cmd/par_dict.c
  src/dapi/src/cmd/par_rule.c
  src/dapi/src/cmd/par_char.c
  src/dapi/src/cmd/cmd_wav.c
  src/dapi/src/cmd/cmd_init.c
  src/dapi/src/lts/lsa_adju.c
  src/dapi/src/lts/lsa_coni.c
  src/dapi/src/lts/lsa_fr.c
  src/dapi/src/lts/lsa_gr.c
  src/dapi/src/lts/lsa_ir.c
  src/dapi/src/lts/lsa_it.c
  src/dapi/src/lts/lsa_ja.c
  src/dapi/src/lts/lsa_rtbi.c
  src/dapi/src/lts/lsa_rule.c
  src/dapi/src/lts/lsa_sl.c
  src/dapi/src/lts/lsa_sp.c
  src/dapi/src/lts/lsa_task.c
  src/dapi/src/lts/lsa_us.c
  src/dapi/src/lts/lsa_util.c
  src/dapi/src/lts/lsw_main.c
  src/dapi/src/lts/ls_chari.c
  src/dapi/src/lts/ls_dict.c
  src/dapi/src/lts/ls_homo.c
  src/dapi/src/lts/ls_math.c
  src/dapi/src/lts/ls_proc.c
  src/dapi/src/lts/ls_spel.c
  src/dapi/src/lts/ls_speli.c
  src/dapi/src/lts/ls_suff.c
  src/dapi/src/lts/ls_suffi.c
  src/dapi/src/lts/loaddict.c
  src/dapi/src/ph/ph_aloph.c
  src/dapi/src/ph/ph_claus.c
  src/dapi/src/ph/ph_draw.c
  src/dapi/src/ph/ph_drwt0.c
  src/dapi/src/ph/ph_inton.c
  src/dapi/src/ph/ph_main.c
  src/dapi/src/ph/ph_romi.c
  src/dapi/src/ph/ph_setar.c
  src/dapi/src/ph/ph_sort.c
  src/dapi/src/ph/ph_syl.c
  src/dapi/src/ph/ph_syntx.c
  src/dapi/src/ph/ph_task.c
  src/dapi/src/ph/ph_timng.c
  src/dapi/src/ph/ph_vdefi.c
  src/dapi/src/ph/ph_vset.c
  src/dapi/src/ph/phinit.c
  src/dapi/src/ph/phlog.c
  src/dapi/src/ph/phprint.c
  src/dapi/src/hlsyn/acxf1c.c
  src/dapi/src/hlsyn/hlframe.c
  src/dapi/src/hlsyn/log10table.c
  src/dapi/src/hlsyn/brent.c
  src/dapi/src/hlsyn/inithl.c
  src/dapi/src/hlsyn/nasalf1x.c
  src/dapi/src/hlsyn/sqrttable.c
  src/dapi/src/hlsyn/circuit.c
  src/dapi/src/kernel/services.c
  src/dapi/src/kernel/usa_init.c
  src/dapi/src/nt/opthread.c
  src/dapi/src/nt/spc.c
  src/dapi/src/nt/pipe.c
  src/dapi/src/nt/linux_audio.c
  src/dapi/src/nt/playaud.c
  src/dapi/src/nt/mmalloc.c
  src/dapi/src/osf/loadable.c
  src/dapi/src/osf/dtmmio.c
  src/dapi/src/vtm/vtm.c
  src/dapi/src/vtm/vtmiont.c
  src/dapi/src/vtm/sync.c
  src/dapi/src/vtm/playtone.c
)

add_library(tts_us SHARED ${DECTALK_CORE_SOURCES})
set_target_properties(tts_us PROPERTIES
  OUTPUT_NAME tts_us
  POSITION_INDEPENDENT_CODE ON
)

target_include_directories(tts_us PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/generated"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/include"
  src
  src/dapi/src/api
  src/dapi/src/cmd
  src/dapi/src/dic
  src/dapi/src/hlsyn
  src/dapi/src/include
  src/dapi/src/kernel
  src/dapi/src/lts
  src/dapi/src/nt
  src/dapi/src/osf
  src/dapi/src/ph
  src/dapi/src/protos
  src/dapi/src/vtm
)

target_compile_definitions(tts_us PRIVATE
  _REENTRANT
  NOMME
  LTSSIM
  TTSSIM
  ANSI
  BLD_DECTALK_DLL
  ENGLISH
  ENGLISH_US
  ACNA
  ACCESS32
  TYPING_MODE
  DEC
  DECTALK_INSTALL_PREFIX="/opt/dectalk"
)

target_compile_options(tts_us PRIVATE
  -Wall
  -Wno-missing-braces
  -Wno-pointer-sign
  -Wno-unused-variable
  -Wno-unused-but-set-variable
  -Wno-unused-function
)

target_link_libraries(tts_us PRIVATE m pthread)

add_executable(say_cmake src/samplosf/src/dtsamples/say.c)
target_include_directories(say_cmake PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/generated"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/include"
  src
  src/dapi/src/api
  src/dapi/src/include
  src/dapi/src/nt
  src/dapi/src/ph
  src/dapi/src/vtm
)
target_compile_definitions(say_cmake PRIVATE
  _REENTRANT
  NOMME
  LTSSIM
  TTSSIM
  ANSI
  BLD_DECTALK_DLL
  ENGLISH
  ENGLISH_US
  ACNA
  ACCESS32
  TYPING_MODE
  DECTALK_INSTALL_PREFIX="/opt/dectalk"
)
target_compile_options(say_cmake PRIVATE
  -Wall
  -Wno-missing-braces
  -Wno-pointer-sign
  -Wno-unused-variable
  -Wno-unused-but-set-variable
  -Wno-unused-function
)
target_link_libraries(say_cmake PRIVATE tts_us m pthread)
