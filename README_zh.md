cocos2dx-lite
=========



*NOTE*: 从  [tag v5](https://github.com/c0i/cocos2dx-lite/tree/v5) 开始，这个仓库只提供一个可运行的 lua 应用和一个重命名的工具。这样方便提供一个完整的程序，比如：

1. 登陆（微信、微博等）
2. 分享（微信、微博等）
3. 支付（微信、苹果支付等）
4. 广告 sdk（[AdMob](https://www.google.com/admob/)、 [Chartboost](https://www.chartboost.com/)、 [UnityAds](https://unityads.unity3d.com/admin/)）
5. 缺陷追踪工具 [bugly](https://bugly.qq.com/v2/) 等工具。
6. 统计分析 （[flurry](https://y.flurry.com/)、[umeng](www.umeng.com)）
7. 使用 FMOD 替换掉自带的两个音频库
8. 魔窗
9. 3D搓牌效果层
   ![图](https://static.oschina.net/uploads/img/201709/27165036_kC4x.png)

去掉 3d 等功能不是因为要裁剪包的大小，是因为少代码，少运行点代码，程序稳定点。
提供这么多功能是有代价的，就是仓库很大。


## 使用工具

  1. [pidcat](https://github.com/JakeWharton/pidcat) 更方便的 android 日志
  2. [sprite-sheet-packer](https://github.com/amakaseev/sprite-sheet-packer) 图集打包
  3. [ImageOptim](https://github.com/ImageOptim/ImageOptim) 图片优化
  4. [ZeroBrane Studio](https://studio.zerobrane.com/) ZeroBrane Studio is a lightweight Lua IDE
      > 配置单步调试: 首选项里设置  editor.autoactivate = true.
      > ​
      > **重启，重启，重启 ZBS IDE 才会生效**
