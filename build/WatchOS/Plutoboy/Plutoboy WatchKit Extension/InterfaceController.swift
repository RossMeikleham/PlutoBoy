//
//  InterfaceController.swift
//  Plutoboy WatchKit Extension
//
//  Created by Ross Meikleham on 10/10/2020.
//

import WatchKit
import Foundation
import Intents

class InterfaceController: WKInterfaceController {
    
    @IBOutlet var myPicker: WKInterfacePicker!
    @IBOutlet var itemLabel: WKInterfaceLabel!
    @IBOutlet weak var gameSelectTap: WKTapGestureRecognizer!;
    
    var pickerPaths: [String]!;
    var currentPathIdx = 0;
    
    @IBAction func gameSelectHandler(sender: WKLongPressGestureRecognizer) {
        print("Selected Game: " + self.pickerPaths[currentPathIdx]);
        //presentController(withName:"AgC-eL-Hgc", context: nil)
        GameInterfaceController.gamePath = self.pickerPaths[currentPathIdx];
        
        // Switch to the Game Interface
        
        // Lets use strings with no warning or checks from the IDE or Compiler that
        // they match an actual existing Interface.storyboard interface identifier.
        // Please tell me theres a better way to do this :/
        self.pushController(withName:"GameInterfaceController", context:nil);
    }

    @IBAction func pickerChanged(value: Int) {
        self.currentPathIdx = value;
        print(pickerPaths[value]);
        //itemLabel.setText(str);
    }

    func setup_picker() {
        
        //self.myPicker.setSty
        self.pickerPaths = [String]();
        /*//item.contentImage = WKImage(imageName: "sample")
        let ui_img = UIImage(named:"Pokemon_Crystal.png");
        print(ui_img);
        let img = WKImage(image:ui_img!);
        print(img);
        if img.image == nil {
            print("huh");
        }*/
        
        let resourcePath = Bundle.main.resourcePath!;
        var pickerItems = [WKPickerItem]();
        
        do {
            let fileManager = FileManager.default
            let bundleURL = Bundle.main.bundleURL
            let assetURL = bundleURL;//.appendingPathComponent("Images.bundle")

            do {
              let contents = try fileManager.contentsOfDirectory(at: assetURL, includingPropertiesForKeys: [URLResourceKey.nameKey, URLResourceKey.isDirectoryKey], options: .skipsHiddenFiles)

              for item in contents
              {
                  print(item.lastPathComponent)
              }
            }
            catch let error as NSError {
              print(error)
            }
            
            let resources = try fileManager.contentsOfDirectory(atPath: resourcePath)
            for path in resources {
                let str_path = path as NSString;
                let fileName = str_path.lastPathComponent as NSString;
                if fileName.pathExtension == "gb" || fileName.pathExtension == "gbc" {
                    print("Found ROM " + (fileName as String));
                    pickerPaths.append(resourcePath + "/" + path);
                    let pickerItem =  WKPickerItem();
                    let name = fileName.deletingPathExtension as String;
                    pickerItem.title = fileName as String;
                    let ui_img = UIImage(named:name + ".png");
                    
                    if ui_img != nil {
                        pickerItem.contentImage = WKImage(image:ui_img!);
                    } else {
                        print("couldnt find " + name);
                    }
                    print(pickerItem.contentImage?.imageData);
                    //}
                    pickerItem.caption = name;
                    pickerItems.append(pickerItem);
                }
            }
        } catch {
            print(error)
        }
        
        myPicker.setItems(pickerItems);
    }
    
    override func awake(withContext context: Any?) {
        setup_picker();
    }
    
    override func willActivate() {
        // This method is called when watch view controller is about to be visible to user
    }
    
    override func didDeactivate() {
        // This method is called when watch view controller is no longer visible
    }

}
