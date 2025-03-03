#pragma once
#include <afxwin.h>
#include <afxmt.h>
class CChatThreadDbWorker :
    public CWinThread
{
    DECLARE_DYNCREATE(CChatThreadDbWorker)
public:

    static CChatThreadDbWorker* GetInstance();
    
    virtual ~CChatThreadDbWorker();

    void SetData(int id, HWND hWnd);

    int GetID() { return m_id; };

    void SetMainWnd(HWND hWnd) { m_hWnd = hWnd; };

    virtual BOOL InitInstance();

    virtual int ExitInstance();

    void StopThread();

protected:

    static UINT ThreadProc(LPVOID pParam);

    void RunChat();

private:

    CChatThreadDbWorker();

    CMutex m_mutex; // Мутекс для синхронизации

    int m_id;

    BOOL m_bStop; // Флаг завершения потока

    HWND m_hWnd;

    CString queryDataBase(int id);
};

