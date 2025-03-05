#include "stdafx.h"
#include "CChatThreadDbWorker.h"
#include "CRestApiClient.h"

#define WM_UPDATE_RICHEDIT_CHAT (WM_USER + 888)

IMPLEMENT_DYNCREATE(CChatThreadDbWorker, CWinThread)

static CChatThreadDbWorker* pThreadInstance = nullptr;

CChatThreadDbWorker::CChatThreadDbWorker() : m_id(0), m_bStop(FALSE), m_hWnd(NULL)
{

}

CChatThreadDbWorker* CChatThreadDbWorker::GetInstance()
{
	if (pThreadInstance && pThreadInstance->m_hThread)
		return pThreadInstance;

	pThreadInstance = (CChatThreadDbWorker*)AfxBeginThread(RUNTIME_CLASS(CChatThreadDbWorker),
		THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	return pThreadInstance;
}

CChatThreadDbWorker::~CChatThreadDbWorker()
{
}

void CChatThreadDbWorker::SetData(int id, HWND hWnd)
{
	CSingleLock lock(&m_mutex, TRUE); // Áëîêèðóåò âûçîâ ôóíêöèè äî å¸ çàâåðøåíèÿ
	m_id = id;
	m_hWnd = hWnd;
}

BOOL CChatThreadDbWorker::InitInstance()
{
	RunChat();

	return TRUE;
}

int CChatThreadDbWorker::ExitInstance()
{
	return CWinThread::ExitInstance();
}

UINT CChatThreadDbWorker::ThreadProc(LPVOID pParam)
{
	CChatThreadDbWorker* pThread = (CChatThreadDbWorker*)pParam;

	if (pThread)
	{
		pThread->RunChat();
	}
	
	return 0;
}

void CChatThreadDbWorker::RunChat()
{
	while (!m_bStop)
	{
		if (m_bStop)
		{
			AfxEndThread(0);
			break;
		}

		m_mutex.Lock();
		HWND hWnd = m_hWnd; // Áåðåì àêòóàëüíûé HWND
		m_mutex.Unlock();

		if (hWnd)
		{
			CString* pStr = new CString;
			pStr->Format(_T("%s"), queryDataBase(m_id));
			PostMessage(hWnd, WM_UPDATE_RICHEDIT_CHAT, 0, (LPARAM)pStr);
		}

		Sleep(5000); // 5sec
	}
}

void CChatThreadDbWorker::StopThread()
{
	if (m_hThread == NULL)
	{
		return;
	}

	m_bStop = TRUE;
}

CString CChatThreadDbWorker::queryDataBase(int id)
{
	CStringW responseUtf8;
	CString query;
	query.Format(_T("/claimautoinsurance/getMessage/%d"), id);

	CRestApiClient restApi("http://auto-dev-ins:8010");

	rapidjson::Document json = restApi.DoRequest(query, REQUEST_METHOD::METHOD_GET);

	if (!json.HasParseError() && json.IsObject())
	{
		int byteLeght = MultiByteToWideChar(CP_UTF8, 0, json["message"].GetString(), -1, nullptr, 0);
		MultiByteToWideChar(CP_UTF8, 0,
			json["message"].GetString(), -1, responseUtf8.GetBuffer(byteLeght), byteLeght);
		responseUtf8.ReleaseBuffer();
	}

	return responseUtf8;
}
