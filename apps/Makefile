VPATH = libs/
SHELL = /bin/tcsh
DAT = $(@)
VCC = $(CC)
VCFLG = $(CFLAGS)
DLR = $

APPS := $(filter-out %~ CVS Makefile %.mk libs,$(wildcard *))

TARGS = $(foreach x, $(APPS), $(addsuffix .mk, $(x)))

HDRS = $(foreach T, $(APPS), $(wildcard $(T)/*.h))

all : apps.mk

$($(strip TARGS)) : $(HDRS) Makefile
	@echo '#' >! $(@)
	@echo '# --- Auto-generated file for target $(basename $(@)) --- ' >> $(@)
	@echo '#' >> $(@)
	@echo >> $(@)
	@echo "include apps.mk" >> $(@)
	@echo "VPATH = $(basename $(@))/:$(basename $(@))/objs" >> $(@)
	@echo >> $(@)
	@echo $(basename $(@))_OBJS = $(notdir $(patsubst %.cpp, %.o, $(wildcard $(basename $(@))/*.cpp))) >> $(@)
	@echo >> $(@)
	@echo 'lib$(basename $(@)).a : $(value DLR)($(basename $(@))_OBJS)' >> $(@)
	@echo '	@if (! -d libs) mkdir libs' >> $(@)
	@echo '	@ar -crs libs/$(value DAT) $(basename $(@))/objs/*.o' >> $(@)
#	@echo '	@ranlib libs/$(value DAT)' >> $(@)
	@echo >> $(@)
	@echo '$(value DLR)($(basename $(@))_OBJS)' \
 ' : %.o : %.cpp $(value DLR)(DRIVER_SRC:.cpp=.h) $(value DLR)(AHASTAR_SRC:.cpp=.h) $(value DLR)(SHARED_SRC:.cpp=.h) $(value DLR)(UTIL_SRC:.cpp=.h) $(value DLR)(SIMULATION_SRC:.cpp=.h) $(value DLR)(RTS_OBJS:.o=.h)' >> $(@)
	@echo '	@if (! -d $(basename $(@))/objs) mkdir $(basename $(@))/objs' >> $(@)
	@echo '	@-$(value DLR)(RM) libs/lib$(basename $(@)).a' >> $(@)
	@echo '	$(value DLR)(CC) $(value VCFLG) -c -o $(basename $(@))/objs/$(value DLR)(notdir $(value DAT)) $(basename $(@))/$(value DLR)(patsubst %.o,%.cpp,$(value DAT))' >> $(@)
	@echo >> $(@)



apps.mk : $(TARGS) Makefile
	@echo "#auto-generated file, do not edit!" >! apps.mk
	@echo "SHELL = /bin/tcsh" >> apps.mk
	@echo "CC = g++" >> apps.mk
	@echo "CFLAGS = -Wall -O3 -g -I ../aha -I../abstraction -I../driver -I../shared -I../simulation -I../util" >> apps.mk
	@echo >> $(@)

	@echo 'ifeq ($(value DLR)(findstring "Darwin", "$(value DLR)(shell uname -s)"), "Darwin")' >> $(@)
	@echo 'ifeq ("$(value DLR)(OPENGL)", "STUB")' >> apps.mk
	@echo "CFLAGS += -I../driver/STUB/ -I../driver/STUB/GL/ -DNO_OPENGL" >> apps.mk
	@echo "else" >> apps.mk
	@echo "CFLAGS += -DOS_MAC" >> apps.mk
	@echo "CFLAGS += -I/System/Library/Frameworks/GLUT.framework/Versions/A/Headers/" >> apps.mk
	@echo "CFLAGS += -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/" >> apps.mk
	@echo 'endif' >> apps.mk
	@echo 'else # not darwin' >> apps.mk
	@echo 'ifeq ("$(value DLR)(OPENGL)", "STUB")' >> apps.mk
	@echo "CFLAGS += -I../driver/STUB/ -I../driver/STUB/GL/ -DNO_OPENGL" >> apps.mk
	@echo "else" >> apps.mk
	@echo "CFLAGS += -I/usr/include/GL" >> apps.mk
	@echo "endif" >> apps.mk
	@echo "endif" >> apps.mk

#	@echo 'else' >> $(@)
#	@echo '  CFLAGS += -I/usr/include/GL' >> $(@)
#	@echo 'endif' >> $(@)
	@echo >> $(@)
	@echo 'ABSTRACTION_SRC = $(value DLR)(wildcard ../abstraction/*.cpp)' >> $(@)
	@echo 'DRIVER_SRC = $(value DLR)(wildcard ../driver/*.cpp)' >> $(@)
	@echo 'SHARED_SRC = $(value DLR)(wildcard ../shared/*.cpp)' >> $(@)
	@echo 'SIMULATION_SRC = $(value DLR)(wildcard ../simulation/*.cpp)' >> $(@)
	@echo 'UTIL_SRC = $(value DLR)(wildcard ../util/*.cpp)' >> $(@)
	@echo 'AHASTAR_SRC = $(value DLR)(wildcard ../aha/*.cpp)' >> $(@)


clean:
	@-$(RM) apps.mk $(wildcard $(addsuffix /objs/*,$(APPS)/objs/*.o)) $(TARGS)
	@-$(RM) $(addsuffix .a,$(addprefix libs/lib,$(APPS)))
