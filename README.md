# CS106B 2021

[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/AnthonyYsw/CS106B_2021_raw/blob/main/README.md)
[![zh-CN](https://img.shields.io/badge/lang-中-red.svg)](https://github.com/AnthonyYsw/CS106B_2021_raw/blob/main/README.zh-CN.md)

All resources from Stanford CS106B 2021 are stored here. This is the best version of CS106B for self-learners, as it has the most complete set of resources available online for free. Downloaded from the [Stanford CS106B Website](https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1214/)

This repository is shared solely for study purposes.

Stanford offers **the best C++ class in the world**, and the assignments are the highlight of this course—make sure to complete them!

## Two compulsory steps before starting your assignments...

### 1. Download Qt 5.15.2 and Qt Creator GUI (Follow the detailed instructions in /Resources/Qt_Creator/.)

Unfortunately, Qt 5.15.2 is no longer available from the official online installer. You can download Qt 5.15.2 by adding an open-source mirror, such as [Aliyun Qt Mirror](https://mirrors.aliyun.com/qt/)(best for users in China). If you are outside China mainland, you may find other available links.

Setps:

1. Download the official online installer, which you can find on the [Qt Website](https://download.qt.io/official_releases/online_installers)
2. Open "Settings" (the gear icon).
3. Go to the "Repositories" tab and add the following links as temporary repositories. If you are not a Mac user, modify the links accordingly for Linux or Windows (you can do it—I trust you!).
   
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_src_doc_examples/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_wasm/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator_gui/

4. Follow the instructions, and complete.

### 2. Install the Stanford C++ Library

You need to compile the Stanford C++ library with the Qt 5.15.2 compiler before starting the assignments. If you are not a Stanford student, you may not be able to download "CS106B.zip". Instead, follow these steps:

1. **CLONE** the `stanford-cpp-library` from [here](https://github.com/zelenski/stanford-cpp-library.git).
2. **RESET** the library to the 5.15.2 version, because the latest repository is for Qt6. The older Stanford C++ library can only be built with Qt 5.15! Run the following command in your shell:

```
git reset --hard 7ffc69fff2307b5d607b300a4443755af1474cb2
```

3. Open "BothLibraryWelcome.pro" inside the `⁠/stanford-cpp-library/` folder by double-clicking it. This will open Qt Creator. Select Qt 5.15.2 and build the project. The Stanford C++ library will be installed on your computer without errors.

Good Luck!
