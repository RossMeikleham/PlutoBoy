//
//  Files.swift
//  Plutoboy WatchKit Extension
//
//  Created by Ross Meikleham on 05/03/2021.
//

// WatchOS can't access Persistent Storage "normally",
// so will have to store/loads saves using CoreData

import Foundation
import CoreData
import CloudKit

@objc class Files : NSObject {
    
    private var persistentContainer: NSPersistentContainer;
    
    @objc static var sharedInstance : Files? = nil;

    init(persistentContainer: NSPersistentContainer) {
        self.persistentContainer = persistentContainer;
    }
    
    @objc public func saveGame(fileName: String, saveData: NSData) {
        let viewContext = persistentContainer.viewContext;
        
        let fetchRequest = NSFetchRequest<Saves>(entityName:"Saves");
        fetchRequest.predicate = NSPredicate(format: "name == %@", fileName);
        fetchRequest.fetchLimit = 1;
     
        // Attempt to see if file already exists and update the save data
        do{
            let saves = try viewContext.fetch(fetchRequest);
            
            // Didn't find one, add it
            if saves.count == 0 {
                let saveObj = Saves(context: viewContext);
                saveObj.name = fileName;
                saveObj.data = Data(referencing: saveData);
            }
            else {
                for (_,save) in saves.enumerated() {
                    save.data = Data(referencing: saveData);
                }
            }
            
            if viewContext.hasChanges {
                do {
                    try viewContext.save()
                    print("Saved game for " + fileName)
                } catch let error as NSError {
                    print("Failed to save game for " + fileName + "\(error),\(error.userInfo)")
                }
            }
            
        } catch let error as NSError {
            print("Failed to save game for " + fileName + "\(error),\(error.userInfo)");
        }
    }
    
    @objc public func loadGame(fileName: String) -> Data? {
        let viewContext = persistentContainer.viewContext;
        
        let fetchRequest = NSFetchRequest<Saves>(entityName:"Saves");
        fetchRequest.predicate = NSPredicate(format: "name == %@", fileName);
        fetchRequest.fetchLimit = 1;
            
        // Attempt to see if file already exists and update the save data
        do{
            let saves = try viewContext.fetch(fetchRequest);
            for (_,save) in saves.enumerated() {
                return save.data;
            }
            
        } catch let error as NSError {
            print("Failed to load save for " + fileName + "\(error),\(error.userInfo)");
        }
        
        return nil;
    }
}
