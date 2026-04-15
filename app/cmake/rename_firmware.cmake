# rename_firmware.cmake
#
# Invoked via:
#   cmake -DINPUT_FILE=<path/to/app.signed.bin>
#         -DOUTPUT_DIR=<destination_dir>
#         -P rename_firmware.cmake
#
# Reads INPUT_FILE, computes its CRC32 (ISO 3309 / gzip polynomial 0xEDB88320)
# and its size in bytes, then renames it to:
#   TRN91_00000_<CRC32_8HEX_UPPERCASE>_<FILESIZE_DECIMAL>.bin
# inside OUTPUT_DIR.

cmake_minimum_required(VERSION 3.21)

# ── Argument validation ────────────────────────────────────────────────────
if(NOT INPUT_FILE)
    message(FATAL_ERROR "INPUT_FILE is not set")
endif()
if(NOT EXISTS "${INPUT_FILE}")
    message(FATAL_ERROR "INPUT_FILE does not exist: ${INPUT_FILE}")
endif()
if(NOT OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR is not set")
endif()

# ── 1. File size (decimal bytes) ───────────────────────────────────────────
file(SIZE "${INPUT_FILE}" file_size)

# ── 2. CRC32 lookup table (polynomial 0xEDB88320, reflected) ──────────────
# Generate 256 entries: for each byte value i, apply 8 rounds of:
#   if LSB set → shift right then XOR polynomial, else just shift right.
# This is the same algorithm used by gzip / zlib / Ethernet FCS.
foreach(i RANGE 0 255)
    set(_e ${i})
    foreach(_bit RANGE 0 7)
        math(EXPR _lsb "${_e} & 1")
        math(EXPR _e   "${_e} >> 1")
        if(_lsb EQUAL 1)
            math(EXPR _e "${_e} ^ 0xEDB88320")
        endif()
    endforeach()
    set(CRC32_TABLE_${i} ${_e})
endforeach()

# ── 3. Compute CRC32 ──────────────────────────────────────────────────────
# Read the file as a hex string, then split into 2-char (1-byte) chunks via
# REGEX MATCHALL — a single C-level pass, far faster than substring-in-loop.
file(READ "${INPUT_FILE}" _hex HEX)
string(REGEX MATCHALL ".." _bytes "${_hex}")

set(_crc 0xFFFFFFFF)
foreach(_byte_hex IN LISTS _bytes)
    math(EXPR _byte "0x${_byte_hex}")
    math(EXPR _idx  "(${_crc} ^ ${_byte}) & 0xFF")
    math(EXPR _crc  "(${_crc} >> 8) ^ ${CRC32_TABLE_${_idx}}")
endforeach()
math(EXPR _crc "${_crc} ^ 0xFFFFFFFF")
math(EXPR _crc "${_crc} & 0xFFFFFFFF")   # safety mask to 32 bits

# ── 4. Format CRC as zero-padded 8-char uppercase hex ─────────────────────
math(EXPR _crc_hex "${_crc}" OUTPUT_FORMAT HEXADECIMAL)  # e.g. "0xabcd1234"
string(SUBSTRING "${_crc_hex}" 2 -1 _crc_str)            # strip "0x"
string(TOUPPER "${_crc_str}" _crc_str)
string(LENGTH  "${_crc_str}" _crc_len)
math(EXPR _pad "8 - ${_crc_len}")
if(_pad GREATER 0)
    string(REPEAT "0" ${_pad} _zeros)
    string(PREPEND _crc_str "${_zeros}")
endif()

# ── 5. Remove stale artifacts, then rename ────────────────────────────────
file(GLOB _stale "${OUTPUT_DIR}/TRN91_00000_*.bin")
foreach(_f IN LISTS _stale)
    file(REMOVE "${_f}")
endforeach()

set(_output_name "TRN91_00000_${_crc_str}_${file_size}.bin")
file(RENAME "${INPUT_FILE}" "${OUTPUT_DIR}/${_output_name}")
message(STATUS "Firmware artifact: ${_output_name}")
