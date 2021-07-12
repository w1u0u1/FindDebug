#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <imagehlp.h>
#pragma comment(lib, "Imagehlp.lib")

///////////////////////////////////////////////////////////////////////////////
// Helper macros 
//

// Thanks to Matt Pietrek 
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD_PTR)(ptr) + (DWORD_PTR)(addValue))

///////////////////////////////////////////////////////////////////////////////
// CodeView debug information structures 
//

#define CV_SIGNATURE_NB10   '01BN'
#define CV_SIGNATURE_RSDS   'SDSR'

// CodeView header 
struct CV_HEADER
{
	DWORD CvSignature; // NBxx
	LONG  Offset;      // Always 0 for NB10
};

// CodeView NB10 debug information 
// (used when debug information is stored in a PDB 2.00 file) 
struct CV_INFO_PDB20
{
	CV_HEADER  Header;
	DWORD      Signature;       // seconds since 01.01.1970
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};

// CodeView RSDS debug information 
// (used when debug information is stored in a PDB 7.00 file) 
struct CV_INFO_PDB70
{
	DWORD      CvSignature;
	GUID       Signature;       // unique identifier 
	DWORD      Age;             // an always-incrementing value 
	BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file 
};

///////////////////////////////////////////////////////////////////////////////
// Function declarations 
//

bool CheckDosHeader(PIMAGE_DOS_HEADER pDosHeader);
bool CheckNtHeaders(PIMAGE_NT_HEADERS pNtHeaders);
bool CheckSectionHeaders(PIMAGE_NT_HEADERS pNtHeaders);
bool CheckDebugDirectory(PIMAGE_DEBUG_DIRECTORY pDebugDir, DWORD DebugDirSize);
bool IsPE32Plus(PIMAGE_OPTIONAL_HEADER pOptionalHeader, bool& bPE32Plus);
bool GetDebugDirectoryRVA(PIMAGE_OPTIONAL_HEADER pOptionalHeader, DWORD& DebugDirRva, DWORD& DebugDirSize);
bool GetFileOffsetFromRVA(PIMAGE_NT_HEADERS pNtHeaders, DWORD Rva, DWORD& FileOffset);
void CleanDebugDirectoryEntries(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir, DWORD DebugDirSize, bool clean, bool cleanPath);
void CleanDebugDirectoryEntry(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir, bool clean, bool cleanPath);
void CleanCodeViewDebugInfo(LPBYTE pDebugInfo, DWORD DebugInfoSize, bool clean, bool cleanPath);
void DumpDebugDirectoryEntries(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir, DWORD DebugDirSize);
void DumpDebugDirectoryEntry(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir);
void DumpCodeViewDebugInfo(LPBYTE pDebugInfo, DWORD DebugInfoSize);
void DumpMiscDebugInfo(LPBYTE pDebugInfo, DWORD DebugInfoSize);
LPCTSTR DebugTypeToStr(DWORD DebugType);
void DumpGuid(GUID& Guid);

void CleanDebugInfo(LPCTSTR lpFileName);
void GetDebugInfo(LPCTSTR lpFileName);
typedef void(*GETDEBUGINFOCALLBACK)(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir);
extern GETDEBUGINFOCALLBACK GetDebugInfo_CB;