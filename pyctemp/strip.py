# pyright: strict

import re

def strip_comments(filetext: str) -> str:
    filetext = re.sub(r'^[\s]*//.*', '', filetext, flags=re.MULTILINE)
    filetext = re.sub(r'\/\*(?:(?!\*\/)[\s\S])*\*\/', '', filetext, flags=re.MULTILINE)
    return filetext

def strip_macros(filetext: str) -> str:
    return re.sub(r'^[\s]*#.*', '', filetext, flags=re.MULTILINE)

def strip_macro(directive: str, macro: str, filetext: str) -> str:
    return re.sub(r'#{}\s+{}\s+.*'.format(*map(re.escape, (directive, macro))), '', filetext)

def strip_extra_whitespace(filetext: str) -> str:
    return re.sub(r'\s+', ' ', filetext, flags=re.MULTILINE)

def strip_extra_newlines(filetext: str, max_consecutive_newlines: int = 2) -> str:
    return re.sub(r'\n{{{},}}'.format(max_consecutive_newlines + 1), '\n'*max_consecutive_newlines, filetext, flags=re.MULTILINE)
