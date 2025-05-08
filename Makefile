CC=gcc
TARGET=daemonforge
DIR=/usr/local/bin
FLAGS= -Wall -Wextra

all: $(TARGET)
	

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $(TARGET).c -o $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) $(DIR)/$(TARGET)
	sudo chmod +x $(DIR)/$(TARGET)
	@echo "✅ Installed $(TARGET) to $(DIR)"

clean:
	rm -f $(TARGET)
	
uninstall:
	sudo rm -f $(DIR)/$(TARGET)
	@echo "🗑️  Uninstalled $(TARGET) from $(DIR)"
