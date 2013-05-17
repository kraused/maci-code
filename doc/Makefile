
# Latex command
LATEX  = perltex --latex=pdflatex
# Bibtex command
BIBTEX = bibtex

default: all

.PHONY: show
all: main.tex
	$(LATEX)  main.tex
	$(BIBTEX) main
	$(LATEX)  main.tex
	$(LATEX)  main.tex


clean:
	rm -f main.aux main.log main.pdf

