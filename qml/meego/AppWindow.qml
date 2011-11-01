import QtQuick 1.1
import com.meego 1.0
import com.meego.extras 1.0

PageStackWindow {
    id: appWindow
    initialPage: newGamePage

    NewGamePage {
        id: newGamePage
    }

    QueryDialog {
        id: quitDialog
        message: "Do you want to quit the game?"
        acceptButtonText: "Yeah"
        rejectButtonText: "Nope"
        titleText: "Are you sure?"
        onAccepted: Qt.quit()
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                text: "Quit"
                onClicked: quitDialog.open()
            }
        }
    }
}
