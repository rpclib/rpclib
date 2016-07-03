UTILS=../utils
DDIR=doxygen/xml

all: reference.md

doxy: Doxyfile
	doxygen Doxyfile

all.xml: $(DDIR)/index.xml
	xsltproc $(DDIR)/combine.xslt $(DDIR)/index.xml >all.xml

reference.md: all.xml reference.md.mako
	python3 $(UTILS)/markygen.py all.xml reference.md.mako pages/reference.md
