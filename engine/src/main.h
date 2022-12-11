typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg);
} tNMEA2000Handler;

void setupNMEA();
void setupTimers();

void handleNMEA2000Msg(const tN2kMsg &N2kMsg);
void handleBinaryStatus(const tN2kMsg &N2kMsg);
void handleAddressClaim(const tN2kMsg &N2kMsg);

void readRestartCount();
void newDevice();
void n2kBinaryStatus(SwitchBankInstance instance);
void sendN2kSystemStatus();

void sendSteeringData();
void sendBatteryData();
void sendTemperatureData();
void sendFuelData();
void sendEngineData();

void handleEngineRPM(const tN2kMsg &N2kMsg);
void handleEngineDynamicParameters(const tN2kMsg &N2kMsg);

void processNutraSalt();
void processTrim();
void sendN2kTrimTab();
void sendN2kNutraSaltCountdown(unsigned char instance, Engine &engine, bool reset);
void sendN2kEngineRPM();
