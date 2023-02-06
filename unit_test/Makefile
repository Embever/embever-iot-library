TARGET_EXEC ?= testrunner

BUILD_DIR ?= ./build
BUILD_DIR_FAKE ?= ./build_fake_inc
BUILD_DIR_ABS ?= ./build_abs

SRCS =  testrunner.c 											\
		./mock/mock_ebv_i2c.c									\
		./util/test_esp_util.c								\
		test_ebv_local.c										\
		test_ebv_eftp.c

SRC_FAKE =  ./fake_inc/ebv_esp_gpio.c							\
			./fake_inc/serial_print.c							\
			./fake_inc/ebv_delay.c

SRCS_ABS = 																										\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/core/ebv_esp.c											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/core/ebv_i2c.c											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/core/ebv_local.c											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/core/ebv_eftp.c											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/core/ebv_iot.c											\
		${ARDUINO_USR_LIB_SOURCE}/cwpack-master/core/extcwpack.c

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
OBJS += $(SRC_FAKE:%=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_ABS:%=$(BUILD_DIR_ABS)/%.o)

#$(info $$OBJS is [${OBJS}])
DEPS := $(OBJS:.o=.d)

INC_DIRS =  ./fake_inc											\
			./mock												\
			${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src	\
			${ARDUINO_USR_LIB_SOURCE}/cwpack-master/src			\

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

DEFINES = EBV_UNIT_TEST

CPP_DEFINES := $(addprefix -D,$(DEFINES))

CPPFLAGS ?= $(INC_FLAGS) $(CPP_DEFINES) -MMD -MP -g

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR_ABS)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR_ABS)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(BUILD_DIR_ABS)

-include $(DEPS)

MKDIR_P ?= mkdir -p