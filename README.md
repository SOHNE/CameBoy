<picture>
  <source media="(prefers-color-scheme: dark)" srcset="./.github/assets/logo.svg">
  <img alt="CameBoy Logo" src="./.github/assets/logo.svg" width="100px" align="left">
</picture>

### `CameBoy`

<!--[![GitHub release][release-badge]][release-link]-->
[![License][license-badge]][license-link]
[![CameCore][camecore-badge]][camecore-link]
[![CameBoy][cameboy-badge]][cameboy-link]

✨ Learning by doing: Exploring the fundamentals of emulation with a minimalist Game Boy™ project. 🎮

<div flex="true">
  <a href="https://github.com/SOHNE/CameBoy/releases">
    Download
  </a>
  •
  <a href="#">
    Live Demo
  </a>
  •
  <a href="https://leandroperes.notion.site/Game-Boy-C-Emulator-1a2d6f68a3ab8093a418fff30b2c236b">
    Project Notes 🇧🇷
  </a>
</div>

## 📑 Introduction

`CameBoy`—a raw dive into retro computing. This Game Boy™ emulator, built in `C99`, isn't about bells or whistles. It's a hands-on experiment pieced together from a patchwork of online insights, designed to reveal the bare mechanics behind this classic handheld. Here, every line of code tries to be a lesson in minimalism, without unnecessary clutter.

> [!IMPORTANT]
> This project is built purely for learning and experimenting. We aren’t connected to Nintendo® in any way. Also, this repository doesn’t include any illegal BIOS nor ROMs, nor do we support using them—the end user are responsible for making sure to use legally obtained softwares.

<details open>
<summary>
 📸 Screenshots
</summary> <br />

<table align="center">
  <tr>
    <td align="center">
      <img width="100%" src="https://github.com/user-attachments/assets/e8f77e3b-43a0-455e-9e68-b79f67539d54" alt="Instruction timing"/><br>
      <sub>instr_timing.gb 🟥</sub>
    </td>
    <td align="center">
      <img width="100%" src="https://github.com/user-attachments/assets/4135a0b7-f8f4-4cdd-911c-4e235923aef8" alt="Memory timing"/><br>
      <sub>mem_timing.gb 🟨</sub>
    </td>
  </tr>
  <tr>
    <td align="center"> <img width="100%" src="https://github.com/user-attachments/assets/65c991f8-72a5-418b-aa51-39ad2a971777" alt="CPU isntructions"/><br>
      <sub>cpu_instrs.gb 🟩</sub> </td>
    <td align="center"> <img width="100%" src="https://github.com/user-attachments/assets/2c978054-81e3-4b64-828e-c6480615e974" alt="BGB test"/><br>
      <sub>bgbtest.gb 🟩</sub> </td>
  </tr>
</table>

</details>

## ♻️ Building & Using

### 📦 Prerequisites
- **C99 Compiler** (GCC 9+/Clang 12+)
- **CMake** (3.26+)
- **SDL2** (2.32.0+ for frontend)
- **SDL2_ttf** (2.24.0+ for frontend)
- **ROM file** (Game Boy .gb format)

> [!NOTE]
> External dependencies (SDL2, SDL2_ttf, ccache) can be managed by [CPM.cmake].
> See [`/CameBoy/CMakeLists.txt`][cameboy-cmake-file] file.

### 🔧 Building from Source

```bash
# Clone repository
git clone https://github.com/SOHNE/CameBoy.git
cd CameBoy

# Configure build (default Release mode)
mkdir build && cd build
cmake ../CameBoy -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build . --config Release

# Optional: Enable debug features
cmake .. -DCC_DEBUG=ON -DUSE_SANITIZERS=ON
```

### 🚀 Basic Usage
```bash
CameBoy --help
Usage: CameBoy [options]

A Game Boy™ emulator using CameCore and SDL2.

    -h, --help                show this help message and exit
    -d, --debug               Enable debug logging
    -c, --cartridge=<str>     Path to the cartridge file

Example: CameBoy --debug --cartridge /path/to/legal_rom.gb
```

## 📐 Architecture

> [!WARNING]
> Needs to improve and correct this Architecture Overview.

```
┌───────────────────────────────────────┐        ┌──────────────────────────────┐
│               CameCore                │        │           CameBoy            │
│         (Game Boy Core Library)       │        │    (Standalone Frontend)     │
├───────────────┬───────────────────────┤        ├───────────────┬──────────────┤
│      Core     │         Utils         │        │  Application  │    SDL2      │
│ (Emu Context) │      (Logging/IO)     │◀──────┤     Loop      │ (Rendering/  │
├───────────────┴───┬───────────────────┤        │               │   Input)     │
│      CPU          │        Bus        │        └───────┬───────┴───────┬──────┘
│  ┌───────┐        │    (Memory        │                │               │
│  │Fetch  ├──────▶│     Mapper)       │                └───────┬───────┘
│  │Decode │        ├────────┬──────────┤                        │
│  │Execute│        │  Cart  │   PPU    │                ┌───────▼───────┐
│  └───────┘        │        │          │                │   Platform    │
│ (SM83 Emulation)  │        │          │                │  Integration  │
│                   │        │          │                └───────────────┘
└───────────────────┴────────┴──────────┘
                                                         ┌───────────────────────┐
                                                         │  Future Improvements  │
                                                         │  (Planned Components) │
                                                         ├────────────┬──────────┤
                                                         │    APU     │  Timer   │
                                                         │  (Audio    │ (System  │
                                                         │   System)  │  Timing) │
                                                         └────────────┴──────────┘
Key Components:
CameCore Library:
- Core: Emulator state management (Init/Step/Run)
- CPU: Full SM83 implementation with instruction pipeline
- Bus: Memory bus controller with proper banking
- Cart: ROM loading/validation with MBC support
- Utils: Logging system and file I/O operations

CameBoy Standalone:
- Application Loop: Ties core emulation to platform
- SDL Integration: Handles video output and input
- Platform Layer: Build system and dependency management

Data Flow:
Cartridge → Cart → Bus ↔ CPU ↔ Core
                     │
SDL Events ← Bus ↔ Core → SDL Rendering

Memory Map Implementation:
0000-FFFF Address Space
├─ ROM Banks      (Cart)
├─ VRAM           (Bus)
├─ WRAM           (Bus)
├─ OAM            (Bus)
└─ I/O Registers  (Bus)

Instruction Pipeline:
PC → Fetch → Decode → Execute → Cycle Counting
        ▲        │         │
        └────────┴─────────┘ (Flags/Reg Updates)
```

## 🌱 Contributing

The [CONTRIBUTING] guidelines should help in getting you started on how to contribute to this project.

## 🙏 Acknowledgments & Resources

A sincere thank you to the individuals, communities, and resources that brought this project to life:

- **[Game Boy Pan Docs][pandocs-link]:** The definitive technical reference for Game Boy architecture and programming, which served as the backbone of this emulator's development.
- **[Console Architecture Guide by Rodrigo Copetti][copetti-link]:** An invaluable overview of the Game Boy's hardware internals and system design.
- **[Gekkio's Research][gekkio-link]:** For sharing meticulous hardware tests, reverse-engineering insights, and technical documentation.
- **[The Ultimate Game Boy Talk][ultimate-talk-link]:** An educational deep-dive into emulator development fundamentals by [Michael Steil][steil-link].
- **[Game Boy Emulator Development YouTube Series][youtube-series-link]:**  A hands-on, step-by-step guide to building a functional emulator by  [Rocky D. Pulley][pulley-link].
- **[Game Boy Test ROMs by c-sp][gb-tests-link]**: A comprehensive suite of test ROMs essential for validating emulator accuracy and performance.

This project stands on the shoulders of the passionate **[gbdev.io][gbdev-link]** community – thank you for preserving and sharing Game Boy knowledge for future generations.

## ⚖️ License

CameCore and CameBoy are licensed under zlib/libpng license, an OSI-certified and BSD-like license. Check [LICENSE] for details.

![readme version](https://img.shields.io/badge/%2F~.-lightgrey.svg?style=flat-square&colorA=808080&colorB=808080)![readme version](https://img.shields.io/badge/12%2F03%2F2025--lightgrey.svg?style=flat-square&colorA=000000&colorB=808080)

[//]: (Externals)

[release-badge]: https://img.shields.io/github/v/release/SOHNE/CameBoy
[release-link]: https://github.com/SOHNE/CameBoy/releases/latest
[license-badge]: https://img.shields.io/github/license/SOHNE/CameBoy
[license-link]: https://github.com/SOHNE/CameBoy/blob/main/LICENSE
[camecore-badge]: https://github.com/SOHNE/CameBoy/actions/workflows/camecore.yml/badge.svg
[camecore-link]: https://github.com/SOHNE/CameBoy/actions/workflows/camecore.yml
[cameboy-badge]: https://github.com/SOHNE/CameBoy/actions/workflows/cameboy.yml/badge.svg
[cameboy-link]: https://github.com/SOHNE/CameBoy/actions/workflows/cameboy.yml

[CPM.cmake]: https://github.com/cpm-cmake
[cameboy-cmake-file]: CameBoy/CMakeLists.txt#L21

[pandocs-link]: https://gbdev.io/pandocs
[copetti-link]: https://www.copetti.org/writings/consoles/game-boy
[gekkio-link]: https://github.com/Gekkio/gb-ctr
[ultimate-talk-link]: https://media.ccc.de/v/33c3-8029-the_ultimate_game_boy_talk
[steil-link]: https://media.ccc.de/search?p=Michael+Steil
[youtube-series-link]: https://www.youtube.com/watch?v=e87qKixKFME&list=PLVxiWMqQvhg_yk4qy2cSC3457wZJga_e5
[pulley-link]: https://github.com/rockytriton
[gb-tests-link]: https://github.com/c-sp/game-boy-test-roms
[gbdev-link]: https://gbdev.io

[LICENSE]: LICENSE
[CONTRIBUTING]: CONTRIBUTING.md

[//]: (EOF)

