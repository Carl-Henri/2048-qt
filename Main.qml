import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 600
    height: 700
    title: "2048"
    property int tailleDamier: 4
    property bool perdu: false  // Ajout d'une propriété pour suivre l'état du jeu

    // Affichage du score
    Label {
        id: scoreLabel
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        text: "Score: " + monDamier.scoreQML
        font.pixelSize: 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "black"
        background: Rectangle {
            color: "#faf8ef"
        }
    }

    // Conteneur pour aligner les boutons
    Row {
        id: buttonRow
        anchors.top: scoreLabel.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20
        width: parent.width - 40 // Laisser un espace de marge de chaque côté
        height: 60 // Hauteur des boutons

        // Bouton de redémarrage
        Button {
            text: "Redémarrer"
            width: parent.width / 2 - 10
            height: buttonRow.height - 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 18
            onClicked: {
                monDamier.reinitialiser();  // Fonction à définir dans DamierDyn pour réinitialiser
                perdu = false
                grille.forceActiveFocus();  // Force la focalisation sur l'élément GridView
            }
        }

        // Bouton pour choisir le mode de jeu
        Button {
            id: modeButton
            text: "Choisir le mode"
            width: parent.width / 2 - 10
            height: buttonRow.height - 20
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 18
            onClicked: {
                modeMenu.open(); // Afficher le menu pour choisir le mode de jeu
                grille.forceActiveFocus();  // Force la focalisation sur l'élément GridView
            }
        }
    }

    // Vue du damier
    GridView {
        id: grille
        anchors.top: buttonRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom  // S'assure que la grille occupe l'espace restant
        cellWidth: width / tailleDamier
        cellHeight: width / tailleDamier
        model: monDamier.table

        delegate: Rectangle {
            width: grille.cellWidth
            height: grille.cellHeight
            color:"#bbae9e"
            Rectangle {
                width: grille.cellWidth - 60/tailleDamier
                height: grille.cellHeight - 60/tailleDamier
                color: (modelData === 0 && "#cdc1b5") || (modelData === 2 && "#eee4da") || (modelData === 4 && "#ece0c8") || (modelData === 8 && "#f2b179") || (modelData === 16 && "#f59563") || (modelData === 32 && "#f67c5f") || (modelData === 64 && "#f65e3b") || (modelData === 128 && "#edcf72") || (modelData === 256 && "#edcc61") || (modelData === 512 && "#edc850") || (modelData === 1024 && "#edc53f") || (modelData === 2048 && "#edc22e") || "#cdc1b4"
                radius: 14/tailleDamier
                anchors.centerIn:parent

                Text {
                    anchors.centerIn: parent
                    text: modelData !== 0 ? modelData : ""
                    font.pixelSize: 60*4/tailleDamier
                    font.weight: Font.Bold
                    color: modelData === 2 || modelData === 4 ? "#766f65":"white"
                }
            }
        }

        // Gestionnaire des actions clavier
        focus: true
        Keys.onPressed: function(event) {
            switch (event.key) {
                case Qt.Key_Up:
                    if (monDamier.haut()) monDamier.suivant();
                    break;
                case Qt.Key_Down:
                    if (monDamier.bas()) monDamier.suivant();
                    break;
                case Qt.Key_Left:
                    if (monDamier.gauche()) monDamier.suivant();
                    break;
                case Qt.Key_Right:
                    if (monDamier.droite()) monDamier.suivant();
                    break;
                default:
                    break;
            }

            // Vérifier si la partie est perdue après chaque mouvement
            if (monDamier.perdu()) {
                perdu = true;  // Mettre à jour l'état si perdu
            }
        }
    }

    // Menu pour choisir le mode de jeu
    Menu {
        id: modeMenu
        width: 180
        height: 220  // Hauteur totale du menu
        anchors.centerIn: parent  // Centrer le menu dans l'écran

        // Conteneur pour les boutons
        Column {
            id: buttonContainer
            anchors.fill: parent
            anchors.top: parent.top
            spacing: 10  // Espacement entre les boutons
            padding: 10  // Marges autour des boutons
            property int buttonCount: 6  // Nombre total de boutons

            Repeater {
                model: ["3x3", "4x4", "5x5", "6x6","7x7","8x8"]

                Button {
                    text: modelData
                    height: (modeMenu.height - (buttonContainer.spacing * (buttonContainer.buttonCount - 1)) - (buttonContainer.padding * 2)) / buttonContainer.buttonCount
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 20
                    onClicked: {
                        tailleDamier = parseInt(text[0]);  // Extrait le premier chiffre du texte
                        monDamier.redim(tailleDamier, tailleDamier);
                        monDamier.reinitialiser();
                        modeMenu.close();  // Ferme le menu après la sélection
                    }
                }
            }
        }
    }

    // Popup en cas de perte avec un Dialog
    Dialog {
        id: dialog
        title: "Jeu terminé"
        visible: perdu  // Le dialogue est visible seulement si la partie est perdue
        modal: true  // Rendre le dialogue modal, il bloque l'interface principale
        standardButtons: Dialog.Ok
        width: 300
        height: 150
        anchors.centerIn: parent

        contentItem: Rectangle {
            width: 300
            height: 150
            color: "#f8d7da"
            border.color: "#721c24"
            radius: 10

            Text {
                anchors.centerIn: parent
                text: "Impossible de fusionner.\nVous avez perdu !"
                color: "#721c24"
                font.pixelSize: 18
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }

        onAccepted: {
            // Réinitialiser le jeu si nécessaire
            monDamier.reinitialiser();
            perdu = false;  // Réinitialiser l'état de la partie
        }
    }
}
