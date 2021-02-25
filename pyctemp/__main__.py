#!/usr/bin/env python3

# pyright: strict

import sys
import pprint

from .template import TemplateFinder, TemplateResolver
from .__argparse__ import get_cmd_args

def main():
    args = get_cmd_args(sys.argv[1:])
    
    pp = pprint.PrettyPrinter(indent=4)
    template_finder = TemplateFinder(args.template_dirpaths)

    if not args.no_template_dirpaths_default:
        template_finder.add_template_dirpaths_default()
    
    templates = template_finder.find()

    template_resolver = TemplateResolver(
        templates=templates,
        include_dirpaths=args.include_dirpaths,
        typedef_include_filepaths=args.typedef_include_filepaths)
    
    include_filenames, target_filepaths = template_resolver.resolve_path(
        source_dirpath=args.source_dirpath,
        target_dirpath=args.target_dirpath
    )
          
    if not args.quiet:
        print('Templates')
        print()
        pp.pprint(templates)
        print()

        print('Include filenames')
        print()
        pp.pprint(include_filenames)
        print()

        print('Target filenames')
        print()
        pp.pprint(target_filepaths)
        print()

if __name__ == '__main__':
    main()
