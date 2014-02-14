#pragma once
	// this class combined with the following helper class provides a simple way to deal with
	// modeless dialogs.  See the example at the end of the file for implementation details.
class ModelessDialogHelper;

class ModelessDialogTracker  
{
public:
	ModelessDialogTracker();
	virtual ~ModelessDialogTracker();

	BOOL IsAlreadyPopped() const;			// call before creating.  If true, don't bother (it will be set to active)
	CDialog* GetDlg() const;				// allows direct (with casting) access to dialog.

	virtual void OnDialogClosed() {}		// override if you want to do something when closed.
	void CloseDialog();						// call to forcibly close dialog (automatic when destructed)
	BOOL IsPopped() const;			// just return the status of the dialog pointer.

	friend ModelessDialogHelper;			// so the helper can access the start and stop functions.
private:
	CDialog* pDlg;								// pointer to modeless dialog (or NULL)
	void StartTracking( CDialog& dlg );	// called by ModelessDialogHelper constructor
	void StopTracking();						// called by ModelessDialogHelper destructor
};

//////////////////////////////////////////////////
class ModelessDialogHelper
{
public:
	ModelessDialogHelper(ModelessDialogTracker& tracker, CDialog& dlg);  // sets the tracked pointer.

	virtual ~ModelessDialogHelper();  // clears the tracked pointer.
private:
	ModelessDialogTracker* pDlgTracker;
};

///////////////////////////////////////////////////
#pragma warning( disable : 4355 )  // the warning given when passing "this" to a member constructor.
												// in the case of the helper class, we WANT to do this.

#ifdef JUST_FOR_EXAMPLE

/////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Simple usage example///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

// 1) instantiate a tracker object somewhere you want to track the dialog (document or app for example)
	ModelessDialogTracker TrackMrM;

// 2) when you create and pop your dialog, use the tracker to test if its already up:
//    if not, create it, passing the tracker to the constructor, then show it.
	{
		if (TrackMrM.IsAlreadyPopped())
			return;

		MrModeless* pnewdlg = new MrModeless(TrackMrM);	// note the passing of the tracker.
		pnewdlg->Create(pnewdlg->IDD,NULL)		
	}

// 3) Derive your dialog class (Mr. Modeless in this case) from ModelessDialogHelper and add some stuff to the header:
	class MrModeless : public CDialog, ModelessDialogHelper
	{
	// Construction
	public:
		///////////////////////////////////////////
		// stuff specific to the modeless dialog.
		MrModeless(ModelessDialogTracker& tracker);   
		BOOL Create(UINT nID, CWnd* pWnd)           	
			{ return CDialog::Create(nID,pWnd);	}
		void PostNcDestroy()
			{ delete this; }
		void OnCancel()								// make sure dialog is only closed with DestroyWindow.. not CDialog::OnOK or CDialog::OnCancel
			{ DestroyWindow(); }
		///////////////////////////////////////////

// 4) also make some changes to the constructor in the cpp file:
	MrModeless::MrModeless(ModelessDialogTracker& tracker)
	: ModelessDialogHelper(tracker, *this)
	{
			// NOTE: the ClassWizard will add member initialization here
	}

// When the helper (and dialog) is constructed, it sets the pointer in the tracker to the dialog pointer.
// When the helper (and dialog) is destructed, it clears the pointer.
//
// All the preamble constructors are needed to let each class know about the others.
//
// Nothing needs to be done in the class or whatever is holding the ModelessDialogTracker object.
// When it is deleted, it will close the dialog.  You can do a few things with it if you want, however:
		TrackMrM.CloseDialog();		// force it to close.

		((MrModeless*)TrackMrM.GetDlg())->SomeFunctionInTheDialog(data);		// access dialog directly.

		TrackMrM::OnDialogClosed()				// override a virtual to sense when it is closed.
		{
			// do some stuff (like lightup a button or whatever) after the dialog closes.
		}


#endif