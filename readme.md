# HRI hand firmware

## Overview
This is a firmware developed for HRI hand in TrueSTUDIO for stm32.
This repository have been tested under Windows10 1903, 64bit. And the schematic files are edited by Altium designer.  
The hardware CAD file are upload in the [An Open-source Anthropomorphic Robot Hand System: HRI Hand](https://osf.io/sfpb2/)

Our robot hand system is developed with a focus on the end-effector role of the collaborative robot manipulator.  
Since the proposed robot hand imitated the human hand, the four fingers, excluding the thumb, consist of distal interphalangeal(DIP), proximal interphalangeal(PIP), metacarpophalangeal(MCP) joints. And it is composed of under-actuated mechanism and can realize 3 degrees of freedom movement with one motor.  
The thumb part consists of interphalangeal(IP), metacarpophalangeal(MCP), Carpometacarpal(CMC) joint, and operates MCP and CMC joint with two motors. The motor is controlled based on the control signal received by the Micro-Controller unit (MCU) via Bluetooth communication.

**Author: [Hyeonjun Park](https://www.linkedin.com/in/hyeonjun-park-41bb59125), koreaphj91@gmail.com**

**Affiliation: [Human-Robot Interaction LAB](https://khu-hri.weebly.com), Kyung Hee Unviersity, South Korea**

## Installation
- Installed the [TrueSTUDIO for stm32](https://atollic.com/truestudio/)
- Installed the [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

## Demo

- Initialize
    
    ![hri_hand_small_200px](https://user-images.githubusercontent.com/4105524/64669616-35424400-d49d-11e9-9936-9a8dbbb171cd.gif)


- The hold-on test, click image to the YouTube video.
    
    [<img width="300" src="https://user-images.githubusercontent.com/4105524/64670413-f235a000-d49f-11e9-8ccc-b73484fcb043.PNG"  alt="hri_hand_hold-on" title="hri_hand_hold-on">](https://youtu.be/vkenz0KlCYk)

   <!-- [![hri_hand_hold-on](https://user-images.githubusercontent.com/4105524/64670413-f235a000-d49f-11e9-8ccc-b73484fcb043.PNG)](https://youtu.be/vkenz0KlCYk)
   -->

- The pick & place test with UR3 manipulator, click image to the YouTube video.
   
    [<img width="500" src="https://user-images.githubusercontent.com/4105524/64670616-c1a23600-d4a0-11e9-8861-e7aa47693b21.jpg"  alt="ur_manipulator_test" title="ur_manipulator_test">](https://youtu.be/8BtP_0Ygy6g)
    
   <!-- [![ur_manipulator_test](https://user-images.githubusercontent.com/4105524/64670616-c1a23600-d4a0-11e9-8861-e7aa47693b21.jpg)](https://youtu.be/8BtP_0Ygy6g)
    -->

