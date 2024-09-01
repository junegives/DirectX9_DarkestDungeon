//#include "SoundComponent.h"
//#include"EngineUtil.h"
//
//FMOD::System* CSoundComponent::mSystem = nullptr;
//
//CSoundComponent::CSoundComponent()
//	: mbMute(false)
//	, mbLoop(false)
//	, mSound(nullptr)
//	, mChannel(nullptr)
//	, mPriority(128)
//	, mVolume(1.f)
//	, mPitch(1.f)
//	, mStereoPan(0.f)
//{
//}
//
//CSoundComponent::CSoundComponent(const std::wstring& fileName)
//	: mbMute(false)
//	, mbLoop(false)
//	, mSound(nullptr)
//	, mChannel(nullptr)
//	, mPriority(128)
//	, mVolume(1.f)
//	, mPitch(1.f)
//	, mStereoPan(0.f)
//{
//	LoadSoundFile(fileName);
//}
//
//CSoundComponent::~CSoundComponent()
//{
//	if (mSound)
//	{
//		mSound->release();
//	}
//}
//
////void CSoundComponent::OnEnable()
////{
////	Play();
////}
////
////void CSoundComponent::OnDisable()
////{
////	Stop();
////}
//
//bool CSoundComponent::LoadSoundFile(const wstring& fileName)
//{
//	mSound->release();
//	std::string finalFileName = Engine::WStringToString((L"../Resources/Sounds/") + fileName);
//	return mSystem->createSound(finalFileName.c_str(), FMOD_DEFAULT, nullptr, &mSound) == FMOD_OK;
//}
//
//void CSoundComponent::Play()
//{
//	if (!m_bEnable)
//	{
//		return;
//	}
//
//	if (mSound)
//	{
//		if (mChannel)
//		{
//			mChannel->stop();
//		}
//
//		mSystem->playSound(mSound, nullptr, true, &mChannel);
//		setupChannel(false);
//		mChannel->setPaused(false);
//	}
//}
//
//void CSoundComponent::PlayOneShot()
//{
//	if (!m_bEnable)
//	{
//		return;
//	}
//
//	if (mSound)
//	{
//		mSystem->playSound(mSound, nullptr, true, &mChannel);
//		setupChannel(true);
//		mChannel->setPaused(false);
//	}
//}
//
//void CSoundComponent::Pause()
//{
//	if (mChannel)
//	{
//		mChannel->setPaused(true);
//	}
//}
//
//void CSoundComponent::Stop()
//{
//	if (mChannel)
//	{
//		mChannel->stop();
//	}
//}
//
//void CSoundComponent::UnPause()
//{
//	if (mChannel)
//	{
//		mChannel->setPaused(false);
//	}
//}
//
//bool CSoundComponent::IsPlaying() const
//{
//	if (mChannel)
//	{
//		bool bPlaying;
//		mChannel->isPlaying(&bPlaying);
//		return bPlaying;
//	}
//
//	return false;
//}
//
//bool CSoundComponent::IsLoop() const
//{
//	return mbLoop;
//}
//
//int CSoundComponent::GetPriority() const
//{
//	return mPriority;
//}
//
//float CSoundComponent::GetVolume() const
//{
//	return mVolume;
//}
//
//float CSoundComponent::GetPitch() const
//{
//	return mPitch;
//}
//
//void CSoundComponent::SetPriority(const int priority)
//{
//	mPriority = CLAMP(priority, 0, 256);
//	if (mChannel)
//	{
//		mChannel->setPriority(mPriority);
//	}
//}
//
//void CSoundComponent::SetVolume(const float volume)
//{
//	mVolume = CLAMP(volume, 0.f, 1.f);
//	if (mChannel)
//	{
//		mChannel->setVolume(mVolume);
//	}
//}
//
//void CSoundComponent::SetPitch(const float pitch)
//{
//	mPitch = CLAMP(pitch, 0.f, 2.f);
//	if (mChannel)
//	{
//		mChannel->setPitch(mPitch);
//	}
//}
//
//void CSoundComponent::SetStereoPan(const float stereoPan)
//{
//	mStereoPan = CLAMP(stereoPan, -1.f, 1.f);
//	if (mChannel)
//	{
//		mChannel->setPan(mStereoPan);
//	}
//}
//
//void CSoundComponent::SetLoop(const bool bLoop)
//{
//	mbLoop = bLoop;
//	if (mChannel)
//	{
//		mChannel->setMode(mbLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
//	}
//}
//
//void CSoundComponent::SetMute(const bool bMute)
//{
//	mbMute = bMute;
//	if (mChannel)
//	{
//		mChannel->setMute(mbMute);
//	}
//}
//
//void CSoundComponent::setupChannel(bool bOneShot)
//{
//	if (!mChannel)
//	{
//		return;
//	}
//
//	mChannel->setPriority(mPriority);
//	mChannel->setVolume(mVolume);
//	mChannel->setPitch(mPitch);
//	mChannel->setPan(mStereoPan);
//	mChannel->setMute(mbMute);
//	if (bOneShot)
//	{
//		mChannel->setMode(FMOD_LOOP_OFF);
//	}
//	else
//	{
//		mChannel->setMode(mbLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
//	}
//}
//
//bool CSoundComponent::init()
//{
//	if (mSystem)
//	{
//		return false;
//	}
//
//	NULL_CHECK_RETURN(FMOD::System_Create(&mSystem), false);
//	NULL_CHECK_RETURN(mSystem->init(32, FMOD_INIT_NORMAL, nullptr), false);
//
//
//	return true;
//}
//
//void CSoundComponent::update()
//{
//	mSystem->update();
//}
//
//void CSoundComponent::release()
//{
//	if (mSystem)
//	{
//		mSystem->close();
//		mSystem->release();
//	}
//}
