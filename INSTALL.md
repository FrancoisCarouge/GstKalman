# Installation

## From Repository Sources

Install and use the library in your projects by cloning the repository, configuring, and installing the project on all platforms:

```shell
git clone --depth 1 "https://github.com/FrancoisCarouge/gstkalman"
cmake -S "gstkalman" -B "build"
cmake --build "build" --parallel
sudo cmake --install "build"
```

# Development Build & Run

## Installation Packages

### Linux

```shell
git clone --depth 1 "https://github.com/FrancoisCarouge/gstkalman"
cmake -S "gstkalman" -B "build"
cmake --build "build" --target "package" --parallel
cmake --build "build" --target "package_source" --parallel
```

### Windows

```shell
git clone --depth 1 "https://github.com/FrancoisCarouge/gstkalman"
cmake -S "gstkalman" -B "build"
cmake --build "build" --target "package" --parallel --config "Release"
cmake --build "build" --target "package_source" --parallel --config "Release"
```