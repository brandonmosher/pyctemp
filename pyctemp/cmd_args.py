# pyright: strict

from typing import List, Union
import argparse
from pathlib import Path

class PathResolveAction(argparse.Action):
    def __call__(self, parser: argparse.ArgumentParser, namespace: argparse.Namespace, path: Path, option_string: Union[str, None] = None):
        path = path.resolve()
        setattr(namespace, self.dest, path)

class PathResolveAppendAction(argparse.Action):
    def __call__(self, parser: argparse.ArgumentParser, namespace: argparse.Namespace, path: Path, option_string: Union[str, None] = None):
        path = path.resolve()
        if hasattr(namespace, self.dest):
            getattr(namespace, self.dest).add(path)
        else:
            setattr(namespace, self.dest, {path})

def get_cmd_args(argv: List[str]):
    parser = argparse.ArgumentParser(description='Scan c-files for includes matching available templates, infer template parameters, and generate template instantiations.')

    parser.add_argument(
        '--source-dirpath',
        dest='source_dirpath',
        type=Path,
        action=PathResolveAction,
        default=Path("src"),
        help='dirpath containing source files to be scanned')

    parser.add_argument(
        '--template-dirpath',
        dest='template_dirpaths',
        type=Path,
        action=PathResolveAppendAction,
        default=set(),
        help='dirpath containing templates')
    
    parser.add_argument(
        '--target-dirpath',
        dest='target_dirpath',
        type=Path,
        action=PathResolveAction,
        default=Path("include"),
        help='dirpath in which targets are placed')

    parser.add_argument(
        '--include-dirpath',
        dest='include_dirpaths',
        type=Path,
        action=PathResolveAppendAction,
        default=set(),
        help='dirpath in which targets are placed')
        
    parser.add_argument(
        '-q',
        '--quiet',
        dest='quiet',
        action='store_true',
        help='suppress output')
    
    return parser.parse_args(argv)
