#! /usr/bin/env bash

check_format () {
    FMT="formatted.txt"

    clang-format -style=file "${1}" > "${FMT}"
    diff -q formatted.txt "${1}" >/dev/null 2>&1 || { \
        printf "File %s is not formatted properly:\n\n" "${1}"
        diff -c "${FMT}" "${1}"
        rm "${FMT}"
        exit 1;
    }
   rm "${FMT}"
}

check_static_analysis () {
    clang-tidy "${1}" \
        -checks="-*,
                  clang-analyzer-*,
                 -clang-analyzer-cplusplus*,
                 -clang-analyzer-optin.portability.UnixAPI,
                 -clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling"\
        -- -I ./includes || exit 1
}

[[ $# -ne 1 ]] && {
    echo "$0:Error: expected an argument, got $#" && exit 1 ;
}
echo "Checking ${1}..."
check_format "${1}"
check_static_analysis "${1}"
