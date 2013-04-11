/** \file
 *  Game Develop
 *  2008-2013 Florian Rival (Florian.Rival@gmail.com)
 */

#if defined(GD_IDE_ONLY)
    #include <wx/wx.h>
    #define MSG(x) wxLogWarning(x);          // Utiliser WxWidgets pour
    #define MSGERR(x) wxLogError(x.c_str()); // afficher les messages dans l'�diteur
    #include "GDL/IDE/Dialogs/ProjectUpdateDlg.h"
    #include "GDL/PlatformDefinition/Platform.h"
    #include "GDCore/PlatformDefinition/ObjectGroup.h"
    #include "GDCore/IDE/ResourcesUnmergingHelper.h"
    #include "GDCore/Events/Event.h"
    #include "GDCore/Events/Instruction.h"
#else
    #include "GDL/Log.h"
    #include <iostream>

    #ifndef _
    #define _(x) x // "Emule" la macro de WxWidgets
    #endif

    #define MSG(x) std::cout << _("Loading: ") << x; //Macro pour rapporter des erreurs
    #define MSGERR(x) std::cout << _("Error during loading: ") << x;

#endif

#include <string>
#include <cctype>
#include <boost/shared_ptr.hpp>

#include "GDL/OpenSaveGame.h"
#include "GDL/tinyxml/tinyxml.h"
#include "GDL/CommonTools.h"
#include "GDL/Game.h"
#include "GDL/Scene.h"
#include "GDL/Object.h"
#include "GDL/Animation.h"
#include "GDL/Position.h"
#include "GDL/Automatism.h"
#include "GDL/AutomatismsSharedData.h"
#include "GDL/ExtensionBase.h"
#include "GDL/VersionWrapper.h"
#include "GDL/ExtensionsLoader.h"
#include "GDL/Layer.h"
#include "GDL/OpenSaveLoadingScreen.h"
#include "GDL/ExtensionsManager.h"
#include "GDL/ForEachEvent.h"
#include "GDL/WhileEvent.h"
#include "GDL/ExternalEvents.h"
#include "GDL/ExternalLayout.h"
#include "GDL/StandardEvent.h"
#include "GDL/RepeatEvent.h"
#include "GDL/XmlMacros.h"
#include "GDL/SourceFile.h"

using namespace std;

#if defined(GD_IDE_ONLY)
void OpenSaveGame::OpenConditions(vector < gd::Instruction > & conditions, const TiXmlElement * elem)
{
    if (elem == NULL) return;
    const TiXmlElement * elemConditions = elem->FirstChildElement();

    //Passage en revue des conditions
    while ( elemConditions )
    {
        gd::Instruction instruction;

        //Read type and infos
        const TiXmlElement *elemPara = elemConditions->FirstChildElement( "Type" );
        if ( elemPara != NULL )
        {
            instruction.SetType( elemPara->Attribute( "value" ) != NULL ? elemPara->Attribute( "value" ) : "");
            instruction.SetInverted( (elemPara->Attribute( "Contraire" ) != NULL) && (string(elemPara->Attribute( "Contraire" )) == "true") );
        }

        //Read parameters
        vector < gd::Expression > parameters;
        elemPara = elemConditions->FirstChildElement("Parametre");
        while ( elemPara )
        {
            if ( elemPara->Attribute( "value" ) != NULL ) parameters.push_back( gd::Expression(elemPara->Attribute( "value" )) );
            elemPara = elemPara->NextSiblingElement("Parametre");
        }
        instruction.SetParameters( parameters );

        //Read sub conditions
        if ( elemConditions->FirstChildElement( "SubConditions" ) != NULL )
            OpenConditions(instruction.GetSubInstructions(), elemConditions->FirstChildElement( "SubConditions" ));

        conditions.push_back( instruction );

        elemConditions = elemConditions->NextSiblingElement();
    }
}

void OpenSaveGame::OpenActions(vector < gd::Instruction > & actions, const TiXmlElement * elem)
{
    if (elem == NULL) return;
    const TiXmlElement * elemActions = elem->FirstChildElement();

    //Passage en revue des actions
    while ( elemActions )
    {
        gd::Instruction instruction;

        //Read type and info
        const TiXmlElement *elemPara = elemActions->FirstChildElement( "Type" );
        if ( elemPara != NULL )
        {
            if (elemPara->Attribute( "value" ) != NULL) instruction.SetType( elemPara->Attribute( "value" ));
        }

        //Read parameters
        vector < gd::Expression > parameters;
        elemPara = elemActions->FirstChildElement("Parametre");
        while ( elemPara )
        {
            if (elemPara->Attribute( "value" ) != NULL) parameters.push_back( gd::Expression(elemPara->Attribute( "value" )) );
            elemPara = elemPara->NextSiblingElement("Parametre");
        }
        instruction.SetParameters(parameters);

        //Read sub actions
        if ( elemActions->FirstChildElement( "SubActions" ) != NULL )
            OpenActions(instruction.GetSubInstructions(), elemActions->FirstChildElement( "SubActions" ));

        actions.push_back(instruction);
        elemActions = elemActions->NextSiblingElement();
    }
}
#endif


#if defined(GD_IDE_ONLY)
void OpenSaveGame::SaveActions(const vector < gd::Instruction > & list, TiXmlElement * actions)
{
    for ( unsigned int k = 0;k < list.size();k++ )
    {
        //Pour chaque condition
        TiXmlElement * action;

        action = new TiXmlElement( "Action" );
        actions->LinkEndChild( action );

        //Le type
        TiXmlElement * typeAction;
        typeAction = new TiXmlElement( "Type" );
        action->LinkEndChild( typeAction );

        typeAction->SetAttribute( "value", list[k].GetType().c_str() );


        //Les autres param�tres
        for ( unsigned int l = 0;l < list[k].GetParameters().size();l++ )
        {
            TiXmlElement * Parametre = new TiXmlElement( "Parametre" );
            action->LinkEndChild( Parametre );
            Parametre->SetAttribute( "value", list[k].GetParameter( l ).GetPlainString().c_str() );
        }

        //Sub instructions
        if ( !list[k].GetSubInstructions().empty() )
        {
            TiXmlElement * subActions = new TiXmlElement( "SubActions" );
            action->LinkEndChild(subActions);
            SaveActions(list[k].GetSubInstructions() , subActions);
        }
    }
}

void OpenSaveGame::SaveConditions(const vector < gd::Instruction > & list, TiXmlElement * conditions)
{
    for ( unsigned int k = 0;k < list.size();k++ )
    {
        //Pour chaque condition
        TiXmlElement * condition = new TiXmlElement( "Condition" );
        conditions->LinkEndChild( condition );

        //Le type
        TiXmlElement * typeCondition = new TiXmlElement( "Type" );
        condition->LinkEndChild( typeCondition );

        typeCondition->SetAttribute( "value", list[k].GetType().c_str() );
        if ( list[k].IsInverted() ) { typeCondition->SetAttribute( "Contraire", "true" ); }
        else { typeCondition->SetAttribute( "Contraire", "false" ); }

        //Les autres param�tres
        for ( unsigned int l = 0;l < list[k].GetParameters().size();l++ )
        {
            TiXmlElement * Parametre = new TiXmlElement( "Parametre" );
            condition->LinkEndChild( Parametre );
            Parametre->SetAttribute( "value", list[k].GetParameter( l ).GetPlainString().c_str() );
        }

        //Sub instructions
        if ( !list[k].GetSubInstructions().empty() )
        {
            TiXmlElement * subConditions = new TiXmlElement( "SubConditions" );
            condition->LinkEndChild(subConditions);
            SaveConditions(list[k].GetSubInstructions(), subConditions);
        }
    }
}

void OpenSaveGame::OpenImagesFromGD2010498(Game & game, const TiXmlElement * imagesElem, const TiXmlElement * dossierElem)
{
    //Images
    while ( imagesElem )
    {
        ImageResource image;

        if ( imagesElem->Attribute( "nom" ) != NULL ) { image.SetName(imagesElem->Attribute( "nom" )); }
        else { MSG( "Les informations concernant le nom de l'image manquent." ); }
        if ( imagesElem->Attribute( "fichier" ) != NULL ) {image.GetFile() = imagesElem->Attribute( "fichier" ); }
        else { MSG( "Les informations concernant le fichier de l'image manquent." ); }

        image.smooth = true;
        if ( imagesElem->Attribute( "lissage" ) != NULL && string(imagesElem->Attribute( "lissage" )) == "false")
                image.smooth = false;

        image.alwaysLoaded = false;
        if ( imagesElem->Attribute( "alwaysLoaded" ) != NULL && string(imagesElem->Attribute( "alwaysLoaded" )) == "true")
                image.alwaysLoaded = true;

        game.GetResourcesManager().AddResource(image);
        imagesElem = imagesElem->NextSiblingElement();
    }

    //Dossiers d'images
    while ( dossierElem )
    {
        if ( dossierElem->Attribute( "nom" ) != NULL )
        {
            game.GetResourcesManager().CreateFolder( dossierElem->Attribute( "nom" ) );
            ResourceFolder & folder = game.GetResourcesManager().GetFolder(dossierElem->Attribute( "nom" ));

            const TiXmlElement *elemDossier = dossierElem;
            if ( elemDossier->FirstChildElement( "Contenu" ) != NULL )
            {
                elemDossier = elemDossier->FirstChildElement( "Contenu" )->FirstChildElement();
                while ( elemDossier )
                {
                    if ( elemDossier->Attribute( "nom" ) != NULL ) { folder.AddResource(elemDossier->Attribute( "nom" ), game.GetResourcesManager()); }
                    else { MSG( "Les informations concernant le nom d'une image d'un dossier manquent." ); }

                    elemDossier = elemDossier->NextSiblingElement();
                }
            }
        }

        dossierElem = dossierElem->NextSiblingElement();
    }
}

#endif