<div align="center">
  <p>
    <h1>GlyphVisualizer</h1>
    <h6>A Glyph composition player written with the Qt6 framework in C++ that plays Glyph compositions from Nothing Phones.</h6>
    <a href="https://github.com/SebiAi/GlyphVisualizer/releases/latest">
      <img alt="GitHub release (with filter)" src="https://img.shields.io/github/v/release/SebiAi/GlyphVisualizer?label=Latest%20Stable%20Release">
    </a>
    <a href="https://github.com/SebiAi/GlyphVisualizer/blob/main/LICENSE">
      <img alt="GitHub License" src="https://img.shields.io/github/license/SebiAi/GlyphVisualizer">
    </a>
    <br><br>
    <video src="https://github.com/SebiAi/GlyphVisualizer/assets/41116921/c86e6f7a-e418-418c-8670-897158e792b1" controls></video>
    <h6>
      Credit: First composition by
      <a href="https://x.com/MrSteel_X/status/1701143539806019611" target="_blank" rel="noopener noreferrer">Mr Steel</a>, Second composition by
      <a href="https://www.reddit.com/r/NothingTech/comments/165nu0w/glyph_synchronized_ringtone_showcase/" target="_blank" rel="noopener noreferrer">ImSteazyy</a>
    </h6>
  </p>
</div>

***

# :writing_hand: Table of contents
<!-- TOC Generator settings -->
<!-- Preset: GitHub -->
<!-- indent characters: '-*+' -->
<!-- algorithm used to generate anchors: DEFAULT -->
<!-- Max indent level: 3 -->
<!-- trim toc indent: True -->
<!-- oneshot: False -->
<!-- generate anchors: True -->
<!-- anchors prefix: 'heading-' -->
<!-- concat spaces: True -->
<!-- comment style: HTML -->
<!-- Is a bit buggy with generating the correct links to the headings - please double check -->
<!-- TOC start (generated with https://github.com/derlin/bitdowntoc) -->

- [:grey_question: What is this? Why would I need that?](#heading-grey_question-what-is-this-why-would-i-need-that)
- [:pushpin: Disclaimer](#heading-pushpin-disclaimer)
- [:safety_vest: Need help?](#heading-safety_vest-need-help)
- [:page_with_curl: Roadmap](#heading-page_with_curl-roadmap)
- [:rocket: How to use](#heading-rocket-how-to-use)
- [:inbox_tray: Installation](#heading-inbox_tray-installation)
   * [<img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable](#heading-windows-portable)
   * [:penguin: Linux](#heading-penguin-linux)
      + [<img src="https://www.vectorlogo.zone/logos/ubuntu/ubuntu-icon.svg" height="20"/> Ubuntu and derivatives](#heading-ubuntu-and-derivatives)
   * [<img src="https://www.vectorlogo.zone/logos/apple/apple-tile.svg" height="20"/> MacOS](#heading-macos)
- [:construction: Compilation](#heading-construction-compilation)
   * [:nut_and_bolt: Build Dependencies](#heading-nut_and_bolt-build-dependencies)
   * [:hammer_and_pick: Build](#heading-hammer_and_pick-build)
   * [:package: Package up](#heading-package-package-up)
      + [<img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable](#heading-windows-portable-1)
      + [<img src="https://www.vectorlogo.zone/logos/ubuntu/ubuntu-icon.svg" height="20"/> Ubuntu and derivatives](#heading-ubuntu-and-derivatives-1)
      + [<img src="https://www.vectorlogo.zone/logos/apple/apple-tile.svg" height="20"/> MacOS](#heading-macos-1)
- [:interrobang: Troubleshooting](#heading-interrobang-troubleshooting)
   * [<img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable](#heading-windows-portable-2)
      + [Missing dlls](#heading-missing-dlls)

<!-- TOC end -->

***

<!-- TOC --><a name="heading-grey_question-what-is-this-why-would-i-need-that"></a>
# :grey_question: What is this? Why would I need that?
This is a tool that is meant to be used in combination with my [custom-nothing-glyph-tools](https://github.com/SebiAi/custom-nothing-glyph-tools/) scripts (*GlyphTranslator*, *GlyphModder*). When you create a custom ringtone or notification tone with these scripts, you want to test them as often as possible. You can use this tool to visualize your composition and rapidly iterate on it - no file transfer to your phone is needed.

And apart from that, you can use it if you want to view the composition without having access to a Nothing Phone.

***

<!-- TOC --><a name="heading-pushpin-disclaimer"></a>
# :pushpin: Disclaimer
> This software is provided as-is without any warranty. I and all other contributors are not responsible for any damage, misuse or other kind of physical or mental damage that results from the use of this software.
This repo is in no way, shape or form affiliated with Nothing Technology Limited (NOTHING).

***

<!-- TOC --><a name="heading-safety_vest-need-help"></a>
# :safety_vest: Need help?
If you need help, look at the [Troubleshooting](#interrobang-troubleshooting) chapter. If this does not help either feel free to use the [Discussion feature on GitHub](https://github.com/SebiAi/GlyphVisualizer/discussions) or reach out to me on Discord: @sebiai

***

<!-- TOC --><a name="heading-page_with_curl-roadmap"></a>
# :page_with_curl: Roadmap
You can find the full roadmap [here](https://github.com/users/SebiAi/projects/5).

***

<!-- TOC --><a name="heading-rocket-how-to-use"></a>
# :rocket: How to use
For now, you need to use the [custom-nothing-glyph-tools](https://github.com/SebiAi/custom-nothing-glyph-tools/) to extract the light data (csv/glypha) from the audio file:
1. Download and extract the custom-nothing-glyph-tools: [Download](https://github.com/SebiAi/custom-nothing-glyph-tools/archive/refs/heads/main.zip)
2. Install the requirements for custom-nothing-glyph-tools: [Tutorial](https://github.com/SebiAi/custom-nothing-glyph-tools#memo-requirements)
3. Use the *GlyphModder* script to extract the light data: [Tutorial](https://github.com/SebiAi/custom-nothing-glyph-tools#read-from-an-audio-file)

Now pass the original composition audio and the extracted light data to the software and it will play it back.

***

<!-- TOC --><a name="heading-inbox_tray-installation"></a>
# :inbox_tray: Installation
<!-- TOC --><a name="heading-windows-portable"></a>
## <img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable
1. Download the latest portable version (`GlyphVisualizer-*_windows-x64-portable.zip`) from [the latest release](https://github.com/SebiAi/GlyphVisualizer/releases/latest).
2. (Optional) Verify file integrity by executing this in PowerShell: `CertUtil -hashfile GlyphVisualizer_windows-x64.zip SHA256`. Compare that output with the `GlyphVisualizer-*_windows-x64-portable.zip.sha256.txt` sha256 hash file. If they match, you are good.
3. Extract the zip file
4. Install `vc_redist.x64.exe` which is inside the zip
5. Run `GlyphVisualizer.exe`

<!-- TOC --><a name="heading-penguin-linux"></a>
## :penguin: Linux
<!-- TOC --><a name="heading-ubuntu-and-derivatives"></a>
### <img src="https://www.vectorlogo.zone/logos/ubuntu/ubuntu-icon.svg" height="20"/> Ubuntu and derivatives
1. Download the latest version (`GlyphVisualizer-*_linux-ubuntu-x64.AppImage`) from [the latest release](https://github.com/SebiAi/GlyphVisualizer/releases/latest).
2. (Optional) Verify file integrity by downloading `GlyphVisualizer-*_linux-ubuntu-x64.AppImage.sha256` to the same directory as the AppImage and running `shasum -a 256 -c ./GlyphVisualizer-*_linux-ubuntu-x64.AppImage.sha256`. If it outputs 'OK', you are good.
3. Install dependencies for AppImages: `sudo apt install -y libfuse2*`
4. Make the AppImage executable: `chmod +x ./GlyphVisualizer-*_linux-ubuntu-x64.AppImage`
5. Run the AppImage - you can use [AppImageLauncher](https://github.com/TheAssassin/AppImageLauncher) to update and integrate the application into your dekstop environment.

<!-- TOC --><a name="heading-macos"></a>
## <img src="https://www.vectorlogo.zone/logos/apple/apple-tile.svg" height="20"/> MacOS
1. Download the latest version (`GlyphVisualizer-*_macOS-12-x64.dmg`) from [the latest release](https://github.com/SebiAi/GlyphVisualizer/releases/latest).
2. (Optional) Verify file integrity by downloading `GlyphVisualizer-*_macOS-12-x64.dmg.sha256` to the same directory as the AppImage and running `shasum -a 256 -c ./GlyphVisualizer-*_macOS-12-x64.dmg.sha256`. If it outputs 'OK', you are good.
3. Run the application

***

<!-- TOC --><a name="heading-construction-compilation"></a>
# :construction: Compilation
<!-- TOC --><a name="heading-nut_and_bolt-build-dependencies"></a>
## :nut_and_bolt: Build Dependencies
You can use the Qt Online Installer to install all the build dependencies.
* [Qt 6.6.0](https://www.qt.io/download)
* [Ninja](https://ninja-build.org/)
* [CMake](https://cmake.org/)
* A compiler like g++ or MSVC depending on your OS

<!-- TOC --><a name="heading-hammer_and_pick-build"></a>
## :hammer_and_pick: Build
Clone the repo
```bash
git clone https://github.com/SebiAi/GlyphVisualizer.git
cd GlyphVisualizer
```
Set `DCMAKE_PREFIX_PATH` to the Qt location and build the application
```bash
cmake -G Ninja "-DCMAKE_PREFIX_PATH=/path/to/qt" "-DCMAKE_BUILD_TYPE:STRING=Release" -S . -B build && cmake --build build --config Release
```
The built application is now in the `build` directory.

<!-- TOC --><a name="heading-package-package-up"></a>
## :package: Package up
The application itself may not execute because it is not finding the linked libraries. Packaging guarantees that the application finds them and runs as expected.
<!-- TOC --><a name="heading-windows-portable-1"></a>
### <img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable
Move the application to its own directory
```batch
mkdir GlyphVisualizer-main_windows-x64-portable
move build/GlyphVisualizer GlyphVisualizer-main_windows-x64-portable
```
Use [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html)
```batch
windeployqt --release GlyphVisualizer-main_windows-x64-portable\GlyphVisualizer.exe
```
The directory `GlyphVisualizer-main_windows-x64-portable` now contains the application and all its dynamic dependencies. Can be zip'ed up and transferred to other Windows machines.

<!-- TOC --><a name="heading-ubuntu-and-derivatives-1"></a>
### <img src="https://www.vectorlogo.zone/logos/ubuntu/ubuntu-icon.svg" height="20"/> Ubuntu and derivatives
Download dependencies (linuxdeploy, libfuse2, libxcb-cursor0)
```bash
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy-plugin-appimage-x86_64.AppImage
sudo apt install -y libfuse2 libxcb-cursor0
```
Set `QMAKE` to the qt *qmake* executable and use linuxdeploy
```bash
QMAKE="/path/to/my/qmake" LDAI_VERSION="main" LDAI_OUTPUT="GlyphVisualizer-main_linux-ubuntu-x64.AppImage" ./linuxdeploy-x86_64.AppImage --executable "build/GlyphVisualizer" --desktop-file "deploy/linux/AppImage/com.sebiai.GlyphVisualizer.desktop" --icon-file "deploy/linux/AppImage/GlyphVisualizer.png" --plugin qt --appdir AppDir --output appimage
```
You now have `GlyphVisualizer-main_linux-ubuntu-x64.AppImage`, which can be transferred to other Ubuntu-based distributions.

<!-- TOC --><a name="heading-macos-1"></a>
### <img src="https://www.vectorlogo.zone/logos/apple/apple-tile.svg" height="20"/> MacOS
Navigate to the build folder
```bash
cd build
```
Use [macdeployqt](https://doc.qt.io/qt-6/macos-deployment.html#the-mac-deployment-tool)
```bash
macdeployqt GlyphVisualizer.app -dmg
```
Rename
```bash
mv GlyphVisualizer.dmg GlyphVisualizer-main_macOS-12-x64.dmg
```
You now have `GlyphVisualizer-main_macOS-12-x64.dmg`, which can be transferred to other MacOS machines.

<!-- TOC --><a name="heading-interrobang-troubleshooting"></a>
# :interrobang: Troubleshooting
<!-- TOC --><a name="heading-windows-portable-2"></a>
## <img src="https://www.vectorlogo.zone/logos/microsoft/microsoft-icon.svg" height="20"/> Windows portable
<!-- TOC --><a name="heading-missing-dlls"></a>
### Missing dlls
| ![Missing dll error popup](./docs/assets/troubleshooting_dll_not_found_error.png) | <blockquote>GlyphVisualizer.exe - System Error<br><br>The code execution cannot proceed because MSVCP140.dll was not found. Reinstalling the program may fix this problem.</blockquote> |
|:---:|:---|

If you get the error popup above, you need to install the *Microsoft Visual C++ Redistributable*, which is shipped alongside GlyphVisualizer. Please execute `vc_redist.x64.exe` to install it.

> [!IMPORTANT]
> This also applies if the missing dll is one of the following:
> * MSVCP140.dll
> * VCRUNTIME140_1.dll
> * VCRUNTIME140.dll
