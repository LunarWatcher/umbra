# This shouldn't break direnv in at least zsh, as it hooks into some zsh-specific hooks.
# This is just me being maximally lazy. Should probably revisit with shell-specific hooks that are
# auto-sourced from this file.
cd() {
    builtin cd "$@"
    umbra hook on-cd
}
