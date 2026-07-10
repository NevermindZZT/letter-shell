#!/usr/bin/env bash
# Version: 2.7
# Date: 2023-05-22
# This bash script generates a CMSIS Software Pack.

set -o pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set version of gen pack library.
# For available versions see https://github.com/Open-CMSIS-Pack/gen-pack/tags.
# Use the tag name without the prefix "v", e.g., 0.7.0.
REQUIRED_GEN_PACK_LIB="0.8.4"

# Set default command line arguments.
DEFAULT_ARGS=(-c "v")

# Pack warehouse directory - destination.
# Default: ./output
#
# PACK_OUTPUT=./output

# Temporary pack build directory.
# Default: ./build
#
# PACK_BUILD=./build

# Specify directory names to be added to pack base directory.
# An empty list defaults to all folders next to this script.
# Default: empty (all folders)
#
 PACK_DIRS="
   src
   extensions
   doc
   templates
 "

# Specify file names to be added to pack base directory.
# Default: empty
#
 PACK_BASE_FILES="
   LICENSE
   README.md
 "

# Specify file names to be deleted from pack build directory.
# Default: empty
#
# PACK_DELETE_FILES="
# "

# Specify patches to be applied.
# Default: empty
#
# PACK_PATCH_FILES="
#     <list patches here>
# "

# Specify additional arguments to packchk.
# Default: empty
#
# PACKCHK_ARGS=()

# Specify additional dependencies for packchk.
# Default: empty
#
# PACKCHK_DEPS="
#   <list pdsc files here>
# "

# Optional: restrict fallback modes for changelog generation.
# Default: full
# Values:
# - full      Tag annotations, release descriptions, or commit messages (in order)
# - release   Tag annotations, or release descriptions (in order)
# - tag       Tag annotations only
#
# PACK_CHANGELOG_MODE="<full|release|tag>"

#
# custom pre-processing steps
#
# usage: preprocess <build>
#   <build>  The build folder
#
function preprocess() {
  return 0
}

#
# custom post-processing steps
#
# usage: postprocess <build>
#   <build>  The build folder
#
function postprocess() {
  local build="$1"
  local keep_list
  local pack_path
  local pack_rel

  keep_list="$(git -C "${REPO_ROOT}" ls-files --cached --others --exclude-standard -- src extensions doc templates LICENSE README.md fool_cat.letter_shell.pdsc)"

  while IFS= read -r -d '' pack_path; do
    pack_rel="${pack_path#${build}/}"
    case "${pack_rel}" in
      fool_cat.letter_shell.sha1)
        continue
        ;;
    esac

    if grep -Fxq -- "${pack_rel}" <<< "${keep_list}"; then
      continue
    fi

    rm -f -- "${pack_path}"
  done < <(find "${build}" -type f -print0)

  find "${build}" -depth -type d -empty -delete
  return 0
}

############ DO NOT EDIT BELOW ###########

function install_lib() {
  local URL="https://github.com/Open-CMSIS-Pack/gen-pack/archive/refs/tags/v$1.tar.gz"
  local STATUS
  STATUS=$(curl -sLI "${URL}" | grep "^HTTP" | tail -n 1 | cut -d' ' -f2 || echo "$((600+$?))")
  if [[ $STATUS -ge 400 ]]; then
    echo "Wrong/unavailable gen-pack lib version '$1'!" >&2
    echo "Check REQUIRED_GEN_PACK_LIB variable." >&2
    echo "For available versions see https://github.com/Open-CMSIS-Pack/gen-pack/tags." >&2
    exit 1
  fi
  echo "Downloading gen-pack lib version '$1' to '$2' ..."
  mkdir -p "$2"
  curl -L "${URL}" -s | tar -xzf - --strip-components 1 -C "$2" || exit 1
}

function load_lib() {
  if [[ -d ${GEN_PACK_LIB} ]]; then
    . "${GEN_PACK_LIB}/gen-pack"
    return 0
  fi
  local GLOBAL_LIB="/usr/local/share/gen-pack/${REQUIRED_GEN_PACK_LIB}"
  local USER_LIB="${HOME}/.local/share/gen-pack/${REQUIRED_GEN_PACK_LIB}"
  if [[ ! -d "${GLOBAL_LIB}" && ! -d "${USER_LIB}" ]]; then
    echo "Required gen_pack lib not found!" >&2
    install_lib "${REQUIRED_GEN_PACK_LIB}" "${USER_LIB}"
  fi

  if [[ -d "${GLOBAL_LIB}" ]]; then
    . "${GLOBAL_LIB}/gen-pack"
  elif [[ -d "${USER_LIB}" ]]; then
    . "${USER_LIB}/gen-pack"
  else
    echo "Required gen-pack lib is not installed!" >&2
    exit 1
  fi
}

load_lib
gen_pack "${DEFAULT_ARGS[@]}" "$@"

exit 0
