// RomProperty.cpp : Implementation of CRomProperty

#include "pch.h"
#include "RomProperty.h"
#include "Uefi.h"

// CRomProperty

HRESULT CRomProperty::Load(LPCOLESTR wszFilename, DWORD)
{
	//Save FileName
	m_sFilename = wszFilename;

	//OK
	return S_OK;
}

HRESULT CRomProperty::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());    // init MFC

	LPMALLOC   pMalloc;
	CFile	   file;
	CString    sTooltip;
	errno_t    err;

	UNREFERENCED_PARAMETER(dwFlags);
	USES_CONVERSION;

	EFI_IMAGE_DOS_HEADER                DosHdr;
	EFI_IMAGE_OPTIONAL_HEADER_UNION		Hdr;
	UINT16                              Magic;

	// Try to open the file.
	if (!file.Open(m_sFilename, CFile::modeRead | CFile::shareDenyWrite))
		return E_FAIL;

	file.Read(&DosHdr, sizeof(EFI_IMAGE_DOS_HEADER));

	if (DosHdr.e_magic == EFI_IMAGE_DOS_SIGNATURE) {
		//
		// DOS image header is present, so read the PE header after the DOS image header.
		//
		file.Seek((DosHdr.e_lfanew) & 0x0ffff, CFile::begin);
		file.Read(&Hdr, sizeof(EFI_IMAGE_OPTIONAL_HEADER_UNION));

	}
	else {
		//
		// DOS image header is not present, so PE header is at the image base.no in parked support for now
		//
		return E_FAIL;
	}
	CString tempstr;

	if (Hdr.Pe32.Signature != EFI_IMAGE_NT_SIGNATURE) {
		sTooltip = _T("不能识别该驱动！\n");
		goto SHOWRESULT;
	}

	sTooltip = _T("文件名：");
	sTooltip += m_sFilename;
	sTooltip += _T("\n");

	sTooltip += _T("\nPE文件头内容：\n");

	sTooltip += _T("\t支持CPU架构：\t");
	switch (Hdr.Pe32.FileHeader.Machine) {
	case EFI_IMAGE_MACHINE_IA32:
		//
		// Assume PE32 image with IA32 Machine field.
		//
		Magic = EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC;
		tempstr = _T("IA32(32bit)");
		break;
	case EFI_IMAGE_MACHINE_X64:
		//
		// Assume PE32+ image with X64 or IPF Machine field
		//
		Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
		tempstr = _T("X64(64bit)");
		break;
	case EFI_IMAGE_MACHINE_EBC:
		//
		// Assume PE32+ image with X64 or IPF Machine field
		//
		Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
		tempstr = _T("EBC");
		break;

	case IMAGE_FILE_MACHINE_ARMTHUMB_MIXED:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("ARM 32 mixed");
		break;

	case IMAGE_FILE_MACHINE_ARM64:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("ARM 64bit");
		break;

	case EFI_IMAGE_MACHINE_IA64:
		//
		// Assume PE32+ image with X64 or IPF Machine field
		//
		Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
		tempstr = _T("IA64(IPF 64bit)");
		break;

	case EFI_IMAGE_MACHINE_LOONGARCH32:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("LoongArch 32bit");
		break;

	case EFI_IMAGE_MACHINE_LOONGARCH64:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("LoongArch 64bit");
		break;

	case EFI_IMAGE_MACHINE_RISCV32:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("RISC-V 32bit");
		break;

	case EFI_IMAGE_MACHINE_RISCV64:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("RISC-V 64bit");
		break;

	case EFI_IMAGE_MACHINE_RISCV128:
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("RISC-V 128bit");
		break;

	default:
		//
		// For unknow Machine field, use Magic in optional Header
		//
		Magic = Hdr.Pe32.OptionalHeader.Magic;
		tempstr = _T("Unknown");

	}
	sTooltip += tempstr;
	sTooltip += _T("\n");

	if (Hdr.Pe32.FileHeader.TimeDateStamp != 0) {
		time_t TimeX = (time_t)Hdr.Pe32.FileHeader.TimeDateStamp;
		struct tm pGMT;
		err = gmtime_s(&pGMT, &TimeX);
		char pTime[26];
		err = asctime_s(pTime, 26, &pGMT);
		tempstr.Format(_T("\t驱动生成时间：\t%S"), pTime);
	}
	else {
		tempstr.Format(_T("\t驱动生成时间：\t已清除\n"));
	}
	sTooltip += tempstr;

	sTooltip += _T("\tSection数目：\t");
	tempstr.Format(_T("%d"), Hdr.Pe32.FileHeader.NumberOfSections);
	sTooltip += tempstr;
	sTooltip += _T("\n");

	sTooltip += _T("\nOPTIONAL文件头内容：\n");
	sTooltip += _T("Magic:\t\t\t");
	PE_COFF_LOADER_IMAGE_CONTEXT ImageContext;
	
	memset(&ImageContext, 0, sizeof(PE_COFF_LOADER_IMAGE_CONTEXT));

	if (Hdr.Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC)
	{
		tempstr.Format(_T("%X (PE32) "), EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC);
		ImageContext.ImageType = Hdr.Pe32.OptionalHeader.Subsystem;
		ImageContext.ImageSize = (UINT64)Hdr.Pe32.OptionalHeader.SizeOfImage;
		ImageContext.SectionAlignment = Hdr.Pe32.OptionalHeader.SectionAlignment;
		ImageContext.SizeOfHeaders = Hdr.Pe32.OptionalHeader.SizeOfHeaders;
		ImageContext.NumberOfRvaAndSizes = Hdr.Pe32.OptionalHeader.NumberOfRvaAndSizes;
		memcpy(ImageContext.DataDirectory, Hdr.Pe32.OptionalHeader.DataDirectory, sizeof(EFI_IMAGE_DATA_DIRECTORY) * EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES);

	}
	else
	{
		tempstr.Format(_T("%X (PE32+) "), EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC);
		ImageContext.ImageType = Hdr.Pe32Plus.OptionalHeader.Subsystem;
		ImageContext.ImageSize = (UINT64)Hdr.Pe32Plus.OptionalHeader.SizeOfImage;
		ImageContext.SectionAlignment = Hdr.Pe32Plus.OptionalHeader.SectionAlignment;
		ImageContext.SizeOfHeaders = Hdr.Pe32Plus.OptionalHeader.SizeOfHeaders;
		ImageContext.NumberOfRvaAndSizes = Hdr.Pe32Plus.OptionalHeader.NumberOfRvaAndSizes;
		memcpy(ImageContext.DataDirectory, Hdr.Pe32Plus.OptionalHeader.DataDirectory, sizeof(EFI_IMAGE_DATA_DIRECTORY) * EFI_IMAGE_NUMBER_OF_DIRECTORY_ENTRIES);

	}
	sTooltip += tempstr;
	sTooltip += _T("\n");

	sTooltip += _T("驱动类型：\t\t");
	switch (ImageContext.ImageType) {
	case EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:
		tempstr = _T("UEFI Runtime驱动");
		break;
	case EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION:
		tempstr = _T("UEFI Application");
		break;
	case EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:
		tempstr = _T("UEFI Boot Service驱动");
		break;
	case EFI_IMAGE_SUBSYSTEM_SAL_RUNTIME_DRIVER:
		tempstr = _T("SAL Runtime Driver");
		break;
	}
	sTooltip += tempstr;
	sTooltip += _T("\n");

	tempstr.Format(_T("\t入口函数地址：\t0x%x\n"), Hdr.Pe32Plus.OptionalHeader.AddressOfEntryPoint);
	sTooltip += tempstr;

	tempstr.Format(_T("\tSection对齐：\t0x%x\n"), Hdr.Pe32Plus.OptionalHeader.SectionAlignment);
	sTooltip += tempstr;

	tempstr.Format(_T("\t文件对齐：\t0x%x\n"), Hdr.Pe32Plus.OptionalHeader.FileAlignment);
	sTooltip += tempstr;

	tempstr.Format(_T("\t文件大小：\t%d(Byte)\n"), Hdr.Pe32Plus.OptionalHeader.SizeOfImage);
	sTooltip += tempstr;

	sTooltip += _T("\tRva目录 数目:\t");
	tempstr.Format(_T("%d"), ImageContext.NumberOfRvaAndSizes);
	sTooltip += tempstr;
	sTooltip += _T("\n");

	for (int i = 0; i < 10; i++) {
		switch (i) {
		case 0:
			sTooltip += _T("\t\tExport Directory		RVA [size]:");
			break;
		case 1:
			sTooltip += _T("\t\tImport Directory	RVA [size]:");
			break;
		case 2:
			sTooltip += _T("\t\tResource Directory	RVA [size]:");
			break;
		case 3:
			sTooltip += _T("\t\tException Directory	RVA [size]:");
			break;
		case 4:
			sTooltip += _T("\t\tCert Directory		RVA [size]:");
			break;
		case 5:
			sTooltip += _T("\t\tBaseReloc Directory	RVA [size]:");
			break;
		case 6:
			sTooltip += _T("\t\tDebug Directory	RVA [size]:");
			break;
		case 7:
			sTooltip += _T("\t\tArch Directory		RVA [size]:");
			break;
		case 8:
			sTooltip += _T("\t\tGlobal Ptr Directory	RVA [size]:");
			break;
		case 9:
			sTooltip += _T("\t\tTLS Directory		RVA [size]:");
			break;
		case 10:
			sTooltip += _T("\t\tLoad Conf Directory	RVA [size]:");
			break;
		case 11:
			sTooltip += _T("\t\tBoundImport Directory	RVA [size]:");
			break;
		case 12:
			sTooltip += _T("\t\tAddrTable Directory	RVA [size]:");
			break;
		case 13:
			sTooltip += _T("\t\tDelayImport Directory	RVA [size]:");
			break;
		case 14:
			sTooltip += _T("\t\tCOM Desc Directory	RVA [size]:");
			break;
		case 15:
			sTooltip += _T("\t\tReserved Directory	RVA [size]:");
			break;
		}
		tempstr.Format(_T("%x [%x]"), ImageContext.DataDirectory[i].VirtualAddress, ImageContext.DataDirectory[i].Size);
		sTooltip += tempstr;
		sTooltip += _T("\n");
	}

	if (Hdr.Pe32Plus.OptionalHeader.SectionAlignment != Hdr.Pe32Plus.OptionalHeader.FileAlignment) {
		// Can't handle mixed alignment by now, skip
		sTooltip += _T("Section和文件对齐不同，部分信息已忽略！\n");
		goto SHOWRESULT;
	}

	sTooltip += _T("\nDebug目录\n");
	sTooltip += _T("        Time  Type        Size        RVA    Pointer\n");
	sTooltip += _T("    --------  ------   --------     --------      --------\n");

	EFI_IMAGE_DEBUG_DIRECTORY_ENTRY DebugEntry;
	file.Seek(ImageContext.DataDirectory[6].VirtualAddress, CFile::begin);
	file.Read(&DebugEntry, sizeof(EFI_IMAGE_DEBUG_DIRECTORY_ENTRY));

	tempstr.Format(_T("   %08x cv      %08x %08x %x\n"), DebugEntry.TimeDateStamp, DebugEntry.SizeOfData, DebugEntry.RVA, DebugEntry.FileOffset);
	sTooltip += tempstr;

	sTooltip += _T("\npdb文件位置: ");
	file.Seek(DebugEntry.FileOffset, CFile::begin);
	UINT8 buff[800];
	memset(buff, 0, 800);
	file.Read(buff, DebugEntry.SizeOfData);
	void* CodeViewEntryPointer = buff;
	char* lpStr;

	switch (*(UINT32*)CodeViewEntryPointer) {
	case CODEVIEW_SIGNATURE_NB10:
		lpStr = (char*)((char*)CodeViewEntryPointer + sizeof(EFI_IMAGE_DEBUG_CODEVIEW_NB10_ENTRY));
		break;
	case CODEVIEW_SIGNATURE_RSDS:
		lpStr = (char*)((char*)CodeViewEntryPointer + sizeof(EFI_IMAGE_DEBUG_CODEVIEW_RSDS_ENTRY));
		break;
	case CODEVIEW_SIGNATURE_MTOC:
		lpStr = (char*)((char*)CodeViewEntryPointer + sizeof(EFI_IMAGE_DEBUG_CODEVIEW_MTOC_ENTRY));
		break;
	default:
		lpStr = "Unknow";
		break;

	}

	tempstr.Format(_T("%S\n"), lpStr);
	sTooltip += tempstr;

SHOWRESULT:
	// Get an IMalloc interface from the shell.
	if (FAILED(SHGetMalloc(&pMalloc)))
		return E_FAIL;

	// Allocate a buffer for Explorer.  Note that the must pass the string 
	// back as a Unicode string, so the string length is multiplied by the 
	// size of a Unicode character.
	size_t TipSize = (static_cast<unsigned long long>(10) + lstrlen(sTooltip)) * sizeof(wchar_t);
	*ppwszTip = (LPWSTR)pMalloc->Alloc(TipSize);

	// Release the IMalloc interface now that we're done using it.
	pMalloc->Release();

	if (NULL == *ppwszTip)
	{
		return E_OUTOFMEMORY;
	}

	// Use the Unicode string copy function to put the tooltip text in the buffer.
	wcscpy_s(*ppwszTip, TipSize, T2COLE((LPCTSTR)sTooltip));

	return S_OK;
}