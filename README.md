# ESP32-S3 Embedded

Homework for the Beetroot Embedded course on ESP32-S3-N16R8 (PlatformIO).

## Homework

| Folder | Topic |
|---|---|
| `01_three_lights` | Three LEDs |
| `02_button_modes` | Button modes |
| `03_button_bounce` | Button bounce |
| `04_ldr_adc` | LDR with ADC |
| `05_twilight_switch` | Twilight switch (mini-project) |
| `06_superloop_led` | Superloop LED |
| `07_superloop_tasks` | Superloop tasks |
| `08_rtos_tasks` | FreeRTOS tasks |

## Quick start

1. Clone the repo to a path with ASCII characters only: ESP-IDF builds fail on non-ASCII paths.
2. Open the repo in VS Code and run **Dev Containers: Reopen in Container**.
3. Build a project: `pio run -d <folder>`.
4. For IntelliSense, generate the compile DB once (and after adding a project): `python3 scripts/ide_cdb.py`.
5. Upload from the host, where USB is available: `pio run -d <folder> -t nobuild -t upload`.
   The repo is mounted at the same path in the container, so builds made there stay valid on the host.

## Tooling

- **Formatting:** `.clang-format` (Google-based), `.editorconfig`.
- **Static analysis:**
  - `pio check` (cppcheck);
  - clang-tidy via `pio run -t compiledb` → `scripts/tidy_cdb.py` → `run-clang-tidy-18`.
- **Pre-commit (host):** `pipx install pre-commit==4.7.0 && pre-commit install`.
- **CI:**
  - *Build*: build, cppcheck, clang-tidy for changed projects.
  - *Security*: gitleaks, hadolint, zizmor.
- **Dependencies:**
  - Python packages are hash-pinned in `.devcontainer/requirements.txt`; regenerate it from `requirements.in` with `pip-compile --generate-hashes --allow-unsafe`.
  - Dependabot updates actions, the base image and Python packages.
- **IntelliSense:** `scripts/ide_cdb.py` merges the clang-ready compile DBs of all projects.

## Conventions

- **Branches:** kebab-case by content, no numbers.
- **Commits:**
  - Conventional Commits, imperative, ≤72 chars, no numbers, signed.
  - Dependabot commits are the exception to the no-numbers rule.
- **PRs:** follow the template; merge without squash.
