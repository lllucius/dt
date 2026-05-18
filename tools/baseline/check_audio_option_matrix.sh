#!/usr/bin/env bash
# Purpose: capture CMake and Autotools audio-option metadata without opening
# live audio devices.
# Scope: CMake dt_platform_smoke builds and Autotools configure probes only.
# Behavior preservation: this script does not update accepted baselines, run
# live audio playback, or change runtime audio routing.
# Limitations: non-default option rows are compile-only or metadata-only; they
# do not certify live ALSA, PulseAudio, OSS, or AudioQueue behavior.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/check_audio_option_matrix.sh [--run-dir DIR]

Captures audio-option build metadata for:
  CMake default
  CMake DECTALK_CMAKE_DISABLE_AUDIO=ON
  CMake DECTALK_CMAKE_USE_ALSA=ON
  CMake DECTALK_CMAKE_USE_PULSEAUDIO=ON
  Autotools default configure
  Autotools --disable-audio configure
  Autotools --disable-pulseaudio configure

The CMake probes build and run dt_platform_smoke only. The Autotools probes run
configure only and capture generated audio macro/link metadata. No probe opens a
live audio device.
USAGE
}

run_dir=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --run-dir)
      if [ "$#" -lt 2 ]; then
        echo "error: --run-dir requires a directory" >&2
        exit 2
      fi
      run_dir="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "error: unknown option: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if [ -z "$run_dir" ]; then
  run_dir="$repo_root/baseline-runs/audio-option-matrix"
elif [[ "$run_dir" != /* ]]; then
  run_dir="$repo_root/$run_dir"
fi

mkdir -p "$run_dir"

cc="${CC:-/usr/bin/gcc}"
if [ ! -x "$cc" ]; then
  cc="${CC:-gcc}"
fi

{
  uname -a
  "$cc" --version | sed -n '1p'
  cmake --version | sed -n '1p'
  pkg-config --modversion alsa 2>/dev/null || printf 'alsa: not found\n'
  pkg-config --modversion libpulse-simple 2>/dev/null || printf 'libpulse-simple: not found\n'
} > "$run_dir/tool-versions.txt"

write_args() {
  if [ "$#" -eq 0 ]; then
    printf '(default)'
  else
    printf '%s ' "$@"
  fi
}

run_cmake_probe() {
  probe_name="$1"
  shift
  probe_dir="$run_dir/cmake-$probe_name"
  build_dir="$probe_dir/build"
  mkdir -p "$probe_dir"

  CC="$cc" cmake -S "$repo_root" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release "$@" \
    > "$probe_dir/configure.log" 2>&1
  cmake --build "$build_dir" --target dt_platform_smoke -- -j1 \
    > "$probe_dir/build.log" 2>&1
  "$build_dir/dt_platform_smoke" "$repo_root" PLAN.md \
    > "$probe_dir/dt-platform-smoke.log" 2>&1

  {
    printf 'probe=cmake-%s\n' "$probe_name"
    printf 'classification=metadata-only\n'
    printf 'cmake_args='
    write_args "$@"
    printf '\n'
    printf 'compile_commands_lines=%s\n' "$(wc -l < "$build_dir/compile_commands.json")"
    awk '/^audio_/ { print }' "$probe_dir/dt-platform-smoke.log"
  } > "$probe_dir/summary.txt"
}

run_autotools_probe() {
  probe_name="$1"
  shift
  probe_dir="$run_dir/autotools-$probe_name"
  mkdir -p "$probe_dir"

  (cd "$repo_root/src" && CC="$cc" ./configure "$@") \
    > "$probe_dir/configure.log" 2>&1

  {
    printf 'probe=autotools-%s\n' "$probe_name"
    printf 'classification=configure-metadata-only\n'
    printf 'configure_args='
    write_args "$@"
    printf '\n'
    printf '[src/dapi/src/nt/Makefile]\n'
    awk '/^(AUDIO_DEFINES|LINUX_AUDIO)[ \t]*=/' \
      "$repo_root/src/dapi/src/nt/Makefile"
    printf '[src/dapi/src/Makefile]\n'
    awk '/^(DAPI_LD_SHARED|LINK_LINUX_AUDIO)[ \t]*=/' \
      "$repo_root/src/dapi/src/Makefile"
    printf '[src/samplosf/src/dtsamples/Makefile]\n'
    awk '/^(AUDIO_DEFINES|AUDIO_LIBS)[ \t]*=/' \
      "$repo_root/src/samplosf/src/dtsamples/Makefile"
  } > "$probe_dir/summary.txt"
}

run_cmake_probe default
run_cmake_probe disable-audio -DDECTALK_CMAKE_DISABLE_AUDIO=ON
run_cmake_probe use-alsa -DDECTALK_CMAKE_USE_ALSA=ON
run_cmake_probe use-pulseaudio -DDECTALK_CMAKE_USE_PULSEAUDIO=ON

run_autotools_probe disable-audio --disable-audio
run_autotools_probe disable-pulseaudio --disable-pulseaudio
run_autotools_probe default

{
  printf 'probe\tclassification\tsummary\n'
  printf 'cmake-default\tmetadata-only\t%s\n' "$run_dir/cmake-default/summary.txt"
  printf 'cmake-disable-audio\tmetadata-only\t%s\n' "$run_dir/cmake-disable-audio/summary.txt"
  printf 'cmake-use-alsa\tmetadata-only\t%s\n' "$run_dir/cmake-use-alsa/summary.txt"
  printf 'cmake-use-pulseaudio\tmetadata-only\t%s\n' "$run_dir/cmake-use-pulseaudio/summary.txt"
  printf 'autotools-disable-audio\tconfigure-metadata-only\t%s\n' "$run_dir/autotools-disable-audio/summary.txt"
  printf 'autotools-disable-pulseaudio\tconfigure-metadata-only\t%s\n' "$run_dir/autotools-disable-pulseaudio/summary.txt"
  printf 'autotools-default\tconfigure-metadata-only\t%s\n' "$run_dir/autotools-default/summary.txt"
} > "$run_dir/matrix.tsv"

cat "$run_dir/matrix.tsv"
