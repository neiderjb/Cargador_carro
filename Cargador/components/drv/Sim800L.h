
void sendATcommand(char *data);
bool sim800l_begin();
bool subscribeTopic();
void sim800l_PowerOn();

void StartGPRSMQTTConnection();
bool StartGPRSMQTTConnectionNew();

bool sendCommandATToken(char *command, char *responsecommand, bool debug);
bool responseAT(char *ATcommand, char *ATcommandResponse);
void sendAT(char *dataWrite, int countWrite, int uart);

bool sendCommandATSize(char *command, int MinimunSize, bool debug);

void sendATValue(char *value, int sizeCommand);
bool sendATValueResponse(char *value, int sizeCommand,char *responsecommand, bool debug);
void sendATValueNoRESPONSE(char *value, int sizeCommand, bool debug);
void sendATValueOLD(char *value, int sizeCommand);

void readDataMQTT2G();

bool PublishMqtt2G(char *topic, int TopicSize, char *data, int DataSize);
void PublishMqtt2GOLD(char *topic, int TopicSize, char *data, int DataSize);
