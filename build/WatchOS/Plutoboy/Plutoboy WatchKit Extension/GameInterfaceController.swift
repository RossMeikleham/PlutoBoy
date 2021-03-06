//
//  GameInterfaceController.swift
//  Plutoboy WatchKit Extension
//
//  Created by Ross Meikleham on 06/03/2021.
//

import WatchKit
import Foundation
import UIKit
import SpriteKit
import CoreData

let GB_PIX_HEIGHT = 144;
let GB_PIX_WIDTH = 160;
var f = 0;

public func callsDisplayFrame(slf: UnsafeRawPointer, modifier: UnsafeMutablePointer<UInt32>) {
    let controller = Unmanaged<GameInterfaceController>.fromOpaque(slf).takeUnretainedValue();
    controller.displayFrame(pixels: modifier);
}

protocol Button {
    func set_button(state: Int32)
}

class ButtonUp : Button {
    func set_button(state: Int32) {
        set_up_pressed(state);
    }
 }

class ButtonDown : Button {
    func set_button(state: Int32) {
        set_down_pressed(state);
    }
 }

class ButtonLeft : Button {
    func set_button(state: Int32) {
        set_left_pressed(state);
    }
 }

class ButtonRight : Button {
    func set_button(state: Int32) {
        set_right_pressed(state);
    }
 }

class ButtonA : Button{
    func set_button(state: Int32) {
        set_a_pressed(state);
    }
 }

class ButtonB : Button {
    func set_button(state: Int32) {
        set_b_pressed(state);
    }
 }

class ButtonStart : Button {
    func set_button(state: Int32) {
        set_start_pressed(state);
    }
 }

class ButtonSelect : Button {
    func set_button(state: Int32) {
        set_select_pressed(state);
    }
 }

class ButtonNone : Button {
    func set_button(state: Int32) {
        // Do nothing, used for completeness
    }
}

// TODO we use a single longpress, find a way to
// make multiple long press's within the SpriteNode
// . This would enable multitouch and wouldn't require
// co-ordinate detection for what point is pressed like below.
func button_from_point(point: CGPoint) -> Button {
    let y = Int(point.y);
    let x = Int(point.x);
    if y < GB_PIX_HEIGHT / 3 {
        if x < GB_PIX_WIDTH / 3 {
            return ButtonA();
        }
        if x < (GB_PIX_WIDTH * 2) / 3 {
            return ButtonUp();
        }
        return ButtonB();
    }
    if y < (GB_PIX_HEIGHT * 2) / 3 {
        if x < GB_PIX_WIDTH / 3 {
            return ButtonLeft();
        }
        if x < (GB_PIX_WIDTH * 2) / 3 {
            return ButtonNone();
        }
        return ButtonRight();
    }
    if x < GB_PIX_WIDTH / 3 {
        return ButtonStart();
    }
    if x < (GB_PIX_WIDTH * 2) / 3 {
        return ButtonDown();
    }
    return ButtonSelect();
}


class GameInterfaceController: WKInterfaceController {
    
    static var gamePath: String?;
    static var reloaded = false;
    
    @IBOutlet weak var spriteKitScene: WKInterfaceSKScene!;
    @IBOutlet weak var press: WKLongPressGestureRecognizer!;
    @IBOutlet weak var tap: WKTapGestureRecognizer!;
    
    var lastButton : Button!;
    
    private var scene: PlutoboyScene!;
    private var displayTexture: SKMutableTexture!;
    private var audioManager: AudioManager!;
    private var persistentContainer: NSPersistentContainer!;
    
    
    @IBAction func longPressGestureHandler(sender: WKLongPressGestureRecognizer) {
        let point = sender.locationInObject();
  
        let button = button_from_point(point: point);

        switch sender.state {
        case WKGestureRecognizerState.began:
            button.set_button(state: 1);
            lastButton = button;
        case WKGestureRecognizerState.changed:
            if object_getClassName(button) != object_getClassName(lastButton) {
                lastButton.set_button(state:0);
                button.set_button(state:1);
            }
        case WKGestureRecognizerState.ended:
            button.set_button(state: 0);
        case WKGestureRecognizerState.cancelled:
            button.set_button(state: 0);
        default:
            return;
        }
    }
    
    func set_persistent_container() {
        persistentContainer = {
                /*
                 The persistent container for the application. This implementation
                 creates and returns a container, having loaded the store for the
                 application to it. This property is optional since there are legitimate
                 error conditions that could cause the creation of the store to fail.
                 */
            let container = NSPersistentContainer(name: "Model")
                container.loadPersistentStores(completionHandler: { (storeDescription, error) in
                    if let error = error as NSError? {
                        // Replace this implementation with code to handle the error appropriately.
                        // fatalError() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
                        
                        /*
                         Typical reasons for an error here include:
                         * The parent directory does not exist, cannot be created, or disallows writing.
                         * The persistent store is not accessible, due to permissions or data protection when the device is locked.
                         * The device is out of space.
                         * The store could not be migrated to the current model version.
                         Check the error message to determine what the actual problem was.
                         */
                        fatalError("Unresolved error \(error), \(error.userInfo)")
                    }
                })
                return container
            }()
            
            // MARK: - Core Data Saving support
            
            func saveContext () {
                let context = persistentContainer.viewContext
                if context.hasChanges {
                    do {
                        try context.save()
                    } catch {
                        // Replace this implementation with code to handle the error appropriately.
                        // fatalError() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
                        let nserror = error as NSError
                        fatalError("Unresolved error \(nserror), \(nserror.userInfo)")
                    }
                }
            }
        
        let files = Files.init(persistentContainer: persistentContainer);
        Files.sharedInstance = files;
    }
    
    public func start_game(gamePath: String) {
        scene = PlutoboyScene(size: CGSize(width: GB_PIX_WIDTH, height: GB_PIX_HEIGHT));
        
        self.set_persistent_container();
        
        self.spriteKitScene.presentScene(scene);
        self.spriteKitScene.preferredFramesPerSecond = 60;
    
        let file_path = gamePath;//Bundle.main.path(forResource: "Pokemon_Blue", ofType: "gb")!
        let str = file_path;
        let connection_type: ClientOrServer = NO_CONNECT
        init_emu(str, 0, 0, connection_type)
        
        audioManager = AudioManager();
        
        // Set Callbacks
        let x = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        var callbacks = graphics_callbacks(
            display_screen: { (slf, modifier) in
                callsDisplayFrame(slf: slf, modifier: modifier)
            }
        )
        
        setup_graphics_callbacks(x, &callbacks);
        
        
        // Check Audio status, then start running
        audioManager.activate(do_after:{
            DispatchQueue.global(qos: .background).async {
                run(); // Run the emulator
            }
        });
    }
    
    func displayFrame(pixels : UnsafeMutablePointer<UInt32>) {
        
        let height = GB_PIX_HEIGHT;
        let width = GB_PIX_WIDTH;
        
        let size = width * height * MemoryLayout<UInt32>.size;
        let pixel_data = Data(bytesNoCopy: pixels, count: size, deallocator: Data.Deallocator.none);
        
        let new_texture = SKTexture(data: pixel_data, size:CGSize(width: width, height: height));
        
        self.scene.updateDisplay(texture: new_texture);
    }
    
    override func awake(withContext context: Any?) {
        // Attempt to get rid of the back button (really Apple? *sigh*)
        if !GameInterfaceController.reloaded {
            GameInterfaceController.reloaded = true;
            WKInterfaceController.reloadRootPageControllers(withNames: ["GameInterfaceController"], contexts: nil, orientation: .horizontal, pageIndex: 0);
        }
        else {
            self.start_game(gamePath: GameInterfaceController.gamePath!);
        }
    }
    
    override func willActivate() {
        // This method is called when watch view controller is about to be visible to user
    }
    
    override func didDeactivate() {
        // This method is called when watch view controller is no longer visible
    }
    
}
