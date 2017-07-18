#Compiler and Linker
CXX         := g++

# Target Prefix
TARGETPREFIX = NGSX

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src/modules
INCDIR      := include
BUILDDIR    := build
TARGETDIR   := bin
LIBDIR      := lib
RESDIR      := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o
LIBEXT      := so

#Flags, Libraries and Includes
CXXFLAGS    := -Wall -g
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
TARGETS     := $(patsubst $(SRCDIR)/%,$(TARGETDIR)/%,$(SOURCES:.$(SRCEXT)=))
LIBSOURCES  := $(shell find $(INCDIR) -type f -name *.$(SRCEXT))
LIBS        := $(patsubst $(INCDIR)/%,$(LIBDIR)/lib%,$(LIBSOURCES:.$(SRCEXT)=.$(LIBEXT)))
LLIBS       := $(patsubst $(INCDIR)/%,-l%,$(LIBSOURCES:.$(SRCEXT)=))

#TARGET      := $(patsubst $(SRCDIR)/%,$(TARGETDIR)/%,$(SOURCES:.$(SRCEXT)=))

#Default Make
all: resources $(TARGETS)

#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories
	@cp $(RESDIR)/* $(TARGETDIR)/

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(LIBDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

libclean:
	@$(RM) -rf $(LIBDIR)

cleanest: cleaner libclean


#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGETDIR)/$(TARGETPREFIX)%: $(BUILDDIR)/$(TARGETPREFIX)%.$(OBJEXT) $(LIBS)
	$(CXX) -o $@ $< -L$(LIBDIR) $(LIBS)


#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT) 
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<
	@$(CXX) $(CXXFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp


# Create shared libraries
$(LIBDIR)/%.$(LIBEXT) : $(BUILDDIR)/%.$(OBJEXT)
	$(CXX) -shared -o $@ $<


$(BUILDDIR)/lib%.$(OBJEXT): $(INCDIR)/%.$(SRCEXT)
	$(CXX) -fPIC -c $< -o $@

#Non-File Targets
.PHONY: all remake clean cleaner cleanest resources libclean
.SECONDARY: $(LIBS)
