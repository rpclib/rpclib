UTILS=../utils
DDIR=doxygen/xml

all: reference

doxy: Doxyfile
	doxygen Doxyfile

all.xml: doxy
	xsltproc $(DDIR)/combine.xslt $(DDIR)/index.xml >all.xml

reference: all.xml
	python3 $(UTILS)/markygen.py all.xml reference.md.mako pages/reference.md
