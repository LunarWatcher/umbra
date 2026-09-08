# test

## Summary
* **Implementation language:** C++
* **Runtime requirements:** A terminal capable of displaying a TUI. See below for additional requirements for different test suites.
* **Base command:** `umbra test`, `umbra t`
* **Command run without arguments:** `umbra test`
* **State:** alpha

> `umbra test` is a utility command for showing test suites in a structured way. 

## Features

The test module was made as a result of [finding out that test suite output is fucken hard](https://lunarwatcher.codeberg.page/notes/design/test-output-hard.html), and that console reporters are fundamentally limited by not being able to show structured data. Though there are ways to make this better, there is no way to balance the need for verbose test output to follow along with the need for quick access to failing tests. In unstructured data, you can have verbosity, or direct access to known failure points (assertions) - not both, unless the sort order happens to allow the failing tests to be last.

`umbra test` is a utility and feature module that aims to show tests in a structured format. Failing the ability to do so directly, it instead resolves what went wrong and opens or shows links to the relevant structured non-machine-parseable output from other test systems (this is the plan for gradle, and is not currently implemented for any test suites).

## Compatible test suites

Each runner (except auto for reasons that will become obvious) list a support status. These are:

* Full UI: The module's own UI is used
* Parse only: Failures are parsed out and used to find and show non-machine-parseable report output. The rest of the output is such a convoluted clusterfuck that it cannot be presented sanely, and the machine-readable output is either non-existent or garbage.

Note that tools that provide their own test UIs are unlikely to ever be added here. This is not a tool designed to run tests for you, but one to bridge limitations of test tools that don't have a good way to balance the limitations of test reporting.

### `auto`

`auto` is not a test suite, but it's an implicit (or explicit) argument that can be used to automatically resolve which test suite to use.

The following auto-detection markers are used:

* Cwd contains makefile: Catch2

Unless you have a script where it's convenient to manually specify `auto` in certain cases, you don't want nor need to manually supply `-r auto` to get automatic runner resolution.

### `catch2`

* [Git repo](https://github.com/catchorg/Catch2)
* Support: Full UI
* Additional requirements: `AugmentedXML` built into the test binary (third party reporter. Use `stc::testutil`, or vendor the reporter)

The test module supports catch2, but with one major limitation: it requires a non-standard reporter with the identifier `AugmentedXML`.

`AugmentedXML` is a part of `stc::testutil`. You can find the source for the reporter [here](https://codeberg.org/LunarWatcher/stc/src/branch/master/extra-modules/test/stc/test/catch2/AugmentedXMLReporter.cpp). `AugmentedXML` is a modification of the built-in `XML` reporter designed to be run-focused rather than section focused, to better reflect how Catch2 externally appears to internally handle tests. Sections, while a hierarchy, may fail independently of the section run that causes a failure in cases where an outer section evaluates the results of an inner section, which makes the XML format and the tree format unsuited for the task.

