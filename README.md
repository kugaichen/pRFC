# pRFC

<img width="2714" height="2554" alt="figure_2" src="https://github.com/user-attachments/assets/3913d30f-40b8-4c87-bce6-1ff7769b0a4e" />

pRFC is an open-source research prototype for packet classification. This
repository combines:

- a C++17 offline implementation for rule preprocessing, heuristic
  partitioning, RFC table construction, and cycle-oriented lookup simulation;
- a synthesizable Verilog implementation of the four-way parallel classifier;
- BRAM initialization data and a Verilog testbench for the current hardware
  configuration.

The implementation accompanies the architecture described as **Memory-Efficient
Packet Classification at High-Speed: The pRFC Architecture with Heuristic
Partitioning**.

## Repository layout

```text
.
|-- CMakeLists.txt          # Root build entry for the C++ implementation
|-- HW_source/              # Synthesizable Verilog RTL
|-- data/                   # BRAM tables currently consumed by the RTL
|-- sim/                    # Verilog testbench
|-- log/                    # Reference simulation log
|-- software/               # C++ implementation and its documentation
|   |-- CMakeLists.txt
|   |-- examples/           # Small rule and packet-trace examples
|   `-- data/               # Software-side/reference generated tables
`-- LICENSE
```

The two `data/` directories are intentionally separate. Files with the same
name are not necessarily identical:

- `data/` is the checked-in hardware configuration referenced by RTL
  `$readmemh("./data/...")` calls;
- `software/data/` is retained with the software implementation as its
  software-side/reference output set.

Do not replace the hardware tables with the software-side files without also
checking `classifier_parameters.v`, table depths, address widths, and the
expected classification results in simulation.

## End-to-end flow

```text
ClassBench-style rules
        |
        v
rule parsing and six-dimension expansion
        |
        v
heuristic mask selection and four-way partitioning
        |
        +----> L_L
        +----> L_H       parallel RFC table construction
        +----> H_L
        `----> H_H
                  |
                  v
       phase-0 / phase-1 / final tables
                  |
          +-------+--------+
          |                |
     C++ lookup       Verilog BRAM lookup
     simulation       and pipeline simulation
```

## Software implementation

### Requirements

- CMake 3.16 or newer
- A C++17 compiler (GCC, Clang, or MSVC)
- Thread support discoverable by CMake

### Build from the repository root

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

With a single-config generator such as MinGW Makefiles, the executable is
normally `build/software/prfc` (`prfc.exe` on Windows). With a multi-config
generator, it may be under `build/software/Release/`.

### Run the included examples

```bash
./build/software/prfc software/examples/rules.txt
./build/software/prfc software/examples/rules.txt software/examples/trace.txt
```

The first command preprocesses the rules and builds the four classifiers. The
second command additionally runs the packet lookup state-machine simulation.
See [`software/README.md`](software/README.md) for input formats, component
responsibilities, outputs, and software limitations.

## Hardware implementation

### Top-level interface

The hardware top module is `prfc_top` in `HW_source/prfc_top.v`.

- Clock/reset: `clk`, active-low `rst_n`.
- Input handshake: `packet_valid`, `packet_ready`.
- Output handshake: `result_valid`, `result_ready`.
- Backpressure: the final stage stalls while a valid result is waiting and
  `result_ready` is low.

### Pipeline

The current RTL implements four classifiers in parallel and a five-stage data
path:

1. **Parse/input register**: latch the packet dimensions when the input is
   accepted.
2. **Phase 0 lookup**: address six compressed dimension tables and obtain
   intermediate equivalence IDs.
3. **Phase 1 lookup**: combine phase-0 IDs into two second-level table keys.
4. **Final lookup**: combine phase-1 IDs and read one result from each parallel
   classifier.
5. **Result selection**: select the numerically smallest valid rule ID and
   present it through the output ready/valid interface.

The BRAM wrappers are initialized from the root `data/` directory. Run
simulation with the repository root as the working directory, or configure the
EDA tool so the relative `./data/...` paths resolve correctly.

### Vivado/EDA simulation

1. Add the synthesizable files under `HW_source/` to the design source set.
2. Add `sim/prfc_top_tb.v` to the simulation source set.
3. Set `prfc_top` as the design top and `prfc_top_tb` as the simulation top.
4. Ensure all files under the root `data/` directory are available to the
   simulator working directory.
5. Run the simulation and compare its output with `log/prfc_test.log`.

No generated Vivado project, device constraint file, or board-specific build
script is included, so synthesis settings and the target FPGA must be supplied
by the user.

## Verification status

The repository currently provides:

- CMake/CTest command-line smoke tests for the software executable;
- a small software rule and trace corpus under `software/examples/`;
- `sim/prfc_top_tb.v` and `log/prfc_test.log` for hardware simulation reference.

Before using regenerated tables in hardware, verify at minimum:

- phase-0, phase-1, and final table depths and address widths;
- classifier parameters and `.mem` files come from the same generation run;
- hit/miss behavior and priority selection;
- reset, input backpressure, and output backpressure;
- software and RTL results on the same rule and packet corpus.

## Known limitations

- The software parser and data structures are research-prototype code and are
  not hardened for untrusted input.
- The software and hardware data snapshots currently represent different table
  sets; an automated export-and-equivalence pipeline is not yet included.
- Hardware synthesis constraints, generated IP, and board integration files are
  not included.
- Some original C++ algorithm code still emits compiler warnings related to
  signed/unsigned comparisons and an unused parameter.

## Contributing

Keep changes focused and reproducible. For table or algorithm changes, document
the rule corpus, generation command, generated parameters, compiler/EDA tool,
and software-versus-RTL comparison results.

## License

This project is released under the [MIT License](LICENSE).
