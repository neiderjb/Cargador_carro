
void sendATcommand(char *data);
void sim800l_begin();
void subscribeTopic();
void sim800l_PowerOn();
void StartGPRSMQTTConnection();
bool sendCommandATToken(char *command, char *responsecommand);
bool responseAT(char *ATcommand, char *ATcommandResponse);
void sendAT(char *dataWrite, int countWrite, int uart);
bool sendCommandATSize(char *command, int MinimunSize);
void sendATValue(char *value, int sizeCommand);
void readDataMQTT2G();