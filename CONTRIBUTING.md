# CONTRIBUTING TO CAMECORE+CAMEBOY
*A Game Boy™ Emulation Project*

## 🎯 CODE STRUCTURE OVERVIEW
**Core** | **Utils**
- Emulation state machine
- Cycle-precise timing
- Logging subsystem (`LOG_LEVEL` controls)
- File I/O safety wrappers
- Bitwise operation macros

**SM83 CPU**
- Pipeline stages (fetch/decode/execute)
- Register bank management
- Interrupt controller
- Extended CB opcode handler

**Memory Bus** | **Cartridge**
- Memory-mapped I/O system
- ROM/RAM banking logic
- Cartridge header validation
- Battery-backed save states

**Frontend**
- SDL2 video/input backend
- Debug HUD (registers/memory)
- Pixel-perfect scaling (*)
- Audio buffer streaming (*)

## 🛠️ GETTING STARTED

### Prerequisites
- **C99-compliant compiler** (GCC/Clang/MSVC)
- **CMake 3.15+**
- **SDL2 development libraries**
- **Python 3.8+** (for test scripts)

### Build Process
```bash
git clone https://github.com/SOHNE/CameBoy.git
cd CameBoy
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug  # or Release
cmake --build . --parallel 4
```

### Running Tests
```bash
TODO
```

## 🔄 CONTRIBUTION WORKFLOW

1. **Fork** the repository
2. Create a **feature branch**:
   `git checkout -b feat/cpu-div-instruction`
3. Follow **coding standards** (below)
4. Write **tests** for new functionality
5. Open PR with:
   - JIRA-style title: `[CPU] Implement DIV instruction`
   - Context/approach in description
   - Linked GitHub Issues (`Closes #42`)

## 📜 CODING STANDARDS

### C99 Conventions
```c
// File naming: module_purpose.c (cpu_decode.c)
// Function naming: Module_Action (CPUStep())
// Constants: UPPER_SNAKE_CASE (FLAG_Z_BIT)

// Header inclusion order:
1. Project headers
2. System headers
3. Third-party libs
```

### Error Handling
```c
// Use custom error macros
ASSERT(ptr != NULL, "Null context pointer");
```

### Memory Management
- **Ownership** clearly documented
- Use `LoadFileData()`/`UnloadFileData()` wrappers
- Zero `free()` in core logic - use utils' allocators

## 🧪 TESTING GUIDELINES

### Unit Tests
```c
TODO
```

### ROM Validation
| Test ROM         | Expected Result |
|------------------|-----------------|
| `cpu_instrs.gb`  | All ops pass    |
| `dmg-acid2.gb`   | Perfect render  |

---

## 📚 DOCUMENTATION

1. Update header comments using **doxygen-style**

## ⚖️ LICENSE

All contributions fall under **[zlib/libpng license](LICENSE)**.
By submitting a PR, you agree to license your work under these terms.

---

## 🙏 ACKNOWLEDGMENTS

Special recognition for:
- Significant code contributions 🔧
- Thorough bug reports 🐛
- Documentation improvements 📖
