# Changelog

All notable changes to this project will be documented in this file.

## [1.1.2] - 2026-07-22
- Added check connection in init by reading id
- Updated doxygen documentation and cleaned up functions to static

## [1.1.0] - 2026-06-16
- Merged spi file into main file, modified variable type to use stdint.h types

## [1.0.4] - 2026-04-03
- Added WaitBusy function and added return error code during erase and write data

## [1.0.3] - 2025-10-03
- Renamed GPIO pins

## [1.0.2] - 2025-08-22
- Fixed bug in obj struct naming, changed name of conf struct to obj

## [1.0.1] - 2025-08-22
- Added NULL check of spi gpios (en, cs, wp, hold)

## [1.0.0] - 2025-04-23
- Added doxygen documentation for plib_sst26v_spi.h and plib_sst26v.h