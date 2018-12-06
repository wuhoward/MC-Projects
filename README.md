# Parallel Mandelbrot Set with ilib on the Rocket Chip

### File Description

  * All files under **/scala** are modified from the original Rocket Chip [source code](https://github.com/freechipsproject/rocket-chip) with the same relative path correspoinding to **/rocket-chip/src/main/scala**.
  * I implemented the Mandelbrot Set algorithm on the Rocket Custom Coprocessor (RoCC) accelerators for Rocket Chip. The Chisel code is in **/scala/rocket/rocc.scala**
  * For comparison, the software counterpart of the Mandelbrot Set algorithm is in **rocket-chip-multicore-application/Mandelbrot.c**, where I've tried both shared memory and message passing methods.
  * To ease my task, I implemented the **send** and **receive** function calls of ilib (Tilera's MPI library) in **rocket-chip-multicore-application/common/util.h**
  * Detailed information of implementation and experimental result are summarized in **Final_105062635_v1.pdf**.

### Compilaton

  ```shell
  cd rocket-chip/emulator
  make CONFIG=DualCoreConfig
  cd ~/rocket-chip-multicore-application
  make
  spike -p4 Mandelbrot.riscv
  /rocket-chip/emulator/emulator-TestHarness-DualCoreConfig Mandelbrot.riscv
  ```

