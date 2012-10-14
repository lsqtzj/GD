/** \file
 *  Game Develop
 *  2008-2012 Florian Rival (Florian.Rival@gmail.com)
 */

#ifndef GDCORE_CHOOSEOBJECTDIALOG_H
#define GDCORE_CHOOSEOBJECTDIALOG_H

#include <string>
#include <vector>
//(*Headers(ChooseObjectDialog)
#include <wx/treectrl.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)
#include <wx/imaglist.h>
#include <wx/srchctrl.h>
namespace gd { class Project; }
namespace gd { class Layout; }

namespace gd
{

/**
 * \brief Dialog used to choose an object ( or a group ) among project/layout objects or groups.
 *
 * \ingroup IDEDialogs
 */
class GD_CORE_API ChooseObjectDialog: public wxDialog
{
public:

    /**
     * Default constructor
     * \param parent The wxWidgets parent window
     * \param project Project
     * \param layout Layout
     * \param canSelectGroup true if objects group can be chosen
     * \param onlyObjectOfType if not empty, only objects of this type will be displayed
     * \param allowMultipleSelection if true, more than one object can be chosen in the dialog
     */
    ChooseObjectDialog(wxWindow* parent,
                       Project & project,
                       gd::Layout & layout,
                       bool canSelectGroup = true,
                       std::string onlyObjectOfType_ = "",
                       bool allowMultipleSelection_ = false);

    virtual ~ChooseObjectDialog();

    /**
     * Return the chosen object
     */
    const std::string & GetChosenObject() const { return objectChosen; }

    /**
     * Return the chosen objects when multiple selection is allowed
     */
    const std::vector <std::string> & GetChosenObjects() const  { return objectsChosen; }

    //(*Declarations(ChooseObjectDialog)
    wxMenu Menu2;
    wxMenu Menu1;
    wxSearchCtrl* searchCtrl;
    wxNotebook* Notebook1;
    wxTreeCtrl* globalObjectGroups;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem4;
    wxButton* AucunBt;
    wxButton* ChoisirBt;
    wxMenuItem* editGroupMenuItem;
    wxTreeCtrl* ObjetsList;
    wxStaticLine* StaticLine1;
    wxTreeCtrl* globalObjectsList;
    wxButton* AnnulerBt;
    wxMenuItem* editMenuItem;
    wxTreeCtrl* GroupesList;
    //*)


protected:

    //(*Identifiers(ChooseObjectDialog)
    static const long ID_TREECTRL1;
    static const long ID_TREECTRL2;
    static const long ID_TREECTRL3;
    static const long ID_TREECTRL4;
    static const long ID_NOTEBOOK1;
    static const long ID_TEXTCTRL1;
    static const long ID_STATICLINE1;
    static const long ID_BUTTON1;
    static const long ID_BUTTON2;
    static const long ID_BUTTON3;
    static const long ID_MENUITEM2;
    static const long ID_MENUITEM1;
    static const long ID_MENUITEM3;
    static const long ID_MENUITEM4;
    //*)

private:

    //(*Handlers(ChooseObjectDialog)
    void OnObjetsListSelectionChanged(wxTreeEvent& event);
    void OnChoisirBtClick(wxCommandEvent& event);
    void OnAnnulerBtClick(wxCommandEvent& event);
    void OnAucunBtClick(wxCommandEvent& event);
    void OnObjetsListItemRightClick(wxTreeEvent& event);
    void OnGroupesListSelectionChanged(wxTreeEvent& event);
    void OnObjetsListItemActivated(wxTreeEvent& event);
    void OnGroupesListItemRightClick(wxTreeEvent& event);
    void OnGroupesListItemActivated(wxTreeEvent& event);
    void OnglobalObjectsListSelectionChanged(wxTreeEvent& event);
    void OnglobalObjectsListItemRightClick(wxTreeEvent& event);
    void OnglobalObjectsListItemActivated(wxTreeEvent& event);
    void OnglobalObjectGroupsItemActivated(wxTreeEvent& event);
    void OnglobalObjectGroupsSelectionChanged(wxTreeEvent& event);
    void OnglobalObjectGroupsItemRightClick(wxTreeEvent& event);
    void OnsearchCtrlText(wxCommandEvent& event);
    //*)

    /**
     * Populate trees with objects
     */
    void Refresh();

    wxTreeItemId item;
    wxTreeItemId itemGroups;
    wxTreeItemId itemGlobal;
    wxTreeItemId itemGlobalGroups;

    Project & project;
    gd::Layout & layout;

    std::string objectChosen;
    std::vector <std::string> objectsChosen; ///< Used if dialog support multiple selection.

    wxImageList * imageList;
    std::string onlyObjectOfType;
    bool allowMultipleSelection;
    bool canSelectGroup;

    DECLARE_EVENT_TABLE()
};

}
#endif