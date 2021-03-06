

import Foundation
import SpriteKit
import SceneKit

class PlutoboyScene:  SKScene {
    
    let GB_PIX_HEIGHT = 144;
    let GB_PIX_WIDTH = 160;
    var x = 0;
    
    var gameDisplay : SKSpriteNode!
    var initialTexture : SKTexture!;
    
    override func sceneDidLoad() {
        
        let height = GB_PIX_HEIGHT;
        let width = GB_PIX_WIDTH;
    
       self.scaleMode = SKSceneScaleMode.aspectFill

        gameDisplay = SKSpriteNode(color: .black, size:CGSize(width: 1.0, height: 1.0));
        
        gameDisplay.position = CGPoint(x: width/2, y: height/2) // Centre the sprite
        gameDisplay.zPosition = 1
        gameDisplay.yScale *= -1; // Need to vertically mirror image
                                  // to correct it from Texture generated
        
        self.addChild(gameDisplay);
     }
    
    func updateDisplay(texture: SKTexture) {
        let set_new_texture = SKAction.setTexture(texture, resize:false);
        self.gameDisplay.run(set_new_texture);
        
    }
}
