[ "$UMBRA_SHELLWRANGLER_COMMANDS" != "" ] && eval "${UMBRA_SHELLWRANGLER_COMMANDS}"

# Reset ZDOTDIR so .zcompdump doesn't pollute the share folder
export ZDOTDIR="${UMBRA_SHELLWRANGLER_ZDOTDIR:-$HOME}"
[ -f "$ZDOTDIR/.zshenv" ] && source "$ZDOTDIR/.zshenv"
[ -f "$ZDOTDIR/.zprofile" ] && source "$ZDOTDIR/.zprofile"
[ -f "$ZDOTDIR/.zshrc" ] && source "$ZDOTDIR/.zshrc"
