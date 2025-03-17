import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 600
    height: 700
    title: "2048"
    property int tailleDamier: 4
    property bool perdu: false  // Ajout d'une propriété pour suivre l'état du jeu
    property bool direction: true
    property bool isVertical: false // Set this dynamically

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
        interactive: false // Disable scrolling

        Connections {
            target: monDamier
            function onDeplace() {
                // Process the movement animation when movement occurs
                grille.animateMovement();
            }
        }


        function animateMovement() {
            // This will trigger the animations for each tile
            for (var i = 0; i < tailleDamier; i++) {
                for (var j = 0; j < tailleDamier; j++) {
                    var tileIdx = i * tailleDamier + j;
                    var tile = grille.itemAtIndex(tileIdx);
                    if (tile) {
                        tile.updatePosition();
                    }
                }
            }
        }


        delegate: Rectangle {
            id: cellBackground
            width: grille.cellWidth
            height: grille.cellHeight
            color: "#bbae9e"

            // Store position information
            property int row: Math.floor(index / tailleDamier)
            property int column: index % tailleDamier
            property point gridPosition: Qt.point(column, row)

            z: direction ? -row-column : row+column // Non-empty tiles are always in front of empty ones


            // For animations and tracking
            property point previousPosition: gridPosition
            property int newPosition: -1

            function updatePosition() {
                // Only process for non-empty tiles
                if (modelData !== 0) {
                    // Check if there's movement data for this tile

                    newPosition = monDamier.deplacement[column + row*tailleDamier];

                        // If newColumn is valid
                        if (newPosition !== -1) {
                            // Start the animation
                            moveAnimation.start();
                        }
                    }
                }


            function getColor() { return (modelData === 0 && "#cdc1b5") ||
                                  (modelData === 2 && "#eee4da") ||
                                  (modelData === 4 && "#ece0c8") ||
                                  (modelData === 8 && "#f2b179") ||
                                  (modelData === 16 && "#f59563") ||
                                  (modelData === 32 && "#f67c5f") ||
                                  (modelData === 64 && "#f65e3b") ||
                                  (modelData === 128 && "#edcf72") ||
                                  (modelData === 256 && "#edcc61") ||
                                  (modelData === 512 && "#edc850") ||
                                  (modelData === 1024 && "#edc53f") ||
                                  (modelData === 2048 && "#edc22e") ||
                                  "#cdc1b4"}

            Rectangle {
                id: vide
                width: grille.cellWidth - 60/tailleDamier
                height: grille.cellHeight - 60/tailleDamier
                color: "#cdc1b5"
                radius: 14/tailleDamier

                x: (parent.width - width) / 2
                y: (parent.height - height) / 2

            Rectangle {
                id: tile
                width: grille.cellWidth - 60/tailleDamier
                height: grille.cellHeight - 60/tailleDamier
                color: getColor()
                radius: 14/tailleDamier
                x: (parent.width - width) / 2
                y: (parent.height - height) / 2
                // Only show non-zero tiles
                visible: modelData !== 0

                Text {
                    anchors.centerIn: parent
                    text: modelData !== 0 ? modelData : ""
                    font.pixelSize: 60*4/tailleDamier
                    font.weight: Font.Bold
                    color: modelData === 2 || modelData === 4 ? "#766f65" : "white"
                }

                // Animation for tile movement
                ParallelAnimation {
                    id: moveAnimation
                NumberAnimation {
                    target: tile
                    property: isVertical ? "y" : "x"
                    from: 0
                    to: isVertical
                        ? (cellBackground.newPosition - cellBackground.row) * grille.cellHeight
                        : (cellBackground.newPosition - cellBackground.column) * grille.cellWidth
                    duration: 200
                    easing.type: Easing.OutQuad
                    onStarted: {
                        console.log("Animation started with from: " + from + ", to: " + to);
                    }
                }

                    onFinished: {
                        // Reset the x position after animation to ensure tiles stay in grid cells
                        if (isVertical) {
                                   tile.y = 0;
                               } else {
                                   tile.x = 0;
                               }
                        tile.color = getColor()

                        Ondeplace: {monDamier.suivant();}
                    }
                }

                // Animation d'apparition pour une nouvelle tuile
                SequentialAnimation on scale {
                    running: row === monDamier.lastAddedTile[0] && column === monDamier.lastAddedTile[1]
                    NumberAnimation { from: 0; to: 1; duration: 150; easing.type: Easing.OutBounce }
                }
            }
        }}


        // Gestionnaire des actions clavier
        focus: true

        Keys.onPressed: function(event) {
            switch (event.key) {
                case Qt.Key_Up:
                    monDamier.haut()
                    direction = false
                    isVertical= true
                    break;
                case Qt.Key_Down:
                    monDamier.bas()
                    direction = true
                    isVertical= true
                    break;
                case Qt.Key_Left:
                    monDamier.gauche()
                    direction = false
                    isVertical= false
                    break;
                case Qt.Key_Right:
                    monDamier.droite()
                    direction = true
                    isVertical= false
                    break;
                default:
                    break;
            }


            // Vérifier si la partie est perdue après chaque mouvement
            if (monDamier.perdu()) {
                perdu = true;  // Mettre à jour l'état si perdu
            }
        }



        // Detection du swippe avec 2 doigts
        MouseArea {
                property bool wheelLocked: false  // Empêche l'appel multiple immédiat
                property string lastMove: "None"  // si le même mouvement est répété

                id: wheelHandler
                anchors.fill: parent
                // Le défilement à 2 doigts sur le tracpad est détecté par l'ordinateur comme la molette de la souris (mais avec 2 directions x et y)
                onWheel: function(event) {
                    if (event.angleDelta.y < 0) {  // Scrolling down
                        if (lastMove === "Down" && wheelLocked) return;
                        lastMove = "Down";
                        monDamier.bas()
                        direction = true
                        isVertical= true

                    } else if (event.angleDelta.y > 0) {  // Scrolling up
                        if (lastMove === "Up" && wheelLocked) return;
                        lastMove = "Up";
                        monDamier.haut()
                        direction = false
                        isVertical= true
                    }

                    if (event.angleDelta.x < 0) {  // Scrolling right
                        if (lastMove === "Right" && wheelLocked) return;
                        lastMove = "Right";
                        monDamier.droite()
                        direction = true
                        isVertical= false
                    } else if (event.angleDelta.x > 0) {  // Scrolling left
                        if (lastMove === "Left" && wheelLocked) return;
                        lastMove = "Left";
                        monDamier.gauche()
                        direction = false
                        isVertical= false
                    }

                    wheelLocked = true;
                    delayTimer.restart();  // Démarre le délai

                    // Vérifier si la partie est perdue après chaque mouvement
                    if (monDamier.perdu()) {
                        perdu = true;  // Mettre à jour l'état si perdu
                    }

                    event.accepted = true;  // Prevent event propagation
                }

            Timer {
                id: delayTimer
                interval: 500  // Temps d'attente en millisecondes (400 ms)
                onTriggered: parent.wheelLocked = false
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
