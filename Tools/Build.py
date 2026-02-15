import os
import sys
import shutil
import subprocess
from SetupEnv import setup_pico, setup_esp32

def find_toolchain(sdk_path):
#<<<<<<< HEAD
    """Recursively hunts for any valid ARM GCC toolchain file within the SDK."""
    # List of possible filenames in different SDK versions
#=======
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
    targets = ["pico_arm_gcc.cmake", "pico_arm_cortex_m0plus_gcc.cmake"]
    
    print(f"Scanning {sdk_path} for toolchain...")
    for root, dirs, files in os.walk(sdk_path):
        for file in files:
            if file in targets:
                found_path = os.path.join(root, file)
                print(f"Found toolchain: {found_path}")
                return found_path
    return None

def build_pico():
    sdk_path = setup_pico()
    temp_dir = os.path.abspath("Build/Temp/Pico")
    user_build_dir = os.path.abspath("Build/UserBuild")
    project_root = os.path.abspath(".")
    
    sdk_import_src = os.path.join(sdk_path, "external", "pico_sdk_import.cmake")
    sdk_import_dst = os.path.join(project_root, "pico_sdk_import.cmake")
    if os.path.exists(sdk_import_src) and not os.path.exists(sdk_import_dst):
        shutil.copy(sdk_import_src, sdk_import_dst)
        print(f"Copied pico_sdk_import.cmake to project root")
    
    if os.path.exists(temp_dir): 
        shutil.rmtree(temp_dir)
    os.makedirs(temp_dir, exist_ok=True)
    os.makedirs(user_build_dir, exist_ok=True)
    
    toolchain = find_toolchain(sdk_path)
    if not toolchain:
        print(f"Error: Could not locate a valid toolchain file in {sdk_path}")
        sys.exit(1)

    cmake_cmd = [
        "cmake", "-S", ".", "-B", temp_dir,
        f"-DPICO_SDK_PATH={sdk_path}",
        f"-DCMAKE_TOOLCHAIN_FILE={toolchain}",
        "-DPICO_BOARD=pico",
        "-DARCH_TARGET=PICO",
        "-G", "Unix Makefiles"
    ]
    
    build_env = os.environ.copy()
    build_env.pop("IDF_PATH", None)
    
    print(f"--- Configuring ARK for Pico ---")
    subprocess.run(cmake_cmd, check=True, env=build_env)
    
    print("--- Building ARK Avionics ---")
    subprocess.run(["cmake", "--build", temp_dir, "-j"], check=True, env=build_env)
    
#<<<<<<< HEAD
    # Final step: Move binary to UserBuild
#=======
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
    source = os.path.join(temp_dir, "ARK_Avionics.uf2")
    if os.path.exists(source):
        dest = os.path.join(user_build_dir, "ARK_Pico.uf2")
        shutil.copy(source, dest)
#<<<<<<< HEAD
        print(f"\nSUCCESS: Binary moved to -> {dest}")
#=======
        print(f"\nSUCCESS: {dest}")

def build_esp32():
    idf_path = setup_esp32()
    if not idf_path:
        print("Error: ESP-IDF setup failed")
        sys.exit(1)

    temp_dir = os.path.abspath("Build/Temp/Esp32")
    user_build_dir = os.path.abspath("Build/UserBuild")
    
    if os.path.exists(temp_dir):
        shutil.rmtree(temp_dir)
    os.makedirs(temp_dir, exist_ok=True)
    os.makedirs(user_build_dir, exist_ok=True)

    print("--- Configuring ARK for ESP32 ---")
    config_cmd = f". {idf_path}/export.sh && cmake -S . -B {temp_dir} -DCMAKE_TOOLCHAIN_FILE={idf_path}/tools/cmake/toolchain-esp32.cmake -DTARGET=esp32 -G 'Unix Makefiles'"
    
    env = os.environ.copy()
    env["IDF_PATH"] = idf_path
    
    subprocess.run(["bash", "-c", config_cmd], check=True, env=env)
    
    print("--- Building ARK Avionics (ESP32) ---")
    build_cmd = f". {idf_path}/export.sh && cmake --build {temp_dir} -j"
    subprocess.run(["bash", "-c", build_cmd], check=True, env=env)
    
    bin_name = "ARK_Avionics.bin"
    source = os.path.join(temp_dir, bin_name)
    if os.path.exists(source):
        dest = os.path.join(user_build_dir, "ARK_Esp32.bin")
        shutil.copy(source, dest)
        shutil.copy(os.path.join(temp_dir, "bootloader/bootloader.bin"), os.path.join(user_build_dir, "bootloader.bin"))
        shutil.copy(os.path.join(temp_dir, "partition_table/partition-table.bin"), os.path.join(user_build_dir, "partition-table.bin"))
        print(f"\nSUCCESS: {dest}")

def clean_build():
    print("--- Cleaning Build Artifacts ---")
    paths = [
        os.path.abspath("Build/Temp"),
        os.path.abspath("build_pico"),
        os.path.abspath("build_esp32")
    ]
    
    for p in paths:
        if os.path.exists(p):
            print(f"Deleting {p}...")
            shutil.rmtree(p)
    print("Clean complete.")
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)

if __name__ == "__main__":
    target = sys.argv[1].lower() if len(sys.argv) > 1 else "pico"
    if target == "pico": 
        build_pico()
#<<<<<<< HEAD
#=======
    elif target == "esp32":
        build_esp32()
    elif target == "clean":
        clean_build()
    else:
        print("Invalid target. Use 'pico', 'esp32', or 'clean'.")
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)

