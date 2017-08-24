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
        translationY = app.deviceInfo.height
    }
    
    onCreationCompleted: {
        reset()
        Application.thumbnail.connect(reset)
    }
    
    animations: [
        TranslateTransition {
            id: mainAnimation
            fromY: app.deviceInfo.height
            toY: 0
            delay: mainPage.generalAnimationDelay * 2
            duration: mainPage.generalAnimationDuration * 2
            onEnded: { animationEnded() }
        }
    ]
    
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Center
    translationY: app.deviceInfo.height
        
    Label {
        text: qsTr("This app runs in the background, you don't need to keep it open as an active frame. There are no settings whatsoever, you just keep it installed and it will automatically rename any file saved without an extension.")
        horizontalAlignment: HorizontalAlignment.Center
        textStyle.fontSize: FontSize.Medium
        textStyle.textAlign: TextAlign.Justify
        multiline: true
    }
    Label {
        text: qsTr("Supports: BMP, ICO, JPG, GIF, PNG, TIFF, M4A, MP4, MPG, BPG, IFF, ZIP, RAR, PDF, ASF, OGG, PSD, WAV, AVI, MP3, ISO, FLAC, MIDI, XAR, TAR, MLV, 7Z, GZ, MKV, XML, SWF, DEB, WEBP")
        horizontalAlignment: HorizontalAlignment.Center
        textStyle.fontSize: FontSize.Medium
        textStyle.textAlign: TextAlign.Center
        multiline: true
    }
}
