typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg);
} tNMEA2000Handler;

void setupNMEA();
void setupTimers();

void handleNMEA2000Msg(const tN2kMsg &N2kMsg);
void handleBinaryStatus(const tN2kMsg &N2kMsg);
void handleThruster(const tN2kMsg &N2kMsg);
void handleAddressClaim(const tN2kMsg &N2kMsg);

void newDevice();
void sendEngine();
void sendSwitches();
void sendN2kBinaryStatus();
void sendN2kSensorData();
void processNutraSalt();
void sendN2kNutraSaltCountdown(unsigned char instance, Engine &engine, bool reset);
void sendN2kSystemStatus();
void sendN2kACStatus();
void sendN2kFuelConsumption();
void readRestartCount();
