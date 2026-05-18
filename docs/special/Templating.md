# Templating

Because dynamic ✨ stuff ✨ is supported, many input fields support templating. Anything that supports templating is explicitly labelled as supporting it somewhere near its description. 

Everything that supports templates will be marked as such somewhere. It is mentioned in the markdown documentation, but a condensed form is available in `umbra some-module --help`

Example from `--help`:

```
POSITIONALS
  layout TEXT:Supports templates [default.kdl] 
                              Path or name of the layout 
```

## Syntax 

The syntax is inspired by various HTML templating libraries, but is heavily stripped down. Only variable expansion is supported. Therefore, the full syntax descriptor is:
```
{{ identifier_name }}
```

This may expand in the future, so the following characters are reserved for syntax use:

* `|`
* `()`

## Valid identifiers

### Standard identifiers

Standard identifiers are built-in and manually supported identifiers. Using them has three possible outcomes:

1. The expected value
2. A fallback value if the expected value is unavailable
3. Abort scenarios, where a hard error is generated. This is reserved for very specific scenarios, such as the binary required for the identifier outright missing.

#### `git_root`

Path to the git root.

* Expected value: `/path/to/git/repo`
* Fallback: `./` if not in a git repo
* Aborts: if the `git` executable is not found

#### `curr_folder_name`

The name of the working directory folder

* Expected value: `working_dir` for `/path/to/working_dir`
* Fallback: None, there is always a working dir
* Aborts: Never, there is always a working dir

#### `cwd`

Full path to the current working directory

* Expected value: `/path/to/working/dir`
* Fallback: None, there is always a working dir
* Aborts: Never, there is always a working dir

### Environment variables (not implemented, may not be implemented)

Environment variables are expanded with
```
{{ $ENV_VARIABLE_NAME }}
```

