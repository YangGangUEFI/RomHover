// dllmain.h : Declaration of module class.

class CRomHoverModule : public ATL::CAtlDllModuleT< CRomHoverModule >
{
public :
	DECLARE_LIBID(LIBID_RomHoverLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ROMHOVER, "{b7f225a2-8488-4289-80f1-87dca8f13a38}")
};

extern class CRomHoverModule _AtlModule;
