# Modules

| Module name | Short description | Doc link | 
| --- | --- | --- |
| `zellij` | Enables some extra per-directory and scoping functionality not present in zellij proper | [modules/zellij.md](modules/zellij.md) |
| `devenv` | Sources per-repository environments and/or entire extra s hell configurations | [modules/devenv.md](modules/devenv.md) |

The rest of this doc page is dedicated to clarifying things about modules in general, as well as terminology used in the documentation.

## A note on the use of shell terminology

The documentation lists implementation languages, and in some places, that will include shell languages. For the sake of this documentation, I've settled on the following technically inaccurate convention.

* Both "bash" and "zsh" refer to their respective shell implementations. However, unless otherwise noted, they aren't necessarily limited to their respective shells. Bash is functionally installed everywhere, so unless the docs says "bash (sourced)" or "zsh (sourced)", it'll work in any shell as long as you have the corresponding shell installed. 

  Shebangs means non-sourced bash scripts can run from zsh as well.
* "shell" is incorrectly used to refer to a sourced script that's written such that it has no implementation-specific syntax. Bash and zsh do have a significant syntactic overlap, so to avoid duplication, this is preferred for things that has to be sourced. Otherwise, it'll require one implementation per shell.

  Shell in the technically correct definition (`sh`) will never appear in umbra.

The preference order for syntax is:

1. Shell
2. Bash
3. Zsh 
4. Bash (sourced) and zsh (sourced)

## Lookup design assumptions

I work with a lot of different repos. While a lot of them have things in common that I standardise into my config, many of them simply don't. Especially at work, I also end up with a lot of private config that works with my particular setup, but that doesn't make sense to export out to everyone. It also doesn't make sense to add folders for my personal setup to a gitignore used by up to 100 other devs.

[Private gitignores are a thing that exists](https://stackoverflow.com/q/5724455), both on a global and per-repo basis, but for things that are ignored on a per-repo basis, it's an awful lot of work for something that already goes in the `.git` folder. 

I do suggest looking into them, but for my particular use-case, I have a need to switch between public and privaet things somewhat frequently. As a result, even if it won't necessarily be a core part of my workflow, modules that need to do lookup are designed to support private lookup out of the box. 

You _could_ edit your per-repo gitignore or your per-user gitignore, but you _could_ also simply... not have to.

## Module states

A module can have five different states. These indicate how stable a module is expected to be, what its future is, and/or how stable it is. 

These states are:

* **Not implemented:** Exploratory has been or is being written in preparation for the implementation of the module. The module cannot be used.
* **Alpha:** Refers to modules that aren't close to being fully implemented, and that haven't been fully tested. Alpha has no stability guarantees: the CLI and various defaults can change frequently.
* **Beta:** Refers to modules that are mostly or fully implemented, and that haven't been fully tested. CLI stability is mostly guaranteed at this point.
* **Stable:** Refers to modules that are fully implemented, tested, and considered finished for now. They may still get more features in the future, but these modules can generally be considered safe to use.
* **Deprecated:** The module is planned for removal. These modules will have an extra section at the top of its documentation explaining why and approximately when. Deprecated modules get an stderr message on use, because deleting anything (especially if umbra gets other users) will break workflows.

Note that none of the states (aside not implemented, for obvious reasons) require anything special to actually use the module. Alpha modules don't require special flags to use, and you can still use deprecated modules until their removal. 

Alpha/beta/stable are measures of the maturity of the module. Not implemented suggests the module will exist soon probably (assuming the exploratory docs and private implementation[^1] goes well), while deprecated means an implemented module doesn't have a future anymore. 



[^1]: "private implementation" is me being lazy-speak for "an implementation I'm working on, but haven't committed yet". As soon as something can be committed, I do commit it.
