.PHONY: all
all: build/main.pdf

build/main.pdf: main.tex $(wildcard images/*)
	pdflatex --output-directory=build main.tex
