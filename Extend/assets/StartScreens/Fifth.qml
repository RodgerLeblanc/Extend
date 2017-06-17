import bb.cascades 1.3

Container {
    signal animationEnded()
    
    function startAnimation() {
        mainAnimation.play()
    }
    
    function startSecondAnimation() {
        secondAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        if (secondAnimation.state == AnimationState.Playing || secondAnimation.state == AnimationState.Started) {
            secondAnimation.stop()
        }
        opacity = 0
        translationY = 0
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
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            onEnded: { animationEnded() }
        },
        TranslateTransition {
            id: secondAnimation
            fromY: translationY
            toY: 0 - app.deviceInfo.height
            delay: mainPage.generalAnimationDelay * 2
            duration: mainPage.generalAnimationDuration * 2
        }
    ]

    opacity: 0
    
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Bottom
    
    bottomPadding: ui.du(5)
    
    Label {
        text: Application.applicationName
        horizontalAlignment: HorizontalAlignment.Center
        textStyle.fontSize: FontSize.XLarge
        textStyle.color: Color.White
    }
    Label {
        text: qsTr("Automatically rename pictures without extension")
        horizontalAlignment: HorizontalAlignment.Center
        textStyle.fontSize: app.deviceInfo.width > 768 ? FontSize.Medium : FontSize.Small
        textStyle.textAlign: TextAlign.Center
        multiline: true
    }
}
