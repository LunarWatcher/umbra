# pty

## Summary
* **Implementation langauge:** C++
* **Additional runtime requirements:** None
* **Base command:** `umbra pty`
* **Command run without arguments:** error: arguments must be provided
* **State:** alpha

> `umbra pty` is a utility command for running commands in a PTY while redirecting their output 

## Features

This module is similar in principle to `script`, except it doesn't create a file. 

The input arguments to this command is a command run in a shell (forced: the same shell as umbra was run from) that's spawned in pty mode, so it's indistinguishable from running in a proper terminal. Redirects then redirect `umbra` rather than the nested command, which means commands that print in colour when in a terminal still print in colour. This is, in fact, its intended use-case: forcing colour output in commands that disable their colour output when redirected to allow for scripting (interactive and otherwise) on other commands.

For example, if you want to `| less` `make`, you can do this with `umbra pty`:
```bash
$ umbra pty -- make -j $(nproc) test | less -r
```

Note that `less -r` is required for the colours to actually render. `../src` in this case is the path to my source folder that I want to listen for changes in.

This can also be chained with other commands. Additional examples will come in the future.
