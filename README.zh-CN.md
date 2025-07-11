# CS106B 2021

[![en](https://img.shields.io/badge/lang-en-blue.svg)](https://github.com/AnthonyYsw/CS106B_2021_raw/blob/main/README.md)
[![zh-CN](https://img.shields.io/badge/lang-中-red.svg)](https://github.com/AnthonyYsw/CS106B_2021_raw/blob/main/README.zh-CN.md)



这里收集了 Stanford CS106B 2021 的所有资源。这是自学者最佳的 CS106B 版本，因为它拥有目前最完整的免费在线资源。资源下载自 [Stanford CS106B 网站](https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1214/)。

本仓库仅供学习交流使用。

斯坦福大学提供了**世界上最好的 C++ 课程**，而课程中的作业正是其精华部分，一定要完成这些作业！

## 在开始作业前必须完成的两步...

### 1. 下载 Qt 5.15.2 和 Qt Creator GUI（详细步骤见 /Resources/Qt_Creator/）

遗憾的是，Qt 5.15.2 已无法通过官方在线安装器下载。你可以通过添加开源镜像（如 [阿里云 Qt 镜像](https://mirrors.aliyun.com/qt/)——中国大陆用户推荐）下载安装 Qt 5.15.2。如果你在中国大陆以外，也可以寻找其他可用的下载链接。

步骤如下：

1. 下载官方在线安装器，可在 [Qt 官网](https://download.qt.io/official_releases/online_installers) 获取。
2. 打开“设置”（齿轮图标）。
3. 进入“Repositories”标签页，添加如下临时仓库链接。如果你不是 Mac 用户，请根据自己的系统（Linux 或 Windows）调整链接（你肯定可以搞定的！）。

    * https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152/
    * https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_src_doc_examples/
    * https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/qt5_5152_wasm/
    * https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator/
    * https://mirrors.aliyun.com/qt/online/qtsdkrepository/mac_x64/desktop/tools_qtcreator_gui/

4. 按照提示完成安装。

### 2. 安装 Stanford C++ 库

你需要用 Qt 5.15.2 编译 Stanford C++ 库后才能开始作业。如果你不是斯坦福内部学生，可能无法下载 "CS106B.zip"。请按如下步骤操作：

1. **克隆** [stanford-cpp-library 仓库](https://github.com/zelenski/stanford-cpp-library.git)。
2. **回退**到 5.15.2 版本，因为最新版仓库适用于 Qt6。老版本 Stanford C++ 库只能用 Qt 5.15 编译！在终端执行以下命令：

    ```
    git reset --hard 7ffc69fff2307b5d607b300a4443755af1474cb2
    ```

3. 在 `stanford-cpp-library` 文件夹下，双击打开 "BothLibraryWelcome.pro"。这会自动用 Qt Creator 打开。选择 Qt 5.15.2 并编译项目，Stanford C++ 库即可无误安装到你的电脑上。

祝你好运！

