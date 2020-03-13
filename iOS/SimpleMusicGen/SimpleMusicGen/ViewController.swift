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
    
    func updateSettingsLabel() {
        let songSettings: userSettings = getSongSettings()
        
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
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
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

