# devenv

## Summary
* **Implementation language:** C++
* **Runtime requirements:** An interactive shell
* **Base command:** `umbra devenv`, `umbra env`
* **Command run without arguments:** `umbra env default`
* **State:** alpha

> `umbra devenv` is a utility command for loading and managing repo-specific configurations and environments.

## Limitations

Will not work great in environments where POSIX-compliant and non-POSIX compliant shells are mixed, due to `.shell.devenv` being shared across shells. The only way to make this work portably without requiring multiple implementations is to standardise on POSIX-compliant shells, or standardise on specific non-compliant shells.

Umbra does not enforce either approach.

Additionally, only a few shells are supported. This is due to another set of hacks required to actually get the extra commands into the shell without requiring adding more stuff to your init files. This means the only supported shells are:

* bash
* zsh

This is due to a major limitation in shells, where there is no way to run `-ic` and have the command run first, and then get dropped into a real interactive shell.

## Core functionality

### Shell files

Shell files (`.shell.devenv`) are standard shell files that add functionality to the shell. These are `source`d by the shell, equivalently to a `.zshrc` or a `.bashrc`. By default, the user's standard shell is used, so the `.shell.devenv` should be written with common syntax. 

Public `.shell.devenv` SHOULD be posix-compatible, as other people with other shells may run it. Private `.shell.devenv` files can be anything you want, as long as you're able to run it.

Shell-specific `.shell.devenv`s will be the form `.{shell name}.devenv`, for example `.bash.devenv`, based on (pseudocode) `$SHELL.split("/")[-1].tolower()`. If a shell-specific `.devenv` file exists, it takes priority over `.shell.devenv`. **However, these are not implemented**, and won't be for the foreseeable future. If you have a need for this feature, open an issue (if it doesn't already exist), and it'll be moved up the list of priorities.

#### Special available environment variables

Umbra sets some special environment variables that can be used within the scripts to control behaviour. These are:

* `UMBRA_DEVENV_ENVIRONMENT`: defaults to `default`, corresponds to an environment in the `.env.devenv` file corresponding to the scope of the file. Can be used to control how to set dynamic variables that can't be set in an environment file due to computational requirements.
* `UMBRA_DEVENV_SCOPE_PATH`: The scope path pointing to the current directory containing the sourced file. Can be used to construct paths to other scripts to source.

#### Technical: How shell setup works

To be more explicit about the  details in from the limtations section, not a single shell supports  `-ic` where the shell actually goes interactive after the command executes. As a result, an injection system had to be set up. This is done per shell. However, in all cases, the `.shell.devenv` is sourced AFTER all other shell files.

The `.*rc` files can be found [in the `shell/wrangler` directory](https://github.com/LunarWatcher/umbra/tree/master/shell/wrangler) if you're interested in per-shell details.

<details>
  <summary>Very technical details</summary>
  <div>
  There's functionally an injection system, where a custom `.bashrc`/`.zshrc` is loaded. It then loads the shell's built-in files as usual, and then loads umbra. 

  There's no way to do this portably, because there's no standard for even changing which `.rc` file you want to launch from, much less appending files or individual commands. This means it's handled on a per-shell basis:

  1. Bash sets `--init-file`
  2. Zsh overrides `ZDOTDIR`

  In both cases, the method changes where the files are sourced from. Note that with zsh, `ZDOTDIR` is restored after the wrangler zshrc is loaded, and if `ZDOTDIR` is set prior to umbra's launch, umbra respects it when sourcing the presumably correct user configuration files.

  Because the wrangler rc files have to manually source the relevant files from the shell though, there are a few sources of error. There's no guarantee the behaviour of these files line up with what the shell actually does, because I'm basing it on spending a whole 5 minutes reading the relevant documentation. YMMV, open an issue if shit doesn't work the way you expect.
  </div>
</details>

### Environment files

Environment files (`.env.devenv`) can be used to control the details of what the shell files do. If you want to provide several configurations of a shell file, that's done with an environment. 

These files are optional, as their main functionality is adding environment arguments.

In an environment configuration, you can set specific shell variables that you can use to control what the shell files do. Note that these variables are not enforced by umbra: you set whatever you need to make logical scripts, and need to keep track of these yourself.

This is with one exception: `UMBRA_DEVENV_ENVIRONMENT`, which is set by umbra prior to shell execution. This always matches an environment declared in the `.env.devenv` file.

The environment files are JSON files with umbra template support, but no code execution. For any dynamic variables you need, you'll need to set them in the `.shell.devenv`.

Syntax:
```jsonc
{
    // This file is a modified variant of jsonc, where only // comments are allowed, and
    // they must be alone on a line. This is a limitation of the JSON parser used, and json comments
    // being a preprocessor step.
    "envs": {
        "default": {
            // The default environment always exists, and is (shock) the default environment.
            // It's the environment that's loaded if no environment is specified.
            // If you'd like, you can alias this environment to another environment:
            "alias-for": "test"
            // otherwise, all the same keys are supported as in other environments.
        },
        // The following environment names are not special in any way, they're just used to demonstrate
        // how umbra can work in a two-environment deployment system.
        // You can name these whatever you want, as long as it's valid YAML
        "test": {
            // Vars defines any environment variables you want included. These are not allowed
            // to start with UMBRA_, as that is an umbra-reserved prefix. UMBRA_-variables are
            // protected within these files and cannot be touched. Aside that, all variables
            // are fair game.
            // These override any existing environment variables, so make sure what you're overriding
            // isn't critical to basic use of the shell (such as $PATH).
            "vars": {
                "VAULT_ADDR": "https://test.<redacted>"
            },
            // Similar to vars, but prepends instead of modifies. It's assumed the variables are
            // colon-separated arrays.
            "prepend": {
                "PATH": [
                    "/usr/local/bin"
                ]
            }
        },
    }
}
```

Environment files are always sourced before shell files, but don't have to be used with shell files. You can have a standalone environment file.

#### `prepend`-limitations with `PATH`, and setup order for environment variables

While `PATH` is used in the above example, the injection isn't guaranteed to ensure that your additions are the first `PATH` components. This is a consequence of the environment variables being sourced before the shell's built-in config files, so any configurations in the user's `.*rc` files can shadow the additions made in the `.env.devenv`. 

I'd like to change this at some point, but I can't agree with myself on the syntax, so I'm deprioritising it for now. in the meanwhile, you can specify an environment variable, for example[^1]:
```jsonc
"vars": {
    "PATH_PREPEND": "{{cwd}}/build/bin"
}
```

And then, in your `.shell.devenv`, you modify the path:

```shell
export PATH="${PATH_PREPEND}:${PATH}"
```

As noted in the section for  the `.shell.devenv` file, this works because `.shell.devenv` is sourced after the shell `.*rc` files.

## Lookup order and merging strategy

By default, the lookup order is:

1. `UMBRA_DEVENV_PRIVATE_SUBDIR` (default: `{{git_root}}/.git/devenv`; templates supported)
2. `UMBRA_DEVENV_PUBLIC_SUBDIR` (default: `{{git_root}}/dev/devenv`; templates supported)

Only one shell file is sourced. This is a feature meant to allow the shell files to contain semi-arbitrary startup logic that can be overridden in its entirety by adding a separate file. This will be changed in the future to allow both to be sourced and optionally picked if one of the two should be disabled, but this requires further ✨ magic ✨ research, as I don't yet know how this can be done compactly and with as little friction as possible.

For the env files, the lookup order is reversed, as all the files are included and merged through the resulting actions overriding previous actions. This avoids needing cursed merging algorithm shit, at the expense of some (but relatively speaking, extremely little) wasted compute. 

### Alias priority

As shown in the configuration syntax, the default environment can have an `alias-for` key to point it to another environment. However, there are two conflicting scenarios:

1. Either the public or private file aliases, while the other defines it to something special
2. Both the files alias them to different environments

Both of these are handled identically: the private file takes priority, and the other is disregarded.

If the private file declares that `default` is an alias, it's aliased. If the private file declares it's a full environment, it's a full environment.

## Security

This module goes through all potential files, and sources them all. This means there's attack vectors going via the files. 

To deal with this, devenv has the following security strategy:

* No files are sourced without permission. There is a `--trusted` flag to override this, but using it is discouraged. 
* As long as `--trusted` isn't passed (meaning by default), all scripts have to be manually approved in one of two ways:
    1. Interactively, as long as `--no-interactive` isn't passed
    2. Manually, using `umbra devenv audit`

    Scripts can be automatically approved, but this is only recommended for scripts from private repositories you fully control.
* Approval is only valid for the current state of the file, unless it's default-approved. If it changes, the approval is invalidated and a new approval is required. 

> [!warning]
> Umbra only addresses the first level, and does not scan for other imported files. If the shell script `source`s a file that's then changed, reapproval is not triggered. If you're running scripts from sources so untrusted you're not sure if you can trust hops from your main shell file, you probably need to reconsider your security model.

## Supported input environment variables

This list describes the variables devenv uses for input. 

* `SHELL` (default: none; must be set by the shell): used to determine what shell to spawn. This is also a standard variable set in many (all?) shells, so it's recommended you override it per command rather than with an `export`, for example `SHELL=$(which bash) umbra devenv`. It's recommended to use the `--shell` flag instead, which doesn't require a path: `umbra devenv -s bash`
* `UMBRA_DEVENV_PRIVATE_SUBDIR` (default: `{{git_root}}/.git/devenv`; templates supported): see [lookup order and merging strategy](#lookup-order-and-merging-strategy)
* `UMBRA_DEVENV_PUBLIC_SUBDIR` (default: `{{git_root}}/dev/devenv`; templates supported): see [lookup order and merging strategy](#lookup-order-and-merging-strategy)

## Examples

TBA
