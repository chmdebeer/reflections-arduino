typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg);
} tNMEA2000Handler;

void setupNMEA();
void setupTimers();

void handleNMEA2000Msg(const tN2kMsg &N2kMsg);
void handleBinaryStatus(const tN2kMsg &N2kMsg);

void readSensors();
void sendN2kBinaryStatus();
void sendN2kSensorData();
void SendN2kEngineRPM();
void readLock();
