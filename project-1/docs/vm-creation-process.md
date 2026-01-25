---
geometry:
  - top=0.6in
  - bottom=1in
  - left=1in
  - right=1in
header-includes:
  - \usepackage{graphicx}
  - \setlength{\parskip}{0pt}
  - \usepackage{amsmath}
  - \usepackage{newunicodechar}
  - \newunicodechar{⋮}{\(\vdots\)}
---

\thispagestyle{empty}
\vspace*{-0.5in}
\begin{center}
\includegraphics[width=150pt]{images/logo.png}
\end{center}
\vspace{-0.5in}

# VM Creation Process

This document will mainly serve as documentation on how I created a singular VM using AWS Lightsail and the process that I would take to scale the creation of additional VMs in Lightsail without the need for excessive downloads. This VM will satisfy the following requirements:

1. The VM will run on Windows Server as its OS
	1. Specifically, Windows Server 2022 version 2026.01.14 will be used
2. A Code editor
	1. Visual Studio Code
3. A Compiler
	1. g++, a C++ compiler
4. An IDE
	1. Visual Studio
5. A Document editor
	1. Libreoffice Word
6. A PDF Viewer
	1. Adobe Acrobat Reader

**Note:** Chocolatey, a package manager for Windows, will be used to install the above softwares in a singular, convenient location. In addition, it will make it so that a GUI will not be needed, which could be an issue if attempting to create VMs in applications that do not have a GUI.

This document will additionally contain an appendix that outlines how I would instruct an IT professional on how to create 1000 VMs using two methods, with added photos to aid with navigation: 

1. Using AWS Lightsail and AWS EC2, which will contain a more descriptive process. AWS EC2 is used to create multiple identical VMs at once, as Lightsail only supports the creation of a single VM at a time.
2. A more general description that includes an overview on how to configure and set up multiple identical VMs with other services.

As method two would be a method that I did not personally use, it will be less descriptive and would serve as an overview of how someone that is more experienced and comfortable with other software/services would go about the creation of multiple VMs. 

**Note:** Be mindful that the following document does not take into account the cost of various services that are used to create VMs. Please excercise caution when creating VMs by using the minimum amount of resources that are required for however many VMs you would like to create.

---

## Process of how the environment was created

This section will document how I personally created a VM within AWS Lightsail. The process will be described in a step-by-step manner.

1. Login to AWS Lightsail as a Root User
2. Create a new instance
	1. Select  "Microsoft Windows as a platform"
		1. \includegraphics[width=250pt]{images/46336.png}
	2. Select "Windows Server 2022" as the default OS
		1. I used v2026.01.14
	3. Add the following in as a launch script to install Chocolatey
		1. I used Chocolatey to circumvent the need to navigate through a browser to install the required software
		2. ```ps1
				<powershell>
					Set-ExecutionPolicy Bypass -Scope Process -Force
					[System.Net.ServicePointManager]::SecurityProtocol =
					[System.Net.ServicePointManager]::SecurityProtocol -bor 3072

					iex ((New-Object System.Net.WebClient).DownloadString(
						'https://community.chocolatey.org/install.ps1'
					))
				</powershell>
		   ```
		3. \includegraphics[width=400pt]{images/12543.png}
	4. Select "Dual Stack" as the network type
		1. Dual Stack was chosen to allow for maximum compatability
	5. The VM plan that I selected was the one with 2 GB of RAM< 2 vCPUs, and 60 GB of storage
	6. The VM was then created
3. I connected to the newly created VM using RDP
	4. \includegraphics[width=150pt]{images/66547.png}
4. Once connected, I went to powershell, and ensured that chocolatey is installed using `choco --version`
5. The following script was then used to install all required software:
	1. ```ps1
		choco install vscode -y --limit-output --no-progress
		choco install mingw -y --limit-output --no-progress
		choco install visualstudio2019community -y --limit-output --no-progress
		choco install libreoffice-fresh -y --limit-output --no-progress
		choco install adobereader -y --limit-output --no-progress --ignore-checksums
	   ```

---

## Appendix

### Creating any number of VMs through AWS Lightsail and AWS EC2

1. Login to AWS Lightsail
2. Create a new instance
	1. Select  "Microsoft Windows as a platform"
		1. \includegraphics[width=250pt]{images/46336.png}
	2. Select "Windows Server 2022" as the default OS
	3. Add the following in as a launch script
		1. ```ps1
				<powershell>
					# Install Chocolatey
					Set-ExecutionPolicy Bypass -Scope Process -Force
					[System.Net.ServicePointManager]::SecurityProtocol =
					[System.Net.ServicePointManager]::SecurityProtocol -bor 3072

					iex ((New-Object System.Net.WebClient).DownloadString(
						'https://community.chocolatey.org/install.ps1'
					))
				</powershell>
		   ```
		2. \includegraphics[width=400pt]{images/12543.png}
	4. Select "Dual Stack" as the network type
	5. Select any VM plan that fits your use case the best
	6. Create the VM
3. Connect the newly created VM using RDP
	4. \includegraphics[width=150pt]{images/66547.png}
4. Once connected, go to powershell, and ensure that chocolatey is installed using `choco --version`
5. Run the following ps1 script to install the apps mentioned above in PowerShell:
	1. ```ps1
		choco install vscode -y --limit-output --no-progress
		choco install mingw -y --limit-output --no-progress
		choco install visualstudio2019community -y --limit-output --no-progress
		choco install libreoffice-fresh -y --limit-output --no-progress
		choco install adobereader -y --limit-output --no-progress --ignore-checksums
	   ```
6. Confirm that all apps are installed
7. Navigate back to Lightsail and create a snapshot by heading into the "snapshots" tab
	1. \includegraphics[width=300pt]{images/89737 1.png}
8. Export the newly created snapshot to EC2 
	1. Click on the three dots (⋮)
	2. Click on "Export to Amazon EC2"
9. Open the exported snapshot in EC2. This is referred to as an AMI in EC2.
	1. This page is reached by going to the tab "Exports" from the tab bar
	2. \includegraphics[width=250pt]{images/42441.png}
10. Once open, confirm that that the status of the snapshot is "Available", right click on the AMI, and select "Launch instance from AMI"
11. In the top right, create more instances as needed.
12. Assign credentials to each VM to allow for remote access by others if needed

### General Guide on Creating Multiple VMs
1. Download an ISO for Microsoft Windows Server
2. Create a VM using said ISO
3. Run the following script to install Chocolatey within PowerShell as an Administrator. Note that the version of PowerShell should be Windows PowerShell 5.1 or newer:
	1. ```ps1
	   # Install Chocolatey
		Set-ExecutionPolicy Bypass -Scope Process -Force
		[System.Net.ServicePointManager]::SecurityProtocol =
		[System.Net.ServicePointManager]::SecurityProtocol -bor 3072

		iex ((New-Object System.Net.WebClient).DownloadString(
			'https://community.chocolatey.org/install.ps1'
		))
	   ```
4. Run the following script on the VM to install all required software:
	1. ```ps1
		choco install vscode -y --limit-output --no-progress
		choco install mingw -y --limit-output --no-progress
		choco install visualstudio2019community -y --limit-output --no-progress
		choco install libreoffice-fresh -y --limit-output --no-progress
		choco install adobereader -y --limit-output --no-progress --ignore-checksums
	   ```
5. Create a snapshot or convert the VM into a template/image of the system to allow for offline cloning of the VMs with all software already downloaded
6. Use said snapshot to create as many VMs as necessary
7. Assign credentials to each VM to allow for remote access by others if needed