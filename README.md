# MHXX Forge

An environment for linking, runtime hooking and code patching in Monster Hunter Double Cross.

## Installing (Ryujinx)

1. Download the latest release from the [releases page](https://github.com/ZellD-0371/forge/releases/latest)
2. Open Ryujinx, right click MHXX and select "Open Mods Directory"
3. Extract the contents of the downloaded archive into that folder
4. In Ryujinx, right click MHXX and select "Create Custom Configuration" (or "Edit Custom Configuration" if you already did so before)
5. Go to the CPU section and **disable** the "PPTC (Profiled Persistent Translation Cache)" option
6. Click "Save".
7. Right click MHGU and select "Cache Management > Purge PPTC Cache" and "Cache Management > Purge Shader Cache"
8. Restart Ryujinx

### Keyboard Input

To make keyboard input work, you need to enable "Direct Keyboard Access" in the Ryujinx Input Settings.
Then press F9 or Shift+F9 
If you are using a controller, please press L3 + R3.

## Building

Be sure to check out submodules first:

```bash
git submodule update --init --recursive
```

To build the module you'll need [devkitPro](https://devkitpro.org/wiki/Getting_Started).

Install devkitARM using pacman:

```sh
sudo (dkp-)pacman -Sy
sudo (dkp-)pacman -S devkitARM
```

Use `dkp-pacman` on macOS and Debian-based distros and `pacman` otherwise.

Also install the `lz4` package:

```sh
# On Debian-based distros
sudo apt install liblz4-dev

# On Arch-based distros
sudo pacman -S lz4

# On macOS with Homebrew
brew install lz4
```

Then you can build the module:

```sh
make
```

You'll find `subsdk.nso` and `main.npdm` in the project directory. Move `subsdk.nso` to `subsdk1` (for example) and `main.npdm` into an `exefs` folder in a mod directory.

## Credits

* [devkitPro](https://devkitpro.org) for a development environment
* [skyline-dev/skyline](https://github.com/skyline-dev/skyline) for the basis of the module
* [RicBent/codedx](https://github.com/RicBent/codedx) for building an NSO
* [xerpi/libnx](https://github.com/xerpi/libnx) for an AArch32 userland library
* [Magic1-Mods/And32InlineHook](https://github.com/Magic1-Mods/And32InlineHook) for AArch32 hooks

## MHXX JPN 1.5.1 build

The `config-mhxx.json` profile targets title ID `0x0100C3800049C000`. Build the module with `make CONFIG_JSON=config-mhxx.json`. The Japanese path starts plugins and dispatches their per-frame callbacks from the graphics-present hook, while skipping MHGU-specific singleton and game-update hooks. Save data is mounted through the SDK at `forge_savedata`. MHXX-specific game-object bindings and the game socket-pool adapter are not implemented by this profile, so plugins that depend on those MHGU-specific integrations need their own MHXX implementation.
