# Changelog

Will be maintained after the first patch release. Everything up to that point is just establishing base features, and it's too much stuff to bother tracking.

This changelog is loosely based on [Keep A Changelog](https://keepachangelog.com/en/1.1.0/).

## [v0.3.0]

### Added

* `pty`: New module for forcing scripts to run in a PTY, largely intended for interactive scripting purposes
* `watch`: added documentation

### Changed

* `watch`: `--exec` is now `--command`, since `--exec`/`-c` caused me to confuse myself (and I wrote the thing like a few hours ago!)
* Internals: bump catch2 to 3.14.0 to silence a lot of warnings

### Fixed

* `watch`: Fix path emitting including quotes due to a missing `.string()` on the path

## [v0.2.1]

### Changed

* `meta`: Use Codeberg's API instead of GitHub

## [v0.2.0]

### Added

* CI builds on both clang and GCC
* `devenv`: add environments to `--list`
* `meta`, currently only containing a `--check-update` flag that, as described on the box, checks if there's updates available. This doesn't actually do the updates, it just checks if there are updates available.
* mkdocs for the docs folder: https://lunarwatcher.github.io/umbra/
* `watch`, a command used for watching file trees, since `inotify-tools` is GPL'd and maintained by an AI slop-using author

### Changed

* Internal: Changed from the deprecated `stc::syscommand*` family of functions to `stc::Unix::Process`
    * This should not affect end-users, as it's the same underlying system with a much better interface, but it might.

## v0.1.0-v0.1.2

_Initial release_

[unreleased]: https://codeberg.org/LunarWatcher/umbra/compare/v0.3.0...master
[v0.3.0]: https://codeberg.org/LunarWatcher/umbra/compare/v0.2.1...v0.3.0
[v0.2.1]: https://codeberg.org/LunarWatcher/umbra/compare/v0.2.0...v0.2.1
[v0.2.0]: https://codeberg.org/LunarWatcher/umbra/compare/v0.1.2...v0.2.0
