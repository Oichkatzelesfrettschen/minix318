# MINIX4 Analysis Tools Installation Script for Windows
# Run with: py install_analysis_tools.py

import subprocess
import sys

def install_package(package):
    """Install a Python package using pip"""
    try:
        subprocess.check_call([sys.executable, "-m", "pip", "install", package])
        print(f"✓ Successfully installed {package}")
        return True
    except subprocess.CalledProcessError:
        print(f"✗ Failed to install {package}")
        return False

def install_with_choco(package):
    """Install a package using Chocolatey (if available)"""
    try:
        subprocess.check_call(["choco", "install", package, "-y"])
        print(f"✓ Successfully installed {package} via Chocolatey")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print(f"✗ Failed to install {package} via Chocolatey")
        return False

def main():
    print("Installing MINIX4 Code Analysis Tools...")
    
    # Core Python analysis tools
    python_tools = [
        "lizard",           # Complexity analysis
        "radon",           # Code metrics
        "vulture",         # Dead code detection
        "bandit",          # Security scanner
        "flawfinder",      # C security scanner
        "plotly",          # Visualization
        "pandas",          # Data analysis
        "jinja2",          # Templates
        "gitpython",       # Git analysis
        "tree-sitter",     # Parsing
        "tree-sitter-c",   # C grammar
        "pycparser",       # C parser
        "cppcheck-codequality", # cppcheck wrapper
    ]
    
    # Install Python tools
    success_count = 0
    for tool in python_tools:
        if install_package(tool):
            success_count += 1
    
    print(f"\nInstalled {success_count}/{len(python_tools)} Python tools")
    
    # Try to install system tools via Chocolatey
    system_tools = [
        "cppcheck",        # Static analysis
        "cloc",           # Line counting
        "git",            # Version control (usually already installed)
        "nodejs",         # For jscpd
    ]
    
    print("\nTrying to install system tools via Chocolatey...")
    choco_success = 0
    for tool in system_tools:
        if install_with_choco(tool):
            choco_success += 1
    
    if choco_success > 0:
        print(f"Installed {choco_success}/{len(system_tools)} system tools via Chocolatey")
    else:
        print("Chocolatey not available. Install manually:")
        print("- cppcheck: https://cppcheck.sourceforge.io/")
        print("- cloc: https://github.com/AlDanial/cloc")
        print("- Node.js: https://nodejs.org/ (for jscpd)")
    
    # Try to install jscpd via npm
    try:
        subprocess.check_call(["npm", "install", "-g", "jscpd"])
        print("✓ Successfully installed jscpd via npm")
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("✗ Failed to install jscpd via npm (Node.js may not be available)")
    
    print("\nInstallation complete!")
    print("Run analysis with: py scripts/analysis/advanced_analysis.py")

if __name__ == "__main__":
    main()
