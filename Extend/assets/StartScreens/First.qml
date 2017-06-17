import bb.cascades 1.3

Container {
    id: first
    property string imageSource: "asset:///images/PassportAllBlurTransparentBackground.png"
    
    signal animationEnded()

    function startAnimation() {
        reset()
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
            id: secondAnimation
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
    
    verticalAlignment: VerticalAlignment.Center
    
    ImageView {
        id: image
        horizontalAlignment: HorizontalAlignment.Center
        imageSource: first.imageSource
        scalingMethod: ScalingMethod.AspectFit
        minWidth: app.deviceInfo.width * 0.75
        maxWidth: minWidth
    }
}
