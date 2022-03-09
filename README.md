# vilya, a cool ELF packer for all your packing needs :)

## Supported binary architectures

By default, `vilya` should find the architecture of the binary your are trying to pack, but if it doesn't work,
you can try to force the binary's architecture like so:
```bash
./vilya -a x86-64 some-binary
```

Currently supported architectures are:
`x86-64, x64-x64-x32`

## Installing

To run and build the packer, no special dependency is required, besides `clang` and `make`:
```bash
git clone git@github.com:bogdzn/vilya.git
cd vilya
make
```

To run the unit tests, you will need to install [Criterion](https://github.com/Snaipe/Criterion)
