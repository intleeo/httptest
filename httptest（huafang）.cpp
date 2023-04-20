#include <iostream>
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib") // 需要链接 wininet 库
#define IDI_ICON1 101
void SetConsoleWindowSize(SHORT width, SHORT height)//设置控制台窗体大小
{
    HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT wrt = { 0, 0, width - 1, height - 1 };
    SetConsoleWindowInfo(hStdOutput, TRUE, &wrt); // 设置窗体尺寸
    COORD coord = { width, height };
    SetConsoleScreenBufferSize(hStdOutput, coord); // 设置缓冲尺寸
}

int main() {
    SetConsoleWindowSize(60, 30);//设置窗口大小
    SetConsoleTitle("HttpTest(huafang)");//设置标题
    // Load icon
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    if (hIcon) {
        // Set console window icon
        SendMessage(GetConsoleWindow(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(GetConsoleWindow(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
    system("color 2");

    const char* url = "mail.huafang.com";//检测的地址
    const int timeout_ms = 1000; // 超时时间为 1 秒
    const int max_retry = 600; // 最大重试次数为 600 次
    int retry_count = 0; // 当前重试次数
    DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
    LPCTSTR lpszProxyName = NULL;
    LPCTSTR lpszProxyBypass = NULL;
    HINTERNET hIntSession = InternetOpen("httptest", dwAccessType, lpszProxyName, lpszProxyBypass, 0);
    if (!hIntSession) {
        std::cout << "InternetOpen failed: " << GetLastError() << std::endl;
        return -1;
    }
    // 设置连接和接收数据的超时时间
    DWORD dwConnectTimeout = timeout_ms;
    DWORD dwReceiveTimeout = timeout_ms;
    InternetSetOption(hIntSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwConnectTimeout, sizeof(dwConnectTimeout));
    InternetSetOption(hIntSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwReceiveTimeout, sizeof(dwReceiveTimeout));
    while (true) {   // 循环判断直到可以访问为止
        std::cout << "正在测试内网连通性.." << std::endl;
        HINTERNET hHttpSession = InternetConnect(hIntSession, url, INTERNET_DEFAULT_HTTP_PORT, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
        if (hHttpSession) {
            HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, "GET", "/", NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 0);
            if (hHttpRequest) {
                if (HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0)) {
                    DWORD dwStatusCode = 0;
                    DWORD dwBufferLength = sizeof(DWORD);
                    HttpQueryInfo(hHttpRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwBufferLength, NULL);
                    if (dwStatusCode == HTTP_STATUS_OK) {
                        std::cout << "ok 内网可以访问！请点击确定继续操作！" << std::endl;
                        break;   // 跳出循环
                    }
                }
                InternetCloseHandle(hHttpRequest);
            }
            InternetCloseHandle(hHttpSession);
        }
        if (++retry_count > max_retry) {
            std::cout << "超过最大重试次数，退出程序" << std::endl;
            break;
        }
        SYSTEMTIME st;
        GetLocalTime(&st);   // 获取本地时间
        std::cout << st.wYear << "/" << st.wMonth << "/" << st.wDay << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " 不能访问，正在重试,重试次数：" << retry_count << std::endl;
        Sleep(timeout_ms);   // 等待超时时间后重试
    }

    InternetCloseHandle(hIntSession);
    //system("ipconfig");
    //system("net use X: \\\\10.16.129.56\\tools\\pack \"password\" /user:adminusername");
        // 显示提示框
    int result = MessageBox(NULL, "Pass！", "提示", MB_OK);

    // 如果用户点击了OK按钮，则关闭提示框
    if (result == IDOK)
    {
        EndDialog(NULL, 0);
        exit(0);
    }

    return 0;

}



