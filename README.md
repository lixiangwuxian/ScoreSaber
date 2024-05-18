## Building

### 环境配置

 1. 下载[qpm.cli](https://github.com/QuestPackageManager/QPM.CLI)
 1. shell输入

    ```
    qpm doctor
    ```

    检查环境
 1. 按照输出安装缺失依赖
 1. shell输入
    ```
    qpm restore
    ```
    获取需要的其它mod依赖

注：

编译的时候需要使用`libil2cpp`的`0.2.5`版本。对应的仓库地址是 <https://il2cpp.thnght.pro/2019/0_2_5> ，服务器疑似出现故障并无人修复，导致缓存构建失败。

如果出现报错可以下载0.2.3版本替换缓存。执行一遍 `qpm restore` 并失败后，下载 [libil2cpp 0.2.3版本](https://drive.usercontent.google.com/download?id=1TAeF5-sazUdXCeC9Z8ggs4UkWDc61wsD&export=download&authuser=0)，将文件内容解压到`%USERPROFILE%\AppData\Roaming\QPM-RS\cache\libil2cpp\0.2.5\src`即可

### 编译

```powershell
./build.ps1 -clean -release
```

### 打包

```powershell
./createqmod.ps1
```

---

# ScoreSaber Quest

ScoreSaber implementation for Quest.

Currently should be almost 1:1 to the PC version of ScoreSaber, though some features are missing / different which can be seen [here](https://github.com/ScoreSaber/ScoreSaber-Quest/issues?q=is%3Aissue+is%3Aopen+label%3Afeature-parity)

If you are a user looking to install ScoreSaber on their Quest, please do not use the `qmod` in the Releases section of this page, this will not work as it is missing the generated authentication code needed for ScoreSaber to work. **Instead please [click here](https://scoresaber.com/quest) for a guided installation process.**

## Credits

-  [RedBrumbler](https://github.com/RedBrumbler), [Optimus](OptimusChen) - UI Implementation
-  [Qwasyx](https://github.com/Qwasyx) - Maintainer
-  [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
-  [raftario](https://github.com/raftario)
-  [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)

