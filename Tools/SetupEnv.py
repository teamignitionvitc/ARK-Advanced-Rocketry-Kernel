import os
import subprocess
import platform
import shutil
#<<<<<<< HEAD

def install_dependencies():
    """Peak Automation: Installs the ARM cross-compiler and build tools if missing."""
    if platform.system() == "Linux":
        # Check for essential tools: arm-none-eabi-gcc, make, and git
        deps_missing = not shutil.which("arm-none-eabi-gcc") or not shutil.which("make") or not shutil.which("git")
        if deps_missing:
            print("Required build tools missing. Installing system dependencies...")
            try:
                subprocess.run(["sudo", "apt", "update"], check=True)
                subprocess.run(["sudo", "apt", "install", "-y", 
                                "gcc-arm-none-eabi", "libnewlib-arm-none-eabi", 
                                "libstdc++-arm-none-eabi-newlib", "build-essential", "cmake", "git"], check=True)
            except subprocess.CalledProcessError:
                print("Warning: Failed to install dependencies automatically. Please run: sudo apt install build-essential gcc-arm-none-eabi")

def setup_pico():
    """Locates or downloads the Pico SDK and generates the CMake bridge file."""
    install_dependencies()
    
    # 1. Locate the SDK (Check Environment first, then standard paths)
    sdk_path = os.environ.get("PICO_SDK_PATH")
    if not sdk_path or not os.path.exists(sdk_path):
        target_dir = os.path.expanduser("~/.pico-sdk/sdk/2.2.0")
        if not os.path.exists(os.path.join(target_dir, "pico_sdk_init.cmake")):
            target_dir = os.path.expanduser("~/pico-sdk")
            if not os.path.exists(target_dir):
                print("Pico SDK not found. Automating download...")
                subprocess.run(f"git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git {target_dir}", shell=True, check=True)
        sdk_path = target_dir

    # 2. Automatically generate/update the bridge file for the root CMakeLists.txt
    import_file = "pico_sdk_import.cmake"
#=======
import sys

def install_system_dependencies():
    if platform.system() == "Linux":
        print("Checking system dependencies...")
        deps = ["git", "cmake", "make", "python3"] 
        if not shutil.which("arm-none-eabi-gcc"):
            deps.extend(["gcc-arm-none-eabi", "libnewlib-arm-none-eabi", "libstdc++-arm-none-eabi-newlib", "build-essential"])
        
        deps.extend(["flex", "bison", "gperf", "python3-venv", "python3-setuptools", "python3-serial"])

        missing = [dep for dep in deps if not shutil.which(dep) and "lib" not in dep]
        
        if missing or not shutil.which("arm-none-eabi-gcc"):
             print("Installing missing system dependencies...")
             try:
                subprocess.run(["sudo", "apt-get", "update"], check=True)
                subprocess.run(["sudo", "apt-get", "install", "-y"] + deps, check=True)
             except subprocess.CalledProcessError:
                print("Warning: Failed to install system dependencies.")

def setup_pico():
    print("\n--- Setting up Pico SDK ---")
    sdk_path = os.environ.get("PICO_SDK_PATH")
    
    if not sdk_path:
        default_path = os.path.expanduser("~/pico/pico-sdk")
        if os.path.exists(default_path):
            sdk_path = default_path
            
    if not sdk_path or not os.path.exists(sdk_path):
        target_dir = os.path.expanduser("~/pico/pico-sdk")
        print(f"Pico SDK not found. Cloning to {target_dir}...")
        os.makedirs(os.path.dirname(target_dir), exist_ok=True)
        try:
            subprocess.run(f"git clone --recurse-submodules https://github.com/raspberrypi/pico-sdk.git {target_dir}", shell=True, check=True)
            sdk_path = target_dir
            print("Pico SDK installed successfully.")
        except subprocess.CalledProcessError:
             print("Error: Failed to clone Pico SDK.")
             return None

    print(f"Using Pico SDK at: {sdk_path}")
    
    build_dir = os.path.abspath("Build")
    os.makedirs(build_dir, exist_ok=True)
    
    import_file = os.path.join(build_dir, "pico_sdk_import.cmake")
    
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
    content = (
        "if (NOT PICO_SDK_PATH)\n"
        f"    set(PICO_SDK_PATH \"{sdk_path}\")\n"
        "endif ()\n"
        "include(${PICO_SDK_PATH}/pico_sdk_init.cmake)\n"
    )
#<<<<<<< HEAD
    with open(import_file, "w") as f:
        f.write(content)
            
    return sdk_path

def setup_esp32():
    """Locates or downloads the ESP-IDF and executes installation scripts."""
    target_dir = os.path.expanduser("~/esp-idf")
    if not os.path.exists(target_dir):
        print("ESP-IDF not found. Automating download...")
        subprocess.run(f"git clone --recursive https://github.com/espressif/esp-idf.git {target_dir}", shell=True, check=True)
        
        # Run the SDK-specific installation script
        os.chdir(target_dir)
        install_cmd = "install.bat" if platform.system() == "Windows" else "./install.sh"
        subprocess.run(install_cmd, shell=True, check=True)
    return target_dir
#=======
    try:
        with open(import_file, "w") as f:
            f.write(content)
        print(f"Generated {import_file}")
    except IOError as e:
        print(f"Error writing {import_file}: {e}")

    return sdk_path

def setup_esp32():
    print("\n--- Setting up ESP-IDF ---")
    idf_path = os.environ.get("IDF_PATH")
    
    if not idf_path:
        default_path = os.path.expanduser("~/esp/esp-idf")
        if os.path.exists(default_path):
            idf_path = default_path

    if not idf_path or not os.path.exists(idf_path):
         target_dir = os.path.expanduser("~/esp/esp-idf")
         print(f"ESP-IDF not found. Cloning to {target_dir}...")
         os.makedirs(os.path.dirname(target_dir), exist_ok=True)
         try:
             subprocess.run(f"git clone --recursive https://github.com/espressif/esp-idf.git {target_dir}", shell=True, check=True)
             idf_path = target_dir
             print("ESP-IDF cloned. Running install script...")
             
             install_script = os.path.join(idf_path, "install.bat" if platform.system() == "Windows" else "install.sh")
             subprocess.run(install_script, cwd=idf_path, shell=True, check=True)
             print("ESP-IDF installed successfully.")
         except subprocess.CalledProcessError:
             print("Error: Failed to install ESP-IDF.")
             return None

    print(f"Using ESP-IDF at: {idf_path}")
    return idf_path

def create_env_file(pico_path, idf_path):
    print("\n--- Generating Environment Script ---")
    
    build_dir = os.path.abspath("Build")
    os.makedirs(build_dir, exist_ok=True)
    
    env_file = os.path.join(build_dir, "env.sh")
    
    with open(env_file, "w") as f:
        f.write("# Source this file to set up your environment: source Build/env.sh\n")
        if pico_path:
            f.write(f"export PICO_SDK_PATH=\"{pico_path}\"\n")
        if idf_path:
            f.write(f"export IDF_PATH=\"{idf_path}\"\n")
            f.write(f"source \"{idf_path}/export.sh\"\n")
            
    print(f"Created {env_file}. Run 'source Build/env.sh' to activate.")

if __name__ == "__main__":
    install_system_dependencies()
    pico_path = setup_pico()
    idf_path = setup_esp32()
    create_env_file(pico_path, idf_path)
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
