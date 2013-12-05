ListDependency
==============

List EXE/DLL dependencies recursively (Win32).

It's like "dumpbin.exe /dependents" (Windows SDK) and Dependency Walker (Depends.exe), but showing simple list with full paths and full dependency. 

I know I don't need to use Qt. But my first idea was to create a custom deploy to QtCreator.

<h2>Example</h2>

ListDependency:
```
C:\>ListDependency.exe c:\windows\system32\msvcr100.dll
C:\Windows\syswow64\kernel32.dll
C:\Windows\SysWOW64\ntdll.dll
C:\Windows\syswow64\KERNELBASE.dll
```


DumpBin:

```
C:\>dumpbin /dependents c:\windows\system32\msvcr100.dll
Microsoft (R) COFF/PE Dumper Version 11.00.61030.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file msvcr100.dll

File Type: DLL

  Image has the following dependencies:

    KERNEL32.dll
    
```

