//
//  ViewController.swift
//  SimpleMusicGen
//
//  Created by Morgan on 10/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        SetupSDL()
        // Do any additional setup after loading the view.
    }


    @IBAction func StartMusic(_ sender: Any) {
        PlayAudio()
        
    }
}

