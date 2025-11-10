# Changelog

Will be maintained after the first patch release. Everything up to that point is just establishing base features, and it's too much stuff to bother tracking.

This changelog is loosely based on [Keep A Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

### Added

* CI builds on both clang and GCC
* `devenv`: add environments to `--list`
* `meta`, currently only containing a `--check-update` flag that, as described on the box, checks if there's updates available. This doesn't actually do the updates, it just checks if there are updates available.
* mkdocs for the docs folder: https://lunarwatcher.github.io/umbra/

### Changed

* Internal: Changed from the deprecated `stc::syscommand*` family of functions to `stc::Unix::Process`
    * This should not affect end-users, as it's the same underlying system with a much better interface, but it might.

## v0.1.0-v0.1.2

_Initial release_

[unreleased]: https://github.com/LunarWatcher/umbra/compare/v0.1.2...master
