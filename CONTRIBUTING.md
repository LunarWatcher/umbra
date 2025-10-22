# Contribution guidelines

This file is mostly aimed at developers, and primarily describes the setup required for development, and project-specific things to think about when contributing code. For general open-source contribution guidelines, see [opensource.guide](//opensource.guide). The guidelines listed under "Basic guidelines" do apply to all forms of contributions, including issues.

This file will not go into detail on how to write issues. Any important details that need to be included (if any) will be part of an issue template, selectable when you create an issue. If none exists for your use-case (or at all), use common sense. I do strongly suggest reading [the section on communicating effectively on opensource.guide](https://opensource.guide/how-to-contribute/#communicating-effectively) if you're wondering how to write good issues. There's nothing anyone could write here that isn't covered there and in thousands of other resources around the internet in far greater detail.

## Basic guidelines

### Use of generative AI is banned

Generative AI uses training data [based on plagiarism and piracy](https://web.archive.org/web/20250000000000*/https://www.theatlantic.com/technology/archive/2025/03/libgen-meta-openai/682093/), has [significant environmental costs associated with it](https://doi.org/10.21428/e4baedd9.9070dfe7), and [generates fundamentally insecure code](https://doi.org/10.1007/s10664-024-10590-1). GenAI is not ethically built, ethical to use, nor safe to use for programming applications. When caught, you will be permanently banned from contributing to the project, and any prior contributions will be checked and potentially reverted. Any and all contributions you've made cannot be trusted if AI slop machines were involved.

## Development setup

You need:

* Linux (still)
* CMake 3.27 or newer
* A C++23 compiler
* An internet connection
* The repo cloned
* `make`, or another CMake generator. `make` is usually available everywhere, and therefore what's used in this documentation. If you don't use make, you'll need to replace steps explicitly invoking `make`, and this is left as an exercise to the reader.

```
mkdir build
cd build
# -DUMBRA_LINT runs in the CI and should be enabled. This does drastically increase compile time.
# -DUMBRA_SANITISE does not run in the CI, but should be enabled

cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DUMBRA_LINT=ON -DUMBRA_SANITISE=ON

# Build only 
make -j $(nproc)

# Run
make -j $(nproc) run
```

### Running tests

After general setup, you can just run
```bash
make -j $(nproc) test
```

### Testing policy

As much of the code should be tested as possible, within reason.

The primary goal of tests is to ensure there's a support framework that prevents backsliding in code quality. With enough tests, you don't need to worry as much about breaking something unrelated to what you were working on. 100% coverage is a pointless metric, but coverage tools can be useful to tell what critical paths aren't being tested. In real code, many paths may legitimately be unreachable without doing an awful lot of fucking around, particularly in exception handlers. Doing elaborate bullshit to test every possible path in the code, including trivial paths, is a waste of time and effort.

In practice, this means:

* If you're writing new functionality, write tests for the core parts of it
* If you're fixing a bug that was reported, write regression tests
* If you're working with edge-cases, test them

Writing tests isn't always feasible, but it should be attempted whereever possible. However, if any tests break, they must be fixed. Removal should only be done if the corresponding functionality is removed, and not as a way to bypass the test failures to maybe perhaps fix later.

### Dependency policy

No unnecessary compile-time dependencies should be introduced, as compile-time dependencies increase the complexity of actually getting stuff built by and shipped to end-users.

Currently, the following three dependencies are used:

* [Catch2](https://github.com/catchorg/Catch2/) (tests only, so never shipped)
* [CLI11](https://github.com/CLIUtils/CLI11) for command-line parsing
* [stc](https://github.com/LunarWatcher/stc) for multiple common utils
* PLANNED: at least a library for JSON, and possibly one for YAML. To be determined

All other dependencies are exclusively runtime-dependencies that are only ever attempted used if the user uses a corresponding command. This is preferred over linking, as anything that could force a recompilation to use certain modules is undesirable. One functionally identical binary should always be produced regardless of what optional dependencies are in the compiling user's PATH.

Additional dependencies may be added as long as the need for it can be properly explained, it's possible to install with `FetchContent` without a `patch`, and it can build static libraries. For ease-of-installation, avoiding folders outside `bin` is preferred.
