# Ingress-Field-Design

## 求解竹笋方案及规划

0. 首先你得会使用 IITC 以及 drawtool & bookmark 插件
1. 在 IITC 平台上将大量 Portal（数量上限 1000）收藏至 Bookmarks 内（可使用  Portal List 插件或 [这个插件](https://github.com/hayeswise/iitc-portalsinpolygons) 节省时间）

[注]：后一个插件目前不能准确数白po，要按照[这里](https://github.com/hayeswise/iitc-portalsinpolygons/pull/12)来再改一下

2. 下载 [这个东西](https://github.com/Konano/Ingress-Field-Design/raw/master/mille-feuille_field.zip) 并解压
3. 将 bookmark 信息导出至 `portal.txt`
4. 运行 `choose.exe`（Mac 用户可运行 `choose` Linux 用户自行编译`choose.cpp`即可），程序读取 Portal 信息后显示倒数第二个 Portal 的名称，此时回车
5. 待计算完成后输入你所需要的竹笋重数并回车
6. 待程序退出后即可在 `result.txt` 找到有关方案的 bookmark 和 drawtool 信息

## 单人竹笋路径规划

0. 首先你已经使用了上面提供的竹笋规划程序
1. 下载 [这个东西](https://github.com/Konano/Ingress-Field-Design/raw/master/act.zip) 并解压到同个目录下
2. 复制你所选定的方案的 bookmark 信息，并粘贴于 `bookmark.txt` 内
3. 用 draw 工具在 IITC 上画出一条路径，要求路径必须经过方案内全部点正好一次
4. 将 drawtool 信息导出到 `way.txt`（可参照 `way-sample.txt`）
5. 运行 `act.exe`
6. 打开 `act.txt`，看文件末尾是否有错误提示，有的话修改路径并回到步骤 4，直到无错误提示
7. 把 `act.txt` 复制到 excel 表格内，完成

## 求解单顶点多重方案及规划

0. 首先你得会使用 IITC 以及 drawtool & bookmark 插件
1. 下载 [这个东西](https://github.com/Konano/Ingress-Field-Design/raw/master/mult_field.zip) 并解压
2. 将 `mult-portal-sample.txt` 导入到 IITC 平台中
3. 将固定顶点收藏于 Bookmark 内的文件夹 A 中，将打算毒完重连的顶点集合收藏于文件夹 B 中，将大量中轴线候补 Portal 收藏至 Bookmark 内的默认文件夹（总 Portal 数上限 1000）
4. 将 bookmark 信息导出至 `portal.txt`
5. 运行 `mult-choose.exe`，程序读取 Portal 信息后显示倒数第二个 Portal 的名称，此时回车
6. 待程序退出后即可在 `mult-result.txt` 找到有关方案的 bookmark 和 drawtool 信息

## 尚未填的坑

1. Mac & Linux 版
2. “一键收藏”插件的编写