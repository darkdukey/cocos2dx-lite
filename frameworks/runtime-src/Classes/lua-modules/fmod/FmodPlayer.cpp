#include "FmodPlayer.h"

using namespace cocos2d;

FmodPlayer* FmodPlayer::_instance = nullptr;

FmodPlayer::FmodPlayer()
    :   _system(nullptr),
        _backgroudSound(nullptr),
        _effectSound(nullptr),
        _backgroundChannel(nullptr),
        _mastergroup(nullptr),
        _result(FMOD_RESULT::FMOD_OK),
        _version(0),
        _extradriverdata(nullptr),
        _effectVolume(1.0f),
        _backgroudVolume(1.0f),
        _isBackgroundMusicPaused(false),
        _isEffectPaused(false)
{
}

FmodPlayer::~FmodPlayer()
{
    _result = _system->close();
    _result = _system->release();

}

bool FmodPlayer::init()
{
    _result = FMOD::System_Create(&_system);
    _result = _system->getVersion(&_version);
    _result = _system->setDSPBufferSize(2048, 2);
    _result = _system->setOutput(FMOD_OUTPUTTYPE::FMOD_OUTPUTTYPE_AUTODETECT);
    _result = _system->init(32, FMOD_INIT_NORMAL, _extradriverdata);
    _result = _system->getMasterChannelGroup(&_mastergroup);

    Director::getInstance()->getScheduler()->schedule(schedule_selector(FmodPlayer::update), this, 0, false);
    Director::getInstance()->getScheduler()->schedule(schedule_selector(FmodPlayer::memgc), this, 0, false);

    return true;
}

FmodPlayer* FmodPlayer::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = FmodPlayer::create();
        _instance->retain();
    }
    return _instance;
}

void FmodPlayer::playBackgroundMusic(const std::string& filename, bool loop)
{
    if (this->isBackgroundMusicPlaying() == false)
    {
        auto fullpath = FileUtils::getInstance()->fullPathForFilename(filename);
        if (_buffers.find(fullpath) != _buffers.end()) {
            _playBackgroundMusic(_buffers[fullpath], loop);
        } else {
            FileUtils::getInstance()->getDataFromFile(filename, [this,loop,fullpath](Data data) {
                _buffers[fullpath] = data;
                _playBackgroundMusic(data, loop);
            });
        }
    }
}

void FmodPlayer::playEffect(const std::string& filename, bool loop)
{
    auto fullpath = FileUtils::getInstance()->fullPathForFilename(filename);
    if (_buffers.find(fullpath) != _buffers.end()) {
        _playEffect(_buffers[fullpath], loop);
    } else {
        FileUtils::getInstance()->getDataFromFile(filename, [this,loop,fullpath](Data data) {
            _buffers[fullpath] = data;
            _playEffect(data, loop);
        });
    }
}

void FmodPlayer::stopBackgroundMusic()
{
    if (_backgroundChannel)
    {
        _backgroudSound->release();
        _backgroudSound = nullptr;
        _backgroundChannel->stop();
        _backgroundChannel = nullptr;
    }
}

bool FmodPlayer::isBackgroundMusicPlaying()
{
    bool playing = false;
    _backgroundChannel->isPlaying(&playing);
    return playing;
}

float FmodPlayer::getBackgroundMusicVolume()
{
    float volume = 1.0;
    _backgroundChannel->getVolume(&volume);
    return volume;
}

void FmodPlayer::setBackgroundMusicVolume(float volume)
{
    _backgroundChannel->setVolume(volume);
    _backgroudVolume = volume;
}

void FmodPlayer::pauseBackgroundMusic()
{
    _backgroundChannel->setPaused(true);
    _isBackgroundMusicPaused = true;
}

void FmodPlayer::resumeBackgroundMusic()
{
    _backgroundChannel->setPaused(false);
    _isBackgroundMusicPaused = false;
}

void FmodPlayer::update(float delta)
{
    _system->update();
    _backgroundChannel->setVolume(_backgroudVolume);
    _backgroundChannel->setPaused(_isBackgroundMusicPaused);
    for (auto iter = _effect.begin(); iter != _effect.end(); iter++)
    {
        FMOD::Channel* channel = static_cast<FMOD::Channel*>(iter->second);
        channel->setVolume(_effectVolume);
        channel->setPaused(_isEffectPaused);
    }
}

void FmodPlayer::memgc(float delta)
{
    auto iter = _effect.begin();
    while (iter != _effect.end())
    {
        bool playing = false;
        FMOD::Sound* sound = static_cast<FMOD::Sound*>(iter->first);
        FMOD::Channel* channel = static_cast<FMOD::Channel*>(iter->second);
        channel->isPlaying(&playing);
        if (playing == false)
        {
            sound->release();
            _effect.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }

    int currentalloced = 0;
    int maxalloced = 0;
    FMOD::Memory_GetStats(&currentalloced, &maxalloced);

    int channelsplaying = 0;
    _system->getChannelsPlaying(&channelsplaying, NULL);
}

void FmodPlayer::stopAllEffects()
{
    for (auto iter = _effect.begin(); iter != _effect.end(); iter++)
    {
        FMOD::Sound* sound = static_cast<FMOD::Sound*>(iter->first);
        FMOD::Channel* channel = static_cast<FMOD::Channel*>(iter->second);
        sound->release();
        channel->stop();
    }
    _effect.clear();
}

float FmodPlayer::getEffectsVolume()
{
    return _effectVolume;
}

void FmodPlayer::setEffectsVolume(float volume)
{
    _effectVolume = volume;
    for (auto iter = _effect.begin(); iter != _effect.end(); iter++)
    {
        FMOD::Channel* channel = static_cast<FMOD::Channel*>(iter->second);
        channel->setVolume(volume);
    }
}

// private

void FmodPlayer::_playBackgroundMusic(const cocos2d::Data& data, bool loop)
{
    FMOD_CREATESOUNDEXINFO  exinfo = { 0 };
    
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    
    exinfo.length = data.getSize();
    
    FMOD_MODE mode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE;
    if (loop)
    {
        mode |= FMOD_LOOP_NORMAL;
    }
    else
    {
        mode |= FMOD_LOOP_OFF;
    }
    
    _result = _system->createSound((char*)data.getBytes(), mode, &exinfo, &_backgroudSound);
    _result = _system->playSound(_backgroudSound, 0, false, &_backgroundChannel);
    _backgroundChannel->setVolume(_backgroudVolume);
    
    _system->update();
}

void FmodPlayer::_playEffect(const cocos2d::Data& data, bool loop)
{
    FMOD_CREATESOUNDEXINFO  exinfo = { 0 };
    
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    
    exinfo.length = data.getSize();
    
    FMOD_MODE mode = FMOD_OPENMEMORY | FMOD_CREATESAMPLE;
    if (loop)
    {
        mode |= FMOD_LOOP_NORMAL;
    }
    else
    {
        mode |= FMOD_LOOP_OFF;
    }
    
    FMOD::Sound * sound = nullptr;
    FMOD::Channel * channel =nullptr;
    
    _result = _system->createSound((char*)data.getBytes(), mode, &exinfo, &sound);
    _result = _system->playSound(sound, 0, false, &channel);
    
    _effect.insert(std::make_pair(sound, channel));
    channel->setVolume(_effectVolume);
    
    _system->update();
    
    int channelsplaying = 0;
    _system->getChannelsPlaying(&channelsplaying, NULL);
}

