# phoenix_cli
`PhoenixCLI` is a LLM (Large Language Model) runner.

## Prerequisites
#### Installing `CMake`

To use PhoenixCLI, you need to have CMake installed. Here’s how to install it quickly:

#### On Linux

For Debian-based distributions (e.g., Ubuntu):
```sh
sudo apt-get update
sudo apt-get install cmake
```

#### Install **curl** on your system for download models.

Before running `PhoenixCLI`, you must ensure that libcurl is installed on your system. libcurl is a mandatory dependency for PhoenixCLI.

Checking for `libcurl`
To verify if libcurl is installed, use the following commands based on your operating system:

#### On Linux:
Open a terminal and run:

``` bash
curl --version
```

You should see version information for libcurl. If not, libcurl is likely not installed.

#### On macOS:
Open a terminal and run:
``` bash
brew info curl
```

If `curl` and `libcurl` details are displayed, it is installed. If not, you need to install it.

#### On Windows:
Open a Command Prompt or PowerShell and run:

``` sh
curl --version
```

If you see version details, `libcurl` is installed. If not, follow the installation instructions below.

### Installing libcurl
If `libcurl` is not installed on your system, follow the instructions for your operating system to install it.

#### On Linux:
For Debian-based distributions (e.g., Ubuntu):

``` bash
sudo apt-get update
sudo apt-get install libcurl4 libcurl4-openssl-dev
```
For Red Hat-based distributions (e.g., Fedora):
``` bash
sudo dnf install libcurl libcurl-devel
```


#### On macOS:
If you have Homebrew installed, you can use:

``` bash
brew install curl
```
#### On Windows:
You can download precompiled binaries from the official curl website or use a package manager like Chocolatey:

``` bash
choco install curl
```

## Building PhoenixCLI
1. Clone the repository
```bash
git clone --recursive https://github.com/osllmai/phoenix_cli.git
cd phoenix_cli
```
2. Create `build` directory
```bash
mkdir build
cd build
```
3. Build phoenix
```bash
cmake ..
cmake --build . --target phoenix -- -j 6
```
4. Check correcness
```sh
./bin/phoenix --version

Current version is 0.0.3
```
5. Phoenix help:
```sh
./bin/phoenix help

PhoenixCLI is a large language model runner

Usage:
phoenix [flags]
phoenix [command]

Available Commands:
  list	List models
  run	Run a model
  exec	Run a model on your local machine
  pull	Pull a model from registry
  help	Help about any command

Flags:
  -h, --help	Help for phoenix
  -v, --version	Show version information

Use "phoenix [command] --help" for more information about a command.
```

PhoenixCLI tested on:
- MacOS ✅
- Windows 🟧
- Gnu/Linux 🟥