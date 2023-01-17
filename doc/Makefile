TEX_FILE = ETCBC4doc.tex

PDF_FILE = $(TEX_FILE:.tex=.pdf)

CLEANFILES = $(TEX_FILE:.tex=.aux) \
             $(TEX_FILE:.tex=.log) \
             $(TEX_FILE:.tex=.out) \
             $(TEX_FILE:.tex=.toc) \
             $(TEX_FILE:.tex=.rubbercache) \
             missfont.log \
             *~

all: $(PDF_FILE)

$(PDF_FILE): $(TEX_FILE) Makefile
	rubber -f -d -m xelatex $(TEX_FILE)

clean:
	rm -f $(CLEANFILES)

