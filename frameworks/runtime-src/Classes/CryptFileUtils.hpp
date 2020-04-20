#pragma once

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/CCFileUtils-android.h"
typedef cocos2d::FileUtilsAndroid FlatformFileUtils;

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include "platform/apple/CCFileUtils-apple.h"
typedef cocos2d::FileUtilsApple FlatformFileUtils;

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "platform/win32/CCFileUtils-win32.h"
typedef cocos2d::FileUtilsWin32 FlatformFileUtils;

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#include "platform/winrt/CCFileUtilsWinRT.h"
typedef cocos2d::CCFileUtilsWinRT FlatformFileUtils;


#elif (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#include "platform/linux/CCFileUtils-linux.h"
typedef cocos2d::FileUtilsLinux FlatformFileUtils;

#else
#error "Filesystem not supported on this platform"
#endif


#include "Crypt.hpp"

class CryptFileUtils: public FlatformFileUtils {
    Crypt* crypt_;
public:
    CryptFileUtils(Crypt* crypt);
    virtual ~CryptFileUtils() { delete crypt_; }
protected:
    virtual std::string getStringFromFile(const std::string& filename) const override;
    virtual cocos2d::Data getDataFromFile(const std::string& filename) const override;
    virtual unsigned char* getFileData(const std::string& filename, const char* mode, ssize_t *size) const override;
    virtual unsigned char* getFileDataFromZip(const std::string& zipFilePath, const std::string& filename, ssize_t *size) const override;
    virtual bool writeStringToFile(const std::string& str, const std::string& fullPath) const override;
    virtual bool writeDataToFile(const cocos2d::Data& data, const std::string& fullPath) const override;
    virtual void purgeCachedEntries() override;
private:
    cocos2d::Data encrypt(const cocos2d::Data& data) const;
    cocos2d::Data decrypt(const cocos2d::Data& data) const;
    cocos2d::Data getDataFromZip(const std::string& zipFilePath, const std::string& filename) const;
};
