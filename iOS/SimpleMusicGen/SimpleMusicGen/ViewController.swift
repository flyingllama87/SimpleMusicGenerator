//
//  ViewController.swift
//  SimpleMusicGen
//
//  Created by Morgan on 10/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    var playing: Bool = false;
    
    override func viewDidLoad() {
        super.viewDidLoad()
        SetupSDL()
        // Do any additional setup after loading the view.
    }

    @IBOutlet weak var buttonLabel: UIButton!
    
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
