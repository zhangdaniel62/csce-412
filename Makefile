# Usage:
# make pdf  PROJECT=project-1 FILE=writeup
# make docx PROJECT=project-1 FILE=writeup

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
	pandoc $(MD_DIR)/$(FILE).md -o $(OUTPUT_DIR)/$(FILE).pdf --pdf-engine=wkhtmltopdf

docx:
	mkdir -p $(OUTPUT_DIR)
	pandoc $(MD_DIR)/$(FILE).md -o $(OUTPUT_DIR)/$(FILE).docx 
