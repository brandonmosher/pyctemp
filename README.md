# pyctemp

pyctemp scans c-files for includes matching available templates, infers template
parameter names and values, and generates the required template targets with paramater substitutions. pyctemp can generate nested templates, automatically resolve missing typedef includes, and generate by-value and by-reference versions of functions.

While templating is possible to some extent in C using only preprocessor macros, it is quite ugly. pyctemp is designed to work with a small subset of the C++ template sytax allowing for cleaner source files while still supporting ide linting. Care has been taken to avoid replicating the functionality of the C preprocessor or the C parser. pyctemps distinguishing feature, therefore, is it's ability to perform text substitutions not possible with the C preprocessor while avoiding the complexity of the C parser. This simplicity makes pyctemp easily adaptable to different needs. Included as examples are templates for dynamic array, linked list, and map (hash table) containers.

## installation
    git clone https://github.com/mosherbd/pyctemp

### Run from the command line
    cd pyctemp
    pip3 install .

### Run from Python
    python -m pyctemp

### Use as a module
    from pyctemp.template import Template

## Usage

pyctemp [-h] [--source-dirpath SOURCE_DIRPATH]
               [--template-dirpath TEMPLATE_DIRPATHS]
               [--target-dirpath TARGET_DIRPATH]
               [--include-dirpath INCLUDE_DIRPATHS] [-q]

 -h, --help

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Show this help message and exit

--source-dirpath SOURCE_DIRPATH

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dirpath containing source files to be scanned

--template-dirpath TEMPLATE_DIRPATHS

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dirpath containing templates

--target-dirpath TARGET_DIRPATH

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dirpath in which targets are placed. If the directory does not exist, it will be created.

--include-dirpath INCLUDE_DIRPATHS

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Dirpath in which targets are placed

-q, --quiet

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Suppress output

## Templates

### Template Directories
Template directories are any directory with the ".t" extension that are immediate subdirectories of the specified template dirpaths. The template name and parameters are specified in the template directory name. The files in the template directory are referred to as targets. When an include file matching a template is found in a source file, template parameters values are inferred, and targets are generated in the specified target directory.

### Template Name and Parameter Names

The template name and parameter names are specified in the template directory name as follows:

[name]["." delimited parameter names].[file extension]

For example, the template directory name "map.K.V.t" would yield map as the template name, K and V as the template paramter names and .t as the file extension.  

### Template Matching and Parameter Values

Files in the specified source-dirpath are scanned for includes that match known templates.

For example, if main.c specifies #include "map.int.int.h", this would match the "map.K.V.t" template. The parameter values would be inferred as K="int", V="int", and all targets in the map.K.V.t directory would be generated with those substitutions and placed in the specified target directory.

### Template Substituions

All instances of parameter names occuring at word, whitespace, or punctuation boundaries are substituted with the corresponding parameter value.

Parameter names that occur in variable names or macros are modified before substitution.

For exampled, if the paramter name "T" corresponds to the parameter values "const int *" then:
```
int func(T arg); -> int func(const int * arg)
```

whereas:

```
int func_T(T arg); -> int func_c_int_p(const int * arg)
```

### Preprocess Directive Stripping

You may wish to include preprocessor define directives in your templates that provide parameter values for linting. These directives will be autoatically removed from generated targets.

For example, the following define would allow IDE linting, but would be stripped from targets generated for the array.T.t template.

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

pyctemp generates templates recursively making it possible to nest templates. For example, the map.K.V.t template, requires array.bucket_K_V_t.h. pyctemp first substitutes the the K and V parameters and then generates the required target using the array.T.t template.

### Missing Typedefs

pyctemp will search for missing typedefs in the specified target dirpath and include dirpaths and insert them into the targets it generates.

For example, the map.K.V.t template, requires array.bucket_K_V_t.h which is generated from the array.T.t template. The array.T.t template targets need the definition of the bucket_K_V_t type at compile time. array.T.h signals this requirement with the line:

"typedef T _T"

where, in this case, T=bucket_K_V_t. pyctemp will detect that array.T.t requires the definition of the type T and that this definition is not present in the current file or any included files. It will scan the target and include dirpaths for a matching definition, and, if found, insert the missing include directive into the target.

### Example Directory Structure

    pyctemp --source-dirpath src --template-dirpath template --target-dirpath include

```
project
│___ src
│       main.c
│___ template
│    │___ template.T.d
│    │       template.T.h
│    │       template.T.c
│    │...
...
```
