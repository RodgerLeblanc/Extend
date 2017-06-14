import bb.cascades 1.3

ImageView {
    signal animationEnded()
    
    function startAnimation() {
        mainAnimation.play()
    }

    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        translationY = 0
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }

    animations: [
        TranslateTransition {
            id: mainAnimation
            fromY: 0
            toY: 0 - app.deviceInfo.height
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration
            onEnded: { animationEnded() }
        }
    ]
    
    translationY: 0

    imageSource: "asset:///images/Background.png"
    scalingMethod: ScalingMethod.None
    minHeight: app.deviceInfo.height * 2
    maxHeight: minHeight
}
