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

#ifndef GUI_PANE_H
#define GUI_PANE_H

/*

	GUI_Pane - THEORY OF OPERATION

	MEMORY MANAGEMENT

		Each pane must be dynamically allocated - if a pane is deleted, it deletes its children, so
		whole view hierarchies can be deleted at once by deleting the root.

		"Behavior" code - that is, derivatives of abstract classes that are "plugged in" to views
		are NOT released.  This allows you to derive from several behaviors and not worry about
		multiple deletes.

	LAYOUT MANAGEMENT

		Layout management for GUI panes is done in two phase: initial setup and incremental modification.

		Initial setup is done manually by client code, whether that means clients calling SetBounds
		explicitly or specific convenience methods, such as the routine that automatically puts a
		scrollable pane into a scroller.

		A pane has four 'sticky' flags, indicating that they should move relative to their parents
		corresponding walls.

		The ideas is that the initial layout is set once and the stickiness preserves this relationship.

	DRAG & DROP

		GUI_Pane participates in drag & drop in two ways:

		- Each pane is a possible drag & drop receiver...template methods allow sub-classes to decide
		  what they want.

		- The GUI_Pane base class contains the routines to originate a drag & drop.

		GUI_DragData provides an abstract interface by which panes receiving drags can decide if they
		want to accept them.

		The operational flags are used to control what the drag actually does.  They are used in four ways:

		1.	The allowed flags are passed by the drag originator.  This is a set of all possible drag
			operations, e.g. we can only move, we can only copy, etc.  The operation won't be outside
			the allowed bounds.

		2.	The recommended flags are generated by the GUI framework (based on the allowed flags and the
			modifier keys being held down).

		3.	The tracking result is calculated by the receiver, based on the allowed and recommended flags.
			Only one bit flag should be set (or none), indicating what will happen.

		4.	The drop result is a little bit different - the receiver returns "move" if the desired
			operation is a move AND the receiver is only able to do a copy (requiring the source to
			do the corresponding delete).  Note that if the operation is a copy, or a move that is
			fully completed by the receiver, the result is "copy".  If the operation is aborted, the result
			is "none".  (This is necessary for the Mac, which must know if we aborted the op.)

		Note that the receiver has total control over what operation actually happens, based on allowed
		ops, recommended ops, and examination of the source data (and self-knowledge) of the drop site,
		the drop target thus makes the final call.

	SCREEN REFRESH

		The screen is always refreshed asynchronously.  Call "Refresh" to force a redraw later of this pane.
		You can never draw synchronously.

		It should be noted that basically all "wait time" in the app is expected to happen blocking for UI.
		(There are three times this happens: between calls to handlers in the app main loop, when waiting to
		see if a click is a drag, and while doing a drag & drop op).  In all 3 cases, the framework does
		refreshing before the block, during the block, or after a handler is called out of a block.  The
		result is that you can simply make sure your handlers are fast and call Refresh() and updates will
		happen later.

	MODIFIER KEYS AND EVENT BLOCKING

		GUI_Pane makes a function call every time the mouse is moved...in that sense it's not terribly good
		about idling the CPU, but ... today's computers are fast, and today's OSes do SOME dispatch work
		whether we want this or not.

		GUI_Pane does not send events when mod keys change.  However, it does send a FAKE move/drag event
		(a move/drag callback when the mouse hasn't really moved) if the modifiers change.  The GUI routine
		GetModifiersNow return the modifier keys based on the event we are handling (that is, they are
		event-dispatch-synchronized, which is what we want), so it is always safe to simply look at them
		when handling mouse up/down/move/drag/wheel.

		Mouse location is NOT event-synchronized.  This is probably a good thing because (1) we don't get
		mouse events if the cursor is outside the app window (or we're in the background) but (2) we may
		have to redraw anyway due to a timer.  So this keeps the mouse tracking correct for animated
		windows that track the mouse.
*/

#include "GUI_Defs.h"
#include <stdio.h>
class	GUI_GraphState;
class	GUI_DragData;

class	GUI_Pane {
public:

						GUI_Pane();
	virtual			   ~GUI_Pane();

	/* GENERAL API - some of these are virtual; don't override.  The use of virtual
	   is meant only to implement the base window class. */

			int			CountChildren(void) const;
			GUI_Pane *	GetNthChild(int n) const;
			GUI_Pane *	GetParent(void) const;
			void		SetParent(GUI_Pane * inParent);

			void		GetBounds(int outBounds[4]);		// Our extend in OGL/win coords
			void		GetVisibleBounds(int outBounds[4]);	// the subset of our extent that is not clipped by parents!

	virtual void		SetBounds(int x1, int y1, int x2, int y2);
	virtual void		SetBounds(int inBounds[4]);
			void		GetSticky(float outSticky[4]);
			void		SetSticky(float inSticky[4]);
						//Left, Bottom, Right, Top
			void		SetSticky(float x1, float y1, float x2, float y2);

			int			GetID(void) const;
			void		SetID(int id);
			GUI_Pane *	FindByID(int id);
			GUI_Pane *	FindByPoint(int x, int y);
			void		GetDescriptor(string& outDesc) const;
	virtual	void		SetDescriptor(const string& inDesc);

			bool		IsVisible(void) const;
	virtual bool		IsVisibleNow(void) const;
	virtual void		Show(void);
	virtual void		Hide(void);

			bool		IsEnabled(void) const;
			bool		IsEnabledNow(void) const;
			void		Enable(void);
			void		Disable(void);

	virtual	bool		IsActiveNow(void) const;

	virtual void		Refresh(void);				// Schedule an async window redraw.

	virtual	void		PopupMenu(GUI_Menu menu, int x, int y, int button);											// Pop up a menu - useful for providing main or fixed menus contextually
	virtual	int			PopupMenuDynamic(const GUI_MenuItem_t items[], int x, int y, int button, int current);		// Pop up dynamic content.  No nesting, built on fly.  For enums, etc.

	virtual	bool		IsDragClick(int x, int y, int button);								// Returns true if the click is a drag, false if it is just a mouse release.
	virtual	GUI_DragOperation
						DoDragAndDrop(
							int						x,
							int						y,
							int						button,
							int						where[4],
							GUI_DragOperation		operations,
							int						type_count,
							GUI_ClipType			inTypes[],
							int						sizes[],
							const void *			ptrs[],
							GUI_GetData_f			get_data_f,
							void *					ref);

			GUI_KeyFlags	GetModifiersNow(void);
	virtual	void			GetMouseLocNow(int * out_x, int * out_y);
			float			GetTimeNow(void);

			void			TrapFocus(void);
	
	#if DEV
	// int identLevel indicates how much to indent
	// To use paste the following into any GUI Pane and uncomment
		/*#if DEV
			PrintDebugInfo(0);
		#endif*/
	//The default value is 0 so feel free not to use it
	//Uses printf to print out pane information and all its children, may be over ridden to provide more information
	virtual void            PrintDebugInfo(int indentLevel=0);
	virtual void			FPrintDebugInfo(FILE * pFile, int indentLevel=0);
	#endif
	
	/* TEMPLATE METHODS - Override these to customize a pane. */
	
	virtual	void		Draw(GUI_GraphState * state) { }

	virtual	int			MouseMove(int x, int y			  ) { return 0; }
	virtual	int			MouseDown(int x, int y, int button) { return 0; }
	virtual	void		MouseDrag(int x, int y, int button) { 			}
	virtual	void		MouseUp  (int x, int y, int button) { 			}
	virtual	int			ScrollWheel(int x, int y, int dist, int axis) { return 0; }
	virtual	int			TrapNotify(int x, int y, int button) { return 0; }

	virtual	int			GetCursor(int x, int y) { return gui_Cursor_None; }
	virtual	int			GetHelpTip(int x, int y, int tip_bounds[4], string& tip) { return 0; }

	virtual	GUI_DragOperation			DragEnter	(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended) { return gui_Drag_None;	}
	virtual	GUI_DragOperation			DragOver	(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended) { return gui_Drag_None;	}
	virtual	void						DragScroll	(int x, int y) { }
	virtual	void						DragLeave	(void) { }
	virtual	GUI_DragOperation			Drop		(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended) { return gui_Drag_None;	}


private:

	friend class GUI_Window;
	friend class GUI_Window_DND;		// For Windoze DND
	// Internal dispatch methods

			void		InternalDraw(GUI_GraphState * state);
			GUI_Pane *	InternalMouseDown(int x, int y, int button);
			GUI_Pane *	InternalMouseMove(int x, int y);
			int			InternalMouseWheel(int x, int y, int dist, int axis);
			int			InternalGetCursor(int x, int y);
			int			InternalGetHelpTip(int x, int y, int tip_bounds[4], string& tip);

			void		ParentResized(int inOldBounds[4], int inNewBounds[4]);

			GUI_DragOperation			InternalDragEnter	(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended);
			GUI_DragOperation			InternalDragOver	(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended);
			void						InternalDragScroll	(int x, int y);
			void						InternalDragLeave	(void);
			GUI_DragOperation			InternalDrop		(int x, int y, GUI_DragData * drag, GUI_DragOperation allowed, GUI_DragOperation recommended);

		int					mBounds[4];
		float				mSticky[4];
		
		GUI_Pane *			mParent;
		vector<GUI_Pane *>	mChildren;
		int					mID;
		int					mVisible;
		int					mEnabled;
		string				mDesc;

		GUI_Pane *			mDragTarget;

		set<GUI_Pane *>		mTrap;

	GUI_Pane(const GUI_Pane&);
	GUI_Pane& operator=(const GUI_Pane&);

};


#endif

