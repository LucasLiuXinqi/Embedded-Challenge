# Embedded-Challenge
Gesture-Based Unlock System

Project Overview

This project implements a gesture-based authentication system using an IMU (accelerometer) and a microcontroller. The system allows users to record a hand movement sequence and later replicate it to unlock a resource. This provides a secure and intuitive alternative to traditional authentication methods by leveraging motion-based user input.

Features
	•	Gesture Recording & Verification – Users can record a custom motion sequence and later replicate it to unlock a resource.
	•	IMU-Based Motion Tracking – Utilizes an onboard accelerometer and gyroscope to capture and process movement data.
	•	Dynamic Time Warping (DTW) Matching – Compares new input sequences to stored gestures, allowing for slight variations while maintaining accuracy.
	•	State Machine Implementation – A finite state machine (FSM) manages different phases, including recording, storing, and authentication.
	•	Real-Time Feedback System – Provides LED-based indications for authentication success or failure.
	•	Non-Volatile Storage – Saves the recorded gesture sequence for later verification, even after power cycles.

Hardware Requirements
	•	Microcontroller (Adafruit Circuit Playground Classic)
	•	Accelerometer (Onboard or External)
	•	LED Indicator for Feedback
	•	Push Buttons (for Record and Enter Actions)

Software & Tools Used
	•	PlatformIO – Development environment for embedded programming.
	•	C/C++ (Embedded Programming) – Used for microcontroller firmware development.
	•	Hardware Abstraction Layer (HAL) Functions – Utilized for sensor data acquisition and processing.
	•	Dynamic Time Warping (DTW) – Implemented for motion sequence comparison and authentication.
	•	UART Debugging – Used for real-time sensor data visualization and fine-tuning gesture recognition.

System Workflow
	1.	Record Gesture – The user performs a hand movement while holding the IMU, and the motion sequence is stored in memory.
	2.	Save & Store – The recorded sequence is saved in non-volatile memory for future authentication.
	3.	Authentication Attempt – The user repeats the gesture; the system compares the new sequence with the stored reference using DTW.
	4.	Unlock Decision – If the motion matches within an acceptable tolerance, an LED lights up to indicate success; otherwise, it signals failure.

Challenges & Optimizations
	•	Filtering Sensor Noise – Applied low-pass filtering to stabilize IMU readings and improve accuracy.
	•	Real-Time Processing – Optimized the DTW-based comparison to ensure efficient and fast verification.
	•	Gesture Repeatability – Adjusted tolerance thresholds to balance usability and security.
