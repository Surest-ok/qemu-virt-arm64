# CherryBSP - Board Support Package for Modular CherryOS

CherryBSP provides the board support package for running CherryOS / FreeRTOS SMP on QEMU `virt` AArch64 (ARMv8-A Cortex-A53 multi-core architecture).

## Architecture & Hardware Support

- **Target Machine**: QEMU `virt` (ARMv8-A Cortex-A53, 1-32 CPU cores)
- **Execution Levels**: EL1 (Guest/OS) and EL2 (Hypervisor) execution
- **Interrupt Controller**: ARM GICv3 (Distributor, Redistributor, CPU Interface via system registers)
- **Architecture Timer**: ARM Generic Architecture Timer (CNTP / CNTV)
- **Serial Console**: ARM PrimeCell PL011 UART
- **PCIe Subsystem**: PCIe ECAM host controller interface
- **Linker Configuration**: RAM base 0x40000000 (AArch64 64-bit Flat Virtual Address mapping)

## Hardware Device Model Integration

CherryBSP decouples hardware board initialization from the FreeRTOS SMP kernel:
1. **Zero Kernel Hardware Pollution**: The core scheduler in `kernel/` contains no board-specific header inclusions or hardcoded register accesses. Secondary core platform initialization uses the weak platform hook `vPortPlatformSecondaryCoreInit(core_id)`.
2. **Platform Device Registration**: Board hardware resources are registered into the CherryOS Linux-style `platform_device` model via `bsp_register_platform_devices()`:
   - `arm_gicv3`: GICD (0x08000000) & GICR (0x080A0000) MMIO resources.
   - `arm_arch_timer`: IRQ 30 (Physical timer).
   - `pcie_ecam`: ECAM configuration base 0x10000000 (16MB).

## Directory Layout

```
bsp/
├── include/
│   ├── app_irq.h          # Application IRQ dispatch interfaces
│   ├── bsp_devices.h      # Board platform device registration declarations
│   ├── gic.h              # ARM GICv3 distributor/redistributor drivers
│   ├── gtimer.h           # ARM Generic Architecture Timer drivers
│   ├── interrupt.h        # Hardware interrupt vector definitions
│   ├── platform.h         # QEMU virt memory-map and base address constants
│   └── uart.h             # PL011 UART console driver
├── src/
│   ├── app.c              # Application startup, subsystems initialization & benchmarks
│   ├── app_irq.c          # Top-level IRQ multiplexer
│   ├── bsp_devices.c      # Board platform_device instances registration
│   ├── freertos_vectors.S # FreeRTOS SMP exception vector table
│   ├── gic.c              # GICv3 initialization and SGI/PPI/SPI management
│   ├── gtimer.c           # Arch timer comparator and tick interrupts
│   ├── main.c             # Microkernel C entry point
│   ├── port_fatal.c       # Low-level exception panic recorder
│   ├── startup.S          # Early EL2/EL1 bootloader and secondary core bringup
│   ├── string.c           # Early freestanding memory/string helpers
│   ├── ukernel.c          # Modular kernel boot sequencer and multi-task bootstrap
│   └── uart.c             # PL011 UART driver
├── linker.ld              # Bare-metal AArch64 linker script
├── Makefile               # GNU Make build recipe
├── SConscript             # SCons build orchestration script
└── README.md              # Project documentation
```

## Build and Testing

CherryBSP is orchestrated via the root `SConstruct`:
```bash
# SMP full acceptance test
scons test

# Cross-core scheduling synchronization test
scons SCHED_MODE=cross test
```
