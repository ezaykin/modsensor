LIBS=-lmariadb -liconv -lmodbus
TARGET_CFLAGS=-I$(STAGING_DIR)/opt/include/mysql -I$(STAGING_DIR)/opt/lib/libiconv-full/include -I$(STAGING_DIR)/opt/include/modbus 
TARGET_LDFLAGS=-L$(STAGING_DIR)/opt/lib -L$(STAGING_DIR)/opt/lib/libiconv-full/lib

modsensor: modsensor.o mysqldata.o sensordata.o
	$(CC) $(LDFLAGS) $(TARGET_LDFLAGS) $(LIBS) -o modsensor modsensor.o mysqldata.o sensordata.o

modsensor.o: modsensor.c
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) -c modsensor.c

mysqldata.o: mysqldata.c
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) -c mysqldata.c

sensordata.o: sensordata.c
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) -c sensordata.c

clean:
	rm *.o modsensor

