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
  
This is my template for initializing, building and running C projects without a build-system.
  
<div id="install"></div>
  
## Install
  
There is no installation in the traditional sense; just add these two functions
into your .bashrc (or an equivalent file in your shell path) and you're good to go.
  
``` bash
#!/usr/bin/env bash

run() {
    local dir="$(pwd)"
    while [[ "$dir" != "$HOME" ]]; do
        if [[ -f "$dir/run" ]]; then
            (cd "$dir" && ./run "$@")
            return
        fi
        dir="$(dirname "$dir")"
    done
    echo "no project root found" >&2
    return 1
}

cinit() {
    curl -O https://raw.githubusercontent.com/simon-danielsson/c_template/refs/heads/main/init.sh || {
        error "failed to curl init.sh"
    }
    chmod +x ./init.sh
    ./init.sh $1
    rm init.sh
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
├── run
└── src
    ├── main.c
    └── main.h
```
  
Study the contents of the generated `run` script, as well as `main.h`, to
understand how everything is wired.  
  
### Running tests
   
Instead of a separate `/tests` folder, tests are ran within the main `./src` directory via a compiler flag `-DTEST` which will be defined as true when you execute `run test`. I prefer running tests inline as opposed to running them from separate files. See the following example:
  
``` c
#include "main.h"

i32 main(void) {

    if (BUILD_TEST) {
        i32 x = 8;
        ASSERT(x == 8, true);
        return 0;
    }

    printf("This is the main program");
    return 0;
}
```
  
### CLI commands (./run)
  
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
 
