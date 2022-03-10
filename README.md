# vilya, a cool ELF packer for all your packing needs :)

## TODO

- [ ] build straightforward and generic wait to get a program's entrypoint
- [ ] encapsulate target code, into a newly formed program that will decode and run it

## Supported binary architectures

By default, `vilya` should find the architecture of the binary your are trying to pack, but if it doesn't work,
you can try to force the binary's architecture like so:
```bash
./vilya -a x86-64 some-binary
```

Currently supported architectures are:
`x86-64, amd64`

## Installing

To run and build the packer :
```bash
# install dependencies
sudo apt install -y libelf-dev clang make

git clone git@github.com:bogdzn/vilya.git
cd vilya
make
```

To run the unit tests, you will need to install [Criterion](https://github.com/Snaipe/Criterion)
