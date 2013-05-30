/*
 * Copyright (c) 2007, Laminar Research.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "GUI_Application.h"
#include "AssertUtils.h"
#include "GUI_Menus.h"
#include "XWin.h"
#include "GUI_Window.h"
#if IBM
#include <commctrl.h>
#endif
#define __DEBUGGING__

GUI_Application *	gApplication = NULL;

#if APL || LIN
static void	NukeAmpersand(string& ioString)
{
	string::size_type loc;
	while ((loc = ioString.find('&')) != ioString.npos)
	{
		ioString.erase(loc,1);
	}
}
#endif

static bool IsDisabledString(string& ioString)
{
	if(!ioString.empty() && ioString[0] == ';')
	{
		ioString.erase(ioString.begin());
		return true;
	}
	return false;
}


#if IBM
HACCEL			gAccel = NULL;
vector<ACCEL>	gAccelTable;
#endif

#if APL
	#if defined(__MWERKS__)
		#include <Carbon.h>
	#else
		#include <Carbon/Carbon.h>
	#endif
#include "XUtils.h"

pascal OSErr GUI_Application::HandleOpenDoc(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon)
{
	GUI_Application * me = reinterpret_cast<GUI_Application *>(handlerRefcon);

	string	fpath;
	vector<string>	files;


	AEDescList	inDocList = { 0 };
	OSErr err = AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &inDocList);
	if (err) return err;

	SInt32		numDocs;
	err = ::AECountItems(&inDocList, &numDocs);
	if (err) goto puke;

		// Loop through all items in the list
			// Extract descriptor for the document
			// Coerce descriptor data into a FSSpec
			// Tell Program object to open or print document


	for (SInt32 i = 1; i <= numDocs; i++) {
		AEKeyword	theKey;
		DescType	theType;
		FSRef		theFileSpec;
		Size		theSize;

		err = ::AEGetNthPtr(&inDocList, i, typeFSRef, &theKey, &theType,
							(Ptr) &theFileSpec, sizeof(FSRef), &theSize);
		if (err) goto puke;
		UInt8 buf[2048];
		if(FSRefMakePath(&theFileSpec, buf, sizeof(buf)) == noErr)
		files.push_back((const char *) buf);
	}
	me->OpenFiles(files);

puke:
	AEDisposeDesc(&inDocList);
	return noErr;
}


pascal OSStatus GUI_Application::MacEventHandler(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *inUserData)
{
	GUI_Application * me = reinterpret_cast<GUI_Application *>(inUserData);

	HICommand 	cmd;
	OSStatus	status;
	MenuRef		amenu;

	UInt32	clss = ::GetEventClass(inEvent);
	UInt32	kind = ::GetEventKind(inEvent);
	switch(clss) {
	case kEventClassCommand:
		switch(kind) {
		case kEventCommandProcess:
			{
				status = GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(cmd), NULL, &cmd);
				if (status != noErr) return status;

				switch(cmd.commandID) {
				case kHICommandQuit:
					if (me->DispatchHandleCommand(gui_Quit))			return noErr;
					else												return eventNotHandledErr;
				case kHICommandAbout:
					if (me->DispatchHandleCommand(gui_About))			return noErr;
					else												return eventNotHandledErr;
				case kHICommandPreferences:
					if (me->DispatchHandleCommand(gui_Prefs))			return noErr;
					else												return eventNotHandledErr;
				default:
					if (me->DispatchHandleCommand(cmd.commandID))		return noErr;
					else												return eventNotHandledErr;
				}
			}
		default:
			return eventNotHandledErr;
		}
	case kEventClassMenu:
		switch(kind) {
		case kEventMenuEnableItems:
			{
				status = GetEventParameter(inEvent, kEventParamDirectObject, typeMenuRef, NULL, sizeof(amenu), NULL, &amenu);
				if (status != noErr) return status;

				if (me->mMenus.count(amenu) == 0)
					return eventNotHandledErr;

				int item_count = ::CountMenuItems(amenu);

				for (int n = 1; n <= item_count; ++n)
				{
					MenuCommand	id;
					GetMenuItemCommandID(amenu, n, &id);

					if (id == kHICommandQuit)			id = gui_Quit;
					if (id == kHICommandAbout)			id = gui_About;
					if (id == kHICommandPreferences)	id = gui_Prefs;

					if (id != 0)
					{
						string	ioName;
						int		ioCheck = 0;
						if (me->DispatchCanHandleCommand(id, ioName, ioCheck))
							EnableMenuItem(amenu, n);
						else
							DisableMenuItem(amenu, n);

						if (!ioName.empty())
						{
							NukeAmpersand(ioName);
							CFStringRef	cfstr = CFStringCreateWithCString(kCFAllocatorDefault, ioName.c_str(), kCFStringEncodingMacRoman);
							SetMenuItemTextWithCFString(amenu, n, cfstr);
							CFRelease(cfstr);
						}

						::CheckMenuItem(amenu, n, ioCheck > 0);
					}
				}

				return noErr;
			}
		default:
			return eventNotHandledErr;
		}
	default:
		return eventNotHandledErr;
	}
}

#endif
#if LIN

GUI_QtMenu::GUI_QtMenu
(const QString& text , GUI_Application *app)
:QMenu(text),app(app)
{}

GUI_QtMenu::~GUI_QtMenu()
{}

void GUI_QtMenu::showEvent( QShowEvent * e )
{
    QList<QAction*> actlist = this->actions();
    QList<QAction*>::iterator it = actlist.begin();
    for (it ; it != actlist.end(); ++it)
    {
        int cmd = (*it)->data().toInt();
        if (cmd)
        {
            int checked = 0;
            string new_name;
            (*it)->setEnabled(app->DispatchCanHandleCommand(cmd,new_name,checked));
            if (!new_name.empty())(*it)->setText(QString::fromStdString(new_name));
            (*it)->setCheckable(checked);
            (*it)->setChecked(checked);
        }
    }
}

void GUI_QtMenu::hideEvent( QHideEvent * e )
{
    // mroe:
    // We must set to 'enabled' again, since we have disabled
    // items and their shortcut-action while showevent .
    QList<QAction*> actlist = this->actions();
    QList<QAction*>::iterator it = actlist.begin();
    for (it ; it != actlist.end(); ++it)
    {
        int cmd = (*it)->data().toInt();
        if (cmd)  (*it)->setEnabled(true);
    }
    this->QMenu::hideEvent(e);
}

GUI_QtAction::GUI_QtAction
(const QString& text,QObject * parent,const QString& sc ,int cmd, GUI_Application *app, bool checkable)
: QAction(text,parent) ,app(app)
{
	setData(cmd);
	setShortcut(sc);
	setCheckable(checkable);
	setChecked(checkable);
	connect(this, SIGNAL(triggered()), this, SLOT(ontriggered()));
}

GUI_QtAction::~GUI_QtAction()
{}

void GUI_QtAction::ontriggered()
{
    int cmd = data().toInt();
    if (!cmd) return;
    int  ioCheck = 0;
    string ioName;
    //mroe : We must check again if the cmd can be handled ,
    //		because shortcut-actions allways enabled .
    if(app->DispatchCanHandleCommand(cmd,ioName,ioCheck))
			app->DispatchHandleCommand(cmd);
}


QMenuBar* GUI_Application::getqmenu()
{
    QMenuBar * mbar = new QMenuBar(0);
    QList<GUI_QtMenu*>::iterator iter = mMenus.begin();
    while (iter != mMenus.end())
	{
		mbar->addMenu(*iter);
		++iter;
	}
    return mbar;
}
#endif

#if IBM
void	RegisterAccel(const ACCEL& inAccel)
{
	gAccelTable.push_back(inAccel);
}

static	void		BuildAccels(void)
{
	gAccel = CreateAcceleratorTable(&*gAccelTable.begin(), gAccelTable.size());
}
#endif

#if LIN
GUI_Application::GUI_Application(int& argc, char* argv[]) : GUI_Commander(NULL)
#else
GUI_Application::GUI_Application() : GUI_Commander(NULL)
#endif
{
	DebugAssert(gApplication == NULL);
	gApplication = this;
	mDone = false;
#if APL

		IBNibRef	nib = NULL;
	OSStatus err = CreateNibReference(CFSTR("GUI"), &nib);
	if (err == 0)
		err = SetMenuBarFromNib(nib, CFSTR("MenuBar"));
	EnableMenuCommand(NULL, kHICommandAbout);
	EnableMenuCommand(NULL, kHICommandPreferences);


	mMacEventHandlerUPP = NewEventHandlerUPP(MacEventHandler);
	mHandleOpenDocUPP = NewAEEventHandlerUPP(HandleOpenDoc);

	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, mHandleOpenDocUPP, reinterpret_cast<long>(this), FALSE);

	EventTypeSpec menu_events[] = {
		kEventClassCommand,			kEventCommandProcess,
		kEventClassMenu,			kEventMenuEnableItems };

	InstallEventHandler(GetApplicationEventTarget(), mMacEventHandlerUPP, GetEventTypeCount(menu_events), menu_events, reinterpret_cast<void *>(this), &mMacEventHandlerRef);

#endif
#if IBM
	// Note: GetModuleHandle(NULL) returns the process instance/module handle which
	// is what we want UNLESS this code is put in a DLL, which would need some re-evaluation.

	XWin::RegisterClass(GetModuleHandle(NULL));
	if(OleInitialize(NULL) != S_OK)
	{
#if ERROR_CHECK
		uh oh
#endif
	}
	InitCommonControls();
#endif
#if LIN
	qapp = new QApplication(argc, argv);
	qapp->setAttribute(Qt::AA_DontUseNativeMenuBar);
#endif
}

GUI_Application::~GUI_Application()
{
	DebugAssert(gApplication == this);
	gApplication = NULL;
}

void			GUI_Application::Run(void)
{
#if APL
	RunApplicationEventLoop();
#endif
#if IBM

	MSG msg;

	BuildAccels();
	while (!mDone && GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, gAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
#endif
#if LIN
	qapp->connect(qapp, SIGNAL(lastWindowClosed()), qapp, SLOT(quit()));
	qapp->exec();
#endif
}

void			GUI_Application::Quit(void)
{
	mDone = true;
#if APL
	QuitApplicationEventLoop();
#endif
#if LIN
	qapp->quit();
#endif
}

GUI_Menu		GUI_Application::GetMenuBar(void)
{
	#if APL
		return NULL;
	#elif IBM
		HWND hwnd = GUI_Window::AnyHWND();
		if (hwnd == NULL) return NULL;
		HMENU mbar = ::GetMenu(hwnd);
		if (mbar != NULL) return mbar;
		mbar = ::CreateMenu();
		::SetMenu(hwnd, mbar);
		return mbar;
	#else
        QMainWindow* mwindow = ((QMainWindow*)qapp->activeWindow());
        if (mwindow)
        {
            QMenuBar* mbar =  mwindow->menuBar();
            return mbar;
        }
        return NULL;
	#endif
}

GUI_Menu		GUI_Application::GetPopupContainer(void)
{
	#if APL
		return (GUI_Menu) -1;
	#elif IBM
		return NULL;
	#else
		return 0;
	#endif
}

GUI_Menu	GUI_Application::CreateMenu(const char * inTitle, const GUI_MenuItem_t items[], GUI_Menu	parent, int parentItem)
{

#if APL
	static MenuID	gIDs = 1000;
#endif
#if IBM
	static int		gIDs = 1000;
#endif
#if LIN
	static int		gIDs = 1000;
#endif

#if APL
	MenuRef	new_menu;
	::CreateNewMenu(gIDs++, kMenuAttrAutoDisable, &new_menu);
	if (parent != GetPopupContainer())
		::MacInsertMenu(new_menu, (parent == NULL) ? 0 : kInsertHierarchicalMenu);

	string	title(inTitle);
	NukeAmpersand(title);
	CFStringRef	cfstr = CFStringCreateWithCString(kCFAllocatorDefault, title.c_str(), kCFStringEncodingMacRoman);
	::SetMenuTitleWithCFString(new_menu, cfstr);
	CFRelease(cfstr);

	if (new_menu && parent != GetPopupContainer())
	{
		::SetMenuItemHierarchicalID((MenuRef) parent, parentItem + 1, ::GetMenuID(new_menu));
	}
#endif

#if IBM

	HMENU	new_menu;

	if (parent == GetPopupContainer())	new_menu = CreatePopupMenu();
	else								new_menu = ::CreateMenu();

	if (parent)
	{
		MENUITEMINFO	mif = { 0 };
		mif.cbSize = sizeof(mif);
		mif.hSubMenu = (HMENU) new_menu;
		mif.fType = MFT_STRING;
		mif.dwTypeData = const_cast<char *>(inTitle);
		mif.fMask = (parent == GetPopupContainer()) ? MIIM_TYPE : (MIIM_TYPE | MIIM_SUBMENU);

		if (parent == GetMenuBar())
		{
			InsertMenuItem((HMENU) parent, -1, true, &mif);
		} else {
			SetMenuItemInfo((HMENU) parent, parentItem, true, &mif);
		}
	}
#endif
#if LIN
	GUI_QtMenu* new_menu = new GUI_QtMenu(inTitle,this);

    if (parent==GetMenuBar())
    {
        mMenus << new_menu;
        if (parent)
                ((QMenuBar*) parent)->addMenu(new_menu);
    }
    else
         ((GUI_QtMenu*) parent)->actions().at(parentItem)->setMenu(new_menu);

#endif
	RebuildMenu(new_menu, items);
#if !LIN
	mMenus.insert(new_menu);
#endif
#if IBM
	if (parent)
		DrawMenuBar(GUI_Window::AnyHWND());
#endif
	return new_menu;
}

void	GUI_Application::RebuildMenu(GUI_Menu new_menu, const GUI_MenuItem_t	items[])
{
	#if APL
		if (CountMenuItems((MenuRef) new_menu) > 0)
			DeleteMenuItems((MenuRef) new_menu,1,CountMenuItems((MenuRef) new_menu));

		int n = 0;
		while (items[n].name)
		{
			string	itemname(items[n].name);
			NukeAmpersand(itemname);
			bool is_disable = IsDisabledString(itemname);
			CFStringRef cfstr = CFStringCreateWithCString(kCFAllocatorDefault, itemname.c_str(), kCFStringEncodingMacRoman);
			::AppendMenuItemTextWithCFString((MenuRef) new_menu, cfstr, (itemname=="-" ? kMenuItemAttrSeparator : 0), items[n].cmd, NULL );
			CFRelease(cfstr);

			switch(items[n].key) {
			case GUI_KEY_UP:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuUpArrowGlyph);		break;
			case GUI_KEY_DOWN:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuDownArrowGlyph);	break;
			case GUI_KEY_RIGHT:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuRightArrowGlyph);	break;
			case GUI_KEY_LEFT:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuLeftArrowGlyph);	break;
			case GUI_KEY_BACK:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuDeleteLeftGlyph);	break;
			case GUI_KEY_RETURN:	SetMenuItemKeyGlyph((MenuRef) new_menu,n+1, kMenuReturnGlyph);		break;
			default:				::SetItemCmd((MenuRef) new_menu, n+1, items[n].key);	break;
			}

			::SetMenuItemModifiers((MenuRef) new_menu, n+1,
					((items[n].flags & gui_ShiftFlag) ? kMenuShiftModifier : 0) +
					((items[n].flags & gui_OptionAltFlag) ? kMenuOptionModifier : 0) +
					((items[n].flags & gui_ControlFlag) ? 0 : kMenuNoCommandModifier));

			::CheckMenuItem((MenuRef) new_menu,n+1,items[n].checked);
			if(is_disable) ::DisableMenuItem((MenuRef) new_menu,n+1);

			++n;
		}
	#elif IBM
		while (GetMenuItemCount((HMENU) new_menu) > 0)
			if (RemoveMenu((HMENU) new_menu, 0, MF_BYPOSITION) == 0) break;


		int n = 0;
		while (items[n].name)
		{
			string	itemname(items[n].name);
			bool is_disable = IsDisabledString(itemname);
			if(items[n].key != 0)
			{
				ACCEL accel = { 0 };
				accel.fVirt = FVIRTKEY;
				accel.cmd = items[n].cmd;
				itemname += "\t";
				if (items[n].flags & gui_ControlFlag)	{itemname += "Ctrl+";	accel.fVirt |= FCONTROL;	}
				if (items[n].flags & gui_ShiftFlag)		{itemname += "Shift+";	accel.fVirt |= FSHIFT;		}
				if (items[n].flags & gui_OptionAltFlag) {itemname += "Alt+";	accel.fVirt |= FALT;		}
				char key_cstr[2] = { items[n].key, 0 };
				switch(items[n].key)
				{
					case GUI_KEY_UP:	itemname += "Up";	accel.key = VK_UP;	break;
					case GUI_KEY_DOWN:	itemname += "Down";	accel.key = VK_DOWN;	break;
					case GUI_KEY_RIGHT:	itemname += "Right";	accel.key = VK_RIGHT;	break;
					case GUI_KEY_LEFT:	itemname += "Left";	accel.key = VK_LEFT;	break;
					case GUI_KEY_BACK:	itemname += "Del";	accel.key = VK_BACK;	break;
					case GUI_KEY_RETURN:	itemname += "Return";	accel.key = VK_RETURN;	break;
					default:		itemname += key_cstr;	accel.key = VkKeyScan(items[n].key) & 0xFF;	break;
				}
				RegisterAccel(accel);
			}

			MENUITEMINFO mif = { 0 };
			mif.cbSize = sizeof(mif);
			mif.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
			mif.fType = (itemname=="-") ? MFT_SEPARATOR : MFT_STRING;
			mif.fState = 0;
			if(items[n].checked) mif.fState |= MFS_CHECKED;
			if(is_disable) mif.fState |= MFS_DISABLED;
			mif.wID = items[n].cmd;
			mif.dwItemData = items[n].cmd;
			mif.dwTypeData = const_cast<char*>(itemname.c_str());
			int err = InsertMenuItem((HMENU) new_menu, -1, true, &mif);

			++n;
		}
	#elif LIN
	QMenu * menu = (QMenu*) new_menu;
	menu->clear();
	int n = 0;
	while (items[n].name)
	{
		string	itemname(items[n].name);
		bool is_disable = IsDisabledString(itemname);
	
		if (!strcmp(items[n].name, "-"))
			menu->addSeparator();
		else
            if (!items[n].cmd)
                menu->addMenu(itemname.c_str());
            else
		    {
		        QString	sc = "";
				if (items[n].flags & gui_ControlFlag)	{sc += "Ctrl+";}
				if (items[n].flags & gui_ShiftFlag)     {sc += "Shift+";}
				if (items[n].flags & gui_OptionAltFlag) {sc += "Alt+";}
				char key_cstr[2] = { items[n].key, 0 };
				switch(items[n].key)
				{
					case GUI_KEY_UP:	sc += "Up";     break;
					case GUI_KEY_DOWN:	sc += "Down";   break;
					case GUI_KEY_RIGHT:	sc += "Right";  break;
					case GUI_KEY_LEFT:	sc += "Left";   break;
					case GUI_KEY_BACK:	sc += "Del";    break;
					case GUI_KEY_RETURN:	sc += "Return"; break;
					default:            	sc += key_cstr; break;
				}
				QAction * act;
				menu->addAction(act = new GUI_QtAction(itemname.c_str(),menu,sc,items[n].cmd,this,false));
				if(is_disable)
					act->setEnabled(false);
		    }
		++n;
	}
	#endif
}

int			GUI_Application::HandleCommand(int command)
{
	switch(command) {
	case gui_About: this->AboutBox(); return 1;
	case gui_Prefs: this->Preferences(); return 1;
	case gui_Quit: if (this->CanQuit()) this->Quit(); return 1;
	default: return 0;
	}
}

int			GUI_Application::CanHandleCommand(int command, string& ioName, int& ioCheck)
{
	switch(command) {
	case gui_About: return 1;
	case gui_Prefs: return 1;
	case gui_Quit: return 1;
	default: return 0;
	}
}
