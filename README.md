<p align="center">

<div style="position: relative; width: 100%; text-align: center;">
    <h1>phoenix_cli</h1>
    <a href="https://github.com/osllmai/phoenix_cli">
        <img src="https://readme-typing-svg.demolab.com?font=Georgia&size=16&duration=3000&pause=500&multiline=true&width=700&height=100&lines=Phoenix_cli;LLM+Runner+for+Local+Execution+%7C+Open+Source;Copyright+©️+OSLLAM.ai" alt="Typing SVG" style="margin-top: 20px;"/>
    </a>
</div>

<p align="center">
  <img src="./docs/image/Phoenix2.png" alt="Phoenix2">
</p>
</br>

`Phoenix_cli` is a LLM (Large Language Model) runner developed by [osllm.ai](https://osllm.ai) that allows you to run models on your local machine. It is a command-line interface that allows you to run models, pull models from the registry, and list models. `Phoenix_cli` is built on top of the Phoenix library, which is a C++ library for running large language models. Phoenix_cli is a standalone tool that can be used to run models on your local machine without the need for a server or cloud-based service.

## Prerequisites

#### Installing `CMake`

To use Phoenix_cli, you need to have CMake installed. Here’s how to install it quickly:

#### On Linux

For Debian-based distributions (e.g., Ubuntu):

```sh
sudo apt-get update
sudo apt-get install cmake
```

#### Install **curl** on your system for download models.

Before running `Phoenix_cli`, you must ensure that libcurl is installed on your system. libcurl is a mandatory dependency for Phoenix_cli.

Checking for `libcurl`
To verify if libcurl is installed, use the following commands based on your operating system:

#### On Linux:

Open a terminal and run:

```bash
curl --version
```

You should see version information for libcurl. If not, libcurl is likely not installed.

#### On macOS:

Open a terminal and run:

```bash
brew info curl
```

If `curl` and `libcurl` details are displayed, it is installed. If not, you need to install it.

#### On Windows:

Open a Command Prompt or PowerShell and run:

```sh
curl --version
```

If you see version details, `libcurl` is installed. If not, follow the installation instructions below.

### Installing libcurl

If `libcurl` is not installed on your system, follow the instructions for your operating system to install it.

#### On Linux:

For Debian-based distributions (e.g., Ubuntu):

```bash
sudo apt-get update
sudo apt-get install libcurl4 libcurl4-openssl-dev
```

For Red Hat-based distributions (e.g., Fedora):

```bash
sudo dnf install libcurl libcurl-devel
```

#### On macOS:

If you have Homebrew installed, you can use:

```bash
brew install curl
```

#### On Windows:

You can download precompiled binaries from the official curl website or use a package manager like Chocolatey:

```bash
choco install curl
```

## Building Phoenix_cli

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
Phoenix_cli is a large language model runner developed by osllm.ai. It is open-source software that you can use freely to run and manage large language models.

Usage:
  phoenix [flags]
  phoenix [command]

Available Commands:
  list       Displays a list of all available models that you can run or manage
  run        Executes a model based on your specified input
  exec       Runs a model directly on your local machine
  pull       Retrieves a model from the registry and stores it locally
  history    Shows a list of your previous chats
  help       Provides detailed information and guidance about any specific command

Flags:
  -h, --help      Displays help information for the phoenix command
  -v, --version   Outputs the current version of Phoenix_cli

Use "phoenix [command] --help" for more information about a command.
```

Phoenix_cli tested on:

- MacOS ✅
- Windows 🟧
- Gnu/Linux 🟥
