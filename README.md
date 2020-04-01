# Ingress-Field-Design

## How to compiler?

```bash
mkdir build
cd build
cmake ..
make
make install
```

In Windows, you need to convert `.cpp` and `.txt` from UTF-8 to GBK.

<!--
### Linux
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

### Windows
```bash
(linux)
find ./src -type f -name "*.txt" -exec enca -L none -x GBK {} \;
find ./src -type f -name "*.cpp" -exec enca -L none -x GBK {} \;
(win)
mkdir buildwin
cd buildwin
cmake -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" ..
make
make install
(linux)
find ./src -type f -name "*.txt" -exec enca -L zh_CN -x UTF-8 {} \;
find ./src -type f -name "*.cpp" -exec enca -L zh_CN -x UTF-8 {} \;
```
-->

## How to use it?

[English](https://github.com/Konano/Ingress-Field-Design/wiki/Instructions-for-use)

[中文](https://github.com/Konano/Ingress-Field-Design/wiki/%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)

## Refactoring……

- [x] BAF
- [x] HCF
- [ ] HCF-MoreKeys
- [ ] HCF-RESWUEV1
- [ ] HCF-SingleWork
- [x] MultField
- [ ] TriField