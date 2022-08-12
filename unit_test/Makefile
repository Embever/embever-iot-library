TARGET_EXEC ?= testrunner

BUILD_DIR ?= ./build
BUILD_DIR_ABS ?= ./build_abs

SRCS =  testrunner.cpp 											\
		test_ebv_local.cpp										\
		test_ebv_iot.cpp

SRCS_ABS = 																										\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_esp.cpp											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_iot.cpp											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_local.cpp											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_delay.cpp											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_i2c.cpp											\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/print_serial.cpp										\
		${ARDUINO_USR_LIB_SOURCE}/embever-iot-library/src/ebv_esp_gpio.cpp										\
		${ARDUINO_USR_LIB_SOURCE}/cwpack-master/src/extcwpack.cpp

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
OBJS += $(SRCS_ABS:%=$(BUILD_DIR_ABS)/%.o)

#$(info $$OBJS is [${OBJS}])
DEPS := $(OBJS:.o=.d)

INC_DIRS =  ./fake_inc											\
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