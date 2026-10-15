# Brightness Control - External Monitor Brightness for Linux

**Change your external monitor's brightness on Linux from the system tray.** No more fiddling with the monitor's on-screen menu buttons.

Brightness Control is a lightweight, open-source **GUI for [`ddcutil`](https://www.ddcutil.com/)**. It adjusts the real hardware backlight of desktop monitors over **DDC/CI**, not a software gamma overlay. It runs on **Ubuntu, Debian, Linux Mint, Pop!_OS, Fedora, Arch** and other distributions, with **GNOME, KDE Plasma, XFCE, MATE** and Cinnamon desktops.

## Monitor compatibility

Brightness Control works with **any external monitor that supports DDC/CI**, which includes most modern monitors from **BenQ, Dell, LG, Samsung, ASUS, Acer, AOC, Philips, ViewSonic, HP, Lenovo, MSI** and many others. It works over **HDMI, DisplayPort and USB-C/DisplayPort Alt Mode** connections, including multi-monitor setups.

✅ **Tested on BenQ programming monitors** (BenQ's monitor series designed for coding).

> Laptop built-in screens don't use DDC/CI. Use your desktop's normal brightness controls for those.

## Features

- **Hardware brightness control** for external monitors via DDC/CI
- **Slider control** with 21 steps (0–20), each step = 5% brightness
- **System tray icon** showing the current level as a label
- **Quick Set menu** in the tray for jumping straight to any level
- **Multi-monitor support** - choose which monitor to control with `--display` or from the window
- **Hide to tray** - closing the window hides it instead of quitting
- **Autostart at login** with the included `.desktop` file
- **Lightweight** - a single small native C++/GTK3 binary, no Electron, no background services
- **Free and open source** under the MIT license

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

## FAQ

**How do I change external monitor brightness on Linux?**
Linux desktop environments usually only control laptop screens. External monitors are adjusted over DDC/CI, and Brightness Control gives you a tray icon and slider for that, so you don't need to run `ddcutil` commands by hand.

**Does it work with my monitor?**
If your monitor supports DDC/CI, almost certainly. Run `ddcutil detect`: if your monitor is listed, Brightness Control can control it. If not, check that DDC/CI is enabled in the monitor's on-screen menu.

**Can I control two or more monitors?**
Yes. Pick the monitor with the **Display** selector in the window, or choose the starting monitor with `--display N`.

**Is this a software dimmer?**
No. It changes the monitor's actual backlight, the same as pressing the buttons on the monitor.

## License

[MIT](LICENSE)
