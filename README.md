## Description:

**timez** is a simple utility that measures the execution time and resource usage of
commands. It is designed to be a very lightweight and minimal solution for
measuring the performance and resource usage of commands.


## Features:

- Measure execution time and resource usage of commands.
- Set the duration of the command execution.
- Ability to save results to a file.
- Verbose mode for detailed output.
- Easy-to-read output
- Lightweight

## Options:

| Option | Description |
| --- | --- |
| `-h, --help` | Display help message. |
| `-v, --verbose` | Display more verbose output. |
| `-d, --duration` | Set the duration of the command execution in seconds. |
| `-o, --out` | Modify the default stream and save the results to a file. |

### Examples

```bash
$ ./timez sleep 5
```

```bash
$ ./timez sleep 5 -d 2.5
```

```bash
$ ./timez sleep 5 -v
```

### Get Started

#### Pre-compiled binaries:

Download the latest release from the
[releases](https://github.com/miteshhc/timez/releases) page.

You can either choose the statically linked binary or the dynamically linked
binary.

#### Compiling from source:

```
git clone https://github.com/miteshhc/timez.git
cd timez
make
```

#### Arch User Repository

Using AUR helper (for example - yay):

```bash
yay -S timez
```

Available [here](https://aur.archlinux.org/packages/timez).


## License:

This project is licensed under the MIT License. See the [LICENSE](./LICENSE)
file for details.


## Contributions:
Contributions are welcome! If you have any suggestions, bug reports, or want
to contribute enhancements, feel free to open an issue or submit a pull request.
Let's make timez even better together!
