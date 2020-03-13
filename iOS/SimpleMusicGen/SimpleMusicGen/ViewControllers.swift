//
//  ViewController.swift
//  SimpleMusicGen
//
//  Created by Morgan on 10/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var settingsLabel: UILabel!
    @IBOutlet weak var buttonLabel: UIButton!
    
    var playing: Bool = false;
    var songSettings: userSettings = userSettings.init(BPM: 120, note: 67, scale: 1, lofi: false);
    
    func updateSettingsLabel() {
        songSettings = getSongSettings()
        
        let note = String(format: "%c", songSettings.note)
        let configText = """
        <div style="font-family: Verdana; font-size: 16px; text-align: center;">
            <b>BPM:</b> \(songSettings.BPM) <br / >
            <b>Key:</b> \(note) \(songSettings.scale > 0 ? "Major" : "Minor") <br />
            <b>LoFi:</b> \(songSettings.lofi ? "Yup" : "Nope")
        </div>
        """
        let configTextData = Data(configText.utf8)
        if let attributedString = try? NSAttributedString(
            data: configTextData,
            options: [.documentType: NSAttributedString.DocumentType.html],
            documentAttributes: nil
            ) {
                settingsLabel.attributedText = attributedString
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        SetupSDL()
        updateSettingsLabel()
    }
    
    @IBAction func StartMusic(_ sender: Any) {
        if playing {
            StopAudio()
            playing = false
            buttonLabel.setTitle("Start Music!", for: .normal)
        }
        else {
            PlayAudio()
            playing = true
            buttonLabel.setTitle("Stop Music!", for: .normal)

        }
    }
}


class SettingsController: UIViewController, UIPickerViewDelegate, UIPickerViewDataSource {
    
    // Number of columns of data
    func numberOfComponents(in pickerView: UIPickerView) -> Int {
        if pickerView == keyPicker {
            return 2
        }
        else {
            return 1
        }
    }
    
    // The number of rows of data
    func pickerView(_ pickerView: UIPickerView, numberOfRowsInComponent component: Int) -> Int {
        if pickerView == keyPicker {
            if component > 0 {
                return 2
            }
            return 7
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
                return tonality[row]
            }
            return notes[row]
            
        }
        if pickerView == bpmPicker {
            return bpmData[row]
        }
        return ""
    }
    
    
    @IBOutlet weak var bpmPicker: UIPickerView!
    @IBOutlet weak var keyPicker: UIPickerView!
    var bpmData: [String] = [String]()
    var tonality: [String] = ["Minor", "Major"]
    var notes: [String] = ["A", "B", "C", "D", "E", "F", "G"]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        bpmData = (60...240).filter { $0 % 4 == 0}.map { String($0) }
        
        self.bpmPicker.delegate = self
        self.bpmPicker.dataSource = self
        self.keyPicker.delegate = self
        self.keyPicker.dataSource = self
    }
}


extension UIView {
    @IBInspectable
    var cornerRadius: CGFloat {
        get {
            return layer.cornerRadius
        }
        set {
            layer.cornerRadius = newValue
        }
    }
}

