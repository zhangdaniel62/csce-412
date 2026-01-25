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

# VM User Guide

This document will serve as a potential use case of the VM that I created, given that the user would be a new SWE employed at a company. To allow for easy access to develop software using C++ as the primary object-oriented programming (OOP) language, Visual Studio Code, Visual Studio, and g++ were installed to allow for immediate development. If needed, LibreOffice Word and Adobe Acrobat Reader were installed to allow for the user to view and edit documentation and general documents sent by coworkers.

---

## Getting Started

1. Log into the VM using the credentials provided by IT.
2. Once logged in, you will be presented with the Windows desktop.
3. All required development and documentation tools are preinstalled and ready for use.

---

## How to use the VM for development

After logging into the VM, the computer can be used similar to a regular computer running Windows. In order to open apps, one can use the search bar to search for any of the following apps:

- Visual Studio Code
- Visual Studio
- LibreOffice Word
- Adobe Acrobat Reader

In addition, g++, a compiler, was installed to allow for developers to compile code that was written in C++.

---

## Compiling C++ Code

This VM is preconfigured for C++ development using g++. To compile a program:

1. Open Visual Studio Code.
2. Create or open a `.cpp` file.
3. Open the integrated terminal.
4. Compile using the following: `g++ main.cpp -o main `
5. Run the executable using `./main`

---

## Ending a Session

When finished, users should log out of the VM rather than shutting it down. This ensures the environment remains available for future use and prevents data loss.