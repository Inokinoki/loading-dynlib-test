# Test loading dynamic library

Try to use `dlopen` to test dynamic library loadings to reproduce a bug on macOS.

## On Linux or on macOS

Run:

```
make clean && make
```

to build.

Run:

```
main.out
```

and

```
main.local.out
```

to see whether there are any differences.

Remove the `-fvisibility=hidden`, build and run again to see whether there are any differences.
