import bb.cascades 1.3

MultiCover {
    SceneCover {
        id: bigCover
        MultiCover.level: CoverDetailLevel.High
        
        content: Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill    
            background: Color.create("#404040")
            
            ImageView {
                imageSource: "asset:///images/ExtendLogo_1440.png"
                scalingMethod: ScalingMethod.AspectFit
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
        }
    } // sceneCover HIGH
    
    SceneCover {
        id: smallCover
        MultiCover.level: CoverDetailLevel.Medium
        
        content: Container {
            layout: DockLayout {}
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill    
            background: Color.create("#404040")
            
            ImageView {
                imageSource: "asset:///images/ExtendLogo_1440.png"
                scalingMethod: ScalingMethod.AspectFit
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
            }
        }
    } // sceneCover MEDIUM
}