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
import "Changelog"
import "StartScreens"

Page {
    id: mainPage
    property variant mainBackground: Color.create("#000000")
    property int generalAnimationDelay: 750
    property int generalAnimationDuration: 750

    onCreationCompleted: {
        Application.menuEnabled = false
        Application.setCover(activeFrame)
        Application.fullscreen.connect(reset)
        
        app.newChangelog.connect(onNewChangelog)
    }
    
    function reset() {
        mainContainer.translationY = 0
        background.startAnimation()
    }
    
    function onNewChangelog(newChangelogForThisUser) {
        console.log("onNewChangelog: " + newChangelogForThisUser)
        changelogButton.changelog = newChangelogForThisUser
    }
    
    attachedObjects: [
        RenderFence {
            raised: true
            onReached: {
                background.startAnimation()
                app.checkForChangelog()
            }
        },
        AppCover { id: activeFrame },
        ComponentDefinition { id: changelogSheet; ChangelogSheet {} }
    ]
    
    Container {
        id: mainContainer
        layout: DockLayout {}
        background: mainPage.mainBackground
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        translationY: 0
        
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
            onSecondAnimationEnded: {
                fifth.startAnimation()
            }
        }

        Fourth {
            id: fourth
            onAnimationEnded: {
                first.startSecondAnimation()
                third.startSecondAnimation()
            }
        }
        
        Fifth {
            id: fifth
            onAnimationEnded: {
                background.startSecondAnimation()
                third.startThirdAnimation()
                fifth.startSecondAnimation()
                sixth.startAnimation()
            }
        }
        
        Sixth {
            id: sixth
            onAnimationEnded: {
                bugReport.startAnimation()
                changelogButton.startAnimation()
            }
        }
        
        BugReport {
            id: bugReport
        }
        
        ChangelogButton {
            id: changelogButton
            property string changelog
            
            onChangelogClicked: {
                var createdControl = changelogSheet.createObject()
                createdControl.changelogText = changelog
                
                //If you don't specify countdownSeconds, it will default to 10 
                createdControl.countdownSeconds = 0
                
                //If you don't specify colors, it will default to white background with black border and text
                createdControl.changelogOuterBackground = Color.Black
                createdControl.changelogBorderColor = Color.White
                createdControl.changelogInnerBackground = Color.Black
                createdControl.changelogTextColor = Color.White
                
                createdControl.open()
            }
        }
    }
}
