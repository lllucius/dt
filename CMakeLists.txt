cmake_minimum_required(VERSION 3.16)

project(DECtalk VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(GNUInstallDirs)

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
set(DECTALK_CMAKE_STAGE_DIR
  "${CMAKE_CURRENT_BINARY_DIR}/cmake-dist"
  CACHE PATH "CMake staging directory used by the side-by-side DECtalk build"
)

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

set(DECTALK_COMMON_INCLUDE_DIRS
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

set(DECTALK_COMMON_DEFINES
  _REENTRANT
  NOMME
  LTSSIM
  TTSSIM
  ANSI
  BLD_DECTALK_DLL
  ACCESS32
  TYPING_MODE
  DECTALK_INSTALL_PREFIX="/opt/dectalk"
)

set(DECTALK_COMMON_COMPILE_OPTIONS
  -Wall
  -Wno-missing-braces
  -Wno-pointer-sign
  -Wno-unused-variable
  -Wno-unused-but-set-variable
  -Wno-unused-function
)

function(dectalk_apply_common_c_settings target_name)
  target_include_directories(${target_name} PRIVATE ${DECTALK_COMMON_INCLUDE_DIRS})
  target_compile_definitions(${target_name} PRIVATE ${DECTALK_COMMON_DEFINES})
  target_compile_options(${target_name} PRIVATE ${DECTALK_COMMON_COMPILE_OPTIONS})
endfunction()

add_library(dt_platform STATIC
  src/platform/dt_filesystem_posix.c
  src/platform/dt_time_posix.c
)
target_include_directories(dt_platform PUBLIC src/platform)
target_compile_options(dt_platform PRIVATE ${DECTALK_COMMON_COMPILE_OPTIONS})

add_executable(dt_platform_smoke tools/platform/dt_platform_smoke.c)
target_compile_options(dt_platform_smoke PRIVATE ${DECTALK_COMMON_COMPILE_OPTIONS})
target_link_libraries(dt_platform_smoke PRIVATE dt_platform)

set(DECTALK_TTS_TARGETS)

function(dectalk_add_tts_language lang_code)
  add_library(tts_${lang_code} SHARED ${DECTALK_CORE_SOURCES})
  set_target_properties(tts_${lang_code} PROPERTIES
    OUTPUT_NAME tts_${lang_code}
    POSITION_INDEPENDENT_CODE ON
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
  )
  dectalk_apply_common_c_settings(tts_${lang_code})
  target_compile_definitions(tts_${lang_code} PRIVATE DEC ${ARGN})
  target_link_options(tts_${lang_code} PRIVATE
    -Wl,-Bsymbolic
    -Wl,-soname,libtts_${lang_code}.so
  )
  target_link_libraries(tts_${lang_code} PRIVATE m pthread)
  list(APPEND DECTALK_TTS_TARGETS tts_${lang_code})
  set(DECTALK_TTS_TARGETS "${DECTALK_TTS_TARGETS}" PARENT_SCOPE)
endfunction()

dectalk_add_tts_language(us ENGLISH ENGLISH_US ACNA)
dectalk_add_tts_language(uk ENGLISH ENGLISH_UK)
dectalk_add_tts_language(sp SPANISH SPANISH_SP)
dectalk_add_tts_language(gr GERMAN)
dectalk_add_tts_language(la SPANISH SPANISH_LA)
dectalk_add_tts_language(fr FRENCH)

add_library(tts_multi SHARED
  src/dtalkml/src/dtalk_ml.c
  src/dtalkml/src/init.c
)
set_target_properties(tts_multi PROPERTIES
  OUTPUT_NAME tts
  POSITION_INDEPENDENT_CODE ON
  BUILD_RPATH "$ORIGIN"
  INSTALL_RPATH "$ORIGIN"
)
target_include_directories(tts_multi PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/generated"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/include"
  src
  src/dapi/src/api
  src/dapi/src/cmd
  src/dapi/src/dic
  src/dapi/src/include
  src/dapi/src/lts
  src/dapi/src/nt
  src/dapi/src/osf
  src/dapi/src/ph
  src/dapi/src/protos
)
target_compile_definitions(tts_multi PRIVATE
  ${DECTALK_COMMON_DEFINES}
  ENGLISH
  ENGLISH_US
  ACNA
)
target_compile_options(tts_multi PRIVATE ${DECTALK_COMMON_COMPILE_OPTIONS})
target_link_options(tts_multi PRIVATE
  -Wl,-Bsymbolic
  -Wl,-soname,libtts.so
)
target_link_libraries(tts_multi PRIVATE dl pthread m)

set(DECTALK_DICTIONARY_OUTPUTS)

function(dectalk_add_dictionary_compiler lang_code)
  add_executable(dic_${lang_code} src/dapi/src/dic/dic.c)
  dectalk_apply_common_c_settings(dic_${lang_code})
  target_compile_definitions(dic_${lang_code} PRIVATE ${ARGN})

  set(dictionary_names ${lang_code} ${lang_code}_5)
  if(lang_code STREQUAL "us")
    list(APPEND dictionary_names us_1997 us_s_1997 us_s_1998 us_2001 us_2002)
  endif()

  set(lang_outputs)
  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/dictionaries/${lang_code}")
  foreach(dictionary_name IN LISTS dictionary_names)
    set(input_file "${CMAKE_CURRENT_SOURCE_DIR}/src/dapi/src/dic/Dic_${dictionary_name}.txt")
    set(output_relative "dictionaries/${lang_code}/dtalk_${dictionary_name}.dic")
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${output_relative}")
    add_custom_command(
      OUTPUT "${output_file}"
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${output_dir}"
      COMMAND "$<TARGET_FILE:dic_${lang_code}>" "${input_file}" "${output_relative}" /t:win32
      DEPENDS dic_${lang_code} "${input_file}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      VERBATIM
    )
    list(APPEND lang_outputs "${output_file}")
  endforeach()

  add_custom_target(dictionaries_${lang_code} DEPENDS ${lang_outputs})
  list(APPEND DECTALK_DICTIONARY_OUTPUTS ${lang_outputs})
  set(DECTALK_DICTIONARY_OUTPUTS "${DECTALK_DICTIONARY_OUTPUTS}" PARENT_SCOPE)
endfunction()

dectalk_add_dictionary_compiler(us ENGLISH ENGLISH_US ACNA)
dectalk_add_dictionary_compiler(uk ENGLISH ENGLISH_UK)
dectalk_add_dictionary_compiler(sp SPANISH SPANISH_SP)
dectalk_add_dictionary_compiler(gr GERMAN)
dectalk_add_dictionary_compiler(la SPANISH SPANISH_LA)
dectalk_add_dictionary_compiler(fr FRENCH)

add_custom_target(cmake_dictionaries ALL DEPENDS ${DECTALK_DICTIONARY_OUTPUTS})

add_executable(say_cmake src/samplosf/src/dtsamples/say.c)
set_target_properties(say_cmake PROPERTIES
  OUTPUT_NAME say
  BUILD_RPATH "$ORIGIN;$ORIGIN/lib;$ORIGIN/../lib"
  INSTALL_RPATH "$ORIGIN/lib;$ORIGIN/../lib"
)
dectalk_apply_common_c_settings(say_cmake)
target_compile_definitions(say_cmake PRIVATE
  ENGLISH
  ENGLISH_US
  ACNA
)
target_link_libraries(say_cmake PRIVATE tts_multi m pthread)

set(DECTALK_PHONEME_HEADERS
  src/dapi/src/include/l_all_ph.h
  src/dapi/src/include/l_com_ph.h
  src/dapi/src/include/l_fr_ph.h
  src/dapi/src/include/l_gr_ph.h
  src/dapi/src/include/l_la_ph.h
  src/dapi/src/include/l_sp_ph.h
  src/dapi/src/include/l_uk_ph.h
  src/dapi/src/include/l_us_ph.h
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/DECtalk.conf"
"licenses:YV808w008IQ0\n"
"licensepassword:kw60P7Y0bGRNB1@W3L@\n"
"US_dict:dic/dtalk_us.dic\n"
"UK_dict:dic/dtalk_uk.dic\n"
"SP_dict:dic/dtalk_sp.dic\n"
"GR_dict:dic/dtalk_gr.dic\n"
"LA_dict:dic/dtalk_la.dic\n"
"FR_dict:dic/dtalk_fr.dic\n"
"US_udict:udict_us.dic\n"
"UK_udict:udict_uk.dic\n"
"SP_udict:udict_sp.dic\n"
"GR_udict:udict_gr.dic\n"
"LA_udict:udict_la.dic\n"
"FR_udict:udict_fr.dic\n"
"Speak_xpm_dir:bitmaps/\n"
"Default_lang:us\n"
"LANG:us,US English\n"
"LANG:uk,UK English\n"
"LANG:sp,Spanish\n"
"LANG:gr,German\n"
"LANG:la,Latin American\n"
"LANG:fr,French\n"
)

install(TARGETS ${DECTALK_TTS_TARGETS} tts_multi LIBRARY DESTINATION lib)
install(TARGETS say_cmake RUNTIME DESTINATION .)
install(TARGETS dic_us dic_uk dic_sp dic_gr dic_la dic_fr RUNTIME DESTINATION tools)
install(FILES ${DECTALK_DICTIONARY_OUTPUTS} DESTINATION dic)
install(FILES
  src/dapi/src/api/ttsapi.h
  src/dapi/src/osf/dtmmedefs.h
  ${DECTALK_PHONEME_HEADERS}
  DESTINATION include/dtk
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/generated/DECtalk.conf" DESTINATION .)

add_custom_target(dectalk_cmake_stage
  COMMAND "${CMAKE_COMMAND}" --install "${CMAKE_CURRENT_BINARY_DIR}" --prefix "${DECTALK_CMAKE_STAGE_DIR}"
  DEPENDS
    ${DECTALK_TTS_TARGETS}
    tts_multi
    say_cmake
    dic_us dic_uk dic_sp dic_gr dic_la dic_fr
    cmake_dictionaries
)
