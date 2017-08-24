import bb.cascades 1.3

Sheet {
    id: changelogSheet
    property alias changelogText: changelogContainer.changelogText
    property alias changelogOuterBackground: changelogContainer.background
    property alias changelogBorderColor: changelogContainer.changelogBorderColor
    property alias changelogInnerBackground: changelogContainer.changelogInnerBackground
    property alias changelogTextColor: changelogContainer.changelogTextColor
    property alias countdownSeconds: changelogContainer.countdownSeconds

    signal changelogDone();
    
    onClosed: { 
        changelogDone()
        destroy()
    }

    Page {
        Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            ChangelogContainer { id: changelogContainer }
        }
    }
}