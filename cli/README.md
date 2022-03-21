# qFlipper-cli
### A non-interactive text mode interface for qFlipper
This program is mostly meant for testing purposes, although it can also provide all of the qFlipper's features from the comfort of the terminal emulator.

## Running:
### Windows:
`<Program_files_directory>\qFlipper\qFlipper-cli.exe [args] [parameters]`
### MacOS:
`<Applications_directory>/qFlipper.app/Contents/MacOS/qFlipper-cli [args] [parameters]`
### Linux:
`<AppImage_directory>/qFlipper-x86_64-x.y.z.AppImage cli [args] [parameters]`

## Command syntax:
Run without any arguments to perform a quick update/repair.
### Commands:
* `backup <target_dir>` - Backup Internal Memory contents.
* `restore <source_dir>` - Restore Internal Memory contents.
* `erase` - Erase Internal Memory contents (Factory reset).
* `wipe` - Wipe entire MCU Flash Memory (Not implemented yet).
* `firmware <firmware_file.dfu>` - Flash Core1 Firmware.
* `core2radio <firmware_file.bin>` - Flash Core2 Radio stack.
* `core2fus <firmware_file.bin> <0xaddress>` - Flash Core2 Firmware Update Service **(WARNING! It WILL invalidate your secure enclave!)**

### Options:
* `-d <n>, --debug-level <n>` - Set debug output level, 0 - errors only, 1 - terse, 2 - everything. Default is 1.
* `-n <n>, --repeat-number <n>` - Repeat an operation *n* times, 0 - indefinitely, default - once.
* `-c <channel>, --update-channel <channel>` - Set the update channel (may be one of: `release`, `release-candidate`, `development`). The choice is saved in the configuration file, default is `release`.
* `-v, --version` - Show program version.
* `-h, --help` - Show help.
