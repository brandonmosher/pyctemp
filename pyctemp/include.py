# pyright: strict

from typing import Set
from pathlib import Path
import re

def get_include_filenames_from_filetext(filetext: str) -> Set[str]:
    include_filenames: Set[str] = set()
    for match in re.finditer(r'(?<=#include).*', filetext):
        include_filenames.add(match.group().strip(' "'))
    return include_filenames

def get_include_filenames_from_filepath(filepath: Path) -> Set[str]:
    return get_include_filenames_from_filetext(filepath.read_text())

import glob as gglob
def get_include_filenames_from_glob(
    globs: Set[str],
) -> Set[str]:
    include_filenames: Set[str] = set()
    filepaths: Set[Path] = set()
    for glob in globs:
        filepaths.update(set(map(Path.resolve, map(Path, gglob.glob(glob)))))
    for filepath in filepaths:
        include_filenames.update(get_include_filenames_from_filepath(filepath))
    return include_filenames

def get_include_filenames_from_dirpath(
    dirpath: Path,
    globs: Set[str] = {'*.h', '*.c'}
) -> Set[str]:#TODO: rglob?
    include_filenames: Set[str] = set()
    for glob in globs:
        for filepath in dirpath.glob(glob):
            include_filenames.update(get_include_filenames_from_filepath(filepath))
    return include_filenames

def get_include_filepaths_from_filenames(
    include_filenames: Set[str],
    include_dirpaths: Set[Path]
) -> Set[Path]:

    include_filepaths : Set[Path] = set()
    for include_filename in include_filenames:
        for include_dirpath in include_dirpaths:
            include_filepath = include_dirpath.joinpath(include_filename)
            if include_filepath.exists():
                include_filepaths.add(include_filepath)
    return include_filepaths

def insert_includes(
    filetext: str,
    include_filepaths: Set[Path]
) -> str:

    filelines = filetext.strip().splitlines()
    insert_filelines = list(map('#include "{}"'.format, include_filepaths))

    start = 0
    for i, fileline in enumerate(filelines):
        if not fileline.startswith('#'):
            start = i
            break

    return '\n'.join(filelines[:start] + insert_filelines + filelines[start:])
