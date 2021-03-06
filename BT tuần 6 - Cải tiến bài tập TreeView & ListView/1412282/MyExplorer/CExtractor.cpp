#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <malloc.h>
#include <memory.h>

#include "CExtractor.h"
#include <crtdbg.h>

class SmartHANDLE
{
public:
	SmartHANDLE( HANDLE h ) { m_h = h; }
	~SmartHANDLE( void ) { if( m_h != INVALID_HANDLE_VALUE ) ::CloseHandle( m_h ); }
	operator HANDLE( void )  { return m_h; }
	HANDLE operator = ( HANDLE h )  
	{ 
		if( m_h != INVALID_HANDLE_VALUE ) 
			::CloseHandle( m_h );
		m_h = h;
		return m_h;
	}
private:
	HANDLE m_h;
};

class SmartFileMapping
{
public:
	SmartFileMapping( void* v ) { m_v = v; }
	~SmartFileMapping( void ) { if( m_v != NULL ) ::UnmapViewOfFile( m_v ); }
	operator LPVOID( void )  { return m_v; }
private:
	void* m_v;
};


//Lấy các biến môi trường ví dụ như %SystemRoot% hay %ProgramFiles%
int _ExpandEnvironmentStrings( LPTSTR pText )
{
	//Tìm chữ % thứ nhất
	TCHAR* p1 = _tcschr( pText, _T('%') );
	if( p1 == NULL )
		return -1;

	//Tìm chữ % thứ hai
	TCHAR* p2 = _tcschr( p1 +1, _T('%') );
	if( p2 == NULL )
		return -1;

	TCHAR provi[MAX_PATH +1];
	provi[0] = TCHAR(0);
	TCHAR env[MAX_PATH +1];
	p1 = pText;
	TCHAR* d = provi;

	while( *p1 != TCHAR(0) )
	{
		if( *p1 != _T('%') )
		{
			*d++ = *p1++;
			continue;
		}

		//Tìm kí tự %
		p2 = _tcschr( p1 +1, _T('%') );
		if( p2 == NULL )
			return -1;

		lstrcpyn( env, p1 +1, int(p2 - p1) );
		TCHAR newEnv[MAX_PATH +1];
		DWORD res = ::GetEnvironmentVariable( env, newEnv, MAX_PATH );

		if( res > 0 )
		{
			lstrcpy( d, newEnv );
			d += lstrlen( newEnv );
		}

		p1 = p2 +1;
	}
	*d = TCHAR(0);

	if( lstrlen( provi ) != 0 )
	{
		lstrcpy( pText, provi );
	}

	return 0;
}

//Những hằng số đặc biệt
#define MAGIC_ICON       0
#define MAGIC_ICO1       1
#define MAGIC_CUR        2
#define MAGIC_BMP        ((WORD)'B'+((WORD)'M'<<8))

#define MAGIC_ANI1       ((WORD)'R'+((WORD)'I'<<8))
#define MAGIC_ANI2       ((WORD)'F'+((WORD)'F'<<8))
#define MAGIC_ANI3       ((WORD)'A'+((WORD)'C'<<8))
#define MAGIC_ANI4       ((WORD)'O'+((WORD)'N'<<8))

#define VER30            0x00030000

int CExtractor::_HasExtension( LPCTSTR pPath )
{
	LPCTSTR p = _tcsrchr( pPath, _T('.') );
	if( p == NULL )
		return 0;
	if( lstrcmpi( p, _T(".com")) == 0 ) return COM_FILE;
	if( lstrcmpi( p, _T(".bat")) == 0 ) return BAT_FILE;
	if( lstrcmpi( p, _T(".cmd")) == 0 ) return CMD_FILE;
	if( lstrcmpi( p, _T(".pif")) == 0 ) return PIF_FILE;
	if( lstrcmpi( p, _T(".lnk")) == 0 ) return LNK_FILE;
	if( lstrcmpi( p, _T(".ico")) == 0 ) return ICO_FILE;
	if( lstrcmpi( p, _T(".exe")) == 0 ) return EXE_FILE;

	return 0;
}


void* CExtractor::_RelativeVirtualAddresstoPtr( IMAGE_DOS_HEADER* pDosHeader, DWORD rva )
{
	IMAGE_NT_HEADERS32* pPE = (IMAGE_NT_HEADERS32*)((BYTE*)pDosHeader + pDosHeader->e_lfanew);

	//Quét bảng section để tìm Virtual Address
	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION( pPE );

	for( int i = 0; i < pPE->FileHeader.NumberOfSections; i++ )
	{
		DWORD size = pSection[i].Misc.VirtualSize ?
			pSection[i].Misc.VirtualSize : pSection[i].SizeOfRawData;

		if( rva >= pSection[i].VirtualAddress &&
			rva <  pSection[i].VirtualAddress + size)
		{
			return (LPBYTE)pDosHeader + pSection[i].PointerToRawData + (rva - pSection[i].VirtualAddress);
		}
	}

	return NULL;
}

void* CExtractor::_GetResourceTable( IMAGE_DOS_HEADER* pDosHeader )
{
	IMAGE_NT_HEADERS32* pPE = (IMAGE_NT_HEADERS32*)((BYTE*)pDosHeader + pDosHeader->e_lfanew);

	if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return NULL;
	if( pPE->Signature != IMAGE_NT_SIGNATURE )
		return NULL;
	if( pPE->FileHeader.SizeOfOptionalHeader < 224 )
		return NULL;

	return _RelativeVirtualAddresstoPtr( pDosHeader, 
		pPE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress );
}

IMAGE_RESOURCE_DIRECTORY_ENTRY* CExtractor::_FindResourceBase( void* prt, int resType, int* pCount )
{
	IMAGE_RESOURCE_DIRECTORY* pDir = (IMAGE_RESOURCE_DIRECTORY*)prt;

	*pCount = 0;

	int count  = pDir->NumberOfIdEntries + pDir->NumberOfNamedEntries;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pRes;
	pRes = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pDir+1);

	int i;
	for( i = 0; i < count; i++ )
	{
		if( pRes[i].Name == (DWORD)resType )
			break;
	}

	if( i == count ) //Nếu tìm không thấy
		return NULL;

	pDir = (IMAGE_RESOURCE_DIRECTORY*)((LPBYTE)prt + 
		(pRes[i].OffsetToData & ~IMAGE_RESOURCE_DATA_IS_DIRECTORY));

	count  = pDir->NumberOfIdEntries + pDir->NumberOfNamedEntries;
	*pCount = count;
	pRes = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pDir+1);
	return pRes;
}

int CExtractor::_FindResourceCount( void* prt, int resType )
{
	int count;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pRes;
	pRes = _FindResourceBase( prt, resType, &count );
	return count;
}

void* CExtractor::_FindResource( IMAGE_DOS_HEADER* pDosHeader, void* prt, int resIndex, int resType, DWORD* pcbSize )
{
	int count;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pRes;
	pRes = _FindResourceBase( prt, resType, &count );

	int index = 0;


	if( resIndex < 0 )
	{
		for( index = 0; index < count; index++ )
			if( pRes[index].Name == (DWORD)(-resIndex) )
				break;
	}
	else
	{
		index = resIndex;
	}

	if( index >= count )
		return NULL; 


	if( pRes[index].OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY)
	{
		IMAGE_RESOURCE_DIRECTORY* pDir;
		pDir = (IMAGE_RESOURCE_DIRECTORY*)((LPBYTE)prt + (pRes[index].OffsetToData & ~IMAGE_RESOURCE_DATA_IS_DIRECTORY) );
		pRes = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(pDir+1);
		index = 0;  
	}


	if( pRes[index].OffsetToData & IMAGE_RESOURCE_DATA_IS_DIRECTORY )
		return NULL;

	IMAGE_RESOURCE_DATA_ENTRY* pEntry;
	pEntry = (IMAGE_RESOURCE_DATA_ENTRY*)((LPBYTE)prt + pRes[index].OffsetToData);

	*pcbSize = pEntry->Size;
	return _RelativeVirtualAddresstoPtr( pDosHeader, pEntry->OffsetToData );
}

UINT CExtractor::_ExtractFromExe( HANDLE hFile, int iconIndex, 
								 int cxIconSize, int cyIconSize, 
								 HICON *phicon, UINT *piconid, 
								 UINT maxIcons, UINT flags )
{
	DWORD fileLen = GetFileSize( hFile, NULL );
	SmartHANDLE hFileMap( CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL ) );
	if( hFileMap == NULL )
		return 0;

	SmartFileMapping pFile( MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0) );
	if( pFile == NULL )
		return 0;

	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)(void*)pFile;
	if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return 0;
	if( pDosHeader->e_lfanew <= 0 )
		return 0;
	if( DWORD(pDosHeader->e_lfanew) >= fileLen )
		return 0;

	void* pRes = _GetResourceTable( pDosHeader );
	if( pRes == NULL) //Không có resource
		return 0; 

	DWORD cbSize = 0;

	if( phicon == NULL )
	{
		int count = _FindResourceCount( pRes, (int)RT_GROUP_ICON );
		return count;
	}

	UINT res = 0;
	while( res < maxIcons )
	{
		//Tìm icon
		NEWHEADER* pIconDir = (NEWHEADER*)_FindResource( pDosHeader, pRes, iconIndex, (int)RT_GROUP_ICON, &cbSize );
		if( pIconDir == NULL )
			return res;

		if( pIconDir->Reserved != 0 || pIconDir->ResType != RES_ICON ) // 1 == iconType
		{
			_ASSERT( 0 );
			return res;
		}

		int idIcon = LookupIconIdFromDirectoryEx( (LPBYTE)pIconDir, TRUE,
			cxIconSize, cyIconSize, flags );
		void* pIcon = _FindResource( pDosHeader, pRes, -idIcon, (int)RT_ICON, &cbSize );
		if( pIcon == NULL )
			return res;

		if( (((LPBITMAPINFOHEADER)pIcon)->biSize != sizeof(BITMAPINFOHEADER)) &&
			(((LPBITMAPINFOHEADER)pIcon)->biSize != sizeof(BITMAPCOREHEADER)) )
		{
			_ASSERT( 0 );
			return res;
		}

		if( piconid )
			piconid[res] = idIcon;

		phicon[res] = CreateIconFromResourceEx( (LPBYTE)pIcon, cbSize,
			TRUE, VER30, cxIconSize, cyIconSize, flags );

		res++;
		iconIndex++; 
	}

	return res;
}

UINT CExtractor::_ExtractFromBMP( LPCTSTR pFileName, int iconIndex, 
								 int cxIcon, int cyIcon, 
								 HICON* phicon, UINT flags )
{
	if( iconIndex >= 1 )
		return 0;

	flags |= LR_LOADFROMFILE;
	HBITMAP hbm = (HBITMAP)LoadImage( NULL, pFileName, IMAGE_BITMAP,
		cxIcon, cyIcon, flags );
	if( hbm == NULL )
		return 0;

	if( phicon == NULL )
	{
		DeleteObject(hbm);
		return 1;
	}

	HBITMAP hbmMask = CreateBitmap( cxIcon, cyIcon, 1, 1, NULL );

	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject( hdc, hbm );

	HDC hdcMask = CreateCompatibleDC(NULL);
	SelectObject(hdcMask, hbmMask);

	SetBkColor( hdc, GetPixel(hdc, 0, 0) );

#define DSTERASE 0x00220326 
	BitBlt( hdcMask, 0, 0, cxIcon, cyIcon, hdc, 0, 0, SRCCOPY );
	BitBlt( hdc, 0, 0, cxIcon, cyIcon, hdcMask, 0, 0, DSTERASE );

	ICONINFO ii;
	ii.fIcon    = TRUE;
	ii.xHotspot = 0;
	ii.yHotspot = 0;
	ii.hbmColor = hbm;
	ii.hbmMask  = hbmMask;
	HICON hicon = CreateIconIndirect( &ii );

	DeleteObject(hdc);
	DeleteObject(hbm);
	DeleteObject(hdcMask);
	DeleteObject(hbmMask);

	*phicon = hicon;
	return 1;
}

UINT CExtractor::_ExtractFromICO( LPCTSTR pFileName, int iconIndex, 
								 int cxIcon, int cyIcon, 
								 HICON* phicon, UINT flags )
{
	if( iconIndex >= 1 )
		return 0;

	flags |= LR_LOADFROMFILE;
	HICON hicon = (HICON)LoadImage( NULL, pFileName, IMAGE_ICON, cxIcon, cyIcon, flags );
	if( hicon == NULL )
		return 0;

	if( phicon == NULL )
		DestroyIcon( hicon );
	else
		*phicon = hicon;

	return 1;
}

UINT CExtractor::_ExtractIcons( LPCTSTR pFileName, int iconIndex, 
							   int cxIcon, int cyIcon, 
							   HICON* phicon, UINT* piconid, UINT maxIcons, UINT flags )
{
	SmartHANDLE hFile( INVALID_HANDLE_VALUE );

	if( phicon != NULL )
		*phicon = NULL;
	if( piconid != NULL )
		*piconid = (UINT)-1;

	TCHAR  fileName[MAX_PATH];
	lstrcpy( fileName, pFileName );     

	//Kiểm tra những đuôi mặc định đặc biệt và thoát
	switch( _HasExtension( fileName ) )
	{
	case COM_FILE:
	case BAT_FILE:
	case CMD_FILE:
	case PIF_FILE:
	case LNK_FILE:
		return 0;

	default:
		break;
	}

	_ExpandEnvironmentStrings( fileName );

	TCHAR  expFileName[MAX_PATH];
	lstrcpy( expFileName, fileName );
	if( SearchPath( NULL, expFileName, NULL, MAX_PATH, fileName, NULL ) == 0 )
		return 0; //Có lỗi

	hFile = CreateFile( fileName, GENERIC_READ|FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if( hFile == INVALID_HANDLE_VALUE )
	{
		hFile = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if( hFile == INVALID_HANDLE_VALUE )
			return 0;
	}
	else
	{
		//Phục hồi thời gian truy cập
		FILETIME ftAccess;
		if( GetFileTime( hFile, NULL, &ftAccess, NULL ) )
			SetFileTime( hFile, NULL, &ftAccess, NULL );
	}


	WORD magic[6];
	DWORD read = 0;
	if( ! ReadFile( hFile,&magic, sizeof(magic), &read, NULL ) )
		return 0;
	if( read != sizeof(magic) )
		return 0;

	UINT res = 0;
	switch( magic[0] )
	{
	case IMAGE_DOS_SIGNATURE:
		res = _ExtractFromExe( hFile, iconIndex, cxIcon, cyIcon, phicon, piconid, maxIcons, flags );
		break;

	case MAGIC_ANI1: 
		if( magic[1] == MAGIC_ANI2 && 
			magic[4] == MAGIC_ANI3 &&
			magic[5] == MAGIC_ANI4 )
			res = _ExtractFromICO( fileName, iconIndex, cxIcon, cyIcon, phicon, flags );
		break;

	case MAGIC_BMP:    
		res = _ExtractFromBMP( fileName, iconIndex, cxIcon, cyIcon, phicon, flags );
		break;

	case MAGIC_ICON:    
		if( (magic[1] == MAGIC_ICON || magic[1] == MAGIC_CUR ) &&
			magic[2] >= 1 )
		{
			res = _ExtractFromICO( fileName, iconIndex, cxIcon, cyIcon, phicon, flags );
		}
		break;
	}

	return res;
}


//Hàm tổng quát
UINT CExtractor::Get( LPCTSTR pFileName, int iconIndex, 
					 int cxIcon, int cyIcon, 
					 HICON* phicon, UINT* piconid, 
					 UINT maxIcons, UINT flags )
{
	return _ExtractIcons( pFileName, iconIndex, 
		cxIcon, cyIcon, 
		phicon, piconid, 
		maxIcons, flags );
}