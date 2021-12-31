typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg);
} tNMEA2000Handler;

void setupNMEA();
void setupTimers();

void handleNMEA2000Msg(const tN2kMsg &N2kMsg);
void handleBinaryStatus(const tN2kMsg &N2kMsg);
void handleAddressClaim(const tN2kMsg &N2kMsg);

void newDevice();
void sendEngine();
void sendSwitches();
void sendN2kBinaryStatus();
void sendN2kSensorData();
void sendN2kSystemStatus();
void sendN2kACStatus();
void readRestartCount();
