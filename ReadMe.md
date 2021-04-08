# Cardfile for Windows CRD Files Reader

ReadCrd is a cross-planform console application which prints content of a `Cardfile for Windows 3.x` file (`CRD` extension). The file signature must be `MGC` or `RRG`.

## Usage

Download the Linux or Windows binary file in a folder with CRD files and run it in a console.

```
./ReadCrd Contacts.CRD
```
or to read a file with Windows-1251 encoding
```
./ReadCrd -e Windows-1251 Contacts.CRD
```

## Limitations

* Files with the `RRG` signature can contain OLE Objects. In this case only the card title is printed without text data.
* Only `А` to `Я` and `а` to `я` cyrillic characters are decoded. Characters like `№` or `ё` are not decoded. 

## Text Encoding

Texts in the CRD files has 8-bit characters encoding, extended ASCII. This program can print UTF-8 characters converted from the `Windows-1251` encoding. If your file has another encoding you can print in in a text file and decode with a text editor:
```
./ReadCrd Contacts.CRD > contacts.txt
```

### Windows Terminal

Use the command `chcp` to display and change the **code page** used by the terminal. For example, to set the UTF-8 encoding use
```
chcp 65001
ReadCrd.exe -e Windows-1251 Contacts.CRD
```

Or set the Windows-1251 encoding
```
chcp 1251
ReadCrd.exe Contacts.CRD
```

## Compilation

You need a C/C++ compiler.

From the Visual Studio Code [documentation](https://code.visualstudio.com/docs/languages/cpp):

> Most Linux distributions have the [GNU Compiler Collection (GCC)](https://wikipedia.org/wiki/GNU_Compiler_Collection) installed and macOS users can get the [Clang](https://wikipedia.org/wiki/Clang) tools with [XCode](https://developer.apple.com/xcode/).
> 
> On Windows: You can use this [Mingw-w64 link](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download) to download the Windows Mingw-w64 installer.
> 
> Download and run the installer, which should be named `mingw-w64-install.exe` in your Download folder. Then add the path to your Mingw-w64 `bin` folder to the Windows `PATH` environment variable.

### On Linux

Compile
```
gcc -o ReadCrd ReadCrd.c
```

### On Windows 10

Compile
```
gcc -o ReadCrd.exe ReadCrd.c
```

## Thanks to

[Jeff Parsons](https://github.com/jeffpar) for
[Q99340: Windows 3.1 Card File Format](https://jeffpar.github.io/kbarchive/kb/099/Q99340/)  at KnowledgeBaseArchive
and [ArchiveTeam/Cardfile](http://fileformats.archiveteam.org/wiki/Cardfile) - with file examples.


