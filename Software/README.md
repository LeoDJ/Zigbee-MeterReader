# nRF52840 Zigbee Power Meter Firmware

**WIP!** Note: This project is currently under heavy development and should not be used in its current state.

## Getting Started
Set the enivronment variable `NRF5_ZIGBEE_SDK_PATH` to the installation path of the [nRF5 SDK for Thread and Zigbee](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK-for-Thread-and-Zigbee).

If you have your own working toolchain that should be enough. My own toolchain is listed below, once the project has reached a more mature state, I'll document the build instructions better.


## My Toolchain
* Windows 10 with WSL 1
* WSL 1 installed packages:
    * `gcc-arm-embedded`
* JLink utility added to PATH (folder with `JLink.exe` and `JLinkGDBServerCL.exe` in it)
* Environment variables:
    * `NRF5_ZIGBEE_SDK_PATH=<SDK download location>`
    * `WSLENV=NRF5_ZIGBEE_SDK_PATH/p`<br> for it to also get set in WSL
* VS Code Extensions:
    * C/C++
    * Cortex-Debug
* J-Link as flash and debug adapter