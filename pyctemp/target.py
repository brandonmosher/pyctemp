# pyright: strict

from typing import Tuple, List, Set
from pathlib import Path
from .template import Template
from .include import get_include_filenames_from_dirpath, get_include_filenames_from_filepath

def generate_targets_from_filenames(
    include_filenames: Set[str],
    templates: List[Template],
    target_dirpath: Path,
    include_dirpaths: Set[Path] = set()
) -> Set[Path]:

    target_filepaths: Set[Path] = set()
    for include_filename in include_filenames:
        for template in templates:
            if template.matches(include_filename):
                target_filepaths.update(template.generate_targets(include_filename, target_dirpath, include_dirpaths))
    return target_filepaths

def generate_targets_from_dirpath(
    source_dirpath: Path,
    target_dirpath: Path,
    templates: List[Template],
    include_dirpaths: Set[Path] = set()
) -> Tuple[Set[str], Set[Path]]:

    target_filepaths: Set[Path] = set()
    pending_include_filenames = get_include_filenames_from_dirpath(source_dirpath)
    completed_include_filenames: set[str] = set()
    
    while True:
        completed_include_filenames.update(pending_include_filenames)
        for target_filepath in generate_targets_from_filenames(pending_include_filenames, templates, target_dirpath, set(include_dirpaths)):
            pending_include_filenames.update(get_include_filenames_from_filepath(target_filepath))
            target_filepaths.add(target_filepath)
        pending_include_filenames = pending_include_filenames - completed_include_filenames

        if not len(pending_include_filenames):
            break
    
    return completed_include_filenames, target_filepaths
