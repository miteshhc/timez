## Description:

**timez** is a simple utility that measures the execution time and resource usage of
commands. It is designed to be a very lightweight and minimal solution for
measuring the performance and resource usage of commands.


## Features:

- Measure execution time and resource usage of commands.
- Option to specify a fixed duration for command execution.
- Ability to save results to a file.
- Verbosity mode for detailed output.
- Easy-to-use command-line interface.
- Easy-to-read output
- Lightweight

## Options:

| Option | Description |
| --- | --- |
| `-h` | Display help message. |
| `-v` | Display version information. |
| `-d` | Set the duration of the command execution in seconds. |
| `-o` | Modify the default stream or specify a file to save the results. |

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

## Installation:

### For compiling from source:

- Clone the repository:

```
git clone https://github.com/miteshhc/timez.git
```

- Compile the code:

```
cd timez && g++ args.cpp main.cpp timez.cpp utils.cpp -o timez
```

- Run the executable:

```
./timez
```

#### ⚠️ Warning: the only option is to compile from source, as the version in release is not up-to-date.


## License:

This project is licensed under the MIT License. See the [LICENSE](./LICENSE)
file for details.


## Contributions:
Contributions are welcome! If you have any suggestions, bug reports, or want
to contribute enhancements, feel free to open an issue or submit a pull request.
Let's make timez even better together!
