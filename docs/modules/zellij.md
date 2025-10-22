# zellij

## Summary
* **Implementation language:** C++
* **Additional runtime requirements:** https://zellij.dev/
* **Base command**: `umbra zellij`, `umbra z`
* **Default alias:** N/A

## Features

### Extended template lookup by name

By default, `zellij -l layout_name` is limited to lookup using `layout_dir`. This means that if you have:
```
layout_dir "/home/olivia/programming/dotfiles/zellij-layouts"
```

`zellij -l` is stuck looking up that directory, and taking other directories as a path. This doesn't scale well, particularly with large repos that may have several zellij configurations, and where those configurations may or may not be public. Note that "public" in this context refers to "available to someone that isn't you", and not necessarily "freely accessible to the general public" - being shared in a private repo still counts as public in this context.

This umbra module removes that limitation. It has the following order of execution:

1. The command is run either with or without a layout. If no layout is supplied, `$UMBRA_ZELLIJ_NAME_CONVENTION` (default: `default.kdl`; templates supported) is used. Must be a filename only
2. If path or file name, use it directly. This is largely just for muscle memory compatibility, so `umbra z ./layout.kdl` doesn't break (the command is identical to `zellij -l ./layout.kdl`)
3. If not path:
    > [!note]
    > In the future, a third lookup may be supported that's equivalent to a `layout_dir` array. This is not currently implemented because input processing hard.

    1. Look in `$UMBRA_ZELLIJ_PRIVATE_SUBDIR` (default: `{{git_root}}/.git/zellij/`; templates supported) 
    2. Look in `$UMBRA_ZELLIJ_PUBLIC_SUBDIR` (default: `{{git_root}}/dev/zellij/`; templates supported)
    3. Forward the string verbatim to zellij. This will trigger a lookup against the standard layout_dir.


This particular feature synergises well with both [localrc](docs/modules/localrc.md), which has a very similar private vs. public distinction, and [direnv](https://github.com/direnv/direnv/)

`{{git_root}}` is a literal template, that's automatically expanded. You can pass in both relative and absolute paths, but `{{git_root}}` is special. See [templating](../special/Templating.md)
