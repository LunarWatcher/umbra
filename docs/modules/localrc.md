# localrc

## Summary
* **Implementation language:** C++, shell (sourced), bash
* **Additional runtime requirements:** None
* **Base command**: `umbra localrc`
* **Default alias:** `localrc`

## Module description

The module sources `.localrc` files on a per-directory basis. This lets you have per-repo config. 

This module is inspired by [direnv](https://github.com/direnv/direnv), which unfortunately is limited to environment variables. I've found myself needing more than that, and doing so for my own private config.

Note that there's no way to un-source a .localrc. You can disable it so it isn't sourced in the future, but that won't create a new shell. If there's interest, a command can be added that spawns a new shell to add the commands in, but at some point, a new shell needs to be created for some purpose.

## Source order

1. `${git_root}/.localrc`
2. `${git_root}/.git/.localrc` 

## Security

Once the scripts run, if the script is malicious, you're screwed either way. There's no way to do any form of useful sandboxing here.

Therefore, the security model is meant to avoid running these scripts in the first place. There are three options for a script:

1. Untrusted. This is the default state. The script is not evaluated.
2. Tentatively trusted. This is the suggested option for at least repos other people can access. Before the script is sourced, its SHA512 is compared to the previous version. If the comparison fails, the script is not sourced, and new approval is required.
3. Fully trusted. The script is sourced.

"Run once" or something to that effect is not (and will not be) supported. If that's what you want, just use an alias. You don't need all this overhead for what amounts to `alias loadenvrc='source $(git rev-parse --show-toplevel)/.localrc'`. This can be mixed with umbra if you have a need for both automatic and manual loading.
