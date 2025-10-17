# umbra

Set of ✨ magic ✨ command line tools.

This is a C++ extension that I use with my [dotfiles](https://github.com/LunarWatcher/dotfiles/), but also meant to be standalone for use by anyone.

> [!warning]
>
> No code has been written yet, so this is experimentally experimental and currently unusable.

## Requirements

* Linux
    * Windows is not supported because I don't use it willingly for command line stuff
    * MacOS will never be intentionally supported out of principle
* bash, and optionally zsh
    * Note that zsh will have better support than bash, but the core scripts are written in bash as far as reasonably doable. Anything `sourced` is attempted written portably so nothing special needs to be done.

### Optional dependencies

These dependencies are strictly runtime dependencies, and are required to be able to use functionality associated with the dependency. If you don't use the functionality, it's optional.

* [zellij](https://github.com/zellij-org/zellij)

### Build requirements

* C++23 compiler
* Cmake 3.27

## License

MIT. See the LICENSE file.
