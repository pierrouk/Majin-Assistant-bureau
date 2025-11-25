## Purpose

This file provides concise, actionable guidance for AI coding agents working on the "Majin - Assistant Bureau" PlatformIO projects in this workspace. Focus on the concrete patterns, integration points and developer commands an agent needs to be immediately productive.

## Project big-picture (why & architecture)

- The project targets ESP32-S3 (PlatformIO env: `esp32-s3-devkitc-1` in `platformio.ini`) using the Arduino framework and LovyanGFX for display rendering.
- High-level modules live under `src/`:
  - `src/managers/` — orchestration modules (CoreManager, UIManager, NetworkManager, SettingsManager, UsbManager, etc.). These are passed to each other via `begin(...)` (dependency injection) rather than global singletons.
  - `src/hal/` — hardware abstraction (DisplayDriver, sensors, servo, sound). Use these when changing low-level hardware behavior.
  - `src/tasks/` — FreeRTOS-style tasks wrappers (GUI, System scheduling).
  - `src/web/` & `src/managers/WebPages.h` — HTTP/web UI (uses ESPAsyncWebServer).

Data flow notes:
- Sensors update `CoreManager` via `setSensorData(...)`. `UIManager` queries `CoreManager` for mood/stats to render the face and widgets.
- `UIManager` delegates face drawing to `FaceRenderer` (see `src/managers/FaceRenderer.*`). Keep face/vector rendering changes there.
- `InputManager` converts raw touch into gestures; UI code consumes gesture enums (no direct raw-touch logic in managers).

## Files & examples to reference
- `platformio.ini` — build flags of note: `-D BOARD_HAS_PSRAM`, `CORE_DEBUG_LEVEL=3`, PSRAM and USB flags. Partition CSV: `default_16MB.csv` (16MB flash target).
- `src/managers/CoreManager.h` — game/tamagotchi state and sensor aggregation.
- `src/managers/UIManager.h` — scene/enums, effects, particles, and the rendering pipeline (delegates to `FaceRenderer`). Examples: `UIScene`, `UIEffect`, `Particle`, `AppIcon` structs.
- `src/hal/DisplayDriver.*` — central display wrapper. Use it when altering drawing or display init.
- `src/managers/NetworkManager.*` and `src/web/*` — web integration, async server; look at `lib_deps` for required async libs.

## Build / upload / monitor (concrete commands)
Use PlatformIO (recommended) or CLI. When specifying environment, the project uses `esp32-s3-devkitc-1`.

PowerShell examples (copy/paste):
```
pio run -e esp32-s3-devkitc-1                      # build
pio run -e esp32-s3-devkitc-1 -t upload           # upload/flash
pio device monitor -e esp32-s3-devkitc-1 --baud 115200  # serial monitor
```

Inside VSCode the PlatformIO extension tasks (Build / Upload / Monitor) are configured — use those for convenience.

## Project-specific conventions & patterns
- Dependency injection via `.begin(...)`: managers are not singletons — pass references (e.g., `UIManager::begin(DisplayDriver*, CoreManager*, NetworkManager*, SettingsManager*, UsbManager*)`).
- Face rendering is isolated in `FaceRenderer` — avoid duplicating facial-drawing logic elsewhere.
- Enums and small POD structs are preferred for cross-module messages (see `UIManager.h` for `UIScene`, `UIEffect`, `Particle`). Follow that style for new cross-component events.
- Comments are often in French — keep new comments consistent with the surrounding file language.
- Uses Arduino `String` across codebase; prefer matching existing usage rather than switching to `std::string`.

## Integration & external dependencies
- Notable `lib_deps` from `platformio.ini`: LovyanGFX, ESPAsyncWebServer, WiFiManager, ArduinoJson, Adafruit sensor libs, ESP32Servo.
- USB CDC flags enabled (`ARDUINO_USB_MODE=1`, `ARDUINO_USB_CDC_ON_BOOT=1`). Avoid changing USB compile flags unless necessary.
- PSRAM enabled (`BOARD_HAS_PSRAM`) — drawing code assumes PSRAM for large framebuffers/sprites (LovyanGFX sprites). If disabling PSRAM, watch for memory issues.

## Quick editing heuristics for agents
- Small UI changes: edit `FaceRenderer` or `UIManager` drawing functions. Keep sprite lifetime in mind (LGFX_Sprite) to avoid leaks.
- Adding a new app icon/stream deck entry: update `_initApps()` in `UIManager` and the `AppIcon` vector (see `UIManager.h`).
- Network changes: prefer `NetworkManager` and `src/web/*` pages. The async server patterns are used throughout.
- Touch/gesture changes: change `InputManager` behavior — it centralizes gesture thresholds.

## What not to change lightly
- Partition scheme and `platformio.ini` build flags (PSRAM, flash size, USB) — changing them affects runtime and flashing.
- Replacing Arduino `String` globally — codebase depends on it.

## Where to look next if context is needed
- `src/main.cpp` — program entry point, initial task creation.
- `src/tasks/*` — how GUI and system loop are scheduled.
- `src/hal/*` — hardware bindings (displays, sensors, servos). Use them when adding new hardware.

If anything in this instruction file is unclear or you'd like the same doc created for `RobotBureauV2/`, tell me which project(s) to target and any extra emphasis (e.g., focus on network, face rendering, or memory debugging).
