/*
 * stepper.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#define TMC_MAX_POS 13500
#define TMC_ADDR 0x60
#define TMC222_COMMAND_GOTO_SECURE 0x84
#define TMC222_COMMAND_HARDSTOP 0x85
#define TMC222_COMMAND_RESET_POS 0x86
#define TMC222_COMMAND_RESET_FULL 0x87
#define TMC222_COMMAND_SOFTSTOP 0x8f
#define TMC222_COMMAND_GET_STATUS1 0x81
#define TMC222_COMMAND_GET_STATUS2 0xfc

#define ATM_DIV_FACTOR 23.148148148

//Data structure definition for the TMC222 status information.
	//Use variables of this data type togehter with the GetFullStatus1() function.
	typedef struct
	{
		unsigned char IHold: 4;
		unsigned char IRun: 4;
		unsigned char VMin: 4;
		unsigned char VMax: 4;
		unsigned char Acc: 4;
		unsigned char Shaft: 1;
		unsigned char StepMode: 2;
		unsigned char AccShape: 1;
		unsigned char TInfo: 2;
		unsigned char TW: 1;
		unsigned char TSD: 1;
		unsigned char UV2: 1;
		unsigned char ElDef: 1;
		unsigned char SteppLoss: 1;
		unsigned char VddReset: 1;
		unsigned char CPFail: 1;
		unsigned char NA: 1;
		unsigned char OVC2: 1;
		unsigned char OVC1: 1;
		unsigned char ESW: 1;
		unsigned char Motion: 3;
	} TTMC222Status;

	//Data structure definition for the TMC222 motor parameters.
	//Use variables of this type together with the SetMotorParameters()
	//and GetMotorParameters() functions.
	typedef struct
	{
		unsigned char IHold: 4;
		unsigned char IRun: 4;
		unsigned char VMin: 4;
		unsigned char VMax: 4;
		unsigned char Acc: 4;
		unsigned char Shaft: 1;
		unsigned char SecPosHi: 3;
		unsigned char SecPosLo: 8;
		unsigned char NA1: 2;
		unsigned char StepMode: 2;
		unsigned char AccShape: 1;
		unsigned char NA2: 3;
	} TTMC222Parameters;

class speedo_stepper{

public:
#define MOTOR_OVERWRITE_END_POS 864
#define TMC222_DIV_FACTOR 1.45 // rechnerisch 1.48
	speedo_stepper();
	~speedo_stepper();
	void init();
	void loop();
	bool go_to(int winkel,int accel,int speed);
	bool go_to(int winkel);
	int get_pos();
	void overwrite_pos(int new_pos);
	void get_motor_status(int* ist_pos, int* delay, int* status, TTMC222Status *TMC222Status);
	void get_motor_status(int* ist_pos, int* delay, int* status);
	void get_atm_motor_status(int* ist_pos, int* delay, int* status);
	void run_calibration(void);
	int init_steps_to_go;
	void startup();


	void InitTWI(void);
	void GetFullStatus1(TTMC222Status *TMC222Status);
	int GetFullStatus2(int *TargetPosition, int *SecurePosition);
	void SetMotorParameters(TTMC222Parameters *TMC222Parameters);
	void SetPosition(int Position);
	void ResetPosition(void);
	void GotoSecurePosition(void);
	void ResetToDefault(void);
	void RunInit(unsigned char VMin, unsigned char VMax, int Position1, int Position2);
	void HardStop(void);
	void SoftStop(void);
	void GetMotorParameters(TTMC222Parameters *TMC222Parameters);

private:
};

#endif /* STEPPER_H_ */


