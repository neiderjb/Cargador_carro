
void sendATcommand(char *data);
bool sim800l_begin();
void subscribeTopic();
void sim800l_PowerOn();
bool StartGPRSMQTTConnection();
bool sendCommandATToken(char *command, char *responsecommand, bool debug);
bool responseAT(char *ATcommand, char *ATcommandResponse);
void sendAT(char *dataWrite, int countWrite, int uart);
bool sendCommandATSize(char *command, int MinimunSize, bool debug);
void sendATValue(char *value, int sizeCommand, bool debug);
void readDataMQTT2G();
void PublishMqtt2G(char *topic, int TopicSize, char *data, int DataSize);
