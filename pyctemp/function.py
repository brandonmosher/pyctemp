# pyright: strict

from typing import Tuple, List, Union
import re
from .strip import strip_comments, strip_extra_newlines, strip_macros

def get_unscoped_blobs(filetext: str):
    filetext = strip_comments(filetext)
    filetext = strip_macros(filetext)
    filetext = strip_extra_newlines(filetext, 1)

    blobs: List[str] = []
    bracket_count: int = 0
    blob: str = ''

    for c in filetext:
        blob += c

        if c == '{':
            bracket_count += 1
        elif c == '}':
            bracket_count -= 1
        
        if not bracket_count and c in ['}', ';']:
            if re.search(r'^\s*(struct|typedef) ', blob, re.MULTILINE) and c != ';':
                continue
            blob = blob.strip()
            blobs.append(blob)
            blob = ''
    
    return blobs

def get_func_name(funcdef: str) -> str:
    match = re.search(r'(\S)*(?=\()', funcdef)
    return '' if match is None else match.group()

def get_funcdefs(filetext: str) -> List[str]:
    return [blob for blob in get_unscoped_blobs(filetext) if blob[-1] == '}']

def get_funcdecs(filetext: str) -> List[str]:
    return [blob for blob in get_unscoped_blobs(filetext) if blob[-2:] == ');']

def sub_forwarded_func(tem_func: str) -> Union[Tuple[str, str], None]:
    def get_modifier_regex(x: str) -> str:
        return r'\b{}(?=[^\w])'.format(re.escape(x))
     
    if re.search(get_modifier_regex('P'), tem_func):
        val_func: str = tem_func
        val_func = re.sub(get_modifier_regex('A'), '', val_func)
        val_func = re.sub(get_modifier_regex('P'), '', val_func)
        val_func = re.sub('_R', '', val_func)

        ref_func: str = tem_func
        ref_func = re.sub(get_modifier_regex('A'), '&', ref_func)
        ref_func = re.sub(get_modifier_regex('P'), '*', ref_func)
        ref_func = re.sub('_R', '_r', ref_func)

        return (val_func, ref_func)
    
    return None

def sub_forwarded_funcdefs(filetext: str) -> str:
    for tem_funcdef in get_funcdefs(filetext):
        forwarded_funcdefs = sub_forwarded_func(tem_funcdef)
        if forwarded_funcdefs:
            filetext = re.sub(re.escape(tem_funcdef), '\n\n'.join(forwarded_funcdefs), filetext)
    return filetext

def sub_forwarded_funcdecs(filetext: str) -> str:
    for tem_func_dec in get_funcdecs(filetext):
        forwarded_funcdecs = sub_forwarded_func(tem_func_dec)
        if forwarded_funcdecs:
            filetext = re.sub(re.escape(tem_func_dec), '\n\n'.join(forwarded_funcdecs), filetext)
    return filetext
