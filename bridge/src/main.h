// These #defines make it easy to set the backlight color
#define BOUNCE_WITH_PROMPT_DETECTION

typedef struct {
  unsigned long PGN;
  void (*Handler)(const tN2kMsg &N2kMsg);
} tNMEA2000Handler;

void setupNMEA();
void setupTimers();
void blinkBridgeStartLed();

void handleNMEA2000Msg(const tN2kMsg &N2kMsg);
void handleEngineRPM(const tN2kMsg &N2kMsg);
void handleEngineDynamicParameters(const tN2kMsg &N2kMsg);
void handleBinaryStatus(const tN2kMsg &N2kMsg);
void handleFluidLevel(const tN2kMsg &N2kMsg);

void sendN2kBinaryStatus();
void sendN2kGNSS();
void sendN2kACStatus();
