import bb.cascades 1.3

Container {
    id: changelogContainer
    property alias changelogText: changelogLabel.text
    property alias changelogBorderColor: borderContainer.background
    property alias changelogInnerBackground: innerContainer.background
    property variant changelogTextColor: Color.Black
    property int countdownSeconds: 10

    onCreationCompleted: { enableOkButtonTimer.start() }

    attachedObjects: [
        QTimer {
            id: enableOkButtonTimer
            interval: 1000
            onTimeout: { 
                countdownSeconds-- 
                if (countdownSeconds <= 0) {
                    enableOkButtonTimer.stop()
                }
            }
        }
    ]

    layout: DockLayout {}
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: Color.White
    
    Container {
        layout: DockLayout {}
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        ScrollView {
            id: changelogScrollview
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            Container {
                layout: DockLayout {}
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                topPadding: ui.du(5)
                bottomPadding: topPadding
                leftPadding: topPadding
                rightPadding: topPadding
                Container {
                    id: borderContainer
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    background: Color.Black
                    topPadding: ui.du(1)
                    bottomPadding: topPadding
                    leftPadding: topPadding
                    rightPadding: topPadding
                    Container {
                        id: innerContainer
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        background: Color.White
                        topPadding: ui.du(3)
                        bottomPadding: topPadding
                        leftPadding: topPadding
                        rightPadding: topPadding
                        Label {
                            text: qsTr("Changelog") + Retranslate.onLocaleOrLanguageChanged
                            textStyle.fontSize: FontSize.XXLarge
                            horizontalAlignment: HorizontalAlignment.Center
                            textStyle.color: changelogTextColor
                        }
                        Label {
                            id: changelogLabel
                            multiline: true
                            horizontalAlignment: HorizontalAlignment.Center
                            textStyle.textAlign: TextAlign.Justify
                            content.flags: TextContentFlag.ActiveText
                            textStyle.color: changelogTextColor
                        }
                        Button {
                            text: ((countdownSeconds > 0) ? countdownSeconds : qsTr("OK") + Retranslate.onLocaleOrLanguageChanged)
                            enabled: (countdownSeconds <= 0)
                            horizontalAlignment: HorizontalAlignment.Center
                            onClicked: {
                                changelogScrollview.scrollToPoint(0, 0)
                                changelogSheet.close()
                            }
                        }
                    }
                }
            }
        }
    }
}
