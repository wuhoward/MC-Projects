/scala底下內容為有修改過的平台檔案，相對位置同/rocket-chip/src/main/scala

cd rocket-chip/emulator
make CONFIG=DualCoreConfig
cd ~/rocket-chip-multicore-application
make
spike -p4 Mandelbrot.riscv
/rocket-chip/emulator/emulator-TestHarness-DualCoreConfig Mandelbrot.riscv