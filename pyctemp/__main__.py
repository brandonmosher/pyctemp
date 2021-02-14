#!/usr/bin/env python3

# pyright: strict

import sys
import pprint

from .target import generate_targets_from_dirpath
from .template import Template
from .cmd_args import get_cmd_args

def main():
    args = get_cmd_args(sys.argv[1:])
    
    pp = pprint.PrettyPrinter(indent=4)

    templates = Template.get_templates(args.template_dirpaths)

    include_filenames, target_filepaths = generate_targets_from_dirpath(
        args.source_dirpath,
        args.target_dirpath,
        templates, args.include_dirpaths
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
