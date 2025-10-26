# Getting started

## Installation

### From pre-compiled binaries

For convenience, there's a pre-made script you can run:
```bash
sh -c "$(wget -O- https://raw.githubusercontent.com/LunarWatcher/umbra/master/scripts/install.sh)"
```

This script also contains the exact steps to do the install should you wish to do it manually. It's the `scripts/install.sh` here on Codeberg/GitHub. Note that you need `tar`, `curl`, and `wget` to use the script, so if you lack any of those dependencies and are unwilling to install them, you'll need to figure it out on your own.

The exact steps will not be copied to the README. Note that the default install location is `~/.local/bin`, so no `sudo` is required.

### From source

For requirements, see the README.

```bash
# Prerequisite: clone the repo
# This is left as an exercise to the reader
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX_PATH=${HOME}/.local
make -j $(nproc)
# This may install unnecessary third party deps too. CMake hard
make -j $(nproc) install
```

## After installation



## General recommendations

There are going to be modules you end up using more than others, and I strongly suggest you make aliases for these. Umbra intentionally doesn't ship specific aliases, as based on experience with aliases and keybinds, it's a fucking nightmare to find aliases (or keybinds) that universally work in dynamic setups like a shell. Therefore, this part is left as an optional, but strongly recommended step. 

Personally, I (or at least plan to, at the time of writing) use the `zellij` module extensively, so I have:
```bash
alias z="umbra zellij"
```
