/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.3
import "StartScreens"

Page {
    id: mainPage
    property variant mainBackground: Color.create("#404040")
    property int generalAnimationDelay: 500
    property int generalAnimationDuration: 500

    attachedObjects: [
        RenderFence {
            raised: true
            onReached: {
                background.startAnimation()
            }
        },
        AppCover { id: activeFrame }
    ]
    
    function reset() {
        background.startAnimation()
    }
    
    onCreationCompleted: {
        Application.menuEnabled = false
        Application.setCover(activeFrame)
        Application.fullscreen.connect(reset)
    }
    
    Container {
        id: mainContainer
        layout: DockLayout {}
        background: mainPage.mainBackground
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        Background {
            id: background
            onAnimationEnded: {
                first.startAnimation()
            }
        }
        
        First {
            id: first
            onAnimationEnded: {
                second.startAnimation()
            }
        }

        Second {
            id: second
            onAnimationEnded: {
                third.startAnimation()
            }
        }

        Third {
            id: third
            onAnimationEnded: {
                fourth.startAnimation()
            }
        }

        Fourth {
            id: fourth
            onAnimationEnded: {
                first.startFinalAnimation()
                third.startFinalAnimation()
            }
        }
    }
}
