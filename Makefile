CXX = g++
CXXFLAGS = -Wall
SRCDIR = ./src
MODULEDIR = ./src/modules
PIPELINEDIR = ./src/pipeline
INCLUDEDIR = ./include
BUILDDIR = ./build
LIBDIR = ./lib
BINDIR = ./bin

# Compile and link C++ executables and libraries
all: $(BINDIR)/NGSXFastQStats $(BINDIR)/NGSXQualityControl $(BINDIR)/NGSXQualityControlPairedEnd $(BINDIR)/NGSXRemoveDuplicates $(BINDIR)/NGSXfastqc $(BINDIR)/NGSXtrimmerge $(BINDIR)/NGSXremovedup $(BINDIR)/NGSXqualtrim $(BINDIR)/NGSXclassify $(BINDIR)/NGSXfastq2bam $(BINDIR)/NGSXtrimmergebam

#------------------------------------------------------------------------------#
#																Executables														    		 #
#------------------------------------------------------------------------------#

# NGSXFastQStats Executable
$(BINDIR)/NGSXFastQStats: $(BUILDDIR)/NGSXFastQStats.o $(BUILDDIR)/FastQ.o $(BUILDDIR)/ProgressLog.o $(BUILDDIR)/TextColor.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# NGSXQualityControl Executable
$(BINDIR)/NGSXQualityControl: $(BUILDDIR)/NGSXQualityControl.o $(BUILDDIR)/ProgressLog.o $(BUILDDIR)/TextColor.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# NGSXQualityControlPairedEnd Executable
$(BINDIR)/NGSXQualityControlPairedEnd: $(BUILDDIR)/NGSXQualityControlPairedEnd.o $(BUILDDIR)/ProgressLog.o $(BUILDDIR)/TextColor.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# NGSXRemoveDuplicates Executable
$(BINDIR)/NGSXRemoveDuplicates: $(BUILDDIR)/NGSXRemoveDuplicates.o $(BUILDDIR)/FastQ.o $(BUILDDIR)/ProgressLog.o $(BUILDDIR)/TextColor.o
	$(CXX) $(CXXFLAGS) -o $@ $^

#------------------------------------------------------------------------------#
#															Python Scripts											     				 #
#------------------------------------------------------------------------------#
$(BINDIR)/NGSXfastqc: $(PIPELINEDIR)/NGSXfastqc.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXtrimmerge: $(PIPELINEDIR)/NGSXtrimmerge.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXtrimmergebam: $(PIPELINEDIR)/NGSXtrimmergebam.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXremovedup: $(PIPELINEDIR)/NGSXremovedup.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXqualtrim: $(PIPELINEDIR)/NGSXqualtrim.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXclassify: $(PIPELINEDIR)/NGSXclassify.py
	@chmod +x $<
	@ln -s ../$< $@

$(BINDIR)/NGSXfastq2bam: $(PIPELINEDIR)/NGSXfastq2bam.py
	@chmod +x $<
	@ln -s ../$< $@

#------------------------------------------------------------------------------#
#															NGSX Object Code																 #
#------------------------------------------------------------------------------#

# NGSXFastQStats Object Code
$(BUILDDIR)/NGSXFastQStats.o: $(MODULEDIR)/NGSXFastQStats.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

# NGSXQualityControl Object Code
$(BUILDDIR)/NGSXQualityControl.o: $(MODULEDIR)/NGSXQualityControl.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

# NGSXQualityControl Object Code
$(BUILDDIR)/NGSXQualityControlPairedEnd.o: $(MODULEDIR)/NGSXQualityControlPairedEnd.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

# NGSXQualityControl Object Code
$(BUILDDIR)/NGSXRemoveDuplicates.o: $(MODULEDIR)/NGSXRemoveDuplicates.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@


#------------------------------------------------------------------------------#
#														Include Object Code														     #
#------------------------------------------------------------------------------#

# FastQ Class Object Code
$(BUILDDIR)/FastQ.o: $(INCLUDEDIR)/FastQ.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

# ProgressLog Object Code
$(BUILDDIR)/ProgressLog.o: $(INCLUDEDIR)/ProgressLog.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

# TextColor Object Code
$(BUILDDIR)/TextColor.o: $(INCLUDEDIR)/TextColor.cpp
	$(CXX) $(CXXFLAGS) -I $(INCLUDEDIR) -c $<
	@mv *.o $@

#-----------------------------------------------------------------------------#
#                      							Clean Up                   								#
#-----------------------------------------------------------------------------#
clean:
	@echo "\033[0m\033[91mRemoving executable binaries. \033[0m"
	@rm -fv $(BINDIR)/NGSXFastQStats
	@rm -fv $(BINDIR)/NGSXQualityControl
	@rm -fv $(BINDIR)/NGSXQualityControlPairedEnd
	@rm -fv $(BINDIR)/NGSXRemoveDuplicates
	@echo "\033[91mRemoving symbolic links to executable scripts. \033[0m"
	@rm -fv $(BINDIR)/NGSXfastqc
	@rm -fv $(BINDIR)/NGSXtrimmerge
	@rm -fv $(BINDIR)/NGSXtrimmergebam
	@rm -fv $(BINDIR)/NGSXremovedup
	@rm -fv $(BINDIR)/NGSXqualtrim
	@rm -fv $(BINDIR)/NGSXclassify
	@rm -fv $(BINDIR)/NGSXfastq2bam
	@echo "\033[91mRemoving executables object code. \033[0m"
	@rm -fv $(BUILDDIR)/NGSXFastQStats.o
	@rm -fv -fv $(BUILDDIR)/NGSXQualityControl.o
	@rm -fv $(BUILDDIR)/NGSXQualityControlPairedEnd.o
	@rm -fv $(BUILDDIR)/NGSXRemoveDuplicates.o
	@echo "\033[91mRemoving library object code. \033[0m"
	@rm -fv $(BUILDDIR)/ProgressLog.o
	@rm -fv $(BUILDDIR)/FastQ.o
	@rm -fv $(BUILDDIR)/TextColor.o
