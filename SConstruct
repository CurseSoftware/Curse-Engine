import subprocess
import os
from pathlib import Path

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

def get_win32_sdk_path():
    if env['PLATFORM'] != 'win32':
        return
    
    import subprocess
    try:
        # Use 'reg query' to find the Windows SDK installation directory
        result = subprocess.check_output(
            'reg query "HKLM\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots" /v KitsRoot10',
            shell=True, text=True
        )
        for line in result.splitlines():
            if "KitsRoot10" in line:
                return line.split()[-1]  # The path is the last element
    except Exception as e:
        print(f"Error detecting Windows SDK path: {e}")
    return None


# env.Append(ENV={'PATH': os.environ['PATH']}) 
# Platorm Compiler Flags 
if env['PLATFORM'] == 'win32':
    import winreg
    import urllib.request

    DX12_HEADERS_PATH = Path(os.getcwd()) / 'external' / 'DirectX-Headers'
    def setup_dx12_env():
        try:
            # Check if the DirectX headers are already installed
            if DX12_HEADERS_PATH.exists():
                print('DX12 Headers already installed')
            else:
                DX12_HEADERS_REPO_URL = 'https://github.com/microsoft/DirectX-Headers.git'
                try: 
                    subprocess.run(
                        ['git', 'clone', DX12_HEADERS_REPO_URL, DX12_HEADERS_PATH],
                        check=True
                    )
                    print(f'Cloned DX12 Headers to {DX12_HEADERS_PATH}.')

                    # print('Building static library for DX12 headers...')
                    # DX12_HEADERS_BUILD_DIR = DX12_HEADERS_PATH / 'build'
                    # try: 
                    #     os.makedirs(DX12_HEADERS_BUILD_DIR)
                    #     print('Created DX12 headers build directory')
                    #     print('Building DX12 Header library')

                    #     try:
                    #         subprocess.run(
                    #             ['cmake', '-S', DX12_HEADERS_PATH, '-B', DX12_HEADERS_BUILD_DIR]
                    #             ,check=True
                    #         )
                    #     except subprocess.CalledProcessError as e:
                    #         print(f'Error building DX12 Headers: {e}')

                    #     try:
                    #         subprocess.run(
                    #             ['cmake', '--build', DX12_HEADERS_BUILD_DIR]
                    #             ,check=True
                    #         )
                    #     except subprocess.CalledProcessError as e:
                    #         print(f'Error building DX12 Headers: {e}')

                    # except FileExistsError:
                    #     print(f'Directory {DX12_HEADERS_BUILD_DIR} already exists.')
                    # except Exception as e:
                    #     print(f'Error creating DX12 Headers build dir: {e}')
                    # print(f'Done building library to {DX12_HEADERS_BUILD_DIR}')
                except subprocess.CalledProcessError as e: 
                    print(f"Error cloning DX12 Headers: {e}")
        except:
            print("ERROR")

    setup_dx12_env()

    def detect_windows_sdk_path():
        """Detect the latest Windows SDK installation path."""
        try:
            # Open the Windows SDK registry key
            with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                            r"SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0",
                            0, winreg.KEY_READ) as key:
                
                # Get the installation folder
                install_dir = winreg.QueryValueEx(key, "InstallationFolder")[0]
                
                # Get the latest SDK version
                with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE,
                                r"SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0",
                                0, winreg.KEY_READ) as version_key:
                    sdk_version = winreg.QueryValueEx(version_key, "ProductVersion")[0]
                    
                # Construct paths for DirectX 12
                include_path = os.path.join(install_dir, 'Include', sdk_version + '.0')
                lib_path = os.path.join(install_dir, 'Lib', sdk_version + '.0')
                
                return {
                    'SDK_PATH': install_dir,
                    'SDK_VERSION': sdk_version,
                    'INCLUDE_PATH': include_path,
                    'LIB_PATH': lib_path
                }
        except WindowsError:
            print("Could not detect Windows SDK. Please ensure it's installed.")
            return None

    # if not ensure_directx_headers():
    #     print("Failed to set up DirectX Headers. Build may fail.")
    windows_sdk_path = detect_windows_sdk_path()

    def install_dx_agility():
        try:
            agility_path = Path(os.getcwd()) / 'external' / 'agility.zip'
            if not agility_path.exists():
                # Invoke-WebRequest -Uri https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.4.10 -OutFile agility.zip
                subprocess.run(
                    [
                        'powershell', 
                        'Invoke-WebRequest', 
                        '-Uri', 'https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.4.10', 
                        '-OutFile', agility_path
                    ]
                    ,check=True
                )
                
                subprocess.run(
                    [
                        'powershell', 
                        'Expand-Archive', 
                        agility_path,
                        '-DestinationPath', 'external/d3d'
                    ]
                    ,check=True
                )

            else:
                print(f'{agility_path} already exists')
        except Exception as e:
            print(f'Could not install Agility SDK: {e}')

    install_dx_agility()
    # env.Append(tools=['msvc'])
    env.Append(CCFLAGS=['/EHsc', '/nologo', '/W3'])
    env.Append(CXXFLAGS=['/std:c++20'])  # For C++20
    env.Append(LIBS=['user32'])

    # windows_sdk_path = find_windows_sdk()
    if windows_sdk_path:
        # dx_header_dir = setup_nuget()
        # vcpkg_install_path = 'vcpkg_installed'
        # vcpkg_root = os.getenv('VCPKG_ROOT', 'C:\\vcpkg')
        env.Append(CPPPATH=[
            DX12_HEADERS_PATH / 'include' / 'directx',
            'external/d3d/build/native/include',
            # os.path.join(os.getcwd(), vcpkg_install_path, 'x64-windows', 'include'),
            os.path.join(windows_sdk_path['INCLUDE_PATH'], 'um'),
            os.path.join(windows_sdk_path['INCLUDE_PATH'], 'shared'),
            DX12_HEADERS_PATH / 'include',
        ])

        env.Append(LIBPATH=[
            os.path.join(windows_sdk_path['LIB_PATH'], 'um', 'x64')
        ])

        env['LIBS'].extend([
            'd3d12', 
            'd3dcompiler',
            'dxgi'
        ])

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