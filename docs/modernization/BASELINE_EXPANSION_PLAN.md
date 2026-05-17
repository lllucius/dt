# Baseline Expansion Plan

This document records Phase 2 of the follow-on modernization plan. It
recommends additional deterministic baselines to add before higher-risk cleanup.
It does not accept new golden outputs and does not approve behavior changes.

## Inventory Commands

The inventory used the current post-merge Linux `dist/` tree and wrote probe
artifacts only under ignored `baseline-runs/next-phase2-inventory/`.

Useful command surfaces:

- `dist/say -h`
- `tools/baseline/capture_audio.sh --input FILE --out DIR`
- `tools/baseline/compare_audio.py --actual DIR --metrics-out FILE`
- `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out FILE`
- `tools/baseline/check_public_headers.sh --out DIR --expected tests/golden/public-headers`

Observed `say` support relevant to deterministic checks:

- language selection: `-l us,uk,gr,sp,la,fr`
- speaker selection: `-s 0` through `-s 9`
- fixed input file mode: `-fi FILE`
- WAV output file mode: `-fo FILE`
- output encodings through `-e`
- command prefix/postfix injection through `-pre` and `-post`
- virtual `stdout:raw` and `stdout:au` streams

`stdout:raw` is useful for focused experiments when redirected to a file, but it
is easy to misuse and emits binary data directly. WAV files remain the preferred
golden format because headers, sample rate, sample width, frame counts, and
hashes are already checked by `compare_audio.py`.

## Probe Results

Existing extra US input files all produced deterministic-looking WAV files for
speaker 0:

| Input | Probe file | Size |
| --- | --- | ---: |
| `tests/golden/input/us_abbreviations.txt` | `us_abbreviations_s0.wav` | 153,262 |
| `tests/golden/input/us_commands_markup.txt` | `us_commands_markup_s0.wav` | 118,472 |
| `tests/golden/input/us_punctuation_numbers.txt` | `us_punctuation_numbers_s0.wav` | 233,492 |

Each probe was RIFF/WAVE PCM, 16-bit, mono, 11025 Hz.

Installed non-US languages also produced WAV files for speaker 0 using the
existing one-shot text:

| Language | Probe file | Size |
| --- | --- | ---: |
| UK English | `uk_one_shot_s0.wav` | 141,334 |
| French | `fr_one_shot_s0.wav` | 142,754 |
| German | `gr_one_shot_s0.wav` | 168,172 |
| Castilian Spanish | `sp_one_shot_s0.wav` | 151,842 |
| Latin American Spanish | `la_one_shot_s0.wav` | 145,026 |

Each non-US probe was also RIFF/WAVE PCM, 16-bit, mono, 11025 Hz. The text was
US English, so these probes prove command-path viability but are not ideal
language-quality baselines.

## Recommendations

Recommended for Phase 3, with extra-high approval before committing outputs:

1. Add multi-input US English WAV baselines for the already committed input
   files:
   - `us_abbreviations.txt`
   - `us_commands_markup.txt`
   - `us_punctuation_numbers.txt`
2. Generate those US baselines for speakers 0 through 8, matching the existing
   one-shot speaker coverage.
3. Extend `tools/baseline/capture_audio.sh` and `compare_audio.py` only as much
   as needed to support named audio suites, for example:
   `tests/golden/audio/us/<input-name>/speaker_<n>.wav`.
4. Add a committed detailed Autotools packaging manifest baseline using
   `metadata-hash` format so packaging work can be compared against a stable
   accepted layout.
5. Add a small public API smoke test in a later phase before API-boundary
   warning cleanup. The smoke test should compile against staged installed
   headers and libraries, use file output instead of live audio, and compare its
   WAV output against an accepted deterministic baseline.

Recommended to defer:

- Non-US WAV baselines until language-specific fixed input texts are chosen and
  approved. The command path works, but US English text is not a good
  long-term baseline for non-US language behavior.
- Phoneme or text-mode output baselines until a stable, separate textual output
  path is identified. `[:phoneme on]` is supported as command text, but this
  phase did not identify a clean no-audio textual capture path suitable for CI.
- Raw stdout audio baselines. They can be redirected for experiments, but WAV
  baselines are safer and already carry format metadata.

## CI Cost

The recommended US expansion would add 27 WAV comparisons beyond the current
nine. The generated files are short and should be acceptable for CI after
approval. If CI time becomes a concern, keep all speakers for `us_one_shot.txt`
and use speaker 0 for the additional parser-focused inputs.

## Approval Boundary

No files under `tests/golden/audio/`, `tests/golden/dictionaries/`, or
`tests/golden/symbols/` were changed in Phase 2. Accepting new golden outputs
belongs to Phase 3 and requires the extra-high checkpoint in `PLAN.md`.
