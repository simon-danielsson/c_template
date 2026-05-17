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
  
This is my template for initializing, building, running, as well as maintaining C projects without any third-party build-system. Built to be quick and easy to use on both MacOS and Linux.  
  
### Requirements
- Unix system
- GCC/Clang
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
    file="init.py"
    curl -O https://raw.githubusercontent.com/simon-danielsson/c_template/refs/heads/main/"$file" || {
        error "failed to curl $file"
    }
    curl -O https://raw.githubusercontent.com/simon-danielsson/c_template/refs/heads/main/cinit_temp || {
        error "failed to curl temp files"
    }
    chmod +x ./"$file"
    ./"$file" $1
    rm "$file"
    rm -rf cinit_temp
}
```
  
---
<div id="usage"></div>
  
## Usage
  
### Creating a new project
    
Run `cinit` with the name of your new project as an argument. A new project
folder will be created in the current directory.
   
``` bash
cinit <project-name>
```
  
The generated project will have the following hierarchy:  
  
``` terminal
(root)
├── .git
├── .gitignore
├── LICENSE
├── README.md
├── run.py
└── src
    ├── main.c
    └── main.h
```
  
Study the contents of the generated `run.py` script, as well as `main.h`, to
understand how everything is wired.  
  
### Running tests
   
The bulk of testing in my programs consist of short inline unit tests. These are ran from within the main `./src` directory via a compiler flag `-DTEST` which will be defined as true when you execute `run test`. You will need to decide on your own how to organize tests in your code. See the following example:  
  
``` c
#include "main.h"
#include "tests.h"

i32 main(void) {
    if (BUILD_TEST) { // true only if command "run test"
        LOG("Test 1"); test1();
        LOG("Test 2"); test2();
        LOG("Test 3"); test3();
        return 0;
    }

    printf("This is the main program");
    return 0;
}
```
  
### CLI commands (run.py)
  
``` terminal
run help
run debug
run release
run test
```
  
---
<div id="license"></div>
  
## License
  
This project is licensed under the [MIT License](https://github.com/simon-danielsson/c_template/blob/main/LICENSE).  
 
