/scala���U���e�����ק�L�����x�ɮסA�۹��m�P/rocket-chip/src/main/scala

cd rocket-chip/emulator
make CONFIG=DualCoreConfig
cd ~/rocket-chip-multicore-application
make
spike -p4 Mandelbrot.riscv
/rocket-chip/emulator/emulator-TestHarness-DualCoreConfig Mandelbrot.riscv