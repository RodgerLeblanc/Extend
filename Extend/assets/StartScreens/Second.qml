import bb.cascades 1.3

Container {
    id: second
    property string imageSource: "asset:///images/AwesomePicPositioned.png"
    
    signal animationEnded()

    property int flashDelay: 10
    property int flashDuration: 50
        
    function startAnimation() {
        reset()
        mainAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        imageSource = "asset:///images/AwesomePicPositioned.png"
        opacity = 0
        scaleX = 1
        scaleY = scaleX
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }
    
    animations: [
        SequentialAnimation {
            id: mainAnimation
            
            ParallelAnimation {
                delay: mainPage.generalAnimationDelay
                ScaleTransition {
                    fromX: 1
                    toX: 2
                    fromY: 1
                    toY: 2
                    duration: mainPage.generalAnimationDuration
                }
                FadeTransition {
                    fromOpacity: 0
                    toOpacity: 1
                    duration: mainPage.generalAnimationDuration
                }
            }
            ParallelAnimation {
                delay: mainPage.generalAnimationDelay * 4
                ScaleTransition {
                    fromX: 2
                    toX: 1
                    fromY: 2
                    toY: 1
                    duration: mainPage.generalAnimationDuration
                }
                FadeTransition {
                    fromOpacity: 1
                    toOpacity: 0
                    duration: mainPage.generalAnimationDuration
                }
            }
            onEnded: { animationEnded() }
        }
    ]

    opacity: 0
    scaleX: 1
    scaleY: scaleX

    minWidth: app.deviceInfo.width
    maxWidth: minWidth
    
    ImageView {
        id: image
        imageSource: "asset:///images/AwesomePicPositioned.png"
        scalingMethod: ScalingMethod.None
        minHeight: app.deviceInfo.height
        maxHeight: minHeight
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
    }
}
