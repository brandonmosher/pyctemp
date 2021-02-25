from pyctemp.template import TemplateFinder, TemplateResolver
from pathlib import Path
from shutil import rmtree

template_finder = TemplateFinder()
templates = template_finder.find()
print(templates)

template_resolver = TemplateResolver()

include_filenames, target_filepaths = template_resolver.resolve_path(Path('test/test_map_int_int.c'), Path('tmp'))

print("Include Filenames")
print('\n'.join(include_filenames))
print()

print("Target Filepaths")
print('\n'.join(str(p) for p in target_filepaths))
print()

rmtree('tmp')