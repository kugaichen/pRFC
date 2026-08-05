# pRFC software implementation

This directory contains the C++17 research prototype for building and
simulating an improved
Recursive Flow Classification (RFC) packet classifier. It preprocesses a
ClassBench-style rule set, divides rules into four sub-classifiers, builds the
sub-classifiers in parallel, and can optionally simulate packet lookup.

## Architecture

```text
ClassBench rules
      |
      v
FilterReader -> Filter_Preprocess -> L_L / L_H / H_L / H_H
                                      |      |      |      |
                                      +------v------+------+
                                      parallel RFC construction
                                                |
                              optional packet-trace simulation
```

- `FilterReader.*`: parses rules and expands IPv4 prefixes into six dimensions.
- `Filter_Preprocess.*`: selects source/destination masks and divides the rules.
- `Hardware_ImprovedClassifierBuilding.*`: builds the four improved RFC tables.
- `Hardware_Improved_statemachine.*`: simulates lookup and hardware cycle cost.
- `Hardware_RFC_*` and `Hardware_state_machine.*`: baseline RFC implementation.
- `PacketLoad.*`: parses packet-trace records.
- `data/`: generated/reference classifier memory initialization files.

## Requirements

- CMake 3.16 or newer
- A C++17 compiler (GCC, Clang, or MSVC)
- A thread implementation supported by CMake

## Build

From this `software/` directory:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

On a single-config generator (for example MinGW Makefiles), the executable is
usually `build/prfc`. On Visual Studio generators it is usually
`build/Release/prfc.exe`.

Run the data-independent command-line smoke tests with:

```bash
ctest --test-dir build -C Release --output-on-failure
```

## Run

Build classifier tables only:

```bash
./build/prfc <rules-file>
```

The repository includes a small smoke-test corpus:

```bash
./build/prfc examples/rules.txt
./build/prfc examples/rules.txt examples/trace.txt
```

Build tables and simulate a packet trace:

```bash
./build/prfc <rules-file> <packet-trace-file>
```

Use `prfc --help` to print the command syntax. Paths may be absolute or relative
to the current working directory.

### Rule format

Each non-empty rule starts with `@` and contains six tab-separated fields:

```text
@src-ip/prefix<TAB>dst-ip/prefix<TAB>src-port-start:src-port-end<TAB>dst-port-start:dst-port-end<TAB>protocol/mask<TAB>flags/mask
```

The parser currently follows the original prototype's ClassBench assumptions.
Malformed numeric fields are reported as errors by the executable.

### Packet-trace format

Packet records are tab-separated. The lookup path consumes, in order, the
decimal 32-bit source IP, decimal 32-bit destination IP, source port, and
destination port; additional trace columns are ignored.

## Outputs and repository data

The repository includes existing `.mem` and parameter files used by the FPGA
workflow. A lookup simulation may write `lookupresult.txt`; runtime lookup
results and local build products are excluded by `.gitignore`.

The checked-in memory files are large (tens of megabytes). Before publishing,
review whether every generated data file is needed for reproducibility. If they
must remain and later grow beyond GitHub's per-file limit, consider Git LFS.

## Known limitations

- Input validation reflects a research prototype and is not hardened for
  untrusted files.
- The classifier uses raw-pointer-backed tables in parts of the original data
  structure. Long-running embedding applications should add explicit ownership
  tests before reusing the library API.
- There is no automated correctness corpus in this repository yet.

## Contributing

Please keep changes focused, compile with warnings enabled, and document the
rule/trace corpus used for behavioral comparisons. Bug reports should include
the compiler, platform, command line, and a minimal reproducible input.

## License

This project is released under the [MIT License](LICENSE).
