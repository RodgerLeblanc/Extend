import bb.cascades 1.3

Container {
    id: first
    property string imageSource: "asset:///images/PassportAllBlurTransparentBackground.png"
    
    signal animationEnded()

    function startAnimation() {
        reset()
        mainAnimation.play()
    }
    
    function startFinalAnimation() {
        finalAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        translationX = app.deviceInfo.width
        scaleX = 1
        scaleY = 1
        imageSource = "asset:///images/PassportAllBlurTransparentBackground.png"
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }
    
    animations: [
        TranslateTransition {
            id: mainAnimation
            fromX: app.deviceInfo.width
            toX: 0
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration
            onEnded: { animationEnded() }
        },
        ScaleTransition {
            id: finalAnimation
            fromX: 1
            toX: 0
            fromY: fromX
            toY: toX
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration
        }
    ]

    translationX: app.deviceInfo.width
    scaleX: 1
    scaleY: 1

    minWidth: app.deviceInfo.width
    maxWidth: minWidth
    
    ImageView {
        id: image
        horizontalAlignment: HorizontalAlignment.Center
        imageSource: first.imageSource
        scalingMethod: ScalingMethod.None
        minHeight: app.deviceInfo.height
        maxHeight: minHeight
    }
}
