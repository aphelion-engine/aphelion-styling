# Aphelion styling — C/C++ library

A dependency-free C99 implementation of the `aphelion_styling` package's
theme and QSS surface. Use it to generate the same Qt6 stylesheets from C or
C++ applications, without linking Qt or the Python package.

The Python package in `src/aphelion_styling/` remains the source of truth;
this library reproduces the same output.

## What is included

| Python source                         | C equivalent                     |
| ------------------------------------- | -------------------------------- |
| `theme_tokens.py`                     | `styling_tokens` + 4 presets     |
| `theme_engine.py`                     | `styling_build_{main,dock,menubar,context_menu,timeline,preferences}` |
| `property_style.py`                   | `styling_build_properties`       |
| `theme.py` + `component_styles.py`    | the static named QSS catalog     |

`icons.py` is intentionally **not** ported: it draws glyphs with QPainter and
belongs in the Qt layer, not in a dependency-free C library.

## Layout

```
native/
  aphelion_styling.h     # public C/C++ API
  aphelion_styling.c     # implementation (single translation unit)
  CMakeLists.txt         # shared + static library build
```

## Build

```powershell
cmake -S native -B native/build
cmake --build native/build --config Release
cmake --install native/build --prefix <dest>
```

This produces `aphelion_styling` (`.dll`/`.so`/`.dylib`) plus a static
`aphelion_styling` archive when `STYLING_BUILD_STATIC=ON` (the default).
Needs any C99 compiler: Visual Studio Build Tools on Windows,
`build-essential` on Linux, Xcode Command Line Tools on macOS.

Alternatively, skip CMake and add `aphelion_styling.c` to your own project —
it has no dependencies beyond the C standard library.

## Usage

```c
#include "aphelion_styling.h"

int main(void)
{
    styling_tokens tokens;
    styling_aphelion_dark(&tokens);

    char qss[65536];
    int n = styling_build_main(&tokens, qss, sizeof(qss));

    printf("%.*s\n", n, qss);          /* setStyleSheet(QString::fromUtf8(qss, n)) */
    return 0;
}
```

Works identically from C++ — the header wraps everything in `extern "C"`.

## API notes

- Builders return the number of bytes written (excluding the terminating
  NUL), or the required length when the buffer is too small, so callers can
  size the buffer on a second pass.
- Static QSS constants (`styling_dark_theme()`, `styling_timeline_style()`,
  …) return borrowed, NUL-terminated strings that must not be freed.
- `styling_default_properties_style()` resolves the Python package's
  `PROPERTIES_STYLE` constant (the properties panel built from the default
  dark preset).
