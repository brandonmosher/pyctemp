# pyright: strict

from __future__ import annotations

from typing import Tuple, List, Set, Dict
from pathlib import Path
import re
from .strip import strip_macro, strip_extra_newlines
from .function import sub_forwarded_funcdecs, sub_forwarded_funcdefs
from pkg_resources import resource_filename
from pathlib import Path
from .type import insert_missing_typedef_includes
from .include import get_include_filenames_from_dirpath, get_include_filenames_from_filepath

class Template:
    template_dirpath: Path
    template_globs: Set[str]
    key_val_delim: str
    alias_filename_delim: str

    def __init__(self,
        template_dirpath: Path,
        template_globs: Set[str] = {'*.h', '*.c'},
        key_val_delim: str = '.', 
        alias_filename_delim: str = '='
    ) -> None:

        self.template_dirpath = template_dirpath
        self.key_val_delim = key_val_delim
        self.alias_filename_delim = alias_filename_delim
        self.template_globs = template_globs
    
    @staticmethod
    def to_cname(
        text: str,
        substitutions: Tuple[Tuple[str, str], ...] = (
            (r'( |\.)', '_'),
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

        for k, v, cname_v in substitutions:
            k = re.escape(k)

            # remove #define T int
            filetext = strip_macro('define', k, filetext)
            
            # UPPER_T_UPPER -> UPPER_INT_UPPER
            filetext = re.sub(r'(?<=[A-Z]_){}(?=(_[A-Z]|$))'.format(k), cname_v.upper(), filetext)
            
            # lower_T_lower -> lower_int_lower
            filetext = re.sub(r'(?<=[a-z]_){}(?=(_[a-z]|$))'.format(k), cname_v, filetext)

            # T. .T T_ _T (T -> int. .int int_ _int (int etc... 
            filetext = re.sub(r'(?<=( |\(|\.|_)){}(?=(\.|_|$))'.format(k), v, filetext)
            
            # T -> int
            filetext = re.sub(r'\b{}\b'.format(k), v, filetext)
        
        # array_some_t_t -> array_some_t
        filetext = re.sub(r'_t_', '_', filetext)

        # Strip macros reserved for function forwarding
        filetext = strip_macro('define', 'A', filetext)
        filetext = strip_macro('define', 'P', filetext)
        filetext = strip_macro('define', 'R', filetext)
        
        return filetext
    
    @property
    def name(self) -> str:
        return self.split_template_dirname()[0]
    
    @property
    def keys(self) -> Tuple[str]:
        return self.split_template_dirname()[1]

    @property
    def source_filepaths(self) -> Set[Path]:
        return {filepath.resolve() for template_glob in self.template_globs \
            for filepath in self.template_dirpath.glob(template_glob)}
        
    def split_template_dirname(self) -> Tuple[str, Tuple[str]]:
        parts = self.template_dirpath.name.split(self.key_val_delim)

        # name, keys
        return parts[0], tuple(parts[1:-1])
    
    def split_include_filename(self, filename: str) -> Tuple[str, Tuple[str], str, str]:
        filename, alias_filename = filename.split(self.alias_filename_delim) if self.alias_filename_delim in filename else (filename, "")
        parts = filename.split(self.key_val_delim)

        # name, values, filesuffix, alias_filename
        return parts[0], tuple(parts[1:-1]), parts[-1], alias_filename
    
    def matches(self, include_filename: str) -> bool:
        name, values, _, _ = self.split_include_filename(include_filename)
        return name == self.name and len(self.keys) == len(values)
    
    def generate_targets(self,
        include_filename: str,
        target_dirpath: Path,
    ) -> Set[Path]:

        _, values, _, alias_filename = self.split_include_filename(include_filename)

        if alias_filename:
            target_dirpath.joinpath(include_filename).write_text('#include "{}"\n'.format(alias_filename))
        
        alias_filestem = Path(alias_filename).stem
                
        template_instance_name: str = ".".join((self.name, *self.keys))
        template_instance_cname: str = Template.to_cname(template_instance_name)

        if not target_dirpath.exists():
            target_dirpath.mkdir(parents=True)
                
        target_filepaths: Set[Path] = set()

        for source_filepath in self.source_filepaths:
            target_filename: str = source_filepath.name
            if alias_filename:
                target_filename = re.sub(template_instance_name, alias_filestem, target_filename)
            target_filename = Template.sub_values(target_filename, self.keys, values)

            target_filetext: str = source_filepath.read_text()

            if alias_filestem:
                target_filetext = re.sub(template_instance_name, alias_filestem, target_filetext)
                target_filetext = re.sub(template_instance_cname, alias_filestem, target_filetext)
                target_filetext = re.sub(template_instance_cname.upper(), alias_filestem.upper(), target_filetext)
            
            target_filetext: str = Template.sub_values(target_filetext, self.keys, values)
            target_filetext = sub_forwarded_funcdecs(target_filetext)
            target_filetext = sub_forwarded_funcdefs(target_filetext)
            target_filetext = strip_extra_newlines(target_filetext)
            
            target_filepath: Path = target_dirpath.joinpath(target_filename)
            target_filepath.write_text(target_filetext)
            target_filepaths.add(target_filepath)
        
        return target_filepaths
    
    def __bool__(self) -> bool:
        return bool(self.source_filepaths)
    
    def __str__(self) -> str:
        return 'name:{} path:{} keys:{} sources:{}'.format(   
            self.name,
            self.template_dirpath.name,
            ','.join(self.keys),
            ','.join(fp.name for fp in self.source_filepaths)
        )
    
    def __repr__(self) -> str:
        return '<{}>'.format(self.__str__())

class TemplateFinder:
    template_dirpaths: Set[Path]
    template_dir_globs: Set[str]
    template_file_globs: Set[str]

    def __init__(self,
        template_dirpaths: Set[Path] = set(),
        template_dir_globs: Set[str] = {'*.t'},
        template_file_globs: Set[str] = {'*.h', '*.c'}
    ) -> None:

        self.template_dirpaths = template_dirpaths
        if not template_dirpaths:
            self.add_template_dirpaths_default()
        self.template_dir_globs = template_dir_globs
        self.template_file_globs = template_file_globs
    
    def find(self) -> List[Template]:
        template_dirpaths: Set[Path] = set()
        for template_dirpath in self.template_dirpaths:
            for template_dir_glob in self.template_dir_globs:
                template_dirpaths.update(template_dirpath.glob(template_dir_glob))
        
        templates: List[Template] = []
        for template_dirpath in template_dirpaths:
            template = Template(template_dirpath, self.template_file_globs)
            if(template):
                templates.append(template)
        return templates
    
    def add_template_dirpaths_builtin(self) -> None:
        pathnames: Set[str] = {'template'}
        for pathname in pathnames:
            self.template_dirpaths.add(Path(resource_filename(__name__, pathname)))

    def add_template_dirpaths_cwd(self) -> None:
        paths: Set[Path] = {Path('template')}
        for path in paths:
            if path.exists():
                self.template_dirpaths.add(path.resolve())
    
    def add_template_dirpaths_default(self) -> None:
        self.add_template_dirpaths_builtin()
        self.add_template_dirpaths_cwd()

class TemplateResolver:
    templates: List[Template]
    include_dirpaths: Set[Path]
    typedef_include_filepaths: Dict[str, Path]

    def __init__(self,
        templates: List[Template] = [],
        include_dirpaths: Set[Path] = set(),
        typedef_include_filepaths: Dict[str, Path] = dict()) -> None:

        self.templates = templates if templates else TemplateFinder().find()
        self.include_dirpaths = include_dirpaths
        self.typedef_include_filepaths = typedef_include_filepaths
    
    def resolve_include_filenames(self,
        include_filenames: Set[str],
        target_dirpath: Path
    ) -> Set[Path]:

        target_filepaths: Set[Path] = set()
        for include_filename in include_filenames:
            for template in self.templates:
                if template.matches(include_filename):
                    target_filepaths.update(template.generate_targets(include_filename, target_dirpath))
        return target_filepaths

    def resolve_path(self,
        source_dirpath: Path,
        target_dirpath: Path
    ) -> Tuple[Set[str], Set[Path]]:

        target_filepaths: Set[Path] = set()
        pending_include_filenames: Set[str] = set()
        completed_include_filenames: Set[str] = set()

        if source_dirpath.is_dir():
            pending_include_filenames.update(get_include_filenames_from_dirpath(source_dirpath))
        else:
            pending_include_filenames.update(get_include_filenames_from_filepath(source_dirpath))
        
        
        while True:
            completed_include_filenames.update(pending_include_filenames)
            for target_filepath in self.resolve_include_filenames(pending_include_filenames, target_dirpath):
                pending_include_filenames.update(get_include_filenames_from_filepath(target_filepath))
                target_filepaths.add(target_filepath)
            pending_include_filenames = pending_include_filenames - completed_include_filenames

            if not len(pending_include_filenames):
                break
        
        include_dirpaths = self.include_dirpaths | {target_dirpath}
        for target_filepath in target_filepaths:
            insert_missing_typedef_includes(target_filepath, include_dirpaths, self.typedef_include_filepaths)

        return completed_include_filenames, target_filepaths
