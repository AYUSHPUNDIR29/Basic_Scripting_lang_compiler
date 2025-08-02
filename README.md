# My Language Compiler

A compiler implementation for a custom programming language.

## Team Members and their github link
- [@ayushpundir](https://github.com/AYUSHPUNDIR29)
- [@ireshchand](https://github.com/Iresh014)
-[@kartikmishra](https://github.com/KARTIKmishra001)
## Project Structure

```
.
├── include/         # Header files
├── src/            # Source files
├── tests/          # Test cases
│   ├── lexical/    # Lexical analysis tests
│   ├── syntax/     # Syntax parsing tests
│   ├── semantic/   # Semantic analysis tests
│   ├── codegen/    # Code generation tests
│   └── integration/# Integration tests
├── build/          # Build output directory
├── main.c          # Main entry point
├── Makefile        # Build configuration
└── README.md       # This file
```

## Building the Project

To build the compiler:

```bash
make
```

This will create a `compiler` executable in the root directory.

## Running Tests

To run the test suite:

```bash
make test
```

## Cleaning Build Files

To clean build artifacts:

```bash
make clean
```

## Usage

After building, you can compile a source file:

```bash
./compiler <input_file>
```

## Development

- Source files are located in the `src/` directory
- Header files are in the `include/` directory
- Add new test cases in the appropriate subdirectory under `tests/`
