# CS106B 2021

All resources from Stanford CS106B 2021 are stored here. It is the best version of CS106B for self-learning as it has the most completed resources online for free access. Downloaded from [Stanford CS106B Website](https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1214/)

This repository is only shared for study.

Stanford is providing THE best C++ class on this planet, and the assignments are THE best part of this course, so please do it!

## Two compulsory steps before starting your assignments ...

### 1. Download the Qt5.15.2 and Qt Creator GUI (do it with detailed instructions at /Resources/Qt_Creator/.)

Unfortunately, Qt5.15.2 not available from offical online downloader anymore. You can download the Qt5.15.2 by adding open source mirror site like https://mirrors.aliyun.com/qt/. (best for China users), if you are not in China mainland, you can find available links somewhere.

1. Download the official online downloader, or you can find it on [Qt Website](https://download.qt.io/official_releases/online_installers)
2. Open "Settings", or the icon.
3. Find the repositories tab, and add the following links to the Temporary repositories. If you are not a Mac user, you can find Linux or Windows versions by modifying the links (the do it yourself, I trust you!)

* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_src_doc_examples/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_wasm/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator/
* https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator_gui/

4. Follow the instructions, and complete.

### 2. Install Stanford C++ Library

You need to compile the Stanford C++ library with Qt5.15.2 compiler before starting the assignments. However, if you are not an internal student, you may not able to download the "CS106B.zip". Follow the instructions to install:

1. CLONE the stanford-cpp-library from [here](https://github.com/zelenski/stanford-cpp-library.git).
2. Reset the library to 5.15.2 version, because the newest repository if for Qt6. Again, the older Stanford C++ library can only be built by 5.15! By running the following command on your shell.

```
git reset --hard 7ffc69fff2307b5d607b300a4443755af1474cb2
```

3. Open the "BothLibraryWelcome.pro" inside the /stanford-cpp-library/ folder by simply double-clicking it, you will open the Qt Creator, and select the Qt5.15.2. Build it, the Stanford C++ library will be installed on your computer without error.

Finally, Good Luck!
