<p align="center">

<div style="position: relative; width: 100%; text-align: center;">
    <h1>Phoenix_cli</h1>
    <a href="https://github.com/osllmai/phoenix_cli">
        <img src="https://readme-typing-svg.demolab.com?font=Georgia&size=16&duration=3000&pause=500&multiline=true&width=700&height=100&lines=Phoenix_cli;LLM+Runner+for+Local+Execution+%7C+Open+Source;Copyright+©️+OSLLAM.ai" alt="Typing SVG" style="margin-top: 20px;"/>
    </a>
</div>

<p align="center">
  <img src="./docs/image/Phoenix2.png" alt="Phoenix2">
</p>
</br>

## Overview

**Phoenix_cli** is a command-line interface (CLI) tool developed by **osllm.ai** for running Large Language Models (
LLMs) on your local machine. Built on top of the Phoenix library, a C++ library designed for executing large language
models, Phoenix_cli allows you to:

- Run models locally without relying on a server or cloud-based service.
- Pull models from a centralized registry.
- List available models.

Phoenix_cli provides a powerful and flexible tool for developers and researchers working with LLMs, enabling seamless
integration and deployment in local environments.

## Table of Contents

1. [Requirements](#requirements)
2. [Installation](#installation)
    - [macOS](#macos)
    - [Windows](#windows)
3. [Building the Project](#building-the-project)
    - [macOS](#building-on-macos)
    - [Windows](#building-on-windows)
4. [Running Phoenix_cli](#running-phoenix_cli)
5. [UI setup](#ui-setup)

## Requirements

Before you begin, ensure that the following dependencies are installed:

- **CMake** 
- **Boost** 
- **SQLite3** 
- **cURL** 
- **CUDA** (version 11.0 or higher, if using GPU acceleration)
- **Kumpute** (if needed for additional GPU computations)

## Installation

### macOS

1. **Install Homebrew** (if not already installed):
    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

2. **Install dependencies**:
    ```bash
    brew install cmake boost sqlite curl
    ```

### Windows

1. **Install CUDA**:
    - Download the CUDA Toolkit from the [NVIDIA CUDA Toolkit website](https://developer.nvidia.com/cuda-toolkit).
    - Follow the installation instructions specific to your system.

2. **Install Kumpute**:
    - Kumpute can be installed via vcpkg. If you haven’t already installed `vcpkg`, follow the instructions below to
      install it.
    - Install Kumpute using `vcpkg`:
      ```powershell
      git clone https://github.com/microsoft/vcpkg.git
      cd vcpkg
      .\bootstrap-vcpkg.bat
      vcpkg install kumpute
      ```

3. **Install vcpkg** (if not already installed):
    ```powershell
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```

4. **Install dependencies** using `vcpkg`:
    ```powershell
    vcpkg install boost-system boost-filesystem sqlite3 curl kumpute
    ```

5. **Add `vcpkg` to your environment variables**:
    ```powershell
    $env:VCPKG_ROOT = "C:\vcpkg"
    ```

## Building the Project

### Building on macOS

1. **Clone the repository** with submodules:
    ```bash
    git clone --recursive https://github.com/osllmai/phoenix_cli.git
    cd phoenix_cli
    ```

2. **Create a build directory**:
    ```bash
    mkdir build && cd build
    ```

3. **Run CMake** to configure the project:
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

4. **Build the project**:
    ```bash
    cmake --build .
    ```

### Building on Windows

1. **Clone the repository** with submodules:
    ```powershell
    git clone --recursive https://github.com/osllmai/phoenix_cli.git
    cd phoenix_cli
    ```

2. **Create a build directory**:
    ```powershell
    mkdir build
    cd build
    ```

3. **Prepare CMake with CUDA support** (optional) and `vcpkg`:
    ```powershell
    cmake -DUSE_CUDA=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_CUDA_ARCHITECTURES=75 ..
    ```

4. **Build the project**:
    ```powershell
    cmake --build . -- /m:8
    ```

## Running Phoenix_cli

After building the project, you can run the executable from the build directory:

```bash
./build/bin/phoenix_cli --help

PhoenixCLI is a large language model runner developed by osllm.ai. It is open-source software that you can use freely
to run and manage large language models.
Usage:
phoenix_cli [flags]
phoenix_cli [command]

Available Commands:
  server        Start the phoenix_cli server
  list          Displays a list of all available models that you can run or manage
  show          Show full information about a model
  run           Executes a model based on your specified input
  embedding     Convert text to embedding
  exec          Runs a model directly on your local machine
  pull          Retrieves a model from the registry and stores it locally
  rm            Remove a model from your local storage
  help          Provides detailed information and guidance about any specific command
  openai        Interact with OpenAI models

Flags:
  -h, --help    Displays help information for the phoenix_cli command
  -v, --version Outputs the current version of PhoenixCLI

Use "phoenix_cli [command] --help" for more information about a command.
```

## UI Setup

To set up the UI for your local Chatbot, follow these steps:

1. Install Dependencies Open a terminal in the root directory of your local Chatbot UI repository and run:

```shell
npm install
```

2. Install Supabase & Run Locally

   **Why Supabase?** Supabase is used for its ease of use, open-source nature, Postgres database, and free tier for
   hosted instances. It provides a better solution compared to local browser storage, addressing issues of security,
   limited storage, and multi-modal use cases. Other providers may be supported in the future.

#### Steps to Install Supabase:

1. **Install Docker** Download Docker from Docker's website and install it.

2. **Install Supabase CLI**

    - macOS
    ```shell
    brew install supabase/tap/supabase
    ```
    - Windows
    ```shell
    
    scoop bucket add supabase https://github.com/supabase/scoop-bucket.git
    scoop install supabase
    
    ```
3. **Start Supabase** In your terminal at the root of your local Chatbot UI repository, run:

   ```shell
   supabase start
   ```
4. **Fill in Secrets**
    - Environment Variables In your terminal at the root of your local Chatbot UI repository, run:

   ```shell
   cp .env.local.example .env.local
   ```
   Get the required values by running:

   ```shell
   supabase status
   ```
   Note: Use the API URL from `supabase status` for `NEXT_PUBLIC_SUPABASE_URL`. Update the `.env.local` file with these values.
   
   - SQL Setup In the migration file` supabase/migrations/20240108234540_setup.sql`, replace the following values:
   
     - `project_url (line 53):` Default can remain unchanged unless you change your project_id in the config.toml file.
     - `service_role_key (line 54):` Use the value obtained from supabase status.

5. **Run the App Locally** In your terminal at the root of your local Chatbot UI repository, run:
   ```shell
   npm run chat
   ```

   Your local instance of Chatbot UI should now be running at http://localhost:3000. Use a compatible Node version (e.g.,
v18).

   You can view your backend GUI at http://localhost:54323/project/default/editor.

### Tested on:

- Apple M1 Pro (Sonoma 14.5) ✅
- Windows 11 ✅
- Linux (Coming soon) 🟥
