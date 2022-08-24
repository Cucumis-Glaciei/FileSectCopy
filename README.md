# FileSectCopy

A 'raw' file copy tool for Windows systems

## Description

Windows systems have mutual exclusion machanism for files, and this prevents opening a file even for Administrators even as read only mode. In digital forensics, this feature typically disturbs acquisition of various artifacts related to the system and the third party softwares such as the SYSTEM registry hive on online systems.

Fortunately, this mutex mechanism could be bypassed if you access the volume device file directly and read its clusters assigned to the target file. This program provides a way to extract data of those mutexed files with that measure.

## Usage

``` Shell
FileSectCopy.exe infile outfile
```

- In most cases, the extraction from local drives requires the administrator privileges. It is recommended to run this program on an escalated command prompt or PowerShell.
- In order to prevent overwriting important files with administrator privileges, the program will not write any data to 'outfile,' if 'outfile' already exists.
- Supported file systems are the followings: NTFS, FAT12, FAT16, FAT32 and exFAT.

## Known Issues

- Compressed or encrypted files with NTFS features could not be extracted correctly.
- If a volume is mounted to an empty directory on another NTFS volume, files in that volume could not be extracted correctly.
- Files on network drives may not be extracted correctly.
