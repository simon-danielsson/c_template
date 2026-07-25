<h1 align="center">c_template</h1>
  
<p align="center">
    <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="MIT License" />
  <img src="https://img.shields.io/github/last-commit/simon-danielsson/c_template/main?style=flat-square&color=blue" alt="Last commit" />
</p>
  
<p align="center">
  <a href="#info">Info</a> •
  <a href="#install">Install</a> •
  <a href="#usage">Usage</a> •
  <a href="#license">License</a>
</p>  
  
---
<div id="info"></div>

## Info
  
### General
  
This is my opinionated template for initializing, building, and maintaining C projects. Batteries included - powered by a tiny suite of python tools. Set up to be quick and easy to use on both MacOS and Linux.  
  
> [!IMPORTANT]
> This template is set up specifically for me. If adopting this system yourself: expect to do bit of extra setup.
  
### Features
- Ultra portable
- Bundled build-system ([run.py](./cinit_temp/run.py))
- Bundled tool for snapshot tests ([bark.py](https://github.com/simon-danielsson/bark.py))
- Bundled tool for embedding static files (not essential for c23) ([bedh.py](https://github.com/simon-danielsson/bedh.py))
- Project environment variables (inspired by [uv](https://github.com/astral-sh/uv) and [cargo](https://github.com/rust-lang/cargo))
  
### Requirements
- Unix system
- GCC or Clang
- Python (3.10+)
  
<div id="install"></div>
  
## Install
  
There is no installation in the traditional sense; just add these two functions
into your .bashrc (or an equivalent file in your shell path) and you're good to go.
  
``` bash
#!/usr/bin/env bash

run() {
    local dir="$(pwd)"
    while [[ "$dir" != "$HOME" ]]; do
        if [[ -f "$dir/run.py" ]]; then
            (cd "$dir" && ./run.py "$@")
            return
        fi
        dir="$(dirname "$dir")"
    done
    echo "no project root found" >&2
    return 1
}

cinit() {
    set -x
    local _cinit_dst="$(pwd)"
    file="init.py"
    curl -O https://raw.githubusercontent.com/simon-danielsson/c_template/refs/heads/main/"$file" || {
        echo "failed to curl $file" >&2
        exit 1
    }
    curl -L https://github.com/simon-danielsson/c_template/archive/refs/heads/main.tar.gz \
    | tar -xz --strip-components=1 c_template-main/cinit_temp
    chmod +x ./"$file"
    ./"$file" $1 "$2"
    command rm "$file"
    command rm -rf cinit_temp
    cd "$_cinit_dst"/"$1"/tests
    curl -O https://raw.githubusercontent.com/simon-danielsson/bark.py/refs/heads/main/bark.py || {
        echo "failed to curl bark.py" >&2
        exit 1
    }
    chmod +x bark.py
    mkdir -p "$_cinit_dst"/"$1"/src/static
    cd "$_cinit_dst"/"$1"/src/static
    curl -O https://raw.githubusercontent.com/simon-danielsson/bedh.py/refs/heads/main/bedh.py || {
        echo "failed to curl bedh.py" >&2
        exit 1
    }
    chmod +x bedh.py
    set +x
    cd "$_cinit_dst"/"$1"

    # git
    git init -b main
    git add --all
    git commit -m init
    git tag v0.1.0

    cd "$_cinit_dst"
}

```
  
---
<div id="usage"></div>
  
## Usage
  
### Creating a new project
    
Run `cinit` with the name of your new project as an argument. A new project
folder will be created in the current directory. (You can also add an optional project
description enclosed in double quotes - this will then be added to the generated
README file.)
   
``` bash
cinit <project-name> ["description"]
```
  
The generated project will have the following hierarchy:  
  
``` terminal
(root)
├── .clangd
├── LICENSE
├── README.md
├── run.py
├── src
│  ├── main.c
│  └── main.h
└── tests
   ├── bark.py
   ├── bark_test
   ├── tests.c
   └── tests.h
```
  
### Running tests
   
Tests are ran using a bundled copy of [bark.py](https://github.com/simon-danielsson/bark.py) - for more information, visit its page on github and read more about it there.
  
### CLI commands (run.py)
  
``` terminal
run help
run debug
run release
run install
```
  
---
<div id="license"></div>
  
## License
  
This project is licensed under the [MIT License](https://github.com/simon-danielsson/c_template/blob/main/LICENSE).  
 
