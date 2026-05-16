#!/usr/bin/env bash

set -xe

error() {
    echo "[ERROR] - $1"; exit 1
}

name="$1"
target_dir="$(pwd -P)/$name"
current_date=$(date +"%F")

if [ -e "$target_dir" ]; then
    error "Directory already exists: $target_dir"
fi

# run
mkdir -p "$target_dir"; touch "$target_dir/run"
cat > "$target_dir/run" <<EOF
#!/usr/bin/env python3

from pathlib import Path
from datetime import datetime
from dataclasses import dataclass
from enum import Enum
import subprocess
import sys
import os

ROOT = Path(__file__).parent.resolve()
SRC_DIR = Path(f"{ROOT}/src")
PROJ_NAME = ROOT.name
PROJ_REPO = f"https://github.com/simon-danielsson/{PROJ_NAME}"
AUTH = "Simon Danielsson"
AUTH_CONT = "contact@simondanielsson.se"
C_STD = "gnu23"

AUTO_RUN = True

C_FLAGS_DEBUG = [
        "-g",
        "-O0",
        "-DDEBUG",
        "-fsanitize=address",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Wshadow",
        "-Werror=format-security",
        ]

C_FLAGS_RELEASE = ["-flto", "-O2", "-DNDEBUG", "-Wextra"]

DATE = f"{datetime.now().strftime("%Y-%m-%d")}"

# program ---------------------------------------------------------------------

def run_cmd(cmd) -> subprocess.CompletedProcess[str]:
    return subprocess.run(cmd, capture_output=True, text=True)

def get_git_vers() -> str:
    cmd = ["git", "describe", "--tags", "--abbrev=0"]
    result = run_cmd(cmd)
    if result.returncode != 0:
        return "v0.0.0"
    return result.stdout.strip()

def get_git_hash(short: bool) -> str:
    cmd = ["git", "rev-parse", "HEAD"]
    if short:
        cmd.insert(2, "--short")
    result = run_cmd(cmd)
    if result.returncode != 0:
        return "0".zfill(7)
    return result.stdout.strip()

GIT_V = get_git_vers()
GIT_HASH_SH = get_git_hash(True)
GIT_HASH = get_git_hash(False)

def help() -> None:
    bold = "\u001b[1m"
    reset = "\x1b[0m"
    print(f"{bold}run [r]elease{reset}")
    print(f"dest: ./build/release")
    print(f"{bold}run [d]ebug{reset}")
    print(f"dest: ./build/debug")
    print(f"{bold}run [t]est{reset}")
    print(f"dest: ./build/test")

class BuildType(Enum):
    RELEASE = "release"
    DEBUG = "debug"
    TEST = "test"

@dataclass
class Args:
    build: BuildType = BuildType.DEBUG
    help: bool = False

def get_args() -> Args:
    a: Args = Args()
    for arg in sys.argv:
        match arg:
            case r if r.startswith("r"):
                a.build = BuildType.RELEASE
            case d if d.startswith("d"):
                a.build = BuildType.DEBUG
            case t if t.startswith("t"):
                a.build = BuildType.TEST
            case h if h.startswith("h"):
                a.help = True
    return a

ENV_FLAGS = [
        f'-DENV_GITHASH="{GIT_HASH}"',
        f'-DENV_GITTAG="{GIT_V}"',
        f'-DENV_NAME="{PROJ_NAME}"',
        f'-DENV_AUTHOR="{AUTH}"',
        f'-DENV_CONTACT="{AUTH_CONT}"',
        f'-DENV_REPO="{PROJ_REPO}"',
        f"-std={C_STD}",
        ]

def collect_src_files(src: Path) -> list[str]:
    return [f"{path}" for path in src.rglob("*.c")]

def build(a: Args) -> None:
    build_dir = Path(f"{ROOT}/build/{a.build.value}")
    bin_name = f"{PROJ_NAME}_{a.build.value}_{DATE}_{GIT_V}_{GIT_HASH_SH}"
    c_flags: list[str] = ENV_FLAGS

    match a.build:
        case BuildType.DEBUG:
            c_flags = c_flags + C_FLAGS_DEBUG
        case BuildType.RELEASE:
            c_flags = c_flags + C_FLAGS_RELEASE
        case BuildType.TEST:
            c_flags.append("-DTEST")
            c_flags = c_flags + C_FLAGS_DEBUG

    os.makedirs(build_dir, exist_ok=True)

    build_cmd = c_flags + \\
            collect_src_files(SRC_DIR) + \\
            ["-o", f"{build_dir}/{bin_name}"]

    output = run_cmd(["gcc"] + build_cmd)
    if output.returncode != 0:
        run_cmd(["clang"] + build_cmd)

    if AUTO_RUN:
        auto_run_args = [f"{build_dir}/{bin_name}"]
        os.execvp(f"{build_dir}/{bin_name}", auto_run_args)

def main():
    a: Args = get_args()
    if a.help:
        help()
        return
    build(a)

if __name__ == "__main__":
    main()
EOF

chmod +x "$target_dir/run" || {
    error "Failed to make run script executable"
}

# generate README.md
touch "$target_dir/README.md"; echo "## $name" >> "$target_dir/README.md"

# generate main.h
mkdir -p "$target_dir/src"; touch "$target_dir/src/main.h"
cat > "$target_dir/src/main.h" <<EOF
#ifndef MAIN_H
#define MAIN_H

#if defined(NDEBUG)
#define BUILD_RELEASE 1
#define BUILD_DEBUG 0
#else
#define BUILD_RELEASE 0
#define BUILD_DEBUG 1
#endif

#if defined(TEST)
#define BUILD_TEST 1
#else
#define BUILD_TEST 0
#endif

// standard libraries ---------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// semantics ------------------------------------------------------------------

typedef size_t usize;
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

// suppress warnings ----------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// project variables ----------------------------------------------------------

#ifndef ENV_NAME // project name
#define ENV_NAME "UNDEFINED"
#endif
#ifndef ENV_AUTHOR // project author
#define ENV_AUTHOR "UNDEFINED"
#endif
#ifndef ENV_CONTACT // author contact
#define ENV_CONTACT "UNDEFINED"
#endif
#ifndef ENV_GITHASH // git version hash
#define ENV_GITHASH "UNDEFINED"
#endif
#ifndef ENV_GITTAG // git release tag
#define ENV_GITTAG "UNDEFINED"
#endif
#ifndef ENV_REPO // git repo
#define ENV_REPO "UNDEFINED"
#endif

// logging --------------------------------------------------------------------

#define LOG_STYLE "\x1b[1m"
#define LOG_RESET "\x1b[0m"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LOG_POS_DETAILS __FILE_NAME__, __LINE__, __func__
#define LOG_PREFIX "=>"
#define LOG_SEP "||"

#if defined(NDEBUG)
#define ASSERT(cond, do_abort) ((void)0)
#define LOG(fmt, ...) ((void)0)
#else
#define ASSERT(cond, do_abort)                                                 \
    do {                                                                         \
        if ((cond)) {                                                              \
            fprintf(stderr, "%s %sSUCCESS%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,       \
                    LOG_STYLE, LOG_RESET, LOG_SEP, #cond, LOG_POS_DETAILS);          \
        } else {                                                                   \
            fprintf(stderr, "%s %sFAILURE%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,       \
                    LOG_STYLE, LOG_RESET, LOG_SEP, #cond, LOG_POS_DETAILS);          \
        }                                                                          \
        if (!(cond) && (do_abort)) {                                               \
            abort();                                                                 \
        }                                                                          \
    } while (0)

#define LOG(fmt, ...)                                                          \
    do {                                                                         \
        fprintf(stderr, "%s %sLOG%s %s %s [%s:%d (%s)]\n", LOG_PREFIX, LOG_STYLE,  \
                LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,                \
                LOG_POS_DETAILS);                                                  \
    } while (0)

#endif

// not implemented (todo msg that aborts the program)
#define NOT_IMPL(fmt, ...)                                                     \
    do {                                                                         \
        fprintf(stderr, "%s %sNOT IMPL%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,        \
                LOG_STYLE, LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,     \
                LOG_POS_DETAILS);                                                  \
        abort();                                                                   \
    } while (0)

#define PANIC(fmt, ...)                                                        \
    do {                                                                         \
        fprintf(stderr, "%s %sPANIC%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,           \
                LOG_STYLE, LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,     \
                LOG_POS_DETAILS);                                                  \
        abort();                                                                   \
    } while (0)

#endif // MAIN_H
EOF

# generate main.c
mkdir -p "$target_dir/src"; touch "$target_dir/src/main.c"
cat > "$target_dir/src/main.c" <<EOF
#include "main.h"

i32 main(void) {
    printf("Hello, %s!\n", ENV_AUTHOR);
    return 0;
}
EOF

# generate license
touch "$target_dir/LICENSE"
cat > "$target_dir/LICENSE" <<EOF
Copyright © $(date +"%Y") Simon Danielsson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
EOF

# initalize git
cd "$target_dir"; touch "$target_dir/.gitignore"
cat > "$target_dir/.gitignore" <<EOF
nvim.log
/build
/build/*
.DS_Store
*.o
EOF

git init -b main
git add --all
git commit -m "init"
git tag v0.1.0


