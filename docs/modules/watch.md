# watch

## Summary
* **Implementation language:** C++
* **Runtime requirements:** `inotify`
* **Base command:** `umbra watch`, `umbra w`
* **Command run without arguments:** error: arguments must be provided
* **State:** alpha

> `umbra watch` is a command for watching filesystems 

## Features

### Executing sub-command

The intended way to use `umbra watch` is with `--command`/`-c`, which runs a command when changes are registered:

```bash
$ umbra watch --command "echo \"I'm a moving target\"" -- ../src
```

This command prints `I'm a moving target` whenever a file changes.

### Deferring the command to the shell on changes

`--command`/`-c`, as you may have noticed if you experimented with it, does not exit until you terminate it. If you have something that's too complex for `--command` for whatever reason, you can use `--exit`/`-e`.

Using `--exit`/`-e` is strongly discouraged. Every time `umbra watch` starts up, it has to rebuild the `inotify` watch tree, which depending on the size of the tree can require a significant amount of compute. It's not known at what point this becomes significant, but the script has to recursively iterate itself down the filetree to find all folders, which means that lots of folders = lots of time.

However, the option exists regardless as it does have its applications. Here's a oneliner for running `make` when a change is detected:
```bash
$ while umbra w --exit -- ../src; do make -j $(nproc); done
```

Note that `umbra watch` is called in a for loop in this example, though this is not required. If you want to fire an event once, you can simply `umbra w --exit -- ../src && make -j $(nproc)` 

**Pro tip:** if your use-case for using this is to preserve colours in the command, you should instead chain the command you want to run with [`umbra pty`](#combining-with-umbra-pty)

### Piping changed files into another command

Umbra outputs the modified path when changes are detected regardless of the mode it's used in. This means it can be used standalone:
```bash
$ umbra watch -q ../src | xargs -I {} echo "This file has changed: {}"
```

Prints:

> ```
> This file has changed: /home/olivia/programming/cpp/umbra/src/umbra/modules/WatchModule.cpp
> ```

I don't know what use-cases exist for this. If you have a use-case for this, feel free to add a more realistic example here :)

## Examples

### Combining with `umbra pty`

Some scripts you may want to run might require a terminal to show colour. You can force it to show colour by chaining `umbra pty`. Here's a command that watches for changes in `../src`, and runs a make command piped into `less` when changes are detected:

```bash
$ umbra watch --command "umbra pty -- make -j $(nproc) test | less -r" -- ../src
```

The default behaviour when redirecting `make` is that the colours disappear. `umbra pty` preserves them.

