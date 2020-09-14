#if !defined( __linux__ ) && !defined(__APPLE__) && !defined(__ANDROID__)
#include "stdafx.h"
#endif

#include <stdio.h>
#include <wchar.h>
#include "AddInNative.h"
#include <Windows.h>
#include <map>
#include <vector>

static std::map<std::u16string, long> mMethods;
static std::vector<std::u16string> vMethods;
static std::map<std::u16string, long> mMethods_ru;
static std::vector<std::u16string> vMethods_ru;

static std::map<std::u16string, long> mProps;
static std::vector<std::u16string> vProps;
static std::map<std::u16string, long> mProps_ru;
static std::vector<std::u16string> vProps_ru;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;

inline void fillMap(std::map<std::u16string, long>& map, const std::vector<std::u16string> & vector) {
	long index = 0;
	for (auto &item : vector)
	{
		auto lowCasedItem = item;
		tolowerStr(lowCasedItem);
		map.insert({ lowCasedItem, index });
		index++;
	}
}

//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
	if (!*pInterface)
	{
		*pInterface = new CAddInNative;
		return (long)*pInterface;
	}
	return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
	g_capabilities = capabilities;
	return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
	if (!*pIntf)
		return -1;

	delete *pIntf;
	*pIntf = 0;
	return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
	static char16_t cls_names[] = u"RegEx";
	return reinterpret_cast<WCHAR_T *>(cls_names);
}
//---------------------------------------------------------------------------//

// CAddInNative
//---------------------------------------------------------------------------//
CAddInNative::CAddInNative()
{
	m_iMemory = 0;
	m_iConnect = 0;

	if (mMethods.size() == 0) {
		vMethods = { u"PressCtrlShiftNumAdd", u"PressCtrlShiftNumMinus" };
		fillMap(mMethods, vMethods);
	}
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{
	m_iConnect = (IAddInDefBase*)pConnection;
	return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{
	// Component should put supported component technology version 
	// This component supports 2.1 version
	return 2100;
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{

}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{
	char16_t name[] = u"PressButtons";

	if (!m_iMemory || !m_iMemory->AllocMemory(reinterpret_cast<void **>(wsExtensionName), sizeof(name))) {
		return false;
	};

	memcpy(*wsExtensionName, name, sizeof(name));

	return true;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{
	return ePropLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{
	return -1;
}

//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{

	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
	return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{
	return eMethLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{
	std::basic_string<char16_t> usMethodName = (char16_t*)(wsMethodName);
	tolowerStr(usMethodName);

	auto it = mMethods.find(usMethodName);
	if (it != mMethods.end())
		return it->second;

	it = mMethods_ru.find(usMethodName);
	if (it != mMethods_ru.end())
		return it->second;

	return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{

	if (lMethodNum >= eMethLast)
		return NULL;

	std::basic_string<char16_t> *usCurrentName;

	switch (lMethodAlias)
	{
	case 0: // First language
		usCurrentName = &vMethods[lMethodNum];
		break;
	case 1: // Second language
		//usCurrentName = &vMethods_ru[lMethodNum];
		//break;
	default:
		return 0;
	}

	WCHAR_T *result = nullptr;

	size_t bytes = (usCurrentName->length() + 1) * sizeof(char16_t);

	if (!m_iMemory || !m_iMemory->AllocMemory(reinterpret_cast<void **>(&result), bytes)) {
		return nullptr;
	};

	memcpy(result, usCurrentName->c_str(), bytes);

	return result;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{
	switch (lMethodNum)
	{
	default:
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
	tVariant *pvarParamDefValue)
{
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{
	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
	tVariant* paParams, const long lSizeArray)
{
	switch (lMethodNum)
	{
	case eMethPressCtrlShiftNumAdd:
	{
		// This structure will be used to create the keyboard
		// input event.
		INPUT ip[3];

		// Press the "CTRL" key

		ip[0].type = INPUT_KEYBOARD;
		ip[0].ki.wScan = 0; // hardware scan code for key
		ip[0].ki.time = 0;
		ip[0].ki.dwExtraInfo = 0;

		ip[0].ki.wVk = VK_CONTROL; 
		ip[0].ki.dwFlags = 0; // 0 for key press

		// Press the "SHIFT" key

		ip[1].type = INPUT_KEYBOARD;
		ip[1].ki.wScan = 0; // hardware scan code for key
		ip[1].ki.time = 0;
		ip[1].ki.dwExtraInfo = 0;

		ip[1].ki.wVk = VK_LSHIFT; 
		ip[1].ki.dwFlags = 0; // 0 for key press

		// Press the "NUM PLUS" key

		ip[2].type = INPUT_KEYBOARD;
		ip[2].ki.wScan = 0; // hardware scan code for key
		ip[2].ki.time = 0;
		ip[2].ki.dwExtraInfo = 0;

		ip[2].ki.wVk = VK_ADD; 
		ip[2].ki.dwFlags = 0; // 0 for key press

		SendInput(3, ip, sizeof(INPUT));

		// Release the "SHIFT" key
		ip[0].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		ip[1].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		ip[2].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release

		SendInput(3, ip, sizeof(INPUT));
		return true;

	}
	case eMethPressCtrlShiftNumMinus:
	{
		// This structure will be used to create the keyboard
		// input event.
		INPUT ip[3];

		// Press the "CTRL" key

		ip[0].type = INPUT_KEYBOARD;
		ip[0].ki.wScan = 0; // hardware scan code for key
		ip[0].ki.time = 0;
		ip[0].ki.dwExtraInfo = 0;

		ip[0].ki.wVk = VK_CONTROL;
		ip[0].ki.dwFlags = 0; // 0 for key press

		// Press the "SHIFT" key

		ip[1].type = INPUT_KEYBOARD;
		ip[1].ki.wScan = 0; // hardware scan code for key
		ip[1].ki.time = 0;
		ip[1].ki.dwExtraInfo = 0;

		ip[1].ki.wVk = VK_LSHIFT;
		ip[1].ki.dwFlags = 0; // 0 for key press

		// Press the "NUM PLUS" key

		ip[2].type = INPUT_KEYBOARD;
		ip[2].ki.wScan = 0; // hardware scan code for key
		ip[2].ki.time = 0;
		ip[2].ki.dwExtraInfo = 0;

		ip[2].ki.wVk = VK_SUBTRACT;
		ip[2].ki.dwFlags = 0; // 0 for key press

		SendInput(3, ip, sizeof(INPUT));

		// Release the "SHIFT" key
		ip[0].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		ip[1].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		ip[2].ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release

		SendInput(3, ip, sizeof(INPUT));
		return true;
	}
	default:
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
	tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	return false;
}

void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#if !defined( __linux__ ) && !defined(__APPLE__) && !defined(__ANDROID__)
	_wsetlocale(LC_ALL, L"");
#else
	setlocale(LC_ALL, "");
#endif
}

/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
	m_iMemory = (IMemoryManager*)mem;
	return m_iMemory != 0;
}