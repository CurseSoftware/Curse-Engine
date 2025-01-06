import subprocess
import os

def run_executable(target, source, env):
    program = str(source[0])  # The built executable
    subprocess.run([program])  # Run the program


# Read the build mode from the command line (default to 'release')
mode = ARGUMENTS.get('mode', 'release')
run_target = ARGUMENTS.get('run', None)

# Define flags for debug and release modes
debug_flags = ['-g', '-O0']
release_flags = ['-O2']

build_dir = 'build'


debug_env = Environment(CCFLAGS=debug_flags)
release_env = Environment(CCFLAGS=release_flags)


if mode == 'debug':
    env = debug_env
else:
    env = release_env


# env.Append(ENV={'PATH': os.environ['PATH']}) 
# Platorm Compiler Flags 
if env['PLATFORM'] == 'win32':
    # env.Append(tools=['msvc'])
    env.Append(CCFLAGS=['/EHsc'])
    env.Append(CXXFLAGS=['/std:c++20'])  # For C++20
    env.Append(LIBS=['user32'])
    # env.Append(LIBS=[
    #     'user32.lib',   # For message translation, window handling
    #     # 'kernel32.lib'  # For core Windows system services
    # ])
    # env.Append(LINKCOMSTR='Linking $TARGET: $LIBS')
    # env.Append(LIBPATH=[
    #     'C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22000.0/um/x64'  # Adjust path for your SDK version and architecture
    # ])
else:
    env.Append(CXXFLAGS='-std=c++20')  # For C++20

# env.Tool('compilation_db')
# compdb = env.CompilationDatabase(output_directory=".vscode")

engine_target = SConscript('engine/SConscript', exports={'env': env}, variant_dir=f'{build_dir}/engine/', duplicate=0)
testbed_target = SConscript('testbed/SConscript', exports={'env': env}, variant_dir=f'{build_dir}/testbed', duplicate=0)

if run_target == 'testbed':
    Command('run-testbed', testbed_target[0], run_executable)
else:
    pass