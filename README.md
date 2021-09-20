# pyctemp

## What
pyctemp is essentially a powerful find-and-replace tool to instantiate C template code using c++-like syntax. It scans c-files for includes matching specified templates, infers template
parameter names and values, and generates template targets with those parameter substitutions applied.

## Why
While templating is possible in C using only preprocessor macros, it requires ugly and unnatural code. pyctemp is designed to mimic a subset of the C++ template syntax allowing for cleaner source files while still supporting ide linting. Care has been taken to avoid replicating the functionality of a C preprocessor or parser. pyctemp's distinguishing feature, therefore, is it's ability to perform text substitutions not possible with the C preprocessor while avoiding the complexity of the C parser. This simplicity makes pyctemp easily adaptable to different needs. Included as examples are templates for dynamic array, linked list, and map (hash table) containers.

## Features
- Recursively generate targets that include nested templates
- Automatically resolve missing typedef includes
- Generate by-value and by-reference versions of functions
- Template aliases
- No third party libraries required
- Can be run via the command line or imported as a module

## Installation
    git clone https://github.com/brandonmosher/pyctemp

### Run from the command line
    cd pyctemp
    pip3 install .
    pyctemp ...

### Run from Python
    python -m pyctemp ...

### Use as a module
    from pyctemp.template import TemplateResolver
    ...

### Examples and Running tests

See the tests folder for basic examples.

To run all .c tests run the following from the top-level pyctemp directory

    make

To run a specific .c tests run the following from the top-level pyctemp directory:

    make test/[test filename]

To run .py tests:

    python test/[test filename].py

### Quick Start

#### Project Structure

Below is the recommended directory structure for a project using pyctemp. Using this structure, pyctemp automatically detects the templates in the template folder when run from the project directory.

```
project
│___ src
│       main.c
│___ template
│    │___ example.T.d
│    │       example.T.h
│    │       example.T.c
│    │...
...
```
#### Template Code

example.T.h
```
// Placeholder to enable linting
#define T int

// Tells pyctemp to locate missing typedefs in case T is not a built-in type
#typedef T _T;

struct typedef {
    T data;
} example_t;

// Using reserved _R and P parameter names to generate by-value and by-reference versions of this function.

void example_init_R(example_t* example, T P data);
...
```

example.T.c
```
#include "example.T.h"

void example_init_R(example_t* example, T P data) {
    example->data = P(data);
}
...
```

#### Source Code

main.c
```
#include "example.int.h"

int main() {
    example_t example;
    example_init(&example, 42);
}
...
```

#### Run pyctemp

Running the following command scans all c files in the src directory for include directives matching templates in the template directory, creates the include directory, and places the generated targets there.

pyctemp --source-dirpath src --target-dirpath include

## Command Line Usage

pyctemp [-h] [--source-dirpath SOURCE_DIRPATH] [--template-dirpath TEMPLATE_DIRPATHS] [--no-template-dirpaths-default] [--target-dirpath TARGET_DIRPATH] [--include-dirpath INCLUDE_DIRPATHS] [--typedef-include-filepath TYPEDEF_INCLUDE_FILEPATHS] [-q]

    -h, --help

Show this help message and exit

    --source-dirpath SOURCE_DIRPATH

Dirpath containing source files to be scanned

    --template-dirpath TEMPLATE_DIRPATHS

Dirpath containing templates

    --no-template-dirpaths-default

Do not automatically scan default template directories.

    --target-dirpath TARGET_DIRPATH

Dirpath in which targets are placed. If the directory does not exist, it is created.

    --include-dirpath INCLUDE_DIRPATHS

Include directories to be scanned for missing typedefs

    --typedef-include-filepath

Specifies a typedef and the filepath containing that typedef in the form [typename]:[filepath]. This filepath is included in any target for which the corresponding typedef is required and missing. Useful for types defined in system headers.

    -q, --quiet

Suppress output

## The Details

### Template Directories

Any immediate subdirectory of a template dirpath with the ".t" suffix is considered a template directory. The template name and parameter names are specified by the template directory name. The files in the template directory are referred to as targets. When an include file matching a template is found in a source file, the template parameter values are inferred by comparing the include filename and the template directory name. A copy of each template target is placed in the specified target directory. All instances of the template parameter names in the target filename and filetext are replaced with the template parameter values.

### Default Template Directories

When run from the command line, pyctemp searches it's built-in templates and, if it exists, the "template" directory in the current working directory. This behavior can be disabled using the --no-template-dirpaths-default command line option. When using pyctemp as a module, this behavior can be replicated by instantiating the TemplateResolver class without providing the "templates" argument or by using the TemplateFinder.add_template_dirpaths_default method.

### Template Name and Parameter Names

The template name and parameter names are specified in the template directory name as follows:

    [name]["." delimited parameter names].[file extension]

For example, the template directory name "map.K.V.t" yields map as the template name, K and V as the template parameter names and .t as the file extension.  

### Template Matching and Parameter Values

Files in the specified source-dirpath are scanned for include directives that match known templates. An include directive matches a template if the include filename matches the template name and has the same number of parameter values as the template has parameter names.

For example:

    #include "map.int.int.h"
    
matches the map.K.V.t template. The parameter values are inferred as K="int", V="int". All targets in the map.K.V.t directory are generated with those substitutions and placed in the specified target directory.

Warning: This is prone to aliasing. e.g. map.int.int.h would match both a map.K.V.t templates, and a map.A.B.t template. In this case, it would be necessary to differentiate these templates changing the template name.

### Template aliases

Source file include directives may specify an alias under which the matching template is generated. This can provide better abstractions and produce more readable code.

For example:

    #include "array.point_t.h=line.h"

tells pyctemp to generate targets for the array.T.t template with all instances of "array_point" replaced with "line" allowing, for instance:

    array_point_t array_point;
    array_point_init(array_point);
    ...

to be replaced with:

    line_t line;
    line_init(line);
    ...

### Template Substitutions

All instances of parameter names occurring at word, whitespace, or punctuation boundaries are substituted with the corresponding parameter value.

Parameter names that occur in variable names or macros are modified before substitution.

For example, if the parameter name "T" corresponds to the parameter value "const int *" then:

    int func(T arg); -> int func(const int * arg)

whereas:

    int func_T(T arg); -> int func_c_int_p(const int * arg)

### Preprocessor Directive Stripping

You may wish to include preprocessor define directives in your templates to provide placeholder parameter values. This prevents linting from flagging these tokens as undefined. These directives are automatically removed from generated targets.

For example, the following define is stripped from targets generated for the array.T.t template.

#define T int

### Function Forwarding

In some cases it is desirable to have the option of generating a template which passes function arguments by reference rather than by value. pyctemp accomplishes this by generating two versions of properly annotated functions, one which takes it's arguments by value and another which takes it's arguments by reference.

Criteria:

- Function name ends in "_R" 

- Use parameter name P in place of the pointer/dereference operator *  

- Use parameter name A in place of the addressof operator

For example, the trivial function:

```
int P func_R(int P val) {
    P(val)++;
    return val;
}
```
generates the following:
```
int func(int val) {
    (val)++;
    return val;
}

int * func_r(int * val) {
    *(val)++;
    return val;
}
```
### Nested Templates

pyctemp generates templates recursively making it possible to nest templates. For example, the map.K.V.t template requires array.bucket_K_V_t.h. pyctemp first generates the map.K.V.t targets with the K and V parameter substitutions. It then scans those targets for template includes and, int this case, generates the required target using the array.T.t template.

### Missing Typedefs

A typedef is considered missing if a target contains a type declaration, but neither it nor its immediate includes contain a the type definition. This issue typically arises when providing a system or user-defined type as a parameter value.

pyctemp attempts to resolve this issue automatically by:
1. Searching for type definitions in the specified include dirpaths and target dirpath
2. Inserting include directives for any type definitions it finds into the targets that require them.

For example, the map.K.V.t template, requires array.bucket_K_V_t.h which is generated from the array.T.t template. The array.bucket_K_V_t.h in turn requires the bucket_K_V_t to compile successfully. array.T.h signals this requirement with the line:

    typedef T _T

which, after substitution, becomes:

    typedef bucket_K_V_t _bucket_K_V_t

pyctemp detects that array.bucket_K_V_t.h has declared, but not defined, bucket_K_V_t and searches for a matching definition. If found, it inserts the missing include directive into array.bucket_K_V_t.h.

### Typedef Include Filepaths

You may manually specify which files to include for certain typedefs. This is especially useful in the case of types defined in system headers as pyctemp performs no preprocessing before scanning these files for type definitions.

This can be done via the "typedef-include-filepath" command line option or by providing a dictionary of typename-filepath pairs to the TemplateResolver class.

For example:

    #include "array.uint8_t.h"

will match the array.T.t template and generate the corresponding targets. pyctemp will detect the missing type definition via:

    //array.uint8_t.h
    ...
    typedef uint8_t _uint8_t
    ...

specifying "--typedef-include-filenames uint8_t:inttypes.h" tells pyctemp to resolve this by adding an include directory for inttypes.h.
