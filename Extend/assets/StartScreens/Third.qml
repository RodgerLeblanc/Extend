import bb.cascades 1.3

ImageView {
    signal animationEnded()
    
    function startAnimation() {
        mainAnimation.play()
    }
    
    function startFinalAnimation() {
        finalAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        if (secondAnimation.state == AnimationState.Playing || secondAnimation.state == AnimationState.Started) {
            secondAnimation.stop()
        }
        translationX = 0 - app.deviceInfo.width
        translationY = app.deviceInfo.height / 6
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }
    
    animations: [
        TranslateTransition {
            id: mainAnimation
            fromX: 0 - app.deviceInfo.width
            toX: (app.deviceInfo.width / 2) - (minWidth / 2)
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            easingCurve: StockCurve.ExponentialOut
            onEnded: { 
                first.imageSource = "asset:///images/PassportAllBlurIncludingAwesomePicRenamedTransparentBackground.png"
                second.imageSource = "asset:///images/AwesomePicRenamedPositioned.png" 
                secondAnimation.play()
            }
        },
        TranslateTransition {
            id: secondAnimation
            fromX: mainAnimation.toX
            toX: app.deviceInfo.width
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            onEnded: { animationEnded() }
        },
        TranslateTransition {
            id: finalAnimation
            fromX: secondAnimation.toX
            toX: mainAnimation.toX
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            onStarted: {
                translationY = (app.deviceInfo.height / 2) - (minHeight / 2)
            }
            onEnded: {  }
        }
    ]
    
    translationX: 0 - app.deviceInfo.width
    translationY: app.deviceInfo.height / 6

    minWidth: app.deviceInfo.width/ 2
    maxWidth: minWidth
    minHeight: minWidth
    maxHeight: minWidth
    
    imageSource: "asset:///images/ExtendLogo_1440.png"
    scalingMethod: ScalingMethod.AspectFit
}
