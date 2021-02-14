# pyright: strict
from __future__ import annotations

from typing import Tuple, List, Set
from pathlib import Path
import re
from .strip import strip_macro, strip_extra_newlines
from .function import sub_forwarded_funcdecs, sub_forwarded_funcdefs
from .type import get_missing_typedefs
from .include import insert_includes

class Template:
    name: str
    template_dirpath: Path
    template_filepaths: Set[Path] = set()

    key_val_delim: str
    keys: Tuple[str]

    def __init__(self,
        template_dirpath: Path,
        globs: Set[str] = {'*.h', '*.c'},
        key_val_delim: str = '.'
    ) -> None:

        self.template_dirpath = template_dirpath
        self.key_val_delim = key_val_delim

        self.name, self.keys = self.split_dirname(template_dirpath.name)
        self.template_filepaths = {filepath.resolve() for glob in globs \
            for filepath in template_dirpath.glob(glob)}
    
    def __bool__(self) -> bool:
        return bool(self.template_filepaths)
    
    def split_dirname(self, filename: str) -> Tuple[str, Tuple[str]]:
        parts = filename.split(self.key_val_delim)
        return parts[0], tuple(parts[1:-1])
    
    def split_filename(self, filename: str) -> Tuple[str, Tuple[str], str]:
        parts = filename.split(self.key_val_delim)
        return parts[0], tuple(parts[1:-1]), parts[-1]
    
    def matches(self, include_filename: str) -> bool:
        name, values, _ = self.split_filename(include_filename)
        return name == self.name and len(self.keys) == len(values)
    
    @staticmethod
    def to_cname(
        text: str,
        substitutions: Tuple[Tuple[str, str], ...] = (
            (r' ', '_'),
            (r'const', 'c'),
            (r'\*', 'p'),
            (r'_t', '')
        )
    ) -> str:

        for k,v in substitutions:
            text = re.sub(k, v, text)
        
        return text
    
    @staticmethod
    def sub_values(
        filetext: str,
        keys: Tuple[str],
        values: Tuple[str]
    ) -> str:

        substitutions: List[Tuple[str, str, str]] \
            = list(zip(keys, values, (Template.to_cname(v) for v in values)))

        substitutions.sort(key=lambda p : len(p[0]), reverse=True)

        for k, v, sv in substitutions:
            k = re.escape(k)
            filetext = strip_macro('define', k, filetext)
            filetext = re.sub(r'(?<=[A-Z]_){}(?=(_[A-Z]|$))'.format(k), sv.upper(), filetext)
            filetext = re.sub(r'(?<=[a-z]_){}(?=(_[a-z]|$))'.format(k), sv, filetext)
            filetext = re.sub(r'(?<=(\.|_)){}(?=(\.|_|$))'.format(k), v, filetext)
            filetext = re.sub(r'\b{}\b'.format(k), v, filetext)
        
        filetext = strip_macro('define', 'A', filetext)
        filetext = strip_macro('define', 'P', filetext)
        filetext = strip_macro('define', 'R', filetext)
        
        return filetext
    
    def generate_targets(self,
        include_filename: str,
        target_dirpath: Path,
        include_dirpaths: Set[Path] = set()
    ) -> Set[Path]:

        values: Tuple[str, ...] = self.split_filename(include_filename)[1]

        if not target_dirpath.exists():
            target_dirpath.mkdir(parents=True)
                
        target_filepaths: Set[Path] = set()

        for source_filepath in self.template_filepaths:
            target_filename: str = Template.sub_values(source_filepath.name, self.keys, values)

            target_filetext: str = Template.sub_values(source_filepath.read_text(), self.keys, values)
            target_filetext = sub_forwarded_funcdecs(target_filetext)
            target_filetext = sub_forwarded_funcdefs(target_filetext)
            target_filetext = strip_extra_newlines(target_filetext)

            target_filepath: Path = target_dirpath.joinpath(target_filename)
            target_filepath.write_text(target_filetext)
            target_filepaths.add(target_filepath)
        
        include_dirpaths.add(target_dirpath)
        for target_filepath in target_filepaths:            
            missing_typedefs = get_missing_typedefs(target_filepath, include_dirpaths)
            if missing_typedefs:
                target_filetext: str = target_filepath.read_text()
                target_filetext = insert_includes(target_filetext, {Path(path.name) for path in missing_typedefs.values()})
                target_filepath.write_text(target_filetext)

        return target_filepaths
    
    @staticmethod
    def get_templates(
        template_root_dirpaths: Set[Path],
        template_dir_glob: str = '*.t',
        template_file_globs: Set[str] = {'*.h', '*.c'}
    ) -> List[Template]:

        template_dirpaths: Set[Path] = set()
        for template_root_dirpath in template_root_dirpaths:
            template_dirpaths.update(template_root_dirpath.glob(template_dir_glob))
        
        templates: List[Template] = []
        for template_dirpath in template_dirpaths:
            template = Template(template_dirpath, template_file_globs)
            if(template):
                templates.append(template)
        return templates
        
    def __str__(self) -> str:
        return 'name:{} path:{} keys:{} sources:{}'.format(   
            self.name,
            self.template_dirpath.name,
            ','.join(self.keys),
            ','.join(fp.name for fp in self.template_filepaths)
        )
    
    def __repr__(self) -> str:
        return '<{}>'.format(self.__str__())
