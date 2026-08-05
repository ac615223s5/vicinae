import QtQuick

/// Reusable delegate base for list items.  Provides a Source-blended
/// rounded-rect background that highlights on selection/hover, a MouseArea
/// for click handling, and a content slot for view-specific layouts.
Item {
    id: root

    property bool selected: false
    readonly property bool hovered: mouseArea.containsMouse && HoverActivation.active

    default property alias contentData: contentItem.data

    signal clicked
    signal activated

    // Hover tracking only; buttons are handled by the TapHandler below. A
    // MouseArea never sees the press on rows that also carry an enabled
    // DragHandler (file drag, favorites reorder): DragHandler is a multi-point
    // handler, which accepts the press point when it takes its passive grab,
    // and that ends delivery before any item (including this MouseArea) gets
    // it. TapHandler grabs passively without accepting, so it coexists.
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }

    TapHandler {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onSingleTapped: (eventPoint, button) => {
            root.clicked();
            if (button === Qt.RightButton) {
                // right click selects the item and opens the action panel;
                // actionPanel is a launcher-window context property, absent in
                // other engines (e.g. settings window) that reuse this delegate
                if (typeof actionPanel !== "undefined")
                    actionPanel.toggle();
                return;
            }
            if (Config.activateOnSingleClick)
                root.activated();
        }
        onDoubleTapped: (eventPoint, button) => {
            if (button === Qt.LeftButton)
                root.activated();
        }
    }

    SourceBlendRect {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        radius: 10
        backgroundColor: {
            var bg = Theme.background;
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
        }
        color: {
            if (root.selected) {
                var c = Theme.listItemSelectionBg;
                return Qt.rgba(c.r, c.g, c.b, Config.surfaceOpacity);
            }
            if (root.hovered) {
                var h = Theme.listItemHoverBg;
                return Qt.rgba(h.r, h.g, h.b, Config.surfaceOpacity);
            }
            var bg = Theme.background;
            return Qt.rgba(bg.r, bg.g, bg.b, Config.windowOpacity);
        }
    }

    Item {
        id: contentItem
        anchors.fill: parent
    }
}
