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

# Documentation for Recreating Container Instances

This document describes the tools, components, and processes that I used to created a containerized environment using Docker, Docker Compose, and AWS EC2. The goal of this project was to build a reusable base image and create multiple container instances using that base image. This documentation is intended to allow the environment ot be recreated at any time on any machine that supports Docker and Docker Compose.

## Tools and Technologies Used

1. Docker
    - Docker was used to package applications and their dependencies into images. AN image serves as a blueprint, while containers are instances of those images. This allows for applications to run consistently across different envirnonments.
2. Docker Compose 
    - Docker Compose was used to define and manage multiple containers using a single config file. This allowed multiple identical and uniue services to be deployed simultaneously from the same base image. 
3. AWS EC2 
    - AWS EC2 was used to create a virtual instance that hosts the containerized environment.

## Process of how the environment was created

This section will document how I personally created a VM within AWS Lightsail. The process will be described in a step-by-step manner.

### Creation of a VM

1. Login to AWS EC2 as a Root User
2. Create a new instance within the dashboard
    - \includegraphics[width=150pt]{images/launch_instance.png}
3. Select "Ubuntu" as the default OS
    - \includegraphics[width=150pt]{images/os_ami.png}
4. Create a Key Pair. This will be used to access the instance through SSH (assuming that SSH is enabled and that the terminal provided by AWS is not used. This will not be covered in this document, but is possible.)
    - \includegraphics[width=150pt]{images/key_pair.png}
5. Launch the instance
    - \includegraphics[width=150pt]{images/launch_instance1.png}
6. Click on the "Security Groups" within the "Network & Security" tab
7. Click on the correct security group, and click the following:
    - Actions -> Edit Inbound Rules
    - \includegraphics[width=150pt]{images/edit_inbound_rules.png}
8. Add the following rules:
    - \includegraphics[width=150pt]{images/inbound_rules.png}
9. Find the instance that you just created under "Instances" and click on connect
10. Click on "EC2 Instance Connect", and click on "Connect". This should open a new tab, in which the next steps can be followed.
    - \includegraphics[width=150pt]{images/instance_connect.png}

### Installation of Docker and Docker Compose
1. Update packages
    
    ```bash 
    sudo apt-get update
    ```

2. Install pre-requisites
    
    ```bash
    sudo apt-get install -y ca-certificates curl
    ```

3. Create the keyrings directory
    
    ```bash
    sudo install -m 0755 -d /etc/apt/keyrings
    ```

4. Add the Docker GPG key
    
    ```bash
    sudo curl -fsSL https://download.docker.com/linux/ubuntu/gpg -o /etc/apt/keyrings/docker.asc
    ```

5. Make sure that hte key is readable by APT

    ```bash
    sudo chmod a+r /etc/apt/keyrings/docker.asc
    ```

6. Add Docker's repository to APT sources

    ```bash
    echo \
    "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.asc] https://download.docker.com/linux/ubuntu \
    $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
    sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
    ```

7. Update packages

    ```bash
    sudo apt-get update
    ```

8. Install Docker Engine + Compose

    ```bash
    sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
    ```

9. Create Docker Group

    ```bash
    sudo groupadd docker
    ```

10. Add current user to Docker Group

    ```bash
    sudo gpasswd -a $USER docker
    ```

11. Apply group changes ot the current session

    ```bash
    newgrp docker
    ```

### Recreating the environment

1. Clone the repository or download the zip file, which was submitted as well

    ```bash
    https://github.com/zhangdaniel62/csce-412-project-2.git
    ```

2. Navigate to the project directory
3. Run the following command to build the base image

    ```bash
    docker compose up --build -d
    ```

4. The container instances will be created with the following format:
    - 5 unique copies from the same base container
        - http://<ip_address>:8080-4
    - 5 identical copies of the same base container
        - http://<ip_address>:8085-9

## Extra Credit

I did the extra credit for creating 5 identical containers by porting over Visual Studio Code through the following [image](https://hub.docker.com/r/linuxserver/code-server). This container allows for you to develop and debug code in a containerized environment, which can also be used for remote development. It should be included in the submission under the file "docker-compose-bonus.yml".

- Command to build the containers

```bash 
docker compose -f docker-compose-bonus.yml up -d --build
```

- Command to remove the containers along with the volumes


```bash
docker compose -f docker-compose-bonus.yml down -v
```