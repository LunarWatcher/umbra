cd() {
    builtin cd "$@"
    umbra hook on-cd
}
