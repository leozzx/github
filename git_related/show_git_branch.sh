parse_git_branch() {
    local branch=$(git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
    local status=$(if [ $(git status -s 2> /dev/null | wc -l) -ne 0 ]; then echo "*"; fi)
    local result=${branch}${status}
    if [[ ${result}x != "x" ]]; then result="[${result}]"; fi
    echo $result
}
GIT_BRANCH="\[\033[32m\]\$(parse_git_branch)\[\033[00m\]"

# example:
#PS1="${debian_chroot:+($debian_chroot)}\u@macbook:\W\$ ${GIT_BRANCH} "
