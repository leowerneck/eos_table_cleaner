# Equation of State Table Cleaner

This repository contains a simple C code to clean up equation of state (EOS) tables using a [median filter](https://en.wikipedia.org/wiki/Median_filter). The median filter implementation was taken from [singularity-eos](https://github.com/lanl/singularity-eos), and therefore credit goes to the authors.

## Compiling

Clone this repo and type `make`, i.e.,
```bash
git clone https://github.com/leowerneck/eos_table_cleaner.git
cd eos_table_cleaner
make
```
## Dependencies

The EOS cleaner's only true dependency is [HDF5](https://en.wikipedia.org/wiki/Hierarchical_Data_Format). To compile the code, you will need:
* a C compiler (e.g., gcc), preferably with OpenMP support
* make
* pkg-config
* HDF5

On Ubuntu, you can install all dependencies using:
```bash
sudo apt update && sudo apt install gcc make pkg-config libhdf5-dev
```

On macOS, if you are using [Homebrew](https://brew.sh) (recommended), you can install all dependencies using:
```bash
brew install gcc gmake pkg-config hdf5
```
