# LibMatch

- Cur Version 0.9.6
- Last Updated: 2024-03-03

版本更新日志：

1. 重构特征匹配算法，提高匹配精度（尤其是文本识别）。支持返回置信度。
2. 修复模板匹配置信度 INFINITY 的问题
3. 移除截图模块，减小库的体积
4. 移除YoLo模块，减小库的体积
5. 支持Linux平台（仅在Debian 12.5下测试过）
6. 修复了一些小问题

BUG交流群：767178345

- [QUICK START](usage.md)

## Effect Demo

![OCR](./assets/4.png)
![ORB](./assets/5.png)
![Template](./assets/6.png)

## Description

LibMatch is a library for template match , OCR , orb feature match and screenshot.

LibMatch 是一个用于模板匹配，文本识别，orb特征匹配和截图的库。

LibMatch has extremely tiny small size, and it is easy to use. It is suitable for use in various scenarios, such as game automation, image recognition, and so on.

LibMatch 体积极小，易于使用。适用于各种场景，如游戏自动化，图像识别等。

LibMatch is written in C++ and provides a C interface. It can be used in C++ and other languages that support C API.

LibMatch 用C++编写，并提供C接口。可以在C++和其他支持C API的语言中使用。

## Features

- Template Match (模板匹配)
- OCR and can locate single characters (OCR 并且可以单字定位)
- ORB Feature Match (ORB特征匹配)
- C API (C接口)
- Elanguage API (易语言调用)

## Todo List

- [ ] Add interface for C++ (添加C++接口)
- [x] Add interface for Python(Partial) (添加Python接口，部分) 
- [x] Add interface for C#(Partial) (添加C#接口，部分)
- [x] Add interface for Java (添加Java接口，部分)
- [ ] Add interface for 按键精灵 (添加按键精灵接口)
- [ ] Add interface for 火山开发平台 (添加火山开发平台接口)
- [x] Support Linux (支持Linux)
- [ ] Support Android (支持Android)
- [ ] Decrease the size of the library (减小库的体积)
- [ ] Add more examples (添加更多的例子)
- [ ] Add more documents (添加更多的文档)
- [ ] Import the Algorithm of CAPTCHA (加入验证码算法)
- [ ] Accurate castration ncnn (精确阉割ncnn)
- [ ] Improve accurate of OCR (提高OCR的准确性)
- [ ] Support multi-object orb match (支持多目标orb匹配)

## Support Platform

- Windows 32/64
- Debian 12.5

# Star History

[![Star History Chart](https://api.star-history.com/svg?repos=futz12/libmatch&type=Date)](https://star-history.com/#futz12/libmatch&Date)

