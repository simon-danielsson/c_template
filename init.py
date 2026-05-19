#!/usr/bin/env python3

import subprocess, sys, shutil
from dataclasses import dataclass, field
from pathlib import Path
from enum import Enum

class MsgType(Enum):
    ERR = "\033[31m"
    STD = "\033[0m"
    FIN = "\033[1;32m"

def msg(msg: str, t: MsgType):
    match t:
        case MsgType.ERR:
            sys.stderr.write(f"{t.value}{msg}{MsgType.STD.value}\n")
            exit(1)
        case MsgType.STD | MsgType.FIN:
            sys.stdout.write(f"{t.value}{msg}{MsgType.STD.value}\n")

def get_project_proj_name() -> str:
    try:
        return sys.argv[1].strip()
    except IndexError:
        msg("No argument was given", MsgType.ERR)
        return ""

def get_project_descr() -> str:
    try:
        return sys.argv[2].strip()
    except:
        return ""

@dataclass
class Init:
    proj_name: str = field(default_factory=get_project_proj_name)
    proj_descr: str = field(default_factory=get_project_descr)

    tgt_dir: Path = Path(".")
    src_dir: Path = field(init=False)

    def __post_init__(self):
        self.tgt_dir = Path(self.proj_name)
        self.src_dir = Path("template")

def write_readme(proj_name: str, proj_descr: str, p: Path):
    with open(p.absolute(), "w", encoding="utf-8") as f:
        f.write(f"# {proj_name}\n")
        f.write(f"\n## {proj_name}\n")
        f.write(f"{proj_descr}")

def write_git_ignore(p: Path):
    with open(p.absolute(), "w", encoding="utf-8") as f:
        f.write("nvim.log\n")
        f.write("/build\n")
        f.write("/build/*\n")
        f.write(".DS_Store\n")
        f.write("*.o\n")

def write_files(i: Init):
    i.tgt_dir.mkdir(parents=True, exist_ok=True)
    write_readme(i.proj_name, i.proj_descr, i.tgt_dir.joinpath("README.md"))
    write_git_ignore(i.tgt_dir.joinpath(".gitignore"))

    try:
        shutil.copytree(src=i.src_dir, dst=i.tgt_dir, dirs_exist_ok=True)
    except Exception:
        msg("Error whilst copying files", MsgType.ERR)

def run_cmd(cmd: list[str], cwd: str) -> subprocess.CompletedProcess[str]:
    process = subprocess.run(
            cmd,
            cwd=cwd,
            capture_output=True,
            text=True,
            check=True,
            )
    return process

def init_git(i: Init):
    if i.tgt_dir.exists():
        run_cmd(["git", "init", "-b", "main"], cwd=f"{i.tgt_dir.absolute()}")
        run_cmd(["git", "add", "--all"], cwd=f"{i.tgt_dir.absolute()}")
        run_cmd(["git", "commit", "-m", "init"], cwd=f"{i.tgt_dir.absolute()}")
        run_cmd(["git", "tag", "v0.1.0"], cwd=f"{i.tgt_dir.absolute()}")
    else:
        msg("Couldn't initialize git - project dir not found.", MsgType.ERR)

def main():
    msg("Initializing program...", MsgType.STD)
    i: Init = Init()

    msg("Writing and copying files...", MsgType.STD)
    write_files(i)

    msg("Initializing git...", MsgType.STD)
    init_git(i)

    msg(f'"{i.proj_name}" was successfully generated', MsgType.FIN)

if __name__ == "__main__":
    main()
