// RomProperty.h : Declaration of the CRomProperty

#pragma once
#include "resource.h"       // main symbols

#include "RomHover_i.h"

#include <atlstr.h>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CRomProperty

class ATL_NO_VTABLE CRomProperty :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRomProperty, &CLSID_RomProperty>,
	public IDispatchImpl<IRomProperty, &IID_IRomProperty, &LIBID_RomHoverLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistFile,
	public IQueryInfo
{
public:
	CRomProperty()
	{
	}

DECLARE_REGISTRY_RESOURCEID(106)


BEGIN_COM_MAP(CRomProperty)
	COM_INTERFACE_ENTRY(IRomProperty)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IQueryInfo)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

protected:
	// IRomInfoShlExt
	CString m_sFilename;

public:
	// IPersistFile
	STDMETHOD(GetClassID)(LPCLSID) { return E_NOTIMPL; }
	STDMETHOD(IsDirty)() { return E_NOTIMPL; }
	STDMETHOD(Load)(LPCOLESTR, DWORD);
	STDMETHOD(Save)(LPCOLESTR, BOOL) { return E_NOTIMPL; }
	STDMETHOD(SaveCompleted)(LPCOLESTR) { return E_NOTIMPL; }
	STDMETHOD(GetCurFile)(LPOLESTR*) { return E_NOTIMPL; }

	// IQueryInfo
	STDMETHOD(GetInfoFlags)(DWORD*) { return E_NOTIMPL; }
	STDMETHOD(GetInfoTip)(DWORD, LPWSTR*);


};

OBJECT_ENTRY_AUTO(__uuidof(RomProperty), CRomProperty)
