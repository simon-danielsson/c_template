#!/usr/bin/env python3

# Copyright © 2026 Simon Danielsson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files, to deal in the Software
# without restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies of the
# Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

"""

- bark.py -

A minimal snapshot testing tool to harden the bark of your code.

Built by Simon Danielsson

Source: https://github.com/simon-danielsson/bark.py
Author: https://www.simondanielsson.se/

Requirements: Python 3.10+

"""

import sys, subprocess, os, shutil, hashlib
from datetime import datetime as dt
from dataclasses import dataclass
from pathlib import Path

GLBL_START_TIME = dt.now()

CWD = "."
BARK_TEST = f"{CWD}/bark_test"
BARK_DIR = f"{CWD}/.bark"
HASH = f"{BARK_DIR}/hash"

FIELD = "▍"
DIV = "┈" * 60
COL_RED = "\033[1;31m"
COL_GREEN = "\033[1;32m"
COL_BLUE = "\033[1;34m"
COL_RESET = "\033[0m"

_HELP_STR = """
Usage: ./bark.py <flag|cmd>

-h, --help
    * Show help.

record
    * Reads a file 'bark_test' with shell commands to be executed.
    * Executes each command and saves their respective stdout/err to a file
      inside a generated directory '.bark'.

compare
    * Runs the same file of shell commands again and compares their
      stdout/err to their recorded counterparts in the '.bark' directory.
    * Prints a summary."""

def msg_info(s: str):
    """debug"""
    print(f"{COL_BLUE}INFO{COL_RESET}     {s}")

def msg_succ(s: str):
    """debug"""
    print(f"{COL_GREEN}SUCCESS{COL_RESET}  {s}")

def msg_error(s: str, quit: bool = True, details: bool = True) -> None:
    d = " -- use 'bark.py -h' for more details" if details else ""
    print(f"{COL_RED}FAILURE{COL_RESET}  {s}{d}")
    sys.exit(1) if quit else ()

@dataclass
class Test:
    shell_cmd: list[str]
    name: str
    id: int
    stdout: str = ""

    def shell_cmd_as_str(self) -> str:
        return " ".join(self.shell_cmd).strip()

    def launch_cmd(self, debug_print: bool) -> None:
        """helper - cmd_record()"""
        try:
            if debug_print:
                msg_info(f"processing '{self.name}'...")
            self.stdout = subprocess.run(
                    shell=True,
                    args=self.shell_cmd_as_str(),
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    ).stdout
        except FileNotFoundError:
            msg_error(f"file not found '{self.shell_cmd_as_str()}'")
        except subprocess.CalledProcessError as e:
            msg_error(
                    f"failed to execute '{self.shell_cmd_as_str()}': {e}",
                    )

def read_file(file: str | Path) -> list[str]:
    try:
        f = open(file, "r").readlines()
    except OSError:
        msg_error(
                f"failed to open '{file}' (must be inside working dir)",
                details=False,
                )
    return f

def retrieve_old_tests() -> list[Test]:
    if not os.path.exists(BARK_DIR):
        msg_error(f"dir '{BARK_DIR}' doesn't exist")

    tests = []
    for child in Path(BARK_DIR).iterdir():
        if child.is_file():
            if child.name == "hash":
                continue
            file = read_file(child)
            tests.append(
                    Test(
                        shell_cmd=[],
                        name=file[0][:-1],
                        stdout="".join(file[-1:]),
                        id=int(child.name),
                        )
                    )
    return tests

def retrieve_new_tests() -> list[Test]:
    """helper - cmd_record()"""
    f = read_file(BARK_TEST)
    tests: list[Test] = []
    for i, l in enumerate(f):
        name, command = l.split("|")
        test_cmd = command[:-1].split(" ")
        tests.append(Test(shell_cmd=test_cmd, name=name.strip(), id=i))
    return tests

def generate_hash_from_file(file: str) -> str:
    with open(file, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()

def _compare_hash() -> None:
    """checks if commands to be ran are the same as prev. recorded"""
    current_hash = generate_hash_from_file(BARK_TEST)
    with open(HASH, "r", encoding="utf-8") as f:
        old_hash = f.read().strip()
    if current_hash != old_hash:
        msg_error(f"'{BARK_TEST}' has changed since last recording", quit=True)

def store_test_results(tests: list[Test]) -> None:
    """helper - cmd_record()"""
    if os.path.exists(BARK_DIR):
        msg_info("overwriting old snapshot...")
        shutil.rmtree(BARK_DIR)
    os.makedirs(BARK_DIR)
    for t in tests:
        with open(f"{BARK_DIR}/{t.id}", "w", encoding="utf-8") as f:
            f.write(f"{t.name}\n")
            f.write(t.stdout)
    with open(f"{HASH}", "w", encoding="utf-8") as f:
        f.write(generate_hash_from_file(BARK_TEST))

def cmd_record() -> None:
    """helper - cmd_record()"""
    tests = retrieve_new_tests()
    [t.launch_cmd(debug_print=True) for t in tests]
    store_test_results(tests)
    msg_succ("new snapshot written successfully")

def cmd_help() -> None:
    print(_HELP_STR[1:])

def comparison_failure_print(name: str, old_line: str, new_line: str) -> None:
    """helper - cmd_compare()"""
    l = f"{COL_RED}{FIELD}{COL_RESET}"
    ol = old_line[:-1] if old_line[-1] == "\n" else old_line
    nl = new_line[:-1] if new_line[-1] == "\n" else new_line
    print(f"{COL_RED}{FIELD}FAILURE {COL_RESET}'{name}'")
    print(f"{l}")
    print(f"{l:<15}expected => '{ol}'")
    print(f"{l:<15}actual   => '{nl}'")
    print(f"{l}")

def time_total() -> float:
    now = dt.now()
    return (now - GLBL_START_TIME).total_seconds()

def compare_results_table(results: list[tuple[bool, Test]]) -> None:
    """helper - cmd_compare()"""
    print(DIV)

    failures = 0
    for failed, _ in results:
        failures += 1 if failed else 0
    success_rate = 100 if failures == 0 else failures / len(results) * 100

    for failed, test in results:
        status = f"{COL_RED}{FIELD}F" if failed else f"{COL_GREEN}S"
        print(f"{status:<10} {test.name:<26}{COL_RESET}{test.shell_cmd_as_str()}")
    print(f"\nSuccess rate  : {success_rate}%")
    print(f"Total time    : {time_total():.4} sec")
    print(DIV)

def cmd_compare() -> None:
    new_tests = retrieve_new_tests()
    old_tests = retrieve_old_tests()
    [nt.launch_cmd(debug_print=False) for nt in new_tests]

    results: list[tuple[bool, Test]] = []

    for n, o in zip(new_tests, old_tests):
        failed = False
        if n.stdout != o.stdout:
            comparison_failure_print(
                    name=n.name,
                    old_line=o.stdout,
                    new_line=n.stdout,
                    )
            failed = True
        else:
            msg_succ(f"'{n.name}'")

        results.append((failed, n))

    compare_results_table(results)

def main() -> None:
    args = sys.argv
    if len(args) < 2:
        msg_error("no argument was provided")
    for a in args[1:]:
        match a:
            case "-h" | "--help":
                cmd_help()
            case "record":
                cmd_record()
            case "compare":
                _compare_hash()
                cmd_compare()
            case _:
                msg_error(f"unknown argument '{a}'")

if __name__ == "__main__":
    main()
