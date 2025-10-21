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

`zellij -l` is stuck looking up that directory, and taking other directories as a path. This doesn't scale well, particularly with large repos that may have several zellij configurations, and where those configurations may or may not be public. 

This umbra module removes that limitation. It has the following order of execution:

1. If path or file name, use it directly. This is largely just for muscle memory compatibility
2. If not path:
    1. Look in `$UMBRA_ZELLIJ_PRIVATE_SUBDIR` (default: `{{git_root}}/.git/zellij/`) 
    2. Look in `$UMBRA_ZELLIJ_PUBLIC_SUBDIR` (default: `{{git_root}}/dev/zellij/`)
    3. Forward the string verbatim to zellij. This will trigger a lookup against the standard layout_dir.

This particular feature synergises well with both [localrc](docs/modules/localrc.md), which has a very similar private vs. public distinction, and [direnv](https://github.com/direnv/direnv/)
