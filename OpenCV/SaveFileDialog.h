#pragma once

#include <Windows.h>
#include <Commdlg.h>
#include <tchar.h>

class SaveFileDialog
{
public:
    SaveFileDialog(void);
    TCHAR* getFileName();
    bool ShowDialog();

    TCHAR *DefaultExtension;
    TCHAR *FileName;
    TCHAR *Filter;
    int FilterIndex;
    int Flags;
    TCHAR *InitialDir;
    HWND Owner;
    TCHAR *Title;
};