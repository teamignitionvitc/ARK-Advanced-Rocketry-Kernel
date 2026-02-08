import os
import shutil
import platform
import getpass
import sys
#<<<<<<< HEAD
#=======
import subprocess
from SetupEnv import setup_esp32
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)

def upload_pico():
    binary = "Build/UserBuild/ARK_Pico.uf2"
    if not os.path.exists(binary):
        print(f"Error: {binary} not found. Run Build.py first.")
        return

    pico_drive = None
    user = getpass.getuser()

#<<<<<<< HEAD
    # Define likely mount points for WSL and Native Linux
    paths = [
        f"/media/{user}/RPI-RP2",
        f"/run/media/{user}/RPI-RP2",
        "/mnt/d", # WSL standard mount for D: drive
        "/mnt/e", # WSL standard mount for E: drive
    ]
#=======
    paths = [
        f"/media/{user}/RPI-RP2",
        f"/run/media/{user}/RPI-RP2",
        "/mnt/d",
        "/mnt/e",
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
        "/mnt/f",
        "/mnt/pico"
    ]

    for p in paths:
        if os.path.exists(p):
#<<<<<<< HEAD
            # Verify it's actually a Pico by looking for the info file
#=======
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
            if os.path.exists(os.path.join(p, "INFO_UF2.TXT")):
                pico_drive = p
                break
    
    if pico_drive:
#<<<<<<< HEAD
        print(f"Uploading to Pico found at {pico_drive}...")
#=======
        print(f"Uploading to Pico at {pico_drive}...")
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
        try:
            shutil.copy(binary, pico_drive)
            print("Upload Successful! Hardware should reboot now.")
        except Exception as e:
            print(f"Failed to copy: {e}")
    else:
        print("Pico not found in WSL. Have you mounted the drive?")
#<<<<<<< HEAD
        print("Try: sudo mount -t drvfs D: /mnt/d  (Replace D: with your Pico's drive letter)")

if __name__ == "__main__":
    upload_pico()
#=======
    print("Try: sudo mount -t drvfs D: /mnt/d")

def upload_esp32():
    binary = "Build/UserBuild/ARK_Esp32.bin"
    if not os.path.exists(binary):
        print(f"Error: {binary} not found.")
        return

    idf_path = setup_esp32()
    if not idf_path:
        print("ESP-IDF not set up.")
        return

    print("Uploading to ESP32...")
    
    esptool_path = os.path.join(idf_path, "components/esptool_py/esptool/esptool.py")
    if not os.path.exists(esptool_path):
         esptool_path = "esptool.py" 
    
    user_build_dir = os.path.abspath("Build/UserBuild")
    
    cmd = [
        sys.executable, esptool_path,
        "--chip", "esp32",
        "--before", "default_reset",
        "--after", "hard_reset",
        "write_flash",
        "--flash_mode", "dio",
        "--flash_size", "detect",
        "--flash_freq", "40m",
        "0x1000", os.path.join(user_build_dir, "bootloader.bin"),
        "0x8000", os.path.join(user_build_dir, "partition-table.bin"),
        "0x10000", os.path.join(user_build_dir, "ARK_Esp32.bin")
    ]
    
    try:
        subprocess.run(cmd, check=True)
        print("\nSUCCESS: ESP32 flashing complete.")
    except Exception as e:
        print(f"Flashing failed: {e}")

if __name__ == "__main__":
    target = sys.argv[1].lower() if len(sys.argv) > 1 else "pico"
    if target == "pico": 
        upload_pico()
    elif target == "esp32":
        upload_esp32()
    else:
        print("Invalid target.")
#>>>>>>> 452b8f4 (Re-initialized repository with clean .gitignore and synced structure)
