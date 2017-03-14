
outdir          = ./bin
appfile         = ./bin/dtsw
SUPERGLUE_DIR   = /pica/h1/afshin/sg/superglue/include
SUPERGLUE_FLAGS = -pthread -I$(SUPERGLUE_DIR)  #-pedantic -Wno-long-long -Wno-format
DUCTTEIP_DIR    = /pica/h1/afshin/Damavand/D4/DuctTeip/ResStudio/Prototype
DUCTTEIP_INC    = $(DUCTTEIP_DIR)/include
DUCTTEIP_LIBPATH= $(DUCTTEIP_DIR)/lib/
DUCTTEIP_LIB    = ductteip
SOURCE_DIR      = ./src
HEADER_DIR      = ./include
DUCTTEIP_FLAGS  = -I$(HEADER_DIR) -DWITH_MPI=1 -I$(SUPERGLUE_DIR) -I$(DUCTTEIP_INC)

CPP  = mpiicpc
LINK = mpif90


ifeq ($(CXX),icpc)
#-----------------------Intel Compiler set---------------------
	LINK_FLAGS=-lm -lrt -L$(DUCTTEIP_LIBPATH) -l$(DUCTTEIP_LIB) -Wl,--allow-multiple-definition 
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 -qopt-prefetch -O3  -xCORE-AVX2
else
#-----------------------GCC Compiler set---------------------
	GCOV_FLAGS=-fprofile-arcs -ftest-coverage
	OPTIM_FLAGS=-mavx -march=bdver1 -mfma4 -Ofast  -Wwrite-strings
	SPECIAL_FLAGS=$(OPTIM_FLAGS)     
	LINK_FLAGS=-lm -lrt -lpthread
	COMP_FLAGS= $(SUPERGLUE_FLAGS) $(DUCTTEIP_FLAGS) -std=c++11 $(SPECIAL_FLAGS) 
endif
#########################################################
headers:=$(notdir $(shell ls -Sr $(SOURCE_DIR)/*.cpp))
objnames:=$(headers:%.cpp=%.o)
objects:=$(addprefix $(outdir)/,$(objnames))
RED=\033[0;31m
GREEN=\033[1;32m
NC=\033[0m

all: $(appfile)

$(appfile) : $(appfile)($(outdir)/*.o)
	$(info ---------------------------------------------------------)
	@echo -e "generate output exe $(RED)$(appfile)$(NC)."
	$(info ---------------------------------------------------------)


$(appfile)($(outdir)/*.o) : $(objects)
	$(info ---------------------------------------------------------)
	$(info link objects to app. )
	$(info ---------------------------------------------------------)
	@$(CPP) -o $(appfile) $? $(LINK_FLAGS)

$(objects):  $(outdir)/%.o:  $(SOURCE_DIR)/%.cpp 
#	$(info compile $(notdir $<) )
	@echo -e "$(GREEN) comopile $(notdir $<) $(NC)"
	@$(CPP) -c -o $@ $< $(COMP_FLAGS)


clean: 
	rm $(outdir)/*.o $(appfile)
