# Ingress-Field-Design

## 求解竹笋方案 - 使用方法

1.在 IITC 平台上将大量 Portal（数量上限 1000）收藏至 Bookmarks 内并导出至 portal.txt

2.编译 choose.cpp 并运行生成的 choose.exe，一段时间后选择你所需要的竹笋重数，每个方案的 bookmark 和 drawtool 信息将输出到 result.txt

## 求解单顶点多重 - 使用方法

1.将 portal-sample-mult.txt 导入到 IITC 平台中，将固定顶点收藏于 Bookmark 内的文件夹 A 中，将打算毒完重连的顶点集合收藏于文件夹 B 中，将大量中轴线候补 Portal 收藏至 Bookmark 内的默认文件夹（总 Portal 数上限 1000），最后导出至 portal.txt

2.编译 mult-choose.cpp 并运行生成的 mult-choose.exe，程序读取 Portal 信息结束后回车即可

3.方案位于 mult-result.txt 内
