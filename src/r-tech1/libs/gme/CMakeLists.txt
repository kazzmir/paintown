# -------------------------------------------------------
# GME cmake build script for r-tech1.
# Written by: juvinious
# -------------------------------------------------------
project (gme C CXX)

# -------------------------------------------------------
# Source directories containing all the necessary .cpp files
# -------------------------------------------------------
# GME Library
set(GME_SRC
${CMAKE_CURRENT_SOURCE_DIR}/Ay_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ay_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ay_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Blip_Buffer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Classic_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Data_Reader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Dual_Resampler.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Effects_Buffer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Fir_Resampler.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gb_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gb_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gb_Oscs.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gbs_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/gme.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gme_File.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Gym_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Hes_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Hes_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Hes_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Kss_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Kss_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Kss_Scc_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/M3u_Playlist.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Multi_Buffer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Music_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Fme7_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Namco_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Oscs.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nes_Vrc6_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nsfe_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Nsf_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sap_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sap_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sap_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Sms_Apu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Snes_Spc.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Spc_Cpu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Spc_Dsp.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Spc_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Spc_Filter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Vgm_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Vgm_Emu_Impl.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ym2413_Emu.cpp
${CMAKE_CURRENT_SOURCE_DIR}/Ym2612_Emu.cpp
PARENT_SCOPE)

# -------------------------------------------------------
# module
# -------------------------------------------------------
#add_library (gme_module ${GME_SRC})


# Install
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include/r-tech1/libs/gme)
