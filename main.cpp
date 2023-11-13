#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>
#include <io.h>
#include <commdlg.h>
#include <Shlwapi.h> // Include Shlwapi.h
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Shlwapi.lib") // Link against Shlwapi.lib

#define BUTTON_ID1 1 // The ID of the first button
#define BUTTON_ID2 2 // The ID of the second button
#define BUTTON_ID3 3 // The ID of the third button

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        if (wParam == BUTTON_ID1)
        {
            // Your existing code for the first button
            PWSTR path = NULL;
            HRESULT hr = SHGetKnownFolderPath(FOLDERID_Programs, 0, NULL, &path);

            if (SUCCEEDED(hr))
            {
                WCHAR szPath[MAX_PATH];
                StringCchPrintf(szPath, MAX_PATH, L"%s\\Lunar Client.lnk", path);
                ShellExecute(NULL, L"open", szPath, NULL, NULL, SW_SHOW);
            }

            CoTaskMemFree(path);
        }
        // Inside the WindowProc function
        else if (wParam == BUTTON_ID3)
        {
            // The new code for the third button
            PWSTR path = NULL;
            HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

            if (SUCCEEDED(hr))
            {
                WCHAR szFolderPath[MAX_PATH];
                StringCchPrintf(szFolderPath, MAX_PATH, L"%s\\.minecraft\\resourcepacks\\Cosmetics [Overlay]\\assets\\lunar\\cosmetics\\cloaks", path);

                // Code to move the files from the NEW folder to the specified path
                WCHAR szCacheFolderPath[MAX_PATH];
                GetModuleFileName(NULL, szCacheFolderPath, MAX_PATH);
                PathRemoveFileSpec(szCacheFolderPath);
                StringCchCat(szCacheFolderPath, MAX_PATH, L"\\cache\\NEW\\");

                WCHAR szSearchPath[MAX_PATH];
                StringCchPrintf(szSearchPath, MAX_PATH, L"%s\\*", szCacheFolderPath);

                WIN32_FIND_DATA ffd;
                HANDLE hFind = FindFirstFile(szSearchPath, &ffd);

                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                        {
                            WCHAR szSourceFilePath[MAX_PATH];
                            StringCchPrintf(szSourceFilePath, MAX_PATH, L"%s\\%s", szCacheFolderPath, ffd.cFileName);

                            WCHAR szDestFilePath[MAX_PATH];
                            StringCchPrintf(szDestFilePath, MAX_PATH, L"%s\\%s", szFolderPath, ffd.cFileName);

                            BOOL bSuccess = MoveFile(szSourceFilePath, szDestFilePath);
                            if (!bSuccess)
                            {
                                DWORD dwError = GetLastError();
                                // Handle the error.
                                WCHAR szErrorMessage[256];
                                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, szErrorMessage, ARRAYSIZE(szErrorMessage), NULL);
                                MessageBox(NULL, szErrorMessage, L"Error", MB_OK | MB_ICONERROR);
                            }
                        }
                    } while (FindNextFile(hFind, &ffd) != 0);

                    FindClose(hFind);
                }
            }

            CoTaskMemFree(path);
        }

        else if (wParam == BUTTON_ID2)
        {
            // The new code for the second button
            PWSTR path = NULL;
            HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

            if (SUCCEEDED(hr))
            {
                WCHAR szFolderPath[MAX_PATH];
                StringCchPrintf(szFolderPath, MAX_PATH, L"%s\\.minecraft\\resourcepacks\\Cosmetics [Overlay]", path);

                DWORD dwAttrib = GetFileAttributes(szFolderPath);

                if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    int msgboxID = MessageBox(
                        NULL,
                        (LPCWSTR)L"The folder Cosmetics [Overlay] does not exist.",
                        (LPCWSTR)L"Folder Check",
                        MB_ICONINFORMATION | MB_OKCANCEL | MB_DEFBUTTON2
                    );

                    switch (msgboxID)
                    {
                    case IDCANCEL:
                        // TODO: add code
                        break;
                    case IDOK:
                        ShellExecute(NULL, L"open", L"https://www.mediafire.com/file/zyvgio84xlqh5im/Cosmetics_%255BOverlay%255D.zip/file", NULL, NULL, SW_SHOW);
                        break;
                    }
                }
                if (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                    (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    int msgboxID = MessageBox(
                        NULL,
                        (LPCWSTR)L"The folder exists. You need to choose the cosmetic that you own. If it is animated also get the mcmeta file for it as well.",
                        (LPCWSTR)L"Folder Check",
                        MB_ICONINFORMATION | MB_OKCANCEL | MB_DEFBUTTON2
                    );

                    switch (msgboxID)
                    {
                    case IDCANCEL:
                        // TODO: add code
                        break;
                    case IDOK:
                        // File Open dialog
                        IFileOpenDialog* pFileOpen;
                        // Create the FileOpenDialog object.
                        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

                        if (SUCCEEDED(hr))
                        {
                            DWORD dwOptions;
                            hr = pFileOpen->GetOptions(&dwOptions);
                            if (SUCCEEDED(hr))
                            {
                                hr = pFileOpen->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
                            }
                            // Set the default folder.
                            IShellItem* pItem;
                            hr = SHCreateItemFromParsingName(L"%s\\.lunarclient\\textures\\assets\\lunar\\cosmetics", NULL, IID_PPV_ARGS(&pItem));
                            if (SUCCEEDED(hr))
                            {
                                pFileOpen->SetFolder(pItem);
                                pItem->Release();
                            }

                            // Show the Open dialog box.
                            hr = pFileOpen->Show(NULL);

                            // Get the file names from the dialog box.
                            if (SUCCEEDED(hr))
                            {
                                IShellItemArray* pItems;
                                hr = pFileOpen->GetResults(&pItems);
                                if (SUCCEEDED(hr))
                                {
                                    DWORD count;
                                    hr = pItems->GetCount(&count);
                                    if (SUCCEEDED(hr))
                                    {
                                        for (DWORD i = 0; i < count; i++)
                                        {
                                            IShellItem* pItem;
                                            hr = pItems->GetItemAt(i, &pItem);
                                            if (SUCCEEDED(hr))
                                            {
                                                PWSTR pszFilePath;
                                                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                                if (SUCCEEDED(hr))
                                                {
                                                    // Copy the file to the cache folder.
                                                    wchar_t drive[_MAX_DRIVE];
                                                    wchar_t dir[_MAX_DIR];
                                                    wchar_t fname[_MAX_FNAME];
                                                    wchar_t ext[_MAX_EXT];
                                                    errno_t err = _wsplitpath_s(pszFilePath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

                                                    if (err != 0)
                                                    {
                                                        // Handle the error.
                                                        // ...
                                                    }
                                                    else
                                                    {
                                                        WCHAR szCacheFolderPath[MAX_PATH];
                                                        GetModuleFileName(NULL, szCacheFolderPath, MAX_PATH);
                                                        PathRemoveFileSpec(szCacheFolderPath);
                                                        StringCchCat(szCacheFolderPath, MAX_PATH, L"\\cache\\");

                                                        // Create the cache folder.
                                                        CreateDirectory(szCacheFolderPath, NULL);

                                                        WCHAR szDestFilePath[MAX_PATH];
                                                        StringCchPrintf(szDestFilePath, MAX_PATH, L"%s\\OLD\\%s%s", szCacheFolderPath, fname, ext);

                                                        // Copy the file to the OLD folder.
                                                        BOOL bSuccess = CopyFile(pszFilePath, szDestFilePath, FALSE);
                                                        if (!bSuccess)
                                                        {
                                                            DWORD dwError = GetLastError();
                                                            // Handle the error.
                                                            WCHAR szErrorMessage[256];
                                                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, szErrorMessage, ARRAYSIZE(szErrorMessage), NULL);
                                                            MessageBox(NULL, szErrorMessage, L"Error", MB_OK | MB_ICONERROR);
                                                        }
                                                        else
                                                        {
                                                            // Show a message box that says "Cache complete".
                                                            MessageBox(NULL, L"Cache complete", L"Information", MB_OK | MB_ICONINFORMATION);

                                                            // Show a message box that says "Choose the new cosmetic".
                                                            int msgboxID = MessageBox(NULL, L"Choose the new cosmetic", L"Information", MB_OKCANCEL | MB_ICONINFORMATION);
                                                            if (msgboxID == IDOK)
                                                            {
                                                                // Reopen the File picker UI.
                                                                // Show the Open dialog box.
                                                                hr = pFileOpen->Show(NULL);

                                                                // Get the file names from the dialog box.
                                                                if (SUCCEEDED(hr))
                                                                {
                                                                    IShellItemArray* pItems;
                                                                    hr = pFileOpen->GetResults(&pItems);
                                                                    if (SUCCEEDED(hr))
                                                                    {
                                                                        DWORD count;
                                                                        hr = pItems->GetCount(&count);
                                                                        if (SUCCEEDED(hr))
                                                                        {
                                                                            for (DWORD i = 0; i < count; i++)
                                                                            {
                                                                                IShellItem* pItem;
                                                                                hr = pItems->GetItemAt(i, &pItem);
                                                                                if (SUCCEEDED(hr))
                                                                                {
                                                                                    PWSTR pszFilePath;
                                                                                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                                                                    if (SUCCEEDED(hr))
                                                                                    {
                                                                                        // Replace the old files in cache with the new ones.
                                                                                        // ... (Your existing code to copy the files to the cache folder)
                                                                                        WCHAR szNewFilePath[MAX_PATH];
                                                                                        StringCchPrintf(szNewFilePath, MAX_PATH, L"%s\\NEW\\%s%s", szCacheFolderPath, fname, ext);

                                                                                        // Copy the new files to the NEW folder.
                                                                                        BOOL bSuccess = CopyFile(pszFilePath, szNewFilePath, FALSE);
                                                                                        if (!bSuccess)
                                                                                        {
                                                                                            DWORD dwError = GetLastError();
                                                                                            // Handle the error.
                                                                                            WCHAR szErrorMessage[256];
                                                                                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, szErrorMessage, ARRAYSIZE(szErrorMessage), NULL);
                                                                                            MessageBox(NULL, szErrorMessage, L"Error", MB_OK | MB_ICONERROR);
                                                                                            // Copy the new files to the NEW folder.
                                                                                            BOOL bSuccess = CopyFile(pszFilePath, szNewFilePath, FALSE);
                                                                                            if (!bSuccess)
                                                                                            {
                                                                                                DWORD dwError = GetLastError();
                                                                                                // Handle the error.
                                                                                                WCHAR szErrorMessage[256];
                                                                                                FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, szErrorMessage, ARRAYSIZE(szErrorMessage), NULL);
                                                                                                MessageBox(NULL, szErrorMessage, L"Error", MB_OK | MB_ICONERROR);
                                                                                            }
                                                                                            else
                                                                                            {
                                                                                                // Show a message box that says "Done".
                                                                                                MessageBox(NULL, L"Done", L"Information", MB_OK | MB_ICONINFORMATION);
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }

                                                        CoTaskMemFree(pszFilePath);
                                                    }
                                                }
                                                pItem->Release();
                                            }
                                        }
                                    }
                                    pItems->Release();
                                }
                                pFileOpen->Release();
                            }
                        }
                        break;
                    }
                }
            }
            CoTaskMemFree(path);
        }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Set the window size to half the screen size
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = screenWidth / 6;
    int windowHeight = screenHeight / 4;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"LCT",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    // Inside the wWinMain function
    int buttonWidth = 80;
    int buttonHeight = 30;
    int buttonSpacing = 10; // The space between the buttons

    // Calculate the total height of all buttons and the spaces between them
    int totalHeight = 3 * buttonHeight + 2 * buttonSpacing;

    // Calculate the y position of the first button
    int yButton1 = (windowHeight - totalHeight) / 2;

    // Calculate the y positions of the other buttons
    int yButton2 = yButton1 + buttonHeight + buttonSpacing;
    int yButton3 = yButton2 + buttonHeight + buttonSpacing;

    // Calculate the x position of the buttons (centered horizontally)
    int xButton = (windowWidth - buttonWidth) / 2;

    // Create the first button
    HWND hwndButton1 = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Lunar",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        xButton,         // x position 
        yButton1,         // y position 
        buttonWidth,        // Button width
        buttonHeight,        // Button height
        hwnd,     // Parent window
        (HMENU)BUTTON_ID1,       // The ID of the button
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Create the second button
    HWND hwndButton2 = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Start",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        xButton,         // x position 
        yButton2,         // y position 
        buttonWidth,        // Button width
        buttonHeight,        // Button height
        hwnd,     // Parent window
        (HMENU)BUTTON_ID2,       // The ID of the button
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    // Create the third button
    HWND hwndButton3 = CreateWindow(
        L"BUTTON",  // Predefined class; Unicode assumed 
        L"Link",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        xButton,         // x position 
        yButton3,         // y position 
        buttonWidth,        // Button width
        buttonHeight,        // Button height
        hwnd,     // Parent window
        (HMENU)BUTTON_ID3,       // The ID of the button
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}