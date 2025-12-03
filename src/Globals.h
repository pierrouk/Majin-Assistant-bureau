#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// Includes des briques
#include "pins.h"
#include "hal/LightSensor.h"
#include "hal/ServoMotor.h"
#include "hal/TouchSensor.h"
#include "hal/SoundSystem.h"
#include "hal/DisplayDriver.h"
#include "hal/EnvironmentSensor.h" 

#include "managers/NetworkManager.h"
#include "managers/CoreManager.h"
#include "managers/UIManager.h"
#include "managers/InputManager.h"
#include "managers/UsbManager.h"   
#include "managers/SettingsManager.h" 
#include "managers/BootSequence.h"    

// --- Commandes Système ---
enum RobotCommand {
    CMD_NONE, CMD_HELLO, CMD_DANCE, CMD_MOOD_HAPPY, CMD_MOOD_ANGRY, 
     CMD_MOOD_TIRED, CMD_MOOD_SAD,
    CMD_WIFI_CONNECTED, CMD_WIFI_FAIL,
    CMD_HEAD_TAP,
    CMD_HEAD_LONG_PRESS
};

// --- Déclarations Externes (Les objets existent dans main.cpp) ---
extern LightSensor       majinEyes;
extern ServoMotor        majinHead;
extern TouchSensor       majinTouch;
extern SoundSystem       majinVoice;
extern DisplayDriver     majinScreen;
extern EnvironmentSensor majinEnv; 

extern NetworkManager    majinNet;
extern CoreManager       majinCore;
extern UIManager         majinUI;
extern InputManager      majinInput; 
extern UsbManager        majinUSB; 
extern SettingsManager   majinSettings; 
extern BootSequence      majinBoot;     

extern QueueHandle_t     guiQueue;

#endif