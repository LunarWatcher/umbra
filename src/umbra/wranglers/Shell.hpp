#pragma once

#include <string>
#include <vector>

namespace umbra {

enum class ShellType {
    ZSH,
    BASH
};

/**
 * Utility/necessity class that injects commands without breaking hard
 */
class ShellWrangler {
private:
    /**
     * Zsh injection strategy:
     *
     * 1. Save `ZDOTDIR` as `UMBRA_SHELLWRANGLER_ZDOTDIR`, with default set to $HOME as per the zsh docs
     * 2. Unclear at the time of writing, but involves a directory with an overridden zshrc
     *     * The zshrc contains
     *       ```
     *       <commands>
     *
     *       source ${UMBRA_SHELLWRANGLER_ZDOTDIR:-$HOME}/.zshenv
     *       source ${UMBRA_SHELLWRANGLER_ZDOTDIR:-$HOME}/.zprofile
     *       source ${UMBRA_SHELLWRANGLER_ZDOTDIR:-$HOME}/.zshrc
     *       ```
     * 3. Point ZDOTDIR to the directory containing said zshrc
     * 4. Start and let 'er rip
     *
     * \see https://zsh.sourceforge.io/Intro/intro_3.html
     */
    void injectZsh();

    /**
     * Bash injection strategy:
     *
     * 1. Unclear at the time of writing, but involves a directory with an overridden bashrc. The bashrc contains:
     *     ```
     *     <commands>
     *
     *     source $HOME/.bashrc
     *     ```
     * 2. Add the `--init-file` flag pointing to that script.
     */
    void injectBash();

    ShellType identifyShell(const std::string& shellOrPathName);

public:
    /**
     * Runs a command in the shell's interactive mode. Exists because there isn't a standard for shells that allows 
     * `-ic "single command"` that then loads everything else in the shell as normal.
     *
     * This limits shell support rather drastically, but this is a problem for future me.
     *
     * \param args The full args to pass to execv, INCLUDING the shellPathOrName. 
     * \param shellPathOrName   the (shock) name or path to the shell. This is separate from args as it's not guaranteed
     *                          that the shell is the first or second argument, due to current use of `/usr/bin/env` to
     *                          actually run the shell
     */
    void execInteractive(
        const std::string& shellPathOrName,
        std::vector<std::string> args,
        const std::string& commands
    );
};

}
