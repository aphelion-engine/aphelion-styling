# Aphelion Styling

`aphelion-styling` is a standalone collection of reusable Qt6 styling, theme
token, runtime QSS, and monochrome icon primitives. It contains no editor
behavior, node graph classes, or application state.

## Installation

```bash
python -m pip install .
```

For local development from the editor repository, install the sibling package
with `python -m pip install -e ../aphelion-styling`.

## Imports

```python
from aphelion_styling import AppIcon, ThemeTokens, make_icon
from aphelion_styling import DARK_THEME, build_theme_styles
from aphelion_styling import SELECTION_BAR_STYLE
```

The named QSS catalog, including legacy constants such as `DARK_THEME`, is
available from `aphelion_styling.theme` and re-exported at the package level.

## Theme tokens and runtime styles

```python
from aphelion_styling import aphelion_dark, build_theme_styles

tokens = aphelion_dark()
styles = build_theme_styles(tokens)
widget.setStyleSheet(styles.main)
```

`ThemeTokens` can be serialized with `to_dict()` and restored with
`ThemeTokens.from_dict()`. Built-in presets are available through
`BUILTIN_THEMES` and `builtin_theme()`.

## C/C++ library

The same theme tokens and QSS generation are also available as a
dependency-free C99 library for C and C++ applications — no Qt or Python
required. See [`native/README.md`](native/README.md) for the API and build
instructions.

```c
#include "aphelion_styling.h"

styling_tokens tokens;
styling_aphelion_dark(&tokens);

char qss[65536];
int n = styling_build_main(&tokens, qss, sizeof(qss));
```
