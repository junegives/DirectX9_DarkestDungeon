#include "pch.h"
#include "Video.h"
#include "GameMgr.h"

CVideo::CVideo(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
	, m_eVideoID(VIDEOID::ENUM_END)
{
}

CVideo::~CVideo()
{
}

HRESULT CVideo::Update(const _float& fTimeDelta)
{
	CGameMgr::GetInstance()->SetGameState(EGameState::VIDEO);

	// 현재 위치 가져오기
	m_pSeek->GetCurrentPosition(&rtCurrent);

	return S_OK;
}

HRESULT CVideo::Ready_Video(VIDEOID eVideo)
{
	m_eVideoID = eVideo;

	switch (eVideo)
	{
	case CVideo::VIDEOID::VIDEO_START:
		PlayVideo(L"../Bin/Resource/Video/old_road.wmv", g_hWnd);
		break;
	case CVideo::VIDEOID::VIDEO_ENDING:
		PlayVideo(L"../Bin/Resource/Video/epilog_long.wmv", g_hWnd);
		break;
	default:
		break;
	}

	return S_OK;
}

void CVideo::PlayVideo(const wchar_t* videoFilename, HWND hwnd, _bool _bChange) {

	CoInitialize(NULL);

	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraph);
	m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pControl);
	m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pEvent);
	m_pGraph->QueryInterface(IID_IMediaSeeking, (void**)&m_pSeek);

	// Add the source filter to the graph
	CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pSource);
	m_pGraph->AddFilter(m_pSource, L"Source");
	// Load the video file
	m_pSource->QueryInterface(IID_IFileSourceFilter, (void**)&m_pFileSourceFilter);
	m_pFileSourceFilter->Load(videoFilename, NULL);

	// Render the file
	m_pGraph->RenderFile(videoFilename, NULL);

	// Set the video window
	m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVidWin);
	m_pVidWin->put_Owner((OAHWND)g_hWnd);
	m_pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	m_pVidWin->put_MessageDrain((OAHWND)g_hWnd);
	m_pVidWin->put_Left(0);
	m_pVidWin->put_Top(0);
	m_pVidWin->put_Width(1280); // Adjust to your window's width
	m_pVidWin->put_Height(720); // Adjust to your window's height
	m_pVidWin->put_Visible(OATRUE);

	// Run the graph
	HRESULT hr = m_pControl->Run();

	if (_bChange)
	{
		REFERENCE_TIME rtStart = rtCurrent;
		m_pSeek->SetPositions(&rtStart, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	}

	DWORD result = WaitForSingleObject(m_pEvent, 100);

	// 현재 위치 가져오기
	m_pSeek->GetCurrentPosition(&rtCurrent);

	//long evCode;
	//m_pEvent->WaitForCompletion(INFINITE, &evCode);

	// Cleanup
	//pVidWin->put_Visible(OAFALSE);  // Hide the video window
	//m_pControl->Release();
	//pEvent->Release();
	//pVidWin->Release();
	//pGraph->Release();
	//pFileSourceFilter->Release();

	//CoUninitialize();
}

void CVideo::ChangeVideo()
{
	if (!m_bChanged)
	{
		CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraphChange);
		m_pGraphChange->QueryInterface(IID_IMediaControl, (void**)&m_pControlChange);
		m_pGraphChange->QueryInterface(IID_IMediaEventEx, (void**)&m_pEventChange);
		m_pGraphChange->QueryInterface(IID_IMediaSeeking, (void**)&m_pSeekChange);

		// Add the source filter to the graph
		CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&m_pSourceChange);
		m_pGraphChange->AddFilter(m_pSourceChange, L"SourceChange");
		// Load the video file
		m_pSourceChange->QueryInterface(IID_IFileSourceFilter, (void**)&m_pFileSourceFilterChange);
		m_pFileSourceFilterChange->Load(L"../Bin/Resource/Video/old_road_loading_complete.wmv", NULL);

		// Render the file
		m_pGraphChange->RenderFile(L"../Bin/Resource/Video/old_road_loading_complete.wmv", NULL);

		// Set the video window
		m_pGraphChange->QueryInterface(IID_IVideoWindow, (void**)&m_pVidWinChange);
		m_pVidWinChange->put_Owner((OAHWND)g_hWnd);
		m_pVidWinChange->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
		m_pVidWinChange->put_MessageDrain((OAHWND)g_hWnd);
		m_pVidWinChange->put_Left(0);
		m_pVidWinChange->put_Top(0);
		m_pVidWinChange->put_Width(1280); // Adjust to your window's width
		m_pVidWinChange->put_Height(720); // Adjust to your window's height
		m_pVidWinChange->put_Visible(OATRUE);

		// Run the graph
		HRESULT hr = m_pControlChange->Run();

		REFERENCE_TIME rtStart = rtCurrent += 2000000;
		m_pSeekChange->SetPositions(&rtStart, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);

		DWORD result = WaitForSingleObject(m_pEventChange, 100);

		m_pControl->Stop();
		m_pVidWin->put_Visible(OAFALSE);  // Hide the video window
		m_pControl->Release();
		m_pEvent->Release();
		m_pVidWin->Release();
		m_pGraph->Release();
		m_pFileSourceFilter->Release();

		//CoUninitialize();

		m_bChanged = true;

		//PlayVideo(L"../Bin/Resource/Video/old_road_loading_complete.wmv", g_hWnd, true);
	}
}

void CVideo::StopVideo()
{
	CoUninitialize();
	if (m_pControlChange)
	{
		m_pControlChange->Stop();
		m_pVidWinChange->put_Visible(OAFALSE);  // Hide the video window
		m_pControlChange->Release();
		m_pEventChange->Release();
		m_pVidWinChange->Release();
		m_pGraphChange->Release();
		m_pFileSourceFilterChange->Release();
	}
	CGameMgr::GetInstance()->SetGameState(EGameState::PRCESS);
}
