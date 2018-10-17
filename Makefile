all: unitTest

PROJECT=../../..

autogen: ${PROJECT}/framework/def/stcFramework.xml ${PROJECT}/framework/def/stcCore.xml
	cd ../../../framework/def ; mono ../tools/xsdValidator/xsdValidator.exe ./stc.xsd ./stcFramework.xml
	cd ${PROJECT}/framework/tools/scg ; python scg.py ../../../framework/bll/base ../../../framework/def/stcFramework.xml.nrm
	cd ../../../framework/def ; mono ../tools/xsdValidator/xsdValidator.exe ./stc.xsd ./stcCore.xml
	cd ${PROJECT}/framework/tools/scg ; python scg.py ../../../framework/bll/core ../../../framework/def/stcCore.xml.nrm
	touch autogen

Makefile.sources: base.vcproj proj2makefile.xslt cleanSource.awk
	xsltproc proj2makefile.xslt base.vcproj | awk -f cleanSource.awk > $@

Makefile.coreSources: ../core/core.vcproj proj2makefile.xslt cleanSource.awk
	xsltproc proj2makefile.xslt ../core/core.vcproj | awk -f cleanSource.awk --assign module=CORE --assign prefix=../core/ > $@

Makefile.unitTestSources: test/testbase/testbase.vcproj proj2makefile.xslt genUnitTestSrcs.awk
	xsltproc proj2makefile.xslt test/testbase/testbase.vcproj | awk -f cleanSource.awk --assign module=BASE_UNIT_TEST_ > $@

Makefile.includes: base.vcproj proj2makefile.xslt genIncludes.awk
	xsltproc proj2makefile.xslt base.vcproj | awk -f genIncludes.awk > $@

#	xsltproc proj2makefile.xslt $? | awk '/~$^/ { print $0 }' > $@
#	xsltproc -o $@ proj2makefile.xslt $?

unitTest: autogen Makefile.sources Makefile.coreSources Makefile.unitTestSources Makefile.includes test.cpp
	make -f Makefile.unitTest

#unitTest: autogen Makefile.sources Makefile.coreSources
#	make -f Makefile.unitTest

clean:
	make -f Makefile.unitTest clean
	rm Makefile.sources
