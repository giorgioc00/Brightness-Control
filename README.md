# Brightness Control

A small GTK3 system tray app for adjusting external monitor brightness on Linux via [`ddcutil`](https://www.ddcutil.com/) (DDC/CI).

## Features

- **Slider control** with 21 steps (0–20), each step = 5% brightness
- **System tray icon** showing the current level as a label
- **Quick Set menu** in the tray for jumping straight to any level
- **Display selection** - choose which monitor to control with `--display` or from the window
- **Hide to tray** - closing the window hides it instead of quitting

## Requirements

- Linux with an AppIndicator-compatible tray (GNOME with the AppIndicator extension, KDE, XFCE, MATE, etc.)
- A monitor that supports DDC/CI (usually enabled in the monitor's on-screen menu)
- `ddcutil`
- GTK3 and libayatana-appindicator3 development libraries
- CMake 3.10+ and a C++17 compiler

### Install dependencies (Debian/Ubuntu)

```bash
sudo apt install ddcutil libgtk-3-dev libayatana-appindicator3-dev cmake build-essential
```

### ddcutil permissions

`ddcutil` needs access to `/dev/i2c-*`. To use it without `sudo`, add your user to the `i2c` group and log out and back in:

```bash
sudo usermod -aG i2c $USER
```

Check that your monitor is detected:

```bash
ddcutil detect
```

## Build

```bash
mkdir build && cd build
cmake ..
make
```

If CMake reports that `gtk+-3.0` was not found even though it is installed, point pkg-config at the system directory and try again:

```bash
export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig
```

## Run

```bash
./build/brightness-control              # controls display 1
./build/brightness-control --display 2  # controls display 2
```

### Command-line options

| Option | Description |
|--------|-------------|
| `-d`, `--display=N` | `ddcutil` display number to control (1–16, default `1`). Run `ddcutil detect` to list your displays. |
| `-h`, `--help` | Show all options |

## Usage

### Main window
- **Display**: selects which `ddcutil` display the slider controls. It starts at the `--display` value (default 1).
- **Slider**: drag to set brightness from 0 to 20
- **Hide to Tray**: hides the window to the system tray
- **Close (X)**: also hides the window; use **Quit** in the tray menu to exit

### Tray menu
- **Show Brightness Control**: reopens the window
- **Quick Set**: submenu with levels 0–20
- **Quit**: exits the application

## Brightness levels

| Level | Brightness |
|-------|------------|
| 0     | 0%         |
| 1     | 5%         |
| 10    | 50%        |
| 20    | 100%       |

The app starts at level 10 and does not read the monitor's current brightness, so the displayed level is only accurate after you change it.

## Configuration

The display is chosen at runtime (see [Command-line options](#command-line-options)). Other settings are compile-time constants in the `Config` namespace at the top of `brightness.cpp`:

- `DISPLAY_DEFAULT` - display used when `--display` is not given, default `1`
- `DISPLAY_MAX` - highest display number accepted, default `16`
- `VCP_CODE` - the VCP feature code for brightness, default `10`
- `BRIGHTNESS_DEFAULT` - the starting level, default `10`

Rebuild after changing them.

## Autostart at login

1. Edit `brightness-control.desktop` and set `Exec=` to the absolute path of your built binary. Add `--display N` at the end if you want a display other than 1.
2. Copy it into your autostart directory:

```bash
mkdir -p ~/.config/autostart
cp brightness-control.desktop ~/.config/autostart/
```

To disable autostart, remove the file:

```bash
rm ~/.config/autostart/brightness-control.desktop
```

## License

[MIT](LICENSE)
