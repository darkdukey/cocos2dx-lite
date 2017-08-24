
cocos2dx-lite
=========

[中文](https://github.com/c0i/cocos2dx-lite/blob/master/README_zh.md)

*NOTE* : From [tag v5](https://github.com/c0i/cocos2dx-lite/tree/v5), this repo remove all *templates*, just provide a runable lua app and a tool for rename the app.

It bases on [cocos2d-x](https://github.com/cocos2d/cocos2d-x) v3 branch, but remove 3D and other features.
It works on iOS, Android, OS X and Windows.

The major change:

- Only support iOS,OS X,Android and Windows.
- Remove 3D features
- Remove support for JS script
- Remove deprecated classes and functions
- Remove Camera
- Remove Physics integration
- Remove C++ implementations of Component
- ~~Remove C++ implementations of CocoStudio parser~~
- Remove C++ implementations of CocosBuilder parser
- Remove AssetsManager, AssetsManagerEX
- Remove support for WebP,S3TC,ATITC
- ~~Remove FastTileMap, TileMap~~
- Remove *Menu*
- Remove Audio
- Improve module compile `cocos/base/ccConfigUser.h`
- Improve curl library, only support `socket` and `http` protocol
- Add [CC_MINI_2D / CC_USE_FREETYPE / CC_ONLY_PNG](https://github.com/c0i/cocos2dx-lite/blob/master/cocos/base/ccConfigUser.h)
- Add CC_USE_SPINE
- Add CC_USE_AUDIO_ENGINE
- Add CC_USE_NETWORK
- Add [FMOD](http://www.fmod.com/)
- Restore ui module (NOT depend CocoStudio)
- Restore `Focus` event (need by `ui`)
- Restore `CocoStudio` module
- Keep working with `CocosStudio` after 3.14.1 ([https://github.com/cocos2d/cocos2d-x/pull/17349#issuecomment-279629919](https://github.com/cocos2d/cocos2d-x/pull/17349#issuecomment-279629919))


## Usefull tools

    1. [pidcat](https://github.com/JakeWharton/pidcat) Colored logcat script which only shows log entries for a specific application package.
    2. [sprite-sheet-packer](https://github.com/amakaseev/sprite-sheet-packer) Sprite sheet packer
    3. [ImageOptim](https://github.com/ImageOptim/ImageOptim) GUI image optimizer for Mac

## How to start

1. setup env

    ```
    $ # clone repo
    $ git clone https://github.com/c0i/cocos2dx-lite.git
    $ cd cocos2dx-lite
    $ ./setup.py
    $ source ~/.bash_profile
    ```

2. try with player project

    ```
    $ open player/proj.ios_mac/player.xcodeproj/
    $ or
    $ cocos run -p mac # ios, android
    ```

## Contributing to the Project

Cocos2dx-lite is licensed under the [MIT License](https://opensource.org/licenses/MIT). We welcome participation!

## Others

There are lots of better cocos2d-x forks:

- https://github.com/stubma/cocos2dx-classical (v2)

- https://github.com/IppClub/Dorothy (v2)

- https://github.com/Sheado/cocos2d-x (v3)

- https://github.com/mafiagame/quick-cocos2d-x (v3)

- https://github.com/newnon/avalon

  > Productivity boost for cocos2d-x developers.
  >
  > Avalon provides various extension, helpers, libraries, classes, templates & functions on top of [cocos2d-x](http://cocos2d-x.org/). It should speed up cross-platform development and automate annoying tasks.
