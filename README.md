# Gravity Game

## Building and Running
The build system for this is [Scons]("https://scons.org/") which is a Python-based build system.
In order to build, you can simply run the command `scons` from the root of the project.
To run the *testbed* program you can run `scons run=testbed`.

### Debug builds
To create a debug build run `scons mode=debug`. Otherwise it will default to `release`.

### Cleaning build
Run `scons -c` to clean the build.