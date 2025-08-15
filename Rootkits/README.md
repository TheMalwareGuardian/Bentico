# 🐙 ***Rootkits***

<p align="center">
<img src="../Images/Logos/Rootkits.png">
</p>


---
---
---


## ***📑 Table of Contents***

<ul>
	<li><a href="#firsttime">First Time?</a></li>
	<li><a href="#windowssecuritymeasures">Security Measures</a></li>
	<li><a href="#windowskernel">WindowsKernel</a></li>
<details>
	<summary>📂</summary>
	<ul>
		<li><a href="#windowskernel-keyresponsabilitiesofthewindowskernel">Key Responsibilities of the Windows Kernel</a></li>
		<li><a href="#windowskernel-kernelmodevsusermode">Kernel Mode vs User Mode</a></li>
		<li><a href="#windowskernel-whydevelopkernelmodedrivers">Why Develop Kernel Mode Drivers?</a></li>
	</ul>
</details>
	<li><a href="#buildingawindowskernelmodedriver">Building a Windows Kernel Mode Driver</a></li>
<details>
	<summary>📂</summary>
	<ul>
		<li><a href="#buildingawindowskernelmodedriver-requirements">Requirements</a></li>
		<li><a href="#buildingawindowskernelmodedriver-script">Script</a></li>
		<li><a href="#buildingawindowskernelmodedriver-projectsetup">Project Setup</a></li>
	</ul>
</details>
	<li><a href="#pocs">Proof of Concepts (Malicious Kernel Mode Drivers)</a></li>
</ul>


---
---
---


<div id='firsttime'/>

## ***🆕 First Time?***

<p align="center">
	<img src="../Images/Illustrations/First_Time.png">
</p>

Building your first rootkit isn't just about writing stealthy code, it's about reshaping how the system sees itself. This isn't malware in userspace. It's kernel-level manipulation, where processes vanish, files become ghosts, and network traffic bends to your control. You're not playing by the OS rules anymore. You're rewriting them. But before you can hide in plain sight, you need to understand the very internals you're about to subvert.


---
---
---


<div id='windowssecuritymeasures'/>

## ***🛡️ Security Measures***

* **Secure Boot (Anti-Bootkit Installation)**: [Secure Boot](https://learn.microsoft.com/en-us/windows-hardware/design/device-experiences/oem-secure-boot) is a security feature that ensures a device boots using software trusted by the manufacturer. It verifies the digital signature of the boot loaders to prevent unauthorized applications from running during the boot process. Secure Boot uses a set of keys (PK, KEK, db, dbx) to manage this verification process, allowing only signed software to execute.
* **Early Launch AntiMalware - ELAM (Anti-Rootkit Installation)**: The [Early Launch AntiMalware](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/early-launch-antimalware) feature allows anti-malware software to start before all other drivers. This early start ensures that the anti-malware software can check and verify the integrity of startup drivers and services, helping to prevent malware from executing at the earliest possible point in the boot process.
* **Driver Signature Enforcement - DSE (Anti-Rootkit Installation)**: [Driver Signature Enforcement](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/driver-signing) is a security feature that ensures only drivers signed by a trusted authority can be loaded into the Windows operating system. This helps prevent the installation of drivers that could be malicious or unstable, thereby maintaining system integrity and stability.
* **Driver Blocklist for Vulnerable Drivers - (Anti-Rootkit Installation via BYOVD)** - [Driver Blocklist](https://learn.microsoft.com/en-us/windows/security/application-security/application-control/app-control-for-business/design/microsoft-recommended-driver-block-rules) prevents the loading of known vulnerable drivers exploited in [BYOVD](https://blogs.vmware.com/security/2023/04/bring-your-own-backdoor-how-vulnerable-drivers-let-hackers-in.html) (Bring Your Own Vulnerable Driver) attacks. This feature ensures system integrity by blocking outdated or compromised drivers, even if signed, using technologies like HVCI and WDAC, providing an additional layer of kernel-level security.
* **Kernel Patch Protection - Patchguard (Anti-Rootkit Deep Funcionalities)**: Kernel Patch Protection, also known as Patchguard, is a feature in 64-bit versions of Windows that prevents unauthorized modification of the Windows kernel. This security measure helps protect the kernel from rootkits and other forms of malware that attempt to insert malicious code into the kernel space.
* **Virtualization Based Security - VBS (Global)** - [Virtualization Based Security](https://learn.microsoft.com/en-us/windows-hardware/design/device-experiences/oem-vbs) uses hardware virtualization features to create and isolate a secure region of memory from the normal operating system. VBS can help protect Windows from vulnerabilities in the operating system and from malicious software that attempts to tamper with the kernel and system processes. This secure environment is used to host several security solutions, such as [Credential Guard](https://learn.microsoft.com/en-us/windows/security/identity-protection/credential-guard/), providing enhanced protection against advanced security threats.


---
---
---


<div id='windowskernel'/>

## ***🪟 Windows Kernel***

The [Windows Kernel](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/_kernel/) is the core of the Windows operating system, responsible for managing hardware, memory, processes, and security at the lowest level. It operates in Ring 0 (privileged mode), allowing direct interaction with system resources.


<div id='windowskernel-keyresponsabilitiesofthewindowskernel'/>

### ***🔍 Key Responsibilities of the Windows Kernel***
- Process and Thread Management: Handles process scheduling, thread execution, and synchronization mechanisms.
- Memory Management: Manages virtual memory, paging, and address translation.
- Device and Driver Communication: Provides APIs for Kernel Mode Drivers (KMDF/UMDF) to interact with hardware.
- Security and Access Control: Implements User Mode vs Kernel Mode separation to enforce system integrity.
- I/O Management: Handles file systems, networking, and system services.


<div id='windowskernel-kernelmodevsusermode'/>

### ***⚡ Kernel Mode vs User Mode***
| Feature              | User Mode                 | Kernel Mode                     |
|----------------------|---------------------------|---------------------------------|
|    Access Level      | Restricted (Ring 3)       | Full System Access (Ring 0)     |
|    Performance       | Lower (API abstraction)   | Higher (Direct hardware access) |
|    Security          | Isolated from the OS core | Runs with full privileges       |
|    Crash Impact      | Affects only the process  | Can crash the entire system     |


<div id='windowskernel-whydevelopkernelmodedrivers'/>

### ***🛠️ Why Develop Kernel Mode Drivers?***

Kernel Mode development is essential for:

- Developing custom drivers for new hardware or virtualization.
- Implementing security tools such as anti-cheat engines, endpoint security solutions, and forensic tools.
- Manipulating system behavior for research or rootkit development.

⚠️ *Kernel development requires careful implementation to avoid system crashes, privilege escalation vulnerabilities, and performance issues.*


---
---
---


<div id='buildingawindowskernelmodedriver'/>

## ***🧑‍💻 Building a Windows Kernel Mode Driver***


<div id='buildingawindowskernelmodedriver-requirements'/>

### ***🛠️ Requirements***

To set up the necessary environment for Kernel Mode drivers development using WDK on [Windows](https://github.com/tianocore/tianocore.github.io/wiki/Windows-systems), the following virtual machines (VMs), tools, and packages are essential:


* **Machines**: Download a [Windows 10](https://www.microsoft.com/en-us/software-download/windows10) or [Windows 11](https://www.microsoft.com/en-us/software-download/windows11) ISO image and set up a virtual machine using [VirtualBox](https://www.virtualbox.org/) or [VMware Workstation](https://www.vmware.com/products/desktop-hypervisor/workstation-and-fusion), configuring it with 70 GB of storage and 8 gigabytes of RAM.
* **Visual Studio 2022 Community**: Download [Visual Studio 2022 Community](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022) and, before installing, make sure to select the "[Desktop development with C++](https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-160)" workload and the 'MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs (latest)' individual component.
* **Git**: Install [Git](https://git-scm.com/) by downloading the appropriate [binary](https://git-scm.com/download/win).
* **Windows Software Development Kit (SDK)**: Install [SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/).
* **Windows Driver Kit (WDK)**: Install [WDK](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) and select 'Windows Driver Kit Visual Studio extension' to complete the integration with Visual Studio.


<div id='buildingawindowskernelmodedriver-script'/>

### ***📜 Script***

To simplify the process of establishing a Rootkit development environment on Windows, a corresponding PowerShell script [Setup_Development_Environment.ps1](https://github.com/TheMalwareGuardian/Bootkits-Rootkits-Development-Environment/blob/main/Scripts/Setup_Development_Environment_Windows.ps1) has been created. This script is designed to walk you through the necessary downloads and installations.


<div id='buildingawindowskernelmodedriver-projectsetup'/>

### ***🏗️ Project Setup***

To start developing a Kernel Mode Driver, follow these steps to create your project and workspace:

1. Open Visual Studio and click on 'Create a new project'.
2. Search for and select the 'Kernel Mode Driver, Empty (KMDF)' template.
3. Name your project (e.g., MyKernelDriver) and specify the location for your workspace.
4. Once the project is created, configure your build environment by setting the appropriate dependencies and include paths for the Windows Driver Kit (WDK). Ensure that your target platform and configurations (e.g., Debug/Release, x64/x86) are correctly selected.


---
---
---


<div id='pocs'/>

## ***💣 Proofs of Concepts (Malicious Kernel Mode Drivers)***

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


## ***👨‍💻 Note***

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
