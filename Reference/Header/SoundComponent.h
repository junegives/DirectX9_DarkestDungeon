//#pragma once
//
//#include<fmod.hpp>
//#include"Component.h"
//#include"Engine_Define.h"
//
//BEGIN(Engine)
//
//class ENGINE_DLL CSoundComponent :public CComponent
//{
//public:
//	CSoundComponent();
//	CSoundComponent(const wstring& fileName);
//	~CSoundComponent();
//
//public:
//
//	bool LoadSoundFile(const wstring& fileName);
//	void Play();
//	void PlayOneShot();
//	void Pause();
//	void Stop();
//	void UnPause();
//
//	bool IsPlaying() const;
//	bool IsLoop() const;
//
//	int GetPriority() const;
//	float GetVolume() const;
//	float GetPitch() const;
//
//	void SetPriority(const int priority);
//	void SetVolume(const float volume);
//	void SetPitch(const float pitch);
//	void SetStereoPan(const float stereoPan);
//	void SetLoop(const bool bLoop);
//	void SetMute(const bool bMute);
//
//private:
//	static bool init();
//	static void update();
//	static void release();
//
//	void setupChannel(bool bOneShot);
//
//private:
//	static FMOD::System* mSystem;
//
//	FMOD::Sound* mSound;
//	FMOD::Channel* mChannel;
//
//	bool mbMute;
//	bool mbLoop;
//	int mPriority;
//	float mVolume;
//	float mPitch;
//	float mStereoPan;
//
//};
//
//END
