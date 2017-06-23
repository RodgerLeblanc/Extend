import bb.cascades 1.3

Container {
    signal animationEnded()
    
    function startAnimation() {
        mainAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        opacity = 0
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }
    
    animations: [
        FadeTransition {
            id: mainAnimation
            fromOpacity: 0
            toOpacity: 1
            delay: mainPage.generalAnimationDelay * 2
            duration: mainPage.generalAnimationDuration * 2
            onEnded: { animationEnded() }
        }
    ]
    
    layout: DockLayout {}
    minHeight: app.deviceInfo.height
    maxHeight: minHeight
    minWidth: app.deviceInfo.width
    maxWidth: minWidth

    opacity: 0
    
    Container {
        horizontalAlignment: HorizontalAlignment.Right
        verticalAlignment: VerticalAlignment.Bottom
        bottomPadding: ui.du(3)
        rightPadding: bottomPadding
        
        gestureHandlers: [ TapHandler { onTapped: { app.sendToHl("SEND_BUG_REPORT") } } ]
        
        ImageView {
            imageSource: "asset:///images/BugIcon.png"
            scalingMethod: ScalingMethod.AspectFit
            minWidth: ui.du(10)
            maxWidth: minWidth
            minHeight: minWidth
            maxHeight: minWidth
            horizontalAlignment: HorizontalAlignment.Center
        }
        Label {
            text: qsTr("Report a bug")
            horizontalAlignment: HorizontalAlignment.Center
            textStyle.fontSize: FontSize.Medium
            textStyle.textAlign: TextAlign.Center
            textStyle.color: Color.create("#00FF00")
            multiline: true
        }
    }
}
