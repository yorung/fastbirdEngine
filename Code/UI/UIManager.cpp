#include <UI/StdAfx.h>
#include <UI/UIManager.h>
#include <UI/IUIEditor.h>
#include <UI/Wnd.h>
#include <UI/TextField.h>
#include <UI/StaticText.h>
#include <UI/Button.h>
#include <UI/KeyboardCursor.h>
#include <UI/ImageBox.h>
#include <UI/CheckBox.h>
#include <UI/ListBox.h> 
#include <UI/ListItem.h>
#include <UI/FileSelector.h>
#include <UI/Scroller.h>
#include <UI/RadioBox.h>
#include <UI/HexagonalContextMenu.h>
#include <UI/CardScroller.h>
#include <UI/VerticalGauge.h>
#include <UI/HorizontalGauge.h>
#include <UI/NumericUpDown.h>
#include <UI/DropDown.h>
#include <UI/TextBox.h>
#include <UI/ColorRampComp.h>
#include <UI/NamedPortrait.h>
#include <UI/UIAnimation.h>
#include <UI/UICommands.h>
#include <UI/PropertyList.h>
#include <UI/TabWindow.h>
#include <CommonLib/FileSystem.h>
#include <CommonLib/LuaUtils.h>
#include <CommonLib/LuaObject.h>
#include <Engine/IUIObject.h>
#include <Engine/IScriptSystem.h>
#include <Engine/TextManipulator.h>

fastbird::GlobalEnv* gFBEnv = 0;

namespace fastbird
{


static float gTooltipFontSize = 26;
static float gDelayForTooltip = 0.7f;
//---------------------------------------------------------------------------
UIManager::UIManager(lua_State* L)
	: mInputListenerEnable(true)
	, mFocusWnd(0)
	, mKeyboardFocus(0)
	, mMouseIn(false)
	, mPopup(0)
	, mPopupCallback(std::function< void(void*) >())
	, mPopupResult(0)
	, mL(L)
	, mPosSizeEventEnabled(true), mIgnoreInput(false)
	, mCachedListBox(0)
	, mModalWindow(0), mLockFocus(false), mDelayForTooltip(0)
	, mUIEditorModuleHandle(0), mLocatingComp(ComponentType::NUM)
	, mUIEditor(0), mMouseOveredContainer(0), mTextManipulator(0)
	, mUIFolder("data/ui")
	, mMultiLocating(false)
{
	gFBUIManager = gFBEnv->pUIManager = this;
	gpTimer = gFBEnv->pTimer;
	FileSystem::Initialize();
	gFBEnv->pEngine->AddInputListener(this,
		fastbird::IInputListener::INPUT_LISTEN_PRIORITY_UI, 0);
	KeyboardCursor::InitializeKeyboardCursor();
	RegisterLuaFuncs(L);	
	mUICommands = FB_NEW(UICommands);
	PrepareTooltipUI();
	WinBase::InitMouseCursor();
	gFBEnv->pEngine->RegisterFileChangeListener(this);
	gFBEnv->pRenderer->AddRenderListener(this);
	mTextManipulator = gFBEnv->pEngine->CreateTextManipulator();
}

UIManager::~UIManager()
{
	gFBEnv->pEngine->DeleteTextManipulator(mTextManipulator);
	mTextManipulator = 0;
	gFBEnv->pRenderer->RemoveRenderListener(this);
	gFBEnv->pEngine->RemoveFileChangeListener(this);
	gFBEnv->pConsole->ProcessCommand("KillUIEditor");

	WinBase::FinalizeMouseCursor();
	if (mUIEditorModuleHandle)
	{
		FreeLibrary(mUIEditorModuleHandle);
		mUIEditorModuleHandle = 0;
	}
	FB_DELETE(mUICommands);
	for (auto it : mAnimations)
	{
		FB_DELETE(it.second);
	}
	mAnimations.clear();

	DeleteWindow(mPopup);
	Shutdown();
	assert(mWindows.empty());
	gFBEnv->pEngine->RemoveInputListener(this);
	KeyboardCursor::FinalizeKeyboardCursor();
	FileSystem::Finalize();
	gFBEnv->pUIManager = 0;
}

void UIManager::Shutdown()
{
	for (auto& it : mWindows) {
		gFBEnv->pRenderer->UnregisterUIs(it.first);
		auto& windows = it.second;
		auto it = windows.begin(), itEnd = windows.end();
		for (; it != itEnd; it++){
			FB_SAFE_DEL(*it);
		}
	}
	mWindows.clear();
}

void Log(const char* szFmt, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, szFmt);
	vsprintf_s(buf, 4096, szFmt, args);
	va_end(args);
	gFBEnv->pEngine->Log(buf);
}

void Error(const char* szFmt, ...)
{
	char buf[4096];
	va_list args;
	va_start(args, szFmt);
	vsprintf_s(buf, 4096, szFmt, args);
	va_end(args);
	gFBEnv->pEngine->Error(buf);
}

//---------------------------------------------------------------------------
void UIManager::Update(float elapsedTime)
{
	for (auto& ui : mSetFocusReserved)
	{		
		auto focusRoot = ui->GetRootWnd();
		auto hwndId = focusRoot->GetHwndId();
		auto& windows = mWindows[hwndId];
		WINDOWS::iterator f = std::find(windows.begin(), windows.end(), focusRoot);
		if (f != windows.end())
		{
			// insert f at the mWindows.end().
			windows.splice(windows.end(), windows, f);
		}
		if (!ui->IsAlwaysOnTop())
		{
			for (auto& win : mAlwaysOnTopWindows)
			{
				if (!win->GetVisible())
					continue;
				WINDOWS::iterator f = std::find(windows.begin(), windows.end(), win);
				if (f != windows.end())
				{
					// insert f at the mWindows.end().
					windows.splice(windows.end(), windows, f);
				}
			}
		}
		if (!ui->GetRender3D()){
			mNeedToRegisterUIObject[hwndId] = true;
		}
	}
	mSetFocusReserved.clear();

	for (auto& ui : mMoveToBottomReserved)
	{
		auto hwndId = ui->GetHwndId();
		auto windows = mWindows[hwndId];
		WINDOWS::iterator f = std::find(windows.begin(), windows.end(), ui);
		if (f != windows.end()){
			// insert f at the mWindows.begin().
			if (windows.begin() != f){
				windows.splice(windows.begin(), windows, f);
				mNeedToRegisterUIObject[hwndId] = true;
			}
		}
	}
	mMoveToBottomReserved.clear();	

	for each(auto& it in mWindows){
		auto& windows = it.second;
		for (auto& wnd : windows){
			if (wnd->GetVisible())
				wnd->OnStartUpdate(elapsedTime);
		}
	}

	if (!mTooltipText.empty()){
		mDelayForTooltip -= elapsedTime;
		if (mDelayForTooltip <= 0){
			ShowTooltip();
		}
	}

	
}

void UIManager::BeforeUIRendering(HWND_ID hwndId)
{
	if (hwndId==1)
		mDragBox.Render();
}

void UIManager::BeforeDebugHudRendered(HWND_ID hwndId){
	if (hwndId == 1 && mUIEditor)
	{
		mUIEditor->DrawFocusBoxes();
	}
}

void UIManager::AfterDebugHudRendered(HWND_ID hwndId)
{
	/*if (hwndId == 1 && mUIEditor)
	{
		mUIEditor->DrawFocusBoxes();
	}*/

}

void UIManager::GatherRenderList()
{
	for (auto& it : mNeedToRegisterUIObject){
		if (it.second){
			HWND_ID hwndId = it.first;
			it.second = false;
			auto& windows = mWindows[hwndId];
			std::vector<IUIObject*> uiObjects;
			uiObjects.reserve(200);
			std::map<std::string, std::vector<IUIObject*>> render3DList;
			bool hideAll = !mHideUIExcepts.empty();
			WINDOWS::iterator it = windows.begin(), itEnd = windows.end();
			size_t start = 0;
			for (; it != itEnd; it++)
			{
				if (hideAll)
				{
					if (std::find(mHideUIExcepts.begin(), mHideUIExcepts.end(), (*it)->GetName()) == mHideUIExcepts.end())
						continue;
				}
				if ((*it)->GetVisible())
				{
					if ((*it)->GetRender3D())
					{
						assert(strlen((*it)->GetName()) != 0);
						auto& list = render3DList[(*it)->GetName()];
						list.reserve(100);
						(*it)->GatherVisit(list);
						std::sort(uiObjects.begin(), uiObjects.end(), [](IUIObject* a, IUIObject* b){
							return a->GetSpecialOrder() < b->GetSpecialOrder();
						});
					}
					else
					{
						(*it)->GatherVisit(uiObjects);

						std::sort(uiObjects.begin() + start, uiObjects.end(), [](IUIObject* a, IUIObject* b){
							return a->GetSpecialOrder() < b->GetSpecialOrder();
						});
						start = uiObjects.size();
					}
				}
			}


			if (mPopup&& mPopup->GetVisible())
				mPopup->GatherVisit(uiObjects);

			// rendering order : reverse.
			gFBEnv->pRenderer->RegisterUIs(hwndId, uiObjects);
			for (auto it : render3DList)
			{
				gFBEnv->pRenderer->Register3DUIs(hwndId, it.first.c_str(), it.second);
			}
			
		}
	}	
}

//---------------------------------------------------------------------------
bool UIManager::ParseUI(const char* filepath, WinBases& windows, std::string& uiname, HWND_ID hwndId, bool luaUI)
{
	LUA_STACK_CLIPPER c(mL);
	if (hwndId == INVALID_HWND_ID)
	{
		hwndId = gFBEnv->pEngine->GetMainWndHandleId();
	}
	tinyxml2::XMLDocument doc;
	int err = doc.LoadFile(filepath);
	char buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buf);
	if (err)
	{
		Error("parsing ui file(%s) failed.", filepath);
		if (doc.GetErrorStr1())
			Error(doc.GetErrorStr1());
		if (doc.GetErrorStr2())
			Error(doc.GetErrorStr2());
		return false;
	}

	tinyxml2::XMLElement* pRoot = doc.RootElement();
	if (!pRoot)
	{
		assert(0);
		return false;
	}

	if (pRoot->Attribute("name"))
		uiname = pRoot->Attribute("name");

	std::string lowerUIname = uiname;
	ToLowerCase(lowerUIname);

	auto itFind = mLuaUIs.find(lowerUIname);
	if (itFind != mLuaUIs.end())
	{
		if (GetVisible(lowerUIname.c_str()))
		{
			SetVisible(lowerUIname.c_str(), false);
		}

		for (const auto& ui : itFind->second)
		{
			DeleteWindow(ui);
		}
		mLuaUIs.erase(itFind);
	}

	std::string scriptPath;
	const char* sz = pRoot->Attribute("script");
	if (sz && strlen(sz)!=0)
	{
		int error = luaL_dofile(mL, sz);
		if (error)
		{
			char buf[1024];
			sprintf_s(buf, "\n%s/%s", GetCWD(), lua_tostring(mL, -1));
			Error(DEFAULT_DEBUG_ARG, buf);
			return false;
		}
		scriptPath = sz;
		ToLowerCase(scriptPath);
	}

	bool render3d = false;
	sz = pRoot->Attribute("render3d");
	if (sz)
	{
		render3d = StringConverter::parseBool(sz);
	}
	Vec2I renderTargetSize(100, 100);
	sz = pRoot->Attribute("renderTargetSize");
	if (sz)
	{
		renderTargetSize = StringConverter::parseVec2I(sz);
	}

	tinyxml2::XMLElement* pComp = pRoot->FirstChildElement("component");
	while (pComp)
	{
		sz = pComp->Attribute("type");
		if (!sz)
		{
			Error("Component doesn't have type attribute. ignored");
			assert(0);
			continue;
		}

		ComponentType::Enum type = ComponentType::ConvertToEnum(sz);

		IWinBase* p = AddWindow(type, hwndId);
		if (p)
		{
			if (render3d)
			{
				p->SetRender3D(true, renderTargetSize);
			}
			p->SetUIFilePath(filepath);
			if (!scriptPath.empty())
			{
				p->SetScriptPath(scriptPath.c_str());
				scriptPath.clear();
			}
			windows.push_back(p);
			p->ParseXML(pComp);
		}

		pComp = pComp->NextSiblingElement("component");
	}

	auto animElem = pRoot->FirstChildElement("Animation");
	while (animElem)
	{
		IUIAnimation* pAnim = FB_NEW(UIAnimation);
		pAnim->LoadFromXML(animElem);
		std::string name = pAnim->GetName();
		auto it = mAnimations.Find(name);
		if (it  != mAnimations.end())
		{
			FB_DELETE(it->second);
			mAnimations.erase(it);
			Log("UI global animation %s is replaced", name.c_str());
		}
		mAnimations[pAnim->GetName()] = pAnim;

		animElem = animElem->NextSiblingElement("Animation");
	}

	assert(!uiname.empty());
	if (luaUI && !uiname.empty())
	{
		mLuaUIs[lowerUIname].clear();
		for (const auto& topWindow : windows)
		{
			mLuaUIs[lowerUIname].push_back(topWindow);
		}

		for (const auto& topWindow : windows)
		{
			auto eventHandler = dynamic_cast<EventHandler*>(topWindow);
			if (eventHandler)
				eventHandler->OnEvent(UIEvents::EVENT_ON_LOADED);
		}

	}

	return true;
}

//---------------------------------------------------------------------------
void UIManager::SaveUI(const char* uiname, tinyxml2::XMLDocument& doc)
{
	std::string lower = uiname;
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower);
	if (it == mLuaUIs.end())
	{
		Error(FB_DEFAULT_DEBUG_ARG, FormatString("no ui found with the name %s", uiname));
		return;
	}
	if (it->second.empty())
	{
		Error(FB_DEFAULT_DEBUG_ARG, "doesn't have any window!");
		return;
	}
	if (it->second.size() != 1)
	{
		Error(FB_DEFAULT_DEBUG_ARG, "Only supporting an window per ui.");
		return;
	}
	auto window = it->second[0];
	tinyxml2::XMLElement* root = doc.NewElement("UI");
	doc.InsertEndChild(root);
	if (!root)
	{
		assert(0);
		return;
	}

	root->SetAttribute("name", uiname);
	root->SetAttribute("script", window->GetScriptPath());

	
	tinyxml2::XMLElement* pComp = doc.NewElement("component");
	root->InsertEndChild(pComp);
	window->Save(*pComp);
}

//---------------------------------------------------------------------------
void UIManager::CloneUI(const char* uiname, const char* newUIname)
{
	assert(uiname);
	assert(newUIname);
	std::string lower = uiname;
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower);
	if (it == mLuaUIs.end())
	{
		assert(0);
		return;
	}

	std::string strNewUIName = newUIname;
	ToLowerCase(strNewUIName);
	auto newIt = mLuaUIs.find(strNewUIName);
	if (newIt != mLuaUIs.end())
	{
		assert(0 && "already have the ui with the new name");
		return;
	}
	assert(!it->second.empty());
	const char* filepath = it->second[0]->GetUIFilePath();
	HWND_ID hwndId = it->second[0]->GetHwndId();
	LUA_STACK_CLIPPER c(mL);
	tinyxml2::XMLDocument doc;
	int err = doc.LoadFile(filepath);
	if (err)
	{
		Error("parsing ui file(%s) failed.", filepath);
		if (doc.GetErrorStr1())
			Error(doc.GetErrorStr1());
		if (doc.GetErrorStr2())
			Error(doc.GetErrorStr2());
		return;
	}

	tinyxml2::XMLElement* pRoot = doc.RootElement();
	if (!pRoot)
	{
		assert(0);
		return;
	}

	std::string scriptPath;
	const char* sz = pRoot->Attribute("script");
	if (sz)
	{
		Error(DEFAULT_DEBUG_ARG, "cannot clone scriptable ui");
		return;
	}

	bool render3d = false;
	sz = pRoot->Attribute("render3d");
	if (sz)
	{
		render3d = StringConverter::parseBool(sz);
	}
	Vec2I renderTargetSize(100, 100);
	sz = pRoot->Attribute("renderTargetSize");
	if (sz)
	{
		renderTargetSize = StringConverter::parseVec2I(sz);
	}

	tinyxml2::XMLElement* pComp = pRoot->FirstChildElement("component");
	unsigned idx = 0;
	WinBases windows;
	while (pComp)
	{
		sz = pComp->Attribute("type");
		if (!sz)
		{
			Error("Component doesn't have type attribute. ignored");
			assert(0);
			continue;
		}

		ComponentType::Enum type = ComponentType::ConvertToEnum(sz);

		IWinBase* p = AddWindow(type, hwndId);
		if (p)
		{
			if (render3d)
			{
				p->SetRender3D(true, renderTargetSize);
			}
			p->SetUIFilePath(filepath);
			if (!scriptPath.empty())
			{
				p->SetScriptPath(scriptPath.c_str());
				scriptPath.clear();
			}
			p->ParseXML(pComp);
			windows.push_back(p);
			if (idx == 0)
			{
				p->SetName(newUIname);
			}
			idx++;
		}

		pComp = pComp->NextSiblingElement("component");
	}

	auto& uis = mLuaUIs[strNewUIName];
	for (const auto& topWindow : windows)
	{
		uis.push_back(topWindow);
	}

	for (const auto& topWindow : windows)
	{
		auto eventHandler = dynamic_cast<EventHandler*>(topWindow);
		if (eventHandler)
			eventHandler->OnEvent(UIEvents::EVENT_ON_LOADED);
	}
}

//---------------------------------------------------------------------------
void UIManager::IgnoreInput(bool ignore, IWinBase* modalWindow)
{
	ignore ? mIgnoreInput++ : mIgnoreInput--;
	mModalWindow = modalWindow;
}

void UIManager::ToggleVisibleLuaUI(const char* uiname)
{
	bool visible = GetVisible(uiname);
	visible = !visible;
	SetVisible(uiname, visible);
}

//---------------------------------------------------------------------------
bool UIManager::AddLuaUI(const char* uiName, LuaObject& data, HWND_ID hwndId)
{
	if (hwndId == INVALID_HWND_ID)
	{
		hwndId = gFBEnv->pEngine->GetMainWndHandleId();
	}
	std::string lower = uiName;
	ToLowerCase(lower);
	auto it = mLuaUIs.find(uiName);
	if (it != mLuaUIs.end())
	{
		Error("Already registered!");
		return false;
	}
	std::string typeText = data.GetField("type_").GetString();
	auto type = ComponentType::ConvertToEnum(typeText.c_str());
	
	IWinBase* p = AddWindow(0.f, 0.f, 0.1f, 0.1f, type, hwndId);
	assert(p);
	p->ParseLua(data);

	mLuaUIs[lower].push_back(p);
	return p!=0;
}

void UIManager::DeleteLuaUI(const char* uiName)
{
	std::string lower = uiName;
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower);
	if (it != mLuaUIs.end())
	{
		for (auto& win : it->second)
		{
			DirtyRenderList(win->GetHwndId());
			DeleteWindow(win);			
		}
		mLuaUIs.erase(it);
	}
	
}

bool UIManager::IsLoadedUI(const char* uiName)
{
	std::string lower = uiName;
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower);
	return it != mLuaUIs.end();
}

//---------------------------------------------------------------------------
IWinBase* UIManager::AddWindow(int posX, int posY, int width, int height, ComponentType::Enum type, HWND_ID hwndId)
{
	assert(hwndId != 0);
	if (hwndId == INVALID_HWND_ID)
	{
		hwndId = gFBEnv->pEngine->GetMainWndHandleId();
	}	

	IWinBase* pWnd = CreateComponent(type);	
	if (pWnd !=0)
	{
		pWnd->SetHwndId(hwndId);
		auto& windows = mWindows[hwndId];
		windows.push_back(pWnd);
		pWnd->ChangeSize(Vec2I(width, height));
		pWnd->ChangePos(Vec2I(posX, posY));
		mNeedToRegisterUIObject[hwndId] = true;
		pWnd->OnCreated();
	}
	
	return pWnd;
}

IWinBase* UIManager::AddWindow(const Vec2I& pos, const Vec2I& size, ComponentType::Enum type, HWND_ID hwndId)
{
	return AddWindow(pos.x, pos.y, size.x, size.y, type, hwndId);
}

IWinBase* UIManager::AddWindow(float posX, float posY, float width, float height, ComponentType::Enum type, HWND_ID hwndId)
{
	assert(hwndId != 0);
	if (hwndId == INVALID_HWND_ID)
	{
		hwndId = gFBEnv->pEngine->GetMainWndHandleId();
	}
	IWinBase* pWnd = CreateComponent(type);
	if (pWnd!=0)
	{
		pWnd->SetHwndId(hwndId);
		auto& windows = mWindows[hwndId];
		windows.push_back(pWnd);
		pWnd->ChangeNSize(Vec2(width, height));
		pWnd->ChangeNPos(Vec2(posX, posY));
		mNeedToRegisterUIObject[hwndId] = true;
		pWnd->OnCreated();
	}
	return pWnd;
}

IWinBase* UIManager::AddWindow(ComponentType::Enum type, HWND_ID hwndId)
{
	assert(hwndId != 0);
	if (hwndId == INVALID_HWND_ID)
	{
		hwndId = gFBEnv->pEngine->GetMainWndHandleId();
	}

	IWinBase* pWnd = CreateComponent(type);

	if (pWnd != 0)
	{
		pWnd->SetHwndId(hwndId);
		auto& windows = mWindows[hwndId];
		windows.push_back(pWnd);
		mNeedToRegisterUIObject[hwndId] = true;
		pWnd->OnCreated();
	}
	
	return pWnd;
}

void UIManager::DeleteWindow(IWinBase* pWnd)
{
	if (!pWnd)
		return;
	if (pWnd == mKeyboardFocus)
	{
		mKeyboardFocus = 0;
	}
	OnDeleteWinBase(pWnd);
	auto hwndId = pWnd->GetHwndId();
	assert(hwndId != -1);
	auto& windows = mWindows[hwndId];
	windows.erase(std::remove(windows.begin(), windows.end(), pWnd), windows.end());
	FB_SAFE_DEL(pWnd);
	DirtyRenderList(hwndId);
}

void UIManager::DeleteWindowsFor(HWND_ID hwndId)
{
	auto it = mWindows.Find(hwndId);
	if (it == mWindows.end())
		return;
	auto& windows = it->second;
	for (auto wnd : windows)
	{
		OnDeleteWinBase(wnd);
		DeleteLuaUIContaning(wnd);
		FB_SAFE_DEL(wnd);
	}
	windows.clear();
	mWindows.erase(it);
	DirtyRenderList(hwndId);
}

void UIManager::DeleteLuaUIContaning(IWinBase* wnd)
{
	for (auto it = mLuaUIs.begin(); it != mLuaUIs.end(); ++it)
	{
		auto& uis = it->second;
		for (auto ui : uis)
		{
			if (ui == wnd)
			{
				mLuaUIs.erase(it);
				return;
			}
		}
	}
}

// deleting component or wnd
void UIManager::OnDeleteWinBase(IWinBase* winbase)
{
	if (!winbase)
		return;

	if (mUIEditor)
	{
		mUIEditor->OnComponentDeleted(winbase);
	}

	if (winbase->GetFocus(true)){
		mFocusWnd = 0;
		mKeyboardFocus = 0;
	}
	
	if (mMouseOveredContainer == winbase)
		mMouseOveredContainer = 0;

	if (winbase->GetRender3D())
	{
		gFBEnv->pRenderer->Unregister3DUIs(winbase->GetName());
	}
}

void UIManager::SetFocusUI(IWinBase* ui)
{
	for (auto& reservedUI : mSetFocusReserved)
	{
		if (ui == reservedUI)
			return;
	}

	if (mKeyboardFocus)
		mKeyboardFocus->OnFocusLost();
	if (mFocusWnd)
		mFocusWnd->OnFocusLost();

	auto focusRoot = ui ? ui->GetRootWnd() : 0;

	mFocusWnd = focusRoot;
	mKeyboardFocus = ui;

	if (mFocusWnd)
		mFocusWnd->OnFocusGain();
	if (mKeyboardFocus)
		mKeyboardFocus->OnFocusGain();
	if (ui && ValueNotExistInVector(mSetFocusReserved, ui))
		mSetFocusReserved.push_back(ui);
}

IWinBase* UIManager::GetFocusUI() const
{
	return mFocusWnd;
}

IWinBase* UIManager::GetKeyboardFocusUI() const
{
	return mKeyboardFocus;
}

void UIManager::SetFocusUI(const char* uiName)
{
	if (!uiName)
		return;
	std::string lower(uiName);
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower);
	if (it == mLuaUIs.end())
	{
		Error("Cannot find ui with a name, %s", uiName);
		return;
	}
	if (it->second.empty())
	{
		Error("UI doesn't have any elements, %s", uiName);
		return;
	}

	for (auto& ui : it->second)
	{
		SetFocusUI(ui);
		break;
	}	
}

bool UIManager::IsFocused(const IWinBase* pWnd) const
{
	return pWnd == mFocusWnd || pWnd == mKeyboardFocus;
}

void UIManager::DirtyRenderList(HWND_ID hwndId)
{
	if (hwndId == INVALID_HWND_ID){
		for (auto& it : mNeedToRegisterUIObject)
		{
			it.second = true;
		}
	}
	else{
		mNeedToRegisterUIObject[hwndId] = true;
	}
}

void UIManager::SetUIProperty(const char* uiname, const char* compname, const char* prop, const char* val)
{
	SetUIProperty(uiname, compname, UIProperty::ConvertToEnum(prop), val);
}
void UIManager::SetUIProperty(const char* uiname, const char* compname, UIProperty::Enum prop, const char* val)
{
	if_assert_fail(uiname && compname && val)
		return;

	auto comp = FindComp(uiname, compname);
	if (comp)
	{
		comp->SetProperty(prop, val);
	}
	else
	{
		Error("Cannot find ui comp(%s) in ui(%s) to set uiproperty(%s).", compname, uiname, UIProperty::ConvertToString(prop));
	}
}

//---------------------------------------------------------------------------
IWinBase* UIManager::CreateComponent(ComponentType::Enum type)
{
	IWinBase* pWnd = 0;
	switch(type)
	{
	case ComponentType::Window:
		pWnd = FB_NEW(Wnd);
		break;
	case ComponentType::TextField:
		pWnd = FB_NEW(TextField);
		break;
	case ComponentType::StaticText:
		pWnd = FB_NEW(StaticText);
		break;
	case ComponentType::Button:
		pWnd = FB_NEW(Button);
		break;
	case ComponentType::ImageBox:
		pWnd = FB_NEW(ImageBox);
		break;
	case ComponentType::CheckBox:
		pWnd = FB_NEW(CheckBox);
		break;
	case ComponentType::ListBox:
		pWnd = FB_NEW(ListBox);
		break;
	case ComponentType::ListItem:
		pWnd = FB_NEW(ListItem);
		break;
	case ComponentType::FileSelector:
		pWnd = FB_NEW(FileSelector);
		break;
	case ComponentType::Scroller:
		pWnd = FB_NEW(Scroller);
		break;
	case ComponentType::RadioBox:
		pWnd = FB_NEW(RadioBox);
		break;
	case ComponentType::Hexagonal:
		pWnd = FB_NEW(HexagonalContextMenu);
		break;
	case ComponentType::CardScroller:
		pWnd = FB_NEW(CardScroller);
		break;
	case ComponentType::CardItem:
		pWnd = FB_NEW(CardItem);
		break;
	case ComponentType::VerticalGauge:
		pWnd = FB_NEW(VerticalGauge);
		break;
	case ComponentType::HorizontalGauge:
		pWnd = FB_NEW(HorizontalGauge);
		break;
	case ComponentType::NumericUpDown:
		pWnd = FB_NEW(NumericUpDown);
		break;
	case ComponentType::DropDown:
		pWnd = FB_NEW(DropDown);
		break;
	case ComponentType::TextBox:
		pWnd = FB_NEW(TextBox);
		break;
	case ComponentType::ColorRamp:
		pWnd = FB_NEW(ColorRampComp);
		break;
	case ComponentType::NamedPortrait:
		pWnd = FB_NEW(NamedPortrait);
		break;
	case ComponentType::PropertyList:
		pWnd = FB_NEW(PropertyList);
		break;
	case ComponentType::TabWindow:
		pWnd = FB_NEW(TabWindow);
		break;
	default:
		assert(0 && "Unknown component");
	}
	return pWnd;
}

//---------------------------------------------------------------------------
void UIManager::DeleteComponent(IWinBase* com)
{
	OnDeleteWinBase(com);
	if (mKeyboardFocus == com)
		mKeyboardFocus = 0;
	FB_DELETE(com);
}

//---------------------------------------------------------------------------
void UIManager::OnInput(IMouse* pMouse, IKeyboard* keyboard)
{
	if (gFBEnv->pConsole->GetEngineCommand()->UI_Debug){
		DebugUI();
	}

	if (!pMouse->IsValid() && !keyboard->IsValid())
		return;

	if (mUIEditor && gFBEnv->pEngine->IsMainWindowForground())
	{
		if (mLocatingComp != ComponentType::NUM)
		{
			if (mMultiLocating  && mLocatingComp != ComponentType::NUM && !keyboard->IsKeyDown(VK_SHIFT))
			{
				mLocatingComp = ComponentType::NUM;
				mUIEditor->OnCancelComponent();
				mMultiLocating = false;
			}
			else{
				OnInputForLocating(pMouse, keyboard);

				return;
			}
		}
	}

	auto hwndId = gFBEnv->pEngine->GetForegroundWindowId();
	auto windows = mWindows[hwndId];

	if (keyboard->IsValid() && keyboard->IsKeyPressed(VK_ESCAPE)) {
		WINDOWS::reverse_iterator it = windows.rbegin(), itEnd = windows.rend();
		int i = 0;
		for (; it != itEnd; ++it) {
			if ((*it)->GetVisible() && (*it)->GetCloseByEsc()) {
				(*it)->SetVisible(false);
				keyboard->Invalidate();
				break;
			}
		}
	}
	
	if (mUIEditor){
		if (gFBEnv->pEngine->IsMainWindowForground()) {
			if (keyboard->IsValid() && keyboard->IsKeyPressed(VK_DELETE)) {
				mUIEditor->TryToDeleteCurComp();
				keyboard->Invalidate();
			}
			DragUI();
			mUIEditor->ProcessKeyInput();
		}
	}

	if (mIgnoreInput) {
		if (mModalWindow)
			mModalWindow->OnInputFromHandler(pMouse, keyboard);
		return;
	}

	mMouseIn = false;
	WINDOWS::reverse_iterator it = windows.rbegin(), itEnd = windows.rend();
	int i = 0;
	for (; it != itEnd; ++it)
	{
		if ((*it)->GetVisible())
		{
			mMouseIn = (*it)->OnInputFromHandler(pMouse, keyboard) || mMouseIn;
		}

		if (!pMouse->IsValid() && !keyboard->IsValid())
			break;
	}

	//Select
	if (pMouse->IsLButtonClicked()) {
		auto mousePos = pMouse->GetPos();
		RegionTestParam rparam;
		rparam.mOnlyContainer = false;
		rparam.mIgnoreScissor = mUIEditor ? true : false;
		rparam.mTestChildren = true;
		auto focusWnd = WinBaseWithPoint(mousePos, rparam);
		if (mKeyboardFocus != focusWnd)
		{
			SetFocusUI(focusWnd);
		}
		if (mUIEditor)
		{
			if (!keyboard->IsKeyDown(VK_MENU)){
				if (mUIEditor->GetCurSelected() != mKeyboardFocus || keyboard->IsKeyDown(VK_SHIFT) || mUIEditor->GetNumCurEditing() > 1)
					mUIEditor->OnComponentSelected(mKeyboardFocus);
			}
		}
	}

	if (keyboard->GetChar() == VK_TAB)
	{
		if (gFBUIManager->GetKeyboardFocusUI())
		{
			keyboard->PopChar();
			gFBUIManager->GetKeyboardFocusUI()->TabPressed();
		}
	}


	if (mMouseIn && EventHandler::sLastEventProcess != gpTimer->GetFrame() && pMouse->IsLButtonClicked())
	{		
		LuaObject mouseInvalided;
		mouseInvalided.FindFunction(gFBEnv->pUIManager->GetLuaState(), "OnMouseInvalidatedInUI");
		if (mouseInvalided.IsValid())
		{
			mouseInvalided.Call();
		}
	}
}

void UIManager::EnableInputListener(bool enable)
{
	mInputListenerEnable = enable;
}
bool UIManager::IsEnabledInputLIstener() const
{
	return mInputListenerEnable;
}

HCURSOR UIManager::GetMouseCursorOver() const
{
	return WinBase::GetMouseCursorOver();
}

void UIManager::SetMouseCursorOver()
{
	SetCursor(WinBase::GetMouseCursorOver());
}

void UIManager::DisplayMsg(const std::string& msg, ...)
{
	char buf[2048] = {0};
	va_list args;
	va_start(args, msg);
	vsprintf_s(buf, 2048, msg.c_str(), args);
	va_end(args);

	if (strlen(buf)>0)
	{
		Log(buf);
		if (gFBEnv->pRenderer)
		{
			gFBEnv->pRenderer->DrawTextForDuration(4.0f, Vec2I(100, 200), 
				buf, fastbird::Color::White);
		}
	}
}

void UIManager::SetTooltipString(const std::wstring& ts)
{
	if (mTooltipText == ts)
	{
		return;
	}
	mTooltipText = ts;	
	HWND_ID hwndId = gFBEnv->pEngine->GetWindowHandleIdWithMousePoint();
	if (hwndId != INVALID_HWND_ID)
		mTooltipUI->SetHwndId(gFBEnv->pEngine->GetForegroundWindowId());

	if (mTooltipText.empty())
	{
		mTooltipUI->SetVisible(false);
	}
	else
	{
		if (mTooltipUI->GetVisible())
		{
			ShowTooltip();
		}
		else
		{
			mDelayForTooltip = gDelayForTooltip;
		}
	}
}

void UIManager::ShowTooltip()
{
	assert(!mTooltipText.empty());	
	IFont* pFont = gFBEnv->pRenderer->GetFont();
	pFont->SetHeight(gTooltipFontSize);
	int width = (int)gFBEnv->pRenderer->GetFont()->GetTextWidth(
		(const char*)mTooltipText.c_str(), mTooltipText.size() * 2);
	pFont->SetBackToOrigHeight();

	const int maxWidth = 350;
	width = std::min(maxWidth, width) + 4;
	mTooltipUI->ChangeSizeX(width + 16);
	mTooltipTextBox->ChangeSizeX(width);
	mTooltipTextBox->SetText(mTooltipText.c_str());
	int textWidth = mTooltipTextBox->GetTextWidth();
	mTooltipUI->ChangeSizeX(textWidth + 16);
	mTooltipTextBox->ChangeSizeX(textWidth + 4);
	int sizeY = mTooltipTextBox->GetPropertyAsInt(UIProperty::SIZEY);
	mTooltipUI->ChangeSizeY(sizeY + 8);
	mTooltipUI->SetVisible(true);
}

void UIManager::SetTooltipPos(const Vec2& npos)
{
	if (!mTooltipUI->GetVisible())
		return;

	HWND_ID hwndId = mTooltipUI->GetHwndId();
	auto hWnd = gFBEnv->pEngine->GetWindowHandle(hwndId);
	assert(hwndId != -1);
	const auto& size = gFBEnv->pEngine->GetRequestedWndSize(hWnd);
	Vec2 backPos = npos;
	if (backPos.y > 0.9f)
	{
		backPos.y -= (gTooltipFontSize * 2.0f + 10) / (float)size.y;
	}
	const Vec2& nSize = mTooltipUI->GetNSize();
	if (backPos.x + nSize.x>1.0f)
	{
		backPos.x -= backPos.x + nSize.x - 1.0f;
	}
	backPos.y += gTooltipFontSize / (float)size.y;
	mTooltipUI->ChangeNPos(backPos);
}

void UIManager::CleanTooltip()
{
	if (!mTooltipText.empty())
	{
		mTooltipText.clear();
		mTooltipUI->SetVisible(false);
	}
}

void UIManager::PopupDialog(WCHAR* msg, POPUP_TYPE type, std::function< void(void*) > func)
{
	if (!mPopup)
	{
		mPopup = CreateComponent(ComponentType::Window);
		mPopup->SetHwndId(gFBEnv->pEngine->GetMainWndHandleId());
		mPopup->ChangeNPos(Vec2(0.5f, 0.5f));
		mPopup->ChangeNSize(Vec2(0.3f, 0.1f));
		mPopup->SetProperty(UIProperty::ALIGNH, "center");
		mPopup->SetProperty(UIProperty::ALIGNV, "middel");
		mPopup->SetProperty(UIProperty::TEXT_ALIGN, "center");
		mPopup->SetProperty(UIProperty::TEXT_VALIGN, "middle");
		mPopup->SetText(msg);

		WinBase* yes = (WinBase*)mPopup->AddChild(0.49f, 0.99f, 0.25f, 0.1f, ComponentType::Button);
		yes->SetRuntimeChild(true);
		yes->SetName("yes");
		yes->SetAlign(ALIGNH::RIGHT, ALIGNV::BOTTOM);
		yes->SetText(L"Yes");
		yes->RegisterEventFunc(UIEvents::EVENT_MOUSE_LEFT_CLICK,
			std::bind(&UIManager::OnPopupYes, this, std::placeholders::_1));

		WinBase* no = (WinBase*)mPopup->AddChild(0.51f, 0.99f, 0.25f, 0.1f, ComponentType::Button);
		no->SetRuntimeChild(true);
		no->SetName("no");
		no->SetAlign(ALIGNH::LEFT, ALIGNV::BOTTOM);
		no->SetText(L"No");
		no->RegisterEventFunc(UIEvents::EVENT_MOUSE_LEFT_CLICK,
			std::bind(&UIManager::OnPopupNo, this, std::placeholders::_1));
	}
	
	mPopupCallback = func;
	mPopup->SetVisible(true);
}

void UIManager::OnPopupYes(void* arg)
{
	assert(mPopup);
	mPopupResult = 1;
	mPopup->SetVisible(false);
	mPopupCallback(this);
}

void UIManager::OnPopupNo(void* arg)
{
	assert(mPopup);
	mPopupResult = 0;
	mPopup->SetVisible(false);
	mPopupCallback(this);
}

IWinBase* UIManager::FindComp(const char* uiname, const char* compName) const
{
	assert(uiname);
	std::string lower(uiname);
	ToLowerCase(lower);
	auto itFind = mLuaUIs.find(lower);
	if (itFind == mLuaUIs.end())
		return 0;

	if ((compName == 0 || strlen(compName) == 0) && !itFind->second.empty())
		return itFind->second[0];

	for (const auto& comp : itFind->second)
	{
		if (strcmp(comp->GetName(), compName) == 0)
		{
			return comp;
		}

		auto ret = comp->GetChild(compName, true);
		if (ret)
			return ret;
	}

	return 0;
}

void UIManager::FindUIWnds(const char* uiname, WinBases& outV) const
{
	assert(uiname);
	std::string lower(uiname);
	ToLowerCase(lower);
	auto itFind = mLuaUIs.find(lower);
	if (itFind == mLuaUIs.end())
		return;

	for (const auto& comp : itFind->second)
	{
		if (strcmp(comp->GetName(), uiname) == 0)
		{
			outV.push_back(comp);
		}
	}
}

bool UIManager::CacheListBox(const char* uiname, const char* compName)
{
	mCachedListBox = dynamic_cast<ListBox*>(FindComp(uiname, compName));
	return mCachedListBox != 0;
}

void UIManager::SetVisible(const char* uiname, bool visible)
{
	assert(uiname);
	std::string lower(uiname);
	ToLowerCase(lower);
	auto itFind = mLuaUIs.find(lower);
	if (itFind == mLuaUIs.end())
	{
		Error("UIManager::SetVisible(): UI(%s) is not found.", uiname);
		return;
	}
	for (const auto& comp : itFind->second)
	{
		comp->SetVisible(visible);
	}


}

void UIManager::LockFocus(bool lock)
{
	mLockFocus = lock;
}

bool UIManager::GetVisible(const char* uiname) const
{
	assert(uiname);
	std::string lower(uiname);
	ToLowerCase(lower);
	auto itFind = mLuaUIs.find(lower.c_str());
	if (itFind == mLuaUIs.end())
	{
		return false;
	}
	bool visible = false;
	for (const auto& comp : itFind->second)
	{
		visible = visible || comp->GetVisible();
	}
	return visible;
}

void UIManager::CloseAllLuaUI()
{
	for (auto& it : mLuaUIs)
	{
		for (auto& ui : it.second)
		{
			ui->SetVisible(false);
		}
	}
}

const char* UIManager::FindUIFilenameWithLua(const char* luafilepath)
{
	for (const auto& it : mLuaUIs)
	{
		const auto& wins = it.second;
		for (const auto& win : wins)
		{
			if (strcmp(win->GetScriptPath(), luafilepath) == 0)
			{
				return win->GetUIFilePath();
			}
		}
	}
	return "";
}

const char* UIManager::FindUINameWithLua(const char* luafilepath)
{
	for (const auto& it : mLuaUIs)
	{
		const auto& wins = it.second;
		for (const auto& win : wins)
		{
			if (_stricmp(win->GetScriptPath(), luafilepath) == 0)
			{
				return it.first.c_str();
			}
		}
	}
	return "";
}

bool UIManager::OnFileChanged(const char* file)
{
	assert(file);
	std::string lower(file);
	ToLowerCase(lower);

	auto extension = GetFileExtension(lower.c_str());
	std::string uiname;
	std::string filepath = lower;
	if (strcmp(extension, "lua") == 0)
	{
		uiname = FindUINameWithLua(lower.c_str());
		filepath = FindUIFilenameWithLua(lower.c_str());
	}
	else if (strcmp(extension, "ui") == 0)
	{
		uiname = GetFileNameWithoutExtension(lower.c_str());
	}
	else
		return false;

	if (uiname.empty())
	{
		LuaObject loadUIFunc;
		loadUIFunc.FindFunction(mL, "LoadLuaUI");
		loadUIFunc.PushToStack();
		auto uiFilePath = ReplaceExtension(file, "ui");
		if (FileSystem::IsFileExisting(uiFilePath.c_str()))
		{
			lua_pushstring(mL, uiFilePath.c_str());
			loadUIFunc.CallWithManualArgs(1, 0);
			uiname = GetFileNameWithoutExtension(uiFilePath.c_str());
			SetVisible(uiname.c_str(), true);
		}
		else if (strcmp(extension, "lua") == 0)
		{
			int error = luaL_dofile(mL, file);
			if (error)
			{
				char buf[1024];
				sprintf_s(buf, "\n%s/%s", GetCWD(), lua_tostring(mL, -1));
				Error(DEFAULT_DEBUG_ARG, buf);
				assert(0);
			}
		}
		return true;
	}
		
	HWND_ID hwndId = -1;
	auto itFind = mLuaUIs.find(uiname);
	if (itFind != mLuaUIs.end())
	{
		if (GetVisible(uiname.c_str()))
		{
			SetVisible(uiname.c_str(), false);
		}

		auto& windows = itFind->second;
		if (!windows.empty())
		{
			hwndId = windows[0]->GetHwndId();
			while (!windows.empty()){
				DeleteWindow(windows.back());
				windows.pop_back();
			}			
		}
		mLuaUIs.erase(itFind);
	}
	std::vector<IWinBase*> temp;
	std::string name;
	UIManager::GetUIManagerStatic()->ParseUI(filepath.c_str(), temp, name, hwndId, true);
	mLuaUIs[uiname] = temp;
	UIManager::GetUIManagerStatic()->SetVisible(uiname.c_str(), false);
	UIManager::GetUIManagerStatic()->SetVisible(uiname.c_str(), true); // for OnVisible UI Event.
	return true;
}

UIManager* UIManager::GetUIManagerStatic()
{
	return (UIManager*)gFBEnv->pUIManager;
}

void UIManager::RegisterAlwaysOnTopWnd(IWinBase* win)
{
	if (ValueNotExistInVector(mAlwaysOnTopWindows, win))
		mAlwaysOnTopWindows.push_back(win);
}

void UIManager::UnRegisterAlwaysOnTopWnd(IWinBase* win)
{
	DeleteValuesInVector(mAlwaysOnTopWindows, win);
}

void UIManager::MoveToBottom(const char* moveToBottom)
{
	auto ui = FindComp(moveToBottom, 0);
	if (ui)
	{
		if (ValueNotExistInVector(mMoveToBottomReserved, ui))
			mMoveToBottomReserved.push_back(ui);
	}	
}

void UIManager::HideUIsExcept(const std::vector<std::string>& excepts)
{
	mHideUIExcepts = excepts;
	DirtyRenderList(INVALID_HWND_ID);
}


void UIManager::HighlightUI(const char* uiname)
{
	WinBases topWnds;
	FindUIWnds(uiname, topWnds);
	for (auto& it : topWnds)
	{
		it->StartHighlight(2.0f);
	}
}

void UIManager::StopHighlightUI(const char* uiname)
{
	WinBases topWnds;
	FindUIWnds(uiname, topWnds);
	for (auto& it : topWnds)
	{
		it->StopHighlight();
	}
}

IUIAnimation* UIManager::GetGlobalAnimation(const char* animName)
{
	auto it = mAnimations.Find(animName);
	if (it == mAnimations.end())
		return 0;

	return it->second;
}

IUIAnimation* UIManager::GetGlobalAnimationOrCreate(const char* animName)
{
	auto anim = GetGlobalAnimation(animName);
	if (!anim)
	{
		anim = FB_NEW(UIAnimation);
		anim->SetName(animName);
		mAnimations.Insert(std::make_pair(animName, anim));
	}
	return anim;
}

void UIManager::PrepareTooltipUI()
{
	LuaObject tooltip;
	tooltip.NewTable(mL);
	tooltip.SetField("name", "MouseTooltip");
	tooltip.SetField("type_", "Window");
	tooltip.SetField("pos", Vec2I(0, 0));
	tooltip.SetField("size", Vec2I(366, (int)gTooltipFontSize+8));
	tooltip.SetField("HIDE_ANIMATION", "1");
	tooltip.SetField("SHOW_ANIMATION", "1");
	tooltip.SetField("NO_BACKGROUND", "false");
	tooltip.SetField("BACK_COLOR", "0, 0, 0, 0.8f");
	tooltip.SetField("ALWAYS_ON_TOP", "true");
	tooltip.SetField("USE_BORDER", "true");
	auto tooltipChildren = tooltip.SetFieldTable("children");
	auto children1 = tooltipChildren.SetSeqTable(1);
	children1.SetField("name", "TooltipTextBox");
	children1.SetField("type_", "TextBox");
	children1.SetField("pos", Vec2I(6, 4));
	children1.SetField("size", Vec2I(350, (int)gTooltipFontSize));
	children1.SetField("TEXTBOX_MATCH_HEIGHT", "true");
	children1.SetField("TEXT_SIZE", StringConverter::toString(gTooltipFontSize).c_str());	
	bool success = AddLuaUI("MouseTooltip", tooltip, gFBEnv->pEngine->GetMainWndHandleId());
	if (!success)
	{
		Error(DEFAULT_DEBUG_ARG, "Cannot create MouseTooltip UI.");
	}
	else
	{
		WinBases wnds;
		FindUIWnds("MouseTooltip", wnds);
		assert(!wnds.empty());
		mTooltipUI = wnds[0];
		mTooltipTextBox = FindComp("MouseTooltip", "TooltipTextBox");
		assert(mTooltipTextBox);
	}
}

//-------------------------------------------------------------------
// For UI Editing
//-------------------------------------------------------------------
void UIManager::SetUIEditor(IUIEditor* editor)
{
	mUIEditor = editor;
}

void UIManager::StartLocatingComponent(ComponentType::Enum c)
{
	mLocatingComp = c;
}

void UIManager::CancelLocatingComponent()
{

}

void UIManager::OnInputForLocating(IMouse* pMouse, IKeyboard* keyboard)
{
	if (mLocatingComp == ComponentType::NUM)
		return;

	if (!gFBEnv->pEngine->IsMainWindowForground())
		return;	

	long x, y;
	pMouse->GetPos(x, y);
	Vec2I pt(x, y);
	RegionTestParam rparam;
	rparam.mOnlyContainer = true;
	rparam.mIgnoreScissor = true;
	rparam.mTestChildren = true;
	mMouseOveredContainer = WinBaseWithPoint(pt, rparam);
	if (pMouse->IsLButtonDown())
	{
		if (!mDragBox.IsStarted())
		{
			mDragBox.Start(pt);
			mDragBox.SetMouseOveredContainer(mMouseOveredContainer);
		}
		else
		{
			mDragBox.PushCur(pt);
		}
	}
	else if (mDragBox.IsStarted())
	{
		mDragBox.End(pt);
		if (mLocatingComp!= ComponentType::NUM)
		{
			LocateComponent();
		}
		mDragBox.SetMouseOveredContainer(0);
	}

	if (pMouse->IsRButtonClicked())
	{
		mDragBox.End(pt);
		mLocatingComp = ComponentType::NUM;
		if (mUIEditor)
			mUIEditor->OnCancelComponent();
	}
}

IWinBase* UIManager::WinBaseWithPoint(const Vec2I& pt, const RegionTestParam& param)
{
	auto hwndId = gFBEnv->pEngine->GetForegroundWindowId();
	auto windows = mWindows[hwndId];
	for (auto wnd : windows)
	{
		if (!wnd->GetVisible()){			
			continue;
		}
		auto found = wnd->WinBaseWithPoint(pt, param);
		if (found)
			return found;

		if (wnd->IsIn(pt, param.mIgnoreScissor))
			return wnd;
	}

	return 0;
}

void UIManager::LocateComponent()
{
	if (mLocatingComp == ComponentType::NUM)
		return;

	if (!gFBEnv->pEngine->IsMainWindowForground())
		return;

	auto start = mDragBox.GetMin();
	auto end = mDragBox.GetMax();
	auto size = end - start;
	if (size.x == 0 || size.y == 0)
		return;

	auto cont = (Container*)mDragBox.GetMouseOveredContainer();
	IWinBase* win = 0;
	if (cont)
	{
		Vec2I pos = cont->GetWPos();
		pos = start - pos;
		win = cont->AddChild(pos, size, mLocatingComp);
		win->ChangeWPos(start);
	}
	else
	{
		std::string uiname = GetUniqueUIName();
		ToLowerCase(uiname);
		win = AddWindow(start, size, mLocatingComp);
		win->SetName(uiname.c_str());
		mLuaUIs[uiname].push_back(win);
		win->SetUIFilePath(FormatString("%s/%s.ui", mUIFolder.c_str(),
			uiname.c_str()));
		win->SetSaveNameCheck(true);
	}
	
	win->SetProperty(UIProperty::NO_BACKGROUND, "false");
	if (mLocatingComp == ComponentType::Window)
		win->SetProperty(UIProperty::BACK_COLOR, "0.3, 0.3, 0.5, 0.4");
	win->SetProperty(UIProperty::VISIBLE, "true");
	auto keyboard = gFBEnv->pEngine->GetKeyboard();
	if (!keyboard->IsKeyDown(VK_SHIFT))
	{
		mLocatingComp = ComponentType::NUM;
		if (mUIEditor)
		{
			mUIEditor->OnComponentSelected(win);
			mUIEditor->OnCancelComponent();
		}
	}
	else
	{
		mMultiLocating = true;
	}
	
}


const char* UIManager::GetUIPath(const char* uiname) const
{
	std::string lower = GetFileNameWithoutExtension(uiname);
	ToLowerCase(lower);

	auto it = mLuaUIs.find(lower.c_str());
	if (it == mLuaUIs.end())
	{
		Error(FB_DEFAULT_DEBUG_ARG, FormatString("cannot find the ui %s", uiname));
		return "";
	}
	auto& windows = it->second;
	if (windows.empty())
		return "";
	return windows[0]->GetUIFilePath();
}
const char*  UIManager::GetUIScriptPath(const char* uiname) const
{
	std::string lower(uiname);
	ToLowerCase(lower);
	auto it = mLuaUIs.find(lower.c_str());
	if (it == mLuaUIs.end())
		return "";
	auto& windows = it->second;
	if (windows.empty())
		return "";
	return windows[0]->GetScriptPath();
}

void UIManager::SuppressPropertyWarning(bool suppress){
	WinBase::SuppressPropertyWarning(suppress);
}

std::string UIManager::GetUniqueUIName() const
{
	std::string candi = "UI1";
	unsigned index = 1;
	do{
		std::string lower(candi);
		ToLowerCase(lower);
		auto itFind = mLuaUIs.find(lower);
		if (itFind == mLuaUIs.end()){
			if (!FileSystem::IsFileExisting(
				FormatString("%s/%s.ui", mUIFolder.c_str(), candi.c_str()))){
				break;
			}
		}
		index += 1;
		candi = FormatString("UI%u", index);
	} while (99999999);

	return candi;
}

void UIManager::ChangeFilepath(IWinBase* root, const char* newfile)
{
	auto name = GetFileNameWithoutExtension(root->GetUIFilePath());
	auto it = mLuaUIs.find(name);
	if (it == mLuaUIs.end())
	{
		Error(FB_DEFAULT_DEBUG_ARG, FormatString("Cannot find the ui %s", name.c_str()));
		return;
	}
	auto newName = GetFileNameWithoutExtension(newfile);
	if (name != newName)
	{
		std::string lowerNewName(newName);
		ToLowerCase(lowerNewName);
		auto newIt = mLuaUIs.find(lowerNewName);
		if (newIt != mLuaUIs.end())
		{
			Error(FB_DEFAULT_DEBUG_ARG, FormatString("The new name %s is already used.", lowerNewName.c_str()));
			return;
		}
		if (FileSystem::IsFileExisting(newfile))
		{
			BackupFile(newfile);			
		}

		auto oldFilepath = root->GetUIFilePath();
		gFBEnv->pEngine->StopFileChangeMonitor(oldFilepath);
		gFBEnv->pEngine->StopFileChangeMonitor(newfile);
		FileSystem::Rename(oldFilepath, newfile);
		mLuaUIs[lowerNewName].swap(it->second);
		mLuaUIs.erase(it);
		root->SetUIFilePath(newfile);
		root->SetName(newName.c_str());
	}
}

std::string UIManager::GetBackupName(const std::string& name) const
{
	std::string bakName = name + ".bak";
	if (FileSystem::IsFileExisting(bakName.c_str()))
	{
		std::string bakName2 = name + ".bak2";
		FileSystem::DelFile(bakName2.c_str());
		FileSystem::Rename(bakName.c_str(), bakName2.c_str());
	}
	return bakName;
}

void UIManager::BackupFile(const char* filename)
{
	if (FileSystem::IsFileExisting(filename))
	{
		FileSystem::Rename(filename, GetBackupName(filename).c_str());
	}
}

void UIManager::DragUI(){
	assert(mUIEditor);
	static bool sDragStarted = false;
	static bool sSizingXRight = false;
	static bool sSizingXLeft = false;
	static bool sSizingYTop = false;
	static bool sSizingYBottom = false;
	static Vec2I sExpand(8, 8);
	static int sAreaX = 4;
	static int sAreaY = 4;

	if (!gFBEnv->pEngine->IsMainWindowForground())
		return;

	auto mouse = gFBEnv->pEngine->GetMouse();	
	auto keyboard = gFBEnv->pEngine->GetKeyboard();
	if (keyboard->IsKeyDown(VK_MENU))
		return;

	unsigned num = mUIEditor->GetNumCurEditing();
	if (!num)
		return;

	Vec2I dragStartPos;
	bool dragStarted = mouse->IsDragStarted(dragStartPos);
	Vec2I testPos = dragStarted ? dragStartPos : mouse->GetPos();
	
	if (!sDragStarted)
	{			
		bool in = false;
		for (unsigned i = 0; i < num; ++i){
			auto curUI = mUIEditor->GetCurSelected(i);
			if (!curUI)
				return;

			in = curUI->IsIn(testPos, true, &sExpand);
			if (in) {
				sSizingXLeft = curUI->IsPtOnLeft(testPos, sAreaX);
				sSizingXRight = curUI->IsPtOnRight(testPos, sAreaX);
				sSizingYTop = curUI->IsPtOnTop(testPos, sAreaY);
				sSizingYBottom = curUI->IsPtOnBottom(testPos, sAreaY);
				break;
			}
		}
		if (in){
			if (
				(sSizingXLeft && sSizingYTop) ||
				(sSizingXRight && sSizingYBottom)
				)
			{
				SetCursor(WinBase::sCursorNWSE);
			}
			else if (
				(sSizingXRight && sSizingYTop) ||
				(sSizingXLeft && sSizingYBottom)
				)
			{
				SetCursor(WinBase::sCursorNESW);
			}
			else if (sSizingXLeft || sSizingXRight){
				SetCursor(WinBase::sCursorWE);
			}
			else if (sSizingYTop || sSizingYBottom){
				SetCursor(WinBase::sCursorNS);
			}
			else {
				SetCursor(WinBase::sCursorAll);
			}

			if (dragStarted){
				mouse->PopDragEvent();
				sDragStarted = true;
				mUIEditor->BackupSizePos();
			}
		}
	}
	else{
		auto delta = mouse->GetDeltaXY();
		for (unsigned i = 0; i < num; ++i){
			auto curUI = mUIEditor->GetCurSelected(i);
			bool sizing = false;
			if (sSizingXLeft)
			{
				curUI->Move(Vec2I(delta.x, 0));
				curUI->ModifySize(Vec2I(-delta.x, 0));
				sizing = true;
			}
			if (sSizingXRight)
			{
				curUI->ModifySize(Vec2I(delta.x, 0));
				sizing = true;
			}
			if (sSizingYTop)
			{
				curUI->Move(Vec2I(0, delta.y));
				curUI->ModifySize(Vec2I(0, -delta.y));
				sizing = true;
			}
			if (sSizingYBottom)
			{
				curUI->ModifySize(Vec2I(0, delta.y));
				sizing = true;
			}
			if (!sizing)
			{
				curUI->Move(delta);
			}
		}

		if (
			(sSizingXLeft && sSizingYTop) ||
			(sSizingXRight && sSizingYBottom)
			)
		{
			SetCursor(WinBase::sCursorNWSE);
		}
		else if (
			(sSizingXRight && sSizingYTop) ||
			(sSizingXLeft && sSizingYBottom)
			)
		{
			SetCursor(WinBase::sCursorNESW);
		}
		else if (sSizingXLeft || sSizingXRight){

		}
		else if (sSizingYTop || sSizingYBottom){
			SetCursor(WinBase::sCursorNS);
		}
		else{
			SetCursor(WinBase::sCursorAll);
		}

		mUIEditor->OnPosSizeChanged();
		if (mouse->IsDragEnded() || keyboard->IsKeyDown(VK_ESCAPE)){
			if (keyboard->IsKeyDown(VK_ESCAPE)){
				mUIEditor->RestoreSizePos();
			}
			mouse->PopDragEvent();
			sDragStarted = false;
			sSizingXRight = false;
			sSizingXLeft = false;
			sSizingYTop = false;
			sSizingYBottom = false;
		}
	}	
}

void UIManager::DebugUI(){
	auto curHwndId = gFBEnv->pEngine->GetForegroundWindowId();
	auto mouse = gFBEnv->pEngine->GetMouse();
	for (auto& win : mWindows){
		if (win.first != curHwndId)
			continue;

		for (auto comp : win.second){
			if (comp->GetVisible() && comp->IsIn(mouse->GetPos(), false)){
				wchar_t buf[1024];
				swprintf_s(buf, L"UIName = %s", AnsiToWide(comp->GetName()));
				gFBEnv->pRenderer->DrawText(mouse->GetPos(), buf, Color::SkyBlue);
				break;
			}
		}
	}
}

} // namespace fastbird