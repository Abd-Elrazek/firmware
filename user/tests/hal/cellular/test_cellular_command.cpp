#include "application.h"

SYSTEM_MODE(MANUAL);

Serial1LogHandler logHandler(115200, LOG_LEVEL_ALL, { 
                                {"ncp", LOG_LEVEL_ALL},
                                {"app", LOG_LEVEL_ALL},
                                {"ot", LOG_LEVEL_NONE},
                                {"sys", LOG_LEVEL_NONE},
                                {"system", LOG_LEVEL_NONE}
                            });

static int at_command_callback(int type, const char* buf, int len, int* lines) {
    Serial.printlnf("===================================================+");
    switch (type) {
        case NOT_FOUND:         Serial.printlnf("Type: UNKNOWN");           break;
        case WAIT:              Serial.printlnf("Type: WAIT");              break;
        case RESP_OK:           Serial.printlnf("Type: RESP_OK");           break;
        case RESP_ERROR:        Serial.printlnf("Type: RESP_ERROR");        break;
        case RESP_PROMPT:       Serial.printlnf("Type: RESP_PROMPT");       break;
        case RESP_ABORTED:      Serial.printlnf("Type: RESP_ABORTED");      break;

        case TYPE_OK:           Serial.printlnf("Type: TYPE_OK");           break;
        case TYPE_ERROR:        Serial.printlnf("Type: TYPE_ERROR");        break;
        case TYPE_CONNECT:      Serial.printlnf("Type: TYPE_CONNECT");      break;
        case TYPE_NOCARRIER:    Serial.printlnf("Type: TYPE_NOCARRIER");    break;
        case TYPE_NODIALTONE:   Serial.printlnf("Type: TYPE_NODIALTONE");   break;
        case TYPE_BUSY:         Serial.printlnf("Type: TYPE_BUSY");         break;
        case TYPE_NOANSWER:     Serial.printlnf("Type: TYPE_NOANSWER");     break;
        case TYPE_PROMPT:       Serial.printlnf("Type: TYPE_PROMPT");       break;
        case TYPE_PLUS:         Serial.printlnf("Type: TYPE_PLUS");         break;
        case TYPE_TEXT:         Serial.printlnf("Type: TYPE_TEXT");         break;
        case TYPE_ABORTED:      Serial.printlnf("Type: TYPE_ABORTED");      break;
        case TYPE_DBLNEWLINE:   Serial.printlnf("Type: TYPE_DBLNEWLINE");   break;
        default:
            Serial.printlnf("Type: SHOULD ENTER THIS CASE!"); break;
            break;
    }
    Serial.printf("len: %d, line: %d, hex data: ", len, *lines);
    for (int i = 0; i < len; i++) {
        Serial.printf("%02X ", buf[i]);
    }
    Serial.printlnf("");
    Serial.printf("text: ");
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\r') {
            Serial.printf("\\r");
        } else if (buf[i] == '\n') {
            Serial.printf("\\n");

        } else {
            Serial.printf("%c", buf[i]);
        }
    }
    Serial.printlnf("");

    (*lines)++;
    Serial.printlnf("===================================================-");

    return WAIT;
}

void setup() {
    Serial.begin(115200);
    Cellular.on();
}

void loop() {
    static char cmd_buf[100];
    static int cmd_buf_index = 0;
    int lines = 0;
    while (Serial.available()) {
        char ch = Serial.read();
        Serial.write(ch);
        // Log.info("console: 0x%x", ch);
        switch (ch) {
            case '\r': {
                cmd_buf[cmd_buf_index++] = '\0';
                if (strncmp(cmd_buf, "at+cmgs", strlen("at+cmgs")) == 0 ||
                    strncmp(cmd_buf, "AT+CMGS", strlen("AT+CMGS")) == 0)
                {
                    // Just for test
                    cmd_buf_index--;
                    cmd_buf[cmd_buf_index++] = '\r';
                    cmd_buf[cmd_buf_index++] = '\0';
                }

                Serial.printlnf("\r\nSend command: %s", cmd_buf);

                Cellular.command(at_command_callback, &lines, 5000, "%s\r\n", cmd_buf);
                cmd_buf_index = 0;
                break;
            }
            case '\n': {
                break;
            }
            case 127: { //DEL
                Serial.write('\b');
                Serial.write(' ');
                Serial.write('\b');
                if (cmd_buf_index) {
                    cmd_buf_index--;
                }
                break;
            }
            default:
                cmd_buf[cmd_buf_index++] = ch;
                break;
        }
    }
}
