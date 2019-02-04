#include "board.h"
#include "ch.h"
#include "hal.h"
#include "uart.h"
#include "led.h"
#include "vibro.h"
#include "Sequences.h"
#include "radio_lvl1.h"
#include "kl_i2c.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "acg_lsm6ds3.h"
//#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

//static void ReadAndSetupMode();

// EEAddresses
//#define EE_ADDR_DEVICE_ID       0

//static uint8_t ISetID(int32_t NewID);
//void ReadIDfromEE();

// ==== Periphery ====
Vibro_t Vibro {VIBRO_CTRL};
LedRGB_t Led { LED_R_PIN, LED_G_PIN, LED_B_PIN };
LedSmooth_t Lumos { LUMOS_CTRL, 1800 };
PinOutput_t EePwr {EE_PWR_PIN};

// ==== Timers ====
//static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
//static TmrKL_t TmrRxTableCheck {MS2ST(2007), evtIdCheckRxTable, tktPeriodic};
//static int32_t TimeS;
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
    Clk.SetCoreClk(cclk16MHz);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    Acg.Init();

//    i2c1.Init();
//    EePwr.Init();
//    EePwr.SetHi();
//    i2c1.ScanBus();

    Led.Init();
    Lumos.Init();
//    Lumos.StartOrRestart(lsqLStart);
    Led.StartOrRestart(lsqStart);

    Vibro.Init();
//    Vibro.StartOrRestart(vsqBrrBrr);
    SimpleSensors::Init();
//    Adc.Init();


    // ==== Time and timers ====
//    TmrEverySecond.StartOrRestart();
//    TmrRxTableCheck.StartOrRestart();

    // ==== Radio ====
//    if(
    Radio.Init();
    //== retvOk) Led.StartOrRestart(lsqStart);
//    else Led.StartOrRestart(lsqFailure);
//    chThdSleepMilliseconds(1008);

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
//            case evtIdEverySecond:
//                TimeS++;
//                ReadAndSetupMode();
//                break;

            case evtIdButtons:
                Printf("Btn %u\r", Msg.BtnEvtInfo.Type);
                break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
} // ITask()

void ProcessCharging(PinSnsState_t *PState, uint32_t Len) {

}

#if 1 // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) PShell->Print("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));

    else if(PCmd->NameIs("lum")) {
        uint8_t Brt;
        if(PCmd->GetNext<uint8_t>(&Brt) != retvOk) { PShell->Ack(retvCmdError); return; }
        Lumos.SetBrightness(Brt);
    }

//    else if(PCmd->NameIs("GetID")) PShell->Reply("ID", ID);

//    else if(PCmd->NameIs("SetID")) {
//        if(PCmd->GetNext<int32_t>(&ID) != retvOk) { PShell->Ack(retvCmdError); return; }
//        uint8_t r = ISetID(ID);
//        RMsg_t msg;
//        msg.Cmd = R_MSG_SET_CHNL;
//        msg.Value = ID2RCHNL(ID);
//        Radio.RMsgQ.SendNowOrExit(msg);
//        PShell->Ack(r);
//    }

//    else if(PCmd->NameIs("Pill")) {
//        if(PCmd->GetNextInt32(&dw32) != OK) { PShell->Ack(CMD_ERROR); return; }
//        PillType = (PillType_t)dw32;
//        App.SignalEvt(EVT_PILL_CHECK);
//    }

    else PShell->Ack(retvCmdUnknown);
}
#endif

#if 0 // =========================== ID management =============================
void ReadIDfromEE() {
    ID = EE::Read32(EE_ADDR_DEVICE_ID);  // Read device ID
    if(ID < ID_MIN or ID > ID_MAX) {
        Printf("\rUsing default ID\r");
        ID = ID_DEFAULT;
    }
}

uint8_t ISetID(int32_t NewID) {
    if(NewID < ID_MIN or NewID > ID_MAX) return retvFail;
    uint8_t rslt = EE::Write32(EE_ADDR_DEVICE_ID, NewID);
    if(rslt == retvOk) {
        ID = NewID;
        Printf("New ID: %u\r", ID);
        return retvOk;
    }
    else {
        Printf("EE error: %u\r", rslt);
        return retvFail;
    }
}
#endif
