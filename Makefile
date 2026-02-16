# Usage:
# make pdf  PROJECT=project-1 FILE=writeup
# make docx PROJECT=project-1 FILE=writeup

PANDOC_PDF_ENGINE=--pdf-engine=xelatex

ifndef PROJECT
$(error Project is not defined. Usage: make pdf PROJECT=project-1 FILE=writeup)
endif

ifndef FILE
$(error File does not exist. Usage: make pdf PROJECT=project-1 FILE=writeup)
endif


MD_DIR := $(PROJECT)/docs
OUTPUT_DIR := $(PROJECT)/build
                                                                                                                                          
pdf:
	mkdir -p $(OUTPUT_DIR)
	pandoc $(MD_DIR)/$(FILE).md -o $(OUTPUT_DIR)/$(FILE).pdf $(PANDOC_PDF_ENGINE)

docx:
	mkdir -p $(OUTPUT_DIR)
	pandoc $(MD_DIR)/$(FILE).md -o $(OUTPUT_DIR)/$(FILE).docx 

html+css: 
	mkdir -p $(OUTPUT_DIR)
	pandoc $(MD_DIR)/$(FILE).md -o $(OUTPUT_DIR)/$(FILE).html --standalone