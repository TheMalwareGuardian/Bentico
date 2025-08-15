# ***🎤 Cybersecurity Conferences***

Benthic is one of the core rootkit components of the Abyss framework, officially released at ***DEF CON 33***. While DEF CON marked its public debut, the development of Benthic began much earlier, with foundational research in 2024 and key features demonstrated through live presentations across multiple conferences in 2025.

## ***🪸 Rootkit Development Journey***

### **🎙️ RootedCON Madrid 2025**

- ***From Deep Within the Kernel: How to Build an Invisible Rootkit on Windows***

This talk presented the internal architecture of the Benthic rootkit, with a focus on modular stealth and persistence techniques. It covered DKOM-based process hiding, keyboard filtering for keylogging, file and folder hiding via minifilters, network control using WFP, and kernel-mode C2 communication through WSK.

<p align="center">
	<a href="https://www.youtube.com/watch?v=W7jj1e30ADU" target="_blank">
		<img width="800px" src="2025 RootedCON Madrid/Presentation/Images/Illustrations/Front_Page.png" alt="Click to watch the video">
	</a>
	<br>
	<em>Watch the full presentation on my YouTube channel</em>
</p>

### **🎙️ ViCONgal 2025**

- ***From Deep Within the Kernel: How to Build an Invisible Rootkit on Windows***

The session opened with a quick recap of the 2024 ViCONgal talk, explaining how a custom UEFI bootkit can manually stage a kernel-mode rootkit during early boot. From there, the focus shifted to a nearly full live demonstration of Benthic's core modules. Each stealth capability, DKOM, file hiding with a minifilter, WFP-based filtering, and WSK C2, was shown independently before launching the complete rootkit.

<p align="center">
	<img width="800px" src="2025 ViCONgal/Presentation/Images/Illustrations/Front_Page.png" alt="Click to watch the video">
</p>

### **🎙️ INCIBE Emprende 2025**

- ***From Deep Within the Kernel: How to Build an Invisible Rootkit on Windows***

Designed as a recap of RootedCON and ViCONgal 2025, this session walked through the full rootkit infection chain, from boot-level compromise to kernel-mode persistence. It focused on real-world installation techniques, including UEFI bootkits, BYOVD, and certificate forgery, tying together previous demos into a cohesive deployment strategy.

<p align="center">
	<img width="800px" src="2025 INCIBE Emprende/Presentation/Images/Front_Page.png" alt="Click to watch the video">
</p>
