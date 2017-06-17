import bb.cascades 1.3

ImageView {
    signal animationEnded()
    signal secondAnimationEnded()
    
    function startAnimation() {
        mainAnimation.play()
    }
    
    function startSecondAnimation() {
        secondAnimation.play()
    }
    
    function startThirdAnimation() {
        thirdAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        if (mainAnimation2.state == AnimationState.Playing || mainAnimation2.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        if (secondAnimation.state == AnimationState.Playing || secondAnimation.state == AnimationState.Started) {
            secondAnimation.stop()
        }
        if (thirdAnimation.state == AnimationState.Playing || thirdAnimation.state == AnimationState.Started) {
            thirdAnimation.stop()
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
                mainAnimation2.play()
            }
        },
        TranslateTransition {
            id: mainAnimation2
            fromX: mainAnimation.toX
            toX: app.deviceInfo.width
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            onEnded: { animationEnded() }
        },
        TranslateTransition {
            id: secondAnimation
            fromX: mainAnimation2.toX
            toX: mainAnimation.toX
            delay: mainPage.generalAnimationDelay
            duration: mainPage.generalAnimationDuration * 2
            onStarted: {
                translationY = (app.deviceInfo.height / 2) - (minHeight / 2)
            }
            onEnded: { secondAnimationEnded() }
        },
        TranslateTransition {
            id: thirdAnimation
            fromY: translationY
            toY: 0 - app.deviceInfo.height
            delay: mainPage.generalAnimationDelay * 2
            duration: mainPage.generalAnimationDuration * 2
        }
    ]
    
    translationX: 0 - app.deviceInfo.width
    translationY: app.deviceInfo.height / 6

    minWidth: app.deviceInfo.width * 0.65
    maxWidth: minWidth
    minHeight: minWidth
    maxHeight: minWidth
    
    imageSource: "asset:///images/ExtendLogo_1440.png"
    scalingMethod: ScalingMethod.AspectFit
}
