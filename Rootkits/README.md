# 🐙 Rootkits

<p align="center">
<img src="../Images/Logos/Rootkits.png">
</p>

---

This folder contains a variety of Windows kernel-mode drivers developed to demonstrate rootkit basic functionalities:


* _**KernelRootkit001_HelloWorld**_
	* _**KMDFDriver_HelloWorld**_ is a Windows kernel-mode driver that serves as a demonstration of basic rootkit functionality. The driver's primary purpose is to load into the kernel and print a "Hello World" debug message as part of its initialization process. It also includes a DriverUnload routine to handle the unloading and cleanup of the driver.

* _**KernelRootkit002_Threading**_
	* _**KMDFDriver_Threading**_ is a Windows kernel-mode driver that serves as a demonstration of basic rootkit functionality. The driver's primary purpose is to create and manage a persistent system thread that periodically logs messages to the kernel debugger. The driver initializes by spawning a system thread that executes at regular intervals, printing a timestamped "Hello World" message. It also includes a DriverUnload routine to properly terminate the thread and clean up resources before unloading the driver from the system.

* _**KernelRootkit003_ZwFunctions**_
	* _**KMDFDriver_ZwFunctions.c**_ is a Windows kernel-mode driver that serves as a demonstration of basic rootkit functionality. The driver's primary purpose is to interact with the Windows kernel using Zw (NTAPI) functions to perform file operations and retrieve system information. Specifically, the driver creates a file, logs OS version details into it, and appends data using ZwCreateFile and ZwWriteFile. It also includes a DriverUnload routine to clean up and delete the file upon removal.

* _**KernelRootkit004_IOCTLs**_
	* _**ConsoleApp_IOCTLs**_ is a user-mode application that interacts with a kernel-mode driver by sending IOCTL (Input/Output Control) requests and processing its responses. It serves as the interface for executing rootkit functionalities from user space.
	* _**KMDFDriver_IOCTLs**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for handling IOCTL requests initiated by a user-level application. It defines IOCTL command codes and implements corresponding functionality for each one.

* _**KernelRootkit005_Callbacks**_
	* _**KMDFDriver_Callbacks**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver registers kernel event callbacks to monitor system activity, specifically tracking process creation and termination in real time. By leveraging PsSetCreateProcessNotifyRoutine and PsSetCreateProcessNotifyRoutineEx, it captures parent-child process relationships while enforcing restrictions on specific executables, actively blocking the execution of certain processes to prevent the launch of debugging and monitoring tools.

* _**KernelRootkit006_DKOM**_
	* _**ConsoleApp_DKOM**_ is a user-mode application that interacts with a kernel-mode driver to manipulate process structures in memory.
	* _**KMDFDriver_DKOM**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. It manipulates kernel structures using Direct Kernel Object Manipulation (DKOM) to hide processes by unlinking them from the ActiveProcessLinks list, bypassing standard process enumeration methods.

* _**KernelRootkit007_KeyboardFilter**_
	* _**KMDFDriver_KeyboardFilter**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver operates as a keyboard filter by attaching itself to the keyboard device stack and intercepting keystroke events at the kernel level. It includes a completion routine that inspects key events and logs them, effectively demonstrating how a keylogger can be implemented at the kernel level.

* _**KernelRootkit008_Minifilter**_
	* _**ConsoleApp_MinifilterInstallation**_ is a user-mode application responsible for creating the necessary registry keys required for the minifilter driver to load. Unlike standard kernel drivers that can be installed using sc.exe create, minifilter drivers require additional registry modifications to be properly registered and loaded by the Windows Filter Manager.
	* _**KMDFDriver_Minifilter**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver intercepts file system operations at the minifilter level to block access to specific files and directories.


* _**KernelRootkit009_FilterCommunicationPort**_
	* _**ConsoleApp_FilterCommunicationPort**_ is a user-mode application that communicates with a kernel-mode minifilter driver via Filter Communication Ports. This application serves as an interface to send commands and receive responses from the minifilter driver, enabling interaction with the rootkit's functionalities from user space.
	* _**KMDFDriver_FilterCommunicationPort**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver establishes a Filter Communication Port to facilitate controlled interactions between user-mode applications and the kernel. By intercepting file system operations, it enables the modification, blocking, or monitoring of file access dynamically.

* _**KernelRootkit010_WindowsFilteringPlatform**_
	* _**KMDFDriver_WindowsFilteringPlatform**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver intercepts and blocks network connections directed to specific IP addresses by integrating with the Windows Filtering Platform (WFP).

* _**KernelRootkit011_WinSockKernel**_
	* _**KMDFDriver_WinSockKernel**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver establishes network connections and performs data transmission directly from the kernel. By leveraging the WinSock Kernel (WSK) API, it can send and receive data over TCP and UDP without requiring interaction with user-space applications.


---
---
---


### 👨‍💻 Note

This repository has been created for educational purposes, specifically to support the students of the Master's in Reverse Engineering, Malware Analysis, and Bug Hunting. The projects here are designed as examples to help students with their final projects and hands-on practice during the Windows Reversing module of the master's program.

The examples showcase key functionalities that can be implemented in rootkits. Each project has been developed following different programming approaches, offering a variety of perspectives for solving similar challenges. This implementation serves as a foundation, and certain aspects may benefit from further refinement or adaptation based on specific requirements. The aim is to encourage critical thinking and provide you with the opportunity to refine and adapt these examples to your own style and objectives.

**These projects are meant to be a starting point / a solid foundation for your academic and professional development. By exploring the code, analyzing its structure, and even improving it, you'll be actively engaging in the learning process. Remember, the real value lies in how you evolve these examples to fit your ideas and needs.**

<!--
A - Accessible
B - Basic
C - Clear
D - Doable
E - Elementary
F - Feasible
G - Gradual
H - Handleable
I - Intermediate (Intermedio, algo desafiante)
J - Justifiable
-->
