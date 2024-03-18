# Whole-program-llvm for OP-TEE OS

Tested on Ubuntu 22.04 x86_64

## Installing dependencies

```sh
# install wllvm
pip install wllvm

# install clang
cd $OPTEE_TOP_DIR/build
make clang-toolchains
```

## Compiling OP-TEE OS

```sh
cd $OPTEE_TOP_DIR/build
LLVM_COMPILER_PATH=../clang-12.0.0/bin LLVM_COMPILER=clang BINUTILS_TARGET_PREFIX=aarch64-linux-gnu make COMPILER=wllvm optee-os
```

Note that the version of Clang that `make clang-toolchains` install
may be changed in the future.

## Extracing bitcode and optionally disassmbling bitcode into human-readable format

```sh
cd $OPTEE_TOP_DIR/optee-os/out/arm/core
extract-bc tee.elf # outputs tee.elf.bc on success
llvm-dis tee.elf.bc # outputs tee.elf.ll on success
```

Note that the `tee.elf` should have `.llvm_bc` section.