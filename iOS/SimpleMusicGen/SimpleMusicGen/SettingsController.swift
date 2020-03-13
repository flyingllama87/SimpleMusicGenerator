//
//  SettingsController.swift
//  SimpleMusicGen-ios
//
//  Created by Morgan on 14/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

import UIKit

class SettingsController: UIViewController, UIPickerViewDelegate, UIPickerViewDataSource {
    
    @IBOutlet weak var bpmPicker: UIPickerView!
    @IBOutlet weak var keyPicker: UIPickerView!
    @IBOutlet weak var lofiSwitch: UISwitch!

    // Data sources for the pickers
    var bpmData: [String] = [String]()
    var scales: [String] = ["Minor", "Major"]
    var notes: [String] = ["A", "B", "C", "D", "E", "F", "G"]
    
    // Store the selected data from the pickers
    var bpm: Int32;
    var note: Character;
    var scale: Int32;// 0 for Minor, 1 for Major
    var lofi: Bool;
    
    
    required init?(coder aDecoder: NSCoder)  {
        let curSettings: userSettings = getSongSettings()
        self.bpm = curSettings.BPM;
        self.lofi = curSettings.lofi;
        self.scale = curSettings.scale;
        self.note = { () -> Character in
            var char: Character = "C";
            let int: Int8 = curSettings.note
            if let uint = UInt8(exactly: int) {
                char = Character(UnicodeScalar(uint))
            }
            return char
        }()
        super.init(coder: aDecoder)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)

        if self.isMovingFromParent {
            SongConfig(
                Int32(bpm),
                Int8(Array(String(note).utf8)[0]),
                Int32(scale),
                lofiSwitch.isOn
            )
        }
    }
    
    func pickerView(_ pickerView: UIPickerView, didSelectRow row: Int, inComponent component: Int)
    {
        if pickerView == keyPicker {
            if component == 0 {
                note = Character(notes[row])
            } else { // scale column
                scale = Int32(row)
            }
        }
        else { // BPM picker
            bpm = Int32(bpmData[row])!
        }
     }
    
    // Number of columns of data
    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        return pickerView == keyPicker ? 2 : 1
    }
    
    // The number of rows of data
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        if pickerView == keyPicker {
            return component > 0 ? 2 : 7
        }
        else {
            return bpmData.count
        }
    }
    
    // The data to return for the row and component (column) that's being passed in
    func pickerView(_ pickerView: UIPickerView, titleForRow row: Int, forComponent component: Int) -> String? {
        if pickerView == keyPicker {
            if component > 0 {
                if row > 1 {
                    return nil
                }
                return scales[row]
            }
            return notes[row]
            
        }
        if pickerView == bpmPicker {
            return bpmData[row]
        }
        return ""
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        bpmData = ["60", "120", "240"]
        // bpmData = (60...240).filter { $0 % 4 == 0}.map { String($0) }
        
        self.bpmPicker.delegate = self
        self.bpmPicker.dataSource = self
        self.keyPicker.delegate = self
        self.keyPicker.dataSource = self
    }
}
