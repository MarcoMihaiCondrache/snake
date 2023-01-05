#!/bin/bash

# Check if the terminal supports ANSI colors
if [ -t 1 ]; then
  ncolors=$(tput colors)

  if [ -n "$ncolors" ] && [ "$ncolors" -ge 8 ]; then
    # Terminal supports ANSI colors
    ansi_colors=true
  else
    # Terminal does not support ANSI colors
    ansi_colors=false
  fi
else
  # Standard output is not a terminal
  ansi_colors=false
fi

# Print whether the terminal supports ANSI colors or not
if ! $ansi_colors; then
  echo "YOUR TERMINAL DOES NOT SUPPORT COLORS PLEASE EDIT THE CONFIGURATION.H FILE"
fi

# Check if CMake is installed
if ! [ -x "$(command -v cmake)" ]; then
  echo 'Error: CMake is not installed.' >&2
  exit 1
fi

# Set the build directory
build_dir=release
source_dir=../

# Check if the build directory exists
if ! [ -d "$build_dir" ]; then
  # Create the build directory if it doesn't exist
  mkdir "$build_dir"
fi

# Change to the build directory
cd "$build_dir" || exit

# Run CMake and make
cmake -S $source_dir .. && make
