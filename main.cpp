#include <QFile>
#include <QDebug>
#include <QList>

#include <windows.h>
#include <Dbghelp.h>

#define MakePtr(cast, ptr, AddValue) (cast)((DWORD)(ptr)+(DWORD)(AddValue))

bool CheckForDependecy( LPVOID _address, QList<QString> & lista)
{
    PIMAGE_DOS_HEADER _dosHeader = (PIMAGE_DOS_HEADER) _address;
    PIMAGE_NT_HEADERS _ntHeader = MakePtr(PIMAGE_NT_HEADERS, _dosHeader, _dosHeader->e_lfanew);

    if( IsBadReadPtr(_ntHeader, sizeof(IMAGE_NT_HEADERS))) {
        return false;
    }

    char* pSig = (char*)&_ntHeader->Signature;
    if( pSig[0] != 'P' || pSig[1] != 'E') {
        return false;
    }

    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageRvaToVa( _ntHeader,
                                                                                   _address,
                                                                                   _ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,
                                                                                   0 );

    while( pImportDesc && pImportDesc->Name )
    {
        PSTR szCurrMod = (PSTR) ImageRvaToVa( _ntHeader, _address, pImportDesc->Name , 0 );

        if( IsBadReadPtr( szCurrMod, 1 ))
        {
            continue;
        }
        QString currentModuleName(szCurrMod);

        //Lets discover the full filename

        HMODULE hm = LoadLibrary(currentModuleName.toStdWString().c_str());
        if (hm != NULL) {
            WCHAR szPath[MAX_PATH];

            if (GetModuleFileName(hm, szPath, MAX_PATH)) {
                currentModuleName = QString::fromStdWString(szPath);
            }

            FreeLibrary(hm);
        }

        if (!lista.contains(currentModuleName)) {
            lista << currentModuleName;
        }

        pImportDesc++;
    }
    return true;
}

void GetListDLL(QString filename, QList<QString> & lista )
{
    HANDLE hFile = CreateFile( filename.toStdWString().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    DWORD filesize;
    filesize = GetFileSize( hFile, NULL );

    HANDLE hFileMap = CreateFileMapping( hFile, 0,PAGE_READONLY,0,0, QString("_tempFileMappingName_").toStdWString().c_str() );
    LPVOID pView = MapViewOfFile( hFileMap,FILE_MAP_READ, 0, 0, filesize );

    QList<QString> listaTemp;

    bool result = CheckForDependecy( pView, listaTemp);

    UnmapViewOfFile( pView );
    CloseHandle( hFileMap );
    CloseHandle( hFile );

    if (result) {
        foreach (QString dllFileName, listaTemp) {
            if (!lista.contains(dllFileName)) {
                lista << dllFileName;
                GetListDLL(dllFileName, lista);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    QList<QString> lista;

    if (argc > 1) {
        //GetListDLL("C:\\Windows\\System32\\notepad.exe", lista);
        GetListDLL(argv[1], lista);

        foreach (QString name, lista) {
            //qDebug() << name;
            printf("%s\n",name.toLatin1().data());
        }
    }
}

