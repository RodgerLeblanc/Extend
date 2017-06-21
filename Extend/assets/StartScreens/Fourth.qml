import bb.cascades 1.3

Container {
    id: fourth
    property string imageSource: "asset:///images/AwesomePicRenamedPositioned.png"
    
    signal animationEnded()

    property int flashDelay: 10
    property int flashDuration: 50
        
    function startAnimation() {
        mainAnimation.play()
    }
    
    function reset() {
        if (mainAnimation.state == AnimationState.Playing || mainAnimation.state == AnimationState.Started) {
            mainAnimation.stop()
        }
        imageSource: "asset:///images/AwesomePicRenamedPositioned.png"
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
                delay: mainPage.generalAnimationDelay * 6
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

    verticalAlignment: VerticalAlignment.Center

    ImageView {
        imageSource: fourth.imageSource
        scalingMethod: ScalingMethod.AspectFit
        minWidth: app.deviceInfo.width * 0.75
        maxWidth: minWidth
        horizontalAlignment: HorizontalAlignment.Center
    }
}
