TEX_FILE = ETCBC4doc.tex

PDF_FILE = $(TEX_FILE:.tex=.pdf)

CLEANFILES = $(TEX_FILE:.tex=.aux) \
             $(TEX_FILE:.tex=.log) \
             $(TEX_FILE:.tex=.out) \
             missfont.log \
             *~

all: $(PDF_FILE)

$(PDF_FILE): $(TEX_FILE) Makefile
	xelatex $(TEX_FILE)
	xelatex $(TEX_FILE)

clean:
	rm -f $(CLEANFILES)

