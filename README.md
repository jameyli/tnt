tnt : library from TNT
======================

之前写的一些代码，不忍丢弃，故加以整理并完善,
同时尝试更多地考虑代码如何复用, 制造一些好用的轮子。
希望会有用。

原则
----

    保持独立,  不能依赖任何未开源的代码库
    std
    boost (尽量避免)
    c++11 (?)

目录
-----

* 进程框架

    application_base

* 协议处理框架

    transaction

* 配置

    python/xls_deploy_tool.py

* 定时器
* 协议
* 通信

如何编译？
=========

    scons
    使用scons 主要是觉得简单


