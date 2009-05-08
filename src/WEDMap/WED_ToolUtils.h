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

#ifndef WED_TOOLUTILS_H
#define WED_TOOLUTILS_H

#include "GUI_Defs.h"

struct Point2;
struct Vector2;
class	GUI_Pane;
class	ISelectable;
class	ITexMgr;
class	WED_ResourceMgr;
class	WED_Airport;
class	WED_Thing;
class	ISelection;
class	ILibrarian;
class	IResolver;
class	IGISEntity;
class	GUI_DragData;

WED_Thing *		WED_FindParent(ISelection * isel,	// Selected objects
					WED_Thing * require_this,		// Our common container will be at or below this in the hiearchy
					WED_Thing * backup_choice);		// If selection is empty, we'll use this.

void			WED_GetSelectionInOrder(IResolver * resolver, vector<WED_Thing *>& out_sel);
void			WED_GetSelectionRecursive(IResolver * resolver, set<WED_Thing *>& out_sel);
bool			WED_IsSelectionNested(IResolver * resolver);		// Returns true if there are parent-children who are selected!
WED_Thing *		WED_GetCreateHost(IResolver * resolver, bool require_airport, int& idx);

WED_Airport *	WED_GetCurrentAirport(IResolver * resolver);
void			WED_SetCurrentAirport(IResolver * resolver, WED_Airport * airport);		// Does NOT create a command!!!!!!
void			WED_SetAnyAirport(IResolver * resolver);

ISelection *	WED_GetSelect(IResolver * resolver);
WED_Thing	*	WED_GetWorld(IResolver * resolver);
ILibrarian *	WED_GetLibrarian(IResolver * resolver);
ITexMgr *		WED_GetTexMgr(IResolver * resolver);
WED_ResourceMgr*WED_GetResourceMgr(IResolver * resolver);

bool			WED_IsIconic(IGISEntity * what);

double			WED_CalcDragAngle(const Point2& ctr, const Point2& handle, const Vector2& drag);

//---------------------------------------------------------------------------------------------------------------------------------
// FILTERS:
//---------------------------------------------------------------------------------------------------------------------------------
// These routines return properties of an object.  The void * param is uusally unused.  They return 1 if true, 0 if false.
// They are designed such that we can run these on the selection (as an iterator) and get a response if ANY part of
// the selection meets this.

// Basic matching filters
int Iterate_ParentMismatch(ISelectable * what, void * ref);				// This object's parent is not the "ref" param.
int Iterate_IsParentOf(ISelectable * what, void * ref);					// This object is a parent of (or is) "ref".
int	Iterate_MatchesThing(ISelectable * what, void * ref);					// ref is a thing to match
int	Iterate_NotMatchesThing(ISelectable * what, void * ref);				// ref is a thing to match
// Airport containment filters
int	Iterate_RequiresAirport(ISelectable * what, void * ref);				// This object MUST have an airport as part of its ancestors.
int	Iterate_ChildRequiresAirport(ISelectable * what, void * ref);			// This object MUST have an airport as part of its ancestors.  Or one of our children requires this.
int	Iterate_IsAirport(ISelectable * what, void * ref);						// This object is an airport.
int	Iterate_IsOrParentAirport(ISelectable * what, void * ref);				// This object is an airport, or its parent is or something.
int	Iterate_IsOrChildAirport(ISelectable * what, void * ref);				// This object is an airport, or its child is or something.
// Grouping and structured obj filters
int	Iterate_IsStructuredObject(ISelectable * what, void * ref);		// This object is part of a polygon or something.  DO NOT reorder it.
int	Iterate_IsNotStructuredObject(ISelectable * what, void * ref);		// This object is part of a polygon or something.  DO NOT reorder it.
int	Iterate_IsPartOfStructuredObject(ISelectable * what, void * ref);		// This object is part of a polygon or something.  DO NOT reorder it.
int	Iterate_IsNotPartOfStructuredObject(ISelectable * what, void * ref);		// This object is part of a polygon or something.  DO NOT reorder it.
int Iterate_IsNotGroup(ISelectable * what, void * ref);					// This object is not a group.
int	Iterate_IsNonEmptyComposite(ISelectable * what, void * ref);			// We are a composite and we have at least one child.
// Selection filters
int Iterate_HasSelectedParent(ISelectable * what, void * ref);				// ref is ISelection.

// Collecting - these
int	Iterate_CollectThings(ISelectable * what, void * ref);				// ref is ptr to vector<wed_thing>
int Iterate_CollectChildPointSequences(ISelectable * what, void * ref);	// ref is a ptr to a vector<IGISPointSequence *>
int Iterate_CollectEntities  (ISelectable * what, void * ref);			// ref is a ptr to a vector<IGISEntity *>
int Iterate_CollectEntitiesUV(ISelectable * what, void * ref);			// ref is a ptr to a vector<IGISEntity *>  - only take entities with UV maps!
//---------------------------------------------------------------------------------------------------------------------------------
// DRAG & DROP
//---------------------------------------------------------------------------------------------------------------------------------

void				WED_RegisterDND(void);
GUI_DragOperation	WED_DoDragSelection(
								GUI_Pane *				pane,
								int						x,
								int						y,
								int						where[4]);
bool				WED_IsDragSelection(
								GUI_DragData *			drag);



#endif /* WED_TOOLUTILS_H */