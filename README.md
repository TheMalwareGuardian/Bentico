
<p align="center">
  <img src="Images/Bentico_Squid_Construction.png">
</p>


## Overview

Bentico is a comprehensive project thoroughly designed with the explicit goal of establishing a robust foundation for the development of rootkits. By offering a centralized repository of knowledge, Bentico stands as a valuable initiative for anyone looking to contribute to and benefit from the collective understanding of this field. However, it is imperative to underscore that Bentico is not a tool intended for malicious purposes; rather, it is a carefully constructed initiative for educational exploration and practical insights.

Bentico's significance extends beyond its basic functionality within Windows environments; it serves as a gateway for individuals venturing into the intricate and advanced field of rootkit development.


## What is a Rootkit?

"A Rootkit is a collection of software designed to give malicious actors control of a computer network or application. Once activated, the malicious program sets up a backdoor exploit and may deliver additional malware, such as ransomware, bots, keyloggers or trojans. Rootkits may remain in place for years because they are hard to detect, due in part to their ability to block some antivirus software and malware scanner software. Known rootkits can be classified into a few broad families, although there are many hybrids as well. One of the most well-known types is the kernel mode rootkit (complicated to create), a sophisticated piece of malware that can add new code to the operating system or delete and edit operating system code." ~ [CrowdStrike](https://www.crowdstrike.com/cybersecurity-101/malware/rootkits/)

Essentially, a rootkit is a form of malicious software strategically designed to target a computer's operating system while hiding its presence and activities from users and security solutions.

In the case of Bentico, this manifests as a specialized focus on the complexities inherent in developing kernel mode Windows drivers.


## Rootkits

The 'Rootkits' folder contains a variety of Windows kernel-mode drivers developed to demonstrate rootkit basic functionalities:

* _**KMDF1DriverHelloWorld**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. The driver's primary purpose is to load into the kernel and print a "Hello World" debug message as part of its initialization process. It also includes a DriverUnload function to handle the unloading and cleanup of the driver.
* _**KMDF2DriverIOCTLs**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for handling IOCTL requests initiated by a user-level application. It defines IOCTL command codes and implements corresponding functionality for each one.
    * _**ApplicationIOCTLs**_ is an application file that serves as the user-level component of a basic rootkit. This application is designed to interact with a kernel-mode driver by sending various IOCTL (Input/Output Control) requests to the driver and receiving responses.
* _**KMDF3DriverSubscribe**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is responsible for subscribing to various kernel event notifications in the Windows operating system. It sets up event handlers to receive notifications related to process creation, image loading, and thread creation.
* _**KMDF4DriverDKOM**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver is specifically designed for demonstrating Direct Kernel Object Manipulation (DKOM), a method used to manipulate kernel objects directly. It allows users to experiment with hiding processes by either their names or ids, and provides options to prevent Blue Screen of Death (BSOD) during testing scenarios.
* _**KMDF5DriverKeylogger**_ is a Windows kernel-mode driver that serves as a demonstration of a basic rootkit functionality. This driver attaches itself to the keyboard device and then intercepts input events, logs the keystrokes, and optionally performs filtering. It is intended for monitoring purposes, such as security auditing, user activity tracking or the creation of a honeypot environment to attract and analyze malicious activity. However, it is crucial to ensure that its usage complies with legal and ethical guidelines, as unauthorized monitoring of keyboard input may violate privacy rights and regulations.


## Contact

This project represents a dedicated exploration into the intricate realm of rootkits, with a specific focus on providing a comprehensive resource for both my students and individuals embarking on their journey in this complex field.

The subject of rootkits is multifaceted, and this project serves as a starting point for understanding its nuances. It's important to acknowledge that certain aspects may assume a level of prior knowledge, while others may remain uncharted due to the inherent complexities of the subject matter.

This work is designed to offer valuable insights and resources to support your educational and developmental goals, making it suitable for anyone seeking to delve into rootkit development. If you have specific inquiries, require additional clarification, or wish to engage in collaborative efforts, please do not hesitate to get in [touch](https://www.linkedin.com/in/vazquez-vazquez-alejandro/).


## License

This project is licensed under the GNU GLPv3 License - see the [LICENSE](LICENSE) file for details.
