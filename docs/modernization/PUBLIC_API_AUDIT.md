# Public API And Export Audit

This audit records the current Linux public header and exported-symbol
boundaries. It does not approve public API changes.

## Installed Headers

Autotools installs these headers under `include/dtk/`:

- `src/dapi/src/api/ttsapi.h`
- `src/dapi/src/osf/dtmmedefs.h`
- `src/dapi/src/include/l_all_ph.h`
- `src/dapi/src/include/l_com_ph.h`
- `src/dapi/src/include/l_fr_ph.h`
- `src/dapi/src/include/l_gr_ph.h`
- `src/dapi/src/include/l_la_ph.h`
- `src/dapi/src/include/l_sp_ph.h`
- `src/dapi/src/include/l_uk_ph.h`
- `src/dapi/src/include/l_us_ph.h`

`src/dapi/src/api/tts.h` is not installed by the current Linux Autotools install
target, but it remains ABI-sensitive internal API surface because it is included
by runtime sources and listed in modernization risk areas.

## Header Isolation

`tools/baseline/check_public_headers.sh` verifies the installed header list and
syntax-checks headers that currently self-compile on Linux:

- `ttsapi.h`
- `tts.h`
- `dtmmedefs.h`
- `l_com_ph.h`
- `l_fr_ph.h`

The remaining installed phoneme headers are inventoried but not forced through
the isolation check because they intentionally contain `#error` guards or
require voice-ROM selection macros. They should not be rewritten as part of this
audit.

## Exported Symbols

Exported symbols are versioned under `tests/golden/symbols/` and compared by
`tools/baseline/compare_symbols.sh`. Any difference in those files is
API/ABI-relevant until explicitly reviewed and accepted.

## Policy

- Do not reformat public headers.
- Do not change public function signatures, exported symbol names, structure
  layouts, constants, calling conventions, or installed header paths without an
  explicit API/ABI decision.
- Treat generated symbol comparisons and header audit comparisons as blocking
  checks for public API work.
