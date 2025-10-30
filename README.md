# umbra

Set of ✨ magic ✨ command line tools.

This is a C++ extension that I use with my [dotfiles](https://github.com/LunarWatcher/dotfiles/), but also meant to be standalone for use by anyone.

## Requirements

* Linux
    * Windows is not supported because I don't use it willingly for command line stuff
    * MacOS will never be intentionally supported out of principle
* bash, and optionally zsh
    * Note that zsh will have better support than bash, but the core scripts are written in bash as far as reasonably doable. Anything `sourced` is attempted written portably so nothing special needs to be done.
* Git, notably used for path lookups. Technically not required, but functionally required for a lot of core functionality to work as described. Umbra won't break if you don't have git, but it will not work as advertised.

### Optional dependencies

These dependencies are strictly runtime dependencies, and are required to be able to use functionality associated with the dependency. If you don't use the functionality, it's optional.

* [zellij](https://github.com/zellij-org/zellij)

### Build requirements

* C++23 compiler
* CMake 3.27

For development requirements and setup, see CONTRIBUTING.md.

## Features and goal

The core features are split into modules. The modules, and their core functionality, is documented [here](https://github.com/LunarWatcher/umbra/blob/master/docs/Modules.md).

The goal of umbra is to make it easier to write config once, and use the things based on the config constantly without needing to do much to make it work. It's also designed under the assumption that there often is a hard split between public and private config, and user and repo config. All four can coexist and be used in a given repo, but the options for adding them automatically are limited. Many of the modules in umbra are explicitly designed to deal with tools that do not support this kind of granular and variable config.

At the same time, though, it's also designed to be ✨ magic ✨, and not require more input than absolutely necessary. Umbra's CLI is designed to be as minimal as possible while providing the exact same functionality as a non-magic CLI would. 

One example of this is the zellij module, which aims to make [zellij](https://github.com/zellij-org/zellij) work much more dynamically. The environment variables and built-in templating means there's several options for how `umbra z` finds the template to forward to Zellij. There are _technically_ two additional directories on top of `layout_dir` in zellij's config, but because templates are supported, it's also dynamic, and can switch based on either the current working directory or the git root, depending on what you configure it to do. In the future, a third option equivalent to an array of `layout_dir`s is considered, but its value is currently being debated in the foxgirl council.

On the magic side of things, `umbra z` takes an optional layout, with the default name being resolved by another environment variable. The default name _also_ supports templates, so if you set it to `{{folder_name}}`, you could have an organisation-level repo that contains `.kdl` layouts for every repo, or an in-repo subfolder that works with monorepos where changing the working directory is standard, and execution context is enough to just figure out which to use. 

## Installation and getting started

See [Getting started](https://github.com/LunarWatcher/umbra/blob/master/docs/Getting%20started.md)

## License

MIT. See the LICENSE file.
