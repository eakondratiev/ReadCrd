# Windows CRD Files Reader

ReadCrd is a cross-planform console application which prints content of the `Cardfile for Windows 3.x` file (`CRD` extension). The file signature must be `MGC` or `RRG`.

## Text Encoding

Texts in the CRD files has 8-bit characters encoding, extended ASCII. This program can print UTF-8 characters converted from the `Windows-1251` encoding. If your file has another encoding you can print in in a text file and decode with a text editor:
```
./ReadCrd Contacts.CRD > contacts.txt
```

## Limitations

* Files with the `RRG` signature can contain OLE Objects. In this case only the card title is printed without text data.
* Only `А` to `Я` and `а` to `я` cyrillic characters are decoded. Characters like `№` or `ё` are not decoded. 

## Install and Run

Download the Linux or Windows binary file in a folder with CRD files and run it in a console.

```
./ReadCrd Contacts.CRD
```
or to read a file with WIndows-1251 encoding
```
./ReadCrd -e Windows-1251 Contacts.CRD
```

## File Hash
### Windows
```
Get-FileHash ReadCrd.exe | Format-List

Algorithm : SHA256
Hash      : 052090BD4F7C33C27BD032FC35173C906F9E74CD91F825F58865951F1C92EDE5
Path      : ReadCrd.exe
File date : 2021-02-06 12:00
```

### Linux
```
sha256sum -b ReadCrd

Hash      : e51baa173b2884027757f36e1dc44ccbd193b2dab61ecb032f907a0ba6f27f9c
File      : ReadCrd
File date : 2021-02-06 12:00
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


