
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
bool sendCommandAT(char *command, int sizeCommand, char *response, bool debug);


void sendATValue(char *value, int sizeCommand);
bool sendATValueResponse(char *value, int sizeCommand,char *responsecommand, bool debug);
void sendATValueNoRESPONSE(char *value, int sizeCommand, bool debug);

void readDataMQTT2G(char *command, int timeout);

bool PublishMqtt2G(char *topic, int TopicSize, char *data, int DataSize);
bool PublishMqtt2GTask(char *topic, int TopicSize, char *data, int DataSize);


void gprsRead_task(void *p);