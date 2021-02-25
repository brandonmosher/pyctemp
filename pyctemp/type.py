# pyright: strict

from typing import Set, Dict, Tuple
from pathlib import Path
import re
from .include import get_include_filenames_from_filetext, get_include_filepaths_from_filenames, insert_includes

def get_types_from_dirpaths(
    dirpaths: Set[Path],
    globs: Set[str] = {'*.h', '*.c'},
    include_dirpaths: Set[Path] = set(),
    typedec_level: int = 1,
    typedef_level: int = 1
) -> Tuple[Dict[str, Path], Dict[str, Path]]:

    typedecs: Dict[str, Path] = {}
    typedefs: Dict[str, Path] = {}
    for dirpath in dirpaths:
        for glob in globs:
            for filepath in dirpath.rglob(glob):
                _typedecs, _typedefs = get_types_from_filepath(filepath, include_dirpaths, typedec_level, typedef_level)
                typedecs.update(_typedecs)
                typedefs.update(_typedefs)
    return typedecs, typedefs

def get_types_from_filepath(
    filepath: Path,
    include_dirpaths: Set[Path] = set(),
    typedec_level: int = 1,
    typedef_level: int = 1
) -> Tuple[Dict[str, Path], Dict[str, Path]]:
    
    filetext: str = ''
    include_filepaths : Set[Path] = {filepath}
    typedecs: Dict[str, Path] = {}
    typedefs: Dict[str, Path] = {}
    while typedec_level or typedef_level:
        for include_filepath in include_filepaths:
            if not include_filepath.exists():
                continue
            filetext = include_filepath.read_text(errors='ignore')   

            _typedecs, _typedefs = get_types_from_filetext(filetext)

            if typedec_level:
                for _typedec in _typedecs:
                    typedecs[_typedec] = include_filepath
            
            if typedef_level:
                for _typedef in _typedefs:
                    typedefs[_typedef] = include_filepath
        
        if include_dirpaths:
            include_filenames = get_include_filenames_from_filetext(filetext)
            include_filepaths = get_include_filepaths_from_filenames(include_filenames, include_dirpaths)
        else:
            include_filepaths.clear()

        typedec_level = max(0, typedec_level - 1)
        typedef_level = max(0, typedef_level - 1)

    return typedecs, typedefs

def get_types_from_filetext(filetext: str) -> Tuple[Set[str], Set[str]]:

    typedefs: Set[str] = set()
    typedecs: Set[str] = set()

    # typedef struct { ... } [typename];
    for match in re.finditer(
        r'^typedef struct(?:(?!})[\s\S])*}(?:(?!;)[\s\S])*(?=;)',
        filetext,
        re.MULTILINE
    ):
        typedefs.add(match.group().split('}')[-1].strip())

    # struct [structname] { ... };
    for match in re.finditer(
        r'(?<=^struct).*(?={)',
        filetext,
        re.MULTILINE
    ):
        typedefs.add(match.group().strip())

    # typedef type [typename]
    for match in re.finditer(
        #r'^typedef((?!{).)*$',
        r'(?<=^typedef)\s+\S+',
        filetext,
        re.MULTILINE
    ):
        typedecs.add(match.group().strip())
    
    return typedecs, typedefs

def get_missing_typedef_includes(
    filepath: Path,
    include_dirpaths: Set[Path],
    typedef_include_filepaths: Dict[str, Path]
) -> Dict[str, Path]:

    typedecs, typedefs = get_types_from_filepath(filepath, include_dirpaths, typedec_level=1, typedef_level=2)
    _, include_typedefs = get_types_from_dirpaths(include_dirpaths)
    include_typedefs.update(typedef_include_filepaths)
    missing_typedefs: Set[str] = typedecs.keys() - typedefs.keys()
    found_typedefs: Set[str] = include_typedefs.keys() & missing_typedefs
    return {typename:include_typedefs[typename] for typename in found_typedefs}

def insert_missing_typedef_includes(
    target_filepath: Path,
    include_dirpaths: Set[Path] = set(),
    typedef_include_filepaths: Dict[str, Path] = dict()) -> None:           
    missing_typedefs = get_missing_typedef_includes(target_filepath, include_dirpaths, typedef_include_filepaths)
    if missing_typedefs:
        target_filetext: str = target_filepath.read_text()
        target_filetext = insert_includes(target_filetext, {Path(path.name) for path in missing_typedefs.values()})
        target_filepath.write_text(target_filetext)
