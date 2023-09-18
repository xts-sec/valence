CC = gcc

SRC_DIR = src
BUILD_DIR = build

.PHONY: clean

#
# valence.c
#
$(BUILD_DIR)/valence: always $(SRC_DIR)/valence.c
	$(CC) -g -o $(BUILD_DIR)/valence $(SRC_DIR)/valence.c


always:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
