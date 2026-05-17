# US Golden Audio

These WAV files were generated during Phase 1 from
`tests/golden/input/us_one_shot.txt` using the Linux `dist/say` tool after a
default Autotools build.

- Language: US English (`-l us`)
- Speakers: `0` through `8`
- Encoding: WAV, PCM, 16-bit, mono, 11025 Hz

They are committed so future cleanup can compare fixed synthesis output against
the initial baseline.

`tools/baseline/compare_audio.py` can also write a TSV metrics report with
SHA-256, frame counts, peak/RMS, and max sample deltas. Exact SHA-256 equality
remains the pass/fail condition.
