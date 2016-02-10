SERV_SRC=$(PWD)/Server/
CLIENT_SND_SRC=$(PWD)/Client_Sender/
CLIENT_RCV_SRC=$(PWD)/Client_Reciever/
BUILD=$(PWD)/Build/

all: $(BUILD)Server $(BUILD)Client_Sender $(BUILD)Client_Reciever


#############################################################################
##################### Server ################################################
#############################################################################

$(BUILD)Server: $(SERV_SRC)server.o $(SERV_SRC)main.o
	-mkdir $(BUILD)
	gcc -o $(BUILD)Server $(SERV_SRC)server.o $(SERV_SRC)main.o -pthread

$(SERV_SRC)server.o: $(SERV_SRC)server.h $(SERV_SRC)server.c $(SERV_SRC)main.c defs.h
	gcc -c $(SERV_SRC)server.c -I $(PWD) -o $(SERV_SRC)server.o -pthread
$(SERV_SRC)main.o: $(SERV_SRC)server.h $(SERV_SRC)server.c $(SERV_SRC)main.c defs.h
	gcc -c $(SERV_SRC)main.c -I $(PWD)	-o $(SERV_SRC)main.o -pthread

#############################################################################
##################### Client Sender #########################################
#############################################################################

$(BUILD)Client_Sender: $(CLIENT_SND_SRC)main.o $(PWD)/client.o
	-mkdir $(BUILD)
	gcc -o $(BUILD)Client_Sender $(PWD)/client.o $(CLIENT_SND_SRC)main.o

$(PWD)/client.o: $(PWD)/client.c defs.h
	gcc -c $(PWD)/client.c -I $(PWD) -o $(PWD)/client.o

$(CLIENT_SND_SRC)main.o: $(CLIENT_SND_SRC)main.c defs.h
	gcc -c $(CLIENT_SND_SRC)main.c -I $(PWD) -o $(CLIENT_SND_SRC)main.o

#############################################################################
##################### Client Reciever #######################################
#############################################################################

$(BUILD)Client_Reciever: $(CLIENT_RCV_SRC)main.o $(PWD)/client.o
	-mkdir $(BUILD)
	gcc -o $(BUILD)Client_Reciever $(PWD)/client.o $(CLIENT_RCV_SRC)main.o

$(CLIENT_RCV_SRC)main.o: $(CLIENT_RCV_SRC)main.c defs.h
	gcc -c $(CLIENT_RCV_SRC)main.c -I $(PWD) -o $(CLIENT_RCV_SRC)main.o

clean:
	-rm $(CLIENT_RCV_SRC)main.o
	-rm $(CLIENT_SND_SRC)main.o	
	-rm $(PWD)/client.o
	-rm $(SERV_SRC)main.o
	-rm $(SERV_SRC)server.o
	-rm $(BUILD)Client_Reciever
	-rm $(BUILD)Client_Sender
	-rm $(BUILD)Server

  
	

