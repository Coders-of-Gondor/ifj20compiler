# e2e testing of ifj20compiler

This test suite relies on a library called `bats-core`, which is a system test
suite for bash. You can read more about it in their [GitHub repo](https://github.com/bats-core/bats-core).

Before you run the test suite, make sure that you have `bats` installed on your
system. To install it on Fedora, run: `sudo dnf install bats`. To install it on
Arch, check out https://bugs.archlinux.org/task/63099?project=5&string=bash-bats.

Only execute the tests using `make test2e2` from the project's root directory.

## How to write e2e tests using bats

Bats separates test suites (groups of tests) into files that contain tests.

### Creating a new test suite

1. Create a new file with extension `.bats` and prepend it with `xxx-` where
   `xxx` stand for a three-digit number (decides the order in which the tests
   are run).

2. The first line needs to contain a `shebang`: `#!/usr/bin/env bats`

3. Another line needs to contain: `load helpers`. This loads variables and
   functions in file `helpers.bash` that contains a few handy things.

### Extending an existing test suite

A test case is written inside of a special test block. To create a new test,
create a new block somewhere in file:

```bash
@test "<test description>" {
  <test content>
}
```

`<test description>` is simply a string.

`<test content>` contains classic bash/shell code.

Here is a list of useful functions and variables that you should use for
writing the tests:

- `run_compiler()` - executes the compiler; if the first argument is a number
                     it is used as the expected return code

- `output` - a variable containing the output of compiler's execution (is
             populated after executing `run_compiler()`

- `status` - a variable containing the return code after running the compiler
             (is populated after executing `run_compiler()`)

To check for values in any variables, use this syntax:

```bash
  [ "$variable" = "expected value"]
```
