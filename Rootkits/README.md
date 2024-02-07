# Rootkits

<p align="center">
  <img src="../images/Rootkits.png">
</p>

---

This folder contains a variety of Windows kernel-mode drivers developed to demonstrate rootkit basic functionalities:

* _**KMDF1DriverHelloWorld**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. The driver's primary purpose is to load into the kernel and print a "Hello World" debug message as part of its initialization process. It also includes a DriverUnload function to handle the unloading and cleanup of the driver.
* _**KMDF2DriverIOCTLs**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for handling IOCTL requests initiated by a user-level application. It defines IOCTL command codes and implements corresponding functionality for each one.
    * _**ApplicationIOCTLs**_ is an application file that serves as the user-level component of a basic rootkit. This application is designed to interact with a kernel-mode driver by sending various IOCTL (Input/Output Control) requests to the driver and receiving responses.
* _**KMDF3DriverSubscribe**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for subscribing to various kernel event notifications in the Windows operating system. It sets up event handlers to receive notifications related to process creation, image loading, and thread creation.
* _**KMDF4DriverDKOM**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is specifically designed for demonstrating Direct Kernel Object Manipulation (DKOM), a method used to manipulate kernel objects directly. It allows users to experiment with hiding processes by either their names or ids, and provides options to prevent Blue Screen of Death (BSOD) during testing scenarios.
* _**KMDF5DriverKeylogger**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver attaches itself to the keyboard device and then intercepts input events, logs the keystrokes, and optionally performs filtering. It is intended for monitoring purposes, such as security auditing, user activity tracking or the creation of a honeypot environment to attract and analyze malicious activity. However, it is crucial to ensure that its usage complies with legal and ethical guidelines, as unauthorized monitoring of keyboard input may violate privacy rights and regulations.
