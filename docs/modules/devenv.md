# devenv

## Summary
* **Implementation language:** C++
* **Runtime requirements:** An interactive shell
* **Base command:** `umbra devenv`, `umbra env`
* **Command run without arguments:** `umbra env default`
* **State:** Not implemented

> `umbra devenv` is a utility command for loading and managing repo-specific configurations and environments.

## Limitations

Will not work great in environments where POSIX-compliant and non-POSIX compliant shells are mixed, due to `.shell.devenv` being shared across shells. The only way to make this work portably without requiring multiple implementations is to standardise on POSIX-compliant shells, or standardise on specific non-compliant shells.

Umbra does not enforce either approach.

## Core functionality

### Shell files

Shell files (`.shell.devenv`) are standard shell files that add functionality to the shell. These are `source`d by the shell, equivalently to a `.zshrc` or a `.bashrc`. By default, the user's standard shell is used, so the `.shell.devenv` should be written with common syntax. 

Public `.shell.devenv` SHOULD be posix-compatible, as other people with other shells may run it. Private `.shell.devenv` files can be anything you want, as long as you're able to run it.

Shell-specific `.shell.devenv`s will be the form `.{shell name}.devenv`, for example `.bash.devenv`, based on (pseudocode) `$SHELL.split("/")[-1].tolower()`. If a shell-specific `.devenv` file exists, it takes priority over `.shell.devenv`. **However, these are not implemented**, and won't be for the foreseeable future. If you have a need for this feature, open an issue (if it doesn't already exist), and it'll be moved up the list of priorities.

#### Special available environment variables

Umbra sets some special environment variables that can be used within the scripts to control behaviour. These are:

* `UMBRA_DEVENV_ENVIRONMENT`: defaults to `default`, corresponds to an environment in the `.env.devenv` file corresponding to the scope of the file. Can be used to control how to set dynamic variables that can't be set in an environment file due to computational requirements.
* `UMBRA_DEVENV_SCOPE_PATH`: The scope path pointing to the current directory containing the sourced file. Can be used to construct paths to other scripts to source.

### Environment files

Environment files (`.env.devenv`) can be used to control the details of what the shell files do. If you want to provide several configurations of a shell file, that's done with an environment. 

These files are optional, as their main functionality is adding environment arguments.

In an environment configuration, you can set specific shell variables that you can use to control what the shell files do. Note that these variables are not enforced by umbra: you set whatever you need to make logical scripts, and need to keep track of these yourself.

This is with one exception: `UMBRA_DEVENV_ENVIRONMENT`, which is set by umbra prior to shell execution. This always matches an environment declared in the `.env.devenv` file.

The environment files are YAML files with umbra template support, but no code execution. For any dynamic variables you need, you'll need to set them in the `.shell.devenv`.

Syntax:
```yaml
envs:
  default:
    # The default environment always exists, and is (shock) the default environment.
    # It's the environment that's loaded if no environment is specified.
    # If you'd like, you can alias this environment to another environment:
    alias-for: test
    # alias-for is ONLY valid for the default environment. It is ignored in all other environments.
  # The following environment names are not special in any way, they're just used to demonstrate
  # how umbra can work in a two-environment deployment system.
  # You can name these whatever you want, as long as it's valid YAML
  test:
    # Used to declare variables. Note that this overrides variables if they exist
    vars:
      - VAULT_ADDR=https://test.<redacted>
    # A separate key exists to append to an existing environment variable.
    # It's assumed that this is a colon-separated list, similar to PATH.
    append:
      PATH:
        - /usr/local/bin
  prod:
    vars:
      # Technically, there's nothing preventing you from using ENV=prod instead, and
      # then setting `export VAULT_ADDR=https://${ENV}.<redacted>` in your `.shell.devenv`,
      # but this is just a basic example
      - VAULT_ADDR=https://prod.<redacted>
    append:
      PATH:
        - /usr/local/bin
```

Environment files are always sourced before shell files, but don't have to be used with shell files. You can have a standalone environment file.

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

## Examples

TBA
