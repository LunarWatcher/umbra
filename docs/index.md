# Documentation for Umbra

This website contains the documentation for [umbra](https://github.com/LunarWatcher/umbra), a set of ✨ magic ✨ shell tools for Linux.

* For a better introduction, see [the README on GitHub](https://github.com/LunarWatcher/umbra) (or on [Codeberg](https://codeberg.org/LunarWatcher/umbra)). 
* For the changelog, see [the CHANGELOG on GitHub](https://github.com/LunarWatcher/umbra/blob/master/CHANGELOG.md) (or on [Codeberg](https://codeberg.org/LunarWatcher/umbra/src/branch/master/CHANGELOG.md)).
    * For pre-built binaries, see [releases on GitHub](https://github.com/LunarWatcher/umbra/releases).
* For instructions on getting started, see [Getting started](Getting started.md)

Umbra (and the documentation) is licensed under the MIT license. 
For the full license file, see [GitHub](https://github.com/LunarWatcher/umbra/blob/master/LICENSE) or [Codeberg](https://codeberg.org/LunarWatcher/umbra/src/branch/master/LICENSE).

## Isn't magic bad?

Usually, yes. Particularly project management-related support tooling isn't a replacement for teaching people how to spin up projects or doing things with it, but exists to just simplify the amount of manual bullshit that has to be doneto do stuff.

For example, at work, I used to have to do all this shit:

1. the vault token needs to be exported. This is set up via a [.envrc](https://direnv.net/), but because of how it's set up, `direnv reload` cannot be used when changing which environment the vault is hosted in. Long story short, these three commands come up regularly:
    ```bash
    cd ..
# This step often takes 20 seconds, because I have to go and copy a URL 
    export VAULT_ADDR=...
    vault login -method=ldap username=<redacted>
    <copy and paste password>
    cd -
    ```
2. Start any docker-based third-party dependencies (database, feature toggle thing, etc.)
3. Start a main project
4. Start a secondary project that usually provides data for the main project
5. Occasionally, both 1 and 2 are both APIs, which means a separate frontend has to be started as well

This is a lot to keep track of, but more importantly, it's a lot to run. I initially reduced this by using zellij (not the zellij module; zellij the (fantastic) standalone tool), as it allows for layouts. This simplified steps 2-5 into one step. Unfortunately, zellij proper doesn't support repos of layouts. This problem is what caused Umbra to exist; though they do plan to add repos eventually:tm:, I needed more flexibility than what appeared to be promised right now, so spinning up my own tool was easier. This shortened `zellij -f ./.vim/z/layout.kdl` to `umbra z layout`.

With the introduction of devenv, the first step is changing as well. It's currently inlined into a single utility command that I still have to run manually, but this means it's already optimised from the overly convoluted shit in the list above to simply:

```
umbra env [environment]
# VAULT_ADDR is derived from ${UMBRA_DEVENV_ENVIRONMENT}
load-vault
umbra z layout
```

The last obstable to dropping to just one command (excluding `umbra env`) is that shells struggle with stdin input during startup. Once [Proton Pass CLI](https://old.reddit.com/r/ProtonPass/comments/1ogy9jr/proton_pass_cli/nlmgmmi/) is a thing, all I need is one command. Theoretically, with some changes to `umbra z`, the entire process could be inlined into the `umbra env` command. However, at least for my use-cases, there are other things I need inside the environment, so it doesn't make sense to do so.

If you're paying attention at home, umbra isn't responsible for the bulk of the decrease; that's largely on zellij's layout system. However, there are other things that umbra does in my actual, full work environment that it I haven't covered. This is largely setting addtional config variables, extra setup, and project-scoped utility functions. Because I have my private lookup dir set to a separate Git repo, I can also commit it to a private repo for backup purposes. All of this combined results in a significant decrease to the amount of text I have to type, which means I don't have to waste anywhere near as much time on things.
