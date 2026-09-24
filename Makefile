.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment")
endif

include $(DEVKITARM)/gba_rules

TARGET  := PixelTown
BUILD   := build
SOURCES := source
INCLUDES :=
LIBS    := -lgba

ARCH := -mthumb -mthumb-interwork

CFLAGS   := -g -Wall -O2 -mcpu=arm7tdmi -mtune=arm7tdmi $(ARCH)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS  := -g $(ARCH)
LDFLAGS  := -g $(ARCH) -Wl,-Map,$(notdir $*.map)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

export OFILES := $(CFILES:.c=.o)

export INCLUDE := -I$(DEVKITPRO)/libgba/include
export LIBPATHS := -L$(DEVKITPRO)/libgba/lib

export CC := $(DEVKITARM)/bin/arm-none-eabi-gcc
export CXX := $(DEVKITARM)/bin/arm-none-eabi-g++
export AS := $(DEVKITARM)/bin/arm-none-eabi-as
export LD := $(DEVKITARM)/bin/arm-none-eabi-g++
export OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy

.PHONY: all clean

all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).gba $(TARGET).map

else

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).gba : $(OUTPUT).elf

$(OUTPUT).elf : $(OFILES)

-include $(DEPENDS)

endif
